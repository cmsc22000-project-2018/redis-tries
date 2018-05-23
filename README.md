# Redis-tries Module #

Goal: Add and implement support for prefix tries in Redis

Tries: A trie, also known as a digital tree, radix tree or prefix tree, is an efficient search tree where keys are strings. The name for the data structure comes from the word reTrieval. Using a trie, search complexities can be brought to O(n) where n is the length of the key. Each node of the Trie has a character, indication whether the node represents the end of a word, and multiple branches. Each branch represents a possible character for the path of the key.

Redis: Redis is an open-source, in-memory remote database that offers versatile modules and support for common data structures like hashes, lists and bitmaps. It serves as a popular platform for project development.

We will be implementing Redis support for the prefix trie first with basic functionality of searching, inserting and deleting and will optimize further based on the needs of the other developing functionalities like autocomplete, spellcheck and text searching.

## Setting Up ##

First clone this respository if you haven't already anywhere on your local system.

Inside the module directory, type "make all" into the command line to build the trie.so library.

Print the working directory and write it down, appending "trie.so" to the end of the path. So if my name is Dustin and my redis-tries directory is on my Desktop, I should have written down /Users/Dustin/Desktop/redis-tries/module/trie.so

Before you begin running the Redis server, make sure to check the src directory in redis-stable for a file called "dump.rdb" if it exists, make sure to delete it before running the server every time you decide to run the server. This is because we have not implemented the data persistence functions.

Assuming you have Redis fully installed and functional (instructions here if you don't: https://github.com/cmsc22000-project-2018/redis-tries/wiki/Design-Document-v1), run the Redis server with "redis-server" in the redis-stable directory.

In a separate window, run the Redis client with "redis-cli".

Inside of the Redis client, type in "MODULE LOAD [Path]" where [Path] is the previously copied path to your trie.so library. So in the context of my previous example, "/Users/Dustin/Desktop/redis-tries/module/trie.so"

Your module should be loaded up. Check your server log for an indication that the module trie123az has been loaded. Once good, check out our wiki page https://github.com/cmsc22000-project-2018/redis-tries/wiki/Redis-Trie-Commands for documentation on the available commands.
