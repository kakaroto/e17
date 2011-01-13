/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 */

#include <Eina.h>

#define ST(X) #X
#define STR(X) ST(X)
#define RUN_TEST(func) { \
  printf("Running " EINA_COLOR_WHITE "%-70s\n" EINA_COLOR_RESET, __FILE__ ": " #func); \
  if (!func()) failed = EINA_TRUE; \
  printf("Done    " EINA_COLOR_WHITE "%-70s[%s]\n" EINA_COLOR_RESET, \
         __FILE__ ": " #func, failed ? EINA_COLOR_RED "FAILED" EINA_COLOR_RESET : EINA_COLOR_GREEN "PASSED" EINA_COLOR_RESET); \
         }

#define TEST_ASSERT(cond) do { \
  Eina_Bool result = (cond); \
  print_result(result, __LINE__, #cond); \
  if (!result) return EINA_FALSE;\
} while (0)
