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
#include "perf.h"

unsigned long tnum = 0;
struct timeval time1, time2;
double time_diff;
long prof_counter;

int perf_macros(void);
int perf_strings(void);
int perf_obj(void);
int perf_str(void);
int perf_tok(void);

int
perf_macros(void)
{
    char memset_test[] = "abcdefghijklmnopqrstuvwxyz";
    char sc1 = 'X', sc2 = 'K';
    int si1 = 472, si2 = 8786345;
    unsigned long sl1 = 0x98765432, sl2 = 0xffeeddff;

    PERF_BEGIN("MEMSET() macro");
    PERF_TEST(MEMSET(memset_test, '!', CONST_STRLEN(memset_test)););
    PERF_END();

    PERF_BEGIN("SWAP() macro");
    PERF_TEST(SWAP(sc1, sc2););
    PERF_TEST(SWAP(si1, si2););
    PERF_TEST(SWAP(sl1, sl2););
    PERF_END();

    PERF_BEGIN("BEG_STRCASECMP() macro");
    PERF_TEST(BEG_STRCASECMP("this", "this is a test"););
    PERF_TEST(BEG_STRCASECMP("thx", "this is another test"););
    PERF_TEST(BEG_STRCASECMP("this is still another test", "this is"););
    PERF_END();

    PERF_ENDED("macro");
}

int
perf_strings(void)
{
    char *s1, *s2, *s3, *s4;
#ifdef HAVE_REGEX_H
    regex_t *r = NULL;
#endif
    char **slist;

    PERF_BEGIN("left_str() function");
    PERF_TEST(s1 = left_str("bugger all", 3););
    PERF_TEST(s2 = left_str("testing 1-2-3", 7););
    PERF_TEST(s3 = left_str(NULL, 0););
    PERF_TEST(s4 = left_str("eat me", 0););
    FREE(s1);
    FREE(s2);
    FREE(s3);
    FREE(s4);
    PERF_END();

    PERF_BEGIN("mid_str() function");
    PERF_TEST(s1 = mid_str("pneumonoultramicroscopicsilicovolcanoconiosis", 8, 16););
    PERF_TEST(s2 = mid_str("abc", 7, 5););
    PERF_TEST(s3 = mid_str(NULL, 0, 0););
    PERF_TEST(s4 = mid_str("what the heck", -5, 42););
    FREE(s1);
    FREE(s2);
    FREE(s3);
    FREE(s4);
    PERF_END();

    PERF_BEGIN("right_str() function");
    PERF_TEST(s1 = right_str("bugger all", 3););
    PERF_TEST(s2 = right_str("testing 1-2-3", 5););
    PERF_TEST(s3 = right_str(NULL, 0););
    PERF_TEST(s4 = right_str("eat me", 0););
    FREE(s1);
    FREE(s2);
    FREE(s3);
    FREE(s4);
    PERF_END();

#ifdef HAVE_REGEX_H
    PERF_BEGIN("regexp_match() function");
    PERF_TEST(regexp_match("One particular string", "part"););
    PERF_TEST(regexp_match("Some other strange string", "^[A-Za-z]+$"););
    PERF_TEST(regexp_match("some-rpm-package-1.0.1-4.src.rpm", "^(.*)-([^-]+)-([^-])\\.([a-z0-9]+)\\.rpm$"););
    PERF_TEST(regexp_match("/the/path/to/some/odd/file.txt", "/this/should/not/match"););
    PERF_TEST(regexp_match("1600x1200", "[[:digit:]]+x[[:digit:]]+"););
    PERF_TEST(regexp_match("xxx", NULL););
    PERF_TEST(regexp_match_r("AbCdEfGhIjKlMnOpQrStUvWxYz", "[[:upper:]]", &r););
    PERF_TEST(regexp_match_r("abcdefjhijklmnopqrstuvwxyz", NULL, &r););
    PERF_TEST(regexp_match_r("aaaaa", "[[:lower:]]", &r););
    PERF_END();
#endif

    PERF_BEGIN("split() function");
    PERF_TEST(slist = split(" ", "Splitting a string on spaces"););
    free_array((void **) slist, 0);
    PERF_TEST(slist = split(NULL, "          a\t \ta        a a a a       a     "););
    free_array((void **) slist, 0);
    PERF_TEST(slist = split(NULL, "  first \"just the second\" third \'fourth and \'\"fifth to\"gether last"););
    free_array((void **) slist, 0);
    PERF_TEST(slist = split(NULL, "\'don\\\'t\' try this    at home \"\" "););
    free_array((void **) slist, 0);
    PERF_TEST(slist = split(":", "A:B:C:D:::E"););
    free_array((void **) slist, 0);
    PERF_END();

    PERF_ENDED("string");
    return 0;
}

int
perf_obj(void)
{
    spif_obj_t testobj;
    spif_class_t cls;

    PERF_BEGIN("spif_obj_create_delete");
    PERF_TEST(testobj = spif_obj_new(););
    PERF_TEST(spif_obj_del(testobj););
    PERF_END();

    testobj = spif_obj_new();
    PERF_BEGIN("spif_obj_get_classname");
    PERF_TEST(cls = spif_obj_get_class(testobj););
    PERF_END();
    spif_obj_del(testobj);

    PERF_ENDED("spif_obj_t");
    return 0;
}

int
perf_str(void)
{
    spif_str_t teststr, test2str;
    spif_class_t cls;
    signed char tmp[] = "this is a test";
    signed char buff[4096] = "abcde";
    signed char tmp2[] = "string #1\nstring #2";
    spif_charptr_t foo;

    PERF_BEGIN("spif_str_new_del");
    PERF_TEST(teststr = spif_str_new(););
    PERF_TEST(spif_str_del(teststr););
    PERF_END();

    teststr = spif_str_new();
    PERF_BEGIN("spif_obj_get_classname");
    PERF_TEST(cls = spif_obj_get_class(SPIF_OBJ(teststr)););
    PERF_END();
    spif_str_del(teststr);

    PERF_BEGIN("spif_str_new_del_from_ptr");
    PERF_TEST(teststr = spif_str_new_from_ptr(tmp););
    PERF_TEST(spif_str_del(teststr););
    PERF_END();

    PERF_BEGIN("spif_str_new_del_from_buff");
    PERF_TEST(teststr = spif_str_new_from_buff(buff, sizeof(buff)););
    PERF_TEST(spif_str_del(teststr););
    PERF_END();

    PERF_BEGIN("spif_str_dup");
    PERF_TEST(teststr = spif_str_new_from_ptr(tmp););
    PERF_TEST(test2str = spif_str_dup(teststr););
    PERF_TEST(spif_str_del(teststr););
    PERF_TEST(spif_str_del(test2str););
    PERF_END();

    teststr = spif_str_new_from_ptr(tmp2);

    PERF_BEGIN("spif_str_index");
    PERF_TEST(spif_str_index(teststr, '#'););
    PERF_END();

    PERF_BEGIN("spif_str_rindex");
    PERF_TEST(spif_str_rindex(teststr, '#'););
    PERF_END();

    test2str = spif_str_new_from_ptr(SPIF_CAST(charptr) "ring");
    PERF_BEGIN("spif_str_find");
    PERF_TEST(spif_str_find(teststr, test2str););
    PERF_END();
    spif_str_del(test2str);

    PERF_BEGIN("spif_str_find_from_ptr");
    PERF_TEST(spif_str_find_from_ptr(teststr, SPIF_CAST(charptr) "in"););
    PERF_END();

    spif_str_del(teststr);

    teststr = spif_str_new_from_ptr(tmp);
    PERF_BEGIN("spif_str_substr");
    PERF_TEST(test2str = spif_str_substr(teststr, 2, 5););
    PERF_TEST(spif_str_del(test2str););
    PERF_TEST(test2str = spif_str_substr(teststr, -4, 4););
    PERF_TEST(spif_str_del(test2str););
    PERF_END();
    spif_str_del(teststr);

    teststr = spif_str_new_from_ptr(tmp);
    PERF_BEGIN("spif_str_substr_to_ptr");
    PERF_TEST(foo = spif_str_substr_to_ptr(teststr, 2, 5););
    FREE(foo);
    PERF_TEST(foo = spif_str_substr_to_ptr(teststr, -4, 4););
    FREE(foo);
    PERF_END();
    spif_str_del(teststr);

    teststr = spif_str_new_from_ptr(SPIF_CAST(charptr) "11001001");
    PERF_BEGIN("spif_str_to_num");
    PERF_TEST(spif_str_to_num(teststr, 10););
    PERF_END();
    spif_str_del(teststr);

    teststr = spif_str_new_from_ptr(SPIF_CAST(charptr) "3.1415");
    PERF_BEGIN("spif_str_to_float");
    PERF_TEST(spif_str_to_float(teststr););
    PERF_END();
    spif_str_del(teststr);

    teststr = spif_str_new_from_ptr(SPIF_CAST(charptr) "  \n \r\f       \t    testing 1 2 3    \v\r \n");
    PERF_BEGIN("spif_str_trim");
    PERF_TEST(spif_str_trim(teststr););
    PERF_END();
    spif_str_del(teststr);

    teststr = spif_str_new_from_buff(buff, sizeof(buff));
    PERF_BEGIN("spif_str_reverse");
    PERF_TEST(spif_str_reverse(teststr););
    PERF_END();
    spif_str_del(teststr);

    PERF_ENDED("spif_str_t");
    return 0;
}

int
perf_tok(void)
{
    spif_tok_t testtok, test2tok;
    spif_class_t cls;
    signed char tmp[] = "I \"can\'t\" feel my legs!";
    signed char tmp2[] = ":::some:seedy:colon-delimited::data";
    signed char tmp3[] = "\"this is one token\" and this \'over here\' is \"another one\"";
    signed char tmp4[] = "\"there shouldn't be\"\' any problems at\'\"\"\'\'\' \'\"all parsing this\"";
    spif_charptr_t foo;

    PERF_BEGIN("spif_tok_new_del_from_ptr");
    PERF_TEST(testtok = spif_tok_new_from_ptr(tmp););
    PERF_TEST(spif_tok_del(testtok););
    PERF_END();

    PERF_ENDED("spif_tok_t");
    return 0;
}

int
main(int argc, char *argv[])
{
    int ret = 0;
    struct timeval t1, t2;

    USE_VAR(argc);
    USE_VAR(argv);


    gettimeofday(&t1, NULL);

    if ((ret = perf_macros()) != 0) {
        return ret;
    }
    if ((ret = perf_strings()) != 0) {
        return ret;
    }
    if ((ret = perf_obj()) != 0) {
        return ret;
    }
    if ((ret = perf_str()) != 0) {
        return ret;
    }
    if ((ret = perf_tok()) != 0) {
        return ret;
    }

    gettimeofday(&t2, NULL);

    printf("All profiling done, %6.5f seconds total.\n\n", TDIFF(t1, t2));
    return 0;
}
