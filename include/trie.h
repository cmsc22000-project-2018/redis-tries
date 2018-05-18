#define IN_TRIE 1
#define NOT_IN_TRIE 0 
#define PARTIAL_IN_TRIE (-1)

/*
 * Searches for word in a trie t. 
 *
 * Returns: 
 *  - IN_TRIE if word is found. 
 *  - NOT_IN_TRIE  if word is not found at all.
 *  - PARTIAL_IN_TRIE if word is found but end node's is_word is 0.
 */
int trie_search(char *word, trie_t *t);

