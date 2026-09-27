#include "../src/myalloc.h"
#include <unistd.h>
#include <stdio.h>
#include <stdint.h> // for uintptr_t
typedef struct block_header{
	size_t size;
	int is_free;
	struct block_header *next;
}block_header_t;

static block_header_t *g_head = NULL;
static block_header_t *g_tail = NULL;
/*
Helper's for dump_block
*/
static void write_str(const char * s){
    size_t len = 0;
    // Count characters until we hit the null-terminator
    while (s[len] != '\0') {
        len++;
    }
    // Write exactly 'len' bytes to file descriptor 1 (stdout)
    write(1, s, len);
}

static void write_hex(void * ptr){
	write_str("0x");
	if(ptr==NULL){
		write_str("0");
		return;
	}
	uintptr_t n = (uintptr_t)ptr;
	char buffer[20];
	int pos = 19;
	const char *hex_chars = "0123456789abcdef";
	while (n>0){
		buffer[pos]= hex_chars[n%16]; /*divide it by 16 instead of 10. Because n % 16 gives us the last digit (a remainder between 0 and 15) */
        pos--;
        n = n / 16; /* divide by 16 to chop of the last digit*/
	}
	write(1, &buffer[pos + 1], 19 - pos);
}

static void write_dec(size_t n){
	if (n == 0) {
	write(1, "0", 1);
	return;
	}

    char buf[24]; 
    
    int pos = 23; 
    while (n > 0) {

        buf[pos] = '0' + (n % 10);
        pos--;       // Move backwards in our buffer
        n = n / 10;  // Chop off the last digit
    }

	write(1, &buf[pos + 1], 23 - pos);
}

void dump_blocks(void){
	block_header_t *current = g_head;
	int i = 0;
	while(current != NULL){
		// printf("block %d: addr=%p size=%zu free=%d\n", i, (void *)current, current->size, current->is_free);
		write_str("block:");
		write_dec(i);
		// write_str("\n");

		write_str(" address: ");
		write_hex((void *) current);
		// write_str("\n");

		write_str(" size: ");
		write_dec(current->size);
		// write_str("\n");

		write_str("free: ");
		write_dec(current->is_free);
		write_str("\n");
		current=current->next;
		i++;
	}
}

int check_invariants(void){
	if(g_head==NULL){
		return 0; // Empty List.
	}
	block_header_t *current = g_head;
	void *program_break = sbrk(0);
	while(current != NULL){
		//Rule 3 - check if the size is > 0 and size is divisible by 8
		if(current->size == 0 || (current->size%8)!=0){
			return 3;
		}
		//rule 4  payload <= brk, and size <= (brk - payload)
		char * current_end_block = (char *)current + sizeof(block_header_t) + current->size;
		if(((char*)current_end_block) > ((char *)program_break)){
			return 4;
		}
		//rule 1  if next exists: end of this block <= next
		if(current->next != NULL){
			if (((char *)current_end_block) > ((char *)current->next)){
				return 1;
			}
		}else{
		/*last = current
		rule 2  last == g_tail  */
			if(current != g_tail){
				return 2;
			}
		}
		current = current->next;
	}
	return 0; //If everything passed
}

static block_header_t * split_blocks(size_t size,block_header_t *current){
	
	/*Creation of new block and updating it's contents*/
	block_header_t * new_block = (block_header_t *)((char *)current + sizeof(block_header_t)+ size);
	new_block->size = current->size - size - sizeof(block_header_t);		/*Update new block's size*/
	new_block->is_free = 1; 	/*Update is_free to 1 - means free*/
	new_block->next = current->next;
	if(g_tail == current){
		g_tail = new_block; /* Update the tail/last block details in g_tail*/
	}
	/*Updting the new block size and poitner information of exsiting*/
	current->size = size;
	current->next = new_block;
	return current;
}

static block_header_t * get_free_blocks(size_t size){ 
	block_header_t* current = g_head;
	block_header_t* new_split_block;
	while(current != NULL){
		size_t remaining_space = current->size - size;
		if(current->is_free == 1 && current->size >= size){ 
			if(remaining_space >= (sizeof(block_header_t) + 8)){
				new_split_block = split_blocks(size,current);/* Function to split the blocks if the condition matches*/
				new_split_block->is_free=0;
				return new_split_block;
			}
			current->is_free = 0;
			return current;
		}
		current = current-> next;
	}
	return NULL;
}

void *my_malloc(size_t size)
{	
	if (size == 0){
		return NULL;
	}
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