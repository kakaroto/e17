/*
 * Copyright (C) 1997-2002, Michael Jennings
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

static const char cvs_ident[] = "$Id$";

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <libast_internal.h>

static void
usage(void)
{
    unsigned short i, col;

    printf("%s %s\n\n", libast_program_name, libast_program_version);
    printf("Usage:\n\n");
    printf("%7s %17s %40s\n", "POSIX", "GNU", "Description");
    printf("%8s %10s %41s\n", "=======", "===============================",
           "=========================================");
    for (i = 0; i < SPIFOPT_NUMOPTS_GET(); i++) {
        printf("%*s", (int) spifopt_settings.indent, " ");
        if (SPIFOPT_OPT_SHORT(i)) {
            printf("-%c, ", SPIFOPT_OPT_SHORT(i));
        } else {
            printf("    ");
        }
        printf("--%s", SPIFOPT_OPT_LONG(i));
        for (col = strlen(SPIFOPT_OPT_LONG(i)); col < 30; col++) {
            printf(" ");
        }
        printf("%s\n", SPIFOPT_OPT_DESC(i));
    }
    exit(EXIT_FAILURE);
}

static spif_int32_t
find_long_option(char *opt)
{
    spif_int32_t j;

    /* Check to see if we have a long option that matches this. */
    for (j = 0; j < SPIFOPT_NUMOPTS_GET(); j++) {
        size_t l;

        l = strlen(SPIFOPT_OPT_LONG(j));
        /* Look for matches to the part before the =, if any. */
        if (!strncasecmp(SPIFOPT_OPT_LONG(j), opt, l) && (opt[l] == '=' || !opt[l])) {
            /* Got one. */
            D_OPTIONS(("Match found at %d:  %s == %s\n", j, SPIFOPT_OPT_LONG(j), opt));
            return j;
        }
    }
    /* No matching long option found.  Report an error and
       continue with the next arg. */
    print_error("Unrecognized long option --%s\n", opt);
    CHECK_BAD();
    return ((spif_int32_t) -1);
}

static spif_int32_t
find_short_option(char opt)
{
    spif_int32_t j;

    for (j = 0; j < SPIFOPT_NUMOPTS_GET(); j++) {
        if (SPIFOPT_OPT_SHORT(j) == opt) {
            D_OPTIONS(("Match found at %d:  %c == %c\n", j, SPIFOPT_OPT_SHORT(j), opt));
            return j;
        }
    }
    print_error("unrecognized option -%c\n", opt);
    CHECK_BAD();
    return ((spif_int32_t) -1);
}

static char *
find_value_long(char *arg, char *next_arg, unsigned char *hasequal)
{
    char *val_ptr;

    if ((val_ptr = strchr(arg, '=')) != NULL) {
        *val_ptr = 0;
        val_ptr++;
        *hasequal = 1;
    } else {
        if (next_arg) {
            val_ptr = next_arg;
        }
        *hasequal = 0;
    }
    D_OPTIONS(("hasequal == %d  val_ptr == %10.8p \"%s\"\n", hasequal, val_ptr, (val_ptr ? val_ptr : "(nil)")));
    return val_ptr;
}

static char *
find_value_short(char *arg, char *next_arg)
{
    char *val_ptr = NULL;

    if (arg[1]) {
        val_ptr = arg + 1;
    } else if (next_arg != NULL) {
        val_ptr = next_arg;
    }
    D_OPTIONS(("val_ptr == %10.8p \"%s\"\n", val_ptr, (val_ptr ? val_ptr : "(nil)")));
    return val_ptr;
}

#define NEXT_ARG()     i++; opt = argv[i]; continue
#define NEXT_LETTER()  opt++; continue
#define NEXT_LOOP()    if (islong) {NEXT_ARG();} else {NEXT_LETTER();} NOP

void
spifopt_parse(int argc, char *argv[])
{
    spif_int32_t i, j;
    char *opt;

    /* Process each command line arg one-by-one. */
    for (i = 1, opt = argv[1]; i < argc; ) {
        char *val_ptr = NULL;
        unsigned char islong = 0, hasequal = 0;

        D_OPTIONS(("argv[%d] == \"%s\", opt == \"%s\"\n", i, argv[i], opt));

        /* If it's not an option, skip it. */
        if (*opt != '-') {
            NEXT_ARG();
        }

        /* If the second character is also a hyphen, it's a long option. */
        if (*(opt + 1) == '-') {
            islong = 1;
            /* Skip the leading "--" */
            opt += 2;
            D_OPTIONS(("Long option detected\n"));
            if ((j = find_long_option(opt)) == -1) {
                NEXT_ARG();
            }
        } else {
            if ((j = find_short_option(*opt)) == -1) {
                NEXT_LETTER();
            }
        }

        /* Only continue if this is a pre-parse option and we're pre-parsing, or
           if this is a post-parse option and we're post-parsing. */
        if ((SPIFOPT_FLAGS_IS_SET(SPIFOPT_SETTING_POSTPARSE) && !SPIFOPT_OPT_IS_PREPARSE(j))
            || (!SPIFOPT_FLAGS_IS_SET(SPIFOPT_SETTING_POSTPARSE) && SPIFOPT_OPT_IS_PREPARSE(j))) {
            NEXT_LOOP();
        }

        /* If this option is deprecated, print a warning before continuing. */
        if (SPIFOPT_OPT_IS_DEPRECATED(j)) {
            spif_str_t warn;

            warn = spif_str_new_from_buff("The ", 128);
            if (SPIFOPT_OPT_SHORT(j)) {
                spif_str_append_char(warn, '-');
                spif_str_append_char(warn, SPIFOPT_OPT_SHORT(j));
                spif_str_append_from_ptr(warn, " / --");
            } else {
                spif_str_append_from_ptr(warn, "--");
            }
            spif_str_append_from_ptr(warn, SPIFOPT_OPT_LONG(j));
            spif_str_append_from_ptr(warn, " option is deprecated and should not be used.\n");
            print_warning(SPIF_STR_STR(warn));
            spif_str_del(warn);
        }

        /* If a value was passed to this option, set val_ptr to point to it. */
        if (islong) {
            val_ptr = find_value_long(argv[i], argv[i + 1], &hasequal);
        } else {
            val_ptr = find_value_short(opt, argv[i + 1]);
        }
        if (val_ptr == argv[i + 1]) {
            i++;
        }

        /* Make sure that options which require a parameter have them. */
        if (SPIFOPT_OPT_NEEDS_VALUE(j)) {
            if (val_ptr == NULL) {
                if (islong) {
                    print_error("long option --%s requires a%s value\n", opt,
                                (SPIFOPT_OPT_IS_INTEGER(j)
                                 ? ("n integer")
                                 : (SPIFOPT_OPT_IS_STRING(j)
                                    ? " string"
                                    : (SPIFOPT_OPT_IS_ARGLIST(j)
                                       ? "n argument list"
                                       : ""))));
                } else {
                    print_error("option -%c requires a%s value\n", *opt,
                                (SPIFOPT_OPT_IS_INTEGER(j)
                                 ? ("n integer")
                                 : (SPIFOPT_OPT_IS_STRING(j)
                                    ? " string"
                                    : (SPIFOPT_OPT_IS_ARGLIST(j)
                                       ? "n argument list"
                                       : ""))));
                }
                CHECK_BAD();
                continue;
            }
            /* Also make sure we know what to do with the value. */
            if (SPIFOPT_OPT_VALUE(j) == NULL) {
                NEXT_LOOP();
            }
        } else if (SPIFOPT_OPT_IS_ABSTRACT(j) && SPIFOPT_OPT_VALUE(j) == NULL) {
            /* Also make sure that abstract options have a function pointer. */
            NEXT_LOOP();
        }

        if (SPIFOPT_OPT_IS_BOOLEAN(j)) {
            D_OPTIONS(("Boolean option detected\n"));
            if (val_ptr) {
                /* There's a value, so let's see what it is. */
                if (BOOL_OPT_ISTRUE(val_ptr)) {
                    D_OPTIONS(("\"%s\" == TRUE\n", val_ptr));
                    *((unsigned long *) SPIFOPT_OPT_VALUE(j)) |= SPIFOPT_OPT_MASK(j);
                } else if (BOOL_OPT_ISFALSE(val_ptr)) {
                    D_OPTIONS(("\"%s\" == FALSE\n", val_ptr));
                    *((unsigned long *) SPIFOPT_OPT_VALUE(j)) &= ~SPIFOPT_OPT_MASK(j);
                } else {
                    if (islong) {
                        print_error("unrecognized boolean value \"%s\" for option --%s\n",
                                    val_ptr, opt);
                    } else {
                        print_error("unrecognized boolean value \"%s\" for option -%c\n",
                                    val_ptr, *opt);
                    }
                    CHECK_BAD();
                }
            } else {
                /* No value, so pretend it was true. */
                D_OPTIONS(("Forcing option --%s to TRUE\n", opt));
                *((unsigned long *) SPIFOPT_OPT_VALUE(j)) |= SPIFOPT_OPT_MASK(j);
            }
        } else if (SPIFOPT_OPT_IS_ABSTRACT(j)) {
            D_OPTIONS(("Abstract option detected\n"));
            ((spifopt_abstract_handler_t) SPIFOPT_OPT_VALUE(j))();
        } else if (SPIFOPT_OPT_IS_STRING(j)) {
            D_OPTIONS(("String option detected\n"));
            *((const char **) SPIFOPT_OPT_VALUE(j)) = STRDUP(val_ptr);
        } else if (SPIFOPT_OPT_IS_INTEGER(j)) {
            D_OPTIONS(("Integer option detected\n"));
            *((int *) SPIFOPT_OPT_VALUE(j)) = strtol(val_ptr, (char **) NULL, 0);
        } else if (SPIFOPT_OPT_IS_ARGLIST(j)) {
            char **tmp;
            register unsigned short k;

            D_OPTIONS(("arglist option detected\n"));
            if (hasequal) {
                /* There's an equals sign, so just parse the rest of this option into words. */
                tmp = (char **) MALLOC(sizeof(char *) * (num_words(val_ptr) + 1));

                for (k = 0; val_ptr; k++) {
                    tmp[k] = get_word(1, val_ptr);
                    val_ptr = get_pword(2, val_ptr);
                    D_OPTIONS(("tmp[%d] == %s\n", k, tmp[k]));
                }
                tmp[k] = (char *) NULL;
                *((char ***) SPIFOPT_OPT_VALUE(j)) = tmp;
            } else {
                unsigned short len = argc - i;

                    /* No equals sign, so use the rest of the command line and break. */
                tmp = (char **) MALLOC(sizeof(char *) * (argc - i + 1));

                for (k = 0; k < len; k++) {
                    tmp[k] = STRDUP(argv[k + i]);
                    D_OPTIONS(("tmp[%d] == %s\n", k, tmp[k]));
                }
                tmp[k] = (char *) NULL;
                *((char ***) SPIFOPT_OPT_VALUE(j)) = tmp;
                break;
            }
        } else if (SPIFOPT_OPT_IS_ABSTRACT_VALUE(j)) {
            D_OPTIONS(("Abstract/value option detected\n"));
            ((spifopt_abstract_value_handler_t) SPIFOPT_OPT_VALUE(j))(val_ptr);
        }
        NEXT_LOOP();
    }
    SPIFOPT_FLAGS_SET(SPIFOPT_SETTING_POSTPARSE);
}
