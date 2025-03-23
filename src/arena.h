#ifndef __ARENA_ALLOCATOR___ARENA_H___17052168762686___
#define __ARENA_ALLOCATOR___ARENA_H___17052168762686___

#include <stddef.h>

typedef struct arena arena;
typedef arena simplearena;

// Adapted from Ginger Bill's "is_power_of_two" function: https://www.gingerbill.org/article/2019/02/08/memory-allocation-strategies-002/
#define ISPOWOF2(x) (((x) & ((x) - 1)) == 0)
#define MEM_ALIGN_BYTES 16 /* Equivalent to (2 * sizeof(void*)) */
#define MEM_ALIGN(x) ((x) + (((x) & (MEM_ALIGN_BYTES - 1)) != 0) * (MEM_ALIGN_BYTES - ((x) & (MEM_ALIGN_BYTES - 1))))

#if MEM_ALIGN_BYTES <= 0 || !ISPOWOF2(MEM_ALIGN_BYTES)
    #error "Issue with memory alignment. Please check the MEM_ALIGN_BYTES macro in arena.h"
#endif

int arena_init(arena **a, size_t bytes);
void* arena_alloc(arena * const arena, size_t bytes);
int arena_free(arena **arena);

int simplearena_init(simplearena **a, size_t bytes);
void* simplearena_alloc(simplearena * const a, size_t bytes);
int simplearena_free(simplearena **a);

#endif