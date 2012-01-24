#ifndef _UI_H
#define _UI_H

#include "../mess_header.h"

void ui_file_open_save_dialog_open(Evas_Object *parent, Evas_Smart_Cb func, Eina_Bool save);

Evas_Object *ui_find_dialog_open(Evas_Object *parent, Ecrire_Entry *ent);

Evas_Object *ui_font_dialog_open(Evas_Object *parent, Ecrire_Entry *ent, const char *pfont, int size);

void ui_alert_need_saving(Evas_Object *entry, void (*done)(void *data), void *data);

#endif
