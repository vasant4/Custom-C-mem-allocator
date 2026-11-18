# Custom C Memory Allocator

This is a personal project where I am building my own memory allocator from scratch in C to learn how dynamic memory and system calls actually work under the hood. 

Right now, it manages the heap using the `sbrk` system call and tracks memory chunks using a linked list of block headers. It handles `my_malloc` by searching for free memory using a first-fit strategy, and if a free block is larger than needed, it splits the block to avoid wasting space. Memory can be returned using `my_free`, which marks blocks available for reuse. I also made sure all allocations are 8-byte aligned, added integer overflow protection for `SIZE_MAX`, and wrote a custom `dump_blocks` function that uses raw `write` syscalls instead of `printf` to avoid standard library dependencies. To keep things solid, there's an internal `check_invariants()` checker and an automated test suite that passes Valgrind with 0 errors.

This is v1.0 and a work in progress. Next up, I am working on block coalescing on a separate branch, followed by `calloc` and `realloc` for now.

### How to run it:
Just run `make test_malloc` to compile and `./test_malloc` to run the tests. You can also run it with `valgrind --error-exitcode=1 ./test_malloc` to verify memory safety.