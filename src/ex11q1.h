#ifndef EX11Q1_H
#define EX11Q1_H

#include <stdint.h>

/**
 * @brief   Dynamically allocates an array of unsigned ints large enough to contain
 *          `usize` bits. Bitsize is initialized to be the universe set.
 * @param   usize       (int)           Number of elements in the universe set.
 * @param   bs_arr_size (int *)         Buffer where the size of the bitset as an array is written to.
 * @return              (uint64_t *)    Dynamically allocated array of `uint64_t`s.
 */
uint64_t *bs_create_universe(int usize, int *bs_arr_size);

/**
 * @brief   Dynamically allocates an array of unsigned ints large enough to contain
 *          `usize` bits. Bitsize is initialized to be an empty set.
 * @param   usize       (int)           Number of elements in the universe set.
 * @param   bs_arr_size (int *)         Buffer where the size of the bitset as an array is written to.
 * @return              (uint64_t *)    Dynamically allocated array of `uint64_t`s.
 */
uint64_t *bs_create_empty(int usize, int *bs_arr_size);

/**
 * @brief   Sets a bit to `1` in a bitset.
 * @param   bitset      (uint64_t *)`   Target bitset.
 * @param   bit_idx     (int)           Index of the bit to set to `1`.
 */
void bs_set(uint64_t *bitset, int bit_idx);

/**
 * @brief   Gets the first element found in a bitset.
 * @param   bitset              (uint64_t *)    Target bitset.
 * @param   bs_array_size       (int)           Size of the bitset as an array.
 * @return                      (int)           The first element in the bitset if it exists, `-1` otherwise.
 */
int bs_get_first(uint64_t *bitset, int bs_array_size);

/**
 * @brief   Computes the intersection of two bitsets.
 * @param   bs1                 (uint64_t *)        First operand. Also where the intersection of
 *                                                  both operands is written to, must be big enough.
 * @param   bs2                 (const uint64_t *)  Second operand.
 * @param   bs_array_size       (int)               Size of the bitsets as arrays.
 */
void bs_intersect(uint64_t *bs1, const uint64_t *bs2, int bs_array_size);

/**
 * @brief   Computes the set difference of two bitsets.
 * @param   bs1                 (uint64_t *)        First operand. Also where the set difference
 *                                                  of both operands is written to, must be big enough.
 * @param   bs2                 (const uint64_t *)  Second operand.
 * @param   bs_array_size       (int)            Size of the bitsets as arrays.
 */
void bs_remove(uint64_t *bs1, const uint64_t *bs2, int bs_array_size);

#endif
