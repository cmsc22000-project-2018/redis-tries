/*
    A trie data structure
*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "trie.h"
#include "utils.h"

/* See trie.h */
trie_t *new_trie(char current)
{
    trie_t *t = calloc(1,sizeof(trie_t));
    
    if (t == NULL){
        error("Could not allocate memory for trie_t");
        return NULL;
    } 

    t->current = current;

    t->children = calloc(256,sizeof(trie_t*));
    if (t->children == NULL){
        error("Could not allocate memory for t->children");
        return NULL;
    }

    t->is_word = 0;
    t->parent = NULL;

    return t;
}

int trie_free(trie_t *t)
{
    assert( t != NULL);

    for (int i=0; i<256; i++ ){
        if (t->children[i] !=NULL)
            trie_free(t->children[i]);
    }

    free(t);
    return 0;
}

int add_node(char current, trie_t *t)
{
    assert( t != NULL);

    unsigned c = (unsigned) current;

    if (t->children[c] == NULL)
        t->children[c] = new_trie(current);

    return 0;  

}

int insert_string(char *word, trie_t *t)
{
    assert(t!=NULL);

    if (*word == '\0'){
        t->is_word=1;
        return 0;

    } else {

        char curr = *word;

        int rc = add_node(curr, t);
        if (rc != 0){
            error("Fail to add_node");
            return 1;
        }
        
        word++;
        return insert_string(word, t->children[(unsigned)curr]);
    }
}


trie_t *trie_search_end(char* word, trie_t *t)
{

    int len;
    trie_t* curr;
    trie_t** next;

    len = strlen(word);
    curr = t;
    next = t->children;

    for (int i=0; i<len; i++) {
        int j = (int) word[i];
        curr = next[j];

        if (curr == NULL)
            return NULL;

        next = next[j]->children;
    }

    return curr;
}


int trie_search(char* word, trie_t *t)
{
    trie_t *end = trie_search_end(word,t);

    if (end == NULL)
        return NOT_IN_TRIE;

    if (end->is_word == 1) 
        return IN_TRIE;

    return PARTIAL_IN_TRIE;
}

int count_completion_recursive( trie_t *t){

	int acc = 0;

	if (t == NULL)
		return acc;

	if (t->is_word == 1)
		acc++;

	for (int i=0;i<256;i++)
		acc += count_completion_recursive(t->children[i]);

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
int count_completion(char *pre, trie_t *t){

	trie_t *end = trie_search_end(pre, t);

	if (end == NULL)
		return 0;

	return count_completion_recursive(end);
	
}



