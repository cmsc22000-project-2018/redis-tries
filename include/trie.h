/*
 * An implementation of a trie data structure for storage of dynamic set of
 * plaintext words.
 */

#ifndef INCL_TRIE_H_
#define INCL_TRIE_H_

#define ALPHABET_SIZE 256

/* A self-referential trie implementation */
typedef struct {
    char current;
    trie_t *children[ALPHABET_SIZE];
    int is_word;
    /*bool is_word;*/
    struct trie_t *parent;
} trie_t;

int delete_string(char *word, trie_t *t);
/* bool delete_string(char *word, trie_t *);*/

#endif