// Consult the readme on what an arena allocator is (from my limited knowledge)

#include "arena.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

typedef struct an {
    void *membase;
    void *memcur;
    size_t allocated;
    size_t used;

    struct an *next;
} arenanode;

typedef struct arena {
    arenanode *start;
    arenanode *current;

    // For keeping track of the largest possible thing that can be allocated to one node
    size_t node_memspace;
} arena;

int arenanode_init(arenanode **an, size_t bytes) {
    // Create a base arenanode
    (*an) = malloc(1 * sizeof(**an));
    if(!(*an))
        return -1;

    // Set some values
    (*an)->allocated = bytes;
    (*an)->used = 0;
    (*an)->next = NULL;
    
    // Allocate memory for the node to dish out
    void *mem = malloc(bytes);
    if(!mem) {
        free((*an));
        return -1;
    }

    // Put the memory into the node
    (*an)->membase = mem;
    (*an)->memcur = mem;

    return 0;
}

int arena_init(arena **a, size_t bytes) {
    // Create an arena object
    (*a) = malloc(1 * sizeof(**a));
    if(!(*a))
        return -1;
    (*a)->start = NULL;
    (*a)->current = NULL;
    (*a)->node_memspace = bytes;

    // Create the base arena node & preallocate some memory
    arenanode *base;
    if(arenanode_init(&base, bytes) < 0) {
        free(*a);
        return -1;
    }

    // Put the base arenanode into the arena
    (*a)->start = base;
    (*a)->current = base;

    return 0;
}

void* arena_alloc(arena * const arena, size_t bytes) {
    if(!arena)
        return NULL;
    if(bytes > arena->node_memspace) {
        errno = ENOMEM;
        return NULL;
    }

    // Check if there's enough room in the current node:
        // Enough       - Increment the necessary values and return the desired memory
        // Not enough   - Make a new arenanode, append it to the list, and increment everything

    if(bytes > ((arena->current)->allocated - (arena->current)->used)) {
        arenanode *new;
        if(arenanode_init(&new, arena->node_memspace) < 0)
            return NULL;

        arena->current->next = new;
        arena->current = new;
    }

    void *mem = arena->current->memcur;
    arena->current->memcur = (void*)(((uint8_t*)arena->current->memcur) + bytes); 
        // I believe this is the "correct" way to do void pointer arithmetic. Technically, there is no correct way, but if I'm
        // remembering how array pointer arithmetic works, all that's happening when dereferencing an array via x[y] notation is
        // *(x + y), which if x is the size of a byte and y is the number of bytes, the correct way to do it for void*'s is
        // *((uint8_t*)x + (i * sizeof(element))). If you don't care about some specific element size and just want raw bytes,
        // it's simplified a little into *((uint8_t*)x + i)

    arena->current->used += bytes;

    return mem;
}

int arena_free(arena **arena) {
    if(!(*arena))
        return -1;

    (*arena)->current = (*arena)->start;
    for(arenanode *n; (*arena)->current != NULL;) {
        n = (*arena)->current->next;
        
        free((*arena)->current->membase);
        free((*arena)->current);

        (*arena)->current = n;
    }
    free((*arena));
    (*arena) = NULL; 

    return 0;
}