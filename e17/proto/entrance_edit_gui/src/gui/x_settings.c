#include <limits.h>
#include <stdlib.h>
#include <strings.h>
#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <stdio.h>

static void _xs_cb_apply(void*, void*);
static void _xs_cb_close(void*, void*);
static void _xs_cb_ok(void*, void*);

static void _get_default_values(void);

void _gs_close(void);

static Entrance_Dialog win;

static Entrance_Widget group_daemon;
static Entrance_Widget group_client;

static Entrance_Entry entry_attempts;
static Entrance_Entry entry_xserver;
static Entrance_Entry entry_xession;

int val_attempts;
const char* val_xserver;
const char* val_xsession;

void
egui_x_settings_dialog_show() 
{
	win = ew_dialog_new(_("Enligtenment Configuration - X Settings"), EW_FALSE);

	_get_default_values();

	group_daemon = ew_dialog_group_add(win, _("Daemon Settings"), EW_GROUP_VERTICAL);

	char v[PATH_MAX];
	snprintf(v, PATH_MAX, "%d", val_attempts);
	printf("attempts = %s\n", v);
	entry_attempts = ew_entry_new("X Attempts", v, EW_FALSE);

	ew_group_add(group_daemon, entry_attempts);

	entry_xserver = ew_entry_new("X Commandline", val_xserver, EW_FALSE);
	ew_group_add(group_daemon, entry_xserver);

	group_client = ew_dialog_group_add(win, _("Client Settings"), EW_GROUP_VERTICAL);

	entry_xession = ew_entry_new("X Session Path", val_xsession, EW_FALSE);
	ew_group_add(group_client, entry_xession);

	ew_dialog_close_button_add(win, _xs_cb_close, NULL);
	ew_dialog_apply_button_add(win, _xs_cb_apply, NULL);
	ew_dialog_ok_button_add(win, _xs_cb_ok, NULL);

	ew_dialog_show(win);

	ew_entry_bugfix_makeshow(entry_attempts);
	ew_entry_bugfix_makeshow(entry_xession);
	ew_entry_bugfix_makeshow(entry_xserver);
}


/*priates*/

static void
_xs_cb_close(void * sender, void *data)
{
 	ew_dialog_destroy(win);
}

static void
_xs_cb_apply(void* sender, void* data)
{
	ew_messagebox_ok("Debug", "Apply clicked!", EW_MESSAGEBOX_ICON_MESSAGE);
}

static void
_xs_cb_ok(void *sender, void *data)
{
	ew_messagebox_ok("Debug", "Ok clicked", EW_MESSAGEBOX_ICON_MESSAGE);
}

static void
_get_default_values(void)
{
	val_attempts = entrance_edit_int_get(ENTRANCE_EDIT_KEY_DAEMON_ATTEMPTS_INT);

	val_xserver = strdup(entrance_edit_string_get(ENTRANCE_EDIT_KEY_DAEMON_XSERVER_STR));
	val_xsession = strdup(entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_XSESSION_STR));
}
