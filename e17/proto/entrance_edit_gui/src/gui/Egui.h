#ifndef _EGUI_H
#define _EGUI_H

#define BUTTON 0
#define ENTRY 1
#define LIST 2

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

typedef struct _Egui_Settings_Item {
	void *widget;
	char *entrance_edit_key;
	int type; /*BUTTON, ENTRY, LABEL, or LIST*/
} Egui_Settings_Item;

typedef struct _Egui_Settings_Group {
	char *title;
	int direction;

	int item_count;
	Egui_Settings_Item items[32]; /* It is assfucking nasty, and not good, but it works */
} Egui_Settings_Group;

void egui_theme_dialog_show(void);
void egui_background_dialog_show(void);
void egui_layout_dialog_show(void);

void egui_behavior_dialog_show(void);
void egui_sessions_dialog_show(void);
void egui_x_settings_dialog_show(void);

void egui_gs_dialog_show(Egui_Graphics_Selector);
void egui_settings_dialog_show(char *title, int count, Egui_Settings_Group items[]);

#endif
