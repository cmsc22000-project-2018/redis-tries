#include <criterion/criterion.h>
#include <stdlib.h>
#include <stdbool.h>
#include "suggestion.h"

// Test has_children for success
Test(suggestion, has_children_s0) {
    dict_t *d = dict_new();
    dict_add(d, "abc");

    cr_assert_eq(has_children(d, "ab"), EXIT_SUCCESS, "has_children failed");
}

// Test has_children for failure
Test(suggestion, has_children_f0) {
    dict_t *d = dict_new();
    dict_add(d, "abc");

    cr_assert_eq(has_children(d, "ac"), EXIT_FAILURE, "has_children failed");
}

/*************   These functions individually test values for cmp_match   *************/

void test_cmp_match(char *m1_str, char *m2_str, int m1_val, int m2_val, int expected) {
    match_t *m1 = malloc(sizeof(match_t));
    match_t *m2 = malloc(sizeof(match_t));

    m1->str = m1_str;
    m2->str = m2_str;

    m1->edits_left = 0;
    m2->edits_left = 1;

    int rc = cmp_match((void*)m1, (void*)m2);

    cr_assert_eq(rc, expected, "cmp_match on %s, %d and %s, %d returned %d instead of %d",
                 m1_str, m1_val, m2_str, m2_val, rc, expected);
}

Test(suggestion, cmp_match_0) {
    
    test_cmp_match("aa", "ab", 0, 0, 1);
}

Test(suggestion, cmp_match_1) {
    test_cmp_match("ab", "aa", 0, 0, -1);
}

Test(suggestion, cmp_match_2) {
    test_cmp_match("aa", "aa", 1, 0, -1);
}

Test(suggestion, cmp_match_1) {
    test_cmp_match("aa", "aa", 0, 1, 1);
}

Test(suggestion, cmp_match_null0) {
    match_t *m1 = malloc(sizeof(match_t));

    m1->str = "hi";

    m1->edits_left = 0;

    int rc = cmp_match((void*)m1, (void*)NULL);

    cr_assert_eq(rc, 0)
}

Test(suggestion, cmp_match_null0) {
    match_t *m1 = malloc(sizeof(match_t));

    m1->str = "hi";

    m1->edits_left = 0;

    int rc = cmp_match((void*)NULL, (void*)m1);
}

/*************   These functions individually test helpers of suggestions()   *************/

// Test a simple move_on
Test(suggestion, move_s0) {
    zset_t *set = zset_new("set");
    dict_t *d = dict_new();

    dict_add(d, "f");

    int rc = suggestions(set, d, "", "f", 1);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(zset_rank(set, "f"), 0, "move_on failed");

    // cleanup
    zset_remrangebyrank(set, 0, -1);
}

// Test a full move_on
Test(suggestion, move_s1) {
    zset_t *set = zset_new("set");
    dict_t *d = dict_new();

    dict_add(d, "vdf894jkls");

    int rc = suggestions(set, d, "", "vdf894jkls", 1);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(zset_rank(set, "vdf894jkls"), 0, "move_on failed");

    // cleanup
    zset_remrangebyrank(set, 0, -1);
}

// Test a simple delete
Test(suggestion, delete_s0) {
    zset_t *set = zset_new("set");
    dict_t *d = dict_new();

    int rc = suggestions(set, d, "", "k", 1);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(zset_rank(set, "cds8Dfk"), 0, "try_delete failed");

    // cleanup
    zset_remrangebyrank(set, 0, -1);
}

// Test a double delete
Test(suggestion, delete_s1) {
    zset_t *set = zset_new("set");
    dict_t *d = dict_new();

    dict_add(d, "cds8Dfk");

    int rc = suggestions(set, d, "", "cdisc8Dfk", 2);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(zset_rank(set, "cds8Dfk"), 0, "try_delete failed");

    // cleanup
    zset_remrangebyrank(set, 0, -1);
}

// Test a simple replace
Test(suggestion, replace_s0) {
    zset_t *set = zset_new("set");
    dict_t *d = dict_new();

    dict_add(d, "s");

    int rc = suggestions(set, d, "", "i", 1);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(zset_rank(set, "s"), 0, "try_replace failed");

    // cleanup
    zset_remrangebyrank(set, 0, -1);
}

// Test a double replace
Test(suggestion, replace_s1) {
    zset_t *set = zset_new("set");
    dict_t *d = dict_new();

    dict_add(d, "vT.k=(?");

    int rc = suggestions(set, d, "", "vTik=8?", 2);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(zset_rank(set, "vjikd8-"), 0, "try_replace failed");

    // cleanup
    zset_remrangebyrank(set, 0, -1);
}

// Test a simple swap
Test(suggestion, swap_s0) {
    zset_t *set = zset_new("set");
    dict_t *d = dict_new();

    dict_add(d, "ap");

    int rc = suggestions(set, d, "p", "a", 1);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(zset_rank(set, "ap"), 0, "try_swap failed");

    // cleanup
    zset_remrangebyrank(set, 0, -1);
}

// Test a double swap 
Test(suggestion, swap_s1) {
    zset_t *set = zset_new("set");
    dict_t *d = dict_new();

    dict_add(d, "4uiwifnw");

    int rc = suggestions(set, d, "", "4uiwnfiw", 2);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(zset_rank(set, "4uiwifnw"), 0, "try_swap failed");

    // cleanup
    zset_remrangebyrank(set, 0, -1);
}

// Test a simple insert
Test(suggestion, insert_s0) {
    zset_t *set = zset_new("set");
    dict_t *d = dict_new();

    dict_add(d, "+");

    int rc = suggestions(set, d, "", "", 1);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(zset_rank(set, "+"), 0, "try_insert failed");

    // cleanup
    zset_remrangebyrank(set, 0, -1);
}

// Test a double insert
Test(suggestion, insert_s1) {
    zset_t *set = zset_new("set");
    dict_t *d = dict_new();

    dict_add(d, "fnjk43?>.f@");

    int rc = suggestions(set, d, "", "fnk43?>.f", 2);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(zset_rank(set, "fnjk43?>.f@"), 0, "try_insert failed");

    // cleanup
    zset_remrangebyrank(set, 0, -1);
}

/*************   These functions test suggestions() as a whole   *************/

// Test regular suggestion generation 1
Test(suggestion, suggestions_s0) {
    zset_t *set = zset_new("set");
    dict_t *d = dict_new();

    dict_add(d, "cops");
    dict_add(d, "8op");
    dict_add(d, "op");
    dict_add(d, "ocp");

    int rc = suggestions(set, d, "", "cop", 1);
    cr_assert_eq(0, rc, "suggestions() failed");

    char **results = zset_revrange(set, 0, 3);

    cr_assert_eq(0, strncmp(results[0], "op", MAXLEN), 
                "suggestions() first result incorrect");
    cr_assert_eq(0, strncmp(results[1], "ocp", MAXLEN), 
                "suggestions() second result incorrect");
    cr_assert_eq(0, strncmp(results[2], "cops", MAXLEN), 
                "suggestions() third result incorrect");
    cr_assert_eq(0, strncmp(results[3], "8op", MAXLEN), 
                "suggestions() fourth result incorrect");

    zset_remrangebyrank(set, 0, -1);
}

// Test regular suggestion generation 1
Test(suggestion, suggestions_s1) {
    zset_t *set = zset_new("set");
    dict_t *d = dict_new();

    dict_add(d, "stall");
    dict_add(d, "satll");
    dict_add(d, "drall"); // success but not in list
    dict_add(d, "malt");
    dict_add(d, "malfkt"); // not a return
    dict_add(d, "fn|d7~3"); // not a return

    int rc = suggestions(set, d, "", "small", 2);
    cr_assert_eq(0, rc, "suggestions() failed");

    char **results = zset_revrange(set, 0, 2);

    cr_assert_eq(0, strncmp(results[0], "stall", MAXLEN), 
                "suggestions() first result incorrect");
    cr_assert_eq(0, strncmp(results[1], "satll", MAXLEN), 
                "suggestions() second result incorrect");
    cr_assert_eq(0, strncmp(results[2], "malt", MAXLEN), 
                "suggestions() third result incorrect");

    zset_remrangebyrank(set, 0, -1);
}

/*************   These functions test the easy input functions   *************/

// Test for suggestion_set_new function
Test(suggestion, suggestion_set_new_s0) {
    dict_t *d = dict_new();

    dict_add(d, "only");
    dict_add(d, "onety");
    dict_add(d, "none");
    dict_add(d, "fjmsdk8");
    dict_add(d, "o=9");

    zset_t *set = suggestion_set_new(d, "one", 2);

    char **results = zset_revrange(set, 0, 3);

    cr_assert_eq(0, strncmp(results[0], "none", MAXLEN), 
                "suggestion_set_new() first result incorrect");
    cr_assert_eq(0, strncmp(results[1], "only", MAXLEN), 
                "suggestion_set_new() second result incorrect");
    cr_assert_eq(0, strncmp(results[2], "onety", MAXLEN), 
                "suggestion_set_new() third result incorrect");
    cr_assert_eq(0, strncmp(results[3], "o=9", MAXLEN), 
                "suggestion_set_new() fourth result incorrect");    

    zset_remrangebyrank(set, 0, -1);
}

// Test for suggestion_set_first_n function with regular input
Test(suggestion, suggestion_set_first_n_s0) {
    zset_t *set = zset_new("set");

    zset_add(set, "a", 1);
    zset_add(set, "b", 2);
    zset_add(set, "c", 3);

    char **results = suggestion_set_first_n(set, 2);

    cr_assert_eq(0, strncmp(results[0], "c", MAXLEN), 
                "suggestion_set_first_n() first result incorrect");
    cr_assert_eq(0, strncmp(results[1], "b", MAXLEN), 
                "suggestion_set_first_n() second result incorrect");

    zset_remrangebyrank(set, 0, -1);
}

// Test for suggestion_set_first_n function with not enough matches
Test(suggestion, suggestion_set_first_n_f0) {
    zset_t *set = zset_new("set");

    zset_add(set, "a", 1);
    zset_add(set, "b", 2);

    char **results = suggestion_set_first_n(set, 3);

    cr_assert_eq(0, strncmp(results[0], "b", MAXLEN), 
                "suggestion_set_first_n() first result incorrect");
    cr_assert_eq(0, strncmp(results[1], "a", MAXLEN), 
                "suggestion_set_first_n() second result incorrect");
    cr_assert_null(results[2], 
                "suggestion_set_first_n() third result incorrect");
    
    zset_remrangebyrank(set, 0, -1);
}

// Test for suggestion_list wrapper function simple
Test(suggestion, suggestion_list_s0) {
    dict_t *d = dict_new();

    dict_add(d, "a");

    // lol
    char **result = suggestion_list(d, "a", 0, 1);

    cr_assert_eq(0, strncmp(result[0], "a", MAXLEN), 
                "suggestion_list() first result incorrect");
}

Test(suggestion, suggestion_list_s1) {
    dict_t *d = dict_new();

    dict_add(d, "afij4-8");
    dict_add(d, "ayij48-");
    dict_add(d, "flij4-8");
    dict_add(d, "afij4*8");
    dict_add(d, "antij4-8"); 
    dict_add(d, "nkj345yf");
    dict_add(d, "fdjsk43");
    dict_add(d, "s");
    dict_add(d, "jvu4893jfDJSkds8932ujfvn.`>IW");


    char **result = suggestion_list(d, "afij4-8", 3, 3);

    cr_assert_eq(0, strncmp(result[0], "afij4-8", MAXLEN), 
                "suggestion_list() first result incorrect");
    cr_assert_eq(0, strncmp(result[1], "afij4*8", MAXLEN), 
                "suggestion_list() second result incorrect");
    cr_assert_eq(0, strncmp(result[2], "flij4-8", MAXLEN), 
                "suggestion_list() third result incorrect");
}