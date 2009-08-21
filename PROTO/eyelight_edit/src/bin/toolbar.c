
#include "main.h"


static void _tb_open(void *data, Evas_Object *obj, void *event_info);
static void _pres_fileselector_done(void *data, Evas_Object *obj, void *event_info);
static void _tb_slideshow(void *data, Evas_Object *obj, void *event_info);

Evas_Object *toolbar_create()
{
    Evas_Object *tb;

    tb = elm_toolbar_add(win);
    evas_object_size_hint_weight_set(tb, 0.0, 0.0);
    evas_object_size_hint_align_set(tb, -1.0, 0.0);

    Elm_Toolbar_Item *item_empty = elm_toolbar_item_add(tb, NULL, "", NULL, tb);
    elm_toolbar_item_add(tb, NULL, D_("Open a presentation"), _tb_open, item_empty);
    elm_toolbar_item_add(tb, NULL, D_("Slideshow"), _tb_slideshow, item_empty);

    evas_object_show(tb);
    return tb;
}

static void _tb_open(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *inwin, *fs;

    Elm_Toolbar_Item *item_empty= (Elm_Toolbar_Item*) data;
    elm_toolbar_item_select(item_empty);

    setlocale(LC_ALL, "");

    inwin = elm_win_inwin_add(win);
    evas_object_show(inwin);

    fs = elm_fileselector_add(win);
    elm_fileselector_expandable_set(fs, EINA_FALSE);
    /* start the fileselector in the home dir */
    elm_fileselector_path_set(fs, getenv("HOME"));
    /* allow fs to expand in x & y */
    evas_object_size_hint_weight_set(fs, 1.0, 1.0);
    elm_win_resize_object_add(win, fs);
    evas_object_show(fs);
    /* the 'done' cb is called when the user press ok/cancel */
    evas_object_smart_callback_add(fs, "done", _pres_fileselector_done, inwin);

    elm_win_inwin_content_set(inwin, fs);
}

static void _tb_slideshow(void *data, Evas_Object *obj, void *event_info)
{
    Elm_Toolbar_Item *item_empty= (Elm_Toolbar_Item*) data;
    elm_toolbar_item_select(item_empty);

    slideshow_create();
}


static void _pres_fileselector_done(void *data, Evas_Object *obj, void *event_info)
{
   const char *selected = event_info;

   if (selected)
   {
       eyelight_object_presentation_file_set(pres,selected);
       eyelight_object_thumbnails_size_set(pres, 1024/2, 768/2);
       slides_list_update();
       slides_grid_update();
   }

   evas_object_del(data);
}

