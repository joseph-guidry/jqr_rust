#ifndef WORD_LIST_H_
#define WORD_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trie.h>

#define TEMP_BUFF_LEN 32

typedef struct word_list {
    char * prefix;
    char * temp_buffer;
    int temp_buff_len;
    int current;            // number of words in word_list_array
    int max;                // limint of strings to collect
    char ** word_list_array;
}word_list_t;


// trie functions for main
int word_list_trie_create();

void word_list_trie_destroy();

int word_list_trie_insert();

int word_list_trie_remove();

void word_list_trie_display();
// from prefix node start displaying words
trie_node_t * get_prefix_node(trie_node_t * node, char * prefix);

int word_list_add_word(word_list_t * word_list, char * word);

int word_list_build_internal(word_list_t * word_list, trie_node_t * node, int level);

int word_list_build(word_list_t * word_list);

int word_list_create(word_list_t ** word_list, char * prefix, int max);

void word_list_destroy(word_list_t * word_list);

#endif


