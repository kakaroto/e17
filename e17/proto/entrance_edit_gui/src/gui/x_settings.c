#include <limits.h>
#include <stdlib.h>
#include <strings.h>
#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include "Egui.h"
#include <stdio.h>


void
egui_x_settings_dialog_show(void *data) 
{
	Egui_Settings_Group group_daemon = egui_sg_new("Daemon Settings", EW_GROUP_VERTICAL);
	Egui_Settings_Item item_attempts = egui_si_new(
		ew_entry_new("X Attempts", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_DAEMON_ATTEMPTS_INT,
		EGUI_TYPE_INT
	);

	Egui_Settings_Item item_xserver = egui_si_new(
		ew_entry_new("X Commandline", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_DAEMON_XSERVER_STR,
		EGUI_TYPE_STR
	);

	egui_sg_item_append(group_daemon, item_attempts);
	egui_sg_item_append(group_daemon, item_xserver);


	Egui_Settings_Group group_client = egui_sg_new("Client Settings", EW_GROUP_VERTICAL);
	Egui_Settings_Item item_xsession = egui_si_new(
		ew_entry_new("X Session", NULL, EW_FALSE),
		EGUI_TYPE_ENTRY,
		ENTRANCE_EDIT_KEY_CLIENT_XSESSION_STR,
		EGUI_TYPE_STR
	);

	egui_sg_item_append(group_client, item_xsession);

	Ecore_List *groups =  ecore_list_new();
	if(groups) 
	{
		ecore_list_append(groups, group_daemon);
		ecore_list_append(groups, group_client);

		static Egui_Settings_Dialog esd = NULL;
		if(!esd)
			esd = egui_sd_new("X Settings", groups);

		egui_sd_show(esd);
	}
}
