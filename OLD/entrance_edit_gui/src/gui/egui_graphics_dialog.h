#ifndef _EGUI_GRAPHICS_DIALOG_H
#define _EGUI_GRAPHICS_DIALOG_H

#include <Ecore_Data.h>

#define EGDS_BACKGROUND 0
#define EGDS_THEME 1

typedef struct {
	char *name;
	char *files_path;
	Ecore_List *extra_file_paths;
	char *dialog_title;
	char *list_title;
	char *entrance_edit_key;

	int use_full_path;
	int show_pointer_options;
	int keep_part;
} Egui_Graphics_Dialog_Settings;

typedef struct {

	Entrance_Dialog win;
	Entrance_Preview img_preview;
	Entrance_Widget pointer_preview;
	Entrance_List list_thumbs;
	Entrance_Entry browse_entry;
	Entrance_Widget browse_button;
	Entrance_Widget pointer_browse_button;
	Entrance_Widget group_graphics;
	Entrance_Widget group_preview;
	Entrance_Widget group_options;
	Entrance_Widget group_pointer;

	Egui_Graphics_Dialog_Settings egds;

	Evas *preview_evas;
	Evas_Object *preview_smart;

	char *first;
	int newly_created;

} *Egui_Graphics_Dialog;

Egui_Graphics_Dialog egui_gd_new(Egui_Graphics_Dialog_Settings egds);
/*TODO: egui_gd_free*/
void egui_gd_show(Egui_Graphics_Dialog egd);


#endif
