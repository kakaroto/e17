/* Example code for strings routines.
 *
 * Compile with:  cc -o strings_example strings_example.c -last
 *
 * Run with:
 *    ./strings_example
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
    char *s1, *s2, *s3;

    printf("CONST_STRLEN(\"Testing\") returns %d\n", CONST_STRLEN("Testing"));

    if (argc > 1) {
        s1 = argv[1];
    } else {
        s1 = "This is a test string.";
    }
    if (!BEG_STRCASECMP(s1, "this")) {
        printf("s1 (\"%s\") starts with \"this.\"\n", s1);
    } else {
        printf("s1 (\"%s\") does not start with \"this.\"\n", s1);
    }

    return 0;
}
