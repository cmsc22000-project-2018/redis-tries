LDFLAGS = -shared -Bsymbolic
CFLAGS = -fPIC -g -lc -lm -W -Wall -fno-common -ggdb -std=gnu99 -O2
CC = gcc
SRCS = trie.c
OBJS = $(SRCS:.c=.o)
BINS = trie.so

all: $(BINS)

$(BINS): $(OBJS)
	$(LD) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -rf *.o *.so
