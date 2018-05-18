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
            return NOT_IN_TRIE;

        next = next[word[i]]->children;
    }

    if (curr->is_word == 1) 
        return IN_TRIE;

    return PARTIAL_IN_TRIE;
}

