#include <criterion/criterion.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "suggestion.h"

// Test has_children for success
Test(suggestion, has_children_s0) {
    trie_t *t = trie_new('\0');
    trie_insert_string(t, "abc");

    cr_assert_eq(has_children(t, "ab"), EXIT_SUCCESS, "has_children failed");
}

// Test has_children for failure
Test(suggestion, has_children_f0) {
    trie_t *t = trie_new('\0');
    trie_insert_string(t, "abc");

    cr_assert_eq(has_children(t, "ac"), EXIT_FAILURE, "has_children failed");
}

// Test has_children for empty prefix
Test(suggestion, has_children_empty) {
    trie_t *t = trie_new('\0');
    trie_insert_string(t, "abc");

    cr_assert_eq(has_children(t, ""), EXIT_SUCCESS, "has_children failed");
}

/*************   These functions individually test values for cmp_match   *************/

void test_cmp_match(char *m1_str, char *m2_str, int m1_val, int m2_val, int expected) {
    match_t *m1 = malloc(sizeof(match_t));
    match_t *m2 = malloc(sizeof(match_t));

    m1->str = m1_str;
    m2->str = m2_str;
    

    m1->edits_left = m1_val;
    m2->edits_left = m2_val;

    int rc = cmp_match(&m1, &m2);

    cr_assert_eq(rc, expected, "cmp_match on %s, %d and %s, %d returned %d instead of %d",
                 m1_str, m1_val, m2_str, m2_val, rc, expected);
}

Test(suggestion, cmp_match_0) {
    
    test_cmp_match("aa", "ab", 0, 0, -1);
}

Test(suggestion, cmp_match_1) {
    test_cmp_match("ab", "aa", 0, 0, 1);
}

Test(suggestion, cmp_match_2) {
    test_cmp_match("aa", "aa", 1, 0, -1);
}

Test(suggestion, cmp_match_3) {
    test_cmp_match("aa", "aa", 0, 1, 1);
}

Test(suggestion, cmp_match_null0) {
    match_t *m1 = malloc(sizeof(match_t));
    match_t *null = NULL;

    m1->str = "hi";

    m1->edits_left = 0;

    int rc = cmp_match(&m1, &null);

    cr_assert_eq(rc, -1, "cmp_match with a null argument failed");
}

Test(suggestion, cmp_match_null1) {
    match_t *m1 = malloc(sizeof(match_t));
    match_t *null = NULL;

    m1->str = "hi";

    m1->edits_left = 0;

    int rc = cmp_match(&null, &m1);

    cr_assert_eq(rc, 1, "cmp_match with a null argument failed");
}

/*************   These functions individually test helpers of suggestions()   *************/

// Test a simple move_on
Test(suggestion, move_s0) {
    trie_t *t = trie_new('\0');
    match_t **set = calloc(1, sizeof(match_t*));

    trie_insert_string(t, "f");

    int rc = suggestions(set, t, "", "f", 1, 1);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(strcmp(set[0]->str, "f"), 0, "move_on string failed");
    cr_assert_eq(set[0]->edits_left, 1, "move_on failed");
}

// Test a full move_on
Test(suggestion, move_s1) {
    trie_t *t = trie_new('\0');
    match_t **set = calloc(5, sizeof(match_t*));

    trie_insert_string(t, "vdf894jkls");

    int rc = suggestions(set, t, "", "vdf894jkls", 1, 5);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(strcmp(set[0]->str, "vdf894jkls"), 0, "move_on string failed");
    cr_assert_eq(set[0]->edits_left, 1, "move_on value failed");
}

// Test a simple delete
Test(suggestion, delete_s0) {
    trie_t *t = trie_new('\0');
    match_t **set = calloc(1, sizeof(match_t*));

    trie_insert_string(t, "");

    int rc = suggestions(set, t, "", "k", 1, 1);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(strcmp(set[0]->str, ""), 0, "try_delete string failed");
    cr_assert_eq(set[0]->edits_left, 0, "try_delete value failed");
}

// Test a double delete
Test(suggestion, delete_s1) {
    trie_t *t = trie_new('\0');
    match_t **set = calloc(5, sizeof(match_t*));

    trie_insert_string(t, "cds8Dfk");

    int rc = suggestions(set, t, "", "cdisc8Dfk", 2, 5);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(strcmp(set[0]->str, "cds8Dfk"), 0, "try_delete string failed");
    cr_assert_eq(set[0]->edits_left, 0, "try_delete value failed");
}

// Test a simple replace
Test(suggestion, replace_s0) {
    trie_t *t = trie_new('\0');
    match_t **set = calloc(1, sizeof(match_t*));

    trie_insert_string(t, "s");

    int rc = suggestions(set, t, "", "i", 1, 1);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(strcmp(set[0]->str, "s"), 0, "try_replace string failed");
    cr_assert_eq(set[0]->edits_left, 0, "try_replace value failed");
}

// Test a double replace
Test(suggestion, replace_s1) {
    trie_t *t = trie_new('\0');
    match_t **set = calloc(5, sizeof(match_t*));

    trie_insert_string(t, "vT.k=(?");

    int rc = suggestions(set, t, "", "vTik=8?", 2, 5);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(strcmp(set[0]->str, "vT.k=(?"), 0, "try_replace string failed");
    cr_assert_eq(set[0]->edits_left, 0, "try_replace value failed");
}

// Test a simple swap
Test(suggestion, swap_s0) {
    trie_t *t = trie_new('\0');
    match_t **set = calloc(1, sizeof(match_t*));

    trie_insert_string(t, "ap");

    int rc = suggestions(set, t, "p", "a", 1, 1);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(strcmp(set[0]->str, "ap"), 0, "try_swap string failed");
    cr_assert_eq(set[0]->edits_left, 0, "try_swap value failed");
}

// Test a double swap 
Test(suggestion, swap_s1) {
    trie_t *t = trie_new('\0');
    match_t **set = calloc(5, sizeof(match_t*));

    trie_insert_string(t, "4uiwifnw");

    int rc = suggestions(set, t, "", "4uiwnfiw", 2, 5);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(strcmp(set[0]->str, "4uiwifnw"), 0, "try_swap string failed");
    cr_assert_eq(set[0]->edits_left, 0, "try_swap value failed");
}

// Test a simple insert
Test(suggestion, insert_s0) {
    trie_t *t = trie_new('\0');
    match_t **set = calloc(1, sizeof(match_t*));

    trie_insert_string(t, "+");

    int rc = suggestions(set, t, "", "", 1, 1);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(strcmp(set[0]->str, "+"), 0, "try_swap string failed");
    cr_assert_eq(set[0]->edits_left, 0, "try_swap value failed");
}

// Test a double insert
Test(suggestion, insert_s1) {
    trie_t *t = trie_new('\0');
    match_t **set = calloc(5, sizeof(match_t*));

    trie_insert_string(t, "fnjk43?>.f@");

    int rc = suggestions(set, t, "", "fnk43?>.f", 2, 5);

    cr_assert_eq(0, rc, "suggestions() failed");
    cr_assert_eq(strcmp(set[0]->str, "fnjk43?>.f@"), 0, "try_swap string failed");
    cr_assert_eq(set[0]->edits_left, 0, "try_swap value failed");
}

/*************   These functions test suggestions() as a whole   *************/

// Test regular suggestion generation 1
Test(suggestion, suggestions_s0) {
    trie_t *t = trie_new('\0');
    match_t **set = calloc(4, sizeof(match_t*));

    trie_insert_string(t, "cops");
    trie_insert_string(t, "8op");
    trie_insert_string(t, "op");
    trie_insert_string(t, "ocp");

    int rc = suggestions(set, t, "", "cop", 1, 4);
    cr_assert_eq(0, rc, "suggestions() failed");

    // Get the items in order since suggestions() doesn't sort
    qsort(set, 4, sizeof(match_t*), cmp_match);

    cr_assert_eq(0, strncmp(set[0]->str, "8op", MAXLEN), 
                "suggestions() first result incorrect");
    cr_assert_eq(0, strncmp(set[1]->str, "cops", MAXLEN), 
                "suggestions() second result incorrect");
    cr_assert_eq(0, strncmp(set[2]->str, "ocp", MAXLEN), 
                "suggestions() third result incorrect");
    cr_assert_eq(0, strncmp(set[3]->str, "op", MAXLEN), 
                "suggestions() fourth result incorrect");
}

// Test regular suggestion generation 1
Test(suggestion, suggestions_s1) {
    trie_t *t = trie_new('\0');
    match_t **set = calloc(5, sizeof(match_t*));

    trie_insert_string(t, "stall");
    trie_insert_string(t, "satll"); 
    // Although the above item is within the correct edit distance, there isn't enough space for it
    // since there are too many other matches and it's after the matches alphabetically
    trie_insert_string(t, "drall");
    trie_insert_string(t, "malt");
    trie_insert_string(t, "malfkt"); // not a return
    trie_insert_string(t, "fn|d7~3"); // not a return

    int rc = suggestions(set, t, "", "small", 2, 3);
    cr_assert_eq(0, rc, "suggestions() failed");

    // Get the items in order since suggestions() doesn't sort
    qsort(set, 3, sizeof(match_t*), cmp_match);

    cr_assert_eq(0, strncmp(set[0]->str, "stall", MAXLEN), 
                "suggestions() first result incorrect");
    cr_assert_eq(0, strncmp(set[1]->str, "drall", MAXLEN), 
                "suggestions() second result incorrect");
    cr_assert_eq(0, strncmp(set[2]->str, "malt", MAXLEN), 
                "suggestions() third result incorrect");
}

/*************   These functions test the easy input functions   *************/

// Test for suggestion_set_new function
Test(suggestion, suggestion_set_new_s0) {
    trie_t *t = trie_new('\0');

    trie_insert_string(t, "only");
    trie_insert_string(t, "onety");
    trie_insert_string(t, "none");
    trie_insert_string(t, "fjmsdk8");
    trie_insert_string(t, "o=9");

    match_t **set = suggestion_set_new(t, "one", 2, 4);

    // Get the items in order since suggestions() doesn't sort
    qsort(set, 4, sizeof(match_t*), cmp_match);

    cr_assert_eq(0, strncmp(set[0]->str, "none", MAXLEN), 
                "suggestion_set_new() first result incorrect");
    cr_assert_eq(0, strncmp(set[1]->str, "o=9", MAXLEN), 
                "suggestion_set_new() second result incorrect");
    cr_assert_eq(0, strncmp(set[2]->str, "onety", MAXLEN), 
                "suggestion_set_new() third result incorrect");
    cr_assert_eq(0, strncmp(set[3]->str, "only", MAXLEN), 
                "suggestion_set_new() fourth result incorrect");
}

// Test for suggestion_set_first_n function with regular input
Test(suggestion, suggestion_set_first_n_s0) {
    match_t **set = calloc(5, sizeof(match_t*));

    set[0] = malloc(sizeof(match_t));
    set[1] = malloc(sizeof(match_t));
    set[2] = malloc(sizeof(match_t));

    set[0]->str = "a";
    set[1]->str = "b";
    set[2]->str = "c";

    set[0]->edits_left = 0;
    set[1]->edits_left = 0;
    set[2]->edits_left = 0;

    char **results = suggestion_set_first_n(set, 2);

    cr_assert_eq(0, strncmp(results[0], "a", MAXLEN), 
                "suggestion_set_first_n() first result incorrect");
    cr_assert_eq(0, strncmp(results[1], "b", MAXLEN), 
                "suggestion_set_first_n() second result incorrect");
}

// Test for suggestion_set_first_n function with not enough matches
Test(suggestion, suggestion_set_first_n_f0) {
    match_t **set = calloc(5, sizeof(match_t*));

    set[0] = malloc(sizeof(match_t));
    set[1] = malloc(sizeof(match_t));
    set[2] = malloc(sizeof(match_t));

    set[0]->str = "a";
    set[1]->str = "b";
    set[2]->str = "c";

    set[0]->edits_left = 0;
    set[1]->edits_left = 0;
    set[2]->edits_left = 0;

    char **results = suggestion_set_first_n(set, 2);

    cr_assert_eq(0, strncmp(results[0], "a", MAXLEN), 
                "suggestion_set_first_n() first result incorrect");
    cr_assert_eq(0, strncmp(results[1], "b", MAXLEN), 
                "suggestion_set_first_n() second result incorrect");
    cr_assert_null(results[2], 
                "suggestion_set_first_n() third result incorrect");
}

// Test for suggestion_list wrapper function simple
Test(suggestion, suggestion_list_s0) {
    trie_t *t = trie_new('\0');

    trie_insert_string(t, "a");

    // lol
    char **result = suggestion_list(t, "a", 0, 1);

    cr_assert_eq(0, strncmp(result[0], "a", MAXLEN), 
                "suggestion_list() first result incorrect");
}

Test(suggestion, suggestion_list_s1) {
    trie_t *t = trie_new('\0');

    trie_insert_string(t, "afij4-8");
    trie_insert_string(t, "ayij48-");
    trie_insert_string(t, "flij4-8");
    trie_insert_string(t, "afij4*8");
    trie_insert_string(t, "antij4-8"); 
    trie_insert_string(t, "nkj345yf");
    trie_insert_string(t, "fdjsk43");
    trie_insert_string(t, "s");
    trie_insert_string(t, "jvu4893jfDJSkds8932ujfvn.`>IW");


    char **result = suggestion_list(t, "afij4-8", 3, 3);

    cr_assert_eq(0, strncmp(result[0], "afij4-8", MAXLEN), 
                "suggestion_list() first result incorrect");
    cr_assert_eq(0, strncmp(result[1], "afij4*8", MAXLEN), 
                "suggestion_list() second result incorrect");
    cr_assert_eq(0, strncmp(result[2], "antij4-8", MAXLEN), 
                "suggestion_list() third result incorrect");
}