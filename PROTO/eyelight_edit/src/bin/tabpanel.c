#include "main.h"


static void _tb_presentation(void *data, Evas_Object *obj, void *event_info);
static void _tb_slidelist(void *data, Evas_Object *obj, void *event_info);

static Evas_Object *_pager;

Evas_Object *tabpanel_create()
{
    Evas_Object *vbox,*tb;

    vbox = elm_box_add(win);
    evas_object_size_hint_weight_set(vbox, 1.0, 1.0);
    evas_object_size_hint_align_set(vbox, -1.0, -1.0);
    evas_object_show(vbox);

    //
    tb = elm_toolbar_add(win);
    evas_object_size_hint_weight_set(tb, 0.0, 0.0);
    evas_object_size_hint_align_set(tb, -1.0, 0.0);
    evas_object_show(tb);
    elm_box_pack_end(vbox, tb);
    //

    //
    _pager = elm_pager_add(win);
    evas_object_size_hint_weight_set(_pager, 1.0, 1.0);
    evas_object_size_hint_align_set(_pager, -1.0, -1.0);
    elm_box_pack_end(vbox, _pager);
    evas_object_show(_pager);

    Evas_Object *presentation = presentation_create();
    Evas_Object *slides_grid = slides_grid_create();

    elm_pager_content_push(_pager, presentation);
    elm_pager_content_push(_pager, slides_grid);
    //

    Elm_Toolbar_Item *item = elm_toolbar_item_add(tb, NULL, D_("Presentation"), _tb_presentation, presentation);
    elm_toolbar_item_add(tb, NULL, D_("Slides list"), _tb_slidelist, slides_grid);
    elm_toolbar_item_select(item);

    return vbox;
}

static void _tb_presentation(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *content = (Evas_Object*) data;
    elm_pager_content_promote(_pager, content);
}

static void _tb_slidelist(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *content = (Evas_Object*) data;
    elm_pager_content_promote(_pager, content);
}
