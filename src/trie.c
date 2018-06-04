/*
	 A trie data structure
*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "trie.h"
#include "utils.h"
#include <stdbool.h>

/* See trie.h */
trie_t *trie_new(char current)
{
    trie_t *t = calloc(1, sizeof(trie_t));
    
    if (t == NULL) {
        error("Could not allocate memory for trie_t");
        return NULL;
    } 

    t->current = current;

    t->children = calloc(256, sizeof(trie_t*));
    if (t->children == NULL) {
        error("Could not allocate memory for t->children");
        return NULL;
    }

    t->is_word = 0;
    t->parent = NULL;
    t->charlist = calloc(256, sizeof(char));

    return t;
}

int trie_free(trie_t *t)
{
    assert(t != NULL);

    for (int i = 0; i < 256; i++) {
        if (t->children[i] != NULL)
            trie_free(t->children[i]);
    }
    free(t->charlist);
    free(t);

    return EXIT_SUCCESS;
}

int trie_add_node(trie_t *t, char current)
{
    assert(t != NULL);

    unsigned int c = (unsigned)current;

    if (t->children[c] == NULL)
        t->children[c] = trie_new(current);

    return EXIT_SUCCESS;  
}

int trie_insert_string(trie_t *t, char *word)
{
    assert(t != NULL);

    if (*word == '\0') {
        t->is_word = 1;
        return EXIT_SUCCESS;

    } else {
        int len = strlen(word);
        int index;

        /* 
           For loop that goes through the string
           and adds all the unique characters to the
           trie's wordlist field
         */
        for (int i = 0; i < len; i++) {
            index = (int)word[i];
            t->charlist[index] = word[i];
        }

        char curr = word[0];
        index = (int)curr;

        int rc = trie_add_node(t, curr);
        if (rc != 0) {
            error("Fail to add_node");
            return EXIT_FAILURE;
        }
        
        word++;
        return trie_insert_string(t->children[index],word);
    }
}

bool trie_char_exists(trie_t *t, char c) 
{
    assert(t != NULL);
    assert(t->charlist != NULL);

    int index = (int)c;

    return (t->charlist[index] != '\0');
}

trie_t *trie_get_subtrie(trie_t *t, char* word)
{
    int len;
    trie_t* curr;
    trie_t** next;

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

int trie_search(trie_t *t, char* word)
{
    trie_t *end = trie_get_subtrie(t, word);

    if (end == NULL)
        return NOT_IN_TRIE;

    if (end->is_word == 1) 
        return IN_TRIE;
  
    return PARTIAL_IN_TRIE;
}

int trie_count_completion_recursive(trie_t *t)
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

int trie_count_completion(trie_t *t, char *pre)
{
	trie_t *end = trie_get_subtrie(t, pre);

	if (end == NULL)
		return 0;

	return trie_count_completion_recursive(end);
}
