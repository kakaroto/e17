#include <limits.h>
#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Entrance_Widgets.h>
#include <Entrance_Edit.h>
#include <Egui.h>


void
egui_background_dialog_show(void* data)
{
	Ecore_List *bg_paths = ecore_list_new();
	char e_bg_path[PATH_MAX];

	char* home_dir = getenv("HOME");
	if(home_dir != NULL)
	{
		snprintf(e_bg_path, PATH_MAX, "%s/.e/e/backgrounds", home_dir);
		ecore_list_append(bg_paths, e_bg_path);
	}

	/*TODO: do not add this yet, the preview is still wrong, it should eventually show
	 * currently choosen bg/theme on presently configured theme/bg*/
	/*ecore_list_append(bg_paths, PREFIX "/share/entrance/themes");*/

	Egui_Graphics_Dialog_Settings background_settings = {
		"background",
		/*this is an artifact that will have to go, once ew_*list can handle selected first member automagically*/
		PREFIX "/share/enlightenment/data/backgrounds",
		bg_paths,
		/*"e/desktop/background",*/
		"Entrance Configuration - Background",
		"Backgrounds",
		ENTRANCE_EDIT_KEY_CLIENT_BACKGROUND_STR,
		1,
		0,
		EGDS_THEME
	};

	static Egui_Graphics_Dialog egd = NULL;
	if(!egd)
		egd = egui_gd_new(background_settings);
	egui_gd_show(egd);
}
