#include <limits.h>
#include <stdlib.h>
#include <strings.h>
#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include "Egui.h"
#include <stdio.h>


void
egui_x_settings_dialog_show() 
{
	Egui_Settings_Group group_daemon = es_group_new("Daemon Settings", EW_GROUP_VERTICAL);

	Egui_Settings_Item item_attempts = {
		ew_entry_new("X Attempts", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_DAEMON_ATTEMPTS_INT,
		EGUI_TYPE_INT
	};

	Egui_Settings_Item item_xserver = {
		ew_entry_new("X Commandline", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_DAEMON_XSERVER_STR,
		EGUI_TYPE_STR
	};

	es_group_append(group_daemon, &item_attempts);
	es_group_append(group_daemon, &item_xserver);


	Egui_Settings_Group group_client = es_group_new("Client Settings", EW_GROUP_VERTICAL);

	Egui_Settings_Item item_xsession = {
		ew_entry_new("X Session", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_XSESSION_STR,
		EGUI_TYPE_STR
	};

	es_group_append(group_client, &item_xsession);

	Ecore_List *groups =  ecore_list_new();
	if(groups) 
	{
		ecore_list_append(groups, group_daemon);
		ecore_list_append(groups, group_client);

		es_dialog_show("X Settings", groups);
	}
}
