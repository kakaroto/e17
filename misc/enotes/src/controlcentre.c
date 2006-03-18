
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


#include "note.h"

ControlCentre  *controlcentre;


/**
 * @brief: Sets up the control centre window and its contents and callbacks.
 */
void
setup_cc(void)
{
	setup_cc_with_pos(-1, -1);
	return;
}

void
setup_cc_with_pos(int x, int y)
{
	ControlCentre  *cc;
	char           *edjefn = malloc(PATH_MAX);
	char           *fontpath = malloc(PATH_MAX);
	Evas_Coord      edje_w, edje_h;
	CCPos          *pos;

	cc = malloc(sizeof(ControlCentre));
	controlcentre = cc;

	pos = get_cc_pos();

	if (x >= 0 || y >= 0) {
		pos->x = x;
		pos->y = y;
	}

	/* Setup the Window */
	if (!strcmp(main_config->render_method, "gl")) {
#ifdef HAVE_ECORE_EVAS_GL
		cc->win =
			ecore_evas_gl_x11_new(NULL, 0, pos->x, pos->y,
					      pos->width, pos->height);
#else
		dml("GL not in Ecore_Evas module.  Falling back on software!",
		    1);
		free(main_config->render_method);
		main_config->render_method = strdup("software");
		cc->win =
			ecore_evas_software_x11_new(NULL, 0, pos->x, pos->y,
						    pos->width, pos->height);
#endif
	} else
		cc->win =
			ecore_evas_software_x11_new(NULL, 0, pos->x, pos->y,
						    pos->width, pos->height);

	ecore_evas_title_set(cc->win, "Enotes");
	ecore_evas_name_class_set(cc->win, "Enotes", "Enotes");

	if (main_config->ontop == 1)
		ecore_evas_layer_set(cc->win, 7);
	else
		ecore_evas_layer_set(cc->win, 2);

	if (main_config->sticky == 1)
		ecore_evas_sticky_set(cc->win, 1);
	else
		ecore_evas_sticky_set(cc->win, 0);

	ecore_evas_borderless_set(cc->win, 1);
	ecore_evas_shaped_set(cc->win, 1);
	if (pos->x != 0 && pos->y != 0)
		ecore_evas_resize(cc->win, pos->x, pos->y);
	ecore_evas_show(cc->win);

//      if(main_config->ontop==1)

	/* Moving the damn thing */
	ecore_evas_move(cc->win, pos->x, pos->y);

	/* Setup the Canvas, Render-Method and Font Path */
	cc->evas = ecore_evas_get(cc->win);
	evas_output_method_set(cc->evas,
			       evas_render_method_lookup(main_config->
							 render_method));
	snprintf(fontpath, PATH_MAX, "%s/fonts", PACKAGE_DATA_DIR);
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
	evas_object_pass_events_set(cc->edje, 0);
	evas_object_show(cc->edje);
	if (pos->shaded)
		edje_object_signal_emit(cc->edje, EDJE_SIGNAL_CC_SHADE "_GO",
					"");
	else
		edje_object_signal_emit(cc->edje, EDJE_SIGNAL_CC_UNSHADE "_GO",
					"");

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
	ecore_evas_callback_destroy_set(cc->win, cc_close_win);
	ecore_evas_callback_delete_request_set(cc->win, cc_close_win);

	/* Edje Callbacks */
	edje_object_signal_callback_add(cc->edje,
					EDJE_SIGNAL_CC_MINIMIZE, "",
					(void *) cc_minimize, cc->win);
	edje_object_signal_callback_add(cc->edje, EDJE_SIGNAL_CC_CLOSE, "",
					(void *) cc_close, NULL);
	edje_object_signal_callback_add(cc->edje, EDJE_SIGNAL_CC_SAVELOAD, "",
					(void *) cc_saveload, NULL);
	edje_object_signal_callback_add(cc->edje, EDJE_SIGNAL_CC_SETTINGS, "",
					(void *) cc_settings, NULL);
	edje_object_signal_callback_add(cc->edje, EDJE_SIGNAL_CC_NEW, "",
					(void *) cc_newnote, NULL);
	edje_object_signal_callback_add(cc->edje, EDJE_SIGNAL_CC_SHADE, "",
					(void *) cc_shade, NULL);
	edje_object_signal_callback_add(cc->edje, EDJE_SIGNAL_CC_UNSHADE, "",
					(void *) cc_unshade, NULL);

	free(pos);
	return;
}

CCPos          *
get_cc_pos()
{
	CCPos          *p = malloc(sizeof(CCPos));

	p->x = ecore_config_int_get("controlcentre.x");
	p->y = ecore_config_int_get("controlcentre.y");
	p->width = ecore_config_int_get("controlcentre.w");
	p->height = ecore_config_int_get("controlcentre.h");
	p->shaded = ecore_config_boolean_get("controlcentre.shaded");

	return (p);
}

void
set_cc_pos_by_ccpos(CCPos * p)
{
	set_cc_pos(p->x, p->y, p->width, p->height);
	return;
}

void
set_cc_pos()
{
	int             x, y, width, height;

	if (controlcentre == NULL) {
		return;
	}

	ecore_evas_geometry_get(controlcentre->win, &x, &y, &width, &height);

	ecore_config_int_set("controlcentre.x", x);
	ecore_config_int_set("controlcentre.y", y);
	ecore_config_int_set("controlcentre.w", width);
	ecore_config_int_set("controlcentre.h", height);

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
 * @brief: Edje signal callback for the closing of the window
 *         (window-manager side).  This function simply ends the loop which
 *         concequently allows for the freeing of variables, etc... before
 *         enotes exits.
 */
void
cc_close_win(Ecore_Evas * data)
{
	ecore_main_loop_quit();
}

/**
 * @brief: Ecore signal callback for the closing of the window
 *         (window-manager side).  This function simply ends the loop which
 *         concequently allows for the freeing of variables, etc... before
 *         enotes exits.
 */
void
cc_close(void *data, Evas_Object * obj, const char *emission,
	 const char *source)
{
	ecore_main_loop_quit();
}

/**
 * @brief: Edje signal callback for the clicking or selecting of the saveload option.
 *         This calls up the saveload window.
 */
void
cc_saveload(void *data, Evas_Object * obj, const char *emission,
	    const char *source)
{
	setup_saveload();
	return;
}

/**
 * @brief: Edje signal callback for the clicking or selecting of the new note option.
 *         This calls up a new note.
 */
void
cc_newnote(void *data, Evas_Object * obj, const char *emission,
	   const char *source)
{
	new_note();
	return;
}

/**
 * @brief: Edje signal callback for the clicking or selecting of the settings option.
 *         This calls up the settings window.
 */
void
cc_settings(void *data, Evas_Object * obj, const char *emission,
	    const char *source)
{
	if (!ecore_exe_run("examine enotes", NULL))
		msgbox("No Examine", "Please Install Examine for Settings!");
	return;
}

/**
 * @brief: Edje signal callback for the clicking or selecting of the minimize button.
 */
void
cc_minimize(void *data, Evas_Object * obj, const char *emission,
	    const char *source)
{
	/* FIXME: The line below should be removed when
	 *          * ecore_evas is fixed. */
	ecore_evas_iconified_set((Ecore_Evas *) data, 0);

	ecore_evas_iconified_set((Ecore_Evas *) data, 1);
	return;
}

/**
 * @brief: Edje signal callback for the clicking or selecting of the shaded state.
 */
void
cc_shade(void *data, Evas_Object * obj, const char *emission,
	 const char *source)
{
	ecore_config_boolean_set("controlcentre.shaded", 1);
}

/**
 * @brief: Edje signal callback for the clicking or unselecting of the
 * unshaded state.
 */
void
cc_unshade(void *data, Evas_Object * obj, const char *emission,
	   const char *source)
{
	ecore_config_boolean_set("controlcentre.shaded", 0);
}

/*  Theme Change  */
void
cc_update_theme()
{
	int             w, h;
	char           *edjefn;

	if (!controlcentre)
		return;

	edjefn = malloc(PATH_MAX);
	snprintf(edjefn, PATH_MAX, PACKAGE_DATA_DIR "/themes/%s.edj",
		 main_config->theme);
	edje_object_file_set(controlcentre->edje, edjefn, CC_PART);
	free(edjefn);

	/* EDJE and ECORE min, max and resizing */
	edje_object_size_max_get(controlcentre->edje, &w, &h);
	ecore_evas_size_max_set(controlcentre->win, w, h);
	edje_object_size_min_get(controlcentre->edje, &w, &h);
	ecore_evas_size_min_set(controlcentre->win, w, h);
	ecore_evas_resize(controlcentre->win, w, h);
	evas_object_resize(controlcentre->edje, w, h);
	evas_object_resize(controlcentre->dragger, w, h);
}
