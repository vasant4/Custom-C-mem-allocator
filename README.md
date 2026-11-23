# Custom C Memory Allocator
This is a personal project where I am building my own memory allocator from scratch in C to learn how dynamic memory and system calls actually work under the hood. 

## What it is
 The latest version v1.1 uses sbrk syscall to allocate the new memory. sbrk simply moves the program break.
 it has malloc and free which has ability to split big empty block to reuse it and coalesce two empty blocks to one and reuse the empty blocks.

## Design decisions

### sbrk vs mmap
sbrk: sbrk moves the program break, which is at the end of heap. we use negative symbol to deallocate/free up memory to OS eg: sbrk(-size).
mmap: Allocates a separate virtual memory area outside the main heap; allocated memory can be released using munmap().
I have used sbrk because it is simpler to understand and manage linearly. There are downsides to using sbrk:
1. Memory can only be returned to the OS from the top of the heap (adjacent to the program break), following a LIFO (Last-In, First-Out) order.
2. If an sbrk(size) is called in between the blocks then it will disturb the block's chain and if sbrk(size) is called for an odd size then it will disturb the alignment. "If anything else moves the break between two of my allocations, it leaves a gap between list neighbours (handled by the contiguity check). If that gap isn't a multiple of 8, every later block is misaligned. The fix is to check that sbrk(0) is a multiple of 8 before growing the heap."
### Bestfit vs firstfit
Bestfit: finds the smallest block that is big enough for requested size.
firstfit: It takes the first free block that is big enough for requested size
I have designed the malloc for firstfit, it traverse the list from g_head forward and selects the blocks whose size is greater than or equal to the requested size(current->is_free == 1 && current->size >= size).

### Block layout
This is what a memory block looks like -

g_head                                          g_tail                  sbrk(0)
▼                                               ▼                       ▼
0x1000     0x1020       0x1080     0x10A0       0x1100     0x1120       0x1180
┌──────────┬────────────┬──────────┬────────────┬──────────┬────────────┐
│ Header A │ Payload A  │ Header B │ Payload B  │ Header C │ Payload C  │
│  (32B)   │ 96B IN USE │  (32B)   │  96B FREE  │  (32B)   │ 96B IN USE │
└──────────┴────────────┴──────────┴────────────┴──────────┴────────────┘
▲                       ▲                       ▲                       ▲
├◄───── next/prev ─────►┴◄───── next/prev ─────►┤                  Program Break
└── prev = NULL                                 └── next = NULL

Each block has a block header and payload 


block_header_t contains four things - 
size_t size - the current, 8-byte aligned capacity of the block's pay load; Users raw size request is rounded up to match the 8-byte alignment. 
int is_free - It tracks the block's allocations Status(0 = in use, 1 = free). when my_free() is called, it marks this as 1 and initiates bidirectional coalescing with adjacent free blocks.  
block_header_t *next - pointer to the next block in heap chain.
block_header_t *prev - pointer to the previous blocks in heap chain.

   +0    ┌─────────────────────────────┐ ◄── header
         │      size_t size (8B)       │ ┐
   +8    ├─────────────────────────────┤ │
         │      int is_free (4B)       │ │
  +12    ├┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┄┤ ├─ block_header_t (32Bytes)
         │        padding (4B)         │ │
  +16    ├─────────────────────────────┤ │
         │  block_header_t *next (8B)  │ │
  +24    ├─────────────────────────────┤ │
         │  block_header_t *prev (8B)  │ ┘
  +32    ╞═════════════════════════════╡ ◄── header + 1 (returned)
         │                             │
         │           PAYLOAD           │
         │   size Bytes (8B aligned)   │
         │                             │
+32+size └─────────────────────────────┘


We append the new blocks at tail so that the list stays in order and a single chain is maintained.

### Blocks Spliting
when my_malloc() is called it traverse the heap chain from g_head to next and if it finds free block then it checks if spliting is worthwhile: current->size - size >= sizeof(block_header_t) + 8 (a minimum of 40 bytes).

We have kept the block spliting to 8 bytes Because: 
1. All allocations are 8-byte aligned, remainders are always multiples of 8, so 1- or 2-byte remainders never occur.
2. if after spliting the blocks if the remaining size is less than < size_of(block_header), then it will be wasted as we cannot use that memory for any other purpose especially if it has neighbouring blocks

BEFORE SPLIT (Total footprint: 160 Bytes | split threshold: >= 32B header + 8B = 40B)
+0                   +32                                                         +160
┌────────────────────┬──────────────────────────────────────────────────────────────┐
│    HEADER (32B)    │                     FREE PAYLOAD (128B)                      │
│ size=128 is_free=1 │                      (available space)                       │
└────────────────────┴──────────────────────────────────────────────────────────────┘
▲                    ▲
current              current + 1


AFTER SPLIT (my_malloc(32) — Total footprint stays 160 Bytes)
                                      new_block carved here
                                      ▼
+0                   +32              +64                  +96                   +160
┌────────────────────┬────────────────┬────────────────────┬────────────────────────┐
│   ORIG HDR (32B)   │  USER PAYLOAD  │   NEW HDR (32B)    │ LEFTOVER PAYLOAD (64B) │
│ size=32  is_free=0 │   (32 Bytes)   │ size=64  is_free=1 │   (available space)    │
└────────────────────┴────────────────┴────────────────────┴────────────────────────┘
▲                    ▲                ▲                    ▲
current              current + 1      new_block            new_block + 1
                     (returned)

Links: current ◄──► new_block ◄──► old_next
Notes: Updates new_block->prev = current, sets old_next->prev = new_block (if old_next != NULL), and updates g_tail = new_block if current was the tail.

### Coalescing
- Coalescing operation is performed in my_free(), every time a block is freed the my_free()function will update the block's is_free status to 1 and it checks the neighbouring blocks is_free status and merges it.

BEFORE COALESCING (Forward merge: header + next_block — Total footprint: 192 Bytes)
+0                   +32                  +96                  +128              +192
┌────────────────────┬────────────────────┬────────────────────┬────────────────────┐
│    HEADER (32B)    │      PAYLOAD       │  NEXT HEADER (32B) │    NEXT PAYLOAD    │
│ size=64  is_free=1 │     (64 Bytes)     │ size=64  is_free=1 │     (64 Bytes)     │
└────────────────────┴────────────────────┴────────────────────┴────────────────────┘
▲                                         ▲
header                                    next_block (adjacent & free — absorbed)


AFTER COALESCING (Merged into one contiguous block — Total footprint stays 192 Bytes)
+0                   +32                                                         +192
┌────────────────────┬──────────────────────────────────────────────────────────────┐
│    HEADER (32B)    │                 COMBINED PAYLOAD (160 Bytes)                 │
│ size=160 is_free=1 │   Payload (64B)    ┆ Old Next Hdr (32B) ┆ Next Payload (64B) │
└────────────────────┴──────────────────────────────────────────────────────────────┘
▲
header  (new size = 64 + 32 + 64 = 160 Bytes)

Check: (char *)header + sizeof(block_header_t) + header->size == (char *)header->next
Links: header <--> next_block->next  (updates next->prev = header, or g_tail = header)
Note:  my_free() then runs the same check backward against prev_block, giving full
       3-way coalescing when both neighbors are free and physically adjacent.

### Key Design Decisions & Bugs Solved ("Why We Did It This Way")
* **Physical Contiguity Check Before Coalescing:** Verifies that two adjacent free blocks in the linked list are also physically touching in raw memory ((char *)curr + sizeof(header) + curr->size == (char *)curr->next) before merging. This ensures that any foreign memory placed between blocks (such as external sbrk calls from libc) is preserved and not swallowed or corrupted.
* **Two-Stage Overflow Guard (`SIZE_MAX` & `PTRDIFF_MAX`):**A pair of arithmetic checks that safely reject requests approaching memory limits: Stage 1 (size > SIZE_MAX - 7) blocks integer wrap-around during 8-byte alignment, and Stage 2 (size > PTRDIFF_MAX - sizeof(header)) prevents the total request from overflowing sbrk's signed intptr_t parameter.
* **Non-Allocating Debugging I/O (write() in `dump_blocks`):** We used Non-Allocating I/O for debugging. Nothing that allocates cannot be in allocator. printf allocates internal formatting buffers on the heap. Calling printf inside allocator routines like dump_blocks risks recursive calls to my_malloc, causing stack overflows or deadlocks. That is why we use write() a direct POSIX system call wrapper that performs no dynamic memory allocation.
* **Defensive Invariant Checker (`check_invariants`):** when check_invariants() is called it traverses the heap to enforce structural rules—verifying that every block's size is a non-zero multiple of 8, non-overlapping block bounds, OS break limits (sbrk(0)), g_tail accuracy, and doubly-linked backlink consistency (next->prev == current).

#### Project Structure
* `src/` — core allocator implementation and header
* `tests/` — assertion-based test suite
* `warmups/` — C pointer/padding/linked-list warmup exercises
* `Makefile` — build targets

### Known Limitations & Roadmap
This is v1.1 and a work in progress.
* **Current limitations:** $O(N)$ linear first-fit scan, single-threaded (not thread-safe yet), 8-byte alignment (vs. 16-byte ABI), `sbrk`-only (doesn't return freed memory to OS).
* **Next steps:** `my_calloc` & in-place `my_realloc`, segregated free lists, `mmap`/`munmap` for large blocks, thread safety, `LD_PRELOAD` support.
 Next up, I am working on `calloc` and `realloc`, followed by multithreaded operation for now.


## How to run it:
Just run `make test_malloc` to compile and `./test_malloc` to run the tests. You can also run it with `valgrind --error-exitcode=1 ./test_malloc` to check for invalid memory accesses past the program break (`sbrk(0)`), uninitialized reads, and process-level memory faults 