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

static void _cb_group_load(void*, void*);
static void _cb_group_save(void*, void*);

static void _cb_item_load(void*, void*);
static void _cb_item_save(void*, void*);

static void _cb_foreach_item(void *listdata, void *data);
static void _cb_foreach_group(void *listdata, void *data);

static void _close(void*);
static int _apply(void*);
static void _save_to_file(void*);

static void _load_button(void *w, const char *key, int ktype);
static void _load_entry(void *w, const char *key, int ktype);
static void _load_checkbox(void *w, const char *key, int ktype);

static void _save_button(void *w, const char *key, int ktype);
static void _save_entry(void *w, const char *key, int ktype);
static void _save_checkbox(void *w, const char *key, int ktype);
static void _save_list(void *w, const char *key, int ktype);

static void _item_load(Egui_Settings_Item);
static void _item_save(Egui_Settings_Item);
Egui_Settings_Dialog _egui_sd_create_widgets(Egui_Settings_Dialog esd);



Egui_Settings_Dialog
egui_sd_new(char *title, Ecore_List *groups) 
{
	Egui_Settings_Dialog esd = calloc(1, sizeof(*esd));
	if(!esd)
		return NULL;

	esd->title = title;
	esd->settings_groups = groups;

	esd = _egui_sd_create_widgets(esd);

	return esd;
}


void
egui_sd_show(Egui_Settings_Dialog esd)
{
	if(!esd)
		return;

	if(esd->win == NULL)
		esd = _egui_sd_create_widgets(esd);

	ew_dialog_show(esd->win);
}

Egui_Settings_Dialog
_egui_sd_create_widgets(Egui_Settings_Dialog esd)
{
	if(!esd)
		return NULL;

	esd->win = ew_dialog_new(esd->title, EW_FALSE);

	esd->group_list = ecore_list_new();
	esd->item_list = ecore_list_new();

	ecore_list_for_each(esd->settings_groups, _cb_group_load, esd);
	ecore_list_goto_first(esd->settings_groups);

	ew_dialog_close_button_add(esd->win, _cb_close, esd);
	ew_dialog_apply_button_add(esd->win, _cb_apply, esd);
	ew_dialog_ok_button_add(esd->win, _cb_ok, esd);

	return esd;
}

Egui_Settings_Group 
egui_sg_new(const char* name, int direction)
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
egui_sg_item_append(Egui_Settings_Group g, Egui_Settings_Item i)
{
	if(!g)
		return;

	ecore_list_append(g->items, i);

}

Egui_Settings_Item
egui_si_new(void *widget, int widget_type, const char *entrance_edit_key, int key_type)
{
	Egui_Settings_Item esi = calloc(1, sizeof(*esi));
	if(!esi)
		return NULL;

	esi->entrance_edit_key = strdup(entrance_edit_key);
	esi->key_type = key_type;
	esi->widget_type = widget_type;
	esi->widget = widget;

	return esi;
}



/*private*/

void
_item_load(Egui_Settings_Item item) 
{
	switch(item->widget_type) 
	{
		/*TODO: move widget builders from case body into _widget_build_button, _widget_build_checkbox, etc*/
		case EGUI_TYPE_BUTTON:
			_load_button(item->widget, item->entrance_edit_key, item->key_type);
			break;
		case EGUI_TYPE_ENTRY:
			_load_entry(item->widget, item->entrance_edit_key, item->key_type);
			break;
		case EGUI_TYPE_CHECKBOX:
			_load_checkbox(item->widget, item->entrance_edit_key, item->key_type);
			break;
		default: break;
	}

}

void
_item_save(Egui_Settings_Item item)
{
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
_close(void* data)
{
	Egui_Settings_Dialog esd = data;
	if(!esd)
		return;

	ew_dialog_destroy(esd->win);
	esd->win = NULL;
}


static int
_apply(void* data) 
{
	Egui_Settings_Dialog esd = data;
	if(!esd)
		return;

	ecore_list_for_each(esd->settings_groups, _cb_group_save, NULL);

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
	_close(data);
	/*TODO: free(data) at this point?*/
	data = NULL;
}

static void
_cb_apply(void* sender, void* data)
{
	_apply(data);
}

static void
_cb_ok(void* sender, void* data)
{
	if(_apply(data)) 
		_close(data);
}

static void
_cb_group_load(void *listdata, void* data) 
{
	Egui_Settings_Dialog esd = data;
	if(!esd)
		return;

	Egui_Settings_Group group = listdata;
	Entrance_Widget grp = ew_dialog_group_add(esd->win, group->title, group->direction);
	ecore_list_append(esd->group_list, grp);
	ecore_list_for_each(group->items, _cb_item_load, esd);
}

static void
_cb_group_save(void *listdata, void* data)
{
	Egui_Settings_Group group = listdata;
	ecore_list_for_each(group->items, _cb_item_save, NULL);
}

static void
_cb_item_load(void *listdata, void* data)
{
	Egui_Settings_Dialog esd = data;
	if(!esd)
		return;

	Egui_Settings_Item item = listdata;
	_item_load(item);

	Entrance_Widget group = ecore_list_last(esd->group_list);
	ew_group_add(group, item->widget);
}

static void
_cb_item_save(void *listdata, void* data)
{
	Egui_Settings_Item item = listdata;
	_item_save(item);
}

static void
_cb_foreach_item(void *listdata, void *data)
{
	Egui_Settings_Item item = listdata;
	printf("widget key = %s, type = %d\n", item->entrance_edit_key, item->widget_type);
}

static void
_cb_foreach_group(void *listdata, void *data)
{
	Egui_Settings_Group group = listdata;
	printf("group = %s\n", group->title);
}
