#include "dict.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Trie* create() {
    Trie* node;

    node = (struct Trie*)malloc(sizeof(Trie));

    if (node != NULL) {
        node->end_of_word = false;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

int char_to_index(char c) { return ((int)c - (int)'a'); }

void insert(Trie* dict, char* str) {
    int key_len;
    Trie* current_node = dict;
    int index;

    key_len = strlen(str);
    for (int i = 0; i < key_len; i++) {
        index = char_to_index(str[i]);
        // printf("the index is %d and level is %d\n",index,i);
        if (current_node->children[index] == NULL) {
            current_node->children[index] = create();
        }
        current_node = current_node->children[index];
    }
    current_node->end_of_word = true;
}

bool lookup(Trie* dict, char* str) {
    struct Trie* current_node = dict;
    int key_len;
    int index;
    if (dict == NULL) return false;
    key_len = strlen(str);
    for (int i = 0; i < key_len; i++) {
        // printf("in lookup depth %d and index %d\n",i,index);
        index = char_to_index(str[i]);
        if (current_node->children[index] == NULL)
            return false;
        else
            current_node = current_node->children[index];
    }
    return (current_node->end_of_word);
}

void destroy(Trie* root) {
    if (root == NULL) return;
    for (int i = 0; i < ALPHABET_SIZE; i++) destroy(root->children[i]);
    free(root);
}
