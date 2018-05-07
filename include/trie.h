/*
 * Searches for word in a trie t. 
 *
 * Returns: 
 *  - 1 if word is found. 
 *  - 0 if word is not found at all.
 *  - -1 if word is found but end node's is_word is 0.
 */
int trie_search(char *word, trie_t *t);

