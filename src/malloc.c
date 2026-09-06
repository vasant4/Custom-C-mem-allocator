#include "../src/myalloc.h"
#include <unistd.h>
#include <stdio.h>
typedef struct block_header{
	size_t size;
	int is_free;
	struct block_header *next;
}block_header_t;

static block_header_t *g_head = NULL;
static block_header_t *g_tail = NULL;

void dump_blocks(void){
	block_header_t *current = g_head;
	int i = 0;
	while(current != NULL){
		printf("block %d: addr=%p size=%zu free=%d\n", i, (void *)current, current->size, current->is_free);
		current=current->next;
		i++;
	}
}

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
	if (g_head == NULL) {
		g_head = g_tail = header;
	} else {
    	g_tail->next = header;
    	g_tail = header;
	}
	return (header + 1);
}

void my_free(void *ptr)
{
	if (ptr == NULL) return; //check for NULL pointers
	block_header_t *header = (block_header_t*)ptr-1;
	header->is_free=1;
}