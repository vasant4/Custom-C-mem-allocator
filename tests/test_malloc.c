#include <stdio.h>
#include<stdint.h>
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
    check(check_invariants() == 0, "List valid after exact fit reuse");
    my_free(p4);
    dump_blocks();
    // ... you will continue adding test cases here ...

    // 3. Exit with the number of failures
    return failures ? 1 : 0;
}