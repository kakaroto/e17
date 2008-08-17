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

typedef struct {
	Entrance_Dialog win;

	Entrance_Widget group_autologin;
	Entrance_Widget check_autologin;
	Entrance_Entry entry_autologin_user;

	Entrance_Widget group_preselect;
	Entrance_Widget check_preselect;
	Entrance_Entry entry_preselect_user;

	Entrance_Widget group_remember;
	Entrance_Widget check_remember;
	Entrance_Entry entry_remember_count;
	Entrance_Entry entry_remember_user_count;


	Entrance_Widget group_others;
	Entrance_Widget check_others_auth;
	Entrance_Widget check_others_engine;
	Entrance_Widget check_others_reboot;
	Entrance_Widget check_others_halt;
}* Behaviour_Dialog;

static Behaviour_Dialog _create_widgets(Behaviour_Dialog);
static void _show(Behaviour_Dialog bd);

void
egui_behavior_dialog_show(void *data) 
{
	static Behaviour_Dialog bd = NULL;
	
	if (bd == NULL)
	{
		bd = calloc(1, sizeof(*bd));

		if(!bd)
			return;
	}

	_show(bd);
}


static void
_show(Behaviour_Dialog bd)
{
	if(!bd)
		return;

	if(bd->win == NULL)
	{
		bd = _create_widgets(bd);
	}

	ew_dialog_show(bd->win);
}

Behaviour_Dialog
_create_widgets(Behaviour_Dialog bd) {

	if(!bd)
		return NULL;

	if(bd->win != NULL)
		return bd;

	bd->win = ew_dialog_new("Behaviour", EW_FALSE);

	/*Autologin group*/
	bd->group_autologin = ew_dialog_group_add(bd->win, "Autologin", EW_GROUP_VERTICAL);

	bd->check_autologin = ew_checkbox_new("Enable Autologin");
	egui_load_checkbox(bd->check_autologin, ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_MODE_INT, EGUI_TYPE_INT);
	ew_group_add(bd->group_autologin, bd->check_autologin);

	bd->entry_autologin_user = ew_entry_new("Autologin User", NULL, EW_FALSE);
	egui_load_entry(bd->entry_autologin_user, ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_USER_STR, EGUI_TYPE_STR);
	ew_group_add(bd->group_autologin, bd->entry_autologin_user);

	/* Preselect group */
	bd->group_preselect = ew_dialog_group_add(bd->win, "Preselect", EW_GROUP_VERTICAL);

	bd->check_preselect = ew_checkbox_new("Enable Preselect Mode");
	egui_load_checkbox(bd->check_preselect, ENTRANCE_EDIT_KEY_CLIENT_PRESEL_MODE_INT, EGUI_TYPE_INT);
	ew_group_add(bd->group_preselect, bd->check_preselect);

	bd->entry_preselect_user = ew_entry_new("Preselected User", NULL, EW_FALSE);
	egui_load_entry(bd->entry_preselect_user, ENTRANCE_EDIT_KEY_CLIENT_PRESEL_PREVUSER_STR, EGUI_TYPE_STR);
	ew_group_add(bd->group_preselect, bd->entry_preselect_user);

	/* Remember group */
	bd->group_remember = ew_dialog_group_add(bd->win, "Remember", EW_GROUP_VERTICAL);

	bd->check_remember = ew_checkbox_new("Enable User Remember");
	egui_load_checkbox(bd->check_remember, ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_INT, EGUI_TYPE_INT);
	ew_group_add(bd->group_remember, bd->check_remember);

	bd->entry_remember_count = ew_entry_new("User Remember Count", NULL, EW_FALSE);
	egui_load_entry(bd->entry_remember_count, ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_N_INT, EGUI_TYPE_INT);
	ew_group_add(bd->group_remember, bd->entry_remember_count);

	bd->entry_remember_user_count = ew_entry_new("User Count", NULL, EW_FALSE);
	egui_load_entry(bd->entry_remember_user_count, ENTRANCE_EDIT_KEY_CLIENT_USER_COUNT_INT, EGUI_TYPE_INT);
	ew_group_add(bd->group_remember, bd->entry_remember_user_count);
	
	/* Others group */
	bd->group_others = ew_dialog_group_add(bd->win, "Others", EW_GROUP_VERTICAL);

	bd->check_others_auth = ew_checkbox_new("Use Authentication");
	egui_load_checkbox(bd->check_others_auth, ENTRANCE_EDIT_KEY_CLIENT_AUTH_INT, EGUI_TYPE_INT);
	ew_group_add(bd->group_others, bd->check_others_auth);

	bd->check_others_engine = ew_checkbox_new("Use OpenGL Engine");
	egui_load_checkbox(bd->check_others_engine, ENTRANCE_EDIT_KEY_CLIENT_ENGINE_INT, EGUI_TYPE_INT);
	ew_group_add(bd->group_others, bd->check_others_engine);

	bd->check_others_reboot = ew_checkbox_new("Show Reboot Button");
	egui_load_checkbox(bd->check_others_reboot, ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_REBOOT_INT, EGUI_TYPE_INT);
	ew_group_add(bd->group_others, bd->check_others_reboot);

	bd->check_others_halt = ew_checkbox_new("Show Halt Button");
	egui_load_checkbox(bd->check_others_halt, ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_HALT_INT, EGUI_TYPE_INT);
	ew_group_add(bd->group_others, bd->check_others_halt);

	ew_dialog_close_button_add(bd->win, _cb_close, bd);
	ew_dialog_apply_button_add(bd->win, _cb_apply, bd);
	ew_dialog_ok_button_add(bd->win, _cb_ok, bd);

	ew_dialog_onclose_set(bd->win, _cb_close, bd);

	return bd;
}

/*private*/
static void
_close(void* data)
{
	Behaviour_Dialog bd = data;
	if(!bd)
		return;

	ew_dialog_destroy(bd->win);
	bd->win = NULL;
}


static int
_apply(void* data) 
{
	Behaviour_Dialog bd = data;
	if(!bd)
		return;

	egui_save_checkbox(bd->check_autologin, ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_MODE_INT, EGUI_TYPE_INT);
	egui_save_entry(bd->entry_autologin_user, ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_USER_STR, EGUI_TYPE_STR);

	egui_save_checkbox(bd->check_preselect, ENTRANCE_EDIT_KEY_CLIENT_PRESEL_MODE_INT, EGUI_TYPE_INT);
	egui_save_entry(bd->entry_preselect_user, ENTRANCE_EDIT_KEY_CLIENT_PRESEL_PREVUSER_STR, EGUI_TYPE_STR);

	egui_save_checkbox(bd->check_remember, ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_INT, EGUI_TYPE_INT);
	egui_save_entry(bd->entry_remember_count, ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_N_INT, EGUI_TYPE_INT);
	egui_save_entry(bd->entry_remember_user_count, ENTRANCE_EDIT_KEY_CLIENT_USER_COUNT_INT, EGUI_TYPE_INT);

	egui_save_checkbox(bd->check_others_auth, ENTRANCE_EDIT_KEY_CLIENT_AUTH_INT, EGUI_TYPE_INT);
	egui_save_checkbox(bd->check_others_engine, ENTRANCE_EDIT_KEY_CLIENT_ENGINE_INT, EGUI_TYPE_INT);
	egui_save_checkbox(bd->check_others_reboot, ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_REBOOT_INT, EGUI_TYPE_INT);
	egui_save_checkbox(bd->check_others_halt, ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_HALT_INT, EGUI_TYPE_INT);

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


