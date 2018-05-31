FROM gcc

COPY module /tmp/module
COPY RedisModulesSDK /tmp/RedisModulesSDK

RUN make -C /tmp/module/

FROM redis

COPY --from=0 /tmp/module/trie.so /usr/local/lib

CMD ["redis-server", "--loadmodule", "/usr/local/lib/trie.so"]
