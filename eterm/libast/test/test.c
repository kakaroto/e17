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
int test_obj(void);
int test_str(void);

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
test_obj(void)
{
  spif_obj_t testobj;
  spif_classname_t cname;

  TEST_BEGIN("spif_obj_new");
  testobj = spif_obj_new();
  TEST_FAIL_IF(SPIF_OBJ_ISNULL(testobj));
  TEST_PASS();

  TEST_BEGIN("spif_obj_get_classname");
  cname = spif_obj_get_classname(testobj);
  TEST_FAIL_IF(cname != SPIF_CLASSNAME(obj));
  TEST_PASS();

  TEST_BEGIN("spif_obj_del");
  TEST_FAIL_IF(spif_obj_del(testobj) != TRUE);
  TEST_PASS();

  TEST_PASSED("spif_obj_t");
  return 0;
}

int
test_str(void)
{
  spif_str_t teststr, test2str;
  spif_classname_t cname;
  signed char tmp[] = "this is a test";
  signed char buff[4096] = "abcde";
  signed char tmp2[] = "string #1\nstring #2";
  FILE *fp;
  int fd, mypipe[2];
  spif_charptr_t foo;

  TEST_BEGIN("spif_str_new");
  teststr = spif_str_new();
  TEST_FAIL_IF(SPIF_OBJ_ISNULL(teststr));
  TEST_PASS();

  TEST_BEGIN("spif_obj_get_classname");
  cname = spif_obj_get_classname(SPIF_OBJ(teststr));
  TEST_FAIL_IF(cname != SPIF_CLASSNAME(str));
  TEST_PASS();

  TEST_BEGIN("spif_str_del");
  TEST_FAIL_IF(spif_str_del(teststr) != TRUE);
  TEST_PASS();

  TEST_BEGIN("spif_str_new_from_ptr");
  teststr = spif_str_new_from_ptr(tmp);
  TEST_FAIL_IF(spif_str_cmp_with_ptr(teststr, tmp));
  TEST_FAIL_IF(spif_str_get_size(teststr) != sizeof(tmp));
  TEST_FAIL_IF(spif_str_get_len(teststr) != (sizeof(tmp) - 1));
  spif_str_del(teststr);
  TEST_PASS();

  TEST_BEGIN("spif_str_new_from_buff");
  teststr = spif_str_new_from_buff(buff, sizeof(buff));
  TEST_FAIL_IF(spif_str_casecmp_with_ptr(teststr, buff));
  TEST_FAIL_IF(spif_str_get_size(teststr) != sizeof(buff));
  TEST_FAIL_IF(spif_str_get_len(teststr) != 5);
  spif_str_del(teststr);
  TEST_PASS();

  TEST_BEGIN("spif_str_new_from_fp");
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

  TEST_BEGIN("spif_str_new_from_fd");
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

  TEST_BEGIN("spif_str_dup");
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

  TEST_BEGIN("spif_str_index");
  teststr = spif_str_new_from_ptr(tmp2);
  TEST_FAIL_IF(spif_str_index(teststr, '#') != 7);
  TEST_PASS();

  TEST_BEGIN("spif_str_rindex");
  TEST_FAIL_IF(spif_str_rindex(teststr, '#') != 17);
  TEST_PASS();

  TEST_BEGIN("spif_str_find");
  test2str = spif_str_new_from_ptr("ring");
  TEST_FAIL_IF(spif_str_find(teststr, test2str) != 2);
  spif_str_del(test2str);
  TEST_PASS();

  TEST_BEGIN("spif_str_find_from_ptr");
  TEST_FAIL_IF(spif_str_find_from_ptr(teststr, "in") != 3);
  spif_str_del(teststr);
  TEST_PASS();

  TEST_BEGIN("spif_str_substr");
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

  TEST_BEGIN("spif_str_substr_to_ptr");
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

  TEST_BEGIN("spif_str_to_num");
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

  TEST_BEGIN("spif_str_to_float");
  teststr = spif_str_new_from_ptr("3.1415");
  TEST_FAIL_IF(spif_str_to_float(teststr) != 3.1415);
  spif_str_del(teststr);
  teststr = spif_str_new_from_ptr("2.71");
  TEST_FAIL_IF(spif_str_to_float(teststr) != 2.71);
  spif_str_del(teststr);
  TEST_PASS();

  TEST_BEGIN("spif_str_append");
  teststr = spif_str_new_from_ptr("copy");
  test2str = spif_str_new_from_ptr("cat");
  spif_str_append(teststr, test2str);
  TEST_FAIL_IF(spif_str_cmp_with_ptr(teststr, "copycat"));
  TEST_FAIL_IF(spif_str_get_size(teststr) != 8);
  TEST_FAIL_IF(spif_str_get_len(teststr) != 7);
  spif_str_del(test2str);
  TEST_PASS();

  TEST_BEGIN("spif_str_append_from_ptr");
  spif_str_append_from_ptr(teststr, "crime");
  TEST_FAIL_IF(spif_str_cmp_with_ptr(teststr, "copycatcrime"));
  TEST_FAIL_IF(spif_str_get_size(teststr) != 13);
  TEST_FAIL_IF(spif_str_get_len(teststr) != 12);
  spif_str_del(teststr);
  TEST_PASS();

  TEST_BEGIN("spif_str_trim");
  teststr = spif_str_new_from_ptr("  \n \r\f       \t    testing 1 2 3    \v\r \n");
  spif_str_trim(teststr);
  TEST_FAIL_IF(spif_str_cmp_with_ptr(teststr, "testing 1 2 3"));
  TEST_FAIL_IF(spif_str_get_size(teststr) != 14);
  TEST_FAIL_IF(spif_str_get_len(teststr) != 13);
  spif_str_del(teststr);
  TEST_PASS();

  TEST_BEGIN("spif_str_splice");
  teststr = spif_str_new_from_ptr(tmp);
  test2str = spif_str_new_from_ptr("lots of fun");
  spif_str_splice(teststr, 8, 6, test2str);
  TEST_FAIL_IF(spif_str_cmp_with_ptr(teststr, "this is lots of fun"));
  TEST_FAIL_IF(spif_str_get_size(teststr) != 20);
  TEST_FAIL_IF(spif_str_get_len(teststr) != 19);
  spif_str_del(test2str);
  spif_str_del(teststr);
  TEST_PASS();

  TEST_BEGIN("spif_str_splice_from_ptr");
  teststr = spif_str_new_from_ptr(tmp);
  spif_str_splice_from_ptr(teststr, 8, 0, "not ");
  TEST_FAIL_IF(spif_str_cmp_with_ptr(teststr, "this is not a test"));
  TEST_FAIL_IF(spif_str_get_size(teststr) != 19);
  TEST_FAIL_IF(spif_str_get_len(teststr) != 18);
  spif_str_del(teststr);
  TEST_PASS();

  TEST_BEGIN("spif_str_reverse");
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
  if ((ret = test_obj()) != 0) {
    return ret;
  }
  if ((ret = test_str()) != 0) {
    return ret;
  }

  printf("All tests passed.\n\n");
  return 0;
}
