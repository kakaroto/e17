
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
	setup_note(&new, 0, 0, DEF_TITLE, DEF_CONTENT);
	return;
}

/**
 * @param width: Width of the new note.
 * @param height: Height of the new note.
 * @param title: Title text to begin with.
 * @param content: Content text to begin with.
 * @brief: Opens a new note.
 */
void
new_note_with_values(int width, int height, char *title, char *content)
{
	Evas_List      *new;

	dml("Creating a Note", 2);

	new = append_note();
	setup_note(&new, width, height, title, content);
	return;
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
	Note           *p = evas_list_data(note);
	char           *note_title;

	dml("Closing a Note", 2);

	ecore_timer_del(p->timcomp);
	ecore_evas_free(p->win);
	free(p);
	gbl_notes = evas_list_remove_list(gbl_notes, note);

	/** 
	 * FIXME: When you can get the row and its child text, compare
	 * it to the ewl_entry_get_text(p->title) value and remove the row
	 * from the tree at this point.  Reporting that you've done so with
	 * dml ("Removed note from save/load list", 2); or something.  When ewl
	 * will let you do these things.
	 */

	if (saveload != NULL) {
		dml("Removing note entry from saveload list", 2);
		ewl_tree_destroy_row((Ewl_Tree *) saveload->tree,
				     p->saveload_row);
	}

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
setup_note(Evas_List ** note, int width, int height, char *title, char *content)
{
	Evas_List      *pl;
	Note           *p;

	char           *fontpath = malloc(PATH_MAX);
	char           *edjefn = malloc(PATH_MAX);
	char           *datestr;
	char           *fcontent;

	double          edje_w, edje_h;

	/* Fix Newlines in Content */
	fcontent = fix_newlines(content);

	/* Get the Note from the Evas_List** */
	pl = *note;
	p = evas_list_data(pl);

	/* Setup the Window */
	p->win = ecore_evas_software_x11_new(NULL, 0, 0, 0, width, height);
	ecore_evas_title_set(p->win, "An E-Note");
	ecore_evas_name_class_set (p->win, "Enotes", "Enotes");
	ecore_evas_borderless_set(p->win, 1);
	ecore_evas_shaped_set(p->win, 1);
	ecore_evas_show(p->win);

	/* Setup the Canvas, fonts, etc... */
	p->evas = ecore_evas_get(p->win);
	evas_output_method_set(p->evas,
			       evas_render_method_lookup(main_config->
							 render_method));
	snprintf(fontpath, PATH_MAX, "%s/data/fonts", PACKAGE_DATA_DIR);
	evas_font_path_append(p->evas, fontpath);

	/* Draggable Setup */
	p->dragger = (Evas_Object *) esmart_draggies_new(p->win);
	evas_object_name_set(p->dragger, "dragger");
	evas_object_move(p->dragger, 0, 0);
	evas_object_resize(p->dragger, width, height);
	evas_object_layer_set(p->dragger, 0);
	evas_object_color_set(p->dragger, 255, 255, 255, 0);
	esmart_draggies_button_set(p->dragger, 1);
	evas_object_show(p->dragger);

	/* Setup the Edje */
	p->edje = edje_object_add(p->evas);
	snprintf(edjefn,
		 PATH_MAX, NOTE_EDJE, PACKAGE_DATA_DIR, main_config->theme);
	edje_object_file_set(p->edje, edjefn, NOTE_PART);
	evas_object_name_set(p->edje, "edje");
	evas_object_move(p->edje, 0, 0);
	evas_object_layer_set(p->edje, 1);

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

	/* Setup the date and user */
	edje_object_part_text_set(p->edje, "EnotesUser", getenv("USER"));
	datestr = get_date_string();
	edje_object_part_text_set(p->edje, "EnotesDate", datestr);

	/* Ewl */
	p->emb = ewl_embed_new();
	ewl_object_set_fill_policy((Ewl_Object *) p->emb, EWL_FLAG_FILL_FILL);
	ewl_widget_show(p->emb);
	p->eo = ewl_embed_set_evas(EWL_EMBED(p->emb),
				   ecore_evas_get(p->win), (void *)
				   ecore_evas_software_x11_window_get(p->win));
	evas_object_layer_set(p->eo, 2);
	edje_object_part_swallow(p->edje, EDJE_EWL_CONTAINER, p->eo);

	evas_object_show(p->eo);

	p->vbox = ewl_vbox_new();
	ewl_object_set_fill_policy((Ewl_Object *) p->vbox, EWL_FLAG_FILL_FILL);
	ewl_container_append_child((Ewl_Container *) p->emb, p->vbox);
	ewl_widget_show(p->vbox);

	p->title = ewl_entry_new(title);
	ewl_container_append_child((Ewl_Container *) p->vbox, p->title);
	ewl_widget_show(p->title);

	p->content = ewl_textarea_new(fcontent);
	ewl_container_append_child((Ewl_Container *) p->vbox, p->content);
	ewl_widget_show(p->content);

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

	/* Free Temporarily used Variables */
	if (datestr != NULL)
		free(datestr);
	if (edjefn != NULL)
		free(edjefn);
	if (fontpath != NULL)
		free(fontpath);
	if (fcontent != NULL)
		free(fcontent);

	/* Values Comparison Timer */
	p->timcomp = ecore_timer_add(COMPARE_INTERVAL, &timer_val_compare, p);

	if (saveload != NULL) {
		setup_saveload_opt(saveload->tree, (char *)
				   get_title_by_note(*note), *note);
		dml("Added new note to saveload list", 2);
	}
	return;
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
	ecore_evas_iconified_set(p->win, 1);

	return;
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
	Note           *p = (Note *) data;

	if (p->txt_title != NULL) {
		if (strcmp
		    (p->txt_title,
		     ewl_entry_get_text((Ewl_Entry *) p->title))) {
			if (saveload != NULL)
				ewl_saveload_revert(NULL, NULL, saveload->tree);

			free(p->txt_title);
			p->txt_title =
				ewl_entry_get_text((Ewl_Entry *) p->title);
		}
	} else {
		p->txt_title = ewl_entry_get_text((Ewl_Entry *) p->title);
	}
	return (1);
}

/* External Interaction */

/**
 * @param title: The title to search for.
 * @return: Returns the Evas_List of the note requested by "title".
 * @brief: Searches for and returns the note with the title being "title"
 */
Evas_List      *
get_note_by_title(char *title)
{
	Evas_List      *a;

	a = get_cycle_begin();
	if (!strcmp(get_title_by_note(a), title)) {
		return (a);
	}
	while ((a = get_cycle_next_note(a)) != NULL) {
		if (!strcmp(get_title_by_note(a), title)) {
			return (a);
		}
	}
	return (NULL);
}

/**
 * @param content: The content to search for.
 * @return: Returns the Evas_List of the note requested by "content".
 * @brief: Searches for and returns the note with the content being "content"
 */
Evas_List      *
get_note_by_content(char *content)
{
	Evas_List      *a;

	a = get_cycle_begin();
	if (!strcmp(get_content_by_note(a), content)) {
		return (a);
	}
	while ((a = get_cycle_next_note(a)) != NULL) {
		if (!strcmp(get_content_by_note(a), content)) {
			return (a);
		}
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
	Note           *p = evas_list_data(note);

	return (ewl_entry_get_text((Ewl_Entry *) p->title));
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

	return ((char *) ewl_textarea_get_text((Ewl_TextArea *) p->content));
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
