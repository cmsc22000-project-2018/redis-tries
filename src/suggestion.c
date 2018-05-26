/*
 * A series of functions to generate spelling suggestions
 * 
 * See suggestion.h for function documentation
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "suggestion.h"
#include "trie.h"

int has_children(trie_t *t, char *s) {
    // if (t->is_word == 0) {
    //     return 0;
    // }
    // for (int i = 0; i < 256; i++) {
    //     if (t->children[i] != NULL) {
    //         return 0;
    //     }
    // }

    if (1 || t || s)
        return EXIT_SUCCESS;
}

/* See suggestion.h */
int cmp_match(const void* a, const void* b) {

    const match_t* aa = *(const match_t**)a;
    const match_t* bb = *(const match_t**)b;

    // Put null entries at the end of the list
    if (aa == NULL) {
        return 1;
    } else if (bb == NULL) {
        return -1;
    }

    // Prioritize matches with the higher edits_left score
    if (aa->edits_left != bb->edits_left) {
        return aa->edits_left - bb->edits_left;
    }

    // Otherwise sort lexographically
    return strcmp(aa->str,bb->str);
}

// Helper function for suggestions that just moves on to the next character
int move_on(match_t **set, trie_t *t, char *prefix, char *suffix, int edits_left, int max_matches) {

    int rc = 0;
    int len = strnlen(prefix, MAXLEN);
    char* new_prefix;

    new_prefix = malloc(sizeof(char) * (MAXLEN + 1));
    if (new_prefix == NULL) {
        return rc + EXIT_FAILURE;
    }

    strncpy(new_prefix, prefix, MAXLEN);

    new_prefix[len] = suffix[0];

    if (has_children(t, new_prefix) == EXIT_SUCCESS) {

        // Move on to the next character, don't use up an edit
        rc += suggestions(set, t, new_prefix, suffix + 1, edits_left, max_matches);
    }

    // Save some space now that we're done
    free(new_prefix);

    return rc + EXIT_SUCCESS;
}

// Helper function for suggestions that tries to remove the first character of the suffix
int try_delete(match_t **set, trie_t *t, char *prefix, char *suffix, int edits_left, int max_matches) {

    int rc = 0;

    // Don't need to copy the string over as we aren't changing the prefix

    if (has_children(t, prefix) == EXIT_SUCCESS) {

        // Adding 1 to the suffix pointer will essentially delete the first character
        rc += suggestions(set, t, prefix, suffix + 1, edits_left - 1, max_matches);
    }

    return rc + EXIT_SUCCESS;
}

// Helper function for suggestions that tries to replace the first character in the suffix and move it to the prefix
int try_replace(match_t **set, trie_t *t, char *prefix, char *suffix, int edits_left, int max_matches) {

    int i;
    int rc = 0;
    int len = strnlen(prefix, MAXLEN);
    char *new_prefix;

    // Ran out of space
    if (len == MAXLEN - 1) {
        return rc + EXIT_FAILURE;
    }

    for (i = 1; i < 256; i++) {

        char c = (char)i;

        if (trie_char_exists(t, c) == EXIT_SUCCESS) {

            new_prefix = malloc(sizeof(char) * (MAXLEN + 1));
            if (new_prefix == NULL) {
                return rc + EXIT_FAILURE;
            } else {

                strncpy(new_prefix, prefix, MAXLEN);

                // Try replacing the beginning of the suffix with each ASCII character
                // And move that to the end of the prefix
                new_prefix[len] = c;
                new_prefix[len + 1] = '\0';

                if (has_children(t, new_prefix) == EXIT_SUCCESS) {

                    // Adding 1 to the suffix pointer will essentially delete the first character
                    // Shifting the "replaced" character to the prefix
                    rc += suggestions(set, t, new_prefix, suffix + 1, edits_left - 1, max_matches);
                }

                // Save some space now that we're done
                free(new_prefix);
            }
        }
    }

    return rc + EXIT_SUCCESS;
}

// Helper function that tries to swap the last character of the prefix and first character of the prefix
// and append both to the prefix
int try_swap(match_t **set, trie_t * t, char *prefix, char *suffix, int edits_left, int max_matches) {

    int rc = 0;
    int len = strnlen(prefix, MAXLEN);
    char* new_prefix;

    if (len == 0 || strnlen(suffix, MAXLEN) == 0) {
        return rc + EXIT_SUCCESS;
    }

    new_prefix = malloc(sizeof(char) * (MAXLEN + 1));
    if (new_prefix == NULL) {
        return rc + EXIT_FAILURE;
    }

    strncpy(new_prefix, prefix, MAXLEN);

    // Swap suffix's first character into new_prefix and insert it in front of the last character
    new_prefix[len] = new_prefix[len - 1];
    new_prefix[len - 1] = suffix[0];
    new_prefix[len + 1] = '\0';

    if (has_children(t, new_prefix) == EXIT_SUCCESS) {
        // Adding 1 to the suffix pointer will essentially delete the first character
        rc += suggestions(set, t, new_prefix, suffix + 1, edits_left - 1, max_matches);
    }

    // Save some space now that we're done
    free(new_prefix);

    return rc + EXIT_SUCCESS;
}

// Helper function for suggestions that tries to insert a character to the end of a prefix
int try_insert(match_t **set, trie_t *t, char *prefix, char *suffix, int edits_left, int max_matches) {

    int i;
    int rc = 0;
    int len = strnlen(prefix, MAXLEN);
    char* new_prefix;

    // Ran out of space
    if (len == MAXLEN - 1) {
        return rc + EXIT_FAILURE;
    }

    for (i = 1; i < 256; i++) {

        char c = (char)i;

        if (trie_char_exists(t, c) == EXIT_SUCCESS) {
            
            new_prefix = malloc(sizeof(char) * (MAXLEN + 1));
            if (new_prefix == NULL) {
                return rc + EXIT_FAILURE;
            } else {

                strncpy(new_prefix, prefix, MAXLEN);

                // Try adding on a new character
                new_prefix[len] = c;
                new_prefix[len + 1] = '\0';

                if (has_children(t, new_prefix) == EXIT_SUCCESS) {

                    // Basically just inserting the new ASCII character to the string
                    rc += suggestions(set, t, new_prefix, suffix, edits_left - 1, max_matches);
                } 
            
                // Save some space now that we're done
                free(new_prefix);
            }
        }
    }

    return rc + EXIT_SUCCESS;
}

// Helper function for suggestions(). Attempts to add a match to a suggestion set
int attempt_add(match_t **set, trie_t *t, char *s, int edits_left, int max_matches) {
    int i;

    // Check if the current string is in the trie
    if (trie_search(t, s) == EXIT_SUCCESS) {

        // Look for the string in the set to update it, or add it
        for (i = 0; i < max_matches; i++) {

            // String already exists in suggestion set
            if (strcmp(set[i]->str, s) == 0) {

                // If we found the string in less edits, update its score
                if (set[i]->edits_left > edits_left) {
                    set[i]->edits_left = edits_left;
                }

            } else if (set[i] == NULL) {

                // String does not exist in the set, so add it

                set[i] = (match_t*)malloc(sizeof(match_t));
                if (set[i] == NULL) {
                    return EXIT_FAILURE;
                }

                set[i]->str = strndup(s, MAXLEN);
                set[i]->edits_left = edits_left;

                if (set[i]->str == NULL) {
                    // strndup failed
                    return EXIT_FAILURE;
                }

                break;
            }
        }

        // If we hit the maximum amount of items, then see if we can add it in place of the worst match
        if (i == max_matches - 1) {

            // Sort the set by edits_left and break ties alphabetically
            qsort(set, sizeof(match_t*), max_matches, cmp_match);

            // If we found it in less edits or match was alphabetically first
            if (set[i]->edits_left < edits_left || strncmp(set[i]->str, s, MAXLEN) < 0) {

                set[i]->str = strndup(s, MAXLEN);
                set[i]->edits_left = edits_left;

                if (set[i]->str == NULL) {
                    // strndup failed
                    return EXIT_FAILURE;
                }
            }
        }
    }

    return EXIT_SUCCESS;
}

// Look at suggestion.h for documentation
int suggestions(match_t **set, trie_t *t, char *prefix, char *suffix, int edits_left, int max_matches) {

    char* s;
    int rc = 0;
    
    // Since prefix and suffix can have max length MAXLEN
    s = malloc(sizeof(char) * (MAXLEN + 1) * 2);
    if (s == NULL) {
        return rc + EXIT_FAILURE;
    }

    // Now put prefix and suffix together
    strncpy(s, prefix, MAXLEN);
    strncat(s, suffix, MAXLEN);

    if (attempt_add(set, t, s, edits_left, max_matches) != EXIT_SUCCESS)  {
        return EXIT_FAILURE;
    }

    if (edits_left <= 0) {
        // Hooray for exit conditions!

        free(s);
        return rc;
    }

    // Make sure we aren't at the end of the suffix
    if (suffix[0] != '\0') {

        rc += move_on(set, t, prefix, suffix, edits_left, max_matches);

        rc += try_delete(set, t, prefix, suffix, edits_left, max_matches);

        rc += try_replace(set, t, prefix, suffix, edits_left, max_matches);

        rc += try_swap(set, t, prefix, suffix, edits_left, max_matches);
    }

    // This one doesn't need any fancy suffix stuff
    rc += try_insert(set, t, prefix, suffix, edits_left, max_matches);

    free(s);

    return rc;
}    

match_t** suggestion_set_new(trie_t *t, char *str, int max_edits, int max_matches) {

    assert(t != NULL);
    assert(str != NULL);

    // We'll allocate 4 times as many matches so we can make sure we get the closest matches
    match_t **set = (match_t**)calloc(max_matches, sizeof(match_t*));

    if (suggestions(set, t, "", str, max_edits, max_matches) != 0) {
        return NULL;
    }

    return set;
}

char** suggestion_set_first_n(match_t **set, int n) {
    assert(set != NULL);

    int i;

    char **results = (char**)malloc(sizeof(char*) * n);
    if (results == NULL) {
        return NULL;
    }

    qsort(set, sizeof(match_t*), n, cmp_match);

    // Take the list of strings out of the match_t wrappers
    for (i = 0; i < n; i++) {
        if (set[i] == NULL) {
            results[i] = NULL;
        } else {
            results[i] = set[i]->str;
            free(set[i]);
        }
    }

    free(set);

    return results;
}

char** suggestion_list(trie_t *t, char *str, int max_edits, int amount) {

    assert(t != NULL);
    assert(str != NULL);

    match_t **set = suggestion_set_new(t, str, max_edits, amount);

    if (set == NULL) {
        return NULL;
    }

    char **results = suggestion_set_first_n(set, amount);

    return results;
}