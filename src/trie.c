/*
    A trie data structure
*/

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "trie.h"

/* See trie.h */
*trie_t new_trie(char current)
{
    trie_t *t = calloc(1,sizeof(trie_t));
    
    if (t == NULL){
        error("Could not allocate memory for trie_t");
        return NULL;
    } 

    t->current = current;

    t->children = calloc(1, 256*sizeof(*trie_t));
    if (t->children == NULL){
        error("Could not allocate memory for t->children");
        return NULL;
    }

    t->is_word = 0;
    t->parent = NULL;

    return t;
}

/* See trie.h */
int trie_free(trie_t *t)
{
    assert( t != NULL);

    for (i=0; i<256; i++ ){
        if (t->children[i] !=NULL)
            trie_free(t->children[i]);
    }

    free(t);
    return 0;
}


/* See trie.h */
int add_node(char current, trie_t *t)
{
    assert( t != NULL);

    if (t->children[current] == NULL)
        t->children[current] = new_trie(current);

    return 0;  

}

/* See trie.h */
int insert_string(char *word, trie_t *t)
{
    assert(t!=NULL);

    if (*word == '/0'){
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
        return insert_string(word, t->children[curr]);
    }
}
