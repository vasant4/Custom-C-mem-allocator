#include "../src/myalloc.h"
#include <unistd.h>

void *my_malloc(size_t size)
{
	void *brkop = sbrk(size);
	if (brkop == (void *) -1){
		return NULL;
	}
	return brkop;
}

void my_free(void *ptr)
{
	// Bump allocators only move the break forward. 
    // They cannot reclaim or free individual blocks of memory.
	(void)ptr;
}