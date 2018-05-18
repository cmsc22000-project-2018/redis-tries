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