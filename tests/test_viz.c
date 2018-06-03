#include <criterion/criterion.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "../include/viz.h"
#include "../include/trie.h"

/* TESTS FOR TRIE_VISUALIZATION FNS
 * assumes all viz fns will return string to print
 */

/* EXHAUSTIVE VISUALIZATION TESTS
 * eviz()
 */

/*helper fn for eviz*/

/* compare_char_arr
 * parameters: actual and expected char arrys, int of array size
 * compares char**
 * returns 0 if not identical
 * returns 1 if identical
 */
int compare_char_arr(char** actual, char** expected, int size){
    int i, j;

    //loop through trie
    for(i = 0; i < size; i++){
        if(strcmp(actual[i], expected[i]) == 0)
            return 0;
        }
    }
    return 1; 
}

/* eviz test cases */

Test(eviz, empty_trie_true){
    char** input = [""];
    trie_t *t = new_trie(input);

    int size = 0;
    char** actual = eviz(t);

    int result = compare_char_arr(actual, input, size);

    cr_assert_eq(1, result, "eviz test failed");
}

Test(eviz, one_first_letter_true){
    char** input = ["a", "an", "and", "at"];
    trie_t *t = new_trie(input);

    int size = 4;
    char** actual = eviz(t);

    int result = compare_char_arr(actual, input, size);

    cr_assert_eq(1, result, "eviz test failed");
}

Test(eviz, one_first_letter_false){
    char ** input = ['a', 'an', 'and', 'at'];
    trie_t *t = new_trie(input);

    int size = 4;
    char** actual = eviz(t);
    char** mod_input = ['a', 'an', 'and'];

    int result = compare_char_arr(actual, mod_input, size);

    cr_assert_eq(0, result, "eviz test failed");
}

/* LEAF VISUALIZATION TESTS
 * lviz()
 */

/* lviz test cases */

Test(lviz, one_first_letter_true){
    char **input = ["c", "ca", "cat", "catc", "catch"];
    trie_t *t = new_trie(input);

    int size = 1;
    char **expected = ["catch"];
    char **actual = lviz(t);

    int result = compare_char_arr(actual, expected, size);

    cr_assert_eq(1, result, "lviz test failed");
}

Test(lviz, one_first_letter_false){
    char **input = []"c", "ca", "cat", "catc", "catch"];
    trie_t *t = new_trie(input);

    int size = 1;
    char **expected = ["catc"];
    char **actual = lviz(t);

    int result = compare_char_arr(actual, expected, size);

    cr_assert_eq(0, result, "lviz test failed");
}

/* WORD VISUALIZATION TESTS
 * wviz()
 */


/* wviz test cases */

Test(wviz, one_first_letter_true){
    char **input = ["c", "ca", "cat", "catc", "catch"];

    trie_t *t = new_trie(input);

    int size = 2;
    char **expected = ["cat", "catch"];
    char **actual = lviz(t);

    int result = compare_char_arr(actual, expected, size);

    cr_assert_eq(1, result, "wviz test failed");

}

Test(wviz, one_1st_letter_false){
    char **input = ["c", "ca", "cat", "catc", "catch"];
    trie_t *t = new_trie(input);

    int size = 2;
    char **expected = ["cat", "catc"];
    char **actual = lviz(t);

    int result = compare_char_arr(actual, expected, size);

    cr_assert_eq(1, result, "wviz test failed");

}

/* SUBTREE VISUALIZATION TESTS
 * sviz()
 */

/* sviz test cases */

Test(sviz, working_trie_one_1st_letter){
    char **input = ["c", "ca", "cat", "catc", "catch"];
    trie_t *t = new_trie(input);

    int size = 3;
    char **expected = ["cat", "catc", "catch"];
    char **actual = lviz(t, "cat");

    int result = compare_char_arr(actual, expected, size);

    cr_assert_eq(0, result, "wviz test failed");

}

Test(sviz, working_trie_full_alphabet_start_letters){
    char **input = ["c", "ca", "cat", "catc", "catch"];
    trie_t *t = new_trie(input);

    int size = 3;
    char **expected = ["cat", "ca", "catch"];
    char **actual = lviz(t, "cat");

    int result = compare_char_arr(actual, expected, size);

    cr_assert_eq(0, result, "wviz test failed");

}
