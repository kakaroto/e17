/*
 * Copyright (C) 1997-2003, Michael Jennings
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

#define NEXT_ARG()       D_OPTIONS(("NEXT_ARG()\n")); i++; opt = argv[i]; continue
#define NEXT_LETTER()    D_OPTIONS(("NEXT_LETTER(%s)\n", opt)); if (*(opt + 1)) {opt++;} else {NEXT_ARG();} continue
#define NEXT_LOOP()      D_OPTIONS(("NEXT_LOOP()\n")); if (islong || val_ptr) {NEXT_ARG();} else {NEXT_LETTER();} NOP
#define SHOULD_PARSE(j)  ((SPIFOPT_FLAGS_IS_SET(SPIFOPT_SETTING_POSTPARSE) && !SPIFOPT_OPT_IS_PREPARSE(j)) \
                           || (!SPIFOPT_FLAGS_IS_SET(SPIFOPT_SETTING_POSTPARSE) && SPIFOPT_OPT_IS_PREPARSE(j)))

spifopt_settings_t spifopt_settings;

static const char *
get_option_type_string(spif_uint32_t type)
{
    switch (type) {
        case SPIFOPT_FLAG_BOOLEAN: return "(bool)"; break;
        case SPIFOPT_FLAG_INTEGER: return "(int)"; break;
        case SPIFOPT_FLAG_ARGLIST: return "(strs)"; break;
        default: return "(str)";
    }
    ASSERT_NOTREACHED_RVAL(NULL);
}

void
spifopt_usage(void)
{
    spif_uint16_t i, col, l_long = 0, l_desc = 0;

    /* Find the longest long option and the longest description. */
    for (i = 0; i < SPIFOPT_NUMOPTS_GET(); i++) {
        MAX_IT(l_long, strlen(SPIFOPT_OPT_LONG(i)));
        MAX_IT(l_desc, strlen(SPIFOPT_OPT_DESC(i)));
    }
    l_long += 2;  /* Add 2 for the "--" */
    l_desc += 7;  /* Add 7 for the type and a space */

    printf("%s %s\n", libast_program_name, libast_program_version);
    printf("Usage:\n\n");
    printf("POSIX ");

    for (col = 0; col < (l_long - 3) / 2; col++) printf(" ");
    printf("GNU");
    for (col = 0; col < (l_long - 3) / 2; col++) printf(" ");
    if (!(l_long % 2)) {
        printf(" ");
    }
    printf("  ");

    for (col = 0; col < (l_desc - 11) / 2; col++) printf(" ");
    printf("Description");
    for (col = 0; col < (l_desc - 11) / 2; col++) printf(" ");
    if (!(l_desc % 2)) {
        printf(" ");
    }

    printf("\n");
    printf("----- ");

    for (col = 0; col < l_long; col++) printf("-");
    printf("  ");
    for (col = 0; col < l_desc; col++) printf("-");
    printf("\n");

    for (i = 0, l_long -= 2; i < SPIFOPT_NUMOPTS_GET(); i++) {
        if (SPIFOPT_OPT_SHORT(i)) {
            printf(" -%c   ", SPIFOPT_OPT_SHORT(i));
        } else {
            printf("      ");
        }
        printf("--%s", SPIFOPT_OPT_LONG(i));
        for (col = strlen(SPIFOPT_OPT_LONG(i)); col < l_long; col++) {
            printf(" ");
        }
        printf("  %-6s %s\n", get_option_type_string(SPIFOPT_OPT_TYPE(i)), SPIFOPT_OPT_DESC(i));
    }
    exit(EXIT_FAILURE);
}

static spif_int32_t
find_long_option(char *opt)
{
    spif_int32_t j;

    D_OPTIONS(("opt == \"%s\"\n", NONULL(opt)));
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

    D_OPTIONS(("opt == \"%c\"\n", opt));
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
        val_ptr++;
        *hasequal = 1;
    } else {
        if (next_arg) {
            val_ptr = next_arg;
        }
        *hasequal = 0;
    }
    D_OPTIONS(("hasequal == %d  val_ptr == %10.8p \"%s\"\n", *hasequal, val_ptr, (val_ptr ? val_ptr : "(nil)")));
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
    D_OPTIONS(("val_ptr == %10.8p \"%s\"\n", val_ptr, NONULL(val_ptr)));
    return val_ptr;
}

static spif_bool_t
is_boolean_value(char *val_ptr)
{
    if (!(val_ptr) || !(*val_ptr)) {
        return FALSE;
    }
    return ((BOOL_OPT_ISTRUE(val_ptr) || BOOL_OPT_ISFALSE(val_ptr)) ? (TRUE) : (FALSE));
}

static spif_bool_t
is_valid_option(char *opt)
{
    REQUIRE_RVAL(opt != NULL, FALSE);

    if (*opt != '-') {
        return FALSE;
    }
    opt++;
    if (*opt == '-') {
        opt++;
        if (find_long_option(opt) >= 0) {
            return TRUE;
        }
    } else {
        if (find_short_option(*opt) >= 0) {
            return TRUE;
        }
    }
    return FALSE;
}

static spif_bool_t
handle_boolean(spif_int32_t n, char *val_ptr, unsigned char islong)
{
    D_OPTIONS(("Boolean option detected\n"));
    if (val_ptr && islong) {
        /* There's a value, so let's see what it is. */
        if (BOOL_OPT_ISTRUE(val_ptr)) {
            if (SHOULD_PARSE(n)) {
                D_OPTIONS(("\"%s\" == TRUE\n", val_ptr));
                *((unsigned long *) SPIFOPT_OPT_VALUE(n)) |= SPIFOPT_OPT_MASK(n);
            }
        } else if (BOOL_OPT_ISFALSE(val_ptr)) {
            if (SHOULD_PARSE(n)) {
                D_OPTIONS(("\"%s\" == FALSE\n", val_ptr));
                *((unsigned long *) SPIFOPT_OPT_VALUE(n)) &= ~SPIFOPT_OPT_MASK(n);
            }
        } else {
            if (SHOULD_PARSE(n)) {
                D_OPTIONS(("Forcing option --%s to TRUE\n", SPIFOPT_OPT_LONG(n)));
                *((unsigned long *) SPIFOPT_OPT_VALUE(n)) |= SPIFOPT_OPT_MASK(n);
            }
            return FALSE;
        }
    } else {
        if (SHOULD_PARSE(n)) {
            /* No value, or it was a short option, so pretend it was true. */
            if (islong) {
                D_OPTIONS(("Forcing option --%s to TRUE\n", SPIFOPT_OPT_LONG(n)));
            } else {
                val_ptr = NULL;
                D_OPTIONS(("Forcing option -%c to TRUE\n", SPIFOPT_OPT_SHORT(n)));
            }
            *((unsigned long *) SPIFOPT_OPT_VALUE(n)) |= SPIFOPT_OPT_MASK(n);
        }
    }
    return TRUE;
}

static void
handle_integer(spif_int32_t n, char *val_ptr)
{
    D_OPTIONS(("Integer option detected\n"));
    *((int *) SPIFOPT_OPT_VALUE(n)) = strtol(val_ptr, (char **) NULL, 0);
}

static void
handle_string(spif_int32_t n, char *val_ptr)
{
    D_OPTIONS(("String option detected\n"));
    *((const char **) SPIFOPT_OPT_VALUE(n)) = STRDUP(val_ptr);
}

static void
handle_arglist(spif_int32_t n, char *val_ptr, unsigned char hasequal,
               spif_int32_t i, int argc, char *argv[])
{
    char **tmp;
    register unsigned short k;

    D_OPTIONS(("Argument list option detected\n"));
    if (hasequal) {
        /* There's an equals sign, so just parse the rest of this option into words. */
        tmp = (char **) MALLOC(sizeof(char *) * (num_words(val_ptr) + 1));

        for (k = 0; val_ptr; k++) {
            tmp[k] = get_word(1, val_ptr);
            val_ptr = get_pword(2, val_ptr);
            D_OPTIONS(("tmp[%d] == %s\n", k, tmp[k]));
        }
        tmp[k] = (char *) NULL;
        *((char ***) SPIFOPT_OPT_VALUE(n)) = tmp;
    } else {
        unsigned short len = argc - i;

        /* No equals sign, so use the rest of the command line and break. */
        tmp = (char **) MALLOC(sizeof(char *) * (argc - i + 1));

        for (k = 0; k < len; k++) {
            tmp[k] = STRDUP(argv[k + i]);
            D_OPTIONS(("tmp[%d] == %s\n", k, tmp[k]));
        }
        tmp[k] = (char *) NULL;
        *((char ***) SPIFOPT_OPT_VALUE(n)) = tmp;
    }
}

void
spifopt_parse(int argc, char *argv[])
{
    spif_int32_t i, j;
    char *opt;

    REQUIRE(argc > 1);
    REQUIRE(argv != NULL);

    /* Process each command line arg one-by-one. */
    for (i = 1, opt = argv[1]; i < argc; ) {
        char *val_ptr = NULL;
        unsigned char islong = 0, hasequal = 0;

        D_OPTIONS(("argv[%d] == \"%s\", opt == \"%s\"\n", i, argv[i], opt));

        if (opt == argv[i]) {
            /* If it's not an option, skip it. */
            if (*opt != '-') {
                NEXT_ARG();
            } else {
                opt++;
            }
        }

        /* If the second character is also a hyphen, it's a long option. */
        if (*opt == '-') {
            islong = 1;
            /* Skip the leading "--" */
            opt++;
            D_OPTIONS(("Long option detected\n"));
            if ((j = find_long_option(opt)) == -1) {
                NEXT_ARG();
            }
        } else {
            if ((j = find_short_option(*opt)) == -1) {
                NEXT_LETTER();
            }
        }

        /* If a value was passed to this option, set val_ptr to point to it. */
        if (islong) {
            val_ptr = find_value_long(argv[i], argv[i + 1], &hasequal);
        } else {
            val_ptr = find_value_short(opt, argv[i + 1]);
        }

        /* Boolean options may or may not have a value... */
        if (val_ptr) {
            if (SPIFOPT_OPT_IS_BOOLEAN(j) && !is_boolean_value(val_ptr)) {
                val_ptr = NULL;
            } else if (SPIFOPT_OPT_IS_ABSTRACT(j) && is_valid_option(val_ptr)) {
                val_ptr = NULL;
            }
        }
        if (val_ptr) {
            if (val_ptr == argv[i + 1]) {
                i++;
                opt += strlen(opt);
            }
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

        /* Make sure that options which require a parameter have them. */
        if (SPIFOPT_OPT_NEEDS_VALUE(j)) {
            if (val_ptr == NULL) {
                if (islong) {
                    print_error("long option --%s requires a%s value\n", SPIFOPT_OPT_LONG(j),
                                (SPIFOPT_OPT_IS_INTEGER(j)
                                 ? ("n integer")
                                 : (SPIFOPT_OPT_IS_STRING(j)
                                    ? " string"
                                    : (SPIFOPT_OPT_IS_ARGLIST(j)
                                       ? "n argument list"
                                       : ""))));
                } else {
                    print_error("option -%c requires a%s value\n", SPIFOPT_OPT_SHORT(j),
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
            if (!handle_boolean(j, val_ptr, islong)) {
                i--;
            }
        } else if (SPIFOPT_OPT_IS_STRING(j)) {
            if (SHOULD_PARSE(j)) {
                handle_string(j, val_ptr);
            }
        } else if (SPIFOPT_OPT_IS_INTEGER(j)) {
            if (SHOULD_PARSE(j)) {
                handle_integer(j, val_ptr);
            }
        } else if (SPIFOPT_OPT_IS_ARGLIST(j)) {
            if (SHOULD_PARSE(j)) {
                handle_arglist(j, val_ptr, hasequal, i, argc, argv);
            }
            if (!hasequal) {
                break;
            }
        } else if (SPIFOPT_OPT_IS_ABSTRACT(j)) {
            if (SHOULD_PARSE(j)) {
                D_OPTIONS(("Abstract option detected\n"));
                ((spifopt_abstract_handler_t) SPIFOPT_OPT_VALUE(j))(val_ptr);
            }
        }
        NEXT_LOOP();
    }
    SPIFOPT_FLAGS_SET(SPIFOPT_SETTING_POSTPARSE);
}
