CC     := gcc
CFLAGS := -Wall -Wextra -g

all: malloc.o

malloc.o: src/malloc.c src/malloc.h
	$(CC) $(CFLAGS) -c src/malloc.c -o malloc.o

test_malloc: tests/test_malloc.c src/malloc.c src/malloc.h
	$(CC) $(CFLAGS) tests/test_malloc.c src/malloc.c -o test_malloc

test: test_malloc
	./test_malloc

clean:
	rm -f malloc.o test_malloc test_malloc.exe

.PHONY: all test clean
