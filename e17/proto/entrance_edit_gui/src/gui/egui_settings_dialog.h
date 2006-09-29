#ifndef _EGUI_SETTINGS_DIALOG_H
#define _EGUI_SETTINGS_DIALOG_H

#define EGUI_TYPE_BUTTON 0
#define EGUI_TYPE_ENTRY 1
#define EGUI_TYPE_LIST 2
#define EGUI_TYPE_CHECKBOX 3
#define EGUI_TYPE_INT 4
#define EGUI_TYPE_STR 5

#include <Entrance_Widgets.h>

typedef struct {
	void *widget; 
	int widget_type; /*BUTTON, ENTRY, LABEL, or LIST*/
	const char *entrance_edit_key;
	int key_type; 
} *Egui_Settings_Item;

Egui_Settings_Item egui_si_new(void *widget, int widget_type, const char *entrance_edit_key, int key_type);

typedef struct {
	const char *title;
	int direction;
	Ecore_List *items;
} *Egui_Settings_Group;


Egui_Settings_Group egui_sg_new(const char* name, int direction);
void egui_sg_item_append(Egui_Settings_Group g, Egui_Settings_Item i);

typedef struct {
	Entrance_Dialog win;
	Ecore_List *group_list;
	Ecore_List *item_list;

	Ecore_List *settings_groups;
	char* title;

} *Egui_Settings_Dialog;

Egui_Settings_Dialog egui_sd_new(char *title, Ecore_List *groups);
void egui_sd_show(Egui_Settings_Dialog esd);

#endif
