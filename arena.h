#ifndef _ARENA_H_
#define _ARENA_H_
#define MAX_OBJ 100

typedef struct Arena{
    void* base; // pointing to the base of arena
    size_t offset; 
    size_t capacity;
    size_t check_size[MAX_OBJ];
}Arena;

void* ArenaAlloc(); // for creating arena
void ArenaRelease(); // to free arena
void* ArenaPush(size_t s); // for creating a buffer in arena of size s (in bytes) from the top
void ArenaPop(); // removing the topmost/last object from arena 
void ArenaPopTo(int pos); // removing the objects from arena after position pos from top
void ArenaClear(); // clearing arena
#endif
