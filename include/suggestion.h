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
 *  - t: A tire. Must point to a trie allocated with trie_new
 *  - s: A string. The prefix to check
 * 
 * Returns:
 *  - 0 if there is a word in the dictionary with the prefix
 *  - 1 otherwise
*/
int has_children(trie_t *t, char *s);

/*
 * Comparison function for qsort(). Returns which match_t has higher precedence
 * 
 * Parameters: 
 *  - a, b: match_t structs that have been cast as void
 * 
 * Returns:
 *  - 1 if a has higher precedence than b, or -1 if b has higher precedence than a, or 0 if tied
*/
int cmp_match(const void* a, const void* b);

/*
 * Helper function for suggestions(). Adds or updates a new match_t to a list of match_t's
 * 
 * Parameters:
 *  - set: A lit of match_t's
 *  - t: A trie. Must point to a dictionary allocated with trie_new
 *  - s: The string to add
 *  - edits_left: The maximum levenshtein distance a word can be from prefix+suffix
 *  - max_matches: The length of set (amount of matches in the set)
 * 
 * Returns:
 *  - EXIT_FAILURE if there was a memory allocation error, EXIT_SUCCESS otherwise
 */
int attempt_add(match_t **set, trie_t *t, char *s, int edits_left, int max_matches);

/*
 * Does a fuzzy sting match on all the words in a dictionary
 * 
 * Parameters:
 *  - set: A lit of match_t's
 *  - t: A trie. Must point to a trie allocated with trie_new
 *  - prefix: A prefix of a word. Must be contained in a dictionary or ""
 *  - suffix: A suffix of a word. Can also be ""
 *  - edits_left: The maximum levenshtein distance a word can be from prefix+suffix
 *  - max_matches: The length of set (amount of matches in the set)
 * 
 * Returns:
 *  - 0 for success, or a positive integer n for the number of errors encountered
 */
int suggestions(match_t **set, trie_t *t, char *prefix, char *suffix, int edits_left, int max_matches);

/*
 * Creates a redis sorted set of spelling suggestions for a word using suggestions
 * 
 * Parameters:
 *  - t: A trie. Must point to a trie allocated with trie_new
 *  - str: A string. This will be the (misspelled) word to match
 *  - max_edits: the maximum levenshtein distance the words in the set can have
 *  - max_matches: The length of set (amount of matches in the set)
 * 
 * Returns:
 *  - A pointer to the sorted set containing the words or
 *  - NULL if there was an error
 */
match_t** suggestion_set_new(trie_t *t, char *str, int max_edits, int max_matches);

/*
 * Given a redis sorted set, takes the highest n scoring strings out of it
 * 
 * Parameters:
 *  - set: A lit of match_t's
 *  - n: the amount of strings to return. Strings are the ones with the smallest
 *          distance and ties broken by reverse alphabetic order
 * 
 * Returns:
 *  - The first n strings matching above, or NULL if there was an error
 *  - If there ween't enough matching strings, NULL is returned in each remaining spot
 */
char** suggestion_set_first_n(match_t **set, int n);

/*
 * Returns the n closest words to a given string in a dictionary
 * 
 * Parameters:
 *  - t: A trie. Must point to a trie allocated with trie_new
 *  - str: A string. This will be the (misspelled) word to match
 *  - max_edits: the maximum levenshtein distance the words in the set can have
 *  - the amount of strings to return. Strings are the ones with the smallest
 *          distance and ties broken by alphabetic order
 * 
 * Returns: 
 *  - The first n strings matching above, or NULL if there was an error
 *  - If there ween't enough matching strings, NULL is returned in each remaining spot
 */
char** suggestion_list(trie_t *t, char *str, int max_edits, int amount);

#endif