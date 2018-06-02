/* This file implements a new module native data type called "TRIE".
 * The data structure is based on the idea of a trie (aka a prefix 
 * tree) and largely works the same way.
 */
#define IN_TRIE 1
#define NOT_IN_TRIE 0 
#define PARTIAL_IN_TRIE (-1)

#include "redismodule.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>


static RedisModuleType *trie;

/* ===== Internal data structure (Bare bones functions)  ====== */

struct trie {
    // The first trie_t will be '/0' for any Trie.
    char current;
    
    // ALPHABET_SIZE is 256 for all possible characters.     
    struct trie **children;
    
    // if is_word is 1, indicates that this is the end of a word. Otherwise 0.
    int is_word; 

    // parent trie for traversing backwards
    struct trie *parent;
    
    // list of characters that are contained in the node and its children
    char *charlist;
};

/*
    Creates and allocates memory for new trie.
    
    Parameters:
     - current: A char for the current character
    
    Returns:
     - A pointer to the trie, or REDISMODULE_ERR if a pointer 
       cannot be allocated
*/
struct trie *trie_new(char current)
{
    struct trie *t = RedisModule_Calloc(1, sizeof(struct trie));

    if (t == NULL) {
        fprintf(stderr, "Could not allocate memory for trie\n");
        return NULL;
    } 

    t->current = current;
    t->children = RedisModule_Calloc(256, sizeof(struct trie *));

    if (t->children == NULL) {
        fprintf(stderr, "Could not allocate memory for t->children\n");
        return NULL;
    }

    t->is_word = 0;
    t->parent = NULL;
    t->charlist = RedisModule_Calloc(256, sizeof(char));

    return t;
}

/*
    Free an entire trie.

    Parameters:
     - t: A trie pointer
    
    Returns:
     - Always returns 0
*/
int trie_free(struct trie *t)
{
    if (t != NULL) {
        for (int i = 0; i < 256; i++) {
            if (t->children[i] != NULL)
                /* Called recursively because the entire trie 
                   and all of a trie's children are RedisModule_Calloc'ed 
                 */
                trie_free(t->children[i]); 
        }
    }

    RedisModule_Free(t->charlist);
    /* Used because the data structures are 
       originally RedisModule_Calloc'ed 
     */
    RedisModule_Free(t); 
    return 0;
}

/*
    Creates new node in trie.

    Parameters:
     - t: A pointer to the trie where the node is to be added
     - current: A char indicating the character of the node being added
    
    Returns:
     - 0 on success, 1 if an error occurs.
    Details: 
     - Set t->children[current] to be current
     - is_word for new node set to 0.
*/
int trie_add_node(struct trie *t, char current)
{
    assert(t != NULL);

    /* Current is casted because the compiler 
    will throw unnecessary warnings otherwise */
    unsigned int c = (int)current; 

    if (t->children[c] == NULL)
        t->children[c] = trie_new(current);

    return 0;  
}

/*
    Inserts word into trie.
    Parameters:
     - t: A pointer to the given trie
     - word: A char array to be inserted into the given trie
    
    Returns:
     - 0 on success, 1 if error occurs.
    
    Details:
     - For each trie, check if entry of the next character 
       exists in the children array:
          - If so, move into that node in the array
          - If not, add a new node and move into that node in the array
     - Then move on to the next character in string
     - Set the is_word of the last node to 1
*/
int trie_insert_string(struct trie *t, char *word)
{
    assert(t != NULL);

    if (*word == '\0') {
        t->is_word = 1;
        return 0;
    } else {
        int len = strlen(word);
        int index;
        for (int i = 0; i < len; i++) {
            index = (int)word[i];
            t->charlist[index] = word[i];
        }

        char curr = word[0];
	    index = (int)curr;

        int rc = trie_add_node(t, curr);
        if (rc != 0) {
            fprintf(stderr, "Fail to add node");
            return 1;
        }

        word++;
        return trie_insert_string(t->children[index], word);
    }
}

/* 
    Searches for a word/prefix in a trie. 
 
    Parameters:
     - t: A pointer to the given trie
     - word: A char array in which the end pointer is desired

    Returns: 
     - pointer to the last letter in the word/prefix if word/prefix is found. 
     - NULL if word/prefix is not found.
 */
struct trie *trie_get_subtrie(struct trie *t, char* word)
{
    int len;
    struct trie* curr;
    struct trie** next;

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

/* 
    Searches for word in a trie. 
 
    Parameters:
     - t: A pointer to the given trie
     - word: A char array that will be searched for in the trie 

    Returns: 
     - IN_TRIE if word is found. 
     - NOT_IN_TRIE  if word is not found at all.
     - PARTIAL_IN_TRIE if word is found but end node's is_word is 0.
 */
int trie_search(struct trie *t, char* word)
{
    struct trie *end = trie_get_subtrie(t, word);

    if (end == NULL)
        return NOT_IN_TRIE;

    if (end->is_word == 1) 
        return IN_TRIE;

    return PARTIAL_IN_TRIE;
}

/* Helper function for trie_count_completion */
int trie_count_completion_recursive(struct trie *t)
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

/*
    Count the number of different possible endings of a given prefix in a trie
    
    Parameters:
     - pre: a string of the prefix converned
     - t: a trie pointer
    Returns:
     - an integer of the number of endings if the prefix exists in the trie
     - 0 if the prefix does not exist in the trie
*/
int trie_count_completion(struct trie *t, char *pre)
{
    struct trie *end = trie_get_subtrie(t, pre);

    if (end == NULL)
        return 0;

    return trie_count_completion_recursive(end);
}

/* ===== "trie" type commands (Redis wrapper functions) ===== */

/* TRIE.INSERT key value */
int TrieInsert_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, 
        int argc) {
    RedisModule_AutoMemory(ctx); /* Use automatic memory management. */
    
    if (argc != 3) return RedisModule_WrongArity(ctx);

	RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1],
        REDISMODULE_READ|REDISMODULE_WRITE);
    int type = RedisModule_KeyType(key);
	if (type != REDISMODULE_KEYTYPE_EMPTY &&
        RedisModule_ModuleTypeGetType(key) != trie)
    {
        return RedisModule_ReplyWithError(ctx, REDISMODULE_ERRORMSG_WRONGTYPE);
    }
    size_t dummy;
    char *temp = RedisModule_StringPtrLen(argv[2], &dummy);
    char *empty = "";
    if (temp == NULL || strcmp(temp, empty) == 0) {
    	return RedisModule_ReplyWithError(ctx, "ERR invalid value: must be a string");
    } 

    struct trie *t;
    /* Create an empty value object if the key is currently empty. */
    if (type == REDISMODULE_KEYTYPE_EMPTY) {
    	t = trie_new('\0');
    	RedisModule_ModuleTypeSetValue(key, trie, t);
    } else {
        t = RedisModule_ModuleTypeGetValue(key);
    }

    /* Insert the new string. */
    trie_insert_string(t, temp);
    
	RedisModule_ReplyWithSimpleString(ctx, "Success");    
	RedisModule_ReplicateVerbatim(ctx);
    return REDISMODULE_OK;
}

/* TRIE.CONTAINS key value */
int TrieContains_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, 
        int argc) {
    RedisModule_AutoMemory(ctx); /* Use automatic memory management. */

    if (argc != 3) return RedisModule_WrongArity(ctx);

    RedisModuleKey *key = RedisModule_OpenKey(ctx, argv[1],
        REDISMODULE_READ|REDISMODULE_WRITE);
    int type = RedisModule_KeyType(key);
    if (type == REDISMODULE_KEYTYPE_EMPTY) {
    	return RedisModule_ReplyWithError(ctx, "ERR invalid key: not an existing trie");
    }
    else if (RedisModule_ModuleTypeGetType(key) != trie)
    {
        return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
    }
    size_t dummy;
    char *temp = RedisModule_StringPtrLen(argv[2], &dummy);

    struct trie *t;
    t = RedisModule_ModuleTypeGetValue(key);

    /* Check for the string. */
    int c = trie_search(t, temp);
    
    if (c == 1) {
        RedisModule_ReplyWithSimpleString(ctx, "The trie contains the word.");
    } else if (c == 0) {
        RedisModule_ReplyWithSimpleString(ctx, "The trie does not contain the word.");
    } else {
        RedisModule_ReplyWithSimpleString(ctx, "The trie contains it as a prefix but not as a word.");  
    }       
    RedisModule_ReplicateVerbatim(ctx);
    return REDISMODULE_OK;
}

/* ===== "trie" type methods (Redis data saving and entry functions) ===== */

/* This function must be present on each Redis module. It is used in order to
 * register the commands into the Redis server. 
 */

int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    REDISMODULE_NOT_USED(argv);
    REDISMODULE_NOT_USED(argc);

    if (RedisModule_Init(ctx, "trie123az", 1, REDISMODULE_APIVER_1)
        == REDISMODULE_ERR) 
        return REDISMODULE_ERR;

    RedisModuleTypeMethods tm = {
        .version = REDISMODULE_TYPE_METHOD_VERSION,
        .rdb_load = NULL,
        .rdb_save = NULL,
        .aof_rewrite = NULL,
        .mem_usage = NULL,
        .free = NULL,
        .digest = NULL
    };

    trie = RedisModule_CreateDataType(ctx, "trie123az", 0, &tm);
    if (trie == NULL) 
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "trie.insert",
        TrieInsert_RedisCommand, "write deny-oom", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx, "trie.contains",
        TrieContains_RedisCommand, "readonly", 1, 1, 1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    return REDISMODULE_OK;
}
