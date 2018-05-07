#include <criterion/criterion.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "trie.h"

Test(trie, new)
{
    trie_t *t;

    t = new_trie('c');

    cr_assert_not_null(t, "new_trie() failed to allocate memory");
    cr_assert_eq(t->current, 'c', "new_trie() failed to set current");
    cr_assert_eq(t->is_word, 0, "new_trie() failed to set is_word");
}


Test(trie, add_node_exists)
{
    char n = 'n';
    trie_t *t = new_trie('\0');
    int rc = add_node(n,t);

    cr_assert_eq(rc,0,"add_node failed");
    cr_assert_not_null(t->children[(unsigned)n], "add_node failed to allocate new entry");
    cr_assert_eq(t->children[(unsigned)n]->current,n, "add_node failed to set is_word for new trie");
    cr_assert_eq(t->children[(unsigned)n]->is_word,0, "add_node failed to set is_word for new trie");
}

Test(trie, add_node_new)
{
    char n = 'n';
    trie_t *t = new_trie('\0');

    int fc = add_node(n,t);
    cr_assert_eq(fc,0,"add_node failed");

    int rc = add_node(n,t);

    cr_assert_eq(rc,0,"add_node failed");
    cr_assert_not_null(t->children[(unsigned)n], "add_node failed to allocate new entry");
    cr_assert_eq(t->children[(unsigned)n]->current,n, "add_node failed to set is_word for new trie");
    cr_assert_eq(t->children[(unsigned)n]->is_word,0, "add_node failed to set is_word for new trie");
}

Test(trie, insert_string)
{
    char* s1 = strdup("an");
    char* s2 = strdup("anti");
    char* s3 = strdup("ants");
    trie_t *t = new_trie('\0');

    int r1 = insert_string(s1,t);
    cr_assert_eq(r1,0,"insert_string failed");   
    cr_assert_not_null(t->children['a'], "add_node failed to allocate new entry");
    cr_assert_eq(t->children['a']->is_word,0, "add_node failed to set is_word for new trie");
    cr_assert_not_null(t->children['a']->children['n'] , "add_node failed to allocate new entry");
    cr_assert_eq(t->children['a']->children['n']->is_word,1, "insert_string failed to set is_word for end character");

    int r2 = insert_string(s2,t);
    cr_assert_eq(r2,0,"insert_string failed");
    cr_assert_eq(t->children['a']->children['n']->is_word,1, "insert_string failed to set is_word for end character");
    cr_assert_eq(t->children['a']->children['n']->children['t']->children['i']->is_word,1, "insert_string failed to set is_word for end character");

    int r3 = insert_string(s3,t);
    cr_assert_eq(r3,0,"insert_string failed");
    cr_assert_eq(t->children['a']->children['n']->children['t']->is_word,0, "insert_string failed to set is_word for middle character");
    cr_assert_eq(t->children['a']->children['n']->children['t']->children['s']->is_word,1, "insert_string failed to set is_word for end character");


}

Test(trie, free)
{
    trie_t *t;
    int rc;

    t = new_trie('\0');

    cr_assert_not_null(t, "new_trie() failed to allocate memory");

    rc = trie_free(t);

    cr_assert_eq(rc, 0, "trie_free failed");

}

/* Checks that searching an empty trie returns 0 */
Test(trie, empty_search)
{
    trie_t *t;
    int found;
    
    t = new_trie('\0');
    cr_assert_not_null(t, "new_trie() failed");

    found = trie_search("b", t);

    cr_assert_eq(found, 0, "trie_search() does not return 0 with an empty trie");
}

/* Checks a  trie with only one word */
Test(trie, singleton_search)
{
    trie_t *t;
    int inserted, found;

    t = new_trie('\0');
    cr_assert_not_null(t, "new_trie() failed");

    inserted = insert_string("CS220", t);
    cr_assert_eq(inserted, 0, "insert_string() failed");

    found = trie_search("CS220", t); // Checks if inserted word can be found 
    cr_assert_eq(found, 1, "trie_search() returns %d instead of 1 with inserted word", found );

    found = trie_search("CS", t); // Checks prefix of inserted word
    cr_assert_eq(found, -1, "trie_search() returns %d instead of -1
            with prefix of inserted word", found);

    found = trie_search("CS22000", t); // Checks extension of inserted word
    cr_assert_eq(found, 0, "trie_search() returns %d instead of 0
            with extension of inserted word", found);

    found = trie_search("Computer", t); // Checks different word
    cr_assert_eq(found, 0, "trie_search() returns %d instead of 0
            with word different from inserted word", found);
}


