
/**************************************************
 **               E  -  N O T E S                **
 **                                              **
 **  The contents of this file are released to   **
 **  the public under the General Public Licence **
 **  Version 2.                                  **
 **                                              **
 **  By  Thomas Fletcher (www.fletch.vze.com)    **
 **                                              **
 **************************************************/


#include "controlcentre.h"

extern MainConfig *main_config;


/**
 * @brief: Sets up the control centre window and its contents and callbacks.
 */
void
setup_cc(void)
{
	ControlCentre   controlcentre;
	ControlCentre  *cc = &controlcentre;
	char           *edjefn = malloc(PATH_MAX);
	char           *fontpath = malloc(PATH_MAX);
	double          edje_w, edje_h;

	/* Setup the Window */
	cc->win = ecore_evas_software_x11_new(NULL, 0, 0, 0, 250, 250);
	ecore_evas_title_set(cc->win, "E-Notes Control Centre");
	ecore_evas_borderless_set(cc->win, 1);
	ecore_evas_shaped_set(cc->win, 1);
	ecore_evas_show(cc->win);

	/* Setup the Canvas, Render-Method and Font Path */
	cc->evas = ecore_evas_get(cc->win);
	evas_output_method_set(cc->evas,
			       evas_render_method_lookup(main_config->
							 render_method));
	snprintf(fontpath, PATH_MAX, "%s/data/fonts", PACKAGE_DATA_DIR);
	evas_font_path_append(cc->evas, fontpath);
	free(fontpath);

	/* Draggable Setup */
	cc->dragger = esmart_draggies_new(cc->win);
	evas_object_name_set(cc->dragger, "dragger");
	evas_object_move(cc->dragger, 0, 0);
	evas_object_layer_set(cc->dragger, 0);
	evas_object_color_set(cc->dragger, 255, 255, 255, 0);
	esmart_draggies_button_set(cc->dragger, 1);
	evas_object_show(cc->dragger);

	/* Setup the EDJE */
	cc->edje = edje_object_add(cc->evas);
	snprintf(edjefn, PATH_MAX, CC_EDJE, PACKAGE_DATA_DIR,
		 main_config->theme);
	edje_object_file_set(cc->edje, edjefn, CC_PART);
	free(edjefn);
	evas_object_move(cc->edje, 0, 0);
	evas_object_layer_set(cc->edje, 1);
	evas_object_name_set(cc->edje, "edje");
	evas_object_pass_events_set(cc->edje, 1);
	evas_object_show(cc->edje);

	/* EDJE and ECORE min, max and resizing */
	edje_object_size_max_get(cc->edje, &edje_w, &edje_h);
	ecore_evas_size_max_set(cc->win, edje_w, edje_h);
	edje_object_size_min_get(cc->edje, &edje_w, &edje_h);
	ecore_evas_size_min_set(cc->win, edje_w, edje_h);
	ecore_evas_resize(cc->win, (int) edje_w, (int) edje_h);
	evas_object_resize(cc->edje, edje_w, edje_h);
	evas_object_resize(cc->dragger, edje_w, edje_h);

	/* Ecore Callbacks */
	ecore_evas_callback_resize_set(cc->win, cc_resize);
	ecore_evas_callback_destroy_set(cc->win, cc_close);
	ecore_evas_callback_delete_request_set(cc->win, cc_close);

	/* Edje Callbacks */
	edje_object_signal_callback_add(cc->edje,
					EDJE_SIGNAL_CC_CLOSE, "",
					(void *) cc_close, NULL);
	edje_object_signal_callback_add(cc->edje,
					EDJE_SIGNAL_CC_SAVELOAD, "",
					(void *) cc_saveload, NULL);
	edje_object_signal_callback_add(cc->edje,
					EDJE_SIGNAL_CC_SETTINGS, "",
					(void *) cc_settings, NULL);
	edje_object_signal_callback_add(cc->edje, EDJE_SIGNAL_CC_NEW, "",
					(void *) cc_newnote, NULL);

	return;
}

/**
 * @param ee: The Ecore_Evas the event occurred on.
 * @brief: Ecore callback for the resising of the window.  This function
 *         resises the edje and esmart dragger according to the new
 *         window size.
 */
void
cc_resize(Ecore_Evas * ee)
{
	int             x, y, w, h;

	ecore_evas_geometry_get(ee, &x, &y, &w, &h);
	evas_object_resize(evas_object_name_find
			   (ecore_evas_get(ee), "edje"), w, h);
	evas_object_resize(evas_object_name_find(ecore_evas_get(ee), "dragger"),
			   w, h);
	return;
}

/**
 * @param ee: The Ecore_Evas that has been closed by the wm.
 * @brief: Ecore + Edje signal callback for the closing of the window
 *         (window-manager side).  This function simply ends the loop which
 *         concequently allows for the freeing of variables, etc... before
 *         enotes exits.
 */
void
cc_close(Ecore_Evas * ee)
{
	ecore_main_loop_quit();
	return;
}

/**
 * @param data: This variable isn't used.  It is data that could be supplied when
 *              the callback is made.
 * @brief: Edje signal callback for the clicking or selecting of the saveload option.
 *         This calls up the saveload window.
 */
void
cc_saveload(void *data)
{
	setup_saveload();
	return;
}

/**
 * @param data: This variable isn't used.  It is data that could be supplied when
 *              the callback is made.
 * @brief: Edje signal callback for the clicking or selecting of the new note option.
 *         This calls up a new note.
 */
void
cc_newnote(void *data)
{
	new_note(NOTE_CONTENT);
	return;
}

/**
 * @param data: This variable isn't used.  It is data that could be supplied when
 *              the callback is made.
 * @brief: Edje signal callback for the clicking or selecting of the settings option.
 *         This calls up the settings window.
 */
void
cc_settings(void *data)
{
	setup_settings();
	return;
}
