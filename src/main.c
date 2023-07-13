#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "dict.h"
#include "util.h"
#include "wordle.h"

/*int feedback_eq1(int k, feedback_result *a, feedback_result *b) {
    for (int i = 0; i < k; i++)
        if (a[i] != b[i]) return 0;
    return 1;
}*/
int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: %s [k] [file] [quantum]\n", argv[0]);
        printf("  k       - word length\n");
        printf("  file    - textfile containing the words one per line\n");
        printf(
            "  quantum - whether quantum wordle should be played (y/n), "
            "default false\n");
        exit(EXIT_FAILURE);
    }
    // initialize randomness using current time
    srand48(time(NULL));
    int k = atoi(argv[1]);
    char *filename = argv[2];
    bool quantum = false;
    if (argc > 3) {
        if (argv[3][0] == 'y' || argv[3][0] == 'Y' || argv[3][0] == '1' ||
            argv[3][0] == 't' || argv[3][0] == 'T') {
            quantum = true;
        }
    }
    // allocate space to store the randomly selected word
    // +1 for null terminator
    char *selected = malloc(k + 1);
    // char *guess1 = (char *)malloc(k + 1);
    char *selected2 = quantum ? malloc(k + 1) : NULL;
    // read file containing all words
    Trie *set = generateDict(filename, k, selected, selected2);
    printf("The selected word is \"%s\". (Do not tell anyone)\n", selected);
    if (quantum) {
        printf("The selected dual word is \"%s\". (Do not tell anyone)\n",
               selected2);
    }
    int rounds = 0;
    // feedback_result* win=
    // (feedback_result*)malloc(k*sizeof(feedback_result));
    bool game_won = false;
    while (!game_won) {
        feedback_result *feedback;
        char *guess1;
        rounds++;
        guess1 = guess(set, k);
        feedback = getFeedback(guess1, selected, selected2, k);
        printFeedback(feedback, k);
        if (checkWin(feedback, k)) game_won = true;
        // free(*guess1);
        free(guess1);
        free(feedback);
    }
    printf("You needed %d attempts.\n", rounds);
    // frees all memory of the dictionary
    destroy(set);
    free(selected);
    // free(win);
    if (quantum) free(selected2);
    return EXIT_SUCCESS;
}
