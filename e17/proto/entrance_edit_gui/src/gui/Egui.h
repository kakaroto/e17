#ifndef _EGUI_H
#define _EGUI_H


#include <Ecore_Data.h>

#include "egui_graphics_dialog.h"
#include "egui_settings_dialog.h"

void egui_theme_dialog_show(void*);
void egui_background_dialog_show(void*);
void egui_layout_dialog_show(void*);

void egui_behavior_dialog_show(void*);
void egui_sessions_dialog_show(void*);
void egui_x_settings_dialog_show(void*);

#endif
