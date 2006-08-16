#include <limits.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <Egui.h>


void
egui_theme_dialog_show()
{
	Egui_Graphics_Selector theme_egs = {
		"theme",
		"/usr/share/entrance/themes",
		"Preview",
		"Entrance Configuration - Themes",
		"Themes",
		ENTRANCE_EDIT_KEY_CLIENT_THEME_STR,
		0,
		1
	};

	egui_gs_dialog_show(theme_egs);
}
