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

#include <libast.h>
#include "test.h"

unsigned short tnum = 0;

int test_macros(void);
int test_mem(void);
int test_strings(void);
int test_snprintf(void);
int test_options(void);
int test_obj(void);
int test_str(void);
int test_tok(void);
int test_list(void);

int
test_macros(void)
{
    char memset_test[] = "abcdefghijklmnopqrstuvwxyz";
    char sc1 = 'X', sc2 = 'K';
    int si1 = 472, si2 = 8786345;
    unsigned long sl1 = 0x98765432, sl2 = 0xffeeddff;

    TEST_BEGIN("MEMSET() macro");
    MEMSET(memset_test, '!', CONST_STRLEN(memset_test));
    TEST_EXPECT(!strcmp(memset_test, "!!!!!!!!!!!!!!!!!!!!!!!!!!"));

    TEST_BEGIN("SWAP() macro");
    SWAP(sc1, sc2);
    SWAP(si1, si2);
    SWAP(sl1, sl2);
    TEST_FAIL_IF(sc1 != 'K');
    TEST_FAIL_IF(sc2 != 'X');
    TEST_FAIL_IF(si1 != 8786345);
    TEST_FAIL_IF(si2 != 472);
    TEST_FAIL_IF(sl1 != 0xffeeddff);
    TEST_FAIL_IF(sl2 != 0x98765432);
    TEST_PASS();

    TEST_BEGIN("BEG_STRCASECMP() macro");
    TEST_FAIL_IF(!BEG_STRCASECMP("this", "this is a test"));
    TEST_FAIL_IF(!BEG_STRCASECMP("thx", "this is another test"));
    TEST_FAIL_IF(BEG_STRCASECMP("this is still another test", "this is"));
    TEST_PASS();

    TEST_PASSED("macro");
}

int
test_mem(void)
{

    return 0;
}

int
test_strings(void)
{
    char *s1, *s2, *s3, *s4;
#ifdef HAVE_REGEX_H
    regex_t *r = NULL;
#endif
    char **slist;

    TEST_BEGIN("left_str() function");
    s1 = left_str("bugger all", 3);
    s2 = left_str("testing 1-2-3", 7);
    s3 = left_str(NULL, 0);
    s4 = left_str("eat me", 0);
    TEST_FAIL_IF(strcmp(s1, "bug"));
    TEST_FAIL_IF(strcmp(s2, "testing"));
    TEST_FAIL_IF(s3 != NULL);
    TEST_FAIL_IF(s4 != NULL);
    FREE(s1);
    FREE(s2);
    FREE(s3);
    FREE(s4);
    TEST_PASS();

    TEST_BEGIN("mid_str() function");
    s1 = mid_str("pneumonoultramicroscopicsilicovolcanoconiosis", 8, 16);
    s2 = mid_str("abc", 7, 5);
    s3 = mid_str(NULL, 0, 0);
    s4 = mid_str("what the heck", -5, 42);
    TEST_FAIL_IF(strcmp(s1, "ultramicroscopic"));
    TEST_FAIL_IF(s2 != NULL);
    TEST_FAIL_IF(s3 != NULL);
    TEST_FAIL_IF(s4 != NULL);
    FREE(s1);
    FREE(s2);
    FREE(s3);
    FREE(s4);
    TEST_PASS();

    TEST_BEGIN("right_str() function");
    s1 = right_str("bugger all", 3);
    s2 = right_str("testing 1-2-3", 5);
    s3 = right_str(NULL, 0);
    s4 = right_str("eat me", 0);
    TEST_FAIL_IF(strcmp(s1, "all"));
    TEST_FAIL_IF(strcmp(s2, "1-2-3"));
    TEST_FAIL_IF(s3 != NULL);
    TEST_FAIL_IF(s4 != NULL);
    FREE(s1);
    FREE(s2);
    FREE(s3);
    FREE(s4);
    TEST_PASS();

#ifdef HAVE_REGEX_H
    TEST_BEGIN("regexp_match() function");
    TEST_FAIL_IF(!regexp_match("One particular string", "part"));
    TEST_FAIL_IF(regexp_match("Some other strange string", "^[A-Za-z]+$"));
    TEST_FAIL_IF(!regexp_match("some-rpm-package-1.0.1-4.src.rpm", "^(.*)-([^-]+)-([^-])\\.([a-z0-9]+)\\.rpm$"));
    TEST_FAIL_IF(regexp_match("/the/path/to/some/odd/file.txt", "/this/should/not/match"));
    TEST_FAIL_IF(!regexp_match("1600x1200", "[[:digit:]]+x[[:digit:]]+"));
    TEST_FAIL_IF(regexp_match("xxx", NULL));
    TEST_FAIL_IF(!regexp_match_r("AbCdEfGhIjKlMnOpQrStUvWxYz", "[[:upper:]]", &r));
    TEST_FAIL_IF(regexp_match_r("abcdefjhijklmnopqrstuvwxyz", NULL, &r));
    TEST_FAIL_IF(!regexp_match_r("aaaaa", "[[:lower:]]", &r));
    TEST_PASS();
#endif

    TEST_BEGIN("split() function");
    slist = split(" ", "Splitting a string on spaces");
    TEST_FAIL_IF(!slist);
    TEST_FAIL_IF(!slist[0] || !slist[1] || !slist[2] || !slist[3] || !slist[4] || slist[5]);
    TEST_FAIL_IF(strcmp(slist[0], "Splitting"));
    TEST_FAIL_IF(strcmp(slist[1], "a"));
    TEST_FAIL_IF(strcmp(slist[2], "string"));
    TEST_FAIL_IF(strcmp(slist[3], "on"));
    TEST_FAIL_IF(strcmp(slist[4], "spaces"));
    slist = split(NULL, "          a\t \ta        a a a a       a     ");
    TEST_FAIL_IF(!slist);
    TEST_FAIL_IF(!slist[0] || !slist[1] || !slist[2] || !slist[3] || !slist[4] || !slist[5] || !slist[6] || slist[7]);
    TEST_FAIL_IF(strcmp(slist[0], "a"));
    TEST_FAIL_IF(strcmp(slist[1], "a"));
    TEST_FAIL_IF(strcmp(slist[2], "a"));
    TEST_FAIL_IF(strcmp(slist[3], "a"));
    TEST_FAIL_IF(strcmp(slist[4], "a"));
    TEST_FAIL_IF(strcmp(slist[5], "a"));
    TEST_FAIL_IF(strcmp(slist[6], "a"));
    slist = split(NULL, "  first \"just the second\" third \'fourth and \'\"fifth to\"gether last");
    TEST_FAIL_IF(!slist);
    TEST_FAIL_IF(!slist[0] || !slist[1] || !slist[2] || !slist[3] || !slist[4] || slist[5]);
    TEST_FAIL_IF(strcmp(slist[0], "first"));
    TEST_FAIL_IF(strcmp(slist[1], "just the second"));
    TEST_FAIL_IF(strcmp(slist[2], "third"));
    TEST_FAIL_IF(strcmp(slist[3], "fourth and fifth together"));
    TEST_FAIL_IF(strcmp(slist[4], "last"));
    slist = split(NULL, "\'don\\\'t\' try this    at home \"\" ");
    TEST_FAIL_IF(!slist);
    TEST_FAIL_IF(!slist[0] || !slist[1] || !slist[2] || !slist[3] || !slist[4] || !slist[5] || slist[6]);
    TEST_FAIL_IF(strcmp(slist[0], "don\'t"));
    TEST_FAIL_IF(strcmp(slist[1], "try"));
    TEST_FAIL_IF(strcmp(slist[2], "this"));
    TEST_FAIL_IF(strcmp(slist[3], "at"));
    TEST_FAIL_IF(strcmp(slist[4], "home"));
    TEST_FAIL_IF(slist[5][0]);
    slist = split(":", "A:B:C:D:::E");
    TEST_FAIL_IF(!slist);
    TEST_FAIL_IF(!slist[0] || !slist[1] || !slist[2] || !slist[3] || !slist[4] || slist[5]);
    TEST_FAIL_IF(strcmp(slist[0], "A"));
    TEST_FAIL_IF(strcmp(slist[1], "B"));
    TEST_FAIL_IF(strcmp(slist[2], "C"));
    TEST_FAIL_IF(strcmp(slist[3], "D"));
    TEST_FAIL_IF(strcmp(slist[4], "E"));
    TEST_PASS();

    TEST_PASSED("string");
    return 0;
}

int
test_snprintf(void)
{

    return 0;
}

int
test_options(void)
{
    spif_uint32_t test_flag_var = 0;
    char *file_var = NULL, **exec_list = NULL;
    long num_var = 0, geom_var = 0;
    char *argv1[] = { "test", "-abvf", "somefile", "-n1", "-g", "3", "-e", "help", "me", "rhonda", NULL };
    int argc1 = 10;
    spifopt_t opts1[] = {
        SPIFOPT_BOOL('a', "agony", "scream in agony", test_flag_var, 0x01),
        SPIFOPT_BOOL('b', "bogus", "mark as bogus", test_flag_var, 0x02),
        SPIFOPT_BOOL('c', "crap", "spew some random crap", test_flag_var, 0x04),
        SPIFOPT_BOOL_LONG("dillhole", "are you a dillhole?", test_flag_var, 0x08),
        SPIFOPT_ARGS('e', "exec", "what to exec", exec_list),
        SPIFOPT_STR('f', "file", "set the filename", file_var),
        SPIFOPT_INT('g', "geom", "geometry", geom_var),
        SPIFOPT_INT('n', "num", "number", num_var),
        SPIFOPT_BOOL_PP('v', "verbose", "be verbose", test_flag_var, 0x10)
    };
    char *display = NULL, *name = NULL, *theme = NULL, **exec = NULL, **foo = NULL;
    long color = 0;
    spif_uint32_t options = 0;
    static void handle_theme(char *val_ptr) {theme = STRDUP(val_ptr);}
    char *argv2[] = { "test", "-rt", "mytheme", "--name", "This is a name", "--exec=ssh foo@bar.com", "--scrollbar",
                      "--buttonbar", "no", "--login=0", "-mvd", "foo:0", "--color", "4", "--foo", "blah", "-d", "eatme", NULL };
    int argc2 = 18;
    spifopt_t opts2[] = {
        SPIFOPT_STR_PP('d', "display", "X display to connect to", display),
        SPIFOPT_ARGS_PP('e', "exec", "command to run", exec),
        SPIFOPT_BOOL_PP('l', "login", "login shell", options, 0x01),
        SPIFOPT_BOOL('m', "map-alert", "raise window on beep", options, 0x02),
        SPIFOPT_STR('n', "name", "name", name),
        SPIFOPT_BOOL('r', "reverse-video", "enable reverse video", options, 0x04),
        SPIFOPT_ABST_PP('t', "theme", "theme to use", handle_theme),
        SPIFOPT_BOOL_PP('v', "visual-bell", "enable visual bell", options, 0x08),
        SPIFOPT_BOOL_LONG("scrollbar", "enable scrollbar", options, 0x10),
        SPIFOPT_BOOL_LONG("buttonbar", "enable buttonbar", options, 0x20),
        SPIFOPT_INT_LONG("color", "pick a color", color),
        SPIFOPT_ARGS_LONG("foo", "foo", foo)
    };

    TEST_BEGIN("spifopt_parse() function");
    SPIFOPT_OPTLIST_SET(opts1);
    SPIFOPT_NUMOPTS_SET(sizeof(opts1) / sizeof(spifopt_t));
    SPIFOPT_ALLOWBAD_SET(0);
    spifopt_parse(argc1, argv1);
    TEST_FAIL_IF(test_flag_var != 0x10);
    TEST_FAIL_IF(file_var != NULL);
    TEST_FAIL_IF(exec_list != NULL);
    TEST_FAIL_IF(num_var != 0);
    TEST_FAIL_IF(geom_var != 0);
    spifopt_parse(argc1, argv1);
    TEST_FAIL_IF(test_flag_var != 0x13);
    TEST_FAIL_IF(file_var == NULL);
    TEST_FAIL_IF(strcmp(file_var, "somefile"));
    TEST_FAIL_IF(exec_list == NULL);
    TEST_FAIL_IF(num_var != 1);
    TEST_FAIL_IF(geom_var != 3);

    SPIFOPT_FLAGS_CLEAR(SPIFOPT_SETTING_POSTPARSE);
    SPIFOPT_OPTLIST_SET(opts2);
    SPIFOPT_NUMOPTS_SET(sizeof(opts2) / sizeof(spifopt_t));
    SPIFOPT_ALLOWBAD_SET(0);
    spifopt_parse(argc2, argv2);
    TEST_FAIL_IF(strcmp(display, "foo:0"));
    TEST_FAIL_IF(name != NULL);
    TEST_FAIL_IF(strcmp(theme, "mytheme"));
    TEST_FAIL_IF(exec == NULL);
    TEST_FAIL_IF(strcmp(exec[0], "ssh"));
    TEST_FAIL_IF(strcmp(exec[1], "foo@bar.com"));
    TEST_FAIL_IF(exec[2] != NULL);
    TEST_FAIL_IF(foo != NULL);
    TEST_FAIL_IF(color != 0);
    TEST_FAIL_IF(options != 0x08);
    spifopt_parse(argc2, argv2);
    TEST_FAIL_IF(strcmp(display, "foo:0"));
    TEST_FAIL_IF(strcmp(name, "This is a name"));
    TEST_FAIL_IF(strcmp(theme, "mytheme"));
    TEST_FAIL_IF(exec == NULL);
    TEST_FAIL_IF(strcmp(exec[0], "ssh"));
    TEST_FAIL_IF(strcmp(exec[1], "foo@bar.com"));
    TEST_FAIL_IF(exec[2] != NULL);
    TEST_FAIL_IF(foo == NULL);
    TEST_FAIL_IF(strcmp(foo[0], "blah"));
    TEST_FAIL_IF(strcmp(foo[1], "-d"));
    TEST_FAIL_IF(strcmp(foo[2], "eatme"));
    TEST_FAIL_IF(foo[3] != NULL);
    TEST_FAIL_IF(color != 4);
    TEST_FAIL_IF(options != 0x1e);

    TEST_PASS();

    TEST_PASSED("options");
    return 0;
}

int
test_obj(void)
{
    spif_obj_t testobj;
    spif_class_t cls;

    TEST_BEGIN("spif_obj_new() function");
    testobj = spif_obj_new();
    TEST_FAIL_IF(SPIF_OBJ_ISNULL(testobj));
    TEST_PASS();

    TEST_BEGIN("spif_obj_get_classname() function");
    cls = spif_obj_get_class(testobj);
    TEST_FAIL_IF(cls != SPIF_CLASS_VAR(obj));
    TEST_PASS();

    TEST_BEGIN("spif_obj_del() function");
    TEST_FAIL_IF(spif_obj_del(testobj) != TRUE);
    TEST_PASS();

    TEST_PASSED("spif_obj_t");
    return 0;
}

int
test_str(void)
{
    spif_str_t teststr, test2str;
    spif_class_t cls;
    spif_char_t tmp[] = "this is a test";
    spif_char_t buff[4096] = "abcde";
    spif_char_t tmp2[] = "string #1\nstring #2";
    FILE *fp;
    int fd, mypipe[2];
    spif_charptr_t foo;

    TEST_BEGIN("spif_str_new() function");
    teststr = spif_str_new();
    TEST_FAIL_IF(SPIF_OBJ_ISNULL(teststr));
    TEST_PASS();

    TEST_BEGIN("spif_obj_get_classname() function");
    cls = spif_obj_get_class(SPIF_OBJ(teststr));
    TEST_FAIL_IF(cls != SPIF_CLASS_VAR(str));
    TEST_PASS();

    TEST_BEGIN("spif_str_del() function");
    TEST_FAIL_IF(spif_str_del(teststr) != TRUE);
    TEST_PASS();

    TEST_BEGIN("spif_str_new_from_ptr() function");
    teststr = spif_str_new_from_ptr(tmp);
    TEST_FAIL_IF(spif_str_cmp_with_ptr(teststr, tmp));
    TEST_FAIL_IF(spif_str_get_size(teststr) != sizeof(tmp));
    TEST_FAIL_IF(spif_str_get_len(teststr) != (sizeof(tmp) - 1));
    spif_str_del(teststr);
    TEST_PASS();

    TEST_BEGIN("spif_str_new_from_buff() function");
    teststr = spif_str_new_from_buff(buff, sizeof(buff));
    TEST_FAIL_IF(spif_str_casecmp_with_ptr(teststr, buff));
    TEST_FAIL_IF(spif_str_get_size(teststr) != sizeof(buff));
    TEST_FAIL_IF(spif_str_get_len(teststr) != 5);
    spif_str_del(teststr);
    TEST_PASS();

    TEST_BEGIN("spif_str_new_from_fp() function");
    pipe(mypipe);
    fd = mypipe[0];
    fp = fdopen(fd, "r");
    write(mypipe[1], tmp2, sizeof(tmp2));
    close(mypipe[1]);
    TEST_FAIL_IF(fp == NULL);
    teststr = spif_str_new_from_fp(fp);
    TEST_FAIL_IF(spif_str_ncmp_with_ptr(teststr, "string #1", 9));
    TEST_FAIL_IF(spif_str_get_len(teststr) != 9);
    TEST_FAIL_IF(spif_str_get_size(teststr) != 10);
    spif_str_del(teststr);
    teststr = spif_str_new_from_fp(fp);
    TEST_FAIL_IF(spif_str_ncasecmp_with_ptr(teststr, "string #2", 9));
    TEST_FAIL_IF(spif_str_get_len(teststr) != 9);
    TEST_FAIL_IF(spif_str_get_size(teststr) != 10);
    spif_str_del(teststr);
    fclose(fp);
    TEST_PASS();

    TEST_BEGIN("spif_str_new_from_fd() function");
    pipe(mypipe);
    fd = mypipe[0];
    write(mypipe[1], tmp2, sizeof(tmp2) - 1);
    close(mypipe[1]);
    teststr = spif_str_new_from_fd(fd);
    TEST_FAIL_IF(strcmp(SPIF_STR_STR(teststr), tmp2));
    TEST_FAIL_IF(spif_str_get_len(teststr) != (sizeof(tmp2) - 1));
    TEST_FAIL_IF(spif_str_get_size(teststr) != sizeof(tmp2));
    spif_str_del(teststr);
    close(fd);
    TEST_PASS();

    TEST_BEGIN("spif_str_dup() function");
    teststr = spif_str_new_from_ptr(tmp);
    TEST_FAIL_IF(strcmp(SPIF_STR_STR(teststr), tmp));
    TEST_FAIL_IF(spif_str_get_size(teststr) != sizeof(tmp));
    TEST_FAIL_IF(spif_str_get_len(teststr) != (sizeof(tmp) - 1));
    test2str = spif_str_dup(teststr);
    TEST_FAIL_IF(test2str == teststr);
    TEST_FAIL_IF(SPIF_STR_STR(test2str) == SPIF_STR_STR(teststr));
    TEST_FAIL_IF(spif_str_cmp(teststr, test2str));
    TEST_FAIL_IF(spif_str_casecmp(teststr, test2str));
    TEST_FAIL_IF(spif_str_ncmp(teststr, test2str, spif_str_get_len(teststr)));
    TEST_FAIL_IF(spif_str_ncasecmp(teststr, test2str, spif_str_get_len(test2str)));
    TEST_FAIL_IF(strcmp(SPIF_STR_STR(test2str), tmp));
    TEST_FAIL_IF(spif_str_get_size(test2str) != sizeof(tmp));
    TEST_FAIL_IF(spif_str_get_len(test2str) != (sizeof(tmp) - 1));
    spif_str_del(teststr);
    spif_str_del(test2str);
    TEST_PASS();

    TEST_BEGIN("spif_str_index() function");
    teststr = spif_str_new_from_ptr(tmp2);
    TEST_FAIL_IF(spif_str_index(teststr, '#') != 7);
    TEST_PASS();

    TEST_BEGIN("spif_str_rindex() function");
    TEST_FAIL_IF(spif_str_rindex(teststr, '#') != 17);
    TEST_PASS();

    TEST_BEGIN("spif_str_find() function");
    test2str = spif_str_new_from_ptr("ring");
    TEST_FAIL_IF(spif_str_find(teststr, test2str) != 2);
    spif_str_del(test2str);
    TEST_PASS();

    TEST_BEGIN("spif_str_find_from_ptr() function");
    TEST_FAIL_IF(spif_str_find_from_ptr(teststr, "in") != 3);
    spif_str_del(teststr);
    TEST_PASS();

    TEST_BEGIN("spif_str_substr() function");
    teststr = spif_str_new_from_ptr(tmp);
    test2str = spif_str_substr(teststr, 2, 5);
    TEST_FAIL_IF(spif_str_cmp_with_ptr(test2str, "is is"));
    TEST_FAIL_IF(spif_str_get_size(test2str) != 6);
    TEST_FAIL_IF(spif_str_get_len(test2str) != 5);
    spif_str_del(test2str);
    test2str = spif_str_substr(teststr, -4, 4);
    TEST_FAIL_IF(spif_str_cmp_with_ptr(test2str, "test"));
    TEST_FAIL_IF(spif_str_get_size(test2str) != 5);
    TEST_FAIL_IF(spif_str_get_len(test2str) != 4);
    spif_str_del(test2str);
    spif_str_del(teststr);
    TEST_PASS();

    TEST_BEGIN("spif_str_substr_to_ptr() function");
    teststr = spif_str_new_from_ptr(tmp);
    foo = spif_str_substr_to_ptr(teststr, 2, 5);
    TEST_FAIL_IF(foo == NULL);
    TEST_FAIL_IF(strcmp(foo, "is is"));
    FREE(foo);
    foo = spif_str_substr_to_ptr(teststr, -4, 4);
    TEST_FAIL_IF(foo == NULL);
    TEST_FAIL_IF(strcmp(foo, "test"));
    FREE(foo);
    spif_str_del(teststr);
    TEST_PASS();

    TEST_BEGIN("spif_str_to_num() function");
    teststr = spif_str_new_from_ptr("11001001");
    TEST_FAIL_IF(spif_str_to_num(teststr, 2) != 201);
    TEST_FAIL_IF(spif_str_to_num(teststr, 10) != 11001001);
    spif_str_del(teststr);
    teststr = spif_str_new_from_ptr("0132");
    TEST_FAIL_IF(spif_str_to_num(teststr, 0) != 90);
    TEST_FAIL_IF(spif_str_to_num(teststr, 8) != 90);
    TEST_FAIL_IF(spif_str_to_num(teststr, 10) != 132);
    spif_str_del(teststr);
    teststr = spif_str_new_from_ptr("0xff");
    TEST_FAIL_IF(spif_str_to_num(teststr, 0) != 255);
    TEST_FAIL_IF(spif_str_to_num(teststr, 10) != 0);
    spif_str_del(teststr);
    TEST_PASS();

    TEST_BEGIN("spif_str_to_float() function");
    teststr = spif_str_new_from_ptr("3.1415");
    TEST_FAIL_IF(spif_str_to_float(teststr) != 3.1415);
    spif_str_del(teststr);
    teststr = spif_str_new_from_ptr("2.71");
    TEST_FAIL_IF(spif_str_to_float(teststr) != 2.71);
    spif_str_del(teststr);
    TEST_PASS();

    TEST_BEGIN("spif_str_append() function");
    teststr = spif_str_new_from_ptr("copy");
    test2str = spif_str_new_from_ptr("cat");
    spif_str_append(teststr, test2str);
    TEST_FAIL_IF(spif_str_cmp_with_ptr(teststr, "copycat"));
    TEST_FAIL_IF(spif_str_get_size(teststr) != 8);
    TEST_FAIL_IF(spif_str_get_len(teststr) != 7);
    spif_str_del(test2str);
    TEST_PASS();

    TEST_BEGIN("spif_str_append_char() function");
    test2str = spif_str_new_from_ptr("Hello");
    spif_str_append_char(test2str, ' ');
    spif_str_append_char(test2str, 'w');
    spif_str_append_char(test2str, 'o');
    spif_str_append_char(test2str, 'r');
    spif_str_append_char(test2str, 'l');
    spif_str_append_char(test2str, 'd');
    TEST_FAIL_IF(spif_str_cmp_with_ptr(test2str, "Hello world"));
    TEST_FAIL_IF(spif_str_get_size(test2str) != 12);
    TEST_FAIL_IF(spif_str_get_len(test2str) != 11);
    spif_str_del(test2str);
    TEST_PASS();

    TEST_BEGIN("spif_str_append_from_ptr() function");
    spif_str_append_from_ptr(teststr, "crime");
    TEST_FAIL_IF(spif_str_cmp_with_ptr(teststr, "copycatcrime"));
    TEST_FAIL_IF(spif_str_get_size(teststr) != 13);
    TEST_FAIL_IF(spif_str_get_len(teststr) != 12);
    spif_str_del(teststr);
    TEST_PASS();

    TEST_BEGIN("spif_str_clear() function");
    teststr = spif_str_new_from_ptr("abcdefg");
    spif_str_clear(teststr, 'x');
    TEST_FAIL_IF(spif_str_cmp_with_ptr(teststr, "xxxxxxx"));
    TEST_FAIL_IF(spif_str_get_size(teststr) != 8);
    TEST_FAIL_IF(spif_str_get_len(teststr) != 7);
    spif_str_del(teststr);
    TEST_PASS();

    TEST_BEGIN("spif_str_trim() function");
    teststr = spif_str_new_from_ptr("  \n \r\f       \t    testing 1 2 3    \v\r \n");
    spif_str_trim(teststr);
    TEST_FAIL_IF(spif_str_cmp_with_ptr(teststr, "testing 1 2 3"));
    TEST_FAIL_IF(spif_str_get_size(teststr) != 14);
    TEST_FAIL_IF(spif_str_get_len(teststr) != 13);
    spif_str_del(teststr);
    TEST_PASS();

    TEST_BEGIN("spif_str_splice() function");
    teststr = spif_str_new_from_ptr(tmp);
    test2str = spif_str_new_from_ptr("lots of fun");
    spif_str_splice(teststr, 8, 6, test2str);
    TEST_FAIL_IF(spif_str_cmp_with_ptr(teststr, "this is lots of fun"));
    TEST_FAIL_IF(spif_str_get_size(teststr) != 20);
    TEST_FAIL_IF(spif_str_get_len(teststr) != 19);
    spif_str_del(test2str);
    spif_str_del(teststr);
    TEST_PASS();

    TEST_BEGIN("spif_str_splice_from_ptr() function");
    teststr = spif_str_new_from_ptr(tmp);
    spif_str_splice_from_ptr(teststr, 8, 0, "not ");
    TEST_FAIL_IF(spif_str_cmp_with_ptr(teststr, "this is not a test"));
    TEST_FAIL_IF(spif_str_get_size(teststr) != 19);
    TEST_FAIL_IF(spif_str_get_len(teststr) != 18);
    spif_str_del(teststr);
    TEST_PASS();

    TEST_BEGIN("spif_str_reverse() function");
    teststr = spif_str_new_from_buff(buff, sizeof(buff));
    spif_str_reverse(teststr);
    TEST_FAIL_IF(spif_str_cmp_with_ptr(teststr, "edcba"));
    TEST_FAIL_IF(spif_str_get_size(teststr) != sizeof(buff));
    TEST_FAIL_IF(spif_str_get_len(teststr) != 5);
    spif_str_del(teststr);
    TEST_PASS();

    TEST_PASSED("spif_str_t");
    return 0;
}

int
test_tok(void)
{
    spif_tok_t testtok;
    spif_str_t teststr;
    spif_list_t testlist;
    spif_class_t cls;
    spif_char_t tmp[] = "I \"can\'t\" feel my legs!";
    spif_char_t tmp2[] = ":::some:seedy:colon-delimited::data";
    spif_char_t tmp3[] = "\"this is one token\" and this \'over here\' is \"another one\"";
    spif_char_t tmp4[] = "\"there shouldn't be\"\' any problems at\'\"\"\'\'\' \'\"all parsing this\"";

    TEST_BEGIN("spif_tok_new() function");
    testtok = spif_tok_new();
    TEST_FAIL_IF(SPIF_OBJ_ISNULL(testtok));
    TEST_PASS();

    TEST_BEGIN("spif_obj_get_classname() function");
    cls = spif_obj_get_class(SPIF_OBJ(testtok));
    TEST_FAIL_IF(cls != SPIF_CLASS_VAR(tok));
    TEST_PASS();

    TEST_BEGIN("spif_tok_del() function");
    TEST_FAIL_IF(spif_tok_del(testtok) != TRUE);
    TEST_PASS();

    TEST_BEGIN("spif_tok_new_from_ptr() function");
    testtok = spif_tok_new_from_ptr(tmp);
    TEST_FAIL_IF(spif_str_cmp_with_ptr(spif_tok_get_src(testtok), tmp));
    spif_tok_del(testtok);
    TEST_PASS();

    TEST_BEGIN("spif_tok_eval() function");
    testtok = spif_tok_new_from_ptr(tmp);
    TEST_FAIL_IF(SPIF_TOK_ISNULL(testtok));
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(spif_tok_get_src(testtok), tmp)));
    spif_tok_eval(testtok);
    testlist = SPIF_CAST(list) spif_tok_get_tokens(testtok);
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 0);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "I")));
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 1);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "can't")));
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 2);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "feel")));
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 3);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "my")));
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 4);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "legs!")));
    spif_tok_del(testtok);

    testtok = spif_tok_new_from_ptr(tmp2);
    TEST_FAIL_IF(SPIF_TOK_ISNULL(testtok));
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(spif_tok_get_src(testtok), tmp2)));
    teststr = spif_str_new_from_ptr(":");
    spif_tok_set_sep(testtok, teststr);
    spif_str_del(teststr);
    spif_tok_eval(testtok);
    testlist = SPIF_CAST(list) spif_tok_get_tokens(testtok);
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 0);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "some")));
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 1);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "seedy")));
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 2);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "colon-delimited")));
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 3);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "data")));
    spif_tok_del(testtok);

    testtok = spif_tok_new_from_ptr(tmp3);
    TEST_FAIL_IF(SPIF_TOK_ISNULL(testtok));
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(spif_tok_get_src(testtok), tmp3)));
    spif_tok_eval(testtok);
    testlist = SPIF_CAST(list) spif_tok_get_tokens(testtok);
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 0);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "this is one token")));
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 1);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "and")));
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 2);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "this")));
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 3);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "over here")));
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 4);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "is")));
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 5);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "another one")));
    spif_tok_del(testtok);

    testtok = spif_tok_new_from_ptr(tmp4);
    TEST_FAIL_IF(SPIF_TOK_ISNULL(testtok));
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(spif_tok_get_src(testtok), tmp4)));
    spif_tok_eval(testtok);
    testlist = SPIF_CAST(list) spif_tok_get_tokens(testtok);
    teststr = SPIF_CAST(str) SPIF_LIST_GET(testlist, 0);
    TEST_FAIL_IF(!SPIF_CMP_IS_EQUAL(spif_str_cmp_with_ptr(teststr, "there shouldn't be any problems at all parsing this")));
    spif_tok_del(testtok);

    TEST_PASS();


    TEST_PASSED("spif_tok_t");
    return 0;
}

int
test_list(void)
{
    unsigned short i;
    spif_list_t testlist;
    spif_str_t s;

    for (i = 0; i < 3; i++) {
        if (i == 0) {
            TEST_NOTICE("Testing list interface class, linked_list instance:");
            testlist = SPIF_LIST_NEW(linked_list);
        } else if (i == 1) {
            TEST_NOTICE("Testing list interface class, dlinked_list instance:");
            testlist = SPIF_LIST_NEW(dlinked_list);
        } else if (i == 2) {
            TEST_NOTICE("Testing list interface class, array instance:");
            testlist = SPIF_LIST_NEW(array);
        } else if (i == 3) {
        }

        TEST_BEGIN("SPIF_LIST_APPEND() and SPIF_LIST_PREPEND() macros");
        SPIF_LIST_APPEND(testlist, spif_str_new_from_ptr("1"));
        SPIF_LIST_APPEND(testlist, spif_str_new_from_ptr("2"));
        SPIF_LIST_APPEND(testlist, spif_str_new_from_ptr("3"));
        SPIF_LIST_APPEND(testlist, spif_str_new_from_ptr("4"));
        SPIF_LIST_APPEND(testlist, spif_str_new_from_ptr("5"));
        SPIF_LIST_PREPEND(testlist, spif_str_new_from_ptr("0"));

        s = spif_str_new_from_ptr("0");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 0);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "1");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 1);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "2");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 2);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "3");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 3);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "4");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 4);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "5");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 5);
        spif_str_del(s);
        TEST_PASS();

        TEST_BEGIN("SPIF_LIST_CONTAINS() macro");
        s = spif_str_new_from_ptr("0");
        TEST_FAIL_IF(!SPIF_LIST_CONTAINS(testlist, s));
        spif_str_done(s);
        spif_str_init_from_ptr(s, "3");
        TEST_FAIL_IF(!SPIF_LIST_CONTAINS(testlist, s));
        spif_str_done(s);
        spif_str_init_from_ptr(s, "5");
        TEST_FAIL_IF(!SPIF_LIST_CONTAINS(testlist, s));
        spif_str_del(s);
        TEST_PASS();

        TEST_BEGIN("SPIF_LIST_COUNT() macro");
        TEST_FAIL_IF(SPIF_LIST_COUNT(testlist) != 6);
        TEST_PASS();

        TEST_BEGIN("SPIF_LIST_GET() macro");
        s = spif_str_new_from_ptr("2");
        TEST_FAIL_IF(SPIF_STR_COMP(s, SPIF_STR(SPIF_LIST_GET(testlist, 2))) != SPIF_CMP_EQUAL);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "4");
        TEST_FAIL_IF(SPIF_STR_COMP(s, SPIF_STR(SPIF_LIST_GET(testlist, 4))) != SPIF_CMP_EQUAL);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "6");
        TEST_FAIL_IF(!SPIF_STR_ISNULL(SPIF_STR(SPIF_LIST_GET(testlist, 6))));
        spif_str_del(s);
        TEST_PASS();

        TEST_BEGIN("SPIF_LIST_INDEX() macro");
        s = spif_str_new_from_ptr("4");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 4);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "1");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 1);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "Q");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != ((size_t) -1));
        spif_str_del(s);
        TEST_PASS();

        TEST_BEGIN("SPIF_LIST_INSERT() macro");
        SPIF_LIST_INSERT(testlist, spif_str_new_from_ptr("2.5"));
        SPIF_LIST_INSERT(testlist, spif_str_new_from_ptr("4.5"));
        TEST_FAIL_IF(SPIF_LIST_COUNT(testlist) != 8);
        s = spif_str_new_from_ptr("0");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 0);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "2.5");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 3);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "3");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 4);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "4.5");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 6);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "5");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 7);
        spif_str_del(s);
        TEST_PASS();

        TEST_BEGIN("SPIF_LIST_INSERT_AT() macro");
        SPIF_LIST_INSERT_AT(testlist, spif_str_new_from_ptr("MOO"), 0);
        SPIF_LIST_INSERT_AT(testlist, spif_str_new_from_ptr("GRIN"), 4);
        TEST_FAIL_IF(SPIF_LIST_COUNT(testlist) != 10);
        s = spif_str_new_from_ptr("MOO");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 0);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "0");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 1);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "1");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 2);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "GRIN");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 4);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "5");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 9);
        spif_str_del(s);
        TEST_PASS();

        TEST_BEGIN("SPIF_LIST_REMOVE() macro");
        s = spif_str_new_from_ptr("MOO");
        TEST_FAIL_IF(SPIF_OBJ_ISNULL(SPIF_LIST_REMOVE(testlist, s)));
        spif_str_done(s);
        spif_str_init_from_ptr(s, "GRIN");
        TEST_FAIL_IF(SPIF_OBJ_ISNULL(SPIF_LIST_REMOVE(testlist, s)));
        spif_str_del(s);

        s = spif_str_new_from_ptr("0");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 0);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "2.5");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 3);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "3");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 4);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "4.5");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 6);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "5");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 7);
        spif_str_del(s);
        TEST_PASS();

        TEST_BEGIN("SPIF_LIST_REMOVE_AT() macro");
        SPIF_LIST_REMOVE_AT(testlist, 6);
        SPIF_LIST_REMOVE_AT(testlist, 3);

        s = spif_str_new_from_ptr("0");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 0);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "1");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 1);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "2");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 2);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "3");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 3);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "4");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 4);
        spif_str_done(s);
        spif_str_init_from_ptr(s, "5");
        TEST_FAIL_IF(SPIF_LIST_INDEX(testlist, s) != 5);
        spif_str_del(s);
        TEST_PASS();

        /*SPIF_SHOW(testlist, stdout);*/
    }


    TEST_PASSED("list interface class");
    return 0;
}

int
main(int argc, char *argv[])
{
    int ret = 0;

    USE_VAR(argc);
    USE_VAR(argv);

    if ((ret = test_macros()) != 0) {
        return ret;
    }
    if ((ret = test_mem()) != 0) {
        return ret;
    }
    if ((ret = test_strings()) != 0) {
        return ret;
    }
    if ((ret = test_snprintf()) != 0) {
        return ret;
    }
    if ((ret = test_options()) != 0) {
        return ret;
    }
    if ((ret = test_obj()) != 0) {
        return ret;
    }
    if ((ret = test_str()) != 0) {
        return ret;
    }
    if ((ret = test_tok()) != 0) {
        return ret;
    }
    if ((ret = test_list()) != 0) {
        return ret;
    }

    printf("All tests passed.\n\n");
    return 0;
}
