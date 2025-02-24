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
    (*an) = malloc(1 * sizeof(**an));
    if(!(*an))
        return -1;

    (*an)->allocated = bytes;
    (*an)->used = 0;
    (*an)->next = NULL;
    
    void *mem = malloc(bytes);
    if(!mem) {
        free((*an));
        (*an) = NULL;
        return -1;
    }

    (*an)->membase = mem;
    (*an)->memcur = mem;

    return 0;
}

int arena_init(arena **a, size_t bytes) {
    (*a) = malloc(1 * sizeof(**a));
    if(!(*a))
        return -1;
    (*a)->start = NULL;
    (*a)->current = NULL;
    (*a)->node_memspace = bytes;

    arenanode *base;
    if(arenanode_init(&base, bytes) < 0) {
        free(*a);
        (*a) = NULL;
        return -1;
    }

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

    if(bytes > ((arena->current)->allocated - (arena->current)->used)) {
        arenanode *new;
        if(arenanode_init(&new, arena->node_memspace) < 0)
            return NULL;

        arena->current->next = new;
        arena->current = new;
    }

    void *mem = arena->current->memcur;
    arena->current->memcur = (void*)(((uint8_t*)arena->current->memcur) + bytes); 

    arena->current->used += bytes;

    return mem;
}

int arena_free(arena **arena) {
    if(!arena)
        return -1;
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



// Simple Arena is an arena that can't expand whenever allocating memory, meaning what you originally allocated is what you get

typedef arena simplearena;

int simplearena_init(simplearena **a, size_t bytes) {
    return arena_init(a, bytes);
}

void* simplearena_alloc(simplearena * const a, size_t bytes) {
    // The criteria to allocate new memory in arena_alloc is 'bytes > ((a->current)->allocated - (a->current)->used)', so if this
    // is true, just return NULL & set errno

    if(!a)
        return NULL;
    if(bytes > ((a->current)->allocated - (a->current)->used)) {
        errno = ENOMEM;
        return NULL;
    }

    return arena_alloc(a, bytes);
}

int simplearena_free(simplearena **a) {
    return arena_free(a);
}