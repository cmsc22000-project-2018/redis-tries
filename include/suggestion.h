/*
 * A module to generate suggestions for a word
 */

#ifndef INCLUDE_SUGGESTION_H_
#define INCLUDE_SUGGESTION_H_

#include "trie.h"

// The maximum length of a string we're willing to process
// Longest word in english dictionary is 45 letters lol
#define MAXLEN 60

/* A simple way to store an approximate match and its score */
typedef struct {
    char *str;
    int edits_left;
} match_t;

/*
 * Sees if there are any words in a trie that contain a given prefix
 * NOTE- awaiting the actual function from support tools
 * 
 * Parameters: 
 *  - t: A trie. Must point to a trie allocated with trie_new
 *  - s: A string. The prefix to check
 * 
 * Returns:
 *  - EXIT_SUCCESS if there is a word in the dictionary with the prefix
 *  - EXIT_FAILURE otherwise
*/
int has_children(trie_t *t, char *s);

/*
 * Comparison function for qsort(). Returns which match_t has higher precedence.
 * - Note that qsort() requires a comparison function as input where the arguments 
 *   has been cast as void* and the function should return a positive/negative integer
 * See here for more info: https://www.geeksforgeeks.org/comparator-function-of-qsort-in-c/
 * 
 * Parameters: 
 *  - a, b: match_t structs that have been cast as void*
 * 
 * Returns:
 *  - a positive integer n if if a should go before b in the sorted list
 *  - a negative integer n if if b should go before a in the sorted list
 *  - 0 if a == b
*/
int cmp_match(const void* a, const void* b);

/*
 * Helper function for suggestions(). Adds or updates a new match_t to an array of match_t*'s
 * 
 * Parameters:
 *  - set: An array of match_t's
 *  - t: A trie. Must point to a dictionary allocated with trie_new
 *  - s: The string to add
 *  - edits_left: The maximum levenshtein distance a word can be from prefix+suffix
 *  - n: The length of set (number of matches in the set)
 * 
 * Returns:
 *  - EXIT_FAILURE if there was a memory allocation error, EXIT_SUCCESS otherwise
 */
int attempt_add(match_t **set, trie_t *t, char *s, int edits_left, int n);

/*
 * Performs a mass approximate sting match on all the words in a dictionary beginning with a prefix
 * For more information on approx matching: https://www.wikiwand.com/en/Approximate_string_matching
 * 
 * Parameters:
 *  - set: An array of match_t*'s
 *  - t: A trie. Must point to a trie allocated with trie_new
 *  - prefix: A prefix of a word. Must be contained in a dictionary or ""
 *  - suffix: A suffix of a word. Can also be ""
 *  - edits_left: The maximum levenshtein distance a word can be from prefix+suffix
 *  - n: The length of set (amount of matches in the set)
 * 
 * Returns:
 *  - 0 for success, or a positive integer n for the number of errors encountered
 */
int suggestions(match_t **set, trie_t *t, char *prefix, char *suffix, int edits_left, int n);

/*
 * Creates array of match_t*'s of spelling suggestions for a word using suggestions()
 * 
 * Parameters:
 *  - t: A trie. Must point to a trie allocated with trie_new
 *  - str: A string. This will be the (misspelled) word to match
 *  - max_edits: the maximum levenshtein distance the words in the set can have
 *  - n: The length of set (number of matches in the set)
 * 
 * Returns:
 *  - A pointer to the array of match_t*'s containing the closest n words and their respective distance
 *  - NULL if there was an error
 */
match_t** suggestion_set_new(trie_t *t, char *str, int max_edits, int n);

/*
 * Takes an array of match_t*'s, sorts it, takes the highest n scoring strings out of it by
 * stripping off the match_t wrapper
 * 
 * Parameters:
 *  - set: An array of match_t*'s
 *  - n: the number of strings to return
 * 
 * Returns:
 *  - The first n strings with the smallest distance, where ties are broken by alphabetical order.
 *    If there aren't enough matching strings, each remaining spot is set to NULL.
 *  - NULL if there was an error
 */
char** suggestion_set_first_n(match_t **set, int n);

/*
 * Returns the n closest words to a given string in a trie. First creates a match_t* array with 
 * suggestion_set_new(), then passes it into suggestion_set_first_n() to sort it and strips the 
 * strings of their match_t wrappers.
 * 
 * Parameters:
 *  - t: A trie. Must point to a trie allocated with trie_new
 *  - str: A string. This will be the (misspelled) word to match
 *  - max_edits: the maximum levenshtein distance the words in the set can have
 *  - n: the number of strings to return. 
 * 
 * Returns:
 *  - The first n strings with the smallest distance, where ties are broken by alphabetical order.
 *    If there aren't enough matching strings, each remaining spot is set to NULL.
 *  - NULL if there was an error
 */
char** suggestion_list(trie_t *t, char *str, int max_edits, int n);

#endif