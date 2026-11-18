#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "../src/myalloc.h"

// 1. Setup the test infrastructure
static int failures = 0;

static void check(int ok, const char *what) {
    printf("%s  %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) failures++;
}

int main(void) {
    // 2. Write your tests!
    
    // Test Case: my_free(NULL) shouldn't crash
    my_free(NULL);
    check(1, "my_free(NULL) did not crash"); 
    check(check_invariants() == 0, "List valid after my_free(NULL)");

    // Test Case: Basic Allocation and 8-byte alignment
    char *p1 = my_malloc(10); // Should align to 16
    check(p1 != NULL, "malloc returned a valid pointer");
    check(check_invariants() == 0, "List valid after first malloc");

    // Test Case: Reuse exact fit
    // We use a completely new size so it doesn't accidentally reuse an old block
    char *p2 = my_malloc(100); 
    my_free(p2);
    char *p3 = my_malloc(100); 
    check(p2 == p3, "Freed block of 100 bytes was perfectly reused");
    check(check_invariants() == 0, "List valid after exact fit reuse");
    
    char *p4 = my_malloc(SIZE_MAX);
    check(check_invariants() == 0, "Checking SIZE_MAX, List valid after exact fit reuse");
    my_free(p4);
    
    check(my_malloc(SIZE_MAX) == NULL, "Checking SIZE_MAX returns NULL");

    check(my_malloc(SIZE_MAX - 24) == NULL, "Checking SIZE_MAX-24 returns NULL");

    check(my_malloc(SIZE_MAX - 40) == NULL, "Checking SIZE_MAX-40 returns NULL");

    // Allocate two distinct blocks
    char *a = my_malloc(64);
    char *b = my_malloc(64);
    my_free(a);
    my_free(b); // Should merge a and b into one ~160-byte free block

    // Request a size that only fits if a and b merged:
    char *c = my_malloc(128);
    check(c == a, "Coalescing: merged adjacent blocks reused for larger request");
    check(check_invariants() == 0, "List valid after coalescing");


    char *m1 = my_malloc(64);
    char *foreign = (char *)sbrk(86);
    for (int i = 0; i < 86; i++) foreign[i] = 'Z'; // Fill all 86 foreign bytes

    char *m2 = my_malloc(64);
    my_free(m1);
    my_free(m2); // MUST NOT merge across the 86-byte gap!

    // Now trigger the bug: ask for a block that only fits if m1 & m2 merged
    char *m3 = my_malloc(128);
    check(m3 != m1, "Coalescing: did not merge across foreign gap");

    // Fill the entire new allocation with 'X'
    for (int i = 0; i < 128; i++) m3[i] = 'X';

    // Verify NONE of the 86 foreign bytes were clobbered
    int foreign_intact = 1;
    for (int i = 0; i < 86; i++) {
        if (foreign[i] != 'Z') {
            foreign_intact = 0;
            break;
        }
    }
    check(foreign_intact, "Coalescing: foreign memory between blocks was not corrupted");
    check(check_invariants() == 0, "List valid when blocks have memory gaps");
    // ... continue adding test cases here ...

    // 3. Exit with the number of failures
    return failures ? 1 : 0;
}