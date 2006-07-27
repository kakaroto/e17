/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include "edje_viewer.h"

int main(int argc, char **argv)
{
   if (!etk_init("ecore_evas_software_x11"))
     {
	fprintf(stderr, "Could not init etk. Exiting...\n");
	return 0;
     };

   edje_viewer_config_init();

   if (argc == 2) main_window_show(argv[1]);
   else main_window_show(NULL);

   etk_main();
   edje_viewer_config_shutdown();
   etk_shutdown();

   return 0;
}
