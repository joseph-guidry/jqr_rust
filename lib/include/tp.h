#ifndef THREADPOOL_H_
#define THREADPOOL_H

#include <stdio.h>
#include <stdlib.h>
#include <heap.h>
#include <pthread.h>

#define MIN_WORKERS 2
#define MAX_WORKERS 100
#define DEFAULT_WORKERS 10

enum state { RUN, STOP};

typedef struct task task_t;
typedef struct thread thread_t;
typedef struct threadpool tp_t;

struct task {
    void (*routine)(void *);
    void * args;
    int priority;
    // task_t * next;   // string a list of tasks
};

// create a list of finished work 
    // type of (INSERT|LIST)
    // results (Inserted word | list of words)
struct finished_work {
    void * type;
    void * results;
};

struct thread {
    pthread_t tid;
    thread_t * next;
};

struct threadpool {

    enum state status;

    pthread_mutex_t tp_lock;
    pthread_cond_t work_notify;
    pthread_cond_t stop_work;

    // Does this variable need a mutex? 
    int threadcnt; 
    int minthread;
    int maxthread;

    pthread_attr_t worker_attr;
    thread_t * active_workers;
   
    // priority queue
    heap_t * queue;

    // root node of trie
    // void * data = NULL;
    // pthread_mutex_t data_lock;
};


int tp_create(tp_t ** tp);

int tp_queue_task(tp_t * tp, void (*routine)(void *), void* args, int priority);

int tp_destory(tp_t * tp);

#if 0
// This function will attempt to be a generic function to access what ever the
// data storage mechanism is.  First will be using a trie. Based on the DS lib used,
// this will use #ifdef USE_TRIE_ or other DS types
// WORDLIST will have its operations [INSERT|RETRIEVE] from the DS

int access_data();

#endif

#endif
