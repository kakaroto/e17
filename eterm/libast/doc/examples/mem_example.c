/* Example code for memory allocation system.
 *
 * Compile with:  cc -o mem_example mem_example.c -last
 *
 * Run with:
 *    ./mem_example
 */

/* The following defines are workarounds due to the lack of a config.h. */
#define HAVE_STRSEP 1
#define HAVE_USLEEP 1

/* Set our compile-time debug threshold and include the LibAST header. */
#define DEBUG 5
#include <libast.h>

int
main(int argc, char *argv[])
{
    char *pointer, *dup;
    const unsigned psize = 500;

    /* Make sure debug level is high enough to debug memory allocation. */
    DEBUG_LEVEL = 5;

    /* Allocate memory. */
    pointer = (char *) MALLOC(psize);

    /* Zero it out. */
    MEMSET(pointer, 0, psize);

    /* Fill it with some stuff. */
    strcpy(pointer, "This is a test.");

    /* Make a copy. */
    dup = STRDUP(pointer);

    /* Free the copy. */
    FREE(dup);

    /* Free it again; LibAST should catch this as an "attempt to free
       a NULL pointer." */
    FREE(dup);

    /* Double the size of pointer. */
    pointer = (char *) REALLOC(pointer, psize * 2);

    /* Just before we exit, get a listing of un-freed memory to show
       where we've leaked.  In this (admittedly contrived) case, it
       should just be "pointer." */
    MALLOC_DUMP();
    return 0;
}
