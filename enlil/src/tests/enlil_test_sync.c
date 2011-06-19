#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>

#include "enlil_suite.h"
#include "Enlil.h"

START_TEST(enlil_test_simple)
{
   enlil_init();

   fail_if(NULL != NULL);

   enlil_shutdown();
}
END_TEST

void
enlil_test_sync(TCase *tc)
{
   tcase_add_test(tc, enlil_test_simple);
}
