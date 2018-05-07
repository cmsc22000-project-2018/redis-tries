#include <criterion/criterion.h>
#include <stdlib.h>
#include <stdarg.h>
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

}

Test(trie, add_node_new)
{

}

Test(trie, insert_string)
{
    
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