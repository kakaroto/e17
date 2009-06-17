/* ------------------------------------------- *
 * Retina - Evas powered image viewer          *
 * (C) 2000 - 2005, Joshua Deere               *
 * joshua.deere@gmail.com                      *
 * ------------------------------------------- *
 * See COPYING for license information         *
 * ------------------------------------------- */

#include "retina.h"

int
main(int argc, char **argv)
{
	int i;
	img_count = 0;

	help_state = 0;
	diag_state = 0;
	
	ecore_init();

	retina_gui_create();
	retina_gui_render();

	/* check for command line stuff */
	if(argc > 1){
		for(i = 1; i < argc; i++){
			retina_image_list[i-1] = strdup(argv[i]);
			img_count++;
		}
		cur_sel = 0;
		retina_img_load(retina_image_list[0]);
	} else {
		retina_img_load(PACKAGE_DATA_DIR "/images/retina.png");
	}

	/* everything should be done, attach callbacks */
	retina_cb_attach();

	ecore_main_loop_begin();

	ecore_evas_shutdown();
	ecore_shutdown();

	return 0;
}
