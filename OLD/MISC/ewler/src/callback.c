/**
 * callback manager for ewler
 */
#include <Ewl.h>

#include "ewler.h"

static Ewl_Widget *callbacks_win, *callback_tree, *callback_combo;
static Ecore_Hash *item_map;
static Ewl_Widget *confirm_win, *add_win;

static Ewler_Widget *selected_widget;

EWL_CALLBACK_DEFN(callbacks_toggle)
{
	if( VISIBLE(callbacks_win) )
		ewl_widget_hide(callbacks_win);
	else
		ewl_widget_show(callbacks_win);
}

static EWL_CALLBACK_DEFN(callback_add_cb)
{
	int *response = ev_data;
	Ewl_Widget *entry = user_data;
	Ewl_Widget *selected_cb;
	char *handler;
	int callback;

	if( *response == EWL_RESPONSE_OK ) {
		selected_cb = ewl_combo_selected_get(EWL_COMBO(callback_combo));

		if( selected_cb )
			callback = (int) ecore_hash_get(item_map, selected_cb);
		else
			return;

		handler = ewl_text_text_get(EWL_TEXT(entry));
		if( !selected_widget->callbacks[callback] ) {
			selected_widget->callbacks[callback] = ecore_list_new();
			ecore_list_free_cb_set(selected_widget->callbacks[callback], free);
		}

		ecore_list_append(selected_widget->callbacks[callback], handler);

		callbacks_update(form_selected());
	}

	ewl_widget_destroy(add_win);
	add_win = NULL;
}

static EWL_CALLBACK_DEFN(callback_add)
{
	Ewl_Widget *text, *entry, *button, *hbox;

	if( add_win )
		return;

	add_win = ewl_dialog_new(EWL_POSITION_BOTTOM);
	if( !add_win )
		return;

	hbox = ewl_hbox_new();
	ewl_dialog_widget_add(EWL_DIALOG(add_win), hbox);

	text = ewl_text_new("Callback Handler:");
	ewl_container_child_append(EWL_CONTAINER(hbox), text);
	ewl_widget_show(text);

	entry = ewl_entry_new("");
	ewl_container_child_append(EWL_CONTAINER(hbox), entry);
	ewl_widget_show(entry);

	ewl_widget_show(hbox);

	button = ewl_dialog_button_add(EWL_DIALOG(add_win),
																 EWL_STOCK_OK, EWL_RESPONSE_OK);
	ewl_callback_append(button, EWL_CALLBACK_VALUE_CHANGED,
											callback_add_cb, entry);
	ewl_widget_show(button);

	button = ewl_dialog_button_add(EWL_DIALOG(add_win),
																 EWL_STOCK_CANCEL, EWL_RESPONSE_CANCEL);
	ewl_callback_append(button, EWL_CALLBACK_VALUE_CHANGED,
											callback_add_cb, entry);
	ewl_widget_show(button);

	ewl_widget_show(add_win);
}

/**
 * this is really awkward... needs to look at the currently selected cb from
 * the combo, but there's a lot of control values that need to be passed...
 * (ewler widget, callback, handler name)
 * maybe look at fixing this.
 */
static EWL_CALLBACK_DEFN(callback_remove_cb)
{
	int *response = ev_data;
	Ewl_Widget *name_text = user_data;
	Ewl_Widget *selected_cb;
	int callback;
	char *name, *handler;

	if( !selected_widget )
		return;

	if( *response == EWL_RESPONSE_OK ) {
		selected_cb = ewl_combo_selected_get(EWL_COMBO(callback_combo));

		if( selected_cb )
			callback = (int) ecore_hash_get(item_map, selected_cb);
		else
			return;

		name = ewl_text_text_get(EWL_TEXT(name_text));

		ecore_list_first_goto(selected_widget->callbacks[callback]);

		while( (handler = ecore_list_next(selected_widget->callbacks[callback])) )
			if( !strcmp(handler, name) )
				ecore_list_remove(selected_widget->callbacks[callback]);

		FREE(name);

		callbacks_update(form_selected());
	}

	ewl_widget_destroy(confirm_win);
	confirm_win = NULL;
}

static EWL_CALLBACK_DEFN(callback_remove)
{
	Ecore_List *selected;
	Ewl_Widget *row, *text, *button, *name_text;
	char buf[256];
	char *name;

	if( confirm_win )
		return;

	confirm_win = ewl_dialog_new(EWL_POSITION_BOTTOM);
	if( !confirm_win )
		return;

	selected = ewl_tree_selected_get(EWL_TREE(callback_tree));
	if( ecore_list_count(selected) == 1 ) {
		ecore_list_first_goto(selected);

		while( (row = ecore_list_next(selected)) ) {
			name_text = ewl_tree_row_column_get(EWL_ROW(row), 0);
			name = ewl_text_text_get(EWL_TEXT(name_text));
			
			sprintf(buf, "Are you sure you want to delete the callback %s?", name);
			FREE(name);

			text = ewl_text_new(buf);
			ewl_dialog_widget_add(EWL_DIALOG(confirm_win), text);
			ewl_widget_show(text);

			button = ewl_dialog_button_add(EWL_DIALOG(confirm_win),
																		 EWL_STOCK_OK, EWL_RESPONSE_OK);
			ewl_callback_append(button, EWL_CALLBACK_VALUE_CHANGED,
													callback_remove_cb, name_text);
			ewl_widget_show(button);

			button = ewl_dialog_button_add(EWL_DIALOG(confirm_win),
																		 EWL_STOCK_CANCEL, EWL_RESPONSE_CANCEL);
			ewl_callback_append(button, EWL_CALLBACK_VALUE_CHANGED,
													callback_remove_cb, name_text);
			ewl_widget_show(button);

			ewl_widget_show(confirm_win);
		}
	}
}

static EWL_CALLBACK_DEFN(update_cb)
{
	callbacks_update(form_selected());
}

int
callbacks_init( void )
{
	Ewl_Widget *vbox, *hbox, *button, *label, *item;
	char *header[] = {"Handler", NULL};

	callbacks_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(callbacks_win), "Callbacks");
	ewl_object_size_request(EWL_OBJECT(callbacks_win), 320, 200);
	ewl_callback_append(callbacks_win, EWL_CALLBACK_DELETE_WINDOW,
											callbacks_toggle, NULL);

	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(callbacks_win), vbox);

	hbox = ewl_hbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(hbox),
														 EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);

	label = ewl_text_new("Callback Type");
	ewl_container_child_append(EWL_CONTAINER(hbox), label);
	ewl_widget_show(label);

	item_map = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

	callback_combo = ewl_combo_new("Select a Callback");
	ewl_callback_append(callback_combo, EWL_CALLBACK_VALUE_CHANGED,
											update_cb, NULL);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_EXPOSE");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_EXPOSE);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_REALIZE");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_REALIZE);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_UNREALIZE");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_UNREALIZE);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_SHOW");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_SHOW);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_HIDE");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_HIDE);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_DESTROY");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_DESTROY);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_DELETE_WINDOW");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_DELETE_WINDOW);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_CONFIGURE");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_CONFIGURE);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_REPARENT");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_REPARENT);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_KEY_DOWN");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_KEY_DOWN);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_KEY_UP");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_KEY_UP);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_MOUSE_DOWN");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_MOUSE_DOWN);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_MOUSE_UP");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_MOUSE_UP);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_MOUSE_MOVE");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_MOUSE_MOVE);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_MOUSE_WHEEL");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_MOUSE_WHEEL);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_FOCUS_IN");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_FOCUS_IN);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_FOCUS_OUT");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_FOCUS_OUT);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_SELECT");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_SELECT);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_DESELECT");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_DESELECT);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_CLICKED");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_CLICKED);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_DOUBLE_CLICKED");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_DOUBLE_CLICKED);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_HILITED");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_HILITED);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_VALUE_CHANGED");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_VALUE_CHANGED);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_STATE_CHANGED");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_STATE_CHANGED);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_APPEARANCE_CHANGED");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_APPEARANCE_CHANGED);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_WIDGET_ENABLE");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_WIDGET_ENABLE);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_WIDGET_DISABLE");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_WIDGET_DISABLE);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);
	item = ewl_menu_item_new(NULL, "EWL_CALLBACK_PASTE");
	ecore_hash_set(item_map, item, (void *) EWL_CALLBACK_PASTE);
	ewl_container_child_append(EWL_CONTAINER(callback_combo), item);
	ewl_widget_show(item);

	ewl_container_child_append(EWL_CONTAINER(hbox), callback_combo);
	ewl_widget_show(callback_combo);

	ewl_widget_show(hbox);

	callback_tree = ewl_tree_new(1);
	ewl_tree_headers_set(EWL_TREE(callback_tree), header);
	ewl_object_fill_policy_set(EWL_OBJECT(callback_tree), EWL_FLAG_FILL_FILL);
	ewl_container_child_append(EWL_CONTAINER(vbox), callback_tree);
	ewl_widget_show(callback_tree);

	hbox = ewl_hbox_new();
	ewl_object_fill_policy_set(EWL_OBJECT(hbox),
														 EWL_FLAG_FILL_VSHRINK | EWL_FLAG_FILL_HFILL);
	ewl_container_child_append(EWL_CONTAINER(vbox), hbox);

	button = ewl_button_new("Add");
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, callback_add, NULL);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_widget_show(button);

	button = ewl_button_new("Remove");
	ewl_callback_append(button, EWL_CALLBACK_CLICKED, callback_remove, NULL);
	ewl_container_child_append(EWL_CONTAINER(hbox), button);
	ewl_widget_show(button);

	ewl_widget_show(hbox);

	ewl_widget_show(vbox);

	ewl_widget_show(callbacks_win);

	return 0;
}

void
callbacks_update( Ecore_List *selected )
{
	Ewler_Widget *w;
	Ewl_Widget *selected_cb;
	Ecore_Sheap *shandlers;
	char *handler;
	int callback, count;

	selected_cb = ewl_combo_selected_get(EWL_COMBO(callback_combo));

	if( selected_cb )
		callback = (int) ecore_hash_get(item_map, selected_cb);
	else
		callback = -1;

	ewl_container_reset(EWL_CONTAINER(callback_tree));

	if( !selected || callback < 0 ) {
		selected_widget = NULL;
		return;
	}

	w = ecore_list_first_goto(selected);

	selected_widget = w;

	if( !w || !w->callbacks[callback] )
		return;

	count = ecore_list_count(w->callbacks[callback]);
	ecore_list_first_goto(w->callbacks[callback]);
	shandlers = ecore_sheap_new(ecore_str_compare, count);

	while( (handler = ecore_list_next(w->callbacks[callback])) )
		ecore_sheap_insert(shandlers, handler);

	ecore_sheap_sort(shandlers);

	while( (handler = ecore_sheap_extract(shandlers)) ) {
		Ewl_Widget *text;
		
		text = ewl_text_new(handler);
		ewl_tree_row_add(EWL_TREE(callback_tree), NULL, &text);
		ewl_widget_show(text);
	}

	ecore_sheap_destroy(shandlers);
}
