CC = gcc
AR = ar 
CFLAGS = -fPIC -Wall -Wextra -O2 -g -I./include/
LDFLAGS = -shared
RM = rm -rf
DYNAMIC_LIB = libtrie.so
LIBS = ${DYNAMIC_LIB}
LDLIBS = -lm
SRCS = src/trie.c src/suggestion.c src/viz.c
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${LIBS}

$(DYNAMIC_LIB): $(OBJS)
	$(CC) -shared -o $@ $^ $(LDLIBS)

main: $(OBJS) main.o
	$(CC) $(CFLAGS) $(SRCS) main.c -o main

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< -MT $(patsubst %.d,%.o,$@) > $@

tests: $(LIBS)
	make -C ./tests

include $(SRCS:.c=.d)

.PHONY: clean tests
clean:
	-${RM} ${LIBS} ${OBJS} $(SRCS:.c=.d)
	make -C ./tests clean
