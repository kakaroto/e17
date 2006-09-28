#include <limits.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <Egui.h>


void
egui_background_dialog_show(void* data)
{
	Egui_Graphics_Dialog_Settings background_settings = {
		"background",
		"/usr/share/enlightenment/data/backgrounds",
		"desktop/background",
		"Entrance Configuration - Background",
		"Backgrounds",
		ENTRANCE_EDIT_KEY_CLIENT_BACKGROUND_STR,
		1,
		0
	};

	static Egui_Graphics_Dialog egd = NULL;
	if(!egd)
		egd = egui_gd_new(background_settings);
	egui_gd_show(egd);
}
