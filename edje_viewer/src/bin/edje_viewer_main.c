/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */

#include <Etk.h>

#include "edje_viewer_main.h"

int main(int argc, char **argv)
{
   Gui *gui;
   if (!etk_init(argc, argv))
     {
	fprintf(stderr, "Could not init etk. Exiting...\n");
	return 0;
     };

   if (!edje_viewer_config_init())
     {
	fprintf(stderr, "Could not init edje_viewer config. Exiting...\n");
	return 0;
     }

   if (argc == 2) gui = main_window_show(argv[1]);
   else gui = main_window_show(NULL);

   etk_main();
   if (gui)
     edje_viewer_config_save(gui);
   edje_viewer_config_shutdown();
   etk_shutdown();

   return 0;
}
