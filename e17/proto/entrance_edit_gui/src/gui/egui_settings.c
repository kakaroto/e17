#include <limits.h>
#include <Evas.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Entrance_Edit.h>
#include <Entrance_Widgets.h>
#include "Egui.h"

static void _ests_cb_response(void *, int, void *);
static void _ests_cb_apply(void);
static void _ests_cb_close(void);

static void _ests_group_build(Egui_Settings_Group);
static Entrance_Widget _ests_item_build(Egui_Settings_Item);

static Entrance_Dialog dialog;
static Evas_List *items;

void
egui_settings_dialog_show(char *title, int count, Egui_Settings_Group groups[]) {
	int i = 0;
	dialog = ew_notice_new(title);
	items = NULL;

	for( ; i<count; i++)
		_ests_group_build(groups[i]);
	
	ew_notice_close_button_add(dialog, _ests_cb_response, NULL);
	ew_notice_apply_button_add(dialog, NULL, NULL);
	ew_notice_ok_button_add(dialog, NULL, NULL);

	ew_notice_show(dialog);
}

/*private*/
void
_ests_group_build(Egui_Settings_Group group) {
	Entrance_Widget grp = ew_dialog_group_add(dialog, group.title, group.direction);
	int i = 0;
	
	for( ; i<group.item_count; i++)
		ew_group_add(grp, _ests_item_build(group.items[i]));
}

Entrance_Widget
_ests_item_build(Egui_Settings_Item item) {
	switch(item.type) {
		case BUTTON:
			ew_toggle_button_active_set(item.widget, entrance_edit_int_get(item.entrance_edit_key));
			break;
		case ENTRY:
			ew_entry_set(item.widget, entrance_edit_string_get(item.entrance_edit_key));
			break;
		default: break;
	}
	evas_list_append(items, &item);
	return item.widget;
}

/*callbacks*/
static void
_ests_cb_response(void *owner, int response, void *data) {
	switch(response) {
		case EW_NOTICE_OK_BUTTON:
			_ests_cb_apply();
			_ests_cb_close();
			break;
		case EW_NOTICE_APPLY_BUTTON:
			_ests_cb_apply();
			break;
		case EW_NOTICE_CLOSE_BUTTON:
			_ests_cb_close();
			break;
		default: break;
	}
}

void
_ests_cb_apply() {
	char msg[PATH_MAX];
	Egui_Settings_Item *item;
	Evas_List *last = items;

	while(last = evas_list_next(last)) {
		item = evas_list_data(last);

		switch(item->type) {
			case BUTTON:
				entrance_edit_int_set(item->entrance_edit_key, ew_toggle_button_active_get(item->widget));
				break;
			case ENTRY:
				entrance_edit_string_set(item->entrance_edit_key, ew_entry_get(item->widget));
				break;
			case LIST:
				entrance_edit_string_set(item->entrance_edit_key, ew_list_selected_data_get(item->widget));
			default: break;
		}
	}

	if(!entrance_edit_save())
	{
		snprintf(msg, PATH_MAX, "Can not set a value. Please check your permissions");
		ew_messagebox_ok("Entrance Config - Error", msg, EW_MESSAGEBOX_ICON_ERROR);
	}
}

void
_ests_cb_close() {
	ew_dialog_destroy(dialog);
}
