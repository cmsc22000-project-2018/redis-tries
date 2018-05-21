CC = gcc
AR = ar 
CFLAGS = -std=c99 -fPIC -Wall -Wextra -O2 -g -I./include/
LDFLAGS = -shared
RM = rm -f
DYNAMIC_LIB = libtrie.so
LIBS = ${DYNAMIC_LIB}
LDLIBS = -lm

SRCS = src/trie.c
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: ${LIBS}

$(DYNAMIC_LIB): $(OBJS)
	$(CC) -shared -o $@ $^ $(LDLIBS)

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< -MT $(patsubst %.d,%.o,$@) > $@

-include $(SRCS:.c=.d)

.PHONY: clean tests
clean:
	-${RM} ${LIBS} ${OBJS} $(SRCS:.c=.d)


