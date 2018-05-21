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
#include "stringbuilder.h"


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

/* TRIE.INSERT key value */
int TrieInsert_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx); /* Use automatic memory management. */
    
    if (argc != 3) return RedisModule_WrongArity(ctx);

	RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
        REDISMODULE_READ|REDISMODULE_WRITE);
    int type = RedisModule_KeyType(key);
	if (type != REDISMODULE_KEYTYPE_EMPTY &&
        RedisModule_ModuleTypeGetType(key) != trie)
    {
        return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
    }
    size_t *dummy; // Do we need to free this?
    char *temp = RedisModule_StringPtrLen(argv[2], dummy); // Same question here?
    char *empty = "";
    if (temp == NULL || strcmp(temp, empty) == 0) {
    	return RedisModule_ReplyWithError(ctx,"ERR invalid value: must be a string");
    } 

    trie *t;
    /* Create an empty value object if the key is currently empty. */
    if (type == REDISMODULE_KEYTYPE_EMPTY) {
    	t = new_trie('\0');
    	RedisModule_ModuleTypeSetValue(key,trie,t);
    } else {
        t = RedisModule_ModuleTypeGetValue(key);
    }

    /* Insert the new string. */
    insert_string(temp, t);
    
	RedisModule_ReplyWithSimpleString(ctx, "Success");    
	RedisModule_ReplicateVerbatim(ctx);
    return REDISMODULE_OK;
}

/* TRIE.CONTAINS key value */
int TrieContains_RedisCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    RedisModule_AutoMemory(ctx); /* Use automatic memory management. */

    if (argc != 3) return RedisModule_WrongArity(ctx);

    RedisModuleKey *key = RedisModule_OpenKey(ctx,argv[1],
        REDISMODULE_READ|REDISMODULE_WRITE);
    int type = RedisModule_KeyType(key);
    if (type == REDISMODULE_KEYTYPE_EMPTY) {
    	return RedisModule_ReplyWithError(ctx,"ERR invalid key: not an existing trie");
    }
    else if (RedisModule_ModuleTypeGetType(key) != trie)
    {
        return RedisModule_ReplyWithError(ctx,REDISMODULE_ERRORMSG_WRONGTYPE);
    }
    size_t *dummy; // Do we need to free this?
    char *temp = RedisModule_StringPtrLen(argv[2], dummy); // Same question here?
    char *empty = "";
    if (temp == NULL || strcmp(temp, empty) == 0) {
        return RedisModule_ReplyWithError(ctx,"ERR invalid value: must be a string");
    } 

    trie *t;
    t = RedisModule_ModuleTypeGetValue(key);

    /* Check for the string. */
    int c = trie_search(temp, t);
    
    if (c == 1) {
        RedisModule_ReplyWithSimpleString(ctx, "The trie contains %s", temp);
    } else if (c == 0) {
        RedisModule_ReplyWithSimpleString(ctx, "The trie does not contain %s", temp);
    } else {
        RedisModule_ReplyWithSimpleString(ctx, "The trie contains the prefix %s, but not the word", temp);
    }       
    RedisModule_ReplicateVerbatim(ctx);
    return REDISMODULE_OK;
}

// delete function

// function to show available words in trie

/* ========================== "trie" type methods (Redis data saving and entry functions) ======================= */

/* This function must be present on each Redis module. It is used in order to
 * register the commands into the Redis server. */
/*

void TrieRDBSave(RedisModuleIO *io, void *ptr) {
    trie *tr = ptr;
    //RedisModule_SaveUnsigned(io,da->count);
    for (int i=0; i<256; i++ ){
        if (t->children[i] !=NULL)
            trie_free(t->children[i]);
    }
    for (size_t j = 0; j < da->count; j++)
        RedisModule_SaveDouble(io,da->values[j]);
}


void TrieType_RdbSave(RedisModuleIO *rdb, void *value) {
  TrieType_GenericSave(rdb, (Trie *)value, 1);
}

void TrieType_GenericSave(RedisModuleIO *rdb, Trie *tree, int savePayloads) {
  //RedisModule_SaveUnsigned(rdb, tree->size);
  RedisModuleCtx *ctx = RedisModule_GetContextFromIO(rdb);
  RedisModule_Log(ctx, "notice", "Trie: saving %zd nodes.", tree->size);
  int count = 0;
  if (tree->root) {
    TrieIterator *it = TrieNode_Iterate(tree->root, NULL, NULL, NULL);
    rune *rstr;
    t_len len;
    float score;
    RSPayload payload = {.data = NULL, .len = 0};

    while (TrieIterator_Next(it, &rstr, &len, &payload, &score, NULL)) {
      size_t slen = 0;
      char *s = runesToStr(rstr, len, &slen);
      RedisModule_SaveStringBuffer(rdb, s, slen + 1);
      RedisModule_SaveDouble(rdb, (double)score);

      if (savePayloads) {
        // save an extra space for the null terminator to make the payload null terminated on load
        if (payload.data != NULL && payload.len > 0) {
          RedisModule_SaveStringBuffer(rdb, payload.data, payload.len + 1);
        } else {
          // If there's no payload - we save an empty string
          RedisModule_SaveStringBuffer(rdb, "", 1);
        }
      }
      // TODO: Save a marker for empty payload!
      free(s);
      count++;
    }
    if (count != tree->size) {
      RedisModule_Log(ctx, "warning", "Trie: saving %zd nodes actually iterated only %zd nodes",
                      tree->size, count);
    }
    TrieIterator_Free(it);
  }
}

*/



int RedisModule_OnLoad(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    REDISMODULE_NOT_USED(argv);
    REDISMODULE_NOT_USED(argc);

    if (RedisModule_Init(ctx,"trie123az",1,REDISMODULE_APIVER_1)
        == REDISMODULE_ERR) return REDISMODULE_ERR;

    RedisModuleTypeMethods tm = {
        .version = REDISMODULE_TYPE_METHOD_VERSION,
        .rdb_load = TrieRdbLoad,
        .rdb_save = TrieRdbSave,
        .aof_rewrite = TrieAofRewrite,
        .mem_usage = TrieMemUsage,
        .free = TrieFree,
        .digest = TrieDigest
    };

    trie = RedisModule_CreateDataType(ctx,"trie123az",0,&tm);
    if (trie == NULL) return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"trie.insert",
        TrieInsert_RedisCommand,"write deny-oom",1,1,1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    if (RedisModule_CreateCommand(ctx,"trie.contains",
        TrieContains_RedisCommand,"readonly",1,1,1) == REDISMODULE_ERR)
        return REDISMODULE_ERR;

    return REDISMODULE_OK;
}
