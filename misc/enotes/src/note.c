
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
void
new_note(void)
{
	Evas_List      *new;

	dml("Creating a Note", 2);

	new = append_note();
	setup_note(&new, 0, 0, DEF_TITLE, DEF_CONTENT);
	return;
}

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
Evas_List      *
append_note(void)
{
	Note           *note = malloc(sizeof(Note));

	gbl_notes = evas_list_append(gbl_notes, note);
	return (evas_list_find_list(gbl_notes, note));
}

void
remove_note(Evas_List * note)
{				/* Sometimes here it might segv from the loop for no particular reason. */
	Note           *p = evas_list_data(note);

	dml("Closing a Note", 2);

	ecore_evas_free(p->win);
	free(p);
	gbl_notes = evas_list_remove_list(gbl_notes, note);
	return;
}

/* GUI Setup */
void
setup_note(Evas_List ** note, int width, int height, char *title, char *content)
{
	Evas_List      *pl;
	Note           *p;

	char           *fontpath = malloc(PATH_MAX);
	char           *edjefn = malloc(PATH_MAX);
	char           *datestr;

	double          edje_w, edje_h;

	/* Get the Note from the Evas_List** */
	pl = *note;
	p = evas_list_data(pl);

	/* Setup the Window */
	p->win = ecore_evas_software_x11_new(NULL, 0, 0, 0, width, height);
	ecore_evas_title_set(p->win, "An E-Note");
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
	evas_object_layer_set(p->dragger, 999);
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
	evas_object_layer_set(p->edje, 0);

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
				   ecore_evas_get(p->win),
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

	p->content = ewl_textarea_new(content);
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

	return;
}

/* ECORE Callbacks */
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
void
note_edje_close(Evas_List * note, Evas_Object * o,
		const char *emission, const char *source)
{
	Ecore_Timer    *timer;

	timer = ecore_timer_add(0.001, &note_edje_close_timer, note);
	return;
}

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

int
note_edje_close_timer(void *p)
{
	remove_note((Evas_List *) p);
	return (0);
}

/* External Interaction */
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


char           *
get_title_by_note(Evas_List * note)
{
	Note           *p = evas_list_data(note);

	return (ewl_entry_get_text((Ewl_Entry *) p->title));
}

char           *
get_content_by_note(Evas_List * note)
{
	Note           *p = evas_list_data(note);

	return ((char *) ewl_textarea_get_text((Ewl_TextArea *) p->content));
}


Evas_List      *
get_cycle_begin(void)
{
	return (gbl_notes);
}

Evas_List      *
get_cycle_next_note(Evas_List * note)
{
	return (evas_list_next(note));
}

Evas_List      *
get_cycle_previous_note(Evas_List * note)
{
	return (evas_list_prev(note));
}
