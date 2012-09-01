#ifndef EMPOWER_GUI_H
#define EMPOWER_GUI_H

#include "Empower.h"

Eina_Bool gui_init();
void      gui_show(Empower_Auth_Info *info);
void      gui_hide();
Eina_Bool gui_visible_get();

#endif
