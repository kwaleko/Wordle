#include "wordle.h"

#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "dict.h"


/**
* helper function to initalize the table to zero
* @p ar is pointer to the array and @p k represent 
* the length of the array
* the array might be allocated with malloc
* to avoid garbage value
**/
void init(int* ar, int k) {
    for (int i = 0; i < k; i++) {
        ar[i] = 0;
    }
}

/**
* Given two words and the length
* the function is a predicate determine whether the
* given words are dijoint or have letters in common
*/
bool cand_disjoint(char* word1, char* word2, int k) {
    int occ1[ALPHABET_SIZE];
    int occ2[ALPHABET_SIZE];
    init(occ1, ALPHABET_SIZE);
    init(occ2, ALPHABET_SIZE);
    for (int i = 0; i < k; i++) {
        occ1[char_to_index(word1[i])] = 1;
        occ2[char_to_index(word2[i])] = 1;
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (occ1[i] > 0 && occ2[i] > 0) return false;
    }
    return true;
}

/**
* Select two random word that are disjoing where the choice is uniform.
* the @p data is expected to hold all words in the given dictionary
* @p size should the length the given array.
* @p selected1 is an output parameter that should hold the first 
* uniformally chosen word, while @p selected2 hold the second disjoint word
* notice that here an algorithm called Resevoir Sampeling is being used 
* in the selection of the second word in order to avoid trying lot of choices
* or deleting words for the dictionary.
*/
void select_world_q(char** data, int size, char* selected1, char* selected2,
                    int k) {
    bool found = false;
    do {
        int idx = drand48() * size;
        snprintf(selected1, k + 1, "%s", data[idx]);
        int counter = 0;
        for (int i = 0; i < size; i++) {
            char* word = data[i];
            if (!found) {
                if (cand_disjoint(selected1, word, k)) {
                    snprintf(selected2, k + 1, "%s", word);
                    found = true;
                }
            } else {
                counter++;
                int j = (drand48() * (counter + 1));
                if (j < 1) {
                    if (cand_disjoint(selected1, word, k))
                        snprintf(selected2, k + 1, "%s", word);
                }
            }
        }
    } while (!found);
}

Trie* generateDict(char* filename, int k, /*@out@*/ char* selected1,
                   /*@out@*/ char* selected2) {
    Trie* dict = create();
    FILE* f;
    int read = 0;
    char* word = malloc(k + 1);
    char** data = malloc(sizeof(char*) * 2);
    int capacity = 2;
    int size = 0;

    f = fopen(filename, "r");
    if (f == NULL) {
        printf("File cannot be be opened");
    } else {
        int i = 0;  // keep track of the elements seen in file
        do {
            if (size == capacity) {
                char** ndata =
                    (char**)realloc(data, capacity * 2 * sizeof(char*));
                data = ndata;
                if (data == NULL) printf("error");
                capacity = capacity * 2;
            }
            read = fscanf(f, "%s", word);
            insert(dict, word);
            // Resevoir Sampeling, No QUANTUM
            if (selected2 == NULL) {
                if (i == 0) snprintf(selected1, k + 1, "%s", word);
                int j = (drand48() * (i + 1));
                if (j < 1) snprintf(selected1, k + 1, "%s", word);
            } else {
                data[size] = malloc(k + 1);
                snprintf(data[size], k + 2, "%s", word);
                size++;
            }
            i++;
        } while (read != 0 && read != EOF);
        if (selected2 != NULL)
            select_world_q(data, size, selected1, selected2, k);
    }
    fclose(f);
    free(word);
    for (int i = 0; i < size; i++) free(data[i]);
    free(data);
    return dict;
}
/**
* helper function that convert a string to lowercase
* @p guess is a pointer to char and @p k is the 
* lenght of the given string
**/
char* convert_lower_case(char* guess, int k) {
    for (int i = 0; i < k; i++) guess[i] = tolower(guess[i]);
    return guess;
}
char* guess(Trie* dict, int k) {
    bool in_dic = false;
    char* guess = malloc(k + 1);
    printf("Please input your guess: ");
    do {
        char word[100];
        scanf("%s", word);

        if (dict == NULL) {
            snprintf(guess, k + 1, "%s", word);  // instead of strcpy
            return convert_lower_case(guess, k);
        }
        if (strlen(word) <= k) {
            word[k] = '\0';
            snprintf(guess, k + 1, "%s", word);
            in_dic = lookup(dict, convert_lower_case(guess, k));
        }
        if (!in_dic) {
            printf("Invalid word. Try again: ");
        }
    } while (!in_dic);
    return convert_lower_case(guess, k);
}

/**
 * This function check a guess against two selected word(or one)
 * and give as a result feedback 
 * @p guess is pointer to the word guessed, while @p words1 and @p
 * word2 are both poitners to the randomly selected words
 */
feedback_result* getFeedback(char* guess, char* word1, char* word2, int k) {
    int occurence1[ALPHABET_SIZE];
    int occurence2[ALPHABET_SIZE];
    feedback_result* feedback1 = malloc(k * sizeof(feedback_result));
    feedback_result* feedback2 = malloc(k * sizeof(feedback_result));

    feedback_result* feedback;

    feedback = (feedback_result*)malloc(k * sizeof(feedback_result));

    // determine exact matches
    // set everthing except excact matches to wrong
    for (int i = 0; i < k; i++) {
        (guess[i] == word1[i]) ? (feedback1[i] = CORRECT)
                               : (feedback1[i] = WRONG);
        if (word2 != NULL)
            (guess[i] == word2[i]) ? (feedback2[i] = CORRECT)
                                   : (feedback2[i] = WRONG);
    }

    // init values to zero
    init(occurence1, ALPHABET_SIZE);
    init(occurence2, ALPHABET_SIZE);

    // Number of occurence of each character in the target word
    // excluding the exact match character
    for (int i = 0; i < k; i++) {
        if (feedback1[i] != CORRECT) {
            occurence1[char_to_index(word1[i])] += 1;
        }
        if (word2 != NULL && feedback2[i] != CORRECT) {
            occurence2[char_to_index(word2[i])] += 1;
        }
    }
    // determine Permutation character
    for (int i = 0; i < k; i++) {
        int c = char_to_index(guess[i]);
        if (feedback1[i] != CORRECT) {
            if (occurence1[c] > 0) {
                feedback1[i] = WRONGPOS;
                occurence1[c] -= 1;
            }
        }
        if (word2 != NULL && feedback2[i] != CORRECT) {
            if (occurence2[c] > 0 && word2 != NULL) {
                feedback2[i] = WRONGPOS;
                occurence2[c] -= 1;
            }
        }
    }
    if (word2 == NULL) {
        for (int i = 0; i < k; i++) feedback[i] = feedback1[i];
    } else {
        bool m1 = false;
        bool m2 = false;
        for (int i = 0; i < k; i++) {
            if (feedback1[i] == WRONGPOS || feedback1[i] == CORRECT) m1 = true;
            if (feedback2[i] == WRONGPOS || feedback2[i] == CORRECT) m2 = true;
        }
        if (!m1 || !m2) {
            if (!m2) {
                for (int i = 0; i < k; i++) feedback[i] = feedback1[i];
            } else {
                for (int i = 0; i < k; i++) feedback[i] = feedback2[i];
            }
        } else {
            for (int i = 0; i < k; i++) {
                feedback[i] = WRONG;
                if (feedback1[i] == WRONGPOS || feedback2[i] == WRONGPOS)
                    feedback[i] = QUANTUMWRONGPOS;
                if (feedback1[i] == CORRECT || feedback2[i] == CORRECT)
                    feedback[i] = QUANTUMCORRECT;
            }
        }
    }
    free(feedback1);
    free(feedback2);
    return feedback;
}

void printFeedback(feedback_result* feedback, int k) {
    printf("Result: ");
    for (int i = 0; i < k; i++) {
        if (feedback[i] == CORRECT) printf("%s", CORRECT_UNICODE);
        if (feedback[i] == WRONGPOS) printf("%s", PRESENT_UNICODE);
        if (feedback[i] == QUANTUMCORRECT) printf("%s", QCORRECT_UNICODE);
        if (feedback[i] == QUANTUMWRONGPOS) printf("%s", QPRESENT_UNICODE);
        if (feedback[i] == WRONG) printf("%s", WRONG_UNICODE);
    }
    printf("\n");
}
bool checkWin(feedback_result* feedback, int k) {
    for (int i = 0; i < k; i++) {
        if (feedback[i] != CORRECT)
            return false;
    }
    return true;
}
