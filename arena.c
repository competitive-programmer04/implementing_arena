#include<stdio.h>
#include<sys/mman.h>
#include<string.h>
#include<assert.h>
#include<stdint.h>
#include "arena.h"

#define ARENA_SIZE (64*1024*1024) // 64mb
#define WORD_LEN 8
#define PAGE_SIZE 4*1024

void ArenaAlloc(void *A){
    Arena *new_A = (Arena*)A;
    void *ptr = mmap(NULL, ARENA_SIZE, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    // PROT_NONE flag will reserved the virtual memory i.e. process can not access this virtual memory 
    assert(ptr != MAP_FAILED && "Failed to allocate arena");
    new_A->base = (uint8_t*)ptr;
    new_A->capacity = ARENA_SIZE;
    new_A->offset = 0;
    new_A->commit_position = 0;
    new_A->cnt = 0;
    memset(new_A->check_size, 0, sizeof(new_A->check_size));
    return;
}

void ArenaRelease(void *A){
    Arena *new_A = (Arena*)A;
    int unmap_mem = munmap(new_A->base, new_A->capacity);
    assert(unmap_mem != -1 && "failed to free memory");
    new_A->base = NULL;
    new_A->capacity = 0;
    new_A->offset = 0;
    new_A->commit_position = 0;
    new_A->cnt = 0;
    memset(new_A->check_size, 0, sizeof(new_A->check_size)); 
    return;
}

void* ArenaPush(void *A, size_t size){
    Arena *new_A = (Arena*)A;
    int cpu_alignment = WORD_LEN;
    size_t aligned_offset = (new_A->offset+(cpu_alignment-1))&(~(cpu_alignment-1));
    size_t new_size = (size%WORD_LEN==0) ? size : WORD_LEN*(size/WORD_LEN + 1);
    assert(aligned_offset+new_size <= new_A->capacity && "Memory limit exceeded");
    size_t target_offset = aligned_offset + new_size;
    if(new_A->commit_position < target_offset){
        // means we have to commit virtual memory i.e. we have to create 
        // more pages in physical memory.
        int num_page = (target_offset-new_A->commit_position)/PAGE_SIZE;
        int mod_page = (target_offset-new_A->commit_position)%PAGE_SIZE;
        size_t commit_size = (mod_page == 0) ? num_page*PAGE_SIZE : (num_page+1)*PAGE_SIZE;
        // now we use mprotect to commit the virtual memory
        int commit_flag = mprotect(new_A->base+new_A->commit_position, commit_size, PROT_READ|PROT_WRITE);
        assert(commit_flag != -1 && "failed to commit virtual memory");
        void *ptr = new_A->base + aligned_offset;
        memset((uint8_t*)ptr, 0, new_size);
        new_A->commit_position = new_A->commit_position + commit_size;
        new_A->offset = aligned_offset + new_size;
        new_A->check_size[new_A->cnt++] = new_size;
        return ptr;
    }
    else{
        void *ptr = new_A->base + aligned_offset;
        memset((uint8_t*)ptr, 0, new_size);
        new_A->offset = aligned_offset + new_size;
        new_A->check_size[new_A->cnt++] = new_size;
        return ptr;
    }
}

void ArenaPop(void *A){
    Arena *new_A = (Arena*)A;
    assert(new_A->cnt>0);
    int idx = new_A->cnt-1;
    new_A->offset = new_A->offset - new_A->check_size[idx];
    new_A->cnt--;
    return;
}

void ArenaPopFrom(void *A, int pos){
    Arena *new_A = (Arena*)A;
    assert(pos > 0);
    int idx = new_A->cnt;
    for(int i=idx-1; i>=pos-1; --i){
        new_A->offset = new_A->offset - new_A->check_size[i];
        new_A->cnt--;
    }
    return;
}

void ArenaClear(void *A){
    Arena *new_A = (Arena*)A;
    new_A->offset = 0;
    new_A->cnt = 0;
    memset(new_A->check_size, 0, sizeof(new_A->check_size));
    return;
}

void* PoolAlloc(Pool *pool){
    if(pool->free_list){
        void *ptr = pool->free_list;
        pool->free_list = pool->free_list->next;
        return ptr;
    }
    if(pool->object_size < sizeof(FreeNode)){
        pool->object_size = sizeof(FreeNode); 
    }
    void *ptr = ArenaPush(pool->A, pool->object_size);
    return ptr;
}

void PoolFree(Pool *pool, void *ptr){
    FreeNode *node = (FreeNode*)ptr;
    node->next = pool->free_list;
    pool->free_list = node;
    return;
}
