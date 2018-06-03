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

bool has_children(trie_t *t, char *s) {
    
    if (trie_search(t, s) == NOT_IN_TRIE) {
        return false;
    }

    return true;
}

/* See suggestion.h */
int cmp_match(const void* a, const void* b) {

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
int move_on(match_t **set, trie_t *t, char *prefix, char *suffix, int edits_left, int n) {

    int rc = EXIT_SUCCESS;
    int len = strlen(prefix);
    char* new_prefix;

    new_prefix = malloc(sizeof(char) * (MAXLEN + 1));
    if (new_prefix == NULL) {
        return EXIT_FAILURE;
    }

    strncpy(new_prefix, prefix, MAXLEN);

    new_prefix[len] = suffix[0];

    if (has_children(t, new_prefix) == true) {

        // Move on to the next character, don't use up an edit
        rc = suggestions(set, t, new_prefix, suffix + 1, edits_left, n);
    }

    // Save some space now that we're done
    free(new_prefix);

    return rc;
}

// Helper function for suggestions that tries to remove the first character of the suffix
int try_delete(match_t **set, trie_t *t, char *prefix, char *suffix, int edits_left, int n) {

    int rc = EXIT_SUCCESS;

    // Don't need to copy the string over as we aren't changing the prefix

    if (has_children(t, prefix) == true) {

        // Adding 1 to the suffix pointer will essentially delete the first character
        rc = suggestions(set, t, prefix, suffix + 1, edits_left - 1, n);
    }

    return rc;
}

// Helper function for suggestions that tries to replace the first character in the suffix and move it to the prefix
int try_replace(match_t **set, trie_t *t, char *prefix, char *suffix, int edits_left, int n) {

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

            new_prefix = malloc(sizeof(char) * (MAXLEN + 1));
            if (new_prefix == NULL) {
                return EXIT_FAILURE;
            } else {

                strncpy(new_prefix, prefix, MAXLEN);

                // Try replacing the beginning of the suffix with each ASCII character
                // And move that to the end of the prefix
                new_prefix[len] = c;
                new_prefix[len + 1] = '\0';

                if (has_children(t, new_prefix) == true) {

                    // Adding 1 to the suffix pointer will essentially delete the first character
                    // Shifting the "replaced" character to the prefix
                    rc = suggestions(set, t, new_prefix, suffix + 1, edits_left - 1, n);

                    if (rc != EXIT_SUCCESS) {

                        free(new_prefix);
                        return EXIT_FAILURE;
                    }
                }

                // Save some space now that we're done
                free(new_prefix);
            }
        }
    }

    // EXIT_FAILURE would've been caught above, so if we make it here we succeeded
    return EXIT_SUCCESS;
}

// Helper function that tries to swap the last character of the prefix and first character of the prefix
// and append both to the prefix
int try_swap(match_t **set, trie_t * t, char *prefix, char *suffix, int edits_left, int n) {

    int rc = EXIT_SUCCESS;
    int len = strlen(prefix);
    char* new_prefix;

    if (len == 0 || strlen(suffix) == 0) {
        return EXIT_SUCCESS;
    }

    new_prefix = malloc(sizeof(char) * (MAXLEN + 1));
    if (new_prefix == NULL) {
        return EXIT_FAILURE;
    }

    strncpy(new_prefix, prefix, MAXLEN);

    // Swap suffix's first character into new_prefix and insert it in front of the last character
    new_prefix[len] = new_prefix[len - 1];
    new_prefix[len - 1] = suffix[0];
    new_prefix[len + 1] = '\0';

    if (has_children(t, new_prefix) == true) {
        // Adding 1 to the suffix pointer will essentially delete the first character
        rc = suggestions(set, t, new_prefix, suffix + 1, edits_left - 1, n);
    }

    // Save some space now that we're done
    free(new_prefix);

    return rc;
}

// Helper function for suggestions that tries to insert a character to the end of a prefix
int try_insert(match_t **set, trie_t *t, char *prefix, char *suffix, int edits_left, int n) {

    int i;
    int rc = EXIT_SUCCESS;
    int len = strlen(prefix);
    char* new_prefix;

    // Ran out of space
    if (len == MAXLEN - 1) {
        return EXIT_FAILURE;
    }

    for (i = 1; i < 248; i++) {

        char c = (char)i;

        if (trie_char_exists(t, c) == true) {
            
            new_prefix = malloc(sizeof(char) * (MAXLEN + 1));
            if (new_prefix == NULL) {
                return EXIT_FAILURE;
            } else {

                strncpy(new_prefix, prefix, MAXLEN);

                // Try adding on a new character
                new_prefix[len] = c;
                new_prefix[len + 1] = '\0';

                if (has_children(t, new_prefix) == true) {

                    // Basically just inserting the new ASCII character to the string
                    rc = suggestions(set, t, new_prefix, suffix, edits_left - 1, n);

                    if (rc != EXIT_SUCCESS) {

                        free(new_prefix);
                        return EXIT_FAILURE;
                    }
                } 
            
                // Save some space now that we're done
                free(new_prefix);
            }
        }
    }

    // EXIT_FAILURE would've been caught above, so if we make it here we succeeded
    return EXIT_SUCCESS;
}

// Helper function for suggestions(). Attempts to add a match to a suggestion set
int try_add(match_t **set, trie_t *t, char *s, int edits_left, int n) {
    int i;

    // Check if the current string is in the trie
    if (trie_search(t, s) == IN_TRIE) {

        // Look for the string in the set to update it, or add it
        for (i = 0; i < n; i++) {
            if (set[i] == NULL) {

                // String does not exist in the set, so add it

                set[i] = (match_t*)malloc(sizeof(match_t));
                if (set[i] == NULL) {
                    return EXIT_FAILURE;
                }

                set[i]->str = malloc(sizeof(char) * MAXLEN);
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

                set[i]->str = malloc(sizeof(char) * MAXLEN);
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

// Look at suggestion.h for documentation
int suggestions(match_t **set, trie_t *t, char *prefix, char *suffix, int edits_left, int n) {

    char* s;
    int rc = 0;
    
    // Since prefix and suffix can have max length MAXLEN
    s = malloc(sizeof(char) * (MAXLEN + 1) * 2);
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

        free(s);
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

    free(s);

    if (rc != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}    

match_t **suggestion_set_new(trie_t *t, char *str, int max_edits, int n) {

    assert(t != NULL);
    assert(str != NULL);

    int i;

    // We'll allocate space for as many matches as we need so we can make sure we get the closest matches
    match_t **set = (match_t **)calloc(n, sizeof(match_t*));

    if (set == NULL) {
        return NULL;
    }

    if (suggestions(set, t, "", str, max_edits, n) != EXIT_SUCCESS) {
        // suggestions() failed

        for (i = 0; i < n; i++) {
            if (set[i] != NULL) {
                free(set[i]->str);
                free(set[i]);
            }
        }

        free(set);

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

    qsort(set, n, sizeof(match_t*), cmp_match);

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