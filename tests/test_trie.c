#include <criterion/criterion.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "trie.h"

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


    

