#ifndef _ENNA_BUTTON_H
#define _ENNA_BUTTON_H

Evas_Object    *enna_button_add(Evas * evas);
void            enna_button_cb_add(Evas_Object *obj, void (*func_cb) (void *data), const char *event, void *data);
void            enna_button_label_set(Evas_Object *obj, const char *label);
void            enna_button_icon_set(Evas_Object *obj, const char *icon);

#endif
