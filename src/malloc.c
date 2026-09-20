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

block_header_t * split_blocks(size_t size,block_header_t *current){
	
	/*Creation of new block and updating it's contents*/
	block_header_t * new_block = (block_header_t *)((char *)current + sizeof(block_header_t)+ size);
	new_block->size = current->size - size - sizeof(block_header_t);		/*Update new block's size*/
	new_block->is_free = 1; 	/*Update is_free to 0 - means free*/
	new_block->next = current->next;

	/*Updting the new block size and poitner information of exsiting*/
	current->size = size;
	current->next = new_block;
	return current;
}

block_header_t * get_free_blocks(size_t size){ 
	block_header_t* current = g_head;
	block_header_t* new_split_block;
	while(current != NULL){
		size_t remaining_space = current->size - size;
		if(current->is_free == 1 && current->size >= size && remaining_space >= (sizeof(block_header_t) + 8)){ 
			new_split_block = split_blocks(size,current);/* Function to split the blocks if the condition matches*/
			new_split_block->is_free=0;
			return(new_split_block +1);
		}
		current = current-> next;
	}
	return NULL;
}

void *my_malloc(size_t size)
{
	size = (size + 7) & ~((size_t)7);
	block_header_t * free_blocks;
	free_blocks = get_free_blocks(size);
	if(free_blocks){
		return (free_blocks+1);
	}
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