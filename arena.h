#ifndef _ARENA_H_
#define _ARENA_H_
#define MAX_OBJ 100

typedef Arena{
    void *first; // first block of memory
    void *current; // second block of memory
}

typedef struct ArenaChunk{
    void* next; // pointing to the base of arena
    size_t offset; 
    size_t capacity;
    size_t check_size[MAX_OBJ];
}Arena;

void* ArenaAlloc(); // for creating arena
void ArenaRelease(void *ptr); // to free arena
void* ArenaPush(size_t s); // for creating a buffer in arena of size s (in bytes) from the top
void ArenaPop(); // removing the topmost/last object from arena 
void ArenaPopTo(int pos); // removing the objects from arena after position pos from top
void ArenaClear(); // clearing arena
#endif
