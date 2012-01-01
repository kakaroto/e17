#ifndef _UI_H
#define _UI_H

void ui_file_open_save_dialog_open(Evas_Object *parent, Evas_Smart_Cb func, Eina_Bool save);

Evas_Object *ui_find_dialog_open(Evas_Object *parent, Evas_Object *entry);

Evas_Object *ui_font_dialog_open(Evas_Object *parent, Evas_Object *entry);

void ui_alert_need_saving(void (*done)(void *), void *data);

#endif
