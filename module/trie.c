/* This file implements a new module native data type called "TRIE".
 * The data structure is based on the idea of a trie (aka a prefix 
 * tree) and largely works the same way.
 */

#include "../redismodule.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "utils.h"

static RedisModuleType *trie;

/* ========================== Internal data structure (Bare bones functions)  ======================= */

typedef struct trie trie;

struct trie {
    char current; 
        // The first trie_t will be '/0' for any Trie.
    trie **children;
         // ALPHABET_SIZE is 256 for all possible characters.
    int is_word; 
        // if is_word is 1, indicates that this is the end of a word. Otherwise 0.
    trie *parent;
        // parent trie for traversing backwards
};

/*
    Creates and allocates memory for new trie.
    
    Parameters:
     - current: A char for the current character
    
    Returns:
     - A pointer to the trie, or REDISMODULE_ERR if a pointer 
       cannot be allocated
*/
trie *new_trie (char current)
{
    trie *t = RedisModule_Calloc(1,sizeof(trie));
    
    if (t == NULL){
        error("Could not allocate memory for trie");
        return REDISMODULE_ERR;
    } 

    t->current = current;

    t->children = RedisModule_Calloc(256,sizeof(trie*));
    if (t->children == NULL){
        error("Could not allocate memory for t->children");
        return REDISMODULE_ERR;
    }

    t->is_word = 0;
    t->parent = NULL;

    return t;
}

/*
    Free an entire trie.
    Parameters:
     - t: A trie pointer
    
    Returns:
     - Always returns 0
*/
int trie_free(trie *t)
{
    assert(t != NULL);

    for (int i=0; i<256; i++ ){
        if (t->children[i] !=NULL)
            trie_free(t->children[i]);
    }

    RedisModule_Free(t);
    return 0;
}

/*
    Creates new node in trie.
    Parameters:
     - current: A char indicating the character of the node being added
     - t: A pointer to the trie where the node is to be added
    
    Returns:
     - 0 on success, 1 if an error occurs.
    Details: 
     - Set t->children[current] to be current
     - is_word for new node set to 0.
*/
int add_node(char current, trie *t)
{
    assert(t != NULL);

    unsigned c = (unsigned) current;

    if (t->children[c] == NULL)
        t->children[c] = new_trie(current);

    return 0;  
}

/*
    Inserts word into trie.
    Parameters:
     - word: An char array to be inserted into the given trie
     - t: A pointer to the given trie
    
    Returns:
     - 0 on success, 1 if error occurs.
    
    Details:
     - For each trie, check if entry of the next character exists in the children array:
          - If so, move into that node in the array
          - If not, add a new node and move into that node in the array
     - Then move on to the next character in string
     - Set the is_word of the last node to 1
*/
int insert_string(char *word, trie *t)
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

/* Searches for word in a trie. 
 *
 * Returns: 
 *  - 1 if word is found. 
 *  - 0 if word is not found at all.
 *  - -1 if word is found but end node's is_word is 0.
 */
 int trie_search(char* word, trie *t)
{
    int len;
    trie* curr;
    trie** next;

    len = strlen(word);
    curr = t;
    next = t->children;

    for (int i=0; i<len; i++) {
        int j = (int) word[i];
        curr = next[j];

        if (curr == NULL)
            return 0;

        next = next[j]->children;
    }

    if (curr->is_word == 1) 
        return 1;

    return -1;
}

// Still need delete_string

/* ========================= "trie" type commands (Redis wrapper functions) ======================= */

// insert function (add_node, insert_string and new_trie)
/* TRIE.INSERT key value */
int TrieInsert_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {


}

// contains function (trie_search)

// delete function

// function to show available words in trie?

/* ========================== "trie" type methods (Redis data saving and entry functions) ======================= */
