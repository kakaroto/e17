#include <stdlib.h>

#include <check.h>

#include "ecore_file_test.h"

int
main()
{
   Suite   *s;
   SRunner *sr;
   int      nbr_failed;

   s = ecore_file_tests_suite();
   sr = srunner_create(s);
   srunner_set_log(sr, "ecore_unit_tests.log");
   srunner_set_xml(sr, "ecore_unit_tests.xml");
   srunner_run_all(sr, CK_NORMAL);

   nbr_failed = srunner_ntests_failed(sr);
   srunner_free(sr);

   return (nbr_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
