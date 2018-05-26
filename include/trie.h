/* A trie data structure */

#ifndef INCLUDE_TRIE_H_
#define INCLUDE_TRIE_H_

#define IN_TRIE 1
#define NOT_IN_TRIE 0 
#define PARTIAL_IN_TRIE (-1)

typedef struct trie_t trie_t;
struct trie_t {
    char current; 
        // The first trie_t will be '/0' for any Trie.
    trie_t **children;
         // ALPHABET_SIZE is 256 for all possible characters.
    int is_word; 
        // if is_word is 1, indicates that this is the end of a word. Otherwise 0.
    trie_t *parent;
        // parent trie_t for traversing backwards
};

/*
    Creates and allocates memory for new trie_t.
    
    Parameters:
     - current: A char for the current character
    
    Returns:
     - A pointer to the trie, or NULL if a pointer 
       cannot be allocated
*/
trie_t *trie_new (char current);

/*
    Free an entire trie.

    Parameters:
     - t: A trie pointer
    
    Returns:
     - Always returns 0
*/
int trie_free(trie_t *t);


/*
    Creates new node in trie_t.

    Parameters:
     - current: A char indicating the character of the node being added
     - t: A pointer to the trie where the node is to be added
    
    Returns:
     - 0 on success, 1 if an error occurs.

    Details: 
     - Set t->children[current] to be current
     - is_word for new node set to 0.
*/
int trie_add_node(trie_t *t, char current);

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
int trie_insert_string(trie_t *t, char *word);

/*
    Delete word in trie.

    Parameters:
     - word: A char array to be deleted from the given trie
     - t: A pointer to the given trie

    Returns:
     - 1 if deleted

    Details: 
     - If word is not in trie
        trie is not modified.            
     - If word is completely unique (no other part of the word is part of another word) 
        delete the entire word.
     - If word is the prefix of another word in the trie
        unmark the leaf node.
     - If word is present in the trie, having at least one other word as a prefix, 
        delete all the nodes up to the prefix.
*/
int trie_delete_string(trie_t *t, char *word);



/* Searches for a word/prefix in a trie t. 
 *
 * Returns: 
 *  - pointer to the last letter in the word/prefix if word/prefix is found. 
 *  - NULL if word/prefix is not found.
 */
trie_t *trie_get_subtrie(trie_t *t, char* word);



/* Searches for a word/prefix in a trie t. 
 *
 * Returns: 
 *  - pointer to the last letter in the word/prefix if word/prefix is found. 
 *  - NULL if word/prefix is not found.
 */
trie_t *trie_search_end(char* word, trie_t *t);

/* Searches for word in a trie t. 
 *
 * Returns: 
 *  - IN_TRIE if word is found. 
 *  - NOT_IN_TRIE  if word is not found at all.
 *  - PARTIAL_IN_TRIE if word is found but end node's is_word is 0.
 */
int trie_search(trie_t *t, char *word);

/*
    Count the number of different possible endings of a given prefix in a trie
    
    Parameters:
     - pre: a string of the prefix converned
     - t: a trie pointer

    Returns:
     - an integer of the number of endings if the prefix exists in the trie
     - 0 if the prefix does not exist in the trie
*/
int trie_count_completion(trie_t *t, char *pre);


#endif

