#include <string.h>
#include <Entrance_Edit.h>
#include <Entrance_Widgets.h>
#include "Egui.h"

static void _cb_close(void*, void*);
static void _cb_apply(void*, void*);
static void _cb_ok(void *, void *);

static void _close(void*);
static int _apply(void*);
static void _save_to_file(void*);



typedef struct {
	Entrance_Dialog win;

	Entrance_Widget group_daemon_settings;
	Entrance_Entry entry_x_attempts;
	Entrance_Entry entry_x_commandline;

	Entrance_Widget group_client_settings;
	Entrance_Entry entry_x_session;
}* X_Settings_Dialog;


static X_Settings_Dialog _create_widgets(X_Settings_Dialog);
static void _show(X_Settings_Dialog xsd);


void
egui_x_settings_dialog_show(void *data) 
{
	static X_Settings_Dialog xsd = NULL;
	
	if (xsd == NULL)
	{
		xsd = calloc(1, sizeof(*xsd));

		if(!xsd)
			return;
	}

	_show(xsd);
}


static void
_show(X_Settings_Dialog xsd)
{
	if(!xsd)
		return;

	if(xsd->win == NULL)
	{
		xsd = _create_widgets(xsd);
	}

	ew_dialog_show(xsd->win);
}

static X_Settings_Dialog
_create_widgets(X_Settings_Dialog xsd)
{
	if(!xsd)
		return NULL;

	if(xsd->win != NULL)
		return xsd;

	xsd->win = ew_dialog_new("X Settings", EW_FALSE);

	xsd->group_daemon_settings = ew_dialog_group_add(xsd->win, "Daemon Settings", EW_GROUP_VERTICAL);

	xsd->entry_x_attempts = ew_entry_new("X Attempts", NULL, EW_FALSE);
	egui_load_entry(xsd->entry_x_attempts, ENTRANCE_EDIT_KEY_DAEMON_ATTEMPTS_INT, EGUI_TYPE_INT);

	xsd->entry_x_commandline = ew_entry_new("X Commandline", NULL, EW_FALSE);
	egui_load_entry(xsd->entry_x_commandline, ENTRANCE_EDIT_KEY_DAEMON_XSERVER_STR, EGUI_TYPE_STR);

	ew_group_add(xsd->group_daemon_settings, xsd->entry_x_attempts);
	ew_group_add(xsd->group_daemon_settings, xsd->entry_x_commandline);


	xsd->group_client_settings = ew_dialog_group_add(xsd->win, "Client Settings", EW_GROUP_VERTICAL);

	xsd->entry_x_session = ew_entry_new("X Session", NULL, EW_FALSE);
	egui_load_entry(xsd->entry_x_session, ENTRANCE_EDIT_KEY_CLIENT_XSESSION_STR, EGUI_TYPE_STR);

	ew_group_add(xsd->group_client_settings, xsd->entry_x_session);

	ew_dialog_close_button_add(xsd->win, _cb_close, xsd);
	ew_dialog_apply_button_add(xsd->win, _cb_apply, xsd);
	ew_dialog_ok_button_add(xsd->win, _cb_ok, xsd);

	ew_dialog_onclose_set(xsd->win, _cb_close, xsd);

	return xsd;
}


/*private*/


static void
_close(void* data)
{
	X_Settings_Dialog xsd = data;
	if(!xsd)
		return;

	ew_dialog_destroy(xsd->win);
	xsd->win = NULL;
}


static int
_apply(void* data) 
{
	X_Settings_Dialog xsd = data;
	if(!xsd)
		return;

	egui_save_entry(xsd->entry_x_attempts, ENTRANCE_EDIT_KEY_DAEMON_ATTEMPTS_INT, EGUI_TYPE_INT);
	egui_save_entry(xsd->entry_x_commandline, ENTRANCE_EDIT_KEY_DAEMON_XSERVER_STR, EGUI_TYPE_STR);
	egui_save_entry(xsd->entry_x_session, ENTRANCE_EDIT_KEY_CLIENT_XSESSION_STR, EGUI_TYPE_STR);

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
