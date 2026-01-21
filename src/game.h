#ifndef GAME_H
#define GAME_H

#include <stdio.h>

extern int word_length;
extern int word_count;

/**
 * @brief   Initializes the game by reading the word length and picking a random target word.
 *          If any error occurs, this function will print an error message to `stderr`, close
 *          the file if it was opened, and terminate the process with exit code `1`.
 * @param   word_file   (FILE *)    File pointer to a word file opened in read mode. The file
 *                                  must contain an positive integer N on the first line, each
 *                                  line thereafter must contain one string of N characters.
 */
void game_init(FILE *word_file);

/**
 * @brief   Computes a result of a round of the game given a guess word.
 * @param   guess   (const char *)  Guess word. This string must be of length `word_length`.
 * @param   result  (char *)        String where the round result is written to consisting of
 *                                  'X', 'Y', and 'G'. This string must have been allocated a
 *                                  size of `word_length + 1` bytes.
 * @return          (int)           `1` if the round was completed without errors, `0` otherwise.
 */
int game_round(const char *guess, char *result);

/**
 * @brief   Prints out a coloured string representing the result of a round.
 * @param   guess   (const char *)  The guess made this round.
 * @param   result  (const char *)  The result of this round.
 */
void _print_game_result(const char *guess, const char *result);

#endif
