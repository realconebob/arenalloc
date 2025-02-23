/* The concept of an arena/linear allocator is pretty basic, in that all you do is pre-allocate
// some amount of memory, then instead of calling _alloc() functions, you use arena_alloc
// functions to simply grab some amount of memory from the arena. Doing this offers 2 primary
// benefits: a significantly easier to understand memory lifetime, and an ease of deallocation

// Memory lifetime is how long memory is supposed to be allocated. The lifetime of a variable local
// to a function call is until the function returns. Once it does, the memory is marked free and
// can be reused. The _alloc functions put lifetime management in your hands, which can be quite
// nice because you can have ANY lifetime you want, but it can also be a handful to keep track of
// each and every single thing you have allocated. Using an arena to do your allocation makes it
// easier to keep track because you know it's just based on the arena's lifetime

// As for the deallocation, arenas are actually slightly weird in how they work. You're not allowed
// to / can't deallocate individual bits of arena-allocated memory. You can only deallocate the
// entire arena at once. However, again, not only is this generally nice, but it plays into that
// concept of a defined lifetime, and makes it easier to keep track of everything

// Not everything needs to be allocated in an arena, but it's a tool that most find useful because
// it simplifies a lot about memory management (if implemented well)

// Implementation wise, there are may ways to skin this particular cat, however the most common ones
// use some form of linked list (usually singly-linked) and an arena object. The arena object tends
// to be an opaque structure which manages the linked list of arena nodes, and the nodes deal with
// the allocated memory

*/

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
    arenanode *base = malloc(1 * sizeof(*base));
    if(!base) {
        free((*a));
        return -1;
    }
    base->allocated = bytes;
    base->used = 0;
    base->next = NULL;
    
    void *mem = malloc(bytes);
    if(!mem) {
        free(base);
        free((*a));
        return -1;
    }

    base->membase = mem;
    base->memcur = mem;

    // Put the base arenanode into the arena
    (*a)->start = base;
    (*a)->current = base;

    return 0;
}