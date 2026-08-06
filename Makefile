CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinclude

all: test_malloc list

test_malloc: src/malloc.c tests/test_malloc.c
	$(CC) $(CFLAGS) -o test_malloc src/malloc.c tests/test_malloc.c

list: warmups/list.c
	$(CC) $(CFLAGS) -o list warmups/list.c

padding: warmups/StructPadding.c
	$(CC) $(CFLAGS) -o padding warmups/StructPadding.c

strings: warmups/In_Place_String_Reversal.c
	$(CC) $(CFLAGS) -o strings warmups/In_Place_String_Reversal.c

mem: warmups/mem.c
	$(CC) $(CFLAGS) -o mem warmups/mem.c

test: all
	./test_malloc
	./list
	./mem
	./padding
	./strings

clean:
	rm -f test_malloc list

.PHONY: all test clean
