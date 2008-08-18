#ifndef GUI_H
#define GUI_H

#include "Exige.h"
#include "exg_callback.h"
#include "exg_conf.h"

void exg_gui_init(Exige *exg);
void exg_text_entry_create(Exige *exg);
void exg_window_geometry_set(Exige *exg);
int exg_eapps_init();
void eapp_display(Exige *exg);

#endif
