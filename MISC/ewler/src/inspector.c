#include <Ewl.h>

#include "ewler.h"

static Ewl_Widget *inspector_win;
static Ewl_Widget *tree;

static Ewler_Widget_Elem_Spec *name_spec;

EWL_CALLBACK_DEFN(inspector_toggle)
{
	if( VISIBLE(inspector_win) )
		ewl_widget_hide(inspector_win);
	else
		ewl_widget_show(inspector_win);
}

int
inspector_init( void )
{
	char *header[] = {"Attribute", "Value"};

	if( inspector_win )
		return -1;

	name_spec = spec_elem_get("Ewl_Widget", "name");

	inspector_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(inspector_win), "Inspector");
	ewl_object_size_request(EWL_OBJECT(inspector_win), 200, 320);
	ewl_callback_append(inspector_win, EWL_CALLBACK_DELETE_WINDOW,
											inspector_toggle, NULL);

	tree = ewl_tree_new(2);
	ewl_tree_headers_set(EWL_TREE(tree), header);
	ewl_tree_mode_set(EWL_TREE(tree), EWL_TREE_MODE_NONE);
	ewl_container_child_append(EWL_CONTAINER(inspector_win), tree);
	ewl_widget_show(tree);

	ewl_widget_show(inspector_win);

	return 0;
}

/**
 * non-allocated string
 */
char *
elem_to_s( Ewler_Widget_Elem *elem )
{
	static char buf[256];

	switch( elem->spec->type ) {
		case EWLER_SPEC_ELEM_INT:
			/* TODO: allow for format conventions (0x, 0, blah) */
			sprintf( buf, "%d", elem->info.ivalue );
			break;
		case EWLER_SPEC_ELEM_STRING:
			sprintf( buf, "%s", elem->info.svalue );
			break;
		case EWLER_SPEC_ELEM_ENUM:
			sprintf( buf, "%s", (char *) ecore_hash_get(elem->spec->info.edata.map,
																									(void *) elem->info.evalue) );
			break;
		default:
			buf[0] = '\0';
			break;
	}

	return buf;
}

static EWL_CALLBACK_DEFN(combo_value_changed)
{
	Ewler_Widget_Elem *elem = user_data;
	Ewl_Widget *selected;
	char *text;

	selected = ewl_combo_selected_get(EWL_COMBO(w));
	text = ewl_menu_item_text_get(EWL_MENU_ITEM(selected));

	elem->info.evalue = (int) ecore_hash_get(elem->spec->info.edata.map_rev,
																					 text);
	IF_FREE(text);

	if( elem->spec->set_func )
		elem->spec->set_func(elem->w->w, elem->info.evalue);
}

static EWL_CALLBACK_DEFN(entry_value_changed)
{
	Ewler_Widget_Elem *elem = user_data;
	char *text;

	text = ewl_text_text_get(EWL_TEXT(elem->entry));

	if( !text )
		return;

	switch( elem->spec->type ) {
		case EWLER_SPEC_ELEM_INT:
			elem->info.ivalue = strtol(text, NULL, 0);
			IF_FREE(text);

			if( !elem->changed ) {
				Ewl_Widget *parent;
				char *label = ewl_text_text_get(EWL_TEXT(elem->text));

				parent = elem->text->parent;
				ewl_widget_destroy(elem->text);

				elem->text = ewl_text_new("");
				ewl_text_color_set(EWL_TEXT(elem->text), 255, 0, 0, 255);
				ewl_text_text_append(EWL_TEXT(elem->text), label);
				ewl_container_child_append(EWL_CONTAINER(parent), elem->text);
				ewl_widget_show(elem->text);
				FREE(label);
			}

			elem->changed = true;
			if( elem->spec->set_func )
				elem->spec->set_func(elem->w->w, elem->info.ivalue);
			break;
		case EWLER_SPEC_ELEM_STRING:
			if( elem->spec == name_spec ) {
				if( ewl_widget_name_find(text) ) {
					ewler_error("A widget already exists with the name %s", text);
					FREE(text);
					ewl_text_text_set(EWL_TEXT(elem->entry), elem->info.svalue);
					return;
				}

				form_rename_widget(elem->info.svalue, text);
			}

			elem->info.svalue = text;

			if( !elem->changed ) {
				int len;

				len = ewl_text_length_get(EWL_TEXT(elem->text));
				ewl_text_cursor_position_set(EWL_TEXT(elem->text), 0);
				ewl_text_color_apply(EWL_TEXT(elem->text), 255, 0, 0, 255, len);
			}

			elem->changed = true;
			if( elem->spec->set_func )
				elem->spec->set_func(elem->w->w, elem->info.svalue);
			break;
		default: break;
	}
}

extern Ewl_Widget *last_selected;
extern Ewl_Widget *last_key;

static EWL_CALLBACK_DEFN(entry_focus_out)
{
	if( VISIBLE(EWL_ENTRY(w)->cursor) ) {
		last_selected = NULL;
		last_key = NULL;
		ewl_object_state_remove(EWL_OBJECT(w), EWL_FLAG_STATE_SELECTED);
		ewl_callback_call(w, EWL_CALLBACK_DESELECT);
	}
}

void
inspector_update( Ecore_List *selected )
{
	Ewl_Widget *prow;
	Ecore_Sheap *slist, *sheap;
	Ecore_Hash *elems;
	Ecore_List *keys;
	Ecore_List *prow_stack, *sheap_stack, *elems_stack;
	Ewler_Widget *w;
	char *key;

	ewl_container_reset(EWL_CONTAINER(tree));

	if( !selected )
		return;
	/* add support for multiple selections later */
	w = ecore_list_first_goto(selected);

	if( !w )
		return;
	
	keys = ecore_hash_keys(w->elems);
	slist = ecore_sheap_new(ecore_str_compare, ecore_list_count(keys));

	while( (key = ecore_list_next(keys)) )
		ecore_sheap_insert(slist, key);

	ecore_list_destroy(keys);
	ecore_sheap_sort(slist);

	sheap = slist;
	elems = w->elems;

	sheap_stack = ecore_list_new();
	elems_stack = ecore_list_new();
	prow_stack = ecore_list_new();
	prow = NULL;

	while( (key = ecore_sheap_extract(sheap)) ) {
		Ewler_Widget_Elem *elem;
		Ewl_Widget *row_items[2], *row;
		Ewl_Widget *text, *entry;
		int len;

		elem = ecore_hash_get(elems, key);
		text = ewl_text_new(key);
		len = ewl_text_length_get(EWL_TEXT(text));
		ewl_text_cursor_position_set(EWL_TEXT(text), 0);
		if( elem->changed )
			ewl_text_color_apply(EWL_TEXT(text), 255, 0, 0, 255, len);

		switch( elem->spec->type ) {
			case EWLER_SPEC_ELEM_STRUCT: entry = NULL; break;
			case EWLER_SPEC_ELEM_ENUM:
				entry = ewl_combo_new(elem_to_s(elem));

				if( elem->items )
					ecore_hash_destroy(elem->items);

				elem->items = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
				
				keys = ecore_hash_keys(elem->spec->info.edata.map_rev);
				while( (key = ecore_list_next(keys)) ) {
					Ewl_Widget *combo_item;
					int value;

					value = (int) ecore_hash_get(elem->spec->info.edata.map_rev, key);

					combo_item = ewl_menu_item_new(NULL, key);
					ecore_hash_set(elem->items, (void *) value, combo_item);
					ewl_container_child_append(EWL_CONTAINER(entry), combo_item);
					ewl_widget_show(combo_item);
				}

				ecore_list_destroy(keys);

				ewl_callback_append(entry, EWL_CALLBACK_VALUE_CHANGED,
														combo_value_changed, elem);
				break;
			default:
				entry = ewl_entry_new(elem_to_s(elem));
				if( !elem->spec->modifiable )
					ewl_entry_editable_set(EWL_ENTRY(entry), false);

				ewl_callback_append(entry, EWL_CALLBACK_VALUE_CHANGED,
														entry_value_changed, elem);
				ewl_callback_append(entry, EWL_CALLBACK_DESELECT,
														entry_value_changed, elem);
				ewl_callback_append(entry, EWL_CALLBACK_FOCUS_OUT,
														entry_focus_out, elem);
				break;
		}

		elem->text = text;
		elem->entry = entry;

		row_items[0] = text;
		row_items[1] = entry;

		row = ewl_tree_row_add(EWL_TREE(tree), EWL_ROW(prow), row_items);
		ewl_widget_show(text);
		if( entry )
			ewl_widget_show(entry);

		if( elem->spec->type == EWLER_SPEC_ELEM_STRUCT ) {
			ecore_list_prepend(prow_stack, prow);
			ecore_list_prepend(sheap_stack, sheap);
			ecore_list_prepend(elems_stack, elems);

			prow = row;
			elems = elem->info.children;

			/* TODO: check for indexed struct */
			keys = ecore_hash_keys(elems);
			sheap = ecore_sheap_new(ecore_str_compare, ecore_list_count(keys));

			while( (key = ecore_list_next(keys)) )
				ecore_sheap_insert(sheap, key);

			ecore_list_destroy(keys);
			ecore_sheap_sort(sheap);
		}

		if( sheap->size == 0 && ecore_list_count(sheap_stack) ) {
			ecore_sheap_destroy(sheap);

			prow = ecore_list_first_remove(prow_stack);
			sheap = ecore_list_first_remove(sheap_stack);
			elems = ecore_list_first_remove(elems_stack);
		}
	}

	ecore_list_destroy(sheap_stack);
	ecore_list_destroy(elems_stack);
	ecore_sheap_destroy(slist);
}
