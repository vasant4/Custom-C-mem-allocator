CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinclude

all: test_malloc list

test_malloc: src/malloc.c tests/test_malloc.c
	$(CC) $(CFLAGS) -o test_malloc src/malloc.c tests/test_malloc.c

list: warmups/list.c
	$(CC) $(CFLAGS) -o list warmups/list.c

test: all
	./test_malloc
	./list

clean:
	rm -f test_malloc list

.PHONY: all test clean
