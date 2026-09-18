#include<stdio.h>
#include<sys/mman.h>
#include<string.h>
#include<assert.h>
#include "arena.h"

#define ARENA_SIZE 4*1024 + sizeof(ArenaChunk) // 4kb + 24bytes
#define WORD_LEN 8

Arena *A = {NULL};
static int cnt = 0;
static global_alloc_cnt = 1;

void* ArenaAlloc(){
    ArenaChunk *ptr = (ArenaChunk*)mmap(NULL, ARENA_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    assert(ptr != MAP_FAILED, "memory allocation failed");
    if(global_alloc_cnt == 1){
        A->first = ptr;
        A->current = A->first;
        A->current->next = NULL;
        A->currrent->capacity = ARENA_SIZE;
        A->current->offset = sizeof(ArenaChunk);
        memset(A->current->check_size, 0, sizeof(A->current->check_size));
        global_alloc_cnt++;
    }
    else{
        A->current->next = ptr;
        A->current = ptr;
        A->current->next = NULL;
        A->current->capacity = ARENA_SIZE;
        A->current->offset = sizeof(ArenaChunk);
        memset(A->current->check_size, 0, sizeof(A->current->check_size));
    }
    /*
    A->base = ptr;
    A->offset = 0;
    A->capacity = ARENA_SIZE;
    for(size_t i = 0; i<sizeof(A->check_size); ++i){
        A->check_size[i] = 0;
    }
    */

    return (uint8_t*)A->current + sizeof(ArenaChunk);
}

void ArenaRelease(void *ptr){
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
    ArenaChunk *ptr = A->first;
    while(ptr!=NULL){
        int free = munmap(ptr, ptr->capacity);
        assert(free != -1, "error occured in deleting memory");
        ptr = ptr->next;
    }
    return;
}

void* ArenaPush(size_t size){
    int cpu_alignment = WORD_LEN;
    size_t aligned_offset = (A->current->offset+(cpu_alignment-1))&(~(cpu_alignment-1));
    assert(aligned_offset + size <= A->current->capacity, "Out of memory. Program crashed");
    void *ptr = (uint8_t*)A->current + aligned_offset;
    int rem_num = (size%8)==0 ?0 : 1;
    size_t allocated_size = 8*(size/8 + rem_num);
    memset(ptr, 0, allocated_size);
    A->current->offset = aligned_offset + allocated_size;
    A->current->check_size[cnt++] = allocated_size;
    return ptr;
}

void ArenaPop(){
    A->current->offset = A->current->offset - A->current->check_size[--cnt];
    return;
}

void ArenaPopTo(int pos){
    for(int i=cnt-1; i>=pos-1; --i){
        A->current->offset = A->current->offset - A->current->check_size[i];
    }
    cnt=cnt-pos-1;
    return;
}

void ArenaClear(){
    //A->offset = 0;
    ArenaChunk *ptr = A->first;
    while(ptr != NULL){
        ptr->offset = sizeof(ArenaChunk);
        ptr = ptr->next;
    }
    return;
}

int main(int argc, char *argv[]){
    printf("Hello World\n");
    void *ptr = ArenaAlloc(); 
    printf("%p\n", ptr);
    return 0;
}
