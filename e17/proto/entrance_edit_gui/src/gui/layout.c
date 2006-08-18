#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <stdio.h>
#include "Egui.h"

void
egui_layout_dialog_show() 
{

	Egui_Settings_Group group_greeting = es_group_new("Greetings", EW_GROUP_VERTICAL);

	Egui_Settings_Item before = {
		ew_entry_new("Before", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_GREETING_BEFORE_STR,
		EGUI_TYPE_STR
	};

	Egui_Settings_Item after = {
		ew_entry_new("After", NULL, EW_FALSE), 
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_GREETING_AFTER_STR,
		EGUI_TYPE_STR
	};

	es_group_append(group_greeting, &before);
	es_group_append(group_greeting, &after);

	Egui_Settings_Group group_format = es_group_new("Time/Date Format", EW_GROUP_VERTICAL);

    Egui_Settings_Item time_format = { 
		ew_entry_new("Time format", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_TIME_FORMAT_STR,
		EGUI_TYPE_STR
	};

	Egui_Settings_Item date_format = { 
		ew_entry_new("Data format", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_TIME_FORMAT_STR,
		EGUI_TYPE_STR
	};

	es_group_append(group_format, &time_format);
	es_group_append(group_format, &date_format);
	
	Ecore_List *groups = ecore_list_new();
	ecore_list_append(groups, group_greeting);
	ecore_list_append(groups, group_format);

	es_dialog_show("Layout settings", groups);
}
