#include <stdlib.h>
#include <string.h>

#include <Ecore_File.h>
#include <Eina.h>

#include <check.h>


START_TEST (ecore_file_test_mkdir_rmdir)
{
   int result;

   ecore_file_init();

   result = ecore_file_mkdir("/tmp/ecore_file_test_dir");
   fail_unless(result != 0,
               "Can not create the directory /tmp/ecore_file_test_dir");

   result = ecore_file_rmdir("/tmp/ecore_file_test_dir");
   fail_unless(result != 0,
               "Can not remove the directory /tmp/ecore_file_test_dir");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_mkdirs_recursive_rm)
{
   int result;
   const char *dirs[] = {
        "/tmp/foo",
        "/tmp/foo/bar1",
        "/tmp/foo/bar2"
   };

   ecore_file_init();

   result = ecore_file_mkdirs(dirs);

   fail_unless(result != (-1),
               "Can not create all directories");

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt",
                          "/tmp/foo/ecore_file_test1.txt");
   fail_unless(result != 0,
               "Can not copy ecore_unit_test/ecore_file_test.txt to /tmp/foo/ecore_file_test1.txt");

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt",
                          "/tmp/foo/ecore_file_test2.txt");
   fail_unless(result != 0,
               "Can not copy ecore_unit_test/ecore_file_test.txt to /tmp/foo/ecore_file_test2.txt");

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt",
                          "/tmp/foo/bar1/ecore_file_test.txt");
   fail_unless(result != 0,
               "Can not copy ecore_unit_test/ecore_file_test.txt to /tmp/foo/bar1/ecore_file_test.txt");

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt",
                          "/tmp/foo/bar2/ecore_file_test.txt");
   fail_unless(result != 0,
               "Can not copy ecore_unit_test/ecore_file_test.txt to /tmp/foo/bar2/ecore_file_test.txt");

   result = ecore_file_recursive_rm("/tmp/foo");
   fail_unless(result != 0,
               "Can not remove recursively the directories from /tmp/foo");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_mksubdirs)
{
   int result;
   const char *base = "/tmp/base";
   const char *subdirs[] = {
         "a",
         "a/b",
         "a/b/c"
   };

   ecore_file_init();

   result = ecore_file_mkdir(base);
   fail_unless(result !=0,
               "Can not create the base directory /tmp/foo2");

   result = ecore_file_mksubdirs(base, subdirs);
   fail_unless(result !=(-1),
               "Can not create the sub-directories in a batch");

   result = ecore_file_recursive_rm("/tmp/base");
   fail_unless(result !=0,
               "Can not remove recursively the directories from /tmp/base");

   ecore_file_shutdown();

}
END_TEST

START_TEST (ecore_file_test_mkpath)
{
   int result;

   ecore_file_init();

   result = ecore_file_mkpath("/tmp/foo/bar");
   fail_unless(result != 0,
               "Can not create the directory /tmp/foo/bar from /tmp");

   result = ecore_file_rmdir("/tmp/foo/bar");
   fail_unless(result != 0,
               "Can not remove the directory /tmp/foo/bar");

   result = ecore_file_rmdir("/tmp/foo");
   fail_unless(result != 0,
               "Can not remove the directory /tmp/foo");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_mkpaths)
{
   int result;
   const char *paths[] = {
         "/tmp/foo/bar1",
         "/tmp/foo/bar2",
         "/tmp/foo/bar3"
   };

   ecore_file_init();

   result = ecore_file_mkpaths(paths);
   fail_unless(result != 0,
               "Can not create the all paths in a batch");

   result = ecore_file_recursive_rm("/tmp/foo");
   fail_unless(result != 0,
               "Can not remove recursively the directories from /tmp/foo");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_remove)
{
   int result;

   ecore_file_init();

   result = ecore_file_mkdir("/tmp/ecore_file_test");
   fail_unless(result != 0,
               "Can not create the directory /tmp/ecore_file_test");

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt", "/tmp/ecore_file_test/ecore_file_test1.txt");
   fail_unless(result != 0,
               "Can not copy ecore_unit_test/ecore_file_test.txt to /tmp/ecore_file_test/ecore_file_test1.txt");

   // Remove the given file
   result = ecore_file_remove("/tmp/ecore_file_test/ecore_file_test1.txt");
   fail_unless(result != 0,
               "Can not remove the given file from /tmp/ecore_file_test directory");

   // Remove the given directory
   result = ecore_file_remove("/tmp/ecore_file_test");
   fail_unless(result != 0,
               "Can not remove the given directory from /tmp directory");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_is_dir)
{
   int result;

   ecore_file_init();

   result = ecore_file_mkdir("/tmp/ecore_file_test_dir");
   fail_unless(result != 0,
               "Can not create the directory /tmp/ecore_file_test_dir");

   result = ecore_file_is_dir("/tmp/ecore_file_test_dir");
   fail_unless(result != 0,
               "Can not check that /tmp/ecore_file_test_dir is a directory");

   result = ecore_file_rmdir("/tmp/ecore_file_test_dir");
   fail_unless(result != 0,
               "Can not remove the directory /tmp/ecore_file_test_dir");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_realpath)
{
   char *result;

   ecore_file_init();

   result = ecore_file_realpath("/tmp/../tmp");
   fail_unless(strcmp(result, "/tmp") == 0,
               "Can not get the real path '/tmp' of /tmp/../tmp");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_cp_mv_unlink)
{
   int result;

   ecore_file_init();

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt",
                          "/tmp/ecore_file_test1.txt");
   fail_unless(result != 0,
               "Can not copy ecore_file_test.txt to /tmp/ecore_file_test1.txt");

   result = ecore_file_mv("/tmp/ecore_file_test1.txt",
                          "/tmp/ecore_file_test2.txt");
   fail_unless(result != 0,
               "Can not move /tmp/ecore_file_test1.txt to /tmp/ecore_file_test2.txt");

   result = ecore_file_unlink("/tmp/ecore_file_test2.txt");
   fail_unless(result != 0,
               "Can not delete /tmp/ecore_file_test2.txt");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_symlink)
{
   int result;

   ecore_file_init();

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt",
                          "/tmp/ecore_file_test1.txt");
   fail_unless(result != 0,
               "Can not copy ecore_file_test.txt to /tmp/ecore_file_test1.txt");

   result = ecore_file_symlink("/tmp/ecore_file_test1.txt",
                               "/tmp/ecore_file_test2.txt");
   fail_unless(result != 0,
               "Can not create a link from /tmp/ecore_file_test1.txt to /tmp/ecore_file_test2.txt");

   result = ecore_file_unlink("/tmp/ecore_file_test1.txt");
   fail_unless(result != 0,
               "Can not delete /tmp/ecore_file_test1.txt");

   result = ecore_file_unlink("/tmp/ecore_file_test2.txt");
   fail_unless(result != 0,
               "Can not delete /tmp/ecore_file_test2.txt");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_readlink)
{
   char *path;
   int   result;

   ecore_file_init();

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt",
                          "/tmp/ecore_file_test1.txt");
   fail_unless(result != 0,
               "Can not copy ecore_file_test.txt to /tmp/ecore_file_test1.txt");

   result = ecore_file_symlink("/tmp/ecore_file_test1.txt",
                               "/tmp/ecore_file_test2.txt");
   fail_unless(result != 0,
               "Can not create a link from /tmp/ecore_file_test1.txt to /tmp/ecore_file_test2.txt");

   path = ecore_file_readlink("/tmp/ecore_file_test2.txt");
   fail_unless(path != NULL,
               "Can not retrieve the path of the link /tmp/ecore_file_test2.txt (return NULL)");
   if (path)
     {
        fail_unless(strcmp(path, "/tmp/ecore_file_test1.txt") == 0,
                    "Can not retrieve the path of the link /tmp/ecore_file_test2.txt");
        free(path);
     }

   result = ecore_file_unlink("/tmp/ecore_file_test1.txt");
   fail_unless(result != 0,
               "Can not delete /tmp/ecore_file_test1.txt");

   result = ecore_file_unlink("/tmp/ecore_file_test2.txt");
   fail_unless(result != 0,
               "Can not delete /tmp/ecore_file_test2.txt");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_size)
{
   long long size;
   int       result;

   ecore_file_init();

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt",
                          "/tmp/ecore_file_test.txt");
   fail_unless(result != 0,
               "Can not copy ecore_file_test.txt to /tmp/ecore_file_test.txt");

   size = ecore_file_size("/tmp/ecore_file_test.txt");
   fail_unless(size == 15,
               "Size of /tmp/ecore_file_test.txt different from 15");

   result = ecore_file_unlink("/tmp/ecore_file_test.txt");
   fail_unless(result != 0,
               "Can not delete /tmp/ecore_file_test.txt");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_mod_time)
{
   long long time;
   int       result;

   ecore_file_init();

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt",
                          "/tmp/ecore_file_test.txt");
   fail_unless(result != 0,
               "Can not copy ecore_file_test.txt to /tmp/ecore_file_test.txt");

   time = ecore_file_mod_time("/tmp/ecore_file_test.txt");
   fail_unless(time != 0,
               "Can not get the modification time of /tmp/ecore_file_test.txt");

   result = ecore_file_unlink("/tmp/ecore_file_test.txt");
   fail_unless(result != 0,
               "Can not delete /tmp/ecore_file_test.txt");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_exists)
{
   int result;

   ecore_file_init();

   result = ecore_file_exists(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt");
   fail_unless(result != 0,
               "Can not find ecore_file_test.txt");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_can_read)
{
   int result;

   ecore_file_init();

   result = ecore_file_can_read(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt");
   fail_unless(result != 0,
               "Can not read ecore_file_test.txt");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_can_write)
{
   int result;

   ecore_file_init();

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt",
                          "/tmp/ecore_file_test.txt");
   fail_unless(result != 0,
               "Can not copy ecore_file_test.txt to /tmp/ecore_file_test.txt");

   result = ecore_file_can_write("/tmp/ecore_file_test.txt");
   fail_unless(result != 0,
               "Can not write ecore_file_test.txt");

   result = ecore_file_unlink("/tmp/ecore_file_test.txt");
   fail_unless(result != 0,
               "Can not delete /tmp/ecore_file_test.txt");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_can_exec)
{
   int result;

   ecore_file_init();

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt",
                          "/tmp/ecore_file_test.txt");
   fail_unless(result != 0,
               "Can not copy ecore_file_test.txt to /tmp/ecore_file_test.txt");

   result = ecore_file_can_exec(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt");
   fail_unless(result == 0,
               "Can execute ecore_file_test.txt");

   result = ecore_file_unlink("/tmp/ecore_file_test.txt");
   fail_unless(result != 0,
               "Can not delete /tmp/ecore_file_test.txt");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_strip_ext)
{
   char *file;

   ecore_file_init();

   file = ecore_file_strip_ext(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt");
   fail_unless(file != NULL,
               "Can not strip the extension of ecore_file_test.txt (return NULL)");
   if (file)
     {
        fail_unless(strcmp(file, PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test") == 0,
                    "Can not strip the extension of ecore_file_test.txt");
        free(file);
     }

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_file_get)
{
   const char *result;

   ecore_file_init();

   result = ecore_file_file_get("/home/foo/bar.c");
   fail_unless(strcmp(result, "bar.c") == 0,
               "Can not get the file 'bar.c' from /home/foo/bar.c");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_dir_get)
{
   char *path;

   ecore_file_init();

   path = ecore_file_dir_get("/home/foo/bar.c");
   fail_unless(path != NULL,
               "Can not retrieve the path of the link /tmp/ecore_file_test2.txt (return NULL)");
   if (path)
     {
        fail_unless(strcmp(path, "/home/foo") == 0,
                    "Can not get the path '/home/foo' from /home/foo/bar.c");
        free(path);
     }

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_ls)
{
   int result;
   const char buf[256];
   const char *folder, *file;
   const char *subdirs[] = {
      "testing1",
      "testing2",
      "testing3",
      NULL
   };

   Eina_List *folders;
   Eina_List *files;

   ecore_file_init();

   result = ecore_file_mkdir("/tmp/ecore_file_test_for_ls");
   fail_unless(result != 0,
               "Can not create the directory /tmp/ecore_file_test_for_ls");

   result = ecore_file_mksubdirs("/tmp/ecore_file_test_for_ls", subdirs);
   fail_unless(result != 0,
               "Can not create the sub-directories to /tmp/ecore_file_test_for_ls");

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt", "/tmp/ecore_file_test_for_ls/ecore_file_test1.txt");
   fail_unless(result !=0,
               "Can not copy ecore_unit_test/ecore_file_test.txt to /tmp/ecore_file_test_for_ls/ecore_file_test1.txt");

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/ecore_file_test.txt", "/tmp/ecore_file_test_for_ls/testing1/ecore_file_test1.txt");
   fail_unless(result !=0,
               "Can not copy ecore_unit_test/ecore_file_test.txt to /tmp/ecore_file_test_for_ls/testing1/ecore_file_test1.txt");

   folders = ecore_file_ls("/tmp/ecore_file_test_for_ls");
   fail_unless(folders != NULL,
               "Can not get the list of the files and directories in the given directory");

   EINA_LIST_FREE(folders, folder)
     {
        if (folder)
          {
             snprintf(buf, 256, "%s/%s", "/tmp/ecore_file_test_for_ls", folder);

             result = ecore_file_is_dir(buf);

             if(!result)
               {
                  fail_unless(result == 0,
                              "[%s] is not folder but file only", folder);
               }
             else
               {
                  fail_unless(result != 0,
                              "[%s] is not file but folder", folder);
               }
          }
        free(folder);
     }

   result = ecore_file_recursive_rm("/tmp/ecore_file_test_for_ls");
   fail_unless(result != 0,
               "Can not remove recursively the directories from /tmp/ecore_file_test_for_ls");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_app_exe_get)
{
   int result;
   char *exe;
   Eina_Bool exec_check = EINA_FALSE;

   ecore_file_init();

   result = ecore_file_mkdir("/tmp/foo-exe");
   fail_unless(result != 0,
               "Can not create the directory /tmp/foo-exe");

   result = ecore_file_cp(PACKAGE_DATA_DIR "/ecore_unit_test/screen_clean.sh", "/tmp/foo-exe/screen_clean.sh");
   fail_unless(result != 0,
              "Can not copy screen_clean.sh to /tmp/foo-exe directory");

   chmod("/tmp/foo-exe/screen_clean.sh", 0777);
   
   exe = ecore_file_app_exe_get("/tmp/foo-exe/screen_clean.sh");
   fail_unless(exe != NULL,
               "Application is NULL or on failure");

   exec_check = ecore_file_can_exec("/tmp/foo-exe/screen_clean.sh");
   fail_unless(exec_check != EINA_FALSE,
               "This file is not executable file");

   result = ecore_file_recursive_rm("/tmp/foo-exe");
   fail_unless(result != 0,
               "Can not remove recursively the directories from /tmp/foo-exe");

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_dir_is_empty)
{
   int result;

   ecore_file_init();

   result = ecore_file_mkdir("/tmp/foo-empty");
   fail_unless(result != 0,
               "Can not create the directory /tmp/foo-empty");

   result = ecore_file_dir_is_empty("/tmp/foo-empty");
   fail_unless(((result != 0) || (result !=(-1))),
               "If returned value is 0, it has at least on file. and If -1 is returned, error occurred");

   result = ecore_file_rmdir("/tmp/foo-empty");
   fail_unless(result != 0,
               "Can not remove the directory /tmp/foo-empty");


   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_path_dir_exists)
{
   Eina_Bool result = EINA_FALSE;
   const char *path_dirs = "/usr/bin";

   ecore_file_init();

   result = ecore_file_path_dir_exists(path_dirs);
   fail_unless(result != EINA_FALSE,
               "This directory [%s] not exist in PATH", path_dirs);

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_app_installed)
{
   Eina_Bool result = EINA_FALSE;
   const char *installed_program = "gcc";

   ecore_file_init();

   result = ecore_file_app_installed(installed_program);
   fail_unless(result != EINA_FALSE,
               "This program [%s] is not installed on the system", installed_program);

   ecore_file_shutdown();
}
END_TEST

START_TEST (ecore_file_test_app_list)
{
   Eina_List *installed_app_list;

   ecore_file_init();

   installed_app_list = ecore_file_app_list();
   fail_unless(installed_app_list != NULL,
               "There is no application installed on the system");

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

   tcase_add_test(tc, ecore_file_test_mkdir_rmdir);
   tcase_add_test(tc, ecore_file_test_mkdirs_recursive_rm);
   tcase_add_test(tc, ecore_file_test_mksubdirs);
   tcase_add_test(tc, ecore_file_test_mkpath);
   tcase_add_test(tc, ecore_file_test_mkpaths);
   tcase_add_test(tc, ecore_file_test_is_dir);
   tcase_add_test(tc, ecore_file_test_realpath);
   tcase_add_test(tc, ecore_file_test_cp_mv_unlink);
   tcase_add_test(tc, ecore_file_test_symlink);
   tcase_add_test(tc, ecore_file_test_readlink);
   tcase_add_test(tc, ecore_file_test_size);
   tcase_add_test(tc, ecore_file_test_mod_time);
   tcase_add_test(tc, ecore_file_test_exists);
   tcase_add_test(tc, ecore_file_test_can_read);
   tcase_add_test(tc, ecore_file_test_can_write);
   tcase_add_test(tc, ecore_file_test_can_exec);
   tcase_add_test(tc, ecore_file_test_strip_ext);
   tcase_add_test(tc, ecore_file_test_file_get);
   tcase_add_test(tc, ecore_file_test_dir_get);
   tcase_add_test(tc, ecore_file_test_remove);
   tcase_add_test(tc, ecore_file_test_ls);
   tcase_add_test(tc, ecore_file_test_app_exe_get);
   tcase_add_test(tc, ecore_file_test_dir_is_empty);
   tcase_add_test(tc, ecore_file_test_path_dir_exists);
   tcase_add_test(tc, ecore_file_test_app_installed);
   tcase_add_test(tc, ecore_file_test_app_list);

   suite_add_tcase(s, tc);

   return s;
}
