#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <stdio.h>
#include "Egui.h"

static Entrance_Widget group_display;
static Entrance_Entry display_greeting_before;
static Entrance_Entry display_greeting_after;
static Entrance_Entry display_date_format;
static Entrance_Entry display_time_format;

void
egui_layout_dialog_show() {
	Egui_Settings_Group greeting = {
		"Greeting settings",
		EW_GROUP_VERTICAL,
		2,
		{{ ew_entry_new("Before", NULL, EW_FALSE),
				ENTRANCE_EDIT_KEY_CLIENT_GREETING_BEFORE_STR,
				ENTRY
		},
		{ ew_entry_new("After", NULL, EW_FALSE),
				ENTRANCE_EDIT_KEY_CLIENT_GREETING_AFTER_STR,
				ENTRY
		}}
	};

	Egui_Settings_Group time = {
		"Time/Date format",
		EW_GROUP_VERTICAL,
		2,
		{{ ew_entry_new("Time format", NULL, EW_FALSE),
				ENTRANCE_EDIT_KEY_CLIENT_TIME_FORMAT_STR,
				ENTRY
		},
		{ ew_entry_new("Data format", NULL, EW_FALSE),
				ENTRANCE_EDIT_KEY_CLIENT_TIME_FORMAT_STR,
				ENTRY
		}}
	};
	
	Egui_Settings_Group groups[] = { greeting, time };

	egui_settings_dialog_show("Layout settings", 2, groups);
}

