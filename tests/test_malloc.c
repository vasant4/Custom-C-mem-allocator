#include <stdio.h>
#include "../src/myalloc.h"

int main(void)
{
    void *p = my_malloc(10);
    printf("my_malloc(10) returned %p\n", p);
    my_free(p);
    return 0;
}