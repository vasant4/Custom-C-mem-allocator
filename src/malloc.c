#include "../src/myalloc.h"
#include <unistd.h>

typedef struct block_header{
	size_t size;
	int is_free;
	struct block_header *next;
}block_header_t;

void *my_malloc(size_t size)
{
	size = (size + 7) & ~((size_t)7);
	void *brkop = sbrk((sizeof(block_header_t) + size));
	
	if (brkop == (void *) -1){
		return NULL;
	}
	// 3. Slap the blueprint on the raw memory!
	block_header_t *header = (block_header_t *)brkop;
	header->size = size;
    header->is_free = 0; // 0 means "in use"
    header->next = NULL;
	return (header + 1);
}

void my_free(void *ptr)
{
	if (ptr == NULL) return; //check for NULL pointers
	block_header_t *header = (block_header_t*)ptr-1;
	header->is_free=1;
	(void)ptr;
}