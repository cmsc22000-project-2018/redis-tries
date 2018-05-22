/* Here are examples of our visualization algorithms
We based this on the Trea team design document V1.0
Please also consult our design document for a more thorough elucisdation
of our visulizations.

Made by Marco Harnam Kaisth and Hongji Liu */
/* Separated by Redis-Tries team into .c and .h files */

#include <string.h>
#include <stdio.h>
#include "trie.h"

// Written by Marco Harnam Kaisth

// Leaf visualization
// Based on https://www.geeksforgeeks.org/trie-display-content/
// Takes a trie t, an empty string path to fill as it goes down the trie
// An int level indicating the current level of a trie
// A pointer to the array of strings returned, and the current index of that array
int lviz(trie_t* t, char path[], int level, char** return_arr, int* return_index);

// Word visualization
// Based on https://www.geeksforgeeks.org/trie-display-content/
// Takes a trie t, an empty string path to fill as it goes doewn the trie
// An int level indicating the current level of a trie
// A pointer to the array of strings returned, and the current index of that array
int wviz(trie_t* t, char path[], int level, char** return_arr, int* return_index);
// Written by Hongji Liu

//Visualizes every path in trie t
int eviz(trie_t* t, char* str, int level, char** return_arr, int* return_index);

// Visualizes a given subtree of trie t, given by char* input
int sviz(trie_t* t, char* input, char* str, int level, char** return_arr, int* return_index);