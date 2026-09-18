#ifndef _ARENA_H_
#define _ARENA_H_
#define MAX_OBJ 100

typedef struct ArenaChunk{
    struct ArenaChunk* next; // pointing to the base of arena
    size_t offset; 
    size_t capacity;
    int cnt;
    size_t check_size[MAX_OBJ];
}ArenaChunk;

typedef struct Arena{
    struct ArenaChunk *first; // first block of memory
    struct ArenaChunk *current; // second block of memory
}Arena;

void ArenaAlloc(void *A); // for creating arena
void ArenaRelease(void *A); // to free arena
void* ArenaPush(void *A, size_t s); // for creating a buffer in arena of size s (in bytes) from the top
void ArenaPop(void *A); // removing the topmost/last object from arena 
void ArenaPopTo(void *A, int pos); // removing the objects from arena after position pos from top
void ArenaClear(void *A); // clearing arena
#endif
