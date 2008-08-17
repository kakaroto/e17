/* gui parts of retina */

#include "retina.h"

int
retina_gui_create()
{
	int w, h;
	
	/* create the win, evas, and objects, setup hints */
	if(!ecore_evas_init()){
		printf("Could not initialize evas...DEAD\n");
		return -1;
	}
	
	retina_win = ecore_evas_software_x11_new(NULL, 0, 0, 0, 320, 200);
	if (!retina_win) return -1;

	ecore_evas_name_class_set(retina_win, "Retina", "Retina");
	retina_gui_set_title("Retina");

	retina_evas = ecore_evas_get(retina_win);

	/* draw a checkered bg to render to */
	retina_img_drawchecks();

	/* kinda nasty, but an invisible toplevel object to catch events */
	retina_catch = evas_object_rectangle_add(retina_evas);
	evas_object_color_set(retina_catch, 0, 0, 0, 0);
	evas_object_resize(retina_catch, 9999, 9999);
	evas_object_layer_set(retina_catch, 900);
	evas_object_show(retina_catch);

	/* faded black rect for dialogs */
	retina_diag_bg = evas_object_rectangle_add(retina_evas);
	evas_object_color_set(retina_diag_bg, 0, 0, 0, 175);
	evas_object_layer_set(retina_diag_bg, 200);

	/* help image */
	retina_help_img = evas_object_image_add(retina_evas);
	evas_object_image_file_set(retina_help_img, PACKAGE_DATA_DIR "/images/retina_help.png", NULL);
	evas_object_image_size_get(retina_help_img, &w, &h);
	evas_object_image_fill_set(retina_help_img, 0, 0, w, h);
	evas_object_resize(retina_help_img, w, h);
	evas_object_layer_set(retina_help_img, 300);

	return 0;
}

void retina_gui_set_title(char *title)
{
	/* set the titlebar text */
	char buf[255];
	
	if(img_count > 0){
		snprintf(buf, sizeof(buf), "Retina - %s", title);
		ecore_evas_title_set(retina_win, buf);
	} else
		ecore_evas_title_set(retina_win, title);
}

void
retina_gui_render()
{
	/* show the evas and main win */
	ecore_evas_show(retina_win);
}

