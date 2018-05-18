/* Here are examples of our visualization algorithms
We based this on the Trea team design document V1.0
Please also consult our design document for a more thorough elucisdation
of our visulizations.

Made by Marco Harnam Kaisth and Hongji Liu */

#include <string.h>
#include <stdio.h>

// Written by Marco Harnam Kaisth

// Leaf visualization
// Based on https://www.geeksforgeeks.org/trie-display-content/
// Takes a trie t, an empty string path to fill as it goes down the trie
// An int level indicating the current level of a trie
// A pointer to the array of strings returned, and the current index of that array
int lviz(trie_t* t, char path[], int level, char** return_arr, int* return_index)
{
	if (return_arr == NULL) {
        fprintf(stderr, "lviz: return_arr is NULL");
        return 0;
    }

    if (return_index == NULL) {
        fprintf(stderr, "lviz: return_index is NULL");
        return 0;
    }

	if (!t->children)
		// If current node is a leaf
	{
		path[level] = '\0';
		//Set the current index of the string to terminal char
		return_arr[return_index] = strdup(path);
		//Put the constructed string into the array of strings to be printed
		*return_index++; 
		//Increment the current index of said array
	}

	for (int i = 0; i < 255; i++)
		// For all possible children
	{
		if (t->children[i])
			// If such a child exists
		{
			path[level]=t->current;
			// Make the current index of the string whatever char is present
			lviz(t->children[i], path, ++level, return_arr, return_index);
			// Recursively call lviz on the child node
		}
	}

	return 1;
}

// Word visualization
// Based on https://www.geeksforgeeks.org/trie-display-content/
// Takes a trie t, an empty string path to fill as it goes doewn the trie
// An int level indicating the current level of a trie
// A pointer to the array of strings returned, and the current index of that array
int wviz(trie_t* t, char path[], int level, char** return_arr, int* return_index)
{
	if (return_arr == NULL) {
        fprintf(stderr, "wviz: return_arr is NULL");
        return 0;
    }

    if (return_index == NULL) {
        fprintf(stderr, "wviz: return_index is NULL");
        return 0;
    }

	if (t->is_word)
		// If current node is a word
	{
		path[level] = '\0';
		// Set current char of string to terminating char
		return_arr[return_index] = strdup(path);
		// Place string in constructed array to be printed
		*return_index++;
		// Increment index in said array
	}

	for (int i = 0; i < 255; i++)
		// For all possible characters
	{
		if (t->children[i])
		// If t has such a child
		{
			path[level]=t->current;
			// Add it to the string
			wviz(t->children[i], path, ++level, return_arr, return_index);
			// Recursively call wviz on the current node
		}
	}

	return 1;
	// Return constructed array of strings
}

// Written by Hongji Liu

//Visualizes every path in trie t
int eviz(trie_t* t, char* str, int level, char** return_arr, int* return_index) {

    if (return_arr == NULL) {
        fprintf(stderr, "eviz: return_arr is NULL");
        return 0;
    }

    if (return_index == NULL) {
        fprintf(stderr, "eviz: return_index is NULL");
        return 0;
    }

    /*
     * Similar to Marco's part as t reaches the end of the
     * leaf, ends the str with '\0' and copies the str to
     * return_arr
     */
    if (!t->children) {

        str[level] = '\0';

        return_arr[*return_index] = strdup(str);

        ++*return_index;
    }

    /*
     * Slightly different to Marco's part where str[level]
     * adds the current char in the node of one of its children
     * and the return_arr must add the str since it is exhaustive
     * visualization
     */
    for (int i = 0; i < 255; i++) {

        if (t->children[i]) {

            str[level] = t->children[i].current;

            return_arr[*return_index] = strdup(str + '\0');

            eviz(&t->children[i], str, ++level, return_arr, ++return_index);
        }

    }

    return 1;
}

// Visualizes a given subtree of trie t, given by char* input
int sviz(trie_t* t, char* input, char* str, int level, char** return_arr, int* return_index) {

    if (return_arr == NULL) {
        fprintf(stderr, "eviz: return_arr is NULL");
        return 0;
    }

    if (return_index == NULL) {
        fprintf(stderr, "eviz: return_index is NULL");
        return 0;
    }

    size_t input_size = strlen(input);

    trie_t* subtrie = t;

    /*
     * Gets to the node where the input ends
     */
    for (int j = 0; j < input_size; ++j) {
        subtrie = &subtrie->children[input[j]];
    }

    /*
     * Calls eviz to add the children of the string to
     * return_arr
     */
    eviz(subtrie, str, level, return_arr, return_index);

    /*
     * Add the input string to the front of each
     * string in the return_arr since it is left off
     * in eviz
     */
    for (int i = 0; i < *return_index; ++i) {
        strncat(input, return_arr[i]);
        puts(return_arr[i]);
    }

    return 1;
}
