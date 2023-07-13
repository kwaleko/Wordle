#pragma once

#include <stdbool.h>

#define ALPHABET_SIZE (26)

struct Trie {
    bool end_of_word;
    struct Trie *children[ALPHABET_SIZE];
};

typedef struct Trie Trie;

Trie *create();
void insert(Trie *dict, char *str);
bool lookup(Trie *dict, char *str);
void destroy(Trie *dict);
int char_to_index(char c);
