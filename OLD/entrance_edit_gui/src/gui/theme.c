#include <limits.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <Egui.h>


void
egui_theme_dialog_show(void *data)
{
	Egui_Graphics_Dialog_Settings theme_settings = {
		"theme",
		PREFIX "/share/entrance/themes",
		NULL,
		/*"Preview",*/
		"Entrance Configuration - Themes",
		"Themes",
		ENTRANCE_EDIT_KEY_CLIENT_THEME_STR,
		0,
		1,
		EGDS_BACKGROUND
	};


	static Egui_Graphics_Dialog egd = NULL;
	if(!egd)
	{
		egd = egui_gd_new(theme_settings);
	}
	egui_gd_show(egd);
}
