#ifndef _EGUI_H
#define _EGUI_H
#include "../config.h"
#include "egui_graphics_dialog.h"

#define EGUI_TYPE_BUTTON 0
#define EGUI_TYPE_ENTRY 1
#define EGUI_TYPE_LIST 2
#define EGUI_TYPE_CHECKBOX 3
#define EGUI_TYPE_INT 4
#define EGUI_TYPE_STR 5

void egui_theme_dialog_show(void*);
void egui_background_dialog_show(void*);
void egui_layout_dialog_show(void*);

void egui_behavior_dialog_show(void*);
void egui_sessions_dialog_show(void*);
void egui_x_settings_dialog_show(void*);

void egui_load_button(void *w, const char *key, int ktype);
void egui_load_entry(void *w, const char *key, int ktype);
void egui_load_checkbox(void *w, const char *key, int ktype);

void egui_save_button(void *w, const char *key, int ktype);
void egui_save_entry(void *w, const char *key, int ktype);
void egui_save_checkbox(void *w, const char *key, int ktype);
void egui_save_list(void *w, const char *key, int ktype);

char* egui_get_current_bg(void);
char* egui_get_current_theme(void);

#endif
