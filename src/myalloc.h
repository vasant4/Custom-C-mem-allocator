#ifndef MYALLOC_H
#define MYALLOC_H

#include <stddef.h>

void *my_malloc(size_t size);
void my_free(void *ptr);
void dump_blocks(void);

#endif
