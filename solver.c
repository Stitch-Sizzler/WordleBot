#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "solver.h"

int word_length = 5;
char **la_words = NULL;
char **ta_words = NULL;
int la_count = 0, ta_count = 0;

typedef struct {
    char word[6];
    double e;
} WordScore;

void build_subsets(uint64_t **S, uint64_t ***T, int *array_size);

int get_pattern(const char *guess, const char *target) {
    int g_used[5] = {0}, t_used[5] = {0}, p[5] = {0};
    
    for(int i = 0; i < 5; i++) {
        if(guess[i] == target[i]) { p[i] = 2; g_used[i] = 1; t_used[i] = 1; }
    }
    for(int i = 0; i < 5; i++) {
        if(!g_used[i]) {
            for(int j = 0; j < 5; j++) {
                if(!t_used[j] && guess[i] == target[j]) {
                    p[i] = 1; t_used[j] = 1; break;
                }
            }
        }
    }
    int pattern = 0;
    for(int i = 0; i < 5; i++) { pattern = pattern * 3 + p[i]; }
    return pattern;
}

// Checks if a bit is 1 in the bitset
int bs_contains(uint64_t *bs, int bit_idx) {
    return (bs[bit_idx / 64] & (1UL << (bit_idx % 64))) != 0;
}

// Reads a wordlist into memory
void load_words(const char *filename, char ***word_array, int *count) {
    FILE *f = fopen(filename, "r");
    if (!f) { fprintf(stderr, "Failed to open %s\n", filename); exit(1); }
    char temp[50];
    while (fscanf(f, " %s", temp) == 1) (*count)++;
    rewind(f);
    
    *word_array = malloc(*count * sizeof(char *));
    for (int i = 0; i < *count; i++) {
        (*word_array)[i] = malloc((word_length + 1) * sizeof(char));
        fscanf(f, " %s", (*word_array)[i]);
    }
    fclose(f);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: ./solver <wordle-La.txt> <wordle-Ta.txt> [guess1 result1 ...]\n");
        return 1;
    }

    load_words(argv[1], &la_words, &la_count);
    load_words(argv[2], &ta_words, &ta_count);

    int array_size;

    // Create bitsets for wordle-La.txt only
    uint64_t *G = bs_create_universe(la_count, &array_size);
    uint64_t **S = malloc(26 * sizeof(uint64_t *));
    uint64_t ***T = malloc(26 * sizeof(uint64_t **));
    for (int i = 0; i < 26; i++) {
        T[i] = malloc(word_length * sizeof(uint64_t *));
    }
    build_subsets(S, T, &array_size);

    // Process previous guesses from the command line arguments
    for (int arg = 3; arg < argc; arg += 2) {
        if (arg + 1 >= argc) break;
        char *guess = argv[arg];
        char *result = argv[arg + 1];

        for (int i = 0; i < word_length; i++) {
            int letter = guess[i] - 'a';
            if (result[i] == 'G') {
                bs_intersect(G, T[letter][i], array_size);
            } else if (result[i] == 'Y') {
                bs_intersect(G, S[letter], array_size);
                bs_remove(G, T[letter][i], array_size);
            } else if (result[i] == 'X') {
                bs_remove(G, S[letter], array_size);
            }
        }
    }

    // Count how many valid answers are left
    int rem_count = 0;
    for(int i = 0; i < la_count; i++) {
        if(bs_contains(G, i)) rem_count++;
    }

    if (rem_count == 0) {
        printf("error 0.00\n");
        return 0;
    }

    // If exactly one word is left, print it and exit
    if (rem_count == 1) {
        printf("REM 1\n");
        for(int i = 0; i < la_count; i++) {
            if(bs_contains(G, i)) {
                printf("%s 0.00\n", la_words[i]);
                return 0;
            }
        }
    }

    printf("REM %d\n", rem_count);

    WordScore best[5] = {{"", -1}, {"", -1}, {"", -1}, {"", -1}, {"", -1}};

    int total_words = la_count + ta_count;
    for (int w = 0; w < total_words; w++) {
        char *guess = (w < la_count) ? la_words[w] : ta_words[w - la_count];
        
        int buckets[243] = {0};
        for(int t = 0; t < la_count; t++) {
            if (bs_contains(G, t)) {
                buckets[get_pattern(guess, la_words[t])]++;
            }
        }

        double entropy = 0;
        for(int i = 0; i < 243; i++) {
            if (buckets[i] > 0) {
                double p = (double)buckets[i] / rem_count;
                entropy -= p * log2(p);
            }
        }

        // The Tie-Breaker. If a word is still a valid potential answer, give it a tiny boost.
        if (w < la_count && bs_contains(G, w)) {
            entropy += 0.00001; 
        }

        if (entropy > best[4].e) {
            strcpy(best[4].word, guess); 
            best[4].e = entropy;
            for(int i = 4; i > 0 && best[i].e > best[i-1].e; i--) {
                WordScore temp = best[i];
                best[i] = best[i-1]; 
                best[i-1] = temp;
            }
        }
    }

    for(int i = 0; i < 5; i++) {
        if(best[i].e >= 0) printf("%s %.2f\n", best[i].word, best[i].e);
    }

    return 0;
}


void build_subsets(uint64_t **S, uint64_t ***T, int *array_size) {
    for (int i = 0; i < 26; i++) {
        S[i] = bs_create_empty(la_count, array_size);
        for (int j = 0; j < word_length; j++) {
            T[i][j] = bs_create_empty(la_count, array_size);
        }
    }

    for (int i = 0; i < la_count; i++) {
        char *word = la_words[i];
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

// Standard Bitset functions

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
