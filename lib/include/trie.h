#ifndef TRIE_H_
#define TRIE_H_

#define NUM_LETTERS 26

typedef struct trie_node trie_node_t;

struct trie_node {
    int popularity;
    int is_end;
    int is_leaf;
    int level;
    trie_node_t * nletter[NUM_LETTERS];  // indicator of end of word + avaialble letters
};

int create_node(trie_node_t ** new, int level);

void destroy_node();

int trie_create();

void trie_destroy();

int trie_insert_r();

int trie_insert();

int trie_remove();

void trie_display_r();

int remove_node();

int search_trie();

#endif
