#include <stdio.h>
#include "../src/myalloc.h"
#include <unistd.h>

int main(void)
{
    printf("Break before: %p\n", sbrk(0));
    char *a = my_malloc(10);
    char *b = my_malloc(20);
    char *c = my_malloc(30);

    /* fill each block completely with a distinct byte */
    for (int i = 0; i < 10; i++) a[i] = 'A';
    for (int i = 0; i < 20; i++) b[i] = 'B';
    for (int i = 0; i < 30; i++) c[i] = 'C';
    
    /* read it back — if the memory is real, this survives */
    printf("a = %p  first=%c last=%c\n", (void*)a, a[0], a[9]);
    printf("b = %p  first=%c last=%c\n", (void*)b, b[0], b[19]);
    printf("c = %p  first=%c last=%c\n", (void*)c, c[0], c[29]);

    printf("break after:  %p\n", sbrk(0));

    my_free(a); my_free(b); my_free(c);
    printf("Printing all the current blocks details:\n");
    dump_blocks();
    return 0;
}