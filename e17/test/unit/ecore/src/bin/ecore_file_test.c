#include <Ecore_File.h>

#include <check.h>


START_TEST (ecore_file_test_size)
{
   long long size;

   ecore_file_init();

   size = ecore_file_size(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt");
   fail_unless(size == 15);

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_exists)
{
   int result;

   ecore_file_init();

   result = ecore_file_exists(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt");
   fail_unless(result != 0);

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_is_dir)
{
   int result;

   ecore_file_init();

   result = ecore_file_is_dir(PACKAGE_DATA_DIR "/ecore_unit_test");
   fail_unless(result != 0);

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_can_read)
{
   int result;

   ecore_file_init();

   result = ecore_file_can_read(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt");
   fail_unless(result != 0);

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_can_write)
{
   int result;

   ecore_file_init();

   result = ecore_file_can_write(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt");
   fail_unless(result != 0);

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_can_exec)
{
   int result;

   ecore_file_init();

   result = ecore_file_can_exec(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt");
   fail_unless(result == 0);

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_strip_ext)
{
   char *file;

   ecore_file_init();

   file = ecore_file_strip_ext(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt");
   fail_unless(strcmp(file, PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test") == 0);

   ecore_file_shutdown();
}
END_TEST

Suite *
ecore_file_tests_suite(void)
{
   Suite *s;
   TCase *tc;

   s = suite_create("Ecore File tests");
   tc = tcase_create("test");

   tcase_add_test(tc, ecore_file_test_size);
   tcase_add_test(tc, ecore_file_test_exists);
   tcase_add_test(tc, ecore_file_test_is_dir);
   tcase_add_test(tc, ecore_file_test_can_read);
   tcase_add_test(tc, ecore_file_test_can_write);
   tcase_add_test(tc, ecore_file_test_can_exec);
   tcase_add_test(tc, ecore_file_test_strip_ext);

   suite_add_tcase(s, tc);

   return s;
}
