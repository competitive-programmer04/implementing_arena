#ifndef _ARENA_H_
#define _ARENA_H_
#define MAX_OBJ 100
#include<stdint.h>
#include<stdio.h>

typedef struct Arena{
    uint8_t *base; // pointing to the start of arena
    size_t capacity; // capacity of the arena
    size_t offset; // what is the offset in arena
    size_t commit_position; // it will tell how much virtual memory is committed in physical memory
    int cnt; // it will count the number of objects allocated
    size_t check_size[MAX_OBJ]; 
}Arena;

typedef struct FreeNode{
    struct FreeNode *next;
}FreeNode;

typedef struct Pool{
    Arena *A;
    size_t object_size;
    FreeNode *free_list;
}Pool;

void ArenaAlloc(void *A); // for creating arena
void ArenaRelease(void *A); // to free arena
void* ArenaPush(void *A, size_t s); // for creating a buffer in arena of size s (in bytes) from the top
void ArenaPop(void *A); // removing the topmost/last object from arena 
void ArenaPopFrom(void *A, int pos); // removing the objects from arena from the topmost position till pos
void ArenaClear(void *A); // clearing arena
void InitializePool(Pool *pool, Arena *A, size_t size, FreeNode *fn); // initailizing pool objec
void* PoolAlloc(Pool *pool); // allocating pool of memory
void PoolFree(Pool *pool, void *ptr); // freeing pool of memory 
#endif
