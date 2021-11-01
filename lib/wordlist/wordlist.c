#include <wordlist.h>

void testcall(char *s) {
	printf("Hello, this is a C string [%s]\n", s);
}

void testcall_float(float f) {
	printf("Hello, this is a C float [%f]\n", f);
}

void test_use_tp_from_cmake_define()
{
    printf("this is called as constructor, but included only if using threadpool is defined\n");
}

#ifdef WITH_THREADPOOL
    void test_use_tp_from_cmake_define () __attribute__((constructor));
#endif



static trie_node_t * trie;

int word_list_trie_create () __attribute__((constructor));
void word_list_trie_destroy () __attribute__((destructor));


int word_list_trie_create()
{
    printf("creating trie\n");
    return trie_create(&trie);
}

void word_list_trie_destroy()
{
    printf("destory trie\n");
    trie_destroy(trie);
}

int word_list_trie_insert(char * string)
{
    return trie_insert_r(trie, string, 0);
}

int word_list_trie_remove(char * string)
{
    return trie_remove(trie, string);
}

void word_list_trie_display()
{
    char * buff = calloc(1, 20);
    if(NULL == buff)
    {
        return; //EXIT_FAILURE
    }
    trie_display_r(trie, buff, 0);
    free(buff);
}

// from prefix node start displaying words
trie_node_t * get_prefix_node(trie_node_t * node, char * prefix)
{
    if(*prefix == '\0')
    {
        return node;
    }

    int idx = *prefix - 'a';    // get_idx(char);
    if(node->nletter[idx])
    {
        return get_prefix_node(node->nletter[idx], ++prefix);
    }
    return NULL;
}

int word_list_add_word(word_list_t * word_list, char * word)
{
    char * buff = strdup(word);
    // printf("word list is null? %c\n", word_list->word_list_array == NULL ? 'T':'F');
    void * temp = realloc(word_list->word_list_array, sizeof(char *) * (word_list->current + 1));
    // check if realloc was successful
    if(NULL == temp)
    {
        // memory error
        return EXIT_FAILURE;
    }
    else
    {
        word_list->word_list_array  = temp;
        temp = NULL;
    }

    // printf("lets get bug fixing: %u\n",__LINE__);
    size_t prefix_len = strlen(word_list->prefix);
    size_t word_len = strlen(word);
    // printf("lets get bug fixing: %u\n", __LINE__);
    word_list->word_list_array[word_list->current] = calloc(1, prefix_len + word_len + 1);    // allocate space for prefix + word + '\0'
    // printf("lets get bug fixing: %u\n", __LINE__);
    if(NULL == word_list->word_list_array[word_list->current])
    {
        return EXIT_FAILURE;
    }
    // printf("lets get bug fixing: %u\n", __LINE__);
    strcpy(word_list->word_list_array[word_list->current], word_list->prefix);
    strcpy(word_list->word_list_array[word_list->current] + prefix_len, buff);
    // printf("lets get bug fixing word_list[%d] [%s]\n",word_list->current, word_list->word_list_array[word_list->current]);
    word_list->current++;
    free(buff);
    return 0;
}

int word_list_build_internal(word_list_t * word_list, trie_node_t * node, int level)
{

    // TODO: check if buffer string needs to be resized
    if(node->is_end)
    {
        word_list->temp_buffer[level] = '\0';
        // printf("%s\n", word_list->temp_buffer);

        // check if there is a max limit or current number is less than provided max
        if((word_list->max < 0) || (word_list->current < word_list->max))
        {
            // printf("adding [%s] to list at position [%d]\n", word_list->temp_buffer, word_list->current);

            word_list_add_word(word_list,  word_list->temp_buffer);
        }
    }

    int i;
    for(i = 0; i < NUM_LETTERS; i++)
    {
        if(node->nletter[i])
        {
            word_list->temp_buffer[level] = i + 'a';
            word_list_build_internal(word_list, node->nletter[i], level + 1);
        }
    }
    return EXIT_SUCCESS;
}

int word_list_build(word_list_t * wl)
{
    trie_node_t * prefix_node = get_prefix_node(trie, wl->prefix);
    if(NULL == prefix_node)
    {
        return EXIT_FAILURE;
    }

   if(0 != word_list_build_internal(wl, prefix_node, 0))
   {
       fprintf(stderr, "Failed to build wordlist\n");
       word_list_destroy(wl);
       return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}

int word_list_create(word_list_t ** word_list, char * prefix, int max)
{
    *word_list = calloc(1, sizeof(word_list_t));
    if(NULL == (*word_list))
    {
        return EXIT_FAILURE;
    }

    (*word_list)->current = 0;
    (*word_list)->max =  max == 0 ? -1: max;                                // if max is 0, then there is no max
    (*word_list)->prefix = prefix == NULL ? strdup("") : strdup(prefix);    // strdup(NULL) is undefined behavior
    (*word_list)->temp_buffer = calloc(1, TEMP_BUFF_LEN);
    (*word_list)->temp_buff_len = TEMP_BUFF_LEN;
    (*word_list)->word_list_array = NULL;

    return EXIT_SUCCESS;
}

void word_list_destroy(word_list_t * word_list)
{   
    int i;
    for(i = 0; i < word_list->current; i++)
    {
        free(word_list->word_list_array[i]);
    }
    free(word_list->word_list_array);

    free(word_list->temp_buffer);
    free(word_list->prefix);
    free(word_list);
}