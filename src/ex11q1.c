#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ex11q1.h"
#include "game.h"

void build_subsets(uint64_t **S, uint64_t ***T, int *array_size);
int rank(const char *word);
int wordcmp(const void *a, const void *b);

char **words = NULL;
int letter_count[26];

int main(int argc, char *argv[]) {
    FILE *inputfile = fopen(argv[1], "r");
    FILE *outputfile = fopen(argv[2], "w");

    game_init(inputfile);

    int seed;
    fscanf(inputfile, " %u", &seed);

    words = malloc(word_count * sizeof(char *));
    for (int i = 0; i < word_count; i++) {
        words[i] = malloc((word_length + 1) * sizeof(char));
        fscanf(inputfile, " %s", words[i]);
    }

    for (int i = 0; i < word_count; i++) {
        int used[26] = {0};
        for (int j = 0; j < word_length; j++) {
            int letter = words[i][j] - 'a';
            if (!used[letter]) {
                letter_count[letter]++;
                used[letter] = 1;
            }
        }
    }
    qsort(words, word_count, sizeof(char *), wordcmp);

    int array_size;
    uint64_t *G = bs_create_universe(word_count, &array_size);
    uint64_t ** S = malloc(26 * sizeof(uint64_t *));
    uint64_t *** T = malloc(26 * sizeof(uint64_t **));
    for (int i = 0; i < 26; i++) {
        T[i] = malloc(word_length * sizeof(uint64_t *));
    }
    build_subsets(S, T, &array_size);

    char guess[word_length + 1];
    char result[word_length + 1];

    while (1) {
        strcpy(guess, words[bs_get_first(G, array_size)]);
        game_round(guess, result);
        fprintf(outputfile, "%s\n", guess);

        int green_count = 0;
        for (int i = 0; i < word_length; i++) {
            int letter = guess[i] - 'a';

            if (result[i] == 'G') {
                bs_intersect(G, T[letter][i], array_size);
                green_count++;
            }
            else if (result[i] == 'Y') {
                bs_intersect(G, S[letter], array_size);
                bs_remove(G, T[letter][i], array_size);
            }
            else if (result[i] == 'X') {
                bs_remove(G, S[letter], array_size);
            }
        }
        if (green_count == word_length) {
            break;
        }
    }

    fclose(inputfile);
    fclose(outputfile);

    for (int i = 0; i < word_count; i++) {
        free(words[i]);
    }
    free(words);
    free(G);
    for (int i = 0; i < 26; i++) {
        free(S[i]);
        for (int j = 0; j < word_length; j++) {
            free(T[i][j]);
        }
        free(T[i]);
    }
    free(S);
    free(T);

    return 0;
}

void build_subsets(uint64_t **S, uint64_t ***T, int *array_size) {
    for (int i = 0; i < 26; i++) {
        S[i] = bs_create_empty(word_count, array_size);
        for (int j = 0; j < word_length; j++) {
            T[i][j] = bs_create_empty(word_count, array_size);
        }
    }

    for (int i = 0; i < word_count; i++) {
        char *word = words[i];
        int used_letters[26] = {0};
        for (int j = 0; j < word_length; j++) {
            int letter = word[j] - 'a';
            bs_set(T[letter][j], i);
            used_letters[letter] = 1;
        }
        for (int k = 0; k < 26; k++) {
            if (used_letters[k]) {
                bs_set(S[k], i);
            }
        }

    }
}

// Bitset and ranking functions

uint64_t *bs_create_universe(int usize, int *bs_array_size) {
    int size = usize / 64;
    int leftover = usize % 64;
    if (leftover != 0) size++;
    *bs_array_size = size;

    uint64_t *bitset = malloc(size * sizeof(uint64_t));
    for (int i = 0; i < size; i++) {
        bitset[i] = ~0UL;
    }
    if (leftover != 0) {
        bitset[size - 1] = (1UL << leftover) - 1;
    }

    return bitset;
}

uint64_t *bs_create_empty(int usize, int *bs_array_size){
    int size = usize / 64;
    int leftover = usize % 64;
    if (leftover != 0) size++;
    *bs_array_size = size;

    uint64_t *bitset = malloc(size * sizeof(uint64_t));
    for (int i = 0; i < size; i++) {
        bitset[i] = 0UL;
    }

    return bitset;
}

void bs_set(uint64_t *bs, int bit_idx) {
    int array_idx = bit_idx / 64;
    int bit_position = bit_idx % 64;
    bs[array_idx] |= (1UL << bit_position);
}

int bs_get_first(uint64_t *bs, int bs_array_size) {
    for (int i = 0; i < bs_array_size; i++) {
        if (bs[i] != 0) {
            for (int j = 0; j < 64; j++) {
                if (bs[i] & (1UL << j)) {
                    return i * 64 + j;
                }
            }
        }
    }
    return -1;
}

void bs_intersect(uint64_t *bs1, const uint64_t *bs2, int bs_array_size) {
    for (int i = 0; i < bs_array_size; i++) {
        bs1[i] = bs1[i] & bs2[i];
    }
}

void bs_remove(uint64_t *bs1, const uint64_t *bs2, int bs_array_size) {
    for (int i = 0; i < bs_array_size; i++) {
        bs1[i] = bs1[i] & ~bs2[i];
    }
}

int rank(const char *word){
    int rank = 0;
    int used_letters[26] = {0};
    for (int i = 0; i < strlen(word); i++) {
        int idx = word[i] - 'a';
        if (!used_letters[idx]) {
            rank += letter_count[idx];
            used_letters[idx] = 1;
        }
    }
    return rank;
}

int wordcmp(const void *a, const void *b){
    char *word_a = *(char **)a;
    char *word_b = *(char **)b;
    int rank_a = rank(word_a);
    int rank_b = rank(word_b);

    if (rank_a != rank_b) {
        return rank_b - rank_a;
    }
    return strcmp(word_a, word_b);
}

// Terminal Commands:
// scp "Lab 11"/ex11q1.c lab:~/lab-11-Saad-Mohiuddin/src/ex11q1.c
// ./src/ex11q1 Testcases/1-1-input.txt output.txt