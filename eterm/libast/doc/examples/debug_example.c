/* Example code for debugging system.
 *
 * Compile with:  cc -o debug_example debug_example.c -last
 *
 * Run with:
 *    ./debug_example 10 - 4
 * for no debugging, or
 *    ./debug_example -d 9 + 9
 * for debugging.
 */

/* The following defines are workarounds due to the lack of a config.h. */
#define HAVE_STRSEP 1
#define HAVE_USLEEP 1

/* Set our compile-time debug threshold and include the LibAST header. */
#define DEBUG 5
#include <libast.h>

/* Define the debugging stuff for our program. */
#define DEBUG_MATH 1
#define D_MATH(x) DPRINTF1(x)

int
main(int argc, char *argv[])
{
    int i;
    long num1, num2;
    char op;

    for (i = 0; i < argc; i++) {
        if (!strcasecmp(argv[i], "-d")) {
            /* They asked for debugging output, so increase the debug level. */
            DEBUG_LEVEL = DEBUG_MATH;
            D_MATH(("Debugging is now on.\n"));
        } else {
            switch (*argv[i]) {
                case '+':
                case '-':
                case '*':
                case '/':
                    op = *argv[i];
                    D_MATH(("Got operation \'%c\'.\n", op));
                    break;
                default:
                    if (i <= 2) {
                        num1 = (long) strtol(argv[i], (char **) NULL, 0);
                        D_MATH(("Number 1 is %ld\n", num1));
                    } else {
                        num2 = (long) strtol(argv[i], (char **) NULL, 0);
                        D_MATH(("Number 2 is %ld\n", num2));
                    }
                    break;
            }
        }
    }

    printf("%ld %c %ld = ", num1, op, num2);
    switch (op) {
        case '+':
            num1 += num2;
            break;
        case '-':
            num1 -= num2;
            break;
        case '*':
            num1 *= num2;
            break;
        case '/':
            num1 /= num2;
            break;
    }
    printf("%ld\n", num1);
    return 0;
}
