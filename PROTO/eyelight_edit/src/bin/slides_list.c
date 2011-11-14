/*
 * =====================================================================================
 *
 *       Filename:  slides_list.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/29/2009 02:27:47 PM EDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "slides_list.h"

#include <Eyelight_Smart.h>
#include "../../config.h"


static char *_slides_list_label_get(const void *data, Evas_Object *obj, const char *part);
static Evas_Object *_slides_list_icon_get(const void *data, Evas_Object *obj, const char *part);
static Eina_Bool _slides_list_state_get(const void *data, Evas_Object *obj, const char *part);
static void _slides_list_del(const void *data, Evas_Object *obj);
static void _slides_list_sel(void *data, Evas_Object *obj, void *event_info);
static void _slider_zoom_cb(void *data, Evas_Object *obj, void *event_info);
static void _slide_add_cb(void *data, Evas_Object *obj, void *event_info);
static void _item_menu_cb(void *data, Evas_Object *o, const char *emission, const char *source);

static Elm_Genlist_Item_Class itc_slides;
static Evas_Object *slides_list;
static Evas_Object *_sl = NULL;

static Evas_Object *bt_slide_new;

#define SLIDER_ZOOM_MAX 0.2

Evas_Object *slides_list_new()
{
    Evas_Object *vbox, *btn;

    vbox = elm_box_add(win);
    evas_object_size_hint_weight_set(vbox, -1.0, 1.0);
    evas_object_size_hint_align_set(vbox, -1.0, -1.0);
    evas_object_show(vbox);

    //button add a new slide
    btn = elm_button_add(win);
    bt_slide_new = btn;
    elm_button_label_set(btn, D_("Add a new slide"));
    evas_object_size_hint_weight_set(btn, -1.0, -1.0);
    evas_object_size_hint_align_set(btn, -1.0, -1.0);
    evas_object_smart_callback_add(btn, "clicked", _slide_add_cb, _sl);
    elm_box_pack_end(vbox, btn);
    evas_object_show(btn);
    //

    // The list of slides
    slides_list = elm_genlist_add(win);
    evas_object_size_hint_weight_set(slides_list, -1.0, 1.0);
    evas_object_size_hint_align_set(slides_list, -1.0, -1.0);
    evas_object_show(slides_list);
    elm_box_pack_end(vbox, slides_list);

    itc_slides.item_style     = "icon_top_text_bottom";
    itc_slides.func.label_get = _slides_list_label_get;
    itc_slides.func.content_get  = _slides_list_icon_get;
    itc_slides.func.state_get = _slides_list_state_get;
    itc_slides.func.del       = _slides_list_del;
    //

    //the slider which zoom the list
    _sl = elm_slider_add(win);
    elm_slider_label_set(_sl, "Zoom");
    elm_slider_span_size_set(_sl, 140);
    elm_slider_indicator_format_set(_sl, "%3.0f");
    elm_slider_min_max_set(_sl, 0, 100*SLIDER_ZOOM_MAX);
    elm_slider_value_set(_sl, 10);
    elm_slider_unit_format_set(_sl, "%4.0f");
    evas_object_size_hint_weight_set(_sl, -1.0, -1.0);
    evas_object_size_hint_align_set(_sl, -1.0, 1.0);
    evas_object_smart_callback_add(_sl, "delay,changed", _slider_zoom_cb, _sl);
    evas_object_show(_sl);
    elm_box_pack_end(vbox, _sl);
    //

    return vbox;
}

void slides_list_nopres_disabled_set(Eina_Bool disabled)
{
    elm_object_disabled_set(bt_slide_new, disabled);
}

void slides_list_update()
{
    int i;
    List_Item *item;
    Eina_List *l;

    EINA_LIST_FREE(l_slides, item)
        free(item);

    for(i=0;i<eyelight_object_size_get(pres);i++)
        l_slides = eina_list_append(l_slides, calloc(1, sizeof(List_Item)));


    elm_genlist_clear(slides_list);

    EINA_LIST_FOREACH(l_slides, l, item)
    {
        item->item = elm_genlist_item_append(slides_list, &itc_slides,
                item,
                NULL,
                ELM_GENLIST_ITEM_NONE,
                _slides_list_sel,
                item);
    }
}

static char *_slides_list_label_get(const void *data, Evas_Object *obj, const char *part)
{
    Eina_List *l;
    List_Item *l_item;
    int i = 0;

    List_Item *item = (List_Item*) data;
    EINA_LIST_FOREACH(l_slides, l, l_item)
    {
        if(l_item == item)
            break;
        else
            i++;
    }

    const char* title = eyelight_edit_slide_title_get(eyelight_object_pres_get(pres),i);
    if(!title)
        return NULL;
    return strdup(title);
}

static Evas_Object *_slides_list_icon_get(const void *data, Evas_Object *obj, const char *part)
{
    List_Item *item = (List_Item*) data;
    char buf[PATH_MAX];

    snprintf(buf, sizeof(buf), "%s/theme.edj", PACKAGE_DATA_DIR);

    Evas_Object *o = edje_object_add(evas_object_evas_get(win));
    item->icon = o;
    edje_object_file_set(o, buf, "list/slides/icon");
    edje_object_signal_callback_add(o, "menu","item",_item_menu_cb, item);

    double val = elm_slider_value_get(_sl);
    if(val<1)
    {
        elm_slider_value_set(_sl,1);
        val = 1;
    }
    val /= 100.;

    evas_object_size_hint_min_set(o,1024*val,768*val);
    evas_object_size_hint_max_set(o,1024*val,768*val);

    if(item->thumb)
    {
        int w = item->thumb->w;
        int h = item->thumb->h;
        if(item->icon_data) free(item->icon_data);
        int *image = calloc(w*h, sizeof(int));
        item->icon_data = image;
        memcpy(image, item->thumb->thumb, sizeof(int)*w*h);

        const Evas_Object *o_image = edje_object_part_object_get(o, "object.icon");
        evas_object_image_filled_set((Evas_Object*)o_image,1);
        evas_object_color_set((Evas_Object*)o_image,255,255,255,255);
        evas_object_image_size_set((Evas_Object*)o_image, w, h);
        evas_object_image_data_set((Evas_Object*)o_image, image);

        edje_object_signal_emit(o, "icon,show", "eyelight");
    }

    return o;
}

static Eina_Bool _slides_list_state_get(const void *data, Evas_Object *obj, const char *part)
{
    return EINA_FALSE;
}

static void _slides_list_del(const void *data, Evas_Object *obj)
{
    ;
}

static void _slides_list_sel(void *data, Evas_Object *obj, void *event_info)
{
    Eina_List *l;
    List_Item *l_item;
    int i = 0;

    List_Item *item = (List_Item*) data;
    EINA_LIST_FOREACH(l_slides, l, l_item)
    {
        if(l_item == item)
            break;
        else
            i++;
    }
    eyelight_object_slide_goto(pres, i);
    rightpanel_noslide_disabled_set(EINA_FALSE);
}

static void _slider_zoom_cb(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *sl = (Evas_Object*) data;
    Eina_List *l;
    List_Item *item;

    double val = elm_slider_value_get(sl);
    if(val<1)
    {
        elm_slider_value_set(sl,1);
        val = 1;
    }
    val /= 100.;

    EINA_LIST_FOREACH(l_slides, l, item)
    {
        evas_object_size_hint_min_set(item->icon,1024*val,768*val);
        evas_object_size_hint_max_set(item->icon,1024*val,768*val);
        elm_genlist_item_update(item->item);
    }
}

static void _slide_add_cb(void *data, Evas_Object *obj, void *event_info)
{
    Eina_List *l;
    List_Item *item;
    List_Item* item_prev = NULL;
    EINA_LIST_FOREACH(l_slides, l, item)
        item_prev = item;

    utils_slide_insert(item_prev);
}

void slides_list_slide_append_relative(List_Item *item, List_Item *item_prev)
{
    if(item_prev)
        item->item = elm_genlist_item_insert_after(slides_list, &itc_slides,
                item,
                NULL,
                item_prev->item,
                ELM_GENLIST_ITEM_NONE,
                _slides_list_sel,
                item);
    else
        item->item = elm_genlist_item_prepend(slides_list, &itc_slides,
                item,
                NULL,
                ELM_GENLIST_ITEM_NONE,
                _slides_list_sel,
                item);

    elm_genlist_item_selected_set(item->item,1);
}

void slides_list_slide_delete(List_Item *item)
{
    elm_genlist_item_del(item->item);
}

static void _item_menu_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    List_Item *item = (List_Item*) data;
    int x,y,w,h;

    evas_object_geometry_get(item->icon,&x,&y,&w,&h);
    slide_menu_show(item,x+w/2,y+h/2);
}

void slides_list_item_move(int id, int id_after)
{
    List_Item *item, *item_after = NULL;

    item = eina_list_nth(l_slides, id);
    item_after = eina_list_nth(l_slides, id_after);

    //remove the slide from the list
    elm_genlist_item_del(item->item);

    //remove the slide from the list
    l_slides = eina_list_remove(l_slides, item);

    if(item_after)
    {
        item->item = elm_genlist_item_insert_after(slides_list, &itc_slides,
                item,
                NULL,
                item_after->item,
                ELM_GENLIST_ITEM_NONE,
                _slides_list_sel,
                item);
        l_slides = eina_list_append_relative(l_slides, item, item_after);
    }
    else
    {
        item->item = elm_genlist_item_prepend(slides_list, &itc_slides,
                item,
                NULL,
                ELM_GENLIST_ITEM_NONE,
                _slides_list_sel,
                item);
        l_slides = eina_list_prepend(l_slides, item);
    }

    elm_genlist_item_selected_set(item->item, 1);
}

