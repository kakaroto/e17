/**
 * Ewler form manager
 */

#include <Ewl.h>

#include "config.h"
#include "ewler.h"

static Ewl_Widget *confirm_win;

static Ewler_Form *active_form;

static Ecore_List *forms;

static EWL_CALLBACK_DEFN(activate)
{
	active_form = ewl_widget_data_get(w, "EWLER_FORM");
}

static EWL_CALLBACK_DEFN(form_close_confirm_cb)
{
	Ewler_Form *form = user_data;
	int *response = ev_data;

	if( *response == EWL_RESPONSE_OK )
		form_close(form);

	ewl_widget_destroy(confirm_win);
	confirm_win = NULL;
}

static EWL_CALLBACK_DEFN(form_close_cb)
{
	Ewl_Widget *text, *yes, *no;

	active_form = ewl_widget_data_get(w, "EWLER_FORM");

	if( !active_form->dirty )
		form_close(active_form);

	if( confirm_win )
		return;

	confirm_win = ewl_dialog_new(EWL_POSITION_BOTTOM);
	if( !confirm_win )
		return;

	text = ewl_text_new("Form has not been saved, continue?");
	ewl_dialog_widget_add(EWL_DIALOG(confirm_win), text);
	ewl_widget_show(text);

	yes = ewl_dialog_button_add(EWL_DIALOG(confirm_win),
															EWL_STOCK_OK, EWL_RESPONSE_OK);
	ewl_button_label_set(EWL_BUTTON(yes), "Yes");
	ewl_callback_append(yes, EWL_CALLBACK_VALUE_CHANGED,
											form_close_confirm_cb, active_form);
	ewl_widget_show(yes);

	no = ewl_dialog_button_add(EWL_DIALOG(confirm_win),
														 EWL_STOCK_CANCEL, EWL_RESPONSE_CANCEL);
	ewl_button_label_set(EWL_BUTTON(no), "No");
	ewl_callback_append(no, EWL_CALLBACK_VALUE_CHANGED,
											form_close_confirm_cb, active_form);
	ewl_widget_show(no);

	ewl_widget_show(confirm_win);
}

static EWL_CALLBACK_DEFN(mouse_move)
{
	Ewler_Form *form = user_data;
	Ewler_Widget *ewler_w;

	if( form->selected && ecore_list_count(form->selected) ) {
		ewler_w = ecore_list_first_goto(form->selected);

		ewl_callback_call_with_event_data(ewler_w->fg, EWL_CALLBACK_MOUSE_MOVE,
																			ev_data);
	}
}

static EWL_CALLBACK_DEFN(mouse_down)
{
	Ewl_Event_Mouse_Down *ev = ev_data;

	if( active_form->menu ) {
		ewl_widget_destroy(active_form->menu);
		active_form->menu = NULL;
	}

	switch( ev->button ) {
		case 1:
			if( !widget_under_cursor &&
					!(ev->modifiers & (EWL_KEY_MODIFIER_CTRL | EWL_KEY_MODIFIER_SHIFT)) )
				form_deselect_all();
			break;
		case 3:
			if( ecore_list_count(active_form->selected) )
				form_menu(ev->x, ev->y);
			break;
	}

	widget_under_cursor = false;
}

void
form_close( Ewler_Form *form )
{
	Ewler_Widget *w;

	if( !form )
		return;

	if( !form->ever_saved )
		project_file_delete(form->overlay->w->name);

	ewl_widget_destroy(form->window);
	ecore_list_goto(forms, form);
	ecore_list_remove(forms);

	ecore_list_first_goto(form->widgets);
	while( (w = ecore_list_next(form->widgets)) )
		w->form = NULL;

	ecore_list_destroy(form->widgets);
	ecore_list_destroy(form->selected);

	form->widgets = NULL;
	form->selected = NULL;

	FREE(form);

	active_form = ecore_list_first_goto(forms);

	if( active_form ) {
		inspector_update(active_form->selected);
		callbacks_update(active_form->selected);
	} else {
		inspector_update(NULL);
		callbacks_update(NULL);
	}
}

EWL_CALLBACK_DEFN(form_new)
{
	static int counter = 0;
	char name[32], filename[32];
	char window_name[64];
	bool done = false;
	Ewl_Widget *overlay_fg;

	while( !done ) {
		sprintf( name, "untitled_form" );
		if( counter )
			sprintf(&name[strlen(name)], "%d", counter);
		sprintf(filename, "%s.ui", name);

		if( !project_file_get(name) ) {
			project_file_add(name, filename);
			done = true;
		}

		counter++;
	}

	active_form = NEW(Ewler_Form);
	if( !active_form )
		return;

	active_form->ever_saved = false;
	active_form->dirty = false;
	active_form->menu = NULL;

	active_form->window = ewl_window_new();
	overlay_fg = widget_new("Ewl_Overlay");
	active_form->overlay = ewl_widget_data_get(overlay_fg, "EWLER_WIDGET");
	active_form->widgets = ecore_list_new();
	active_form->selected = ecore_list_new();

	ewl_widget_name_set(active_form->overlay->w, name);
	active_form->overlay->selectable = false;
	active_form->overlay->parent = NULL;
	form_deselect(active_form->overlay);
	ewl_widget_appearance_set(active_form->overlay->bg, "background");
	ewl_theme_data_str_set(active_form->overlay->bg,
												 "/background/file", EWLER_EET);
	ewl_theme_data_str_set(active_form->overlay->bg,
												 "/background/group", "background");
	active_form->window->data = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	ewl_widget_data_set(active_form->window, "EWLER_FORM", active_form);
	sprintf(window_name, "%s_window", name);
	ewl_widget_name_set(active_form->window, window_name);
	ewl_window_title_set(EWL_WINDOW(active_form->window), name);
	ewl_object_size_request(EWL_OBJECT(active_form->window), 800, 600);
	ewl_callback_append(active_form->window, EWL_CALLBACK_MOUSE_MOVE,
											activate, NULL);
	ewl_callback_append(active_form->window, EWL_CALLBACK_DELETE_WINDOW,
											form_close_cb, NULL);
	ewl_widget_show(active_form->window);

	ecore_list_append(active_form->widgets, active_form->overlay);
	ewl_object_fill_policy_set(EWL_OBJECT(active_form->overlay->fg),
														 EWL_FLAG_FILL_FILL);
	ewl_object_fill_policy_set(EWL_OBJECT(active_form->overlay->bg),
														 EWL_FLAG_FILL_FILL);
	ewl_object_fill_policy_set(EWL_OBJECT(active_form->overlay->w),
														 EWL_FLAG_FILL_FILL);
	ewl_callback_append(active_form->overlay->w, EWL_CALLBACK_MOUSE_MOVE,
											mouse_move, active_form);
	ewl_callback_append(active_form->overlay->w, EWL_CALLBACK_MOUSE_DOWN,
											mouse_down, active_form);
	ewl_container_child_append(EWL_CONTAINER(active_form->window),
														 active_form->overlay->fg);

	ecore_list_insert(forms, active_form);
}

EWL_CALLBACK_DEFN(form_open)
{
	char *name = user_data;
	char window_name[64];
	
	if( (active_form = form_get(name)) ) {
		ewl_window_raise(EWL_WINDOW(active_form->window));
	} else {
		active_form = NEW(Ewler_Form);
		if( !active_form )
			return;

		active_form->ever_saved = true;
		active_form->dirty = false;
		active_form->menu = NULL;

		active_form->filename = project_file_get(name);

		active_form->window = ewl_window_new();
		active_form->widgets = ecore_list_new();
		active_form->selected = ecore_list_new();

		if( file_form_open(active_form) < 0 ) {
			ewler_error("Unable to open form: %s", active_form->filename);
			ewl_widget_destroy(active_form->window);
			ecore_list_destroy(active_form->widgets);
			ecore_list_destroy(active_form->selected);
			FREE(active_form);
			return;
		}

		active_form->overlay->selectable = false;
		active_form->overlay->parent = NULL;
		form_deselect(active_form->overlay);
		ewl_widget_appearance_set(active_form->overlay->bg, "background");
		ewl_theme_data_str_set(active_form->overlay->bg,
													 "/background/file", EWLER_EET);
		ewl_theme_data_str_set(active_form->overlay->bg,
													 "/background/group", "background");
		active_form->window->data = ecore_hash_new(ecore_str_hash,
																							 ecore_str_compare);
		ewl_widget_data_set(active_form->window, "EWLER_FORM", active_form);
		sprintf(window_name, "%s_window", name);
		ewl_widget_name_set(active_form->window, window_name);
		ewl_window_title_set(EWL_WINDOW(active_form->window), name);
		ewl_object_size_request(EWL_OBJECT(active_form->window), 800, 600);
		ewl_callback_append(active_form->window, EWL_CALLBACK_FOCUS_IN,
												activate, NULL);
		ewl_widget_show(active_form->window);

		ewl_object_fill_policy_set(EWL_OBJECT(active_form->overlay->fg),
															 EWL_FLAG_FILL_FILL);
		ewl_object_fill_policy_set(EWL_OBJECT(active_form->overlay->bg),
															 EWL_FLAG_FILL_FILL);
		ewl_object_fill_policy_set(EWL_OBJECT(active_form->overlay->w),
															 EWL_FLAG_FILL_FILL);
		ewl_callback_append(active_form->overlay->w, EWL_CALLBACK_MOUSE_MOVE,
												mouse_move, active_form);
		ewl_callback_append(active_form->overlay->w, EWL_CALLBACK_MOUSE_DOWN,
												mouse_down, active_form);
		ewl_container_child_append(EWL_CONTAINER(active_form->window),
															 active_form->overlay->fg);

		ecore_list_insert(forms, active_form);
	}
}

static void
find_minimum( Ecore_List *widgets, int *xp, int *yp )
{
	Ewler_Widget *w;
	unsigned int x, y;

	x = (unsigned int) -1;
	y = (unsigned int) -1;

	if( ecore_list_count(widgets) == 0 ) {
		x = 0;
		y = 0;
	} else {
		ecore_list_first_goto(widgets);
		while( (w = ecore_list_next(widgets)) ) {
			int nx, ny;

			nx = ewl_object_current_x_get(EWL_OBJECT(w->w));
			ny = ewl_object_current_y_get(EWL_OBJECT(w->w));

			if( nx < x )
				x = nx;
			if( ny < y )
				y = ny;
		}
	}

	*xp = x;
	*yp = y;
}

static EWL_CALLBACK_DEFN(form_menu_cb)
{
	Ewler_Form *form = user_data;
	Ewler_Widget *ewler_w, *box;
	Ewl_Widget *box_w;
	Ewl_Widget *parent;
	int action, x, y;

	action = (int) ewl_widget_data_get(w, "Action");

	switch( action ) {
		case EWLER_WIDGET_BREAK:
			ecore_list_first_goto(form->selected);
			while( (ewler_w = ecore_list_next(form->selected)) ) {
				ewl_container_child_append(EWL_CONTAINER(form->overlay->w),
																	 ewler_w->fg);
				IF_FREE(ewler_w->parent);
				ewler_w->parent = (char *) ewl_widget_name_get(form->overlay->w);
			}
			break;
		case EWLER_WIDGET_ALIGN_V:
			box_w = widget_new("Ewl_Box");
			box = ewl_widget_data_get(box_w, "EWLER_WIDGET");
			ewl_box_orientation_set(EWL_BOX(box->w), EWL_ORIENTATION_VERTICAL);
			find_minimum(form->selected, &x, &y);
			ewl_object_position_request(EWL_OBJECT(box->w), x, y);

			ewler_w = ecore_list_first_goto(form->selected);
			parent = ewl_widget_name_find(ewler_w->parent);
			ewl_container_child_append(EWL_CONTAINER(parent), box_w);

			while( (ewler_w = ecore_list_next(form->selected)) ) {
				ewl_container_child_append(EWL_CONTAINER(box->w), ewler_w->fg);
				IF_FREE(ewler_w->parent);
				ewler_w->parent = (char *) ewl_widget_name_get(box->w);
				ewler_w->grouped = true;
			}

			active_form = form;

			form_deselect_all();
			break;
		case EWLER_WIDGET_ALIGN_H:
			box_w = widget_new("Ewl_Box");
			box = ewl_widget_data_get(box_w, "EWLER_WIDGET");
			ewl_box_orientation_set(EWL_BOX(box->w), EWL_ORIENTATION_HORIZONTAL);
			find_minimum(form->selected, &x, &y);
			ewl_object_position_request(EWL_OBJECT(box->w), x, y);

			ewler_w = ecore_list_first_goto(form->selected);
			parent = ewl_widget_name_find(ewler_w->parent);
			ewl_container_child_append(EWL_CONTAINER(parent), box_w);

			while( (ewler_w = ecore_list_next(form->selected)) ) {
				ewl_container_child_append(EWL_CONTAINER(box->w), ewler_w->fg);
				IF_FREE(ewler_w->parent);
				ewler_w->parent = (char *) ewl_widget_name_get(box->w);
				ewler_w->grouped = true;
			}

			active_form = form;

			form_deselect_all();
			break;
		case EWLER_WIDGET_DELETE:
			while( (ewler_w = ecore_list_first_goto(form->selected)) )
				form_destroy(form, ewler_w);

			if( active_form && active_form->selected ) {
				inspector_update(active_form->selected);
				callbacks_update(active_form->selected);
			}
			break;
	}

	ewl_widget_destroy(form->menu);
	form->menu = NULL;
}

static EWL_CALLBACK_DEFN(form_menu_configure)
{
	ewl_callback_del(w, EWL_CALLBACK_CONFIGURE, form_menu_configure);
	ewl_widget_hide(w);

	ewl_widget_layer_set(EWL_IMENU(w)->base.popup, 1000);
}

static bool
has_widget_in_group( Ecore_List *widgets )
{
	Ewler_Widget *w;

	ecore_list_first_goto(widgets);
	while( (w = ecore_list_next(widgets)) )
		if( w->grouped )
			return true;

	return false;
}

static bool
at_same_level( Ecore_List *widgets )
{
	Ewler_Widget *w;
	char *parent;

	w = ecore_list_first_goto(widgets);
	parent = w->parent;
	while( (w = ecore_list_next(widgets)) )
		if( (w->parent && strcmp(w->parent, parent)) ||
				(!w->parent && w->parent != parent) )
			return false;

	return true;
}

void
form_menu( int x, int y )
{
	if( active_form->menu ) {
		ewl_widget_destroy(active_form->menu);
		active_form->menu = NULL;
	} else {
		Ewl_Widget *item;

		active_form->menu = ewl_imenu_new(NULL, "Form Properties");
		ewl_object_position_request(EWL_OBJECT(active_form->menu), x, y);
		ewl_object_fill_policy_set(EWL_OBJECT(active_form->menu),
															 EWL_FLAG_FILL_NONE);
		ewl_container_child_append(EWL_CONTAINER(active_form->window),
															 active_form->menu);
		ewl_callback_append(active_form->menu, EWL_CALLBACK_CONFIGURE,
												form_menu_configure, NULL);
		ewl_callback_call(active_form->menu, EWL_CALLBACK_SELECT);
		ewl_widget_layer_set(EWL_IMENU(active_form->menu)->base.popup, 1000);
		ewl_widget_show(active_form->menu);

		if( ecore_list_count(active_form->selected) &&
				has_widget_in_group(active_form->selected) ) {
			item = ewl_menu_item_new(NULL, "Break Alignment");
			/* HACK FOR NON-STRING DATA HASHES */
			item->data = ecore_hash_new(ecore_str_hash, ecore_str_compare);
			ewl_widget_data_set(item, "Action", (void *) EWLER_WIDGET_BREAK);
			ewl_callback_append(item, EWL_CALLBACK_SELECT, form_menu_cb, active_form);
			ewl_container_child_append(EWL_CONTAINER(active_form->menu), item);
			ewl_widget_show(item);
		} else if( ecore_list_count(active_form->selected) > 1 &&
							 at_same_level(active_form->selected) ) {
			item = ewl_menu_item_new(NULL, "Align Horizontally");
			/* HACK FOR NON-STRING DATA HASHES */
			item->data = ecore_hash_new(ecore_str_hash, ecore_str_compare);
			ewl_widget_data_set(item, "Action", (void *) EWLER_WIDGET_ALIGN_H);
			ewl_callback_append(item, EWL_CALLBACK_SELECT, form_menu_cb, active_form);
			ewl_container_child_append(EWL_CONTAINER(active_form->menu), item);
			ewl_widget_show(item);

			item = ewl_menu_item_new(NULL, "Align Vertically");
			/* HACK FOR NON-STRING DATA HASHES */
			item->data = ecore_hash_new(ecore_str_hash, ecore_str_compare);
			ewl_widget_data_set(item, "Action", (void *) EWLER_WIDGET_ALIGN_V);
			ewl_callback_append(item, EWL_CALLBACK_SELECT, form_menu_cb, active_form);
			ewl_container_child_append(EWL_CONTAINER(active_form->menu), item);
			ewl_widget_show(item);
		}
		
		if( ecore_list_count(active_form->selected) ) {
			item = ewl_menu_item_new(NULL, "Delete");
			/* HACK FOR NON-STRING DATA HASHES */
			item->data = ecore_hash_new(ecore_str_hash, ecore_str_compare);
			ewl_widget_data_set(item, "Action", (void *) EWLER_WIDGET_DELETE);
			ewl_callback_append(item, EWL_CALLBACK_SELECT, form_menu_cb, active_form);
			ewl_container_child_append(EWL_CONTAINER(active_form->menu), item);
			ewl_widget_show(item);
		}
	}
}

Ewler_Form *
form_get( char *name )
{
	Ewl_Widget *form_window;
	Ewler_Form *form = NULL;
	char window_name[64];

	sprintf(window_name, "%s_window", name);

	if( (form_window = ewl_widget_name_find(window_name)) )
		form = ewl_widget_data_get(form_window, "EWLER_FORM");

	return form;
}

void
form_name_set( Ewler_Form *form, char *name )
{
	char window_name[64];

	if( !form )
		return;

	sprintf(window_name, "%s_window", name);

	ewl_widget_name_set(form->window, window_name);
	ewl_widget_name_set(form->overlay->w, name);
	ewl_window_title_set(EWL_WINDOW(form->window), name);
}

Ewl_Widget *
form_active( void )
{
	if( !active_form )
		return NULL;

	return active_form->window;
}

void
form_deselect_all( void )
{
	Ewler_Widget *w;

	if( !active_form )
		return;

	ecore_list_first_goto(active_form->selected);
	while( (w = ecore_list_remove(active_form->selected)) )
		widget_deselect(w);

	ecore_list_first_goto(active_form->widgets);
	while( (w = ecore_list_next(active_form->widgets)) )
		widget_deselect(w);

	inspector_update(active_form->selected);
	callbacks_update(active_form->selected);
}

void
form_select( Ewler_Widget *w )
{
	if( !active_form || !w )
		return;

	ecore_list_prepend(active_form->selected, w);
	widget_select(w);

	inspector_update(active_form->selected);
	callbacks_update(active_form->selected);
}

void
form_deselect( Ewler_Widget *w )
{
	if( !active_form || !w )
		return;

	if( ecore_list_goto(active_form->selected, w) == w )
		ecore_list_remove(active_form->selected);
	widget_deselect(w);

	inspector_update(active_form->selected);
	callbacks_update(active_form->selected);
}

void
form_toggle( Ewler_Widget *w )
{
	Ewler_Widget *old_w;

	if( !active_form || !w )
		return;

	widget_toggle(w);

	if( w->selected ) {
		ecore_list_first_goto(active_form->selected);
		while( (old_w = ecore_list_next(active_form->selected)) )
			widget_clear_ui_hooks(old_w);

		ecore_list_prepend(active_form->selected, w);
	} else if( ecore_list_goto(active_form->selected, w) == w ) {
		ecore_list_remove(active_form->selected);
	}

	inspector_update(active_form->selected);
	callbacks_update(active_form->selected);
}

Ecore_List *
form_selected( void )
{
	if( !active_form )
		return NULL;

	return active_form->selected;
}

void
form_dirty_set( Ewler_Form *form )
{
	form->dirty = true;
}

/* WILL NOT RENAME TOP LEVEL WIDGET PARENTS */
void
form_rename_widget( char *old, char *name )
{
	Ewler_Widget *w;

	if( !active_form || !old || !name )
		return;

	ecore_list_first_goto(active_form->widgets);
	while( (w = ecore_list_next(active_form->widgets)) )
		if( !strcmp(w->parent, old) ) {
			FREE(w->parent);
			w->parent = strdup(name);
		}
}

void
form_remove( Ewler_Form *form, Ewler_Widget *w )
{
	if( form->selected )
		while( ecore_list_goto(form->selected, w) )
			ecore_list_remove(form->selected);

	if( form->widgets )
		while( ecore_list_goto(form->widgets, w) )
			ecore_list_remove(form->widgets);
}

void
form_destroy( Ewler_Form *form, Ewler_Widget *w )
{
	form_remove(form, w);
	ewl_widget_destroy(w->fg);
}

void
form_add( Ewl_Widget *w )
{
	Ewler_Widget *ewler_w;
	Ewl_Widget *container;

	if( !active_form )
		return;

	ewler_w = ewl_widget_data_get(w, "EWLER_WIDGET");
	if( !ewler_w )
		return;

	ewler_w->form = active_form;

	container = active_form->overlay->w;
	if( ecore_list_count(active_form->selected) == 1 ) {
		Ewler_Widget *sel;

		sel = ecore_list_first_goto(active_form->selected);
		if( sel->spec->group ) {
			ewler_w->grouped = true;
			container = sel->w;
		}
	}

	ewl_container_child_append(EWL_CONTAINER(container), w);
	ewler_w->parent = (char *) ewl_widget_name_get(container);

	form_deselect_all();

	ecore_list_append(active_form->widgets, ewler_w);
	form_select(ewler_w);

	form_dirty_set(active_form);

	inspector_update(active_form->selected);
	callbacks_update(active_form->selected);
}

int
forms_init( void )
{
	forms = ecore_list_new();

	return 0;
}
