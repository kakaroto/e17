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

unsigned short tnum = 0;
time_t time1, time2;
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
  MEMSET(memset_test, '!', CONST_STRLEN(memset_test));
  PERF_END();

  PERF_BEGIN("SWAP() macro");
  SWAP(sc1, sc2);
  SWAP(si1, si2);
  SWAP(sl1, sl2);
  PERF_END();

  PERF_BEGIN("BEG_STRCASECMP() macro");
  BEG_STRCASECMP("this", "this is a test");
  BEG_STRCASECMP("thx", "this is another test");
  BEG_STRCASECMP("this is still another test", "this is");
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
  s1 = left_str("bugger all", 3);
  s2 = left_str("testing 1-2-3", 7);
  s3 = left_str(NULL, 0);
  s4 = left_str("eat me", 0);
  FREE(s1);
  FREE(s2);
  FREE(s3);
  FREE(s4);
  PERF_END();

  PERF_BEGIN("mid_str() function");
  s1 = mid_str("pneumonoultramicroscopicsilicovolcanoconiosis", 8, 16);
  s2 = mid_str("abc", 7, 5);
  s3 = mid_str(NULL, 0, 0);
  s4 = mid_str("what the heck", -5, 42);
  FREE(s1);
  FREE(s2);
  FREE(s3);
  FREE(s4);
  PERF_END();

  PERF_BEGIN("right_str() function");
  s1 = right_str("bugger all", 3);
  s2 = right_str("testing 1-2-3", 5);
  s3 = right_str(NULL, 0);
  s4 = right_str("eat me", 0);
  FREE(s1);
  FREE(s2);
  FREE(s3);
  FREE(s4);
  PERF_END();

#ifdef HAVE_REGEX_H
  PERF_BEGIN("regexp_match() function");
  regexp_match("One particular string", "part");
  regexp_match("Some other strange string", "^[A-Za-z]+$");
  regexp_match("some-rpm-package-1.0.1-4.src.rpm", "^(.*)-([^-]+)-([^-])\\.([a-z0-9]+)\\.rpm$");
  regexp_match("/the/path/to/some/odd/file.txt", "/this/should/not/match");
  regexp_match("1600x1200", "[[:digit:]]+x[[:digit:]]+");
  regexp_match("xxx", NULL);
  regexp_match_r("AbCdEfGhIjKlMnOpQrStUvWxYz", "[[:upper:]]", &r);
  regexp_match_r("abcdefjhijklmnopqrstuvwxyz", NULL, &r);
  regexp_match_r("aaaaa", "[[:lower:]]", &r);
  PERF_END();
#endif

  PERF_BEGIN("split() function");
  slist = split(" ", "Splitting a string on spaces");
  slist = split(NULL, "          a\t \ta        a a a a       a     ");
  slist = split(NULL, "  first \"just the second\" third \'fourth and \'\"fifth to\"gether last");
  slist = split(NULL, "\'don\\\'t\' try this    at home \"\" ");
  slist = split(":", "A:B:C:D:::E");
  PERF_END();

  PERF_ENDED("string");
  return 0;
}

int
perf_obj(void)
{
  spif_obj_t testobj;
  spif_classname_t cname;

  PERF_BEGIN("spif_obj_create_delete");
  testobj = spif_obj_new();
  spif_obj_del(testobj);
  PERF_END();

  testobj = spif_obj_new();
  PERF_BEGIN("spif_obj_get_classname");
  cname = spif_obj_get_classname(testobj);
  PERF_END();
  spif_obj_del(testobj);

  PERF_ENDED("spif_obj_t");
  return 0;
}

int
perf_str(void)
{
  spif_str_t teststr, test2str;
  spif_classname_t cname;
  signed char tmp[] = "this is a test";
  signed char buff[4096] = "abcde";
  signed char tmp2[] = "string #1\nstring #2";
  FILE *fp;
  int fd, mypipe[2];
  spif_charptr_t foo;

  PERF_BEGIN("spif_str_new_del");
  teststr = spif_str_new();
  spif_str_del(teststr);
  PERF_END();

  teststr = spif_str_new();
  PERF_BEGIN("spif_obj_get_classname");
  cname = spif_obj_get_classname(SPIF_OBJ(teststr));
  PERF_END();
  spif_str_del(teststr);

  PERF_BEGIN("spif_str_new_del_from_ptr");
  teststr = spif_str_new_from_ptr(tmp);
  spif_str_del(teststr);
  PERF_END();

  PERF_BEGIN("spif_str_new_del_from_buff");
  teststr = spif_str_new_from_buff(buff, sizeof(buff));
  spif_str_del(teststr);
  PERF_END();

  PERF_BEGIN("spif_str_dup");
  teststr = spif_str_new_from_ptr(tmp);
  test2str = spif_str_dup(teststr);
  spif_str_del(teststr);
  spif_str_del(test2str);
  PERF_END();

  teststr = spif_str_new_from_ptr(tmp2);

  PERF_BEGIN("spif_str_index");
  spif_str_index(teststr, '#');
  PERF_END();

  PERF_BEGIN("spif_str_rindex");
  spif_str_rindex(teststr, '#');
  PERF_END();

  test2str = spif_str_new_from_ptr("ring");
  PERF_BEGIN("spif_str_find");
  spif_str_find(teststr, test2str);
  PERF_END();
  spif_str_del(test2str);

  PERF_BEGIN("spif_str_find_from_ptr");
  spif_str_find_from_ptr(teststr, "in");
  PERF_END();

  spif_str_del(teststr);

  teststr = spif_str_new_from_ptr(tmp);
  PERF_BEGIN("spif_str_substr");
  test2str = spif_str_substr(teststr, 2, 5);
  spif_str_del(test2str);
  test2str = spif_str_substr(teststr, -4, 4);
  spif_str_del(test2str);
  PERF_END();
  spif_str_del(teststr);

  teststr = spif_str_new_from_ptr(tmp);
  PERF_BEGIN("spif_str_substr_to_ptr");
  foo = spif_str_substr_to_ptr(teststr, 2, 5);
  FREE(foo);
  foo = spif_str_substr_to_ptr(teststr, -4, 4);
  FREE(foo);
  PERF_END();
  spif_str_del(teststr);

  teststr = spif_str_new_from_ptr("11001001");
  PERF_BEGIN("spif_str_to_num");
  spif_str_to_num(teststr, 10);
  PERF_END();
  spif_str_del(teststr);

  teststr = spif_str_new_from_ptr("3.1415");
  PERF_BEGIN("spif_str_to_float");
  spif_str_to_float(teststr);
  PERF_END();
  spif_str_del(teststr);

  teststr = spif_str_new_from_ptr("  \n \r\f       \t    testing 1 2 3    \v\r \n");
  PERF_BEGIN("spif_str_trim");
  spif_str_trim(teststr);
  PERF_END();
  spif_str_del(teststr);

  teststr = spif_str_new_from_buff(buff, sizeof(buff));
  PERF_BEGIN("spif_str_reverse");
  spif_str_reverse(teststr);
  PERF_END();
  spif_str_del(teststr);

  PERF_ENDED("spif_str_t");
  return 0;
}

int
perf_tok(void)
{
  spif_tok_t testtok, test2tok;
  spif_classname_t cname;
  signed char tmp[] = "I \"can\'t\" feel my legs!";
  signed char tmp2[] = ":::some:seedy:colon-delimited::data";
  signed char tmp3[] = "\"this is one token\" and this \'over here\' is \"another one\"";
  signed char tmp4[] = "\"there shouldn't be\"\' any problems at\'\"\"\'\'\' \'\"all parsing this\"";
  spif_charptr_t foo;

  PERF_BEGIN("spif_tok_new_del_from_ptr");
  testtok = spif_tok_new_from_ptr(tmp);
  spif_tok_del(testtok);
  PERF_END();

  PERF_ENDED("spif_tok_t");
  return 0;
}

int
main(int argc, char *argv[])
{
  int ret = 0;
  time_t t1,t2;

  USE_VAR(argc);
  USE_VAR(argv);

 
  time(&t1);

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

  time(&t2);

  printf("All profiling done, time elapsed %d.\n\n", t2-t1);
  return 0;
}
