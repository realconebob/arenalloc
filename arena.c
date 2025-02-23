// Consult the readme on what an arena allocator is (from my limited knowledge)

#include "arena.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

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
} arena;

int arena_init(arena **a, size_t bytes) {
    // Create an arena object
    (*a) = malloc(1 * sizeof(**a));
    if(!(*a))
        return -1;
    (*a)->start = NULL;
    (*a)->current = NULL;

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