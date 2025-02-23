#ifndef __ARENA_ALLOCATOR___ARENA_H___17052168762686___
#define __ARENA_ALLOCATOR___ARENA_H___17052168762686___

#include <stddef.h>

typedef struct arena arena;

int arena_init(arena **a, size_t bytes);
void* arena_alloc(arena * const arena, size_t bytes);
int arena_free(arena **arena);

#endif