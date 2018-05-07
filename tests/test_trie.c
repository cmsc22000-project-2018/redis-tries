#include <criterion/criterion.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "trie.h"

Test(trie,is_leaf)
{
    /* rudimentary implementation without other fxns, will be modified */
    trie_t *t1, *t2;
    
    t1 = new_trie('o');
    t1->children[110] = new_trie('n');
    t2 = t1->children[110];
    t2->parent = t1;
    t2->is_word = 1;
    
    int rc1 = is_leaf(t1);
    int rc2 = is_leaf(t2);
    
    cr_assert_eq(rc1,0,"is_leaf returned 1 when expected result was 0");
    cr_assert_eq(rc2,1,"is_leaf returned 0 when expected result was 1");
}

Test(trie,pointer_search)
{
    trie_t *t1, *t2, *t_search;
    
    t1 = new_trie('o');
    t1->children[110] = new_trie('n');
    t2 = t1->children[110];
    t2->parent = t1;
    t2->is_word = 1;
    
    int rc1 = pointer_search("on",t1,t_search);
    int rc2 = pointer_search("yes",t1,t_search);
    
    cr_assert_eq(rc1,1,"pointer_search returned 0 when expected result was 1");
    cr_assert_eq(t_search,t2,"pointer_search did not modify input pointer to correct address");
    
    cr_assert_eq(rc2,0,"pointer_search returned 1 when expected result was 0");
}

Test(trie,delete_string_unique)
{
    trie_t *t1, *t2;
    
    t1 = new_trie('\0');
    t1->children[110] = new_trie('n');
    t2 = t1->children[110];
    t2->parent = t1;
    t2->is_word = 1;
    
    int rc = delete_string("n",t1);
    int rc_l = is_leaf(t1);
    
    cr_assert_eq(rc,1,"delete_string failed to remove string from trie");
    cr_assert_eq(rc_l,1,"delete_string improperly removed string from trie");
}

Test(trie,delete_string_not_present)
{
    trie_t *t1, *t2;
    
    t1 = new_trie('\0');
    t1->children[110] = new_trie('n');
    t2 = t1->children[110];
    t2->parent = t1;
    t2->is_word = 1;
    
    int rc = delete_string("o",t1);
    int rc_l = is_leaf(t1);
    
    cr_assert_eq(rc,0,"delete_string returned 1 when expected value was 0");
    cr_assert_eq(rc_l,0,"delete_string removed child trie when string was not in parent");
}

Test(trie,delete_string_prefix)
{
    trie_t *t1, *t2, *t3;
    
    t1 = new_trie('\0');
    t1->children[110] = new_trie('n');
    t2 = t1->children[110];
    t2->parent = t1;
    t2->is_word = 1;
    t3 = t2->children[110] = new_trie('n');
    t3->parent = t2;
    t3->is_word = 1;
    
    
    int rc = delete_string("n",t1);
    int rc_iw = t2->is_word;
    
    cr_assert_eq(rc,1,"delete_string failed to remove prefix string");
    cr_assert_eq(rc_iw,0,"delete_string improperly removed prefix string from trie");
}

Test(trie,delete_string_prefixed)
{
    trie_t *t1, *t2, *t3;
    
    t1 = new_trie('\0');
    t1->children[110] = new_trie('n');
    t2 = t1->children[110];
    t2->parent = t1;
    t2->is_word = 1;
    t3 = t2->children[110] = new_trie('n');
    t3->parent = t2;
    t3->is_word = 1;
    
    int rc = delete_string("nn",t1);
    int rc_l = is_leaf(t2);
    
    cr_assert_eq(rc,1,"delete_string failed to remove prefixed string from trie");
    cr_assert_eq(rc_l,1,"delete_string improperly removed prefixed string from trie");
}