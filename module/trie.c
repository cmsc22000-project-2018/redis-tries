/* This file implements a new module native data type called "TRIE".
 * The data structure is based on the idea of a trie (aka a prefix 
 * tree) and largely works the same way.
 */
#define IN_TRIE 1
#define NOT_IN_TRIE 0 
#define PARTIAL_IN_TRIE (-1)
// The maximum length of a string we're willing to process
// Longest word in the English dictionary is 45 letters
#define MAXLEN 100

#include "redismodule.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>

static RedisModuleType *trie;

/* ===== Internal data structure (Bare bones functions)  ====== */

/* A prefix trie, otherwise known as a trie */
struct trie {
    // The first trie_t will be '/0' for any Trie.
    char current;
    
    // ALPHABET_SIZE is 256 for all possible characters.     
    struct trie **children;
    
    // if is_word is 1, indicates that this is the end of a word. Otherwise 0.
    int is_word; 

    // parent trie for traversing backwards
    struct trie *parent;
    
    // list of characters that are contained in the node and its children
    char *charlist;
};

/* A simple way to store an approximate match and its score */
typedef struct {
    // The approximate match string
    char *str;

    // The number of edits left
    int edits_left;
} match_t;

/*
    Creates and allocates memory for new trie.
    
    Parameters:
     - current: A char for the current character
    
    Returns:
     - A pointer to the trie, or REDISMODULE_ERR if a pointer 
       cannot be allocated
*/
struct trie *trie_new(char current)
{
    struct trie *t = RedisModule_Calloc(1, sizeof(struct trie));

    if (t == NULL) {
        fprintf(stderr, "Could not allocate memory for trie\n");
        return NULL;
    } 

    t->current = current;
    t->children = RedisModule_Calloc(256, sizeof(struct trie *));

    if (t->children == NULL) {
        fprintf(stderr, "Could not allocate memory for t->children\n");
        return NULL;
    }

    t->is_word = 0;
    t->parent = NULL;
    t->charlist = RedisModule_Calloc(256, sizeof(char));

    return t;
}

/*
    Free an entire trie.

    Parameters:
     - t: A trie pointer
    
    Returns:
     - Always returns 0
*/
int trie_free(struct trie *t)
{
    if (t != NULL) {
        for (int i = 0; i < 256; i++) {
            if (t->children[i] != NULL)
                /* Called recursively because the entire trie 
                   and all of a trie's children are RedisModule_Calloc'ed 
                 */
                trie_free(t->children[i]); 
        }
    }

    RedisModule_Free(t->charlist);
    /* Used because the data structures are 
       originally RedisModule_Calloc'ed 
     */
    RedisModule_Free(t); 
    return 0;
}

/*
    Creates new node in trie.

    Parameters:
     - t: A pointer to the trie where the node is to be added
     - current: A char indicating the character of the node being added
    
    Returns:
     - 0 on success, 1 if an error occurs.
    Details: 
     - Set t->children[current] to be current
     - is_word for new node set to 0.
*/
int trie_add_node(struct trie *t, char current)
{
    assert(t != NULL);

    /* Current is casted because the compiler 
    will throw unnecessary warnings otherwise */
    unsigned int c = (int)current; 

    if (t->children[c] == NULL)
        t->children[c] = trie_new(current);

    return 0;  
}

/*
    Inserts word into trie.
    Parameters:
     - t: A pointer to the given trie
     - word: A char array to be inserted into the given trie
    
    Returns:
     - 0 on success, 1 if error occurs.
    
    Details:
     - For each trie, check if entry of the next character 
       exists in the children array:
          - If so, move into that node in the array
          - If not, add a new node and move into that node in the array
     - Then move on to the next character in string
     - Set the is_word of the last node to 1
*/
int trie_insert_string(struct trie *t, char *word)
{
    assert(t != NULL);

    if (*word == '\0') {
        t->is_word = 1;
        return 0;
    } else {
        int len = strlen(word);
        int index;
        for (int i = 0; i < len; i++) {
            index = (int)word[i];
            t->charlist[index] = word[i];
        }

        char curr = word[0];
	index = (int)curr;

        int rc = trie_add_node(t, curr);
        if (rc != 0) {
            fprintf(stderr, "Fail to add node");
            return 1;
        }

        word++;
        return trie_insert_string(t->children[index], word);
    }
}

/*
    Checks if a char exists in a trie 
    Parameters:
     - t: A pointer to the given trie
     - c: The character we want to check

    Returns:
     - true if c exists in t
     - false if it doesn't
*/
bool trie_char_exists(struct trie *t, char c) 
{
    assert(t != NULL);
    assert(t->charlist != NULL);

    int index = (int)c;

    return (t->charlist[index] != '\0');
}

/* 
    Searches for a word/prefix in a trie. 
 
    Parameters:
     - t: A pointer to the given trie
     - word: A char array in which the end pointer is desired

    Returns: 
     - pointer to the last letter in the word/prefix if word/prefix is found. 
     - NULL if word/prefix is not found.
 */
struct trie *trie_get_subtrie(struct trie *t, char* word)
{
    int len;
    struct trie* curr;
    struct trie** next;

    len = strlen(word);
    curr = t;
    next = t->children;

    /* 
       Iterates through each character of the word
       and goes to child of current trie with index
       of the current character casted as an int
     */
    for (int i = 0; i < len; i++) {
        int j = (int)word[i];
        curr = next[j];
        if (curr == NULL)
            return NULL;
        next = next[j]->children;
    }

    return curr;
}

/* 
    Searches for word in a trie. 
 
    Parameters:
     - t: A pointer to the given trie
     - word: A char array that will be searched for in the trie 

    Returns: 
     - IN_TRIE if word is found. 
     - NOT_IN_TRIE  if word is not found at all.
     - PARTIAL_IN_TRIE if word is found but end node's is_word is 0.
 */
int trie_search(struct trie *t, char* word)
{
    struct trie *end = trie_get_subtrie(t, word);

    if (end == NULL)
        return NOT_IN_TRIE;

    if (end->is_word == 1) 
        return IN_TRIE;

    return PARTIAL_IN_TRIE;
}

/* Helper function for trie_count_completion */
int trie_count_completion_recursive(struct trie *t)
{
    int acc = 0;

    if (t == NULL)
        return acc;

    if (t->is_word == 1)
        acc++;

    for (int i = 0; i < 256; i++)
        acc += trie_count_completion_recursive(t->children[i]);

    return acc;
}

/*
    Count the number of different possible endings of a given prefix in a trie
    
    Parameters:
     - pre: a string of the prefix converned
     - t: a trie pointer
    Returns:
     - an integer of the number of endings if the prefix exists in the trie
     - 0 if the prefix does not exist in the trie
*/
int trie_count_completion(struct trie *t, char *pre)
{
    struct trie *end = trie_get_subtrie(t, pre);

    if (end == NULL)
        return 0;

    return trie_count_completion_recursive(end);
}

/* 
    Since modules do not include header files typically, this is an early 
    declaration of the suggestions() function since helper functions use it
 */
int suggestions(match_t **set, struct trie *t, char *prefix, char *suffix, int edits_left, int n);

/*
    Sees if there are any words in a trie that contain a given prefix
  
    Parameters: 
     - t: A trie. Must point to a trie allocated with trie_new
     - s: A string. The prefix to check
  
    Returns:
     - true if there is a word in the dictionary with the prefix
     - false otherwise
*/
bool trie_has_children(struct trie *t, char *s) 
{    
    return trie_search(t, s) != NOT_IN_TRIE;
}

/*
    Comparison function for qsort(). Returns which match_t has higher precedence.
    - Note that qsort() requires a comparison function as input where the arguments 
    has been cast as void* and the function should return a positive/negative integer
    See here for more info: https://www.geeksforgeeks.org/comparator-function-of-qsort-in-c/

    Parameters: 
     - a, b: match_t structs that have been cast as void*
  
    Returns:
     - a positive integer n if if a should go before b in the sorted list
     - a negative integer n if if b should go before a in the sorted list
     - 0 if a == b
*/
int cmp_match(const void* a, const void* b) 
{
    // Recast a and b as match_t's
    match_t *aa = *(match_t **)a;
    match_t *bb = *(match_t **)b;

    // Put null entries at the end of the list
    if (aa == NULL) {
        return 1;
    } else if (bb == NULL) {
        return -1;
    }

    // Prioritize matches with the higher edits_left score
    if (aa->edits_left != bb->edits_left) {
        return bb->edits_left - aa->edits_left;
    }

    // Otherwise sort lexographically
    return strncmp(aa->str, bb->str, MAXLEN);
}

// Helper function for suggestions that just moves on to the next character
int move_on(match_t **set, struct trie *t, char *prefix, char *suffix, int edits_left, int n)
{
    int rc = EXIT_SUCCESS;
    int len = strlen(prefix);
    char* new_prefix;

    new_prefix = RedisModule_Alloc(sizeof(char) * (MAXLEN + 1));
    if (new_prefix == NULL) {
        return EXIT_FAILURE;
    }

    strncpy(new_prefix, prefix, MAXLEN);

    new_prefix[len] = suffix[0];

    if (trie_has_children(t, new_prefix) == true) {

        // Move on to the next character, don't use up an edit
        rc = suggestions(set, t, new_prefix, suffix + 1, edits_left, n);
    }

    // Save some space now that we're done
    RedisModule_Free(new_prefix);

    return rc;
}

// Helper function for suggestions that tries to remove the first character of the suffix
int try_delete(match_t **set, struct trie *t, char *prefix, char *suffix, int edits_left, int n)
{
    int rc = EXIT_SUCCESS;

    // Don't need to copy the string over as we aren't changing the prefix

    if (trie_has_children(t, prefix) == true) {

        // Adding 1 to the suffix pointer will essentially delete the first character
        rc = suggestions(set, t, prefix, suffix + 1, edits_left - 1, n);
    }

    return rc;
}

// Helper function for suggestions that tries to replace the first character in the suffix and move it to the prefix
int try_replace(match_t **set, struct trie *t, char *prefix, char *suffix, int edits_left, int n)
{
    int i;
    int rc = EXIT_SUCCESS;
    int len = strlen(prefix);
    char *new_prefix;

    // Ran out of space
    if (len == MAXLEN - 1) {
        return EXIT_FAILURE;
    }

    for (i = 1; i < 248; i++) {

        char c = (char)i;

        if (trie_char_exists(t, c) == true) {

            new_prefix = RedisModule_Alloc(sizeof(char) * (MAXLEN + 1));
            if (new_prefix == NULL) {

                return EXIT_FAILURE;

            } else {

                strncpy(new_prefix, prefix, MAXLEN);

                // Try replacing the beginning of the suffix with each ASCII character
                // And move that to the end of the prefix
                new_prefix[len] = c;
                new_prefix[len + 1] = '\0';

                if (trie_has_children(t, new_prefix) == true) {

                    // Adding 1 to the suffix pointer will essentially delete the first character
                    // Shifting the "replaced" character to the prefix
                    rc = suggestions(set, t, new_prefix, suffix + 1, edits_left - 1, n);

                    if (rc != EXIT_SUCCESS) {

                        RedisModule_Free(new_prefix);
                        return EXIT_FAILURE;
                    }
                }

                // Save some space now that we're done
                RedisModule_Free(new_prefix);
            }
        }
    }

    // EXIT_FAILURE would've been caught above, so if we make it here we succeeded
    return EXIT_SUCCESS;
}

// Helper function that tries to swap the last character of the prefix and first character of the prefix
// and append both to the prefix
int try_swap(match_t **set, struct trie *t, char *prefix, char *suffix, int edits_left, int n)
{
    int rc = EXIT_SUCCESS;
    int len = strlen(prefix);
    char* new_prefix;

    if (len == 0 || strlen(suffix) == 0) {
        return EXIT_SUCCESS;
    }

    new_prefix = RedisModule_Alloc(sizeof(char) * (MAXLEN + 1));
    if (new_prefix == NULL) {
        return EXIT_FAILURE;
    }

    strncpy(new_prefix, prefix, MAXLEN);

    // Swap suffix's first character into new_prefix and insert it in front of the last character
    new_prefix[len] = new_prefix[len - 1];
    new_prefix[len - 1] = suffix[0];
    new_prefix[len + 1] = '\0';

    if (trie_has_children(t, new_prefix) == true) {
        // Adding 1 to the suffix pointer will essentially delete the first character
        rc = suggestions(set, t, new_prefix, suffix + 1, edits_left - 1, n);
    }

    // Save some space now that we're done
    RedisModule_Free(new_prefix);

    return rc;
}

// Helper function for suggestions that tries to insert a character to the end of a prefix
int try_insert(match_t **set, struct trie *t, char *prefix, char *suffix, int edits_left, int n)
{
    int i;
    int rc = EXIT_SUCCESS;
    int len = strlen(prefix);
    char* new_prefix;

    // Ran out of space
    if (len == MAXLEN - 1) {
        return EXIT_FAILURE;
    }

    /* 
        Loops through all the ASCII characters
        Crashes if 248 <= i <= 255
        No idea why, only when those specific characters are passed through
        even if you attempt each one individually
     */
    for (i = 1; i < 248; i++) {

        char c = (char)i;

        if (trie_char_exists(t, c) == true) {
            
            new_prefix = RedisModule_Alloc(sizeof(char) * (MAXLEN + 1));
            if (new_prefix == NULL) {

                return EXIT_FAILURE;
            
            } else {

                strncpy(new_prefix, prefix, MAXLEN);

                // Try adding on a new character
                new_prefix[len] = c;
                new_prefix[len + 1] = '\0';

                if (trie_has_children(t, new_prefix) == true) {

                    // Basically just inserting the new ASCII character to the string
                    rc = suggestions(set, t, new_prefix, suffix, edits_left - 1, n);

                    if (rc != EXIT_SUCCESS) {

                        RedisModule_Free(new_prefix);
                        return EXIT_FAILURE;
                    }
                } 
            
                // Save some space now that we're done
                RedisModule_Free(new_prefix);
            }
        }
    }

    // EXIT_FAILURE would've been caught above, so if we make it here we succeeded
    return EXIT_SUCCESS;
}

// Helper function for suggestions(). Attempts to add a match to a suggestion set
int try_add(match_t **set, struct trie *t, char *s, int edits_left, int n)
{
    int i;

    // Check if the current string is in the trie
    if (trie_search(t, s) == IN_TRIE) {

        // Look for the string in the set to update it, or add it
        for (i = 0; i < n; i++) {

            if (set[i] == NULL) {

                // String does not exist in the set, so add it

                set[i] = (match_t*)RedisModule_Alloc(sizeof(match_t));
                if (set[i] == NULL) {
                    return EXIT_FAILURE;
                }

                set[i]->str = RedisModule_Alloc(sizeof(char) * MAXLEN);
                if (set[i]->str == NULL) {
                    return EXIT_FAILURE;
                }
                strcpy(set[i]->str, s);
                set[i]->edits_left = edits_left;

                if (set[i]->str == NULL) {
                    // strcpy failed
                    return EXIT_FAILURE;
                }

                return EXIT_SUCCESS;

            } else if (strcmp(set[i]->str, s) == 0) {

                // String already exists in suggestion set

                // If we found the string in less edits, update its score
                if (set[i]->edits_left < edits_left) {
                    set[i]->edits_left = edits_left;
                }

                // We don't want to trigger adding in place of the worst match
                i = n + 1;

                break;
            }
        }

        // If we hit the maximum amount of items, then see if we can add it in place of the worst match
        if (i == n) {

            i--;

            // Sort the set by edits_left and break ties alphabetically
            qsort(set, n, sizeof(match_t*), cmp_match);

            // If we found it in less edits or match was alphabetically first
            if (set[i]->edits_left < edits_left 
                || (set[i]->edits_left == edits_left && (strncmp(set[i]->str, s, MAXLEN) > 0))) {

                set[i]->str = RedisModule_Alloc(sizeof(char) * MAXLEN);
                if (set[i]->str == NULL) {
                    return EXIT_FAILURE;
                }
                strcpy(set[i]->str, s);
                set[i]->edits_left = edits_left;

                if (set[i]->str == NULL) {
                    // strcpy failed
                    return EXIT_FAILURE;
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

/*
    Performs a mass approximate string match on all the words in a dictionary beginning with a prefix
    For more information on approx matching: https://www.wikiwand.com/en/Approximate_string_matching
  
    Parameters:
     - set: An array of match_t*'s
     - t: A trie. Must point to a trie allocated with trie_new
     - prefix: A prefix of a word. Must be contained in a dictionary or ""
     - suffix: A suffix of a word. Can also be ""
     - edits_left: The maximum levenshtein distance a word can be from prefix+suffix
     - n: The length of set (number of matches in the set)
  
    Returns:
     - 0 for success, or a positive integer n for the number of errors encountered
 */
int suggestions(match_t **set, struct trie *t, char *prefix, char *suffix, int edits_left, int n)
{
    char* s;
    int rc = 0;
    
    // Since prefix and suffix can have max length MAXLEN
    s = RedisModule_Alloc(sizeof(char) * (MAXLEN + 1) * 2);
    if (s == NULL) {
        return EXIT_FAILURE;
    }

    // Now put prefix and suffix together
    strncpy(s, prefix, MAXLEN);
    strncat(s, suffix, MAXLEN);

    if (try_add(set, t, s, edits_left, n) != EXIT_SUCCESS)  {
        return EXIT_FAILURE;
    }

    if (edits_left <= 0) {
        // Hooray for exit conditions!

        RedisModule_Free(s);
        return EXIT_SUCCESS;
    }

    // Make sure we aren't at the end of the suffix
    if (suffix[0] != '\0') {

        rc += move_on(set, t, prefix, suffix, edits_left, n);

        rc += try_delete(set, t, prefix, suffix, edits_left, n);

        rc += try_replace(set, t, prefix, suffix, edits_left, n);

        rc += try_swap(set, t, prefix, suffix, edits_left, n);
    }

    // This one doesn't need any fancy suffix checking
    rc += try_insert(set, t, prefix, suffix, edits_left, n);

    RedisModule_Free(s);

    if (rc != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/*
    Creates array of match_t*'s of spelling suggestions for a word using suggestions()
  
    Parameters:
     - t: A trie. Must point to a trie allocated with trie_new
     - str: A string. This will be the (misspelled) word to match
     - max_edits: the maximum levenshtein distance the words in the set can have
     - n: The length of set (number of matches in the set)
  
    Returns:
     - A pointer to the array of match_t*'s containing the closest n words and their respective distance
     - NULL if there was an error
 */
match_t **suggestion_set_new(struct trie *t, char *str, int max_edits, int n) 
{
    assert(t != NULL);
    assert(str != NULL);

    int i;

    // We'll allocate space for as many matches as we need so we can make sure we get the closest matches
    match_t **set = (match_t **)RedisModule_Calloc(n, sizeof(match_t*));

    if (set == NULL) {
        return NULL;
    }

    if (suggestions(set, t, "", str, max_edits, n) != EXIT_SUCCESS) {
        // suggestions() failed

        for (i = 0; i < n; i++) {
            if (set[i] != NULL) {
                RedisModule_Free(set[i]->str);
                RedisModule_Free(set[i]);
            }
        }

        RedisModule_Free(set);

        return NULL;
    }

    return set;
}

/*
    Takes an array of match_t*'s, sorts it, takes the highest n scoring strings out of it by
    stripping off the match_t wrapper
  
    Parameters:
     - set: An array of match_t*'s
     - n: the number of strings to return
  
    Returns:
     - The first n strings with the smallest distance, where ties are broken by alphabetical order.
       If there aren't enough matching strings, each remaining spot is set to NULL.
     - NULL if there was an error
 */
char** suggestion_set_first_n(match_t **set, int n)
{
    assert(set != NULL);

    int i;

    char **results = (char**)RedisModule_Alloc(sizeof(char*) * n);
    if (results == NULL) {
        return NULL;
    }

    qsort(set, n, sizeof(match_t*), cmp_match);

    // Take the list of strings out of the match_t wrappers
    for (i = 0; i < n; i++) {
        if (set[i] == NULL) {
            results[i] = NULL;
        } else {
            results[i] = set[i]->str;
            RedisModule_Free(set[i]);
        }
    }

    RedisModule_Free(set);

    return results;
}

/*
    Returns the n closest words to a given string in a trie. First creates a match_t* array with 
    suggestion_set_new(), then passes it into suggestion_set_first_n() to sort it and strips the 
    strings of their match_t wrappers.
  
    Parameters:
     - t: A trie. Must point to a trie allocated with trie_new
     - str: A string. This will be the (misspelled) word to match
     - max_edits: the maximum levenshtein distance the words in the set can have
     - n: the number of strings to return. 
  
    Returns:
     - The first n strings with the smallest distance, where ties are broken by alphabetical order.
       If there aren't enough matching strings, each remaining spot is set to NULL.
     - NULL if there was an error
 */
char** suggestion_list(struct trie *t, char *str, int max_edits, int n) {

    assert(t != NULL);
    assert(str != NULL);

    match_t **set = suggestion_set_new(t, str, max_edits, n);

    if (set == NULL) {
        return NULL;
    }

    char **results = suggestion_set_first_n(set, n);

    return results;
}    

/* ===== "trie" type commands (Redis wrapper functions) ===== */

/* TRIE.INSERT key value1 value2... valueN */
int TrieInsert_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, 
        int argc) {
    RedisModule_AutoMemory(ctx); /* Use automatic memory management. */
    
    if (argc <= 2) 
        return RedisModule_WrongArity(ctx);

	RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1],
        REDISMODULE_READ | REDISMODULE_WRITE);
    int type = RedisModule_KeyType(key);
	if (type != REDISMODULE_KEYTYPE_EMPTY &&
        RedisModule_ModuleTypeGetType(key) != trie)
    {
        return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
    }
    size_t dummy;
    /* Number of strings to be inserted */
    int nstrings = argc - 2;
    char *empty = "";
    char **temp = RedisModule_Calloc(nstrings, sizeof(char*));
    for (int i = 0; i < nstrings; i++) {
        temp[i] = strdup(RedisModule_StringPtrLen(argv[2 + i], &dummy));
        if (temp[i] == NULL || strcmp(temp[i], empty) == 0) {
            return RedisModule_ReplyWithError(ctx, "ERR invalid value: must be a string");
        } 
    } 
    
    struct trie *t;
    /* Create an empty value object if the key is currently empty. */
    if (type == REDISMODULE_KEYTYPE_EMPTY) {
    	t = trie_new('\0');
    	RedisModule_ModuleTypeSetValue(key, trie, t);
    } else {
        t = RedisModule_ModuleTypeGetValue(key);
    }

    /* Total return value (from all trie_insert_string calls) */
    long long total = 0;
    /* Insert the new string. */
    for (int i = 0; i < nstrings; i++) {
        total += trie_insert_string(t, temp[i]);
    }
    RedisModule_Free(temp);

	RedisModule_ReplyWithLongLong(ctx, total);    
	RedisModule_ReplicateVerbatim(ctx);
    return REDISMODULE_OK;
}

/* TRIE.CONTAINS key value */
int TrieContains_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, 
        int argc) {
    RedisModule_AutoMemory(ctx); /* Use automatic memory management. */

    if (argc != 3) 
        return RedisModule_WrongArity(ctx);

    RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1],
        REDISMODULE_READ | REDISMODULE_WRITE);
    int type = RedisModule_KeyType(key);
    if (type == REDISMODULE_KEYTYPE_EMPTY) {
    	return RedisModule_ReplyWithError(ctx, "ERR invalid key: not an existing trie");
    }
    else if (RedisModule_ModuleTypeGetType(key) != trie)
    {
        return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
    }
    size_t dummy;
    char *temp = strdup(RedisModule_StringPtrLen(argv[2], &dummy));

    struct trie *t;
    t = RedisModule_ModuleTypeGetValue(key);

    /* Check for the string. */
    int c = trie_search(t, temp);
    
    RedisModule_ReplyWithLongLong(ctx, c);      
    RedisModule_ReplicateVerbatim(ctx);
    return REDISMODULE_OK;
}

/* TRIE.COMPLETIONS key value */
int TrieCompletions_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, 
        int argc) {
    RedisModule_AutoMemory(ctx); /* Use automatic memory management. */

    if (argc != 3) 
        return RedisModule_WrongArity(ctx);

    RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1],
        REDISMODULE_READ | REDISMODULE_WRITE);

    size_t dummy;
    char *temp = strdup(RedisModule_StringPtrLen(argv[2], &dummy));

    struct trie *t;
    t = RedisModule_ModuleTypeGetValue(key);

    /* Check for number of completions */
    int c = trie_count_completion(t, temp);

    RedisModule_ReplyWithLongLong(ctx, c); 
    RedisModule_ReplicateVerbatim(ctx);
    return REDISMODULE_OK;
}

/* TRIE.APPROXMATCH key prefix (optional)max_edit_distance (optional)num_matches */
int TrieApproxMatch_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, 
        int argc) {
    RedisModule_AutoMemory(ctx); /* Use automatic memory management. */

    if (argc <= 2 || argc >= 6) {
        return RedisModule_WrongArity(ctx);
    }

    RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1],
        REDISMODULE_READ | REDISMODULE_WRITE);

    int type = RedisModule_KeyType(key);
    if (type == REDISMODULE_KEYTYPE_EMPTY) {
        return RedisModule_ReplyWithError(ctx, "ERR invalid key: not an existing trie");
    }
    else if (RedisModule_ModuleTypeGetType(key) != trie)
    {
        return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
    }

    size_t dummy;
    char *temp = strdup(RedisModule_StringPtrLen(argv[2], &dummy));
    /* Default max number of edits */
    long long medits = 2;
    /* Default amount of matches (strings) to return */
    long long amount = 10;

    /* Check for optional arguments */
    if (argc == 4) {
        RedisModule_StringToLongLong(argv[3], &medits);
    } else if (argc == 5) {
        RedisModule_StringToLongLong(argv[3], &medits);
        RedisModule_StringToLongLong(argv[4], &amount);
    }
    /* Check if arguments are valid */
    if (amount <= 0) {
        return RedisModule_ReplyWithError(ctx, "ERR invalid amount: cannot be less than 1");
    }
    if (medits < 0) {
        return RedisModule_ReplyWithError(ctx, "ERR invalid max number of edits: cannot be less than 0");
    }

    /* Get the trie */
    struct trie *t;
    t = RedisModule_ModuleTypeGetValue(key);
    /* Find the approximate matches */
    char** matches = suggestion_list(t, temp, medits, amount);

    RedisModule_ReplyWithArray(ctx, amount);
    for (int i = 0; i < amount; i++) {
    	if (matches[i] == NULL) {
    		RedisModule_ReplyWithNull(ctx);
    	}
    	else {
        	RedisModule_ReplyWithSimpleString(ctx, matches[i]);
        }
    }
    RedisModule_ReplicateVerbatim(ctx);
    return REDISMODULE_OK;  
}

/* ===== "trie" type methods (Redis data saving and entry functions) ===== */

/* This function must be present on each Redis module. It is used in order to
 * register the commands into the Redis server. 
 */

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    REDISMODULE_NOT_USED(argv);
    REDISMODULE_NOT_USED(argc);

    if (RedisModule_Init(ctx, "trie123az", 1, REDISMODULE_APIVER_1)
        == REDISMODULE_ERR) 
        return REDISMODULE_ERR;

    RedisModuleTypeMethods tm = {
        .version = REDISMODULE_TYPE_METHOD_VERSION,
        .rdb_load = NULL,
        .rdb_save = NULL,
        .aof_rewrite = NULL,
        .mem_usage = NULL,
        .free = NULL,
        .digest = NULL
    };

    trie = RedisModule_CreateDataType(ctx, "trie123az", 0, &tm);
    if (trie == NULL) 
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "trie.insert",
        TrieInsert_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "trie.contains",
        TrieContains_RedisCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "trie.completions",
        TrieCompletions_RedisCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;    

    if (RedisModule_CreateCommand(ctx, "trie.approxmatch",
        TrieApproxMatch_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    return REDISMODULE_OK;
}
