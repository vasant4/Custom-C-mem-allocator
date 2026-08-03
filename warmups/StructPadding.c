#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>


int main(void) {
    struct A { char a;  int b;    char c; };
    struct B { int  b;  char a;   char c; };
    struct C { char a;  char c;   int  b; };
    struct D { char a;  double d; char c; };

    printf("Here is the size of Struct A %lu\n", sizeof(struct A));
    printf("Here is the offset of char a in Struct A %lu\n", offsetof(struct A, a));
    printf("Here is the offset of int b in Struct A %lu\n", offsetof(struct A, b));
    printf("Here is the offset of char c in Struct A %lu\n", offsetof(struct A, c));

    printf("               \n");

    printf("Here is the size of Struct B %lu\n", sizeof(struct B));
    printf("Here is the offset of int a in Struct B %lu\n", offsetof(struct B, b));
    printf("Here is the offset of char b in Struct B %lu\n", offsetof(struct B, a));
    printf("Here is the offset of char c in Struct B %lu\n", offsetof(struct B, c));

    printf("               \n");

    printf("Here is the size of Struct C %lu\n", sizeof(struct C));
    printf("Here is the offset of char a in Struct C %lu\n", offsetof(struct C, a));
    printf("Here is the offset of char b in Struct C %lu\n", offsetof(struct C, b));
    printf("Here is the offset of int c in Struct C %lu\n", offsetof(struct C, c));

    printf("               \n");

    printf("Here is the size of Struct D %lu\n", sizeof(struct D));
    printf("Here is the offset of char a in Struct D %lu\n", offsetof(struct D, a));
    printf("Here is the offset of double d in Struct D %lu\n", offsetof(struct D, d));
    printf("Here is the offset of char c in Struct D %lu\n", offsetof(struct D, c));

    return 0;
}