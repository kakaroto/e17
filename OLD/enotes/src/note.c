
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

static void     note_close_dialog_close_cb(Ewl_Widget * w, void *ev,
					   void *data);
static void     note_close_dialog_delete_cb(Ewl_Widget * w, void *ev,
					    void *data);
static void     note_close_dialog_unload_cb(Ewl_Widget * w, void *ev,
					    void *data);

extern MainConfig *main_config;

Evas_List      *gbl_notes = NULL;

/* High Level */

/**
 * @brief: Opens a new note.
 */
void
new_note(void)
{
	Evas_List      *new;

	dml("Creating a Note", 2);

	new = append_note();
	setup_note(&new, 0, 0, 0, 0, 0, DEF_CONTENT);
	return;
}

void
new_note_with_values(int x, int y, int width, int height, int shaded,
		     char *content)
{
	/*Note *p = */
	new_note_with_values_return(x, y, width, height, shaded, content);
}

Note           *
new_note_with_values_return(int x, int y, int width, int height, int shaded,
			    char *content)
{
	Evas_List      *new;

	dml("Creating a Note", 2);

	new = append_note();
	setup_note(&new, x, y, width, height, shaded, content);
	return (evas_list_data(new));
}

/* Lists and Allocation */

/**
 * @return: Evas_List pointer to the new note created in the list.
 * @brief: Initialise the Note and add it to the list.
 */
Evas_List      *
append_note(void)
{
	Note           *note = malloc(sizeof(Note));

	/* Set NULL's */
	note->txt_title = NULL;

	gbl_notes = evas_list_append(gbl_notes, note);
	return (evas_list_find_list(gbl_notes, note));
}

/**
 * @param note: The pointer to an Evas_List containing the note.
 * @brief: Closes and frees a note.
 */
void
remove_note(Evas_List * note)
{
	Ewl_Widget     *w;
	Note           *p;

	dml("Closing a Note", 2);

	p = evas_list_data(note);
	if (p->timcomp) {
		ecore_timer_del(p->timcomp);
		p->timcomp = NULL;
	}

	if (p->dialog)
		return;

	p->dialog = ewl_dialog_new();
	ewl_window_title_set(EWL_WINDOW(p->dialog), "Enotes Delete");
	ewl_window_name_set(EWL_WINDOW(p->dialog), "Enotes");
	ewl_window_class_set(EWL_WINDOW(p->dialog), "Enotes");
	ewl_callback_append(p->dialog, EWL_CALLBACK_DELETE_WINDOW,
			    note_close_dialog_close_cb, p);

	ewl_dialog_active_area_set(EWL_DIALOG(p->dialog), EWL_POSITION_TOP);
	w = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(w),
			  "Do you want to perminantly delete this note or just unload it?");
	ewl_container_child_append(EWL_CONTAINER(p->dialog), w);
	ewl_widget_show(w);

	ewl_dialog_active_area_set(EWL_DIALOG(p->dialog), EWL_POSITION_BOTTOM);
	w = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(w), "Delete");
	ewl_container_child_append(EWL_CONTAINER(p->dialog), w);
	ewl_callback_append(w, EWL_CALLBACK_CLICKED,
			    note_close_dialog_delete_cb, p);
	ewl_widget_show(w);

	w = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(w), "Unload");
	ewl_container_child_append(EWL_CONTAINER(p->dialog), w);
	ewl_callback_append(w, EWL_CALLBACK_CLICKED,
			    note_close_dialog_unload_cb, p);
	ewl_widget_show(w);

	w = ewl_button_new();
	ewl_button_label_set(EWL_BUTTON(w), "Cancel");
	ewl_container_child_append(EWL_CONTAINER(p->dialog), w);
	ewl_callback_append(w, EWL_CALLBACK_CLICKED, note_close_dialog_close_cb,
			    p);
	ewl_widget_show(w);

	ewl_dialog_active_area_set(EWL_DIALOG(p->dialog), EWL_POSITION_TOP);
	ewl_widget_show(p->dialog);
}

static void
note_close_dialog_close_cb(Ewl_Widget * w, void *ev, void *data)
{
	Note           *p;

	p = (Note *) data;
	ewl_widget_destroy(p->dialog);
	p->dialog = NULL;
}

static void
note_close_dialog_delete_cb(Ewl_Widget * w, void *ev, void *data)
{
	Note           *p;
	char           *path;

	p = data;
	path = malloc(PATH_MAX);
	note_close_dialog_unload_cb(w, ev, data);

	dml("Deleting Saved Note", 2);
	sprintf(path, "%s/.e/apps/enotes/notes/%s", getenv("HOME"),
		get_title_by_content(ewl_text_text_get(EWL_TEXT(p->content))));
	unlink(path);

	/* FIXME more intelligent things once we have sorted
	 * saveload */
	ewl_saveload_revert(NULL, NULL, NULL);
}

static void
note_close_dialog_unload_cb(Ewl_Widget * w, void *ev, void *data)
{
	Note           *p;

	note_close_dialog_close_cb(w, ev, data);

	p = (Note *) data;
	edje_object_part_unswallow(p->edje, p->eo);
	ewl_widget_destroy(p->emb);
	evas_object_del(p->edje);

	ecore_evas_free(p->win);
	free(p);
	gbl_notes = evas_list_remove(gbl_notes, p);

	/* FIXME more intelligent things once we have sorted saveload */
	ewl_saveload_revert(NULL, NULL, NULL);

	/*  Check if it was the last note  */
	if (evas_list_next(gbl_notes) == NULL && controlcentre == NULL)
		ecore_main_loop_quit();

	return;
}

/* GUI Setup */

/**
 * @param note: The note to setup (pointer to a pointer).
 * @param width: Width of the new notes window.
 * @param height: Height of the new notes window.
 * @param title: Title to begin with.
 * @param content: Content to begin with.
 * @brief: Sets up the note objects, window, callbacks, etc...
 */
void
setup_note(Evas_List ** note, int x, int y, int width, int height, int shaded,
	   char *content)
{
	Evas_List      *pl;
	Note           *p;

	char           *edjefn = malloc(PATH_MAX);
	char           *datestr;
	char           *fcontent;
	char           *prop;

	Evas_Coord      edje_w, edje_h;

	/* Fix Newlines in Content */
	fcontent = fix_newlines(content);

	/* Get the Note from the Evas_List** */
	pl = *note;
	p = evas_list_data(pl);


	/* Setup the Window */
	if (!strcmp(main_config->render_method, "gl")) {
#ifdef HAVE_ECORE_EVAS_GL
		p->win = ecore_evas_gl_x11_new(NULL, 0, x, y, width, height);
#else
		dml("GL not in Ecore_Evas module.  Falling back on software!",
		    1);
		free(main_config->render_method);
		main_config->render_method = strdup("software");
		p->win = ecore_evas_software_x11_new(NULL, 0, x, y, width,
						     height);
#endif
	} else
		p->win = ecore_evas_software_x11_new(NULL, 0, x, y, width,
						     height);

	ecore_evas_borderless_set(p->win, 1);
	ecore_evas_shaped_set(p->win, 1);
	ecore_evas_title_set(p->win, "An E-Note");
	ecore_evas_name_class_set(p->win, "Enotes", "Enotes");

	if (main_config->ontop == 1)
		ecore_evas_layer_set(p->win, 7);
	else
		ecore_evas_layer_set(p->win, 2);

	if (main_config->sticky == 1)
		ecore_evas_sticky_set(p->win, 1);
	else
		ecore_evas_sticky_set(p->win, 0);

	ecore_evas_show(p->win);


	/* Move the damn thing  */
	ecore_evas_move(p->win, x, y);

	/* Setup the Canvas, fonts, etc... */
	p->evas = ecore_evas_get(p->win);
	evas_output_method_set(p->evas,
			       evas_render_method_lookup(main_config->
							 render_method));
	evas_font_path_append(p->evas, PACKAGE_DATA_DIR "/fonts");

	/* Draggable Setup */
	p->dragger = (Evas_Object *) esmart_draggies_new(p->win);
	evas_object_name_set(p->dragger, "dragger");
	evas_object_move(p->dragger, 0, 0);
	evas_object_resize(p->dragger, width, height);
	evas_object_layer_set(p->dragger, 0);
	evas_object_color_set(p->dragger, 255, 255, 255, 0);
	esmart_draggies_button_set(p->dragger, 1);
	evas_object_show(p->dragger);

	p->eventer = evas_object_rectangle_add(p->win);
	evas_object_color_set(p->eventer, 0, 0, 0, 0);
	evas_object_resize(p->eventer, width, height);
	evas_object_move(p->eventer, 0.0, 0.0);
	evas_object_layer_set(p->eventer, 9999);
	evas_object_repeat_events_set(p->eventer, 1);
	evas_object_show(p->eventer);

	evas_object_event_callback_add(p->eventer, EVAS_CALLBACK_MOUSE_DOWN,
				       (void *) cb_menu_rightclick, p);

	/* Setup the Edje */
	p->edje = edje_object_add(p->evas);
	snprintf(edjefn,
		 PATH_MAX, PACKAGE_DATA_DIR "/themes/%s.edj",
		 main_config->theme);
	edje_object_file_set(p->edje, edjefn, NOTE_PART);
	evas_object_name_set(p->edje, "edje");
	evas_object_move(p->edje, 0, 0);
	evas_object_layer_set(p->edje, 1);

	p->shaded = shaded;
	if (shaded)
		edje_object_signal_emit(p->edje, EDJE_SIGNAL_NOTE_SHADE "_GO",
					"");
	else
		edje_object_signal_emit(p->edje, EDJE_SIGNAL_NOTE_UNSHADE "_GO",
					"");

	edje_object_size_max_get(p->edje, &edje_w, &edje_h);
	ecore_evas_size_max_set(p->win, edje_w, edje_h);
	edje_object_size_min_get(p->edje, &edje_w, &edje_h);
	ecore_evas_size_min_set(p->win, edje_w, edje_h);

	if (width == 0 && height == 0) {
		ecore_evas_resize(p->win, (int) edje_w, (int) edje_h);
		evas_object_resize(p->edje, (int) edje_w, (int) edje_h);
	} else {
		ecore_evas_resize(p->win, width, height);
		evas_object_resize(p->edje, width, height);
	}

	evas_object_show(p->edje);

	/* Setup the date, user and initial title */
	edje_object_part_text_set(p->edje, EDJE_TEXT_USER, getenv("USER"));
	datestr = get_date_string();
	edje_object_part_text_set(p->edje, EDJE_TEXT_DATE, datestr);
	update_enote_title(p->edje, content);

	/* Ewl */
	p->emb = ewl_embed_new();
	ewl_object_fill_policy_set((Ewl_Object *) p->emb, EWL_FLAG_FILL_ALL);
	ewl_widget_show(p->emb);
	p->eo = ewl_embed_evas_set(EWL_EMBED(p->emb),
				   ecore_evas_get(p->win), (void *)
				   ecore_evas_software_x11_window_get(p->win));
	evas_object_layer_set(p->eo, 2);
	edje_object_part_swallow(p->edje, EDJE_CONTAINER, p->eo);
	evas_object_show(p->eo);

	evas_object_focus_set(p->eo, TRUE);
	ewl_embed_focus_set((Ewl_Embed *) p->emb, TRUE);

	p->pane = ewl_scrollpane_new();
	ewl_container_child_append((Ewl_Container *) p->emb, p->pane);

	if (edje_object_data_get(p->edje, EDJE_INFO_SCROLLBARS) != NULL) {
		configure_scrollbars(p->pane, edjefn);
	}

	ewl_widget_show(p->pane);

	p->content = ewl_entry_new();
	ewl_entry_multiline_set(EWL_ENTRY(p->content), 1);
	ewl_container_child_append((Ewl_Container *) p->pane, p->content);
	ewl_entry_multiline_set((Ewl_Entry *) p->content, 1);

	ewl_theme_data_str_set(p->content, "/entry/group", "none");

	prop = (char *) edje_object_data_get(p->edje, EDJE_INFO_FONTNAME);
	if (prop != NULL)
		ewl_theme_data_str_set(p->content, "/entry/text/font", prop);

	prop = (char *) edje_object_data_get(p->edje, EDJE_INFO_FONTSTYLE);
	if (prop != NULL)
		ewl_theme_data_str_set(p->content, "/entry/text/style", prop);

	prop = (char *) edje_object_data_get(p->edje, EDJE_INFO_FONTSIZE);
	if (prop != NULL)
		ewl_theme_data_int_set(p->content, "/entry/text/font_size",
				       atoi(prop));

	ewl_text_text_set((Ewl_Text *) p->content, fcontent);
	ewl_widget_show(p->content);

	ewl_callback_append(p->emb, EWL_CALLBACK_CONFIGURE, note_move_embed,
			    p->pane);

	/* Ecore Callbacks */
	ecore_evas_callback_resize_set(p->win, note_ecore_resize);
	ecore_evas_callback_destroy_set(p->win, note_ecore_close);
	ecore_evas_callback_delete_request_set(p->win, note_ecore_close);

	/* Edje Callbacks */
	edje_object_signal_callback_add(p->edje,
					EDJE_SIGNAL_NOTE_CLOSE, "",
					(void *) note_edje_close, *note);
	edje_object_signal_callback_add(p->edje,
					EDJE_SIGNAL_NOTE_MINIMISE, "",
					(void *) note_edje_minimise, *note);
	edje_object_signal_callback_add(p->edje, EDJE_SIGNAL_NOTE_SAVE, "",
					(void *) note_edje_save, *note);
	edje_object_signal_callback_add(p->edje, EDJE_SIGNAL_NOTE_SHADE, "",
					(void *) note_edje_shade, *note);
	edje_object_signal_callback_add(p->edje, EDJE_SIGNAL_NOTE_UNSHADE, "",
					(void *) note_edje_unshade, *note);

	/* Free Temporarily used Variables */
	if (datestr != NULL)
		free(datestr);
	if (edjefn != NULL)
		free(edjefn);
	if (fcontent != NULL)
		free(fcontent);

	/* Values Comparison Timer */
	p->timcomp = ecore_timer_add(COMPARE_INTERVAL, &timer_val_compare, p);

	if (saveload != NULL) {
		char           *title;

		title = get_title_by_note(*note);

		/* FIXME more intelligent things once we have sorted saveload */
		ewl_saveload_revert(NULL, NULL, NULL);
		dml("Added new note to saveload list", 2);

		if (title)
			free(title);
	}

	p->dialog = NULL;
	return;
}

void
configure_scrollbars_default(Ewl_Widget * pane)
{
	ewl_theme_data_str_set(pane, "/vscrollbar/increment/file", NULL);
	ewl_theme_data_str_set(pane, "/vscrollbar/decrement/file", NULL);
	ewl_theme_data_str_set(pane, "/vscrollbar/vseeker/file", NULL);
	ewl_theme_data_str_set(pane, "/vscrollbar/vseeker/vbutton/file", NULL);
	ewl_theme_data_str_set(pane, "/hscrollbar/increment/file", NULL);
	ewl_theme_data_str_set(pane, "/hscrollbar/decrement/file", NULL);
	ewl_theme_data_str_set(pane, "/hscrollbar/hseeker/file", NULL);
	ewl_theme_data_str_set(pane, "/hscrollbar/hseeker/hbutton/file", NULL);
	ewl_theme_data_str_set(pane, "/scrollpane/file", NULL);

	ewl_theme_data_str_set(pane, "/vscrollbar/increment/group", NULL);
	ewl_theme_data_str_set(pane, "/vscrollbar/decrement/group", NULL);

	ewl_theme_data_str_set(pane, "/vscrollbar/vseeker/group", NULL);

	ewl_theme_data_str_set(pane, "/vscrollbar/vseeker/vbutton/group", NULL);

	ewl_theme_data_str_set(pane, "/hscrollbar/increment/group", NULL);
	ewl_theme_data_str_set(pane, "/hscrollbar/decrement/group", NULL);

	ewl_theme_data_str_set(pane, "/hscrollbar/hseeker/group", NULL);

	ewl_theme_data_str_set(pane, "/hscrollbar/hseeker/hbutton/group", NULL);
	ewl_theme_data_str_set(pane, "/scrollpane/group", NULL);

	return;
}

void
configure_scrollbars(Ewl_Widget * pane, char *edjefn)
{
	ewl_theme_data_str_set(pane, "/vscrollbar/increment/file", edjefn);
	ewl_theme_data_str_set(pane, "/vscrollbar/decrement/file", edjefn);
	ewl_theme_data_str_set(pane, "/vscrollbar/vseeker/file", edjefn);
	ewl_theme_data_str_set(pane, "/vscrollbar/vseeker/vbutton/file",
			       edjefn);
	ewl_theme_data_str_set(pane, "/hscrollbar/increment/file", edjefn);
	ewl_theme_data_str_set(pane, "/hscrollbar/decrement/file", edjefn);
	ewl_theme_data_str_set(pane, "/hscrollbar/hseeker/file", edjefn);
	ewl_theme_data_str_set(pane, "/hscrollbar/hseeker/hbutton/file",
			       edjefn);
	ewl_theme_data_str_set(pane, "/scrollpane/file", edjefn);

	ewl_theme_data_str_set(pane, "/vscrollbar/increment/group",
			       EDJE_VSCROLLBAR_BTN_INCR);
	ewl_theme_data_str_set(pane, "/vscrollbar/decrement/group",
			       EDJE_VSCROLLBAR_BTN_DECR);
	ewl_theme_data_str_set(pane, "/vscrollbar/vseeker/group",
			       EDJE_VSCROLLBAR_SEEKER);
	ewl_theme_data_str_set(pane, "/vscrollbar/vseeker/vbutton/group",
			       EDJE_SCROLLBAR_BUTTON);
	ewl_theme_data_str_set(pane, "/hscrollbar/increment/group",
			       EDJE_HSCROLLBAR_BTN_INCR);
	ewl_theme_data_str_set(pane, "/hscrollbar/decrement/group",
			       EDJE_HSCROLLBAR_BTN_DECR);
	ewl_theme_data_str_set(pane, "/hscrollbar/hseeker/group",
			       EDJE_HSCROLLBAR_SEEKER);
	ewl_theme_data_str_set(pane, "/hscrollbar/hseeker/hbutton/group",
			       EDJE_SCROLLBAR_BUTTON);
	ewl_theme_data_str_set(pane, "/scrollpane/group", EDJE_SCROLLPANE);

	return;
}

/* MENU Callbacks */

void
cb_menu_rightclick(Note * p, Evas * e, Evas_Object * obj, void *ev_info)
{
	Menu           *menu = menu_create();

	menu_item_add(p->menu, "New Note", (void *) cb_ewl_new_note, NULL);
	menu_show(menu);
	return;
}

void
cb_ewl_new_note(void *data)
{
}

/* ECORE Callbacks */

/**
 * @param ee: The Ecore_Evas which has been resized.
 * @brief: Ecore callback on a window resizing.
 *         Resizes the objects inside the window to
 *         compensate to the new size.
 */
void
note_ecore_resize(Ecore_Evas * ee)
{
	int             x, y, w, h;

	dml("Resizing Note", 2);

	ecore_evas_geometry_get(ee, &x, &y, &w, &h);
	evas_object_resize(evas_object_name_find
			   (ecore_evas_get(ee), "edje"), w, h);
	evas_object_resize(evas_object_name_find(ecore_evas_get(ee), "dragger"),
			   w, h);

	return;
}

/**
 * @param ee: Ecore_Evas which has been requested to close.
 * @brief: Ecore callback which dictates that the wm wants the note closing.
 *         Closes the note.
 */
void
note_ecore_close(Ecore_Evas * ee)
{
	Evas_List      *p = gbl_notes;
	Note           *note;

	if (ee == NULL)
		return;
	while (p != NULL) {
		note = evas_list_data(p);
		if (note->win == ee) {
			remove_note((Evas_List *) p);
			return;
		}
		p = evas_list_next(p);
	}
	return;
}

/* EDJE Callbacks */

/**
 * @param note: Evas_List of the note which is to be closed.
 * @param o: Evas_Object of the object clicked (not used).
 * @param emission: The signal string (not used).
 * @param source: The source of the signal (not used).
 * @brief: Edje callback to close.  Closes the note via a timer
 *         to save trouble with signals, etc... when it all gets freed.
 */
void
note_edje_close(Evas_List * note, Evas_Object * o,
		const char *emission, const char *source)
{
	Ecore_Timer    *timer;

	timer = ecore_timer_add(0.001, &note_edje_close_timer, note);
	return;
}

/**
 * @param note: Evas_List of the note which is to be minimised.
 * @param o: Evas_Object of the object clicked (not used).
 * @param emission: The signal string (not used).
 * @param source: The source of the signal (not used).
 * @brief: Edje callback to minimise.  Minimises the window.
 */
void
note_edje_minimise(Evas_List * note, Evas_Object * o,
		   const char *emission, const char *source)
{
	Note           *p;

	dml("Minimising a Note", 2);

	p = evas_list_data(note);

	/* FIXME: The line below should be removed when
	 * ecore_evas is fixed. */
	ecore_evas_iconified_set(p->win, 0);

	ecore_evas_iconified_set(p->win, 1);

	return;
}

void
note_edje_save(Evas_List * note, Evas_Object * o,
	       const char *emission, const char *source)
{
	Note           *p;
	char           *title;

	dml("Saving a Note", 2);
	p = evas_list_data(note);

	title = get_title_by_content(ewl_text_text_get
				     ((Ewl_Text *) p->content));
	ewl_saveload_save_by_name(title);

	if (title)
		free(title);
	return;
}

void
note_edje_shade(Evas_List * note, Evas_Object * o,
		const char *emission, const char *source)
{
	Note           *p;

	p = evas_list_data(note);
	p->shaded = 1;
}

void
note_edje_unshade(Evas_List * note, Evas_Object * o,
		  const char *emission, const char *source)
{
	Note           *p;

	p = evas_list_data(note);
	p->shaded = 0;
}

/* Misc */

/**
 * @return: Returns the string containing the date (needs free'ing)
 * @brief: Grabs and formats the time into a string.
 */
char           *
get_date_string(void)
{
	char           *retval = malloc(20);
	time_t          tmp;
	struct tm      *localtm;

	tmp = time(NULL);
	localtm = (struct tm *) gmtime(&tmp);
	strftime(retval, 19, "%d/%m/%y", localtm);

	return (retval);
}

/**
 * @param p: Evas_List pointing to the note to be closed.
 * @return: Integer dictating whether the timer ends.
 * @brief: This timer is called from the edje callback to close
 *         the window to save problems with signals when the objects
 *         are freed.  It close the note and ends its own timer by
 *         returning 0.
 */
int
note_edje_close_timer(void *p)
{
	remove_note((Evas_List *) p);
	return (0);
}

/**
 * @param data: The Note of the note which is being checked.
 * @return: Integer dictating whether the timer ends.
 * @brief: Compares the values of the title to the stored values (keep getting
 *         updated) to decide whether to change the value inside of saveload if
 *         required.  This is a timer.
 */
int
timer_val_compare(void *data)
{
	Note           *p;
	char           *tmp;

	p = (Note *) data;
	if (!p->timcomp)
		return (0);

	if (p->txt_title) {
		tmp = get_title_by_note_struct(p);
		if (!tmp || strcmp(p->txt_title, tmp)) {
			if (saveload)
				ewl_saveload_revert(NULL, NULL, saveload->tree);

			if (p->txt_title)
				free(p->txt_title);
			p->txt_title = get_title_by_note_struct(p);
		}
		if (tmp)
			free(tmp);
	} else {
		p->txt_title = get_title_by_note_struct(p);
	}

	update_enote_title(p->edje, p->txt_title);

	return (1);
}

/* External Interaction */

int
get_note_count()
{
	int             a;
	Evas_List      *p;

	p = get_cycle_begin();
	if (p == NULL)
		return (0);
	else
		a = 1;
	while ((p = get_cycle_next_note(p)) != NULL)
		a++;

	return (a);
}

void
notes_update_themes(void)
{
	int             edje_w, edje_h;
	Evas_List      *working;
	Note           *note;

	char           *edjefn = malloc(PATH_MAX);

	snprintf(edjefn,
		 PATH_MAX, PACKAGE_DATA_DIR "/themes/%s.edj",
		 main_config->theme);

	working = get_cycle_begin();
	if (working != NULL) {
		while (working != NULL) {
			note = (Note *) evas_list_data(working);
			if (note != NULL) {
				edje_object_file_set(note->edje, edjefn,
						     NOTE_PART);
				edje_object_size_max_get(note->edje, &edje_w,
							 &edje_h);
				ecore_evas_size_max_set(note->win, edje_w,
							edje_h);
				edje_object_size_min_get(note->edje, &edje_w,
							 &edje_h);
				ecore_evas_size_min_set(note->win, edje_w,
							edje_h);
				edje_object_part_swallow(note->edje,
							 EDJE_CONTAINER,
							 note->eo);
				if (edje_object_data_get
				    (note->edje,
				     EDJE_INFO_SCROLLBARS) != NULL) {
					/* FIXME: What the fuck is happening when
					 * we enable this?: */
//                                      configure_scrollbars(note->pane,edjefn);
				} else {
//                                      configure_scrollbars_default(note->pane);
				}
			}
			working = get_cycle_next_note(working);
		}
	}

	free(edjefn);
	return;
}

/**
 * @param title: The title to search for.
 * @return: Returns the Evas_List of the note requested by "title".
 * @brief: Searches for and returns the note with the title being "title"
 */
Evas_List      *
get_note_by_title(char *title)
{
	Evas_List      *a;
	char           *note_title;

	a = get_cycle_begin();
	note_title = get_title_by_note(a);
	if (!strcmp(note_title, title)) {
		if (note_title)
			free(note_title);
		return (a);
	}
	if (note_title)
		free(note_title);
	while ((a = get_cycle_next_note(a)) != NULL) {
		note_title = get_title_by_note(a);
		if (!strcmp(note_title, title)) {
			if (note_title)
				free(note_title);
			return (a);
		}
		if (note_title)
			free(note_title);
	}
	return (NULL);
}

/**
 * @param note: The note to grab the title from.
 * @return: Returns the title of the supplied note.
 * @brief: Returns the title text of the supplied note.
 */
char           *
get_title_by_note(Evas_List * note)
{
	return (get_title_by_content(get_content_by_note(note)));
}

/**
 * @param note: The note to grab the title from (actual).
 * @return: Returns the title of the supplied note.
 * @brief: Returns the title text of the supplied note.
 */
char           *
get_title_by_note_struct(Note * note)
{
	char           *content, *title;

	content = get_content_by_note_struct(note);
	title = get_title_by_content(content);

	if (content)
		free(content);
	return title;
}

/**
 * @param note: The note to grab the content from.
 * @return: Returns the content of the supplied note.
 * @brief: Returns the content text of the supplied note.
 */
char           *
get_content_by_note(Evas_List * note)
{
	Note           *p = evas_list_data(note);

	return ((char *) ewl_text_text_get((Ewl_Text *) p->content));
}

/**
 * @param note: The note to grab the content from (actual).
 * @return: Returns the content of the supplied note.
 * @brief: Returns the content text of the supplied note.
 */
char           *
get_content_by_note_struct(Note * note)
{
	return ((char *) ewl_text_text_get((Ewl_Text *) note->content));
}

/**
 * @param content: The content of the note.
 * @return: The title from the content.
 * @brief: Takes TITLE_LENGTH worth of characters
 *         from the front (or newline).
 */
char           *
get_title_by_content(char *content)
{
	char           *cont, *start;
	int             newlength, i;

	if (!content)
		return NULL;
	newlength = i = 0;

	start = content;
	while (newlength < TITLE_LENGTH && content[i] != '\0') {
		if (content[i++] == '\n') {
			if (newlength == 0) {
				start++;
				continue;
			} else {
				break;
			}
		}
		newlength++;
	}

	if (newlength == 0)
		return "(blank)";

	cont = malloc((newlength + 1) * sizeof(char));
	snprintf(cont, newlength + 1, "%s", start);
	return cont;
}

/**
 * @return: Returns the beginning node of the note list cycle.
 * @brief: Begin the note list cycle.
 */
Evas_List      *
get_cycle_begin(void)
{
	return (gbl_notes);
}

/**
 * @param note: The note to move forward from.
 * @return: Returns the node to the next note in the cycle.
 * @brief: Move to the next note in the cycle.
 */
Evas_List      *
get_cycle_next_note(Evas_List * note)
{
	return (evas_list_next(note));
}

/**
 * @param note: The note to move backwards from.
 * @return: Returns the node to the previous note in the cycle.
 * @brief: Move to the previous note in the cycle.
 */
Evas_List      *
get_cycle_previous_note(Evas_List * note)
{
	return (evas_list_prev(note));
}

/**
 * @param w: The widget to size according to the embed.
 * @params ev_data and user_data: Callback info.
 * @brief: Moves embed contents to correct location.
 */
void
note_move_embed(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_object_geometry_request(EWL_OBJECT(user_data), CURRENT_X(w),
				    CURRENT_Y(w), CURRENT_W(w), CURRENT_H(w));
}

/**
 * @param content: The content to use for title setting.
 * @brief: Sets the title in the edje.
 */
void
update_enote_title(Evas_Object * edje, char *content)
{
	char           *title;

	title = get_title_by_content(content);
	edje_object_part_text_set(edje, EDJE_TEXT_TITLE, title);
	if (title)
		free(title);
	return;
}
