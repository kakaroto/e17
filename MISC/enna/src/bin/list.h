#ifndef ENNA_LIST_H
#define ENNA_LIST_H

#include "enna.h"

typedef struct _Enna_List_Item Enna_List_Item;

struct _Enna_List_Item
{
    void *sd;
    Evas_Object *o_base;
    unsigned char selected : 1;
    void (*func)(void *data, void *data2);
    void (*func_hilight)(void *data, void *data2);
    void *data;
    void *data2;
};

EAPI Evas_Object *enna_list_add (Evas *evas);
EAPI void enna_list_append(Evas_Object *obj, Evas_Object *item, void (*func) (void *data, void *data2), void (*func_hilight) (void *data, void *data2), void *data, void *data2);
EAPI void enna_list_min_size_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord*h);
EAPI void enna_list_selected_set(Evas_Object *obj, int n);
EAPI int enna_list_selected_get(Evas_Object *obj);
EAPI void enna_list_icon_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
EAPI void enna_list_clear(Evas_Object *obj);
EAPI void enna_list_freeze(Evas_Object *obj);
EAPI void enna_list_thaw(Evas_Object *obj);
EAPI Evas_Object *enna_list_edje_object_get (Evas_Object *obj);
EAPI void * enna_list_selected_data_get(Evas_Object *obj);
EAPI void * enna_list_selected_data2_get(Evas_Object *obj);
EAPI void enna_list_selected_geometry_get(Evas_Object *obj, Evas_Coord *x,
        Evas_Coord *y, Evas_Coord *w, Evas_Coord *h);
EAPI int enna_list_jump_label(Evas_Object *obj, const char *label);
EAPI void enna_list_jump_nth(Evas_Object *obj, int n);
EAPI void enna_list_event_key_down(Evas_Object *obj, void *event_info);
#endif

