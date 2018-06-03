#include <criterion/criterion.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "trie.h"
#include <stdbool.h>

/* Checks if trie_new() can properly allocate a new trie */
Test(trie, trie_new)
{
    trie_t *t;

    t = trie_new('\0');

    cr_assert_not_null(t, "trie_new() failed to allocate memory");
    cr_assert_eq(t->current, '\0', "trie_new() failed to set current");
    cr_assert_eq(t->is_word, 0, "trie_new() failed to set is_word");
}

/* Checks if trie_add_node() can handle adding a node that already exists */
Test(trie, trie_add_node_exists)
{
    char n = 'n';
    trie_t *t = trie_new('\0');
    int rc = trie_add_node(t, n);

    cr_assert_eq(rc, 0, "trie_add_node failed");
    cr_assert_not_null(t->children[(unsigned)n], "trie_add_node() failed \
        to allocate new entry");
    cr_assert_eq(t->children[(unsigned)n]->current, n, "trie_add_node() \
        failed to set is_word for new trie");
    cr_assert_eq(t->children[(unsigned)n]->is_word, 0, "trie_add_node() \
    failed to set is_word for new trie");
}

/* Checks if trie_add_node() can insert a new char */
Test(trie, trie_add_node_new)
{
    char n = 'n';
    trie_t *t = trie_new('\0');

    int fc = trie_add_node(t, n);
    cr_assert_eq(fc, 0, "trie_add_node failed");

    int rc = trie_add_node(t, n);

    cr_assert_eq(rc, 0, "trie_add_node failed");
    cr_assert_not_null(t->children[(unsigned)n], "trie_add_node() failed to \
        allocate new entry");
    cr_assert_eq(t->children[(unsigned)n]->current, n, "trie_add_node() \
        failed to set is_word for new trie");
    cr_assert_eq(t->children[(unsigned)n]->is_word, 0, "trie_add_node() \
        failed to set is_word for new trie");
}

/* Checks if trie_insert_string() can properly insert a string */
Test(trie, trie_insert_string)
{
    char* s1 = "an";
    char* s2 = "anti";
    char* s3 = "ants";
    trie_t *t = trie_new('\0');

    int r1 = trie_insert_string(t,s1);
    cr_assert_eq(r1, 0, "trie_insert_string failed");   
    cr_assert_not_null(t->children['a'], "trie_add_node() failed to allocate \
        new entry");
    cr_assert_eq(t->children['a']->is_word,0, "trie_add_node() failed to \
        set is_word for new trie");
    cr_assert_not_null(t->children['a']->children['n'], "trie_add_node() \
        failed to allocate new entry");
    cr_assert_eq(t->children['a']->children['n']->is_word, 1, 
        "trie_insert_string() failed to set is_word for end character");

    int r2 = trie_insert_string(t, s2);
    cr_assert_eq(r2, 0, "trie_insert_string() failed");
    cr_assert_eq(t->children['a']->children['n']->is_word, 1, 
        "trie_insert_string() failed to set is_word for end character");
    cr_assert_eq(t->children['a']->children['n']->children['t']->children['i']->is_word,
        1, "trie_insert_string() failed to set is_word for end character");
    int r3 = trie_insert_string(t, s3);
    cr_assert_eq(r3, 0, "trie_insert_string() failed");
    cr_assert_eq(t->children['a']->children['n']->children['t']->is_word, 
        0, "trie_insert_string() failed to set is_word for middle character");
    cr_assert_eq(t->children['a']->children['n']->children['t']->children['s']->is_word,
        1, "trie_insert_string() failed to set is_word for end character");
}

/* Checks if trie_free() can properly free a trie */ 
Test(trie, trie_free)
{
    trie_t *t;
    int rc;

    t = trie_new('\0');

    cr_assert_not_null(t, "trie_new() failed to allocate memory");

    rc = trie_free(t);

    cr_assert_eq(rc, 0, "trie_free() failed");
}

/* Checks trie_free() on a long trie */
Test(trie, trie_free_long)
{
    trie_t *t;
    int inserted, rc;
    char* words[21] = {"Ever", "loved", "someone", "so", "much,", "you", 
    "would", "do", "anything", "for", "them?", "Yeah,", "well,", "make", 
    "that", "yourself", "and", "whatever", "the", "hell", "want."};

    t = trie_new('\0');
    cr_assert_not_null(t, "trie_new() failed");

    for (int i = 0; i < 21; i++) {
        inserted = trie_insert_string(t, words[i]);
        cr_assert_eq(inserted, 0, "trie_insert_string() failed for \
            %dth string %s", i, words[i]);
    }

    rc = trie_free(t);

    cr_assert_eq(rc, 0, "trie_free() failed");
}

/* Checks trie_search() on an empty trie returns 0 */
Test(trie, trie_search_empty)
{
    trie_t *t;
    int found;
    
    t = trie_new('\0');
    cr_assert_not_null(t, "trie_new() failed");

    found = trie_search(t, "b");

    cr_assert_eq(found, 0, "trie_search() does not return 0 with an \
        empty trie");
}

/* Checks a trie with only one word to see if trie_search() returns expected for given str */
void singleton_search(char* str, int expected)
{
    trie_t *t;
    int inserted, found;

    t = trie_new('\0');
    cr_assert_not_null(t, "trie_new() failed");

    inserted = trie_insert_string(t,"CS220");
    cr_assert_eq(inserted, 0, "trie_insert_string() failed");

    found = trie_search(t,str); // Checks if str can be found 
    cr_assert_eq(found, expected, "trie_search() returns %s instead of %s \
        with inserted word",
        (found==0)? "NOT_IN_TRIE" : ((found==1)? "IN_TRIE" : "PARTIAL_IN_TRIE"),
        (expected==0)? "NOT_IN_TRIE" : ((expected==1)? "IN_TRIE" : "PARTIAL_IN_TRIE"));
}

/* Checks for the inserted word in a singleton trie */
Test(trie, singleton_search_inserted)
{
    singleton_search("CS220", IN_TRIE);
}

/* Checks for the prefix of the inserted word in a singleton trie */
Test(trie, singleton_search_prefix)
{
    singleton_search("CS", PARTIAL_IN_TRIE);
}

/* Checks for the extension of the inserted word in a singleton trie */
Test(trie, singleton_search_extension)
{
    singleton_search("CS22000", NOT_IN_TRIE);
}

/* Checks for a non-inserted word in a singleton trie */
Test(trie, singleton_search_not_inserted)
{
    singleton_search("Computer", NOT_IN_TRIE);
}

/* 
   Checks a trie with 21 words to see if trie_search() returns 
   expected for given str 
 */
void twenty_search(char* str, int expected)
{
    trie_t *t;
    int inserted, found;
    char* words[21] = {"Ever", "loved", "someone", "so", "much,", "you", "would", "do", 
        "anything", "for", "them?", "Yeah,", "well,", "make", "that", "yourself", "and", 
        "whatever", "the", "hell", "want."};
    t = trie_new('\0');
    cr_assert_not_null(t, "trie_new() failed");

    for (int i = 0; i < 21; i++) {
        inserted = trie_insert_string(t, words[i]);
        cr_assert_eq(inserted, 0, "trie_insert_string() failed for %dth \
            string %s", i, words[i]);
    }

    found = trie_search(t, str); // Checks if str can be found 
    cr_assert_eq(found, expected, "trie_search() for %s returns %s instead of \
        %s with inserted word", str,
        (found==0)? "NOT_IN_TRIE" : ((found==1)? "IN_TRIE" : "PARTIAL_IN_TRIE"),
        (expected==0)? "NOT_IN_TRIE" : ((expected==1)? "IN_TRIE" : "PARTIAL_IN_TRIE"));
}

/* Checks that inserted words are in 21-word trie */
Test(trie, twenty_search_inserted)
{
    char* words[21] = {"Ever", "loved", "someone", "so", "much,", "you", 
    "would", "do", "anything", "for", "them?", "Yeah,", "well,", "make", 
    "that", "yourself", "and", "whatever", "the", "hell", "want."};

    for (int i = 0; i < 21; i++) 
        twenty_search(words[i], IN_TRIE);
}

/* Checks that prefixes of inserted words are partially in 21-word trie */
Test(trie, twenty_search_prefix)
{
    char* words[21] = {"Eve", "lov", "som", "s", "much", "yo", "woul", "d", 
    "any", "fo", "th", "Yeah", "well", "mak", "tha", "yoursel", "a", "whateve", 
    "th", "hel", "want"}; 

    for (int i = 0; i < 21; i++) 
        twenty_search(words[i], PARTIAL_IN_TRIE);
}

/* Checks that extensions of inserted words are not in 21-word trie */
Test(trie, twenty_search_extension)
{
    char* words[21] = {"Every", "loved!", "someones", "sos", "much,?", 
    "you're", "would've", "done", "anything0", "fore", "them?!", "Yeah,pers", 
    "well,p", "maker", "that'll", "yourselfes", "andar", "whatevers", "there", 
    "helluva", "want.ing"}; 

    for (int i = 0; i < 21; i++) 
        twenty_search(words[i], NOT_IN_TRIE);
}

/* Checks that not inserted words are not in 21-word trie */
Test(trie, twenty_search_not_inserted)
{
    char* words[8] = {"Writing", "tests", "0", "1", "infinity", 
    "law", "Harvey", "Specter"}; 

    for (int i = 0; i < 8; i++) 
        twenty_search(words[i], NOT_IN_TRIE);
}

/* Helper function for check_completion() testing */
void search_completion(char* pre, int expected)
{
    trie_t *t;
    int inserted, number;
    char* words[8] = {"an","ant","anti", "antique", "antiquity", "antelope", 
    "antman","anthropology"}; 
    t = trie_new('\0');
    cr_assert_not_null(t, "trie_new() failed");

    for (int i = 0; i < 8; i++) {
        inserted = trie_insert_string(t, words[i]);
        cr_assert_eq(inserted, 0, "trie_insert_string() failed for %dth \
            string %s", i, words[i]);
    }

    number = trie_count_completion(t, pre); // Checks if str can be found 
    cr_assert_eq(number, expected, "count_completion() for %s returns %d \
        completions instead of %d.\n",
        pre, number, expected);
}

/* Checks if count_completion can handle a prefix not in the trie */
Test(trie, count_completion_prefix_not_in_trie)
{
    search_completion("banana", 0);
    search_completion("anthropologist", 0);
}

/* Checks if count_completion can handle a prefix in the trie */
Test(trie, count_completion_prefix_in_trie_1)
{
    search_completion("a", 8);
}

/* Checks if count_completion can handle a prefix in the trie */
Test(trie, count_completion_prefix_in_trie_2)
{
    search_completion("an", 8);
}

/* Checks if count_completion can handle a prefix in the trie */
Test(count_completion, prefix_in_trie_3)
{
    search_completion("ant", 7);
}

/* Checks if count_completion can handle a prefix in the trie */
Test(count_completion, prefix_in_trie_4)
{
    search_completion("anti", 3);
}

/* Checks if count_completion can handle a prefix in the trie */
Test(count_completion, prefix_in_trie_5)
{
    search_completion("antiq", 2);
}

/* Checking if trie_char_exists() can check for a char not in trie */
Test(trie, trie_char_exists_failure0) {
    trie_t *t;
    int rc;

    t = trie_new('\0');

    rc = trie_char_exists(t, 'c');

    cr_assert_eq(rc, false, "trie_char_exists succeeded when it shouldn't have");
}

/* Checking if trie_char_exists() can check for a char not in trie */
Test(trie, trie_char_exists_failure1) {
    trie_t *t;
    int rc;

    t = trie_new('\0');

    trie_insert_string(t, "candy");

    rc = trie_char_exists(t, '7');

    cr_assert_eq(rc, false, "trie_char_exists succeeded when it shouldn't have");
}

/* Checking if trie_char_exists() can check for a char in trie */
Test(trie, trie_char_exists_success0) {
    trie_t *t;
    int rc;

    t = trie_new('\0');

    trie_insert_string(t, "jkl538-yfv");

    rc = trie_char_exists(t, '-');

    cr_assert_eq(rc, true, "trie_char_exists failed when it should have succeeded");
}

/* Checking if trie_char_exists() can check for a char in trie */
Test(trie, trie_char_exists_success1) {
    trie_t *t;
    int rc;

    t = trie_new('\0');

    trie_insert_string(t, "candy");

    rc = trie_char_exists(t, 'd');

    cr_assert_eq(rc, true, "trie_char_exists failed when it should have succeeded");
}

/* Checking if trie_char_exists() can check for a terminal character in trie */
Test(trie, trie_chars_exists_null) {
    trie_t *t;
    int rc;

    t = trie_new('\0');

    rc = trie_char_exists(t, '\0');

    cr_assert_eq(rc, false, "trie_char_exists succeeded when it shouldn't have");
}

/* 
   Integration test using trie_new, trie_insert_string (which uses 
   trie_add_node), trie_search, and trie_free 
 */
void integration(char* str, int expected)
{
    trie_t *t;
    int inserted, found;
    char* words[9] = {"Everybody", "has", "a", "chapter", "they", "don't", 
    "read", "out", "loud."};

    t = trie_new('\0');
    cr_assert_not_null(t, "trie_new() failed");

    for (int i = 0; i < 9; i++) {
        inserted = trie_insert_string(t, words[i]);
        cr_assert_eq(inserted, 0, "trie_insert_string() failed for %dth \
            string %s", i, words[i]);
    }

    found = trie_search(t, str); // Checks if str can be found 
    cr_assert_eq(found, expected, "trie_search() for %s returns %s instead of \
        %s with inserted word", str,
        (found==0)? "NOT_IN_TRIE" : ((found==1)? "IN_TRIE" : "PARTIAL_IN_TRIE"),
        (expected==0)? "NOT_IN_TRIE" : ((expected==1)? "IN_TRIE" : "PARTIAL_IN_TRIE"));

    trie_free(t);
}

/* Checks inserted strings in integration test */
Test(trie, integration_inserted)
{
    char* words[9] = {"Everybody", "has", "a", "chapter", "they", "don't", 
    "read", "out", "loud."};

    for (int i = 0; i < 9; i++) 
        integration(words[i], IN_TRIE);
}

/* Checks prefixes of inserted strings in integration test */
Test(trie, integration_prefix)
{
    char* words[8] = {"Every", "ha", "chap", "the", "do", "rea", "o", 
    "loud"}; // - Unknown

    for (int i = 0; i < 8; i++) 
        integration(words[i], PARTIAL_IN_TRIE);
}

/* Checks of non-inserted strings in integration test */
Test(trie, integration_not_inserted)
{
    char* words[8] = {"Blah", "Tuba", "Player", "Wow", "7th", "Coolio", 
    "whattt", "ranking"};

    for (int i = 0; i < 8; i++) 
        integration(words[i], NOT_IN_TRIE);
}

/* Checks extensions of inserted strings in integration test */
Test(trie, integration_extension)
{
    char* words[9] = {"Everybody's", "hasp", "an", "chapters", "they're", 
    "don't0", "reading", "outer", "loud.duol"};

    for (int i = 0; i < 9; i++) 
        integration(words[i], NOT_IN_TRIE);
}