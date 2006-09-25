#include <limits.h>
#include <Evas.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Entrance_Edit.h>
#include <Entrance_Widgets.h>
#include "Egui.h"

static void _cb_close(void*, void*);
static void _cb_apply(void*, void*);
static void _cb_ok(void *, void *);

static void _cb_group_build(void*, void*);
static void _cb_item_build(void*, void*);

static void _close(void);
static int _apply(void);
static void _save_to_file(void);

static void _load_button(void *w, const char *key, int ktype);
static void _load_entry(void *w, const char *key, int ktype);
static void _load_checkbox(void *w, const char *key, int ktype);

static void _save_button(void *w, const char *key, int ktype);
static void _save_entry(void *w, const char *key, int ktype);
static void _save_checkbox(void *w, const char *key, int ktype);
static void _save_list(void *w, const char *key, int ktype);

static Entrance_Widget _item_build(Egui_Settings_Item);

static Entrance_Dialog win;
static Ecore_List *items;


Egui_Settings_Group es_group_new(const char* name, int direction)
{
	Egui_Settings_Group esg = calloc(1, sizeof(*esg));
	if(esg)
	{
		esg->title = name;
		esg->direction = direction;
		esg->items = ecore_list_new();
	}

	return esg;
}

void
es_group_append(Egui_Settings_Group g, Egui_Settings_Item *i)
{
	ecore_list_append(g->items, i);
}

void
es_dialog_show(char *title, Ecore_List *groups) 
{
	win = ew_dialog_new(title, EW_FALSE);
	items = ecore_list_new();

	ecore_list_for_each(groups, _cb_group_build, NULL);
	
	ew_dialog_close_button_add(win, _cb_close, NULL);
	ew_dialog_apply_button_add(win, _cb_apply, NULL);
	ew_dialog_ok_button_add(win, _cb_ok, NULL);

	ew_dialog_show(win);
}

/*private*/

Entrance_Widget
_item_build(Egui_Settings_Item item) 
{
	switch(item.widget_type) 
	{
		/*TODO: move widget builders from case body into _widget_build_button, _widget_build_checkbox, etc*/
		case EGUI_TYPE_BUTTON:
			_load_button(item.widget, item.entrance_edit_key, item.key_type);
			break;
		case EGUI_TYPE_ENTRY:
			_load_entry(item.widget, item.entrance_edit_key, item.key_type);
			break;
		case EGUI_TYPE_CHECKBOX:
			_load_checkbox(item.widget, item.entrance_edit_key, item.key_type);
			break;
		default: break;
	}

	return item.widget;
}

static void
_load_button(void *w, const char *key, int ktype)
{
	if(ktype == EGUI_TYPE_INT)
	 ew_toggle_button_active_set(w, entrance_edit_int_get(key));
}

static void
_load_entry(void *w, const char *key, int ktype)
{
	if(ktype == EGUI_TYPE_STR)
		ew_entry_set(w, entrance_edit_string_get(key));
	else if(ktype == EGUI_TYPE_INT)
	{
		char msg[PATH_MAX];
		snprintf(msg, PATH_MAX, "%d", entrance_edit_int_get(key));
		ew_entry_set(w, msg);
	}
}

static void
_load_checkbox(void *w, const char *key, int ktype)
{
	if(ktype == EGUI_TYPE_INT)
	{
		if(entrance_edit_int_get(key))
			ew_checkbox_toggle(w);
	}
}

static void
_save_button(void *w, const char *key, int ktype)
{
	if(ktype == EGUI_TYPE_INT)
		entrance_edit_int_set(key, ew_toggle_button_active_get(w));
}

static void
_save_entry(void *w, const char *key, int ktype)
{
/*	printf("value of entry = %s, key = %s\n", ew_entry_get(w), key);*/
	if(ktype == EGUI_TYPE_STR)
		entrance_edit_string_set(key, ew_entry_get(w));
	else if(ktype == EGUI_TYPE_INT)
		entrance_edit_int_set(key, atoi(ew_entry_get(w)));
}

static void
_save_list(void *w, const char *key, int ktype)
{
	if(ktype == EGUI_TYPE_STR)
		entrance_edit_string_set(key, ew_list_selected_data_get(w));
	else if(ktype = EGUI_TYPE_INT)
	{
		char *s = ew_list_selected_data_get(w);
		entrance_edit_int_set(key, atoi(s));
	}
}

static void
_save_checkbox(void *w, const char *key, int ktype)
{
	if(ktype == EGUI_TYPE_INT)
		entrance_edit_int_set(key, ew_checkbox_is_active(w));
}

static void
_close(void)
{
	ew_dialog_destroy(win);
}



static int
_apply(void) 
{
	/*printf("starting apply main\n");*/
	Egui_Settings_Item *item;
	ecore_list_goto_first(items);

	while(item = ecore_list_next(items)) {

		/*printf("first widget key = %s, type = %d\n", item->entrance_edit_key, item->widget_type);*/
		switch(item->widget_type) {
			case EGUI_TYPE_BUTTON:
				_save_button(item->widget, item->entrance_edit_key, item->key_type);
				break;
			case EGUI_TYPE_ENTRY:
				_save_entry(item->widget, item->entrance_edit_key, item->key_type);
				break;
			case EGUI_TYPE_LIST:
				_save_list(item->widget, item->entrance_edit_key, item->key_type);
				break;
			case EGUI_TYPE_CHECKBOX:
				_save_checkbox(item->widget, item->entrance_edit_key, item->key_type);
				break;
			default: break;
		}
	}

	if(!entrance_edit_save())
	{
		ew_messagebox_ok("Entrance Configuration - Error", "Can not save to config file. Please check your permissions", EW_MESSAGEBOX_ICON_ERROR);
		return 0;
	}

	return 1;
}

/*callbacks*/
static void
_cb_close(void* sender, void* data)
{
	_close();
}

static void
_cb_apply(void* sender, void* data)
{
	/*printf("calling on apply to apply me\n");*/
	_apply();
}

static void
_cb_ok(void* sender, void* data)
{
	if(_apply()) 
		_close();
}

static void
_cb_group_build(void *listdata, void* data) 
{
	Egui_Settings_Group group = listdata;
	Entrance_Widget grp = ew_dialog_group_add(win, group->title, group->direction);

	ecore_list_for_each(group->items, _cb_item_build, grp);
}

static void
_cb_item_build(void * listdata, void* data)
{
	Egui_Settings_Item *itemp = listdata;
	Egui_Settings_Item item = *itemp;

	Entrance_Widget group = data;

	ew_group_add(group, _item_build(item));

	ecore_list_append(items, &item);
}
