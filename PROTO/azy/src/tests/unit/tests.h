/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#include <string.h>

#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define NORMAL "\033[0;39m"
#define WHITE "\033[1;37m"
#define ST(X) #X
#define STR(X) ST(X)
#define RUN_TEST(func) { \
  printf("Running " WHITE "%-70s\n" NORMAL, __FILE__ ": " #func); \
  int rs = func(); if (!rs) failed = EINA_TRUE; \
  printf("Done    " WHITE "%-70s[%s]\n" NORMAL, __FILE__ ": " #func, !rs ? RED "FAILED" NORMAL : GREEN "PASSED" NORMAL); }

#define TEST_ASSERT(cond) { \
  int result = (cond); \
  printf("        " WHITE "%-70s[%s]\n" NORMAL, __FILE__ "["STR(__LINE__)"]: " #cond, !(result) ? RED "FAILED" NORMAL : GREEN "PASSED" NORMAL); \
  if (!result) \
    return EINA_FALSE; }
