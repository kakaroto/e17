
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


#include "saveload.h"

char           *saveload_selected;
SaveLoad       *saveload;

char           *load_selected;
Load           *load;



/** SAVE/LOAD WINDOW **/

void
setup_saveload(void)
{
	if (saveload == NULL) {
		dml("Opening the Saveload Window", 2);
		saveload = malloc(sizeof(SaveLoad));
		setup_saveload_win();
		fill_saveload_tree();
	} else {
		dml("Won't Open Another Saveload Window", 2);
	}
	return;
}

void
setup_saveload_win(void)
{
	char           *headers[1];

	/* Setup the Window */
	saveload->win =
		ecore_evas_software_x11_new(NULL, 0, SAVELOAD_X, SAVELOAD_Y,
					    SAVELOAD_W, SAVELOAD_H);
	ecore_evas_title_set(saveload->win, "E-Notes: Save/Load");
	ecore_evas_show(saveload->win);

	/* Setup the Canvas, Render-Method */
	saveload->evas = ecore_evas_get(saveload->win);
	evas_output_method_set(saveload->evas,
			       evas_render_method_lookup(main_config->
							 render_method));

	/* Setup the EWL Widgets */
	saveload->emb = ewl_embed_new();
	ewl_object_set_fill_policy((Ewl_Object *) saveload->emb,
				   EWL_FLAG_FILL_FILL);
	ewl_widget_set_appearance(saveload->emb, "window");
	ewl_widget_show(saveload->emb);

	saveload->eo =
		ewl_embed_set_evas((Ewl_Embed *) saveload->emb, saveload->evas,
				   ecore_evas_software_x11_window_get(saveload->
								      win));
	evas_object_name_set(saveload->eo, "eo");
	evas_object_layer_set(saveload->eo, 0);
	evas_object_move(saveload->eo, 0, 0);
	evas_object_resize(saveload->eo, SAVELOAD_W, SAVELOAD_H);
	evas_object_show(saveload->eo);

	saveload->vbox = ewl_vbox_new();
	ewl_container_append_child((Ewl_Container *) saveload->emb,
				   saveload->vbox);
	ewl_object_set_fill_policy((Ewl_Object *) saveload->vbox,
				   EWL_FLAG_FILL_FILL);
	ewl_widget_show(saveload->vbox);

	saveload->tree = ewl_tree_new(1);
	ewl_container_append_child((Ewl_Container *) saveload->vbox,
				   saveload->tree);
	ewl_object_set_fill_policy((Ewl_Object *) saveload->tree,
				   EWL_FLAG_FILL_FILL);

	headers[0] = strdup("Note Title");
	ewl_tree_set_headers((Ewl_Tree *) saveload->tree, headers);
	free(headers[0]);

	ewl_widget_show(saveload->tree);

	saveload->txt_selected = ewl_text_new("Selected: N/A");
	ewl_container_append_child((Ewl_Container *) saveload->vbox,
				   saveload->txt_selected);
	ewl_widget_show(saveload->txt_selected);

	saveload->hbox = ewl_hbox_new();
	ewl_container_append_child((Ewl_Container *) saveload->vbox,
				   saveload->hbox);
	ewl_object_set_fill_policy((Ewl_Object *) saveload->hbox,
				   EWL_FLAG_FILL_HFILL);
	ewl_widget_show(saveload->hbox);

	saveload_setup_button(saveload->hbox, &saveload->savebtn, "Save.");
	saveload_setup_button(saveload->hbox, &saveload->loadbtn, "Load.");
	saveload_setup_button(saveload->hbox, &saveload->refreshbtn,
			      "Refresh.");
	saveload_setup_button(saveload->hbox, &saveload->closebtn, "Close.");

	/* Ecore Callbacks */
	ecore_evas_callback_resize_set(saveload->win, ecore_saveload_resize);
	ecore_evas_callback_delete_request_set(saveload->win,
					       ecore_saveload_close);
	ecore_evas_callback_destroy_set(saveload->win, ecore_saveload_close);

	/* EWL Callbacks */
	ewl_callback_append(saveload->refreshbtn, EWL_CALLBACK_CLICKED,
			    (void *) ewl_saveload_revert,
			    (void *) saveload->tree);
	ewl_callback_append(saveload->closebtn, EWL_CALLBACK_CLICKED,
			    (void *) ewl_saveload_close,
			    (void *) saveload->win);
	ewl_callback_append(saveload->savebtn, EWL_CALLBACK_CLICKED,
			    (void *) ewl_saveload_save, NULL);
	ewl_callback_append(saveload->loadbtn, EWL_CALLBACK_CLICKED,
			    (void *) ewl_saveload_load, NULL);

	return;
}

void
saveload_setup_button(Ewl_Widget * c, Ewl_Widget ** b, char *label)
{
	*b = ewl_button_new(label);
	ewl_container_append_child((Ewl_Container *) c, *b);
	ewl_widget_show(*b);
	return;
}

void
fill_saveload_tree(void)
{
	Evas_List      *p;

	p = (Evas_List *) get_cycle_begin();
	if (p != NULL) {
		setup_saveload_opt(saveload->tree,
				   (char *) get_title_by_note(p));
		while ((p = (Evas_List *) get_cycle_next_note(p)) != NULL) {
			if (strcmp(get_title_by_note(p), "")) {
				setup_saveload_opt(saveload->tree,
						   (char *)
						   get_title_by_note(p));
			}
		}
	}
	return;
}

void
setup_saveload_opt(Ewl_Widget * tree, char *caption)
{
	Ewl_Widget     *capt;

	capt = ewl_text_new(caption);
	ewl_callback_append(capt, EWL_CALLBACK_CLICKED,
			    (void *) ewl_saveload_listitem_click, NULL);
	ewl_widget_show(capt);
	ewl_tree_add_row((Ewl_Tree *) tree, 0, &capt);
	return;
}

/* Callbacks */
void
ecore_saveload_resize(Ecore_Evas * ee)
{
	int             x, y, w, h;

	dml("Resizing the Saveload Window", 2);

	ecore_evas_geometry_get(ee, &x, &y, &w, &h);
	evas_object_resize(evas_object_name_find(ecore_evas_get(ee), "eo"),
			   w, h);
	return;
}

void
ecore_saveload_close(Ecore_Evas * ee)
{
	dml("Closing the Saveload Window", 2);
	ecore_evas_free(ee);
	free(saveload);
	saveload = NULL;
	saveload_selected = NULL;
	return;
}

void
ewl_saveload_revert(Ewl_Widget * widget, void *ev_data, Ewl_Widget * p)
{
	dml("Refreshing the Saveload List", 2);
	ewl_container_reset((Ewl_Container *) p);
	fill_saveload_tree();
	return;
}

void
ewl_saveload_close(Ewl_Widget * o, void *ev_data, Ecore_Evas * ee)
{
	ecore_saveload_close(ee);
	return;
}

void
ewl_saveload_listitem_click(Ewl_Widget * o, void *ev_data, void *null)
{
	char           *tmp = malloc(MAX_TITLE);

	saveload_selected = ewl_text_get_text((Ewl_Text *) o);
	snprintf(tmp, MAX_TITLE, "Selected: %s", saveload_selected);
	ewl_text_set_text((Ewl_Text *) saveload->txt_selected, tmp);
	free(tmp);
	return;
}

void
ewl_saveload_load(Ewl_Widget * o, void *ev_data, void *null)
{
	setup_load();
	return;
}

void
ewl_saveload_save(Ewl_Widget * o, void *ev_data, void *null)
{
	int             x, y, w, h;
	Note           *note;
	Evas_List      *p;
	NoteStor       *n = malloc(sizeof(NoteStor));

	dml("Saving Selection", 2);

	if (saveload_selected == NULL)
		return;
	if ((p = get_note_by_title(saveload_selected)) == NULL) {
		dml("Trying to save a note that doesn't exist", 2);
		return;
	}
	note = evas_list_data(p);
	ecore_evas_geometry_get(note->win, &x, &y, &w, &h);
	n->width = w;
	n->height = h;
	n->title = strdup(get_title_by_note(p));
	n->content = strdup(get_content_by_note(p));

	append_note_stor(n);

	free_note_stor(n);
	return;
}



/** LOAD WINDOW **/

void
setup_load(void)
{
	if (load == NULL) {
		dml("Opening the Load Note Window", 2);
		load = malloc(sizeof(Load));
		setup_load_win();
		fill_load_tree();
	} else {
		dml("Won't Open Another Load Note Window", 2);
	}
	return;
}

void
setup_load_win(void)
{
	char           *headers[1];

	/* Setup the Window */
	load->win = ecore_evas_software_x11_new(NULL, 0, LOAD_X, LOAD_Y,
						LOAD_W, LOAD_H);
	ecore_evas_title_set(load->win, "E-Notes: Load Note");
	ecore_evas_show(load->win);

	/* Setup the Canvas, Render-Method */
	load->evas = ecore_evas_get(load->win);
	evas_output_method_set(load->evas,
			       evas_render_method_lookup(main_config->
							 render_method));

	/* Setup the EWL Widgets */
	load->emb = ewl_embed_new();
	ewl_object_set_fill_policy((Ewl_Object *) load->emb,
				   EWL_FLAG_FILL_FILL);
	ewl_widget_set_appearance(load->emb, "window");
	ewl_widget_show(load->emb);

	load->eo = ewl_embed_set_evas((Ewl_Embed *) load->emb, load->evas,
				      ecore_evas_software_x11_window_get(load->
									 win));
	evas_object_name_set(load->eo, "eo");
	evas_object_layer_set(load->eo, 0);
	evas_object_move(load->eo, 0, 0);
	evas_object_resize(load->eo, LOAD_W, LOAD_H);
	evas_object_show(load->eo);

	load->vbox = ewl_vbox_new();
	ewl_container_append_child((Ewl_Container *) load->emb, load->vbox);
	ewl_object_set_fill_policy((Ewl_Object *) load->vbox,
				   EWL_FLAG_FILL_FILL);
	ewl_widget_show(load->vbox);

	load->tree = ewl_tree_new(1);
	ewl_container_append_child((Ewl_Container *) load->vbox, load->tree);
	ewl_object_set_fill_policy((Ewl_Object *) load->tree,
				   EWL_FLAG_FILL_FILL);

	headers[0] = strdup("Note Title");
	ewl_tree_set_headers((Ewl_Tree *) load->tree, headers);
	free(headers[0]);

	ewl_widget_show(load->tree);

	load->txt_selected = ewl_text_new("Selected: N/A");
	ewl_container_append_child((Ewl_Container *) load->vbox,
				   load->txt_selected);
	ewl_widget_show(load->txt_selected);

	load->hbox = ewl_hbox_new();
	ewl_container_append_child((Ewl_Container *) load->vbox, load->hbox);
	ewl_object_set_fill_policy((Ewl_Object *) load->hbox,
				   EWL_FLAG_FILL_HFILL);
	ewl_widget_show(load->hbox);

	load_setup_button(load->hbox, &load->loadbtn, "Load.");
	load_setup_button(load->hbox, &load->deletebtn, "Delete.");
	load_setup_button(load->hbox, &load->refreshbtn, "Refresh.");
	load_setup_button(load->hbox, &load->closebtn, "Close.");

	/* Ecore Callbacks */
	ecore_evas_callback_resize_set(load->win, ecore_load_resize);
	ecore_evas_callback_delete_request_set(load->win, ecore_load_close);
	ecore_evas_callback_destroy_set(load->win, ecore_load_close);

	/* EWL Callbacks */
	ewl_callback_append(load->refreshbtn, EWL_CALLBACK_CLICKED,
			    (void *) ewl_load_revert, (void *) load->tree);
	ewl_callback_append(load->closebtn, EWL_CALLBACK_CLICKED,
			    (void *) ewl_load_close, (void *) load->win);
	ewl_callback_append(load->loadbtn, EWL_CALLBACK_CLICKED,
			    (void *) ewl_load_load, NULL);
	ewl_callback_append(load->deletebtn, EWL_CALLBACK_CLICKED,
			    (void *) ewl_load_delete, NULL);

	return;
}

void
load_setup_button(Ewl_Widget * c, Ewl_Widget ** b, char *label)
{
	*b = ewl_button_new(label);
	ewl_container_append_child((Ewl_Container *) c, *b);
	ewl_widget_show(*b);
	return;
}

void
fill_load_tree(void)
{
	NoteStor       *p;
	XmlReadHandle  *r;

	r = stor_cycle_begin();

	if (r != NULL) {
		while (r->cur != NULL) {
			p = stor_cycle_get_notestor(r);
			setup_load_opt(load->tree, p->title);
			free_note_stor(p);
			stor_cycle_next(r);
		}
		stor_cycle_end(r);
	}
	return;
}

void
setup_load_opt(Ewl_Widget * tree, char *caption)
{
	Ewl_Widget     *capt;

	capt = ewl_text_new(caption);
	ewl_callback_append(capt, EWL_CALLBACK_CLICKED,
			    (void *) ewl_load_listitem_click, NULL);
	ewl_widget_show(capt);
	ewl_tree_add_row((Ewl_Tree *) tree, 0, &capt);
	return;
}

void
ecore_load_resize(Ecore_Evas * ee)
{
	int             x, y, w, h;

	dml("Resizing the Load Note Window", 2);

	ecore_evas_geometry_get(ee, &x, &y, &w, &h);
	evas_object_resize(evas_object_name_find(ecore_evas_get(ee), "eo"),
			   w, h);
	return;
}

void
ecore_load_close(Ecore_Evas * ee)
{
	dml("Closing the Load Note Window", 2);
	ecore_evas_free(ee);
	free(load);
	load = NULL;
	load_selected = NULL;
	return;
}

void
ewl_load_revert(Ewl_Widget * widget, void *ev_data, Ewl_Widget * p)
{
	dml("Refreshing the Load Note List", 2);
	ewl_container_reset((Ewl_Container *) load->tree);
	fill_load_tree();
	return;
}

void
ewl_load_close(Ewl_Widget * o, void *ev_data, Ecore_Evas * ee)
{
	ecore_load_close(ee);
	return;
}

void
ewl_load_load(Ewl_Widget * o, void *ev_data, void *null)
{
	NoteStor       *p;
	XmlReadHandle  *r;

	dml("Loading Saved Note", 2);

	r = stor_cycle_begin();

	if (r != NULL) {
		while (r->cur != NULL) {
			p = stor_cycle_get_notestor(r);
			if (!strcmp(p->title, load_selected))
				new_note_with_values(p->width, p->height,
						     p->title, p->content);
			free_note_stor(p);
			stor_cycle_next(r);
		}
		stor_cycle_end(r);
	}
	return;
}

void
ewl_load_listitem_click(Ewl_Widget * o, void *ev_data, void *null)
{
	char           *tmp = malloc(MAX_TITLE);

	load_selected = ewl_text_get_text((Ewl_Text *) o);
	snprintf(tmp, MAX_TITLE, "Selected: %s", load_selected);
	ewl_text_set_text((Ewl_Text *) load->txt_selected, tmp);
	free(tmp);
	return;
}

void
ewl_load_delete(Ewl_Widget * o, void *ev_data, void *null)
{
	NoteStor       *p;
	XmlReadHandle  *r;

	dml("Deleting Saved Note", 2);

	r = stor_cycle_begin();
	if (r != NULL) {
		while (r->cur != NULL) {
			p = stor_cycle_get_notestor(r);
			if (!strcmp(p->title, load_selected)) {
				break;
			}
			free_note_stor(p);
			stor_cycle_next(r);
		}
	}

	stor_cycle_end(r);
	remove_note_stor(p);

	free_note_stor(p);
	ewl_load_revert(NULL, NULL, NULL);
	return;
}
