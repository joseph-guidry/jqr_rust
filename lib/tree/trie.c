#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trie.h>

int is_node_leaf(trie_node_t * node)
{
    // This check might be simplier if each node held number of children
    // scan if there are any leaf nodes
    
    int idx;
    for(idx = 0; idx < NUM_LETTERS; idx++)
    {
        if(node->nletter[idx] != NULL)
        {
            break;
        }
    }

    // printf("update node if leaf [%c]\n", idx + 'a');

    if(NUM_LETTERS == idx)
    {
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

int is_end_of_word(trie_node_t * node)
{   
    if(node->is_end)
    {
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

int create_node(trie_node_t ** new, int level)
{
    *new = malloc(sizeof(trie_node_t));
    if(NULL == new)
    {
        return EXIT_FAILURE;
    }

    (*new)->popularity = -1;
    (*new)->level = level;
    (*new)->is_end = 0;

    int i;
    for(i = 0; i < NUM_LETTERS; i++)
    {
        (*new)->nletter[i] = NULL;
    }

    return EXIT_SUCCESS;
}

void destroy_node(trie_node_t * node)
{
    if(node)
    {
        free(node);
        node = NULL;
    }
}

int trie_create(trie_node_t ** root)
{
    if(create_node(root, 0))
    {
        // setting errno?
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void trie_destroy(trie_node_t * root)
{
    int idx;
    for(idx = 0; idx < NUM_LETTERS; idx++)
    {
        if(root->nletter[idx] != NULL)
        {
            // printf("looking to destroy each letter branch:[%c]\n", 'a'+ idx);
            trie_destroy(root->nletter[idx]);
            
        }
    }
    destroy_node(root);

}

int trie_insert_r(trie_node_t * root, char * str, int level)
{
    // printf("inserting [%c]\n", *str);
    if(*str == '\0')
    {
        root->is_end = 1;
        root->level = level;
        return EXIT_SUCCESS;
    }

    // Insert current letter in the string to node in the tree
    int idx = *str - 'a';
    // printf("insert at index [%d]\n", idx);
    if(NULL == root->nletter[idx])
    {
        // printf("CREATING node for [%c]\n", *str);
        if(create_node(&root->nletter[idx], level))
        {
            return EXIT_FAILURE;
        }
    }
    
    // Insert next letter into the
    trie_insert_r(root->nletter[idx], ++str, level++);
    return EXIT_SUCCESS;
}

#if 1
int trie_remove(trie_node_t * root, char * str)
{
    if(*str == '\0')
    {
        // Find the end of the string
        root->is_end = 0;
        return EXIT_SUCCESS;
    }
    
    int idx = *str - 'a';
    if(NULL != root->nletter[idx])
    {
        trie_remove(root->nletter[idx], str +1 );

        printf("check if a leaf node [%c]\n", *str);
        if(0 == is_node_leaf(root->nletter[idx]))
        {
            // delete node
            printf("deleting node\n");
            destroy_node(root->nletter[idx]);
            root->nletter[idx] = NULL;
        }
    }
    
    return EXIT_SUCCESS;
    
}
#endif

void trie_display_r(trie_node_t * root, char * str, int level)
{
    if(!is_end_of_word(root))
    {
        str[level] = '\0';
        printf("%s\n", str);
    }

    int i;
    for(i = 0; i < NUM_LETTERS; i++)
    {
        if(root->nletter[i])
        {
            str[level] = i + 'a';
            trie_display_r(root->nletter[i], str, level + 1);
        }
    }
}
