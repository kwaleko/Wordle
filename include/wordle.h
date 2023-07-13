#pragma once

#include "dict.h"

#define CORRECT_UNICODE "🟩"
#define PRESENT_UNICODE "🟨"
#define WRONG_UNICODE "⬛"
#define QCORRECT_UNICODE "🟢"
#define QPRESENT_UNICODE "🟡"

typedef enum {
    CORRECT,
    WRONGPOS,
    WRONG,
    QUANTUMCORRECT,
    QUANTUMWRONGPOS,
} feedback_result;

Trie *generateDict(char *filename, int k, /*@out@*/ char *selected1,
                   /*@out@*/ char *selected2);
char *guess(Trie *dict, int k);
feedback_result *getFeedback(char *guess, char *word1, char *word2, int k);
void printFeedback(feedback_result *feedback, int k);
bool checkWin(feedback_result *feedback, int k);
void choose_random_word(Trie *dict, char *exclude, char *res, int k);
void init(int *ar, int k);
bool cand_disjoint(char* word1, char* word2, int k);
void rand_tree_idx(Trie *dict, char *excluded, int *res, int k);


/*
if (selected2 != NULL) {
                if (w2_changed == false) {
                    if (i == 0) snprintf(selected1, k + 1, "%s", word);
                    if (cand_disjoint(selected1, word, k)) {
                        snprintf(selected2, k + 1, "%s", word);
                        w2_changed = true;
                    }
                    // in case both words are found, then switch them with
                    // probality 50%
                    if (w2_changed == true) {
                        int j = (drand48() * 101);
                        if (j < 50) swap_words(selected1, selected2, k);
                    }
                } else {
                    int j = (drand48() * (i + 1));
                    if (j <= 1) {
                        if (j == 0) {
                            if (cand_disjoint(selected2, word, k))
                                snprintf(selected1, k + 1, "%s", word);
                        }
                        if (j == 1) {
                            if (cand_disjoint(selected1, word, k))
                                snprintf(selected2, k + 1, "%s", word);
                        }
                    }
                }
            }
            */
