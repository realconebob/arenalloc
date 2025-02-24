#include "arena.h"

#include <string.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>

#define STATIC_ARRLEN(arr) (sizeof((arr)) / sizeof((arr)[0]))
#define TESTDATA ((const int[]){1, 2, 3, 4, 5, 6, 7, 8, 9, 10})
#define TESTDATA_LEN STATIC_ARRLEN(TESTDATA)

int main(void) {

    arena *arena;
    if(arena_init(&arena, sizeof(TESTDATA)) < 0)
        error(1, 0, "Couldn't initalize arena");


    // Test to see if memory is actually allocated and usable
    int *test = arena_alloc(arena, sizeof(TESTDATA));
    if(!test)
        error(1, errno, "Couldn't get memory from arena_alloc");

    memmove(test, TESTDATA, sizeof(TESTDATA));
    for(int i = 0; i < 10; i++)
        printf("%d\n", test[i]);


    // Test to make sure further allocation on the standard arena works
    arena_alloc(arena, 6 * sizeof(int));
    arena_alloc(arena, 7 * sizeof(int));

    arena_free(&arena);



    simplearena *sarena;
    if(simplearena_init(&sarena, sizeof(TESTDATA)) < 0)
        error(1, errno, "Could not initialize simple arena");

    // Confirm that trying to allocate too much memory at once causes simplearena_alloc to return NULL
    if(simplearena_alloc(sarena, 101) != NULL)
        error(1, ENOTRECOVERABLE, "Was able to allocate more memory than should be possible in a simple arena");

    simplearena_free(&sarena);

    return 0;
}