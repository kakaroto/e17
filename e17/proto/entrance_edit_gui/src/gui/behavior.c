#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <stdio.h>
#include "Egui.h"

void
egui_behavior_dialog_show(void *data) {
	Ecore_List *groups = ecore_list_new();

	/* Autologin group */
	Egui_Settings_Group autologin = egui_sg_new("AutoLogin", EW_GROUP_VERTICAL);
	Egui_Settings_Item autologin_check = egui_si_new(
		ew_checkbox_new("Enable Autologin"),
		EGUI_TYPE_CHECKBOX,
		ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_MODE_INT,
		EGUI_TYPE_INT
	);

	Egui_Settings_Item autologin_entry = egui_si_new(
		ew_entry_new("Autologin User", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_AUTOLOGIN_USER_STR,
		EGUI_TYPE_STR
	);

	/* Preselect group */
	Egui_Settings_Group preselect = egui_sg_new("Preselect", EW_GROUP_VERTICAL);
	Egui_Settings_Item preselect_check = egui_si_new(
		ew_checkbox_new("Enable Preselect Mode"),
		EGUI_TYPE_CHECKBOX,
		ENTRANCE_EDIT_KEY_CLIENT_PRESEL_MODE_INT,
		EGUI_TYPE_INT
	);

	Egui_Settings_Item preselect_entry = egui_si_new(
		ew_entry_new("Previous Preselect User", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_PRESEL_PREVUSER_STR,
		EGUI_TYPE_STR
	);

	/* Remember group */
	Egui_Settings_Group remember = egui_sg_new("Remember", EW_GROUP_VERTICAL);
	Egui_Settings_Item remember_check = egui_si_new(
		ew_checkbox_new("Enable User Remember"),
		EGUI_TYPE_CHECKBOX,
		ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_INT,
		EGUI_TYPE_INT
	);

	Egui_Settings_Item remember_count = egui_si_new(
		ew_entry_new("User Remember Count", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_USER_REMEMBER_N_INT,
		EGUI_TYPE_INT
	);

	Egui_Settings_Item remember_user_count = egui_si_new(
		ew_entry_new("User Count", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_USER_COUNT_INT,
		EGUI_TYPE_INT
	);
	
	/* Others group */
	Egui_Settings_Group others = egui_sg_new("Others", EW_GROUP_VERTICAL);
	Egui_Settings_Item others_auth = egui_si_new(
		ew_checkbox_new("Use Authentication"),
		EGUI_TYPE_CHECKBOX,
		ENTRANCE_EDIT_KEY_CLIENT_AUTH_INT,
		EGUI_TYPE_INT
	);

	Egui_Settings_Item others_engine = egui_si_new(
		ew_checkbox_new("Use OpenGL Engine"),
		EGUI_TYPE_CHECKBOX,
		ENTRANCE_EDIT_KEY_CLIENT_ENGINE_INT,
		EGUI_TYPE_INT
	);

	Egui_Settings_Item others_reboot = egui_si_new(
		ew_checkbox_new("Show Reboot Button"),
		EGUI_TYPE_CHECKBOX,
		ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_REBOOT_INT,
		EGUI_TYPE_INT
	);

	Egui_Settings_Item others_halt = egui_si_new(
		ew_checkbox_new("Show Halt Button"),
		EGUI_TYPE_CHECKBOX,
		ENTRANCE_EDIT_KEY_CLIENT_SYSTEM_HALT_INT,
		EGUI_TYPE_INT
	);

	/* init */
	egui_sg_item_append(autologin, autologin_check);
	egui_sg_item_append(autologin, autologin_entry);
	ecore_list_append(groups, autologin);

	egui_sg_item_append(preselect, preselect_check);
	egui_sg_item_append(preselect, preselect_entry);
	ecore_list_append(groups, preselect);

	egui_sg_item_append(remember, remember_check);
	egui_sg_item_append(remember, remember_count);
	egui_sg_item_append(remember, remember_user_count);
	ecore_list_append(groups, remember);

	egui_sg_item_append(others, others_auth);
	egui_sg_item_append(others, others_engine);
	egui_sg_item_append(others, others_reboot);
	egui_sg_item_append(others, others_halt);
	ecore_list_append(groups, others);
	/* show */
	static Egui_Settings_Dialog esd = NULL;
	if(!esd)
		esd = egui_sd_new("Behaviour", groups);

	egui_sd_show(esd);
}
