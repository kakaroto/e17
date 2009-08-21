
#include "main.h"

/**
 * A list of functions used to manage a presentation
 * for example add a slide, delete ...
 */
Evas_Object *_inwin_delete_slide;

static void _utils_slide_delete_cb(void *data, Evas_Object *obj, void *event_info);
static void _utils_slide_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info);

void utils_slide_insert(List_Item *item_prev)
{
    List_Item *item = calloc(1, sizeof(List_Item));

    Eina_List *l;
    List_Item *_item;
    int i_prev = -1;
    if(item_prev)
    {
        i_prev = 0;
        EINA_LIST_FOREACH(l_slides, l, _item)
        {
            if(item_prev == _item)
                break;
            else
                i_prev++;
        }
    }

    if(!item_prev)
        l_slides = eina_list_prepend(l_slides, item);
    else
        l_slides = eina_list_append_relative(l_slides, item, item_prev);

    eyelight_edit_slide_insert(eyelight_object_pres_get(pres), i_prev);

    slides_grid_append_relative(item, item_prev);
    slides_list_slide_append_relative(item, item_prev);
}

void utils_slide_delete(List_Item *item)
{
    int i = 0;
    Eina_List *l;
    List_Item *_item;

    EINA_LIST_FOREACH(l_slides, l, _item)
    {
        if(item == _item)
            break;
        else
            i++;
    }


    Evas_Object *lbl, *tb, *bt, *icon;
    char buf[PATH_MAX];
    snprintf(buf, sizeof(buf), "%s/theme.edj", PACKAGE_DATA_DIR);

    _inwin_delete_slide = elm_win_inwin_add(win);
    evas_object_show(_inwin_delete_slide);
    elm_win_inwin_style_set(_inwin_delete_slide, "minimal");

    tb = elm_table_add(win);
    elm_win_inwin_content_set(_inwin_delete_slide, tb);

    //icon
    icon = edje_object_add(evas_object_evas_get(win));
    edje_object_file_set(icon, buf, "list/slides/icon");

    evas_object_size_hint_min_set(icon,1024*0.20,768*0.20);
    evas_object_size_hint_max_set(icon,1024*0.20,768*0.20);

    if(item->thumb)
    {
        int w = item->thumb->w;
        int h = item->thumb->h;
        int *image = calloc(w*h, sizeof(int));
        memcpy(image, item->thumb->thumb, sizeof(int)*w*h);

        const Evas_Object *o_image = edje_object_part_object_get(icon, "object.icon");
        evas_object_image_filled_set((Evas_Object*)o_image,1);
        evas_object_color_set((Evas_Object*)o_image,255,255,255,255);
        evas_object_image_size_set((Evas_Object*)o_image, w, h);
        evas_object_image_data_set((Evas_Object*)o_image, image);

        edje_object_signal_emit(icon, "icon,show", "eyelight");
    }
    evas_object_show(icon);
    elm_table_pack(tb, icon, 0, 0, 2, 1);
    //

    bt= elm_button_add(win);
    elm_button_label_set(bt, "Invisible button to add a vertical space");
    elm_table_pack(tb, bt, 0, 1, 1, 1);

    lbl = elm_label_add(win);
    elm_label_label_set(lbl, D_("Are you sure you want to delete this slide ?"));
    elm_table_pack(tb, lbl, 0, 2, 2, 1);
    evas_object_show(lbl);

    bt= elm_button_add(win);
    elm_button_label_set(bt, "Invisible button to add a vertical space");
    elm_table_pack(tb, bt, 0, 3, 1, 1);

    bt= elm_button_add(win);
    elm_button_label_set(bt, D_("Yes, Delete the slide"));
    evas_object_smart_callback_add(bt, "clicked", _utils_slide_delete_cb, item);
    evas_object_color_set(bt, 255, 0, 0, 255);
    elm_table_pack(tb, bt, 0, 4, 1, 1);
    evas_object_show(bt);

    bt= elm_button_add(win);
    elm_button_label_set(bt, D_("No, do not delete the slide"));
    evas_object_smart_callback_add(bt, "clicked", _utils_slide_delete_cancel_cb, item);
    elm_table_pack(tb, bt, 1, 4, 1, 1);
    evas_object_show(bt);
}

static void _utils_slide_delete_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
    evas_object_del(_inwin_delete_slide);
}

static void _utils_slide_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
    List_Item *item = (List_Item*) data;
    int i = 0;
    Eina_List *l;
    List_Item *_item;
    List_Item *item_prev = NULL;

    EINA_LIST_FOREACH(l_slides, l, _item)
    {
        if(item == _item)
            break;
        else
        {
            item_prev = _item;
            i++;
        }
    }

    //delete the slide
    eyelight_edit_slide_delete(eyelight_object_pres_get(pres), i);

    //remove the slide from the list
    slides_list_slide_delete(item);

    //remove the slide from the grid
    slides_grid_remove(item);

    //free the item
    l_slides = eina_list_remove(l_slides, item);
    free(item);

    //select an other slide
    if(item_prev)
        elm_genlist_item_selected_set(item_prev->item, 1);
    else if(l_slides)
    {
        List_Item *_item = eina_list_data_get(l_slides);
        elm_genlist_item_selected_set(_item->item,1);
    }

    evas_object_del(_inwin_delete_slide);
}

void utils_slide_move(int id_slide, int id_after)
{
    eyelight_edit_slide_move(eyelight_object_pres_get(pres), id_slide, id_after);

    slides_list_item_move(id_slide, id_after);
}


