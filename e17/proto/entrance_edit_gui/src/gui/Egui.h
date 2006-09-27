#ifndef _EGUI_H
#define _EGUI_H


#include <Ecore_Data.h>

#include "egui_settings.h"

typedef struct _Egui_Graphics_Selector {
	char *name;
	char *files_path;
	char *preview_edje_part;
	char *dialog_title;
	char *list_title;
	char *entrance_edit_key;

	int use_full_path;
	int show_pointer_options;
} Egui_Graphics_Selector;

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

	Egui_Graphics_Selector egs;

	char *first;
	int newly_created;

} *Egui_Graphics_Dialog;


void egui_theme_dialog_show(void*);
void egui_background_dialog_show(void*);
void egui_layout_dialog_show(void*);

void egui_behavior_dialog_show(void*);
void egui_sessions_dialog_show(void*);
void egui_x_settings_dialog_show(void*);
/*void egui_preview_show(void*);*/

Egui_Graphics_Dialog egui_gd_new(Egui_Graphics_Selector egs);
void egui_gd_show(Egui_Graphics_Dialog egd);


#endif
