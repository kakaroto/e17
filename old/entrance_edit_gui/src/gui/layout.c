#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <stdio.h>
#include "Egui.h"

static void _cb_close(void*, void*);
static void _cb_apply(void*, void*);
static void _cb_ok(void *, void *);

static void _close(void*);
static int _apply(void*);
static void _save_to_file(void*);

typedef struct  {
	Entrance_Dialog win;

	Entrance_Widget group_greeting;
	Entrance_Entry entry_greeting_before;
	Entrance_Entry entry_greeting_after;

	Entrance_Widget group_format;
	Entrance_Entry entry_time_format;
	Entrance_Entry entry_date_format;

}* Layout_Dialog;

static Layout_Dialog _create_widgets(Layout_Dialog);
static void _show(Layout_Dialog ld);

void
egui_layout_dialog_show(void *data) 
{
	static Layout_Dialog ld = NULL;
	
	if (ld == NULL)
	{
		ld = calloc(1, sizeof(*ld));

		if(!ld)
			return;
	}

	_show(ld);
}


static void
_show(Layout_Dialog ld)
{
	if(!ld)
		return;

	if(ld->win == NULL)
	{
		ld = _create_widgets(ld);
	}

	ew_dialog_show(ld->win);
}


static Layout_Dialog
_create_widgets(Layout_Dialog ld) 
{
	if (!ld)
		return NULL;

	if(ld->win != NULL)
		return ld;

	ld->win = ew_dialog_new("Layout Settings", EW_FALSE);

	ld->group_greeting = ew_dialog_group_add(ld->win, "Greetings", EW_GROUP_VERTICAL);

	ld->entry_greeting_before = ew_entry_new("Before", NULL, EW_FALSE);
	egui_load_entry(ld->entry_greeting_before, ENTRANCE_EDIT_KEY_CLIENT_GREETING_BEFORE_STR, EGUI_TYPE_STR);
	ew_group_add(ld->group_greeting, ld->entry_greeting_before);

	ld->entry_greeting_after = ew_entry_new("After", NULL, EW_FALSE);
	egui_load_entry(ld->entry_greeting_after, ENTRANCE_EDIT_KEY_CLIENT_GREETING_AFTER_STR, EGUI_TYPE_STR);
	ew_group_add(ld->group_greeting, ld->entry_greeting_after);

	ld->group_format = ew_dialog_group_add(ld->win, "Time/Date Format", EW_GROUP_VERTICAL);

	ld->entry_time_format = ew_entry_new("Time Format", NULL, EW_FALSE);
	egui_load_entry(ld->entry_time_format, ENTRANCE_EDIT_KEY_CLIENT_TIME_FORMAT_STR, EGUI_TYPE_STR);
	ew_group_add(ld->group_format, ld->entry_time_format);

	ld->entry_date_format = ew_entry_new("Date Format", NULL, EW_FALSE);
	egui_load_entry(ld->entry_date_format, ENTRANCE_EDIT_KEY_CLIENT_DATE_FORMAT_STR, EGUI_TYPE_STR);
	ew_group_add(ld->group_format, ld->entry_date_format);

	ew_dialog_close_button_add(ld->win, _cb_close, ld);
	ew_dialog_apply_button_add(ld->win, _cb_apply, ld);
	ew_dialog_ok_button_add(ld->win, _cb_ok, ld);

	ew_dialog_onclose_set(ld->win, _cb_close, ld);

	return ld;
}




/*private*/
static void
_close(void* data)
{
	Layout_Dialog ld = data;
	if(!ld)
		return;

	ew_dialog_destroy(ld->win);
	ld->win = NULL;
}


static int
_apply(void* data) 
{
	Layout_Dialog ld = data;
	if(!ld)
		return;

	egui_save_entry(ld->entry_greeting_before, ENTRANCE_EDIT_KEY_CLIENT_GREETING_BEFORE_STR, EGUI_TYPE_STR);
	egui_save_entry(ld->entry_greeting_after, ENTRANCE_EDIT_KEY_CLIENT_GREETING_AFTER_STR, EGUI_TYPE_STR);
	egui_save_entry(ld->entry_time_format, ENTRANCE_EDIT_KEY_CLIENT_TIME_FORMAT_STR, EGUI_TYPE_STR);
	egui_save_entry(ld->entry_date_format, ENTRANCE_EDIT_KEY_CLIENT_DATE_FORMAT_STR, EGUI_TYPE_STR);

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
