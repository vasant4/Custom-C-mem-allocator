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
    
    /* === Test 1: my_free(NULL) safety (should not crash or corrupt list) === */
    my_free(NULL);
    check(1, "my_free(NULL) did not crash"); 
    check(check_invariants() == 0, "List valid after my_free(NULL)");


    /* === Test 2: Basic allocation and 8-byte alignment === */
    char *p1 = my_malloc(10); // Should align to 16
    check(p1 != NULL, "malloc returned a valid pointer");
    check(check_invariants() == 0, "List valid after first malloc");


    /* === Test 3: Exact-fit block reuse === */
    // We use a completely new size so it doesn't accidentally reuse an old block
    char *p2 = my_malloc(100); 
    my_free(p2);
    char *p3 = my_malloc(100); 
    check(p2 == p3, "Freed block of 100 bytes was perfectly reused");
    check(check_invariants() == 0, "List valid after exact fit reuse");


    /* === Test 4: Overflow guards (SIZE_MAX edge cases) === */
    char *p4 = my_malloc(SIZE_MAX);
    check(check_invariants() == 0, "Checking SIZE_MAX, List valid after exact fit reuse");
    my_free(p4);
    
    check(my_malloc(SIZE_MAX) == NULL, "Checking SIZE_MAX returns NULL");

    check(my_malloc(SIZE_MAX - 24) == NULL, "Checking SIZE_MAX-24 returns NULL");

    check(my_malloc(SIZE_MAX - 40) == NULL, "Checking SIZE_MAX-40 returns NULL");


    /* === Test 5: Coalescing - 2-way adjacent block merge === */
    // Allocate two distinct blocks
    char *a = my_malloc(64);
    char *b = my_malloc(64);
    my_free(a);
    my_free(b); // Should merge a and b into one ~160-byte free block

    // Request a size that only fits if a and b merged:
    char *c = my_malloc(128);
    check(c == a, "Coalescing: merged adjacent blocks reused for larger request");
    check(check_invariants() == 0, "List valid after coalescing");


    /* === Test 6: Coalescing - 3-way merge (left + middle + right) === */
    char *x = my_malloc(64);
    char *y = my_malloc(64);
    char *z = my_malloc(64);
    my_free(x);
    my_free(z);
    my_free(y); // Freeing y should merge x, y, and z into one block

    char *big = my_malloc(230);
    check(big == x, "Coalescing: 3-way merge correctly combined x, y, z");
    check(check_invariants() == 0, "List valid after 3-way merge");


    /* === Test 7: Coalescing - Foreign memory gap overwrite check === */
    char *n1 = my_malloc(256);
    char *foreign_check = (char *)sbrk(86);
    for (int i = 0; i < 86; i++) foreign_check[i] = 'Z'; // Fill all 86 foreign bytes

    char *n2 = my_malloc(256);
    my_free(n1);
    my_free(n2); // MUST NOT merge across the 86-byte gap!

    // Now trigger the bug: ask for a block that only fits if n1 & n2 merged
    char *n3 = my_malloc(400);
    check(n3 != n1, "Coalescing: did not merge across foreign gap");

    // Fill the entire new allocation with 'X'
    for (int i = 0; i < 400; i++) n3[i] = 'X';

    // Verify NONE of the 86 foreign bytes were clobbered
    int foreign_intact = 1;
    for (int i = 0; i < 86; i++) {
        if (foreign_check[i] != 'Z') {
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