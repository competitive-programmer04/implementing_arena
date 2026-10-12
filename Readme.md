# C Arena & Pool Memory Allocator

A lightweight, high-performance C memory management library built directly on Linux virtual memory system calls (`mmap` and `mprotect`). 

This project implements a **Chained Virtual-Address-Space Arena Allocator** combined with an **Intrusive Pool Allocator**, providing constant-time $O(1)$ allocations, zero external memory fragmentation, hardware alignment, and sub-lifetime recycling.

---


## Key Architectural Features

* **Virtual Address Space Reservation:** Uses Linux `mmap()` with `PROT_NONE` to reserve a 64MB contiguous virtual address space upfront without allocating physical RAM.
* **Lazy Page Committing:** Automatically commits 4KB physical RAM pages on-demand using `mprotect(..., PROT_READ|PROT_WRITE)` as allocation offsets cross page boundaries.
* **Hardware Word Alignment:** Enforces strict 8-byte CPU memory alignment using fast bitwise operations (`(offset + 7) & ~7`).
* **Sub-Lifetime Stack Rollbacks:** Supports fine-grained memory pop operations (`ArenaPop`, `ArenaPopFrom`) and single-step resetting (`ArenaClear`).
* **Intrusive Pool Allocator:** Built on top of the Arena, allowing out-of-order element recycling (`PoolFree`) for uniform object sizes via a linked list (`FreeNode`).

---


## Memory Layout Architecture

```text
64MB Virtual Memory Reservation (mmap with PROT_NONE)
┌───────────────────────────────────────────┬───────────────────────────────────────────┐
│ Committed Physical RAM (mprotect)         │ Uncommitted Virtual Address Space         │
│ ┌─────────────┬─────────────┬───────────┐ │ (PROT_NONE - Consumes 0 Physical RAM)     │
│ │ Object 1    │ Object 2    │ Unused    │ │                                           │
│ │ (32 Bytes)  │ (64 Bytes)  │ Committed │ │                                           │
│ └─────────────┴─────────────┴───────────┘ │                                           │
└───────────────────────────────────────────┴───────────────────────────────────────────┘
  ▲                           ▲             ▲                                           ▲
  │                           │             │                                           │
 base                       offset    commit_position                                capacity
 (0)                        (96B)         (4096B)                                     (64MB)
 ```

 ## API References

 Arena Core Functions

 - void ArenaAlloc(void *A)
 Allocates a contiguous 64MB block in process virtual address space using mmap with flag PROT_NONE

 - void ArenaRelease(void *A)
 Unmaps the entire virtual address space and returns physical memory to the kernel via munmap.
 
 - void* ArenaPush(void *A, size_t s)
 Pushes an 8-byte aligned allocation to the top of the arena. Commits physical 4KB pages via mprotect as needed.

 - void ArenaPop(void *A)
 Pops the most recent allocation from the top of the arena stack.

 - void ArenaPopFrom(void *A, int pos)
 Rolls back the arena offset to a specific recorded allocation index.

 - void ArenaClear(void *A)
 Resets the arena's allocation offset to zero  in O(1) time while keeping the physical pages committed for reuse.

 Pool Allocator functions

 - void InitializePool(Pool *p, Arena *A, size_t size, FreeNode *fn)
 Initializes a Pool of memory to keep track of the objects.

 - void* PoolAlloc(Pool *pool)
Recycles an empty slot from pool->free_list if available; otherwise, carves a fresh slot from the underlying Arena using ArenaPush.

- void PoolFree(Pool *pool, void *ptr)
Recycles memory in O(1) time by casting ptr to FreeNode and pushing it to the head of free_list.


## Build and Usage

1. File Structure

```text
├── arena.h       # Header file containing Arena, Pool, and FreeNode declarations
├── arena.c       # Core C implementation file
├── main.c        # Example program
└── Makefile      # GNU Make build script
```
2. Compilation

```bash
make
```
It will generate the executable file arena

To use it

```bash
./arena
```
Finally to remove it 

```bash
make clean
```

### License

MIT License. Free to use for academic and open-source applications.
