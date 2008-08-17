#include <limits.h>
#include <Entrance_Edit.h>
#include <Entrance_Widgets.h>
#include "Egui.h"

void
egui_load_button(void *w, const char *key, int ktype)
{
	if(ktype == EGUI_TYPE_INT)
	 ew_toggle_button_active_set(w, entrance_edit_int_get(key));
}

void
egui_load_entry(void *w, const char *key, int ktype)
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

void
egui_load_checkbox(void *w, const char *key, int ktype)
{
	if(ktype == EGUI_TYPE_INT)
	{
		if(entrance_edit_int_get(key))
			ew_checkbox_toggle(w);
	}
}

void
egui_save_button(void *w, const char *key, int ktype)
{
	if(ktype == EGUI_TYPE_INT)
		entrance_edit_int_set(key, ew_toggle_button_active_get(w));
}

void
egui_save_entry(void *w, const char *key, int ktype)
{
	if(ktype == EGUI_TYPE_STR)
		entrance_edit_string_set(key, ew_entry_get(w));
	else if(ktype == EGUI_TYPE_INT)
		entrance_edit_int_set(key, atoi(ew_entry_get(w)));
}

void
egui_save_list(void *w, const char *key, int ktype)
{
	if(ktype == EGUI_TYPE_STR)
		entrance_edit_string_set(key, ew_list_selected_data_get(w));
	else if(ktype = EGUI_TYPE_INT)
	{
		char *s = ew_list_selected_data_get(w);
		entrance_edit_int_set(key, atoi(s));
	}
}

void
egui_save_checkbox(void *w, const char *key, int ktype)
{
	if(ktype == EGUI_TYPE_INT)
		entrance_edit_int_set(key, ew_checkbox_is_active(w));
}

char*
egui_get_current_bg(void)
{
	return entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_BACKGROUND_STR);
}

char*
egui_get_current_theme(void)
{
	char *res = calloc(PATH_MAX, sizeof(*res));
	if(!res)
		return NULL;

	char* theme = entrance_edit_string_get(ENTRANCE_EDIT_KEY_CLIENT_THEME_STR);
	snprintf(res, PATH_MAX, PREFIX "/share/entrance/themes/%s", theme);

	return res;
}
