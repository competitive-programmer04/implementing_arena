#include<stdio.h>
#include<sys/mman.h>
#include<string.h>
#include<assert.h>
#include "arena.h"

#define ARENA_SIZE (4*1024 + sizeof(ArenaChunk)) // 4kb + 24bytes
#define WORD_LEN 8

//Arena *A = {NULL};
//static int cnt = 0;
//static int global_alloc_cnt = 1;

void ArenaAlloc(void *A){
    Arena *new_A = (Arena*)A;
    ArenaChunk *ptr = (ArenaChunk*)mmap(NULL, ARENA_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    assert(ptr != MAP_FAILED && "memory allocation failed");
    ptr->next = NULL;
    ptr->capacity = ARENA_SIZE;
    ptr->offset = sizeof(ArenaChunk);
    ptr->cnt = 0;
    memset(ptr->check_size, 0, sizeof(ptr->check_size));
    if(new_A->first == NULL){
        new_A->first = ptr;
        new_A->current = new_A->first;
        // new_A->current->next = NULL;
        // new_A->current->capacity = ARENA_SIZE;
        // new_A->current->offset = sizeof(ArenaChunk);
        // memset(new_A->current->check_size, 0, sizeof(new_A->current->check_size));
        //global_alloc_cnt++;
    }
    else{
        new_A->current->next = ptr;
        new_A->current = ptr;
        // new_A->current->next = NULL;
        // new_A->current->capacity = ARENA_SIZE;
        // new_A->current->offset = sizeof(ArenaChunk);
        // memset(new_A->current->check_size, 0, sizeof(new_A->current->check_size));
    }
    /*
    A->base = ptr;
    A->offset = 0;
    A->capacity = ARENA_SIZE;
    for(size_t i = 0; i<sizeof(A->check_size); ++i){
        A->check_size[i] = 0;
    }
    */

    //return (void*)((uint8_t*)A->current + sizeof(ArenaChunk));
    return;
}

void ArenaRelease(void *A){
    /*
    int free = munmap(ptr, A->capacity);
    assert(free != -1, "error occured in deleting memory");
    A->capacity = 0;
    A->offset = 0;
    for(size_t i = 0; i<sizeof(A->check_size); ++i){
        A->check_size[i] = 0;
    }
    cnt = 0;
    */
   Arena *new_A = (Arena*)A;
    ArenaChunk *ptr = new_A->first;
    while(ptr!=NULL){
        ArenaChunk *next = ptr->next;
        int free = munmap(ptr, ptr->capacity);
        assert(free != -1 && "error occured in deleting memory");
        ptr = next;
    }
    new_A->first = NULL;
    new_A->current = NULL;
    return;
}

void* ArenaPush(void *A, size_t size){
    Arena *new_A = (Arena*)A;
    if(new_A->first == NULL){
        ArenaAlloc(new_A);
    }
    int cpu_alignment = WORD_LEN;
    size_t aligned_offset = (new_A->current->offset+(cpu_alignment-1))&(~(cpu_alignment-1));
    //assert(aligned_offset + size <= A->current->capacity, "Out of memory. Program crashed");
    if(aligned_offset + size > new_A->current->capacity){
        ArenaAlloc(new_A);
        aligned_offset = (new_A->current->offset+(cpu_alignment-1))&(~(cpu_alignment-1));
    }
    void *ptr = (uint8_t*)new_A->current + aligned_offset;
    int rem_num = (size%8)==0 ?0 : 1;
    size_t allocated_size = 8*(size/8 + rem_num);
    memset(ptr, 0, allocated_size);
    new_A->current->offset = aligned_offset + allocated_size;
    int chunk_cnt = new_A->current->cnt;
    assert(chunk_cnt < MAX_OBJ && "maximum size limit exceeded");
    new_A->current->check_size[chunk_cnt] = allocated_size;
    new_A->current->cnt++;
    return ptr;
}

void ArenaPop(void *A){
    Arena *new_A = (Arena*)A;
    assert(new_A->current && new_A->current->cnt > 0);
    int idx = new_A->current->cnt - 1;
    new_A->current->offset = new_A->current->offset - new_A->current->check_size[idx];
    new_A->current->cnt--;
    return;
}

void ArenaPopTo(void *A, int pos){
    Arena *new_A = (Arena*)A;
    if(new_A->current && pos > 0){
        int idx = new_A->current->cnt;
        for(int i=idx-1; i>=pos-1; --i){
        new_A->current->offset = new_A->current->offset - new_A->current->check_size[i];
        new_A->current->cnt--;
      }
    }
    //cnt=cnt-pos-1;
    return;
}

void ArenaClear(void *A){
    //A->offset = 0;
    Arena *new_A = (Arena*)A;
    ArenaChunk *ptr = new_A->first;
    while(ptr != NULL){
        ArenaChunk *next = ptr->next;
        ptr->offset = sizeof(ArenaChunk);
        ptr->cnt = 0;
        ptr = next;
    }
    new_A->current = new_A->first;
    return;
}
/*
int main(int argc, char *argv[]){
    printf("Hello World\n");
    //void *ptr = ArenaAlloc(); 
    printf("%p\n", ptr);
    return 0;
}
*/
