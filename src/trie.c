#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "trie.h"

/* Returns 1/true if the trie in question is a leaf, else 0/false.
 */
/* bool is_leaf(trie_t *t) */
int is_leaf(trie_t *t)
{
    int i;
    for(i=0 ; i<ALPHABET_SIZE ; i++)
        if(t->children[i]!=NULL) return 0;
    return 1;
}

/* A naive implementation of trie_search that also takes a trie pointer input
 * and changes that pointer to the trie that contains the terminal constituent
 * of the input word. */
/* bool pointer_search(char *word, trie_t *t, trie_t *searched) */
int pointer_search(char *word, trie_t *t, trie_t *searched)
{
    int len = strlen(word);
    trie_t *traverse = t;
    trie_t *check;
    int i,j;
    
    searched = NULL;
    
    for(i=0 ; i<len ; i++) {
        for(j=0 ; j<ALPHABET_SIZE ; j++) {
            check = traverse;
            if(word[i]==traverse->children[j]->current)
                traverse = traverse->children[j];
            if(check==traverse) return 0;
        }
        if(i==(len-1)&&traverse->is_word==1) {
        /*if(i==(len-1)&&traverse->is_word==true)*/
            searched = traverse;
            return 1;
        }
        if(i==(len-1)&&traverse->is_word==1) {
        /*if(i==(len-1)&&traverse->is_word==true)*/
            return -1;
        }
    }
}
    
/* Deletes word in trie. Returns 1 if word is deleted and 0 if not. If word is
 * not in trie, the trie is unmodified and returns 0. If the word is entirely
 * unique, the word is deleted constituent tries are freed. If the word is the
 * prefix of another word in the trie, the final constituent trie is unmarked
 * as a word. If there is another word that prefixes the removed word, then all
 * constituent tries up until that word are removed and freed.
 */
int delete_string(char *word, trie_t *t)
/* bool delete_string(char *word, trie_t *) */
{
    trie_t *searched, *traverse_up, *temp;
    int present = pointer_search(word,t,searched);
    if(present <= 0) return 0; /*return false;*/
    
    searched = t;
    traverse_up = searched;
    
    if(is_leaf(searched)==1) {
        int len = strlen(word);
        traverse_up->is_word = 0;
        for(i=0 ; i<strlen ; i++) {
            if(traverse_up->is_word==0) {
                temp = traverse_up;
                traverse_up = traverse_up->parent;
                trie_free(temp);
            }
            return 1; /*return true;*/
        }
    } else {
        searched->is_word = 0;
        /*searched is_word = false;*/
        return 1; /*return true;*/
    }
}