#include<stdio.h>
#include<sys/mman.h>
#include<stdlib.h>
#include<assert.h>
#include "arena.h"

#define ARENA_SIZE 4*1024 // 4kb
#define WORD_LEN 8

Arena *A = {NULL};
static int cnt = 0;

void* ArenaAlloc(){
    void *ptr = mmap(NULL, ARENA_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    assert(ptr != MAP_FAILED);
    A->base = ptr;
    A->offset = 0;
    A->capacity = ARENA_SIZE;
    for(size_t i = 0; i<sizeof(A->check_size); ++i){
        A->check_size[i] = 0;
    }
    return A->base;
}

void ArenaRelease(){
    free(A->base);
    A->capacity = 0;
    A->offset = 0;
    for(size_t i = 0; i<sizeof(A->check_size); ++i){
        A->check_size[i] = 0;
    }
    cnt = 0;
    return;
}

void* ArenaPush(size_t size){
    int cpu_alignment = 8;
    size_aligned_offset = (A->offset+(cpu_alignment-1))&(~(cpu_alignment-1));
    assert(aligned_offset + size <= A->capacity, "Out of memory. Program crashed");
    void *ptr = (uint8_t*)A->base + aligned_offset;
    memset(ptr, 0, size);
    A->offset = aligned_offset + size;
    int rem_num = size%8==0 ?0 : 1;
    A->check_size[cnt++] = 8*(size/8 + rem_num);
    return ptr;
}


int main(int argc, char *argv[]){
    printf("Hello World\n");
    void *ptr = ArenaAlloc();
    printf("%p\n", ptr);
    return 0;
}

