// a single header file is required
#include <memory.h>
#include <sys/types.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>

#include <ev.h>

#include <stdio.h> // for puts
#include <stdlib.h>

struct my_io 
{
    ev_io io;   
    int sendfd;             // sendto C2 listener
    void * dest;            // hold the addr info for accepted connection (eventual C2 server)
};

typedef struct my_io my_io;

#define fatal(s) \
    fprintf(stderr, "error %s in file %s on line %u\n", s, __FILE__, __LINE__); exit(EXIT_FAILURE);

#define PORT_NO     4333
#define BUFFER_SIZE 8192

static void stdin_cb (EV_P_ ev_io *w, int revents);
static void tcp_accept_cb(EV_P_ ev_io* w, int revents);
static void tcp_read_cb(EV_P_ ev_io* w, int revents);
static void nlink_cb (EV_P_ ev_io *w, int revents);

void parseRtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len)
{
    memset(tb, 0, sizeof(struct rtattr *) * (max + 1));

    while (RTA_OK(rta, len)) {  // while not end of the message
        if (rta->rta_type <= max) {
            tb[rta->rta_type] = rta; // read attr
        }
        rta = RTA_NEXT(rta,len);    // get next attr
    }
}

// all watcher callbacks have a similar signature
// this callback is called when data is readable on stdin restart after printing the data
static void stdin_cb (EV_P_ ev_io *w, int revents)
{
    char * line  = NULL;
    size_t len = 0, len2;
    ev_io_stop(EV_A_ w);
    puts ("stdin ready");
    // for one-shot events, one must manually stop the watcher
    // with its corresponding stop function.

    len2 = getline(&line, &len, stdin);
    line[strlen(line) - 1] = '\0';

    printf("RECV: [%s]", line);
    ev_io_start(EV_A_ w);
}

static void tcp_accept_cb(EV_P_ ev_io* w, int revents)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sd;
    struct ev_io *w_client = (struct ev_io*) malloc (sizeof(struct ev_io));

    if(EV_ERROR & revents)
    {
        perror("got invalid event");
        return;
    }

    // Accept client request
    client_sd = accept(w->fd, (struct sockaddr *)&client_addr, &client_len);
    puts("doing accept");
    if (client_sd < 0)
    {
        perror("accept error");
        return;
    }

    puts("save the client addr info");

    // Initialize and start watcher to read client requests
    ev_io_init(w_client, tcp_read_cb, client_sd, EV_READ);
    ev_io_start(loop, w_client);
}

static void tcp_read_cb(EV_P_ ev_io* w, int revents)
    {
    char buffer[BUFFER_SIZE];
    ssize_t read;

    if(EV_ERROR & revents)
    {
    perror("got invalid event");
    return;
    }

    // Receive message from client socket
    read = recv(w->fd, buffer, BUFFER_SIZE, 0);

    if(read < 0)
    {
    perror("read error");
    return;
    }

    if(read == 0)
    {
    // Stop and free watchet if client socket is closing
    ev_io_stop(loop,w);
    free(w);
    perror("peer might closing");
    return;
    }
    else
    {
    printf("message:%s",buffer);
    send(w->fd, buffer, read, 0);
    }

    bzero(buffer, read);
    }

static void udp_cb(EV_P_ ev_io *w, int revents) {
    puts("udp socket has become readable");
    char buf[BUFFER_SIZE];             // message buffer
    struct sockaddr_storage addr;
    socklen_t addr_len;
    socklen_t bytes = recvfrom(w->fd, buf, BUFFER_SIZE - 1, 0, (struct sockaddr*) &addr, (socklen_t *) &addr_len);

    // add a null to terminate the input, as we're going to use it as a string
    buf[bytes] = '\0';
    printf("udp client said: %s", buf);

    // Echo it back.
    // WARNING: this is probably not the right way to do it with libev.
    // Question: should we be setting a callback on sd becomming writable here instead?
    sendto(w->fd, buf, bytes, 0, (struct sockaddr*) &addr, sizeof(addr));
}

char * make_message(const char *fmt, ...)
{
    int size = 0;
    char *p = NULL;
    va_list ap;

    /* Determine required size */

    va_start(ap, fmt);
    size = vsnprintf(p, size, fmt, ap);
    va_end(ap);

    if (size < 0)
        return NULL;

    size++;             /* For '\0' */
    p = malloc(size);
    if (p == NULL)
        return NULL;

    va_start(ap, fmt);
    size = vsnprintf(p, size, fmt, ap);
    va_end(ap);

    if (size < 0) {
        free(p);
        return NULL;
    }

    return p;
}

// Callback when netlink socket is ready to read
static void nlink_cb (EV_P_ ev_io *w_, int revents)
{
    my_io *w = (my_io *)w_;

    if(EV_ERROR & revents)
    {
        perror("got invalid event");
        return;
    }

    // this causes all nested ev_run's to stop iterating
    // ev_break (EV_A_ EVBREAK_ALL);  

    char buf[BUFFER_SIZE];             // message buffer
    struct iovec iov = {
        .iov_base = buf,
        .iov_len = sizeof(buf)
    };

    struct sockaddr_nl local = {
        .nl_family = AF_NETLINK,
        .nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE,
        .nl_pid = getpid()
    };

    struct msghdr msg = {
        .msg_name = &local,
        .msg_namelen = sizeof(local),
        .msg_iov = &iov,
        .msg_iovlen = sizeof(1)
    };

    // read and parse all messages from the
    ssize_t status = recvmsg(w->io.fd, &msg, MSG_DONTWAIT);

    //  check status
    if (status < 0) {
        if (errno == EINTR || errno == EAGAIN)
        {
            return;
        }

        printf("Failed to read netlink: %s", (char*)strerror(errno));
        return;
    }

    if (msg.msg_namelen != sizeof(local)) { // check message length, just in case
        printf("Invalid length of the sender address struct\n");
        return;
    }

    // message parser
    struct nlmsghdr *h;
    for (h = (struct nlmsghdr*)buf; status >= (ssize_t)sizeof(*h); ) {   // read all messagess headers
        int len = h->nlmsg_len;
        int l = len - sizeof(*h);
        char *ifName;

        if ((l < 0) || (len > status)) {
            printf("Invalid message length: %i\n", len);
            return;
        }

        // now we can check message type
        if ((h->nlmsg_type == RTM_NEWROUTE) || (h->nlmsg_type == RTM_DELROUTE)) { // some changes in routing table
            printf("NOT IMPORTANT: Routing table was changed\n");  
        } else {    // in other case we need to go deeper
            char *ifUpp;
            char *ifRunn;
            struct ifinfomsg *ifi;  // structure for network interface info
            struct rtattr *tb[IFLA_MAX + 1];

            ifi = (struct ifinfomsg*) NLMSG_DATA(h);    // get information about changed network interface

            parseRtattr(tb, IFLA_MAX, IFLA_RTA(ifi), h->nlmsg_len);  // get attributes
            
            if (tb[IFLA_IFNAME]) {  // validation
                ifName = (char*)RTA_DATA(tb[IFLA_IFNAME]); // get network interface name
            }

            if (ifi->ifi_flags & IFF_UP) { // get UP flag of the network interface
                ifUpp = (char*)"UP";
            } else {
                ifUpp = (char*)"DOWN";
            }

            if (ifi->ifi_flags & IFF_RUNNING) { // get RUNNING flag of the network interface
                ifRunn = (char*)"RUNNING";
            } else {
                ifRunn = (char*)"NOT RUNNING";
            }

            char ifAddress[256];    // network addr
            struct ifaddrmsg *ifa; // structure for network interface data
            struct rtattr *tba[IFA_MAX+1];

            ifa = (struct ifaddrmsg*)NLMSG_DATA(h); // get data from the network interface

            parseRtattr(tba, IFA_MAX, IFA_RTA(ifa), h->nlmsg_len);

            if (tba[IFA_LOCAL]) {
                inet_ntop(AF_INET, RTA_DATA(tba[IFA_LOCAL]), ifAddress, sizeof(ifAddress)); // get IP addr
            }

            char * output;
            switch (h->nlmsg_type) { // what is actually happenned?
                case RTM_DELADDR:
                    // printf("Interface %s: address was removed\n", ifName);
                    output = make_message("Interface %s: address was removed\n", ifName);
                    break;

                case RTM_DELLINK:
                    printf("Network interface %s was removed\n", ifName);
                    output = make_message("Network interface %s was removed\n", ifName);
                    break;

                case RTM_NEWLINK:
                    printf("New network interface %s, state: %s %s\n", ifName, ifUpp, ifRunn);
                    output = make_message("New network interface %s, state: %s %s\n", ifName, ifUpp, ifRunn);
                    break;

                case RTM_NEWADDR:
                    printf("Interface %s: new address was assigned: %s\n", ifName, ifAddress);
                    output = make_message("Interface %s: new address was assigned: %s\n", ifName, ifAddress);
                    break;
            }

            if(output)
                sendto(w->sendfd, output, strlen(output), 0, (struct sockaddr*)(w->dest), sizeof(struct sockaddr ));
        }

        status -= NLMSG_ALIGN(len); // align offsets by the message length, this is important

        h = (struct nlmsghdr*)((char*)h + NLMSG_ALIGN(len));    // get next message
    }
}

// Open netlink socket 
static int open_nlink_socket()
    {
    #if 1 // Create a listening socket for netlink msgs that indicate the change of interface status
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);   // create netlink socket

    if (fd < 0) {
        printf("Failed to create netlink socket: %s\n", (char*)strerror(errno));
        return -1;
    }

    struct sockaddr_nl local = {
        .nl_family = AF_NETLINK,
        .nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE,
        .nl_pid = getpid()
    };

    if (bind(fd, (struct sockaddr*)&local, sizeof(local)) < 0) {     // bind socket
        printf("Failed to bind netlink socket: %s\n", (char*)strerror(errno));
        close(fd);
        return -1;
    }  
    #endif
    return fd;
    }

// Open TCP socket
static int open_tcp_socket()
    {
    int sd;

    // Create server socket
    if( (sd = socket(PF_INET, SOCK_STREAM, 0)) < 0 )
    {
        perror("socket error");
        return -1;
    }

    struct sockaddr_in addr = {
        .sin_addr.s_addr = INADDR_ANY,
        .sin_family = AF_INET,
        .sin_port = htons(PORT_NO)
    };

    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));

    // Bind socket to address
    if (bind(sd, (struct sockaddr*) &addr, sizeof(addr)) != 0)
    {
        perror("bind error");
    }

    // Start listing on the socket
    if (listen(sd, 2) < 0)
    {
        perror("listen error");
        return -1;
    }
        return sd;
    }

static int open_udp_socket()
{
	int sd;

    // Create server socket
    if( (sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0 )
    {
        perror("socket error");
        return -1;
    }

    return sd;
}

// Socket cleanup function
void close_socket(int * fd)
    {
    puts("closing fd");
    close(*fd);
    }

void send_heartbeat(int fd, void * dst)
{
    puts("sending heartbeat");
    sendto(fd, "Hello\n", strlen("Hello\n"), 0, (struct sockaddr*)dst, sizeof(struct sockaddr ));
}

int main (void)
{
    puts("doing it");
    // every watcher type has its own typedef'd struct
    // with the name ev_TYPE
    ev_io stdin_watcher;
    //   ev_io nlink_watcher;
    ev_io sock_watcher;



    // use the default event loop unless you have special needs
    struct ev_loop *loop = EV_DEFAULT;

    // initialise an io watcher, then start it
    // this one will watch for stdin to become readable
    ev_io_init (&stdin_watcher, stdin_cb, /*STDIN_FILENO*/ 0, EV_READ);
    ev_io_start (loop, &stdin_watcher);

    // use gcc attributes to close socket when they leave scope of main
    int nfd __attribute__((__cleanup__(close_socket)));
    int tfd __attribute__((__cleanup__(close_socket)));
    int ufd __attribute__((__cleanup__(close_socket)));

    if(( (nfd =  open_nlink_socket()) < 0) || /* ((tfd = open_tcp_socket()) < 0) || */ ((ufd = open_udp_socket()) < 0) )
    {
        fatal("could not initialise all sockets");
    }

    struct sockaddr_in dst = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT_NO),
        .sin_addr.s_addr = inet_addr("127.0.0.1")       // pull this out of the binary at a specific location
    };

    send_heartbeat(ufd, &dst);

    puts("starting the watchters");
    
    my_io netlink_watcher = {
        .sendfd = ufd,
        .dest = (void *)&dst
    };

    // using a tcp socket 
    // ev_io_init (&sock_watcher, tcp_accept_cb, tfd, EV_READ);
    // ev_io_start (loop, &sock_watcher);

    // using a udp socket for sending messages
    ev_io_init (&sock_watcher, udp_cb, ufd, EV_READ);
    ev_io_start (loop, &sock_watcher);
    

    //   ev_io_init (&nlink_watcher, nlink_cb, nfd, EV_READ);
    //   ev_io_start (loop, &nlink_watcher);

    ev_io_init(&(netlink_watcher.io), nlink_cb, nfd, EV_READ);
    ev_io_start(loop, &netlink_watcher.io);


    // now wait for events to arrive
    ev_run (loop, 0);

    // break was called, so exit
    return 0;
}