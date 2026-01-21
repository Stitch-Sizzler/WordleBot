#include "game.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_WORD_LENGTH 50

int word_length = -1;
int word_count;
char _target[MAX_WORD_LENGTH + 1];

/**
 * @brief   Initializes the game by reading the word length and picking a random target word.
 *          If any error occurs, this function will print an error message to `stderr`, close
 *          the file if it was opened, and terminate the process with exit code `1`.
 * @param   word_file   (FILE *)    File pointer to a word file opened in read mode. The file
 *                                  must contain an positive integer N on the first line, each
 *                                  line thereafter must contain one string of N characters.
 */
void game_init(FILE *word_file)
{
    if (word_file == NULL)
    {
        fprintf(stderr, "Error: input file pointer is NULL. Make sure the file was opened correctly.\n");
        exit(1);
    }

    rewind(word_file);

    unsigned int seed;
    if (fscanf(word_file, " %u", &seed) != 1)
    {
        fprintf(stderr, "Error: Failed to read first line of input file. Make sure to open the file using `fopen(filepath, \"r\")`.\n");
        fclose(word_file);
        exit(1);
    }
    srand(seed);

    // Find word length and count the number of words in the file
    word_count = 0;
    word_length = -1;
    while (fscanf(word_file, " %s", _target) == 1)
    {
        word_count++;
        int len = strlen(_target);
        if (word_length == -1) word_length = len;
        if (len != word_length)
        {
            fprintf(stderr, "Error: Word '%s' in input file on line %d does not match the word length of the first word.\n", _target, word_count + 1);
            fclose(word_file);
            exit(1);
        }
    }

    if (word_count == 0)
    {
        fprintf(stderr, "Error: No words found in the input file.\n");
        fclose(word_file);
        exit(1);
    }

    // Pick a random word
    const int rand_word_index = rand() % word_count;

    // Read the random word from the file
    rewind(word_file);
    fscanf(word_file, " %u", &seed);
    for (int i = 0; i <= rand_word_index; i++)
    {
        fscanf(word_file, " %s", _target);
    }

    rewind(word_file);
}

/**
 * @brief   Prints out a coloured string representing the result of a round.
 * @param   guess   (const char *)  The guess made this round.
 * @param   result  (const char *)  The result of this round.
 */
void _print_game_result(const char *guess, const char *result)
{
    for (int i = 0; i < word_length; i++)
    {
        char c = guess[i];
        if (result[i] == 'G') printf("\033[97;42m%c\033[0m", c);
        else if (result[i] == 'Y') printf("\033[97;43m%c\033[0m", c);
        else printf("\033[97;47m%c\033[0m", c);
    }
    printf("\n");
}

/**
 * @brief   Computes a result of a round of the game given a guess word.
 * @param   guess   (const char *)  Guess word. This string must be of length `word_length`.
 * @param   result  (char *)        String where the round result is written to consisting of
 *                                  'X', 'Y', and 'G'. This string must have been allocated a
 *                                  size of `word_length + 1` bytes.
 * @return          (int)           `1` if the round was completed without errors, `0` otherwise.
 */
int game_round(const char *guess, char *result)
{
    #ifdef MANUAL
        printf("%s\n", guess);
        scanf("%s", result);  // Get result manually
        _print_game_result(guess, result);
        return 1;
    #endif

    if (strlen(guess) != word_length)
    {
        return 0;
    }

    // Cache letters that were used already
    int letter_used[word_length];
    for (int i = 0; i < word_length; i++) letter_used[i] = 0;

    // All letters gray by default
    for (int i = 0; i < word_length; i++)
    {
        result[i] = 'X';
    }

    // Find green letters
    for (int i = 0; i < word_length; i++)
    {
        if (guess[i] == _target[i])
        {
            result[i] = 'G';
            letter_used[i] = 1;
        }
    }

    // Find yellow letters
    for (int i = 0; i < word_length; i++)
    {
        for (int j = 0; j < word_length; j++)
        {
            if (guess[i] == _target[j] && !letter_used[j] && result[i] != 'G')
            {
                result[i] = 'Y';
                letter_used[j] = 1;
            }
        }
    }

    _print_game_result(guess, result);

    return 1;
}
