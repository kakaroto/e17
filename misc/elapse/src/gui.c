#include "Elapse.h"

void elapse_gui_init(Elapse *elapse, int argc, const char **argv)
{
	double iw, ih;
	int x, y, w, h;
	Ecore_X_Window win;
	debug(DEBUG_INFO, elapse, "elapse_gui_init()");

	debug(DEBUG_INFO, elapse, "ecore_init()");
	ecore_init();
	ecore_app_args_set(argc, argv);

	debug(DEBUG_INFO, elapse, "ecore_evas_init()");
	ecore_evas_init();
	elapse->ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 0, 0);
	ecore_evas_title_set(elapse->ee, "Elapse");
   ecore_evas_name_class_set(elapse->ee, "Elapse", "Elapse");
	ecore_evas_borderless_set(elapse->ee, 1);
	win = ecore_evas_software_x11_window_get(elapse->ee);
	ecore_x_window_prop_layer_set(win, ECORE_X_WINDOW_LAYER_BELOW);
	if (elapse->conf.sticky)
		ecore_x_window_prop_sticky_set(win, 1);
	
	elapse->evas = ecore_evas_get(elapse->ee);
	
	debug(DEBUG_INFO, elapse, "esmart_trans_x11_new()");
	elapse->smart = esmart_trans_x11_new(elapse->evas);
	evas_object_move(elapse->smart, 0, 0);
	evas_object_layer_set(elapse->smart, -5);
	evas_object_name_set(elapse->smart, "root_bg");
	evas_object_show(elapse->smart);

	debug(DEBUG_INFO, elapse, "edje_init()");
	edje_init();
	elapse->edje = edje_object_add(elapse->evas);
	x = edje_object_file_set(elapse->edje, elapse->conf.theme, "elapse");
	if (!x) {
		debug(DEBUG_FUCKED, elapse, "Could not load theme");
		exit(1);
	}
	evas_object_move(elapse->edje, 0, 0);
	edje_object_size_min_get(elapse->edje, &iw, &ih);
	evas_object_show(elapse->edje);

	debug(DEBUG_INFO, elapse, "object resize");
	evas_object_resize(elapse->edje, (int)iw, (int)ih);
	evas_object_resize(elapse->smart, (int)iw, (int)ih);
	ecore_evas_resize(elapse->ee, (int)iw, (int)ih);

	debug(DEBUG_INFO, elapse, "ecore_evas_show()");
	ecore_evas_show(elapse->ee);

	ecore_evas_geometry_get(elapse->ee, &x, &y, &w, &h);
	esmart_trans_x11_freshen(elapse->smart, x, y, w, h);

	ecore_evas_callback_move_set(elapse->ee, elapse_cb_window_move);
	edje_object_signal_callback_add(elapse->edje, "quit", "quit", 
			(void *)ecore_main_loop_quit, NULL);
}

void elapse_cb_window_move(Ecore_Evas *ee)
{
	int x, y, w, h;
	Evas_Object *o;
	Elapse elapse;
	
	elapse.conf.debug = 1;
	
	ecore_evas_geometry_get(ee, &x, &y, &w, &h);

	o = evas_object_name_find(ecore_evas_get(ee), "root_bg");

	if (!o) debug(DEBUG_FUCKED, &elapse, "Smart Object Not Found");

	esmart_trans_x11_freshen(o, x, y, w, h);
}
