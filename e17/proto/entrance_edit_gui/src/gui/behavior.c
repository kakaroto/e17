#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <stdio.h>
#include "Egui.h"

void
egui_behavior_dialog_show(void *data) {
	Ecore_List *groups = ecore_list_new();

	/* Autologin group */
	Egui_Settings_Group autologin = es_group_new("AutoLogin", EW_GROUP_VERTICAL);
	Egui_Settings_Item autologin_check = {
		ew_checkbox_new("Enable Autologin"),
		EGUI_TYPE_CHECKBOX,
		ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_MODE_INT,
		EGUI_TYPE_INT
	};
	Egui_Settings_Item autologin_entry = {
		ew_entry_new("Autologin User", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_USER_STR,
		EGUI_TYPE_STR
	};

	/* Preselect group */
	Egui_Settings_Group preselect = es_group_new("Preselect", EW_GROUP_VERTICAL);
	Egui_Settings_Item preselect_check = {
		ew_checkbox_new("Enable Preselect Mode"),
		EGUI_TYPE_CHECKBOX,
		ENTRANCE_EDIT_KEY_CLIENT_PRESEL_MODE_INT,
		EGUI_TYPE_INT
	};
	Egui_Settings_Item preselect_entry = {
		ew_entry_new("Previous Preselect User", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_PRESEL_PREVUSER_STR,
		EGUI_TYPE_STR
	};

	/* Remember group */
	Egui_Settings_Group remember = es_group_new("Remember", EW_GROUP_VERTICAL);
	Egui_Settings_Item remember_check = {
		ew_checkbox_new("Enable User Remember"),
		EGUI_TYPE_CHECKBOX,
		ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_INT,
		EGUI_TYPE_INT
	};
	Egui_Settings_Item remember_count = {
		ew_entry_new("User Remember Count", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_N_INT,
		EGUI_TYPE_INT
	};
	Egui_Settings_Item remember_user_count = {
		ew_entry_new("User Count", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_USER_COUNT_INT,
		EGUI_TYPE_INT
	};
	
	/* Others group */
	Egui_Settings_Group others = es_group_new("Others", EW_GROUP_VERTICAL);
	Egui_Settings_Item others_auth = {
		ew_checkbox_new("Use Authentication"),
		EGUI_TYPE_CHECKBOX,
		ENTRANCE_EDIT_KEY_CLIENT_AUTH_INT,
		EGUI_TYPE_INT
	};
	Egui_Settings_Item others_engine = {
		ew_checkbox_new("Use OpenGL Engine"),
		EGUI_TYPE_CHECKBOX,
		ENTRANCE_EDIT_KEY_CLIENT_ENGINE_INT,
		EGUI_TYPE_INT
	};
	Egui_Settings_Item others_reboot = {
		ew_checkbox_new("Show Reboot Button"),
		EGUI_TYPE_CHECKBOX,
		ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_REBOOT_INT,
		EGUI_TYPE_INT
	};
	Egui_Settings_Item others_halt = {
		ew_checkbox_new("Show Halt Button"),
		EGUI_TYPE_CHECKBOX,
		ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_HALT_INT,
		EGUI_TYPE_INT
	};

	/* init */
	es_group_append(autologin, &autologin_check);
	es_group_append(autologin, &autologin_entry);
	ecore_list_append(groups, autologin);

	es_group_append(preselect, &preselect_check);
	es_group_append(preselect, &preselect_entry);
	ecore_list_append(groups, preselect);

	es_group_append(remember, &remember_check);
	es_group_append(remember, &remember_count);
	es_group_append(remember, &remember_user_count);
	ecore_list_append(groups, remember);

	es_group_append(others, &others_auth);
	es_group_append(others, &others_engine);
	es_group_append(others, &others_reboot);
	es_group_append(others, &others_halt);
	ecore_list_append(groups, others);
	/* show */
	es_dialog_show("Behaviour", groups);
}
