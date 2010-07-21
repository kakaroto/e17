// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#ifndef TABPANEL_H_
#define TABPANEL_H_


#include <Elementary.h>


typedef struct tabpanel Tabpanel;
typedef struct tabpanel_item Tabpanel_Item;
typedef void (*TabLanelSelectCB)  (void *data, Tabpanel *tabpanel, Tabpanel_Item *item);


Tabpanel *tabpanel_add(Evas_Object *parent);
Evas_Object *tabpanel_tabs_obj_get(Tabpanel *tab);
Evas_Object *tabpanel_panels_obj_get(Tabpanel *tab);
Tabpanel_Item *tabpanel_item_add(Tabpanel *tab, const char *label, Evas_Object *content,
        TabLanelSelectCB select_cb, void *data);
void tabpanel_item_del(Tabpanel_Item *item);
void tabpanel_del(Tabpanel *tab);
void tabpanel_item_select(Tabpanel_Item *item);
void tabpanel_item_label_set(Tabpanel_Item *item, const char *label);

#endif
