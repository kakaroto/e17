#include <limits.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <Egui.h>


void
egui_background_dialog_show()
{
	Egui_Graphics_Selector background_egs = {
		"background",
		"/usr/share/enlightenment/data/backgrounds",
		"desktop/background",
		"Entrance Configuration - Background",
		"Backgrounds",
		ENTRANCE_EDIT_KEY_CLIENT_BACKGROUND_STR,
		1
	};

	egui_gs_dialog_show(background_egs);

}
