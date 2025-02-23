#include "arena.h"

#include <string.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>

int main(void) {
    const int TESTDATA[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    arena *arena;
    if(arena_init(&arena, 10 * sizeof(int)) < 0)
        error(1, 0, "Couldn't initalize arena");

    int *test = arena_alloc(arena, 10 * sizeof(int));
    if(!test)
        error(1, errno, "Couldn't get memory from arena_alloc");

    memmove(test, TESTDATA, sizeof(TESTDATA));
    for(int i = 0; i < 10; i++)
        printf("%d\n", test[i]); 

    arena_free(&arena);

    return 0;
}