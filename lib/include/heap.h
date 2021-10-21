#ifndef HEAP_H_
#define HEAP_H_

#define enqueue(h, d) heap_insert(h, d)
#define dequeue(h, d) heap_remove(h, d)

typedef struct heap heap_t;

struct heap {
    int size;
    int (*compare)(const void * key1, const void * key2);
    void (*destroy)(void * data);
    void ** tree;
};

int heap_init(heap_t ** heap, 
              int (*compare)(const void * key1, const void * key2),
              void (*destroy)(void * data));

void heap_destroy(heap_t * heap);

int heap_insert(heap_t * heap, const void * data);

int heap_remove(heap_t * heap, void ** data);

#define heap_size(s) ((s)->size)


#endif