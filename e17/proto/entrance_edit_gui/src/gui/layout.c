#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <stdio.h>
#include "Egui.h"

void
egui_layout_dialog_show(void *data) 
{

	Egui_Settings_Group group_greeting = egui_sg_new("Greetings", EW_GROUP_VERTICAL);

	Egui_Settings_Item before = egui_si_new(
		ew_entry_new("Before", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_GREETING_BEFORE_STR,
		EGUI_TYPE_STR
	);

	Egui_Settings_Item after = egui_si_new(
		ew_entry_new("After", NULL, EW_FALSE), 
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_GREETING_AFTER_STR,
		EGUI_TYPE_STR
	);

	egui_sg_item_append(group_greeting, before);
	egui_sg_item_append(group_greeting, after);

	Egui_Settings_Group group_format = egui_sg_new("Time/Date Format", EW_GROUP_VERTICAL);

    Egui_Settings_Item time_format = egui_si_new( 
		ew_entry_new("Time format", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_TIME_FORMAT_STR,
		EGUI_TYPE_STR
	);

	Egui_Settings_Item date_format = egui_si_new( 
		ew_entry_new("Data format", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_TIME_FORMAT_STR,
		EGUI_TYPE_STR
	);

	egui_sg_item_append(group_format, time_format);
	egui_sg_item_append(group_format, date_format);
	
	Ecore_List *groups = ecore_list_new();
	ecore_list_append(groups, group_greeting);
	ecore_list_append(groups, group_format);

	static Egui_Settings_Dialog esd = NULL;
	if(!esd)
		esd = egui_sd_new("Layout settings", groups);

	egui_sd_show(esd);
}
