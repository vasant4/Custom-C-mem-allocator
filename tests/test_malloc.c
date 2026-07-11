#include <stdio.h>

#include "../src/malloc.h"

int main(void)
{
    void *p = my_malloc(10);
    printf("my_malloc(10) = %p\n", p);
    my_free(p);
    return 0;
}
