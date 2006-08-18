#ifndef _EGUI_SETTINGS_H
#define _EGUI_SETTINGS_H

#define EGUI_TYPE_BUTTON 0
#define EGUI_TYPE_ENTRY 1
#define EGUI_TYPE_LIST 2
#define EGUI_TYPE_CHECKBOX 3
#define EGUI_TYPE_INT 4
#define EGUI_TYPE_STR 5

typedef struct _Egui_Settings_Item {
	void *widget;
	int widget_type;
	const char *entrance_edit_key;
	int key_type; /*BUTTON, ENTRY, LABEL, or LIST*/
} Egui_Settings_Item;

typedef struct _Egui_Settings_Group {
	const char *title;
	int direction;
	Ecore_List *items;
} *Egui_Settings_Group;

void es_dialog_show(char *title, Ecore_List *groups);

Egui_Settings_Group es_group_new(const char* name, int direction);
void es_group_append(Egui_Settings_Group g, Egui_Settings_Item *i);

#endif
