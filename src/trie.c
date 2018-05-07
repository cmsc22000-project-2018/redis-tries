int trie_search(char* word, trie_t *t)
{
    int len;
    trie_t* curr
    trie_t** next;

    len = strlen(word);
    curr = t;
    next = t->children;

    for (int i=0; i<len; i++) {
        curr = next[word[i]];

        if (curr == NULL)
            return 0;

        next = next[word[i]]->children;
    }

    if (curr->is_word == 1) 
        return 1;

    return -1;
}

