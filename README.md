# Redis Tries #

Welcome to the Redis Tries module repository! The purpose of this repository is to add and implement support for prefix tries in Redis. 

What are tries? A trie, also known as a digital tree, radix tree, or prefix tree, is an efficient search tree where keys are strings. The name for the data structure comes from the word reTrieval. Using a trie, search complexities can be brought to O(n) where n is the length of the key. Each node of the Trie has a character, indication whether the node represents the end of a word, and multiple branches. Each branch represents a possible character for the path of the key. Read more about tries here: https://www.geeksforgeeks.org/trie-insert-and-search/

What is redis? Redis is an open-source, in-memory remote database that offers versatile modules and support for common data structures like hashes, lists and bitmaps. It serves as a popular platform for project development. Read more about Redis here: https://redis.io/

We will be implementing Redis support for the prefix trie first with basic functionality of searching, inserting and deleting and will optimize further based on the needs of the other developing 
functionalities like autocomplete, spellcheck and text searching.

## Our Trie Implementation ##

The trie class is composed of the trie_t struct and its respective operations. 

The trie_t struct is composed of:
	1. char current // The character the current trie contains
	2. trie_t \*children[ALPHABET_SIZE] // ALPHABET_SIZE is 256 for all possible characters.
	3. int is_word // If is_word is 1, indicates that this is the end of a word. Otherwise 0.
	4. trie_t \*parent // Parent trie_t for traversing backwards
  5. char \*charlist // Array of characters that are contained in the node and its children 

The Operations for trie_t are as follows:
1. \*trie_t trie_new(char current)
    
    **Purpose:** Creates and allocates memory for new trie_t.
    
    **Details:** Sets current to be current, all children are initialized to NULL, is_word set to 0.

2. \*trie_t trie_add_node(trie_t \*t, char c)

    **Purpose:** Creates new node in trie_t.
    
    **Details:** Set t->children[current] to be current, is_word for new node set to 0. 

3. int trie_insert_string(trie_t \*t, char \*word) 

    **Purpose:** Inserts word into trie. 

    **Details:** Returns 0 if word is successfully inserted into trie, 1 otherwise. In the trie containing the last character, is_word is set to 1. 

4. int trie_char_exists(trie_t \*t, char c)

    **Purpose:** Checks if a character is contained with a trie.

    **Details:** Returns 0 if the character is contained, 1 if not.

5. trie_t\* trie_get_subtrie(trie_t \*t, char \*word)
    
    **Purpose:** Searches for a word/prefix in a trie. 

    **Details:** Returns a pointer to the end of the prefix (word) if it exists in the trie.

6. int trie_search(trie_t \*t, char \*word)

    **Purpose:** Search for a word in a trie. 

    **Details:** Returns IN_TRIE (1) if word is found. Returns NOT_IN_TRIE (0) if word is not found at all and PARTIAL_IN_TRIE (-1) if word is found but end node's is_word is 0.

7. int trie_count_completion(trie_t \*t, char \*pre)

    **Purpose:** Count the number of different possible endings of a given prefix in a trie. 

    **Details:** Returns the number of endings of the prefix given, 0 if prefix doesn't exist.

8. int trie_free(trie_t \*t)

    **Purpose:** Free an entire trie.

    **Details:** Returns 0 if freed properly.

## Redis ##
[Here](https://www.youtube.com/watch?v=Hbt56gFj998) is a very good video guide for installing/learning the basic functionality of Redis. Text instructions are below.

### Installation ###
1. First download Redis either from the [redis.io](https://redis.io/) website or from this special link that always directs you to the latest version of Redis: http://download.redis.io/redis-stable.tar.gz
2. To compile Redis, enter the following lines in your terminal in sequence:

    `wget http://download.redis.io/redis-stable.tar.gz`
    
    `tar xvzf redis-stable.tar.gz`

    `cd redis-stable`

    `make`

***

*** Note about the src directory ***
    
The src directory contains multiple executables. Here is a brief overview of each one:
    
* **redis-server** is the Redis Server itself
    
* **redis-sentinel** is the Redis Sentinel executable (monitoring and failover)
    
* **redis-cli** is the command line interface utility to talk with Redis
    
* **redis-benchmark** is used to check Redis performances

* **redis-check-aof** and **redis-check-dump** are useful in the rare event of corrupted data files

***


3. (Optional but highly recommended) Test whether the build works properly with: `make test`. If it works properly, you should see "All tests passed without errors!".

4. (Optional but highly recommended) To copy the command line and interface into the proper places for use, type the command: `sudo make install`. This is synonymous to typing:

    `sudo cp src/redis-server /usr/local/bin/`

    `sudo cp src/redis-cli /usr/local/bin/`

5. From here on out, it'll be assumed that /usr/local/bin is in your PATH environment. Type `redis-server` into the command line to start the Redis server (this is simply executing the redis-server binary) \*Note this starts Redis without any configuration file. This is sufficient when first starting out or when you're intending to play around with it. In order to start Redis with a configuration file, simply type in the full file path after redis-server as an argument. I.e. if your Redis file is called "redis.conf" and it's in the etc/ directory, you would type `redis-server /etc/redis.conf`


6. To first check if Redis is working, type `redis-cli ping` (WHILE the Redis server is running). You should get a response of "PONG". This sends a command and its arguments to the instance of the Redis server currently running. You can also simply type in `redis-cli` to start an interactive mode.

[More](https://redis.io/topics/quickstart)

### Data Structures

Redis is a data structures server so unlike in plain-key value stores in which string keys are associated with string values, string keys can be associated with other types of values (more complex data structures). Some of the data structures Redis provides support for are:

* Binary-safe strings
* Lists- essentially linked lists that are sorted according to order of assertion
* Sets- unique unsorted elements
* Sorted Sets- sets in which every string element is assorted to a floating point value. Gives the possibility of retrieving a certain subset of elements (i.e. top 10)
* Hashes- just like an normal hash in which string hashes are associated with string values
* Bit arrays (bitmaps)- allows for the possibility of handling string values like an array of bits and manipulating bits
* HyperLogLogs- probabilistic data structure which is used in order to estimate the cardinality of a set

[More](https://redis.io/topics/data-types-intro) 

## Installing the Module ##

First clone this respository if you haven't already anywhere on your local system. To do this, first go into the directory you wish to clone the repository and then type into the command line: 

    $ git clone https://github.com/cmsc22000-project-2018/redis-tries.git

Inside the repository, initialize the submodule by typing into the command line:

    $ git submodule update --init --recursive

Inside the *module* directory, type "make all" into the command line to build the trie.so library. (Note: The compiler warnings can be ignored.)

Print the working directory and write it down, appending "trie.so" to the end of the path. So if my name is Dustin and my redis-tries directory is on my Desktop, I should have written down /Users/Dustin/Desktop/redis-tries/module/trie.so

Before you begin running the Redis server, make sure to check the src directory in redis-stable for a file called "dump.rdb" if it exists, make sure to delete it before running the server every time you decide to run the server. This is because we have not implemented the data persistence functions.

At this point, we will assume you have Redis fully installed and functional. (Instructions are here if you don't: https://github.com/cmsc22000-project-2018/redis-tries/wiki/Design-Document-v1.)

Inside the redis-stable directory, run the Redis server by typing: 

    $ redis-server

At the end of every session, close the server with CTRL+C.

> Note: If you receive an error message saying "Creating Server TCP listening socket \*:6379: bind: Address already in use," type:

    $ redis-cli ping

> If Redis replies with PONG, you should type:
 
    $ redis-cli shutdown

In a separate window, run the Redis client by typing:

    $ redis-cli

Inside of the Redis client, type in "MODULE LOAD [Path]" where [Path] is the previously copied path to your trie.so library. So in the context of my previous example, "/Users/Dustin/Desktop/redis-tries/module/trie.so"

Your module should be loaded up. Check your server log for an indication that the module trie123az has been loaded.

## Module Commands ##

Here are the current commands the module provides support for:

### TRIE.INSERT key value1 value2 ... valueN
TRIE.INSERT inserts a string into a given trie key. It can insert as many strings as the user types into the commandline. If the key does not previously exist, a new trie will be created and the string will be inserted into this new trie; otherwise the string will be inserted into the existing trie. Returns 0 on success and otherwise, an integer showing how many words were failed to be inserted

       redis> TRIE.INSERT key1 helloworld foo bar
       (int) 0

### TRIE.CONTAINS key value
TRIE.CONTAINS checks if a string exists in a given trie key. If the trie key does not exist, an error will be thrown. Otherwise, different messages will be printed based on whether the string is contained within the trie. If the string is contained within the trie, the integer 1 will be returned. If the string is not contained within the trie, the integer 0 will be returned. If the string is contained within the trie as a prefix but not as a word, the integer -1 will be returned.

       redis> TRIE.INSERT key1 helloworld
       (int) 0
       redis> TRIE.CONTAINS key1 helloworld
       (int) 1
       redis> TRIE.CONTAINS key1 goodbye
       (int) 0
       redis> TRIE.CONTAINS key1 hello
       (int) -1

### TRIE.APPROXMATCH key prefix (optional)max_edit_distance (optional)num_matches
TRIE.APPROXMATCH returns a list of suggested words that have the given prefix. It requires at least a key, whose value is an existing trie, and a prefix (prefix) to look for within the trie. The first optional argument (max_edit_distance) specifies the edit distance (or how close the words returned can be to the given prefix). If no value is given, the default is 2. The second optional argument (num_matches) specifies the number of "matches", or possible completions, that will be returned by the command. If no value is given, the default is 10 (meaning 10 possible words will be given, if there aren't 10 possible endings the remaining slots will be filled with Redis (nil) values).

       redis> TRIE.INSERT key1 bat back ball bash baffle
       (int) 0
       redis> TRIE.APPROXMATCH key1 ba
        1) bat
        2) back
        3) ball
        4) bash
        5) (nil)
        6) (nil)
        7) (nil)
        8) (nil)
        9) (nil)
        10) (nil)
       redis> TRIE.APPROXMATCH key1 ba 3
        1) bat
        2) back
        3) ball
        4) bash
        5) baffle
        6) (nil)
        7) (nil)
        8) (nil)
        9) (nil)
        10) (nil)
       redis> TRIE.APPROXMATCH key1 ba 2 5
        1) bat
        2) back
        3) ball
        4) bash
        5) baffle
