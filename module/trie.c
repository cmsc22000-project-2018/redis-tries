/* This file implements a new module native data type called "TRIE".
 * The data structure is based on the idea of a trie (aka a prefix 
 * tree) and largely works the same way.
 */
#define IN_TRIE 1
#define NOT_IN_TRIE 0 
#define PARTIAL_IN_TRIE (-1)

#include "../redismodule.h"
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
};

/*
    Creates and allocates memory for new trie.
    
    Parameters:
     - current: A char for the current character
    
    Returns:
     - A pointer to the trie, or REDISMODULE_ERR if a pointer 
       cannot be allocated
*/
struct trie *trie_new (char current)
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
                 * and all of a trie's children are RedisModule_Calloc'ed 
                 */
                trie_free(t->children[i]); 
        }
        /* Used because the data structures are 
         * originally RedisModule_Calloc'ed 
         */
        RedisModule_Free(t); 
    }
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
int trie_add_node(struct trie *t, char current)
{
    assert(t != NULL);
    /* Current is casted because the compiler 
    will throw unnecessary warnings otherwise */
    unsigned c = current; 
    if (t->children[c] == NULL)
        t->children[c] = trie_new(current);
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
        char curr = *word;
        int rc = trie_add_node(t, curr);
        if (rc != 0) {
            fprintf(stderr, "Fail to add node");
            return 1;
        }
        word++;
        return trie_insert_string(t->children[curr], word);
    }
}

/* Searches for word in a trie. 
 *
 * Returns: 
 *  - 1 if word is found. 
 *  - 0 if word is not found at all.
 *  - -1 if word is found but end node's is_word is 0.
 */
int trie_search(struct trie *t, char* word)
{
    int len;
    struct trie* curr;
    struct trie** next;
    len = strlen(word);
    curr = t;
    next = t->children;
    for (int i = 0; i < len; i++) {
        int j = (int) word[i];
        curr = next[j];
        if (curr == NULL)
            return NOT_IN_TRIE;
        next = next[j]->children;
    }
    if (curr->is_word == 1) 
        return IN_TRIE;
    return PARTIAL_IN_TRIE;
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
    	return RedisModule_ReplyWithError(ctx, "ERR invalid value: must \
                be a string");
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
    	return RedisModule_ReplyWithError(ctx, "ERR invalid key: not an \
                existing trie");
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
        RedisModule_ReplyWithSimpleString(ctx, "The trie does not contain \
                the word.");
    } else {
        RedisModule_ReplyWithSimpleString(ctx, "The trie contains it as a \
                prefix but not as a word.");  
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
