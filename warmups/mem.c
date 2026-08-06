#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <stddef.h> // Required for size_t

void *my_memcpy(void *dst, const void *src, size_t n) {
    // 1. Cast the void pointers to 1-byte unsigned char pointers
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    
    // 2. Loop n times, copying one byte at a time
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    
    // 3. Return the original, unmodified dst pointer
    return dst;
}
#include <stddef.h> // Required for size_t

void *my_memset(void *s, int c, size_t n) {
    // 1. Cast the void pointer to a 1-byte unsigned char pointer
    unsigned char *ptr = (unsigned char *)s;
    
    // 2. Cast the integer 'c' down to a 1-byte unsigned char
    unsigned char value = (unsigned char)c;
    
    // 3. Loop n times, writing the 1-byte value into memory
    for (size_t i = 0; i < n; i++) {
        ptr[i] = value;
    }
    
    // 4. Return the original pointer
    return s;
}
// (Assume my_memcpy and my_memset are pasted here)

int main() {
    printf("--- Testing my_memset ---\n");
    
    // Use Case 1: Initializing a text buffer
    char text_buffer[10];
    // Fill the first 9 bytes with the letter 'A'
    my_memset(text_buffer, 'A', 9);
    // Manually add the null terminator so we can print it safely
    text_buffer[9] = '\0'; 
    printf("Text buffer: %s\n", text_buffer);

    // Use Case 2: "Zeroing out" an integer array
    // This is the most common use of memset in the real world
    int numbers[5] = {99, 99, 99, 99, 99};
    // sizeof(numbers) calculates exactly how many bytes the whole array takes (5 * 4 = 20 bytes)
    my_memset(numbers, 0, sizeof(numbers)); 
    
    printf("Zeroed numbers: ");
    for(int i = 0; i < 5; i++) {
        printf("%d ", numbers[i]); // Will print: 0 0 0 0 0
    }
    printf("\n\n");


    printf("--- Testing my_memcpy ---\n");

    // Use Case 3: Copying an array of integers
    int src_arr[4] = {10, 20, 30, 40};
    int dest_arr[4]; // Currently holds random garbage memory

    // Copy the raw memory from src to dest
    my_memcpy(dest_arr, src_arr, sizeof(src_arr));

    printf("Copied integers: ");
    for(int i = 0; i < 4; i++) {
        printf("%d ", dest_arr[i]); // Will print: 10 20 30 40
    }
    printf("\n");

    // Use Case 4: Copying a custom struct
    typedef struct {
        int id;
        double salary;
    } Employee;

    Employee emp1 = {101, 75000.50};
    Employee emp2; // Empty

    // Memcpy doesn't care what the data is, it just copies the raw bytes!
    my_memcpy(&emp2, &emp1, sizeof(Employee));
    
    printf("Copied Employee ID: %d, Salary: %.2f\n", emp2.id, emp2.salary);

    return 0;
}