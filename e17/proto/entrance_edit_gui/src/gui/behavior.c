#include <limits.h>
#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <stdio.h>

static void _cb_close(void*, void*);
static void _cb_apply(void*, void*);
static void _cb_ok(void*, void*);

static void _close(void);
static int _apply(void);
static int _save_to_file(void);

static Entrance_Dialog win;

static Entrance_Widget group_login;
static Entrance_Widget login_autologin_mode;
static Entrance_Entry login_autologin_user;

static int val_login_mode;
static const char* val_login_user;

static void _build_group_login(void);
static void _load_login(void);
static void _save_login(void);

static Entrance_Widget group_presel;
static Entrance_Widget presel_mode;
static Entrance_Entry presel_prevuser;

static int val_presel_mode;
static const char* val_presel_prevuser;

static void _build_group_presel(void);
static void _load_presel(void);
static void _save_presel(void);

static Entrance_Widget group_remember;
static Entrance_Widget remember_user_remember;
static Entrance_Entry remember_remember_count;
static Entrance_Entry remember_user_count;

static int val_user_remember;
static int val_user_remember_count;
static int val_user_count;

static void _build_group_remember(void);
static void _load_remember(void);
static void _save_remember(void);


static Entrance_Widget group_others;
static Entrance_Widget others_auth;
static Entrance_Widget others_engine;
static Entrance_Widget others_reboot;
static Entrance_Widget others_halt;

static int val_auth;
static int val_engine;
static int val_reboot;
static int val_halt;

static void _build_group_others(void);
static void _load_others(void);
static void _save_others(void);

void
egui_behavior_dialog_show() {
	win = ew_dialog_new(_("Entrance Configuration - Behaviour"), EW_FALSE);

	_build_group_login();
	_build_group_presel();
	_build_group_remember();
	_build_group_others();

	ew_dialog_close_button_add(win, _cb_close, NULL);
	ew_dialog_apply_button_add(win, _cb_apply, NULL);
	ew_dialog_ok_button_add(win, _cb_ok, NULL);

	ew_dialog_show(win);
}

static void
_build_group_login(void)
{
	group_login = ew_dialog_group_add(win, _("AutoLogin"), EW_GROUP_VERTICAL);

	val_login_user = NULL;

	_load_login();

	login_autologin_mode = ew_checkbox_new(_("Enable Autologin"));
	if(val_login_mode)
		ew_checkbox_toggle(login_autologin_mode);
	ew_group_add(group_login, login_autologin_mode);

	login_autologin_user = ew_entry_new(_("Autologin User"), val_login_user, EW_FALSE);
	ew_group_add(group_login, login_autologin_user);
}


static void
_build_group_presel(void)
{
	group_presel = ew_dialog_group_add(win, _("Preselect"), EW_GROUP_VERTICAL);

	val_presel_prevuser = NULL;
	_load_presel();

	presel_mode = ew_checkbox_new(_("Enable Preselect Mode"));
	if(val_presel_mode)
		ew_checkbox_toggle(presel_mode);
	ew_group_add(group_presel, presel_mode);

	presel_prevuser = ew_entry_new(_("Previous Preselect User"), val_presel_prevuser, EW_FALSE);
	ew_group_add(group_presel, presel_prevuser);
}



static void
_build_group_others(void)
{
	group_others = ew_dialog_group_add(win, _("Others"), EW_GROUP_VERTICAL);

	_load_others();

	others_auth = ew_checkbox_new(_("Use Authentication"));
	if(val_auth)
		ew_checkbox_toggle(others_auth);
	ew_group_add(group_others, others_auth);

	others_engine = ew_checkbox_new(_("Use Engine"));
	if(val_engine)
		ew_checkbox_toggle(others_engine);
	ew_group_add(group_others, others_engine);

	others_reboot = ew_checkbox_new(_("Enable Reboot Button"));
	if(val_reboot)
		ew_checkbox_toggle(others_reboot);
	ew_group_add(group_others, others_reboot);

	others_halt = ew_checkbox_new(_("Enable Halt Button"));
	if(val_halt)
		ew_checkbox_toggle(others_halt);
	ew_group_add(group_others, others_halt);
}

static void
_build_group_remember(void)
{
	char str[PATH_MAX];
	group_remember = ew_dialog_group_add(win, _("Remember"), EW_GROUP_VERTICAL);

	_load_remember();

	remember_user_remember = ew_checkbox_new(_("Enable User Remember"));
	if(val_user_remember)
		ew_checkbox_toggle(remember_user_remember);
	ew_group_add(group_remember, remember_user_remember);


	snprintf(str, PATH_MAX, "%d", val_user_remember_count);
	remember_remember_count	= ew_entry_new(_("User Remember Count"), str, EW_FALSE);
	ew_group_add(group_remember, remember_remember_count);

	snprintf(str, PATH_MAX, "%d", val_user_count);
	remember_user_count	= ew_entry_new(_("User Count"), str,  EW_FALSE);
	ew_group_add(group_remember, remember_user_count);
}


static void
_load_login(void)
{
	val_login_mode = entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_MODE_INT);
	val_login_user = entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_USER_STR);
}

static void
_load_presel(void)
{
	val_presel_mode = entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_PRESEL_MODE_INT);
	val_presel_prevuser = entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_PRESEL_PREVUSER_STR);
}

static void
_load_others(void)
{
	val_auth = entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_AUTH_INT);
	val_engine = entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_ENGINE_INT);
	val_reboot = entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_REBOOT_INT);
	val_halt = entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_HALT_INT);
}

static void
_load_remember(void)
{
	val_user_remember = entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_INT);
	val_user_remember_count	 = entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_N_INT);
	val_user_count = entrance_edit_int_get(ENTRANCE_EDIT_KEY_CLIENT_USER_COUNT_INT);
}

static void
_save_login(void)
{
	if(ew_checkbox_is_active(login_autologin_mode))
		val_login_mode = 1;
	else
		val_login_mode = 0;

	val_login_user = ew_entry_get(login_autologin_user);

	entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_MODE_INT, val_login_mode);
	entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_USER_STR, val_login_user);
}

static void
_save_presel(void)
{
	if(ew_checkbox_is_active(presel_mode))
		val_presel_mode = 1;
	else
		val_presel_mode = 0;

	val_presel_prevuser = ew_entry_get(presel_prevuser);

	entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_PRESEL_MODE_INT, val_presel_mode);
	entrance_edit_string_set(ENTRANCE_EDIT_KEY_CLIENT_PRESEL_PREVUSER_STR, val_presel_prevuser);
}

static void
_save_remember(void)
{
	if(ew_checkbox_is_active(remember_user_remember))
		val_user_remember = 1;
	else
		val_user_remember = 0;

	val_user_remember_count = atoi(ew_entry_get(remember_remember_count));
	val_user_count	= atoi(ew_entry_get(remember_user_count));

	entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_INT, val_user_remember);
	entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_N_INT, val_user_remember_count);
	entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_USER_COUNT_INT, val_user_count);
}


static void
_save_others(void)
{
	if(ew_checkbox_is_active(others_auth))
		val_auth = 1;
	else
		val_auth = 0;

	if(ew_checkbox_is_active(others_engine))
		val_engine = 1;
	else 
		val_engine = 0;

	if(ew_checkbox_is_active(others_reboot))
		val_reboot = 1;
	else
		val_reboot = 0;

	if(ew_checkbox_is_active(others_halt))
		val_halt = 1;
	else
		val_halt = 0;

	entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_AUTH_INT, val_auth);
	entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_ENGINE_INT, val_engine);
	entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_REBOOT_INT, val_reboot);
	entrance_edit_int_set(ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_HALT_INT, val_halt);
}

static int
_save_to_file(void)
{
	if(entrance_edit_save())
		return 1;
	else
		return 0;
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
	_apply();
}

static void
_cb_ok(void* sender, void* data)
{
	if(_apply()) 
		_close();
}


static void
_close(void)
{
	ew_dialog_destroy(win);
}


static int
_apply(void)
{
	_save_login();
	_save_presel();
	_save_remember();
	_save_others();

	if(_save_to_file())
		return 1;
	else
		ew_messagebox_ok("Entrance Configuration - Error", "Cannot save changes. Please Check You Permissions", EW_MESSAGEBOX_ICON_ERROR);

	return 0;
}
