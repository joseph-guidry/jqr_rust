#include <stdio.h>
#include <stdlib.h>
// #include <stddef.h>

// #define container_of(ptr, type, member) ({                      \
//         const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
//         (type *)( (char *)__mptr - offsetof(type,member) );})



#define LIST_ANCHOR(l) (l->anchor)

typedef struct list_node * list_node_t;
typedef struct list * list_t;

struct list_node {
    void * data;
    list_node_t prev;
    list_node_t next;
};

struct list {
    long count;
    list_node_t anchor;

    void (*remove_item_f)(void *);          // how to free item
    void (*print_item_f)(void *);          // how to display item
};

// create new node
int create_node(list_node_t * node, void * data);

//destroy new node
void destroy_node(list_node_t  node);


// create a new list
int create_list(list_t * list, 
                void (*destroy)(void * data),
                void (*display)(void * data)
                );

// destroy all items in a list
int destroy_list();

#define DESTORY_LIST(l) destroy_list(l, 1)
#define CLEAR_LIST(l) destroy_list(l, 0)

// add item to a list 
int insert_list_item();

#define INSERT_TAIL(l, d) (insert_list_item(l, d, -1))
#define INSERT_HEAD(l, d) (insert_list_item(l, d, 1))

// remove item from a list
int remove_list_item(list_t list, void ** node, int position);

#define REMOVE_TAIL(l,n) remove_list_item(l, n, -1)
#define REMOVE_HEAD(l,n) remove_list_item(l, n, 0)

// display contents of a list
void display_list();

// search for item in a list
int find_list_item();