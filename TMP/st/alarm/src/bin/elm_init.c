#include <Elementary.h>

EAPI void
elm_init(int argc, char **argv)
{
   eet_init();
   ecore_init();
   ecore_file_init();
   evas_init();
   edje_init();
   ecore_app_args_set(argc, (const char **)argv);
}
