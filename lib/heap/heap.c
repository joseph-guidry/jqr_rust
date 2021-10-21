#include <heap.h>
#include <stdlib.h>



#define HEAP_SIZE(h) (h->size)
#define heap_parent(npos) ((int)(((npos) -1 ) / 2))
#define heap_left(npos) (((npos) * 2) + 1)
#define heap_right(npos) (((npos) * 2) + 2)

int heap_init(heap_t ** heap, 
              int (*compare)(const void * key1, const void * key2),
              void (*destroy)(void * data))
{
    (*heap) = calloc(1, sizeof(heap_t));
    if(NULL == (*heap))
    {
        return EXIT_FAILURE;
    }

    (*heap)->compare = compare;
    (*heap)->destroy = destroy;
    (*heap)->size = 0;
    (*heap)->tree = NULL;

    return EXIT_SUCCESS;
}

void heap_destroy(heap_t * heap)
{
    int i;
    if(NULL == heap->destroy)
    {
        for(i = 0; i < heap_size(heap); i++)
        {
            heap->destroy(heap->tree[i]);
        }
    }

    free(heap->tree);
    free(heap);
}

int heap_insert(heap_t * heap, const void * data)
{
    void * temp;
    int ipos, ppos;
    if(NULL == (temp = (void **)realloc(heap->tree, (heap_size(heap) + 1) * sizeof(void *))))
    {
        return EXIT_FAILURE;
    }
    else
    {
        heap->tree = temp;
    }

    // insert data at last position in tree
    heap->tree[heap_size(heap)] = (void *)data;

    ipos = heap_size(heap);
    ppos = heap_parent(ipos);

    while( ipos > 0 && heap->compare(heap->tree[ppos], heap->tree[ipos]) < 0)
    {
        // do some swapping
        temp = heap->tree[ppos];
        heap->tree[ppos] = heap->tree[ipos];
        heap->tree[ipos] = temp;

        // move up one level
        ipos = ppos;
        ppos = heap_parent(ipos);
    }

    heap->size++;
    return EXIT_SUCCESS;
}

int heap_remove(heap_t * heap, void ** data)
{
    void *save, *temp;

    int ipos, lpos, rpos, mpos;

    if(0 == heap_size(heap))
    {
        return EXIT_FAILURE;
    }

    // data to return 
    *data = heap->tree[0];

    save = heap->tree[heap_size(heap) - 1];
    
    if(heap_size(heap) - 1 > 0)
    {
        // reduce size of the tree
        if(NULL == (temp = (void **)realloc(heap->tree, (heap_size(heap) - 1) * sizeof(void *))))
        {
            return EXIT_FAILURE;
        }
        else
        {
            heap->tree = temp;
        }
        heap->size--;
    }
    else
    {
        free(heap->tree);
        heap->tree = NULL;
        heap->size = 0;
        return EXIT_SUCCESS;
    }
    
    heap->tree[0] = save;
    ipos = 0;
    lpos = heap_left(ipos);
    rpos =heap_right(ipos);

    while(1)
    {
        lpos = heap_left(ipos);
        rpos = heap_right(ipos);

        if(lpos < heap_size(heap) && heap->compare(heap->tree[lpos], heap->tree[ipos]) > 0)
        {
            mpos = lpos;
        }
        else
        {
            mpos = ipos;
        }

        if(rpos < heap_size(heap) && heap->compare(heap->tree[rpos], heap->tree[mpos]) > 0)
        {
            mpos = rpos;
        }

        if (mpos == ipos)
        {
            break;
        }
        else
        {
            temp = heap->tree[mpos];
            heap->tree[mpos] = heap->tree[ipos];
            heap->tree[ipos] = temp;

            // move up one level
            ipos = mpos;
        }
    }
    return EXIT_SUCCESS;
}

int heap_is_empty(heap_t * heap)
{
    return HEAP_SIZE(heap) == 0 ? 0 : 1;
}
