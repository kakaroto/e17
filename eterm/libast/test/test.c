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
  TEST_FAIL_IF(s3);
  TEST_FAIL_IF(s4);
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
  TEST_FAIL_IF(s2);
  TEST_FAIL_IF(s3);
  TEST_FAIL_IF(s4);
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
  TEST_FAIL_IF(s3);
  TEST_FAIL_IF(s4);
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
main(int argc, char *argv[])
{
  int ret = 0;

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

  printf("All tests passed.\n\n");
  return 0;
}
