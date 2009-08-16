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

typedef struct list_item List_Item;

struct list_item {
    int id_slide;
    Eyelight_Thumb *thumb;
    Elm_Genlist_Item *item;
    Evas_Object *icon;
};

List_Item* _l_items = NULL;
int _i_item_idle;
Ecore_Idler* _idle = NULL;
Evas_Object *_sl = NULL;

char *_slides_list_label_get(const void *data, Evas_Object *obj, const char *part);
Evas_Object *_slides_list_icon_get(const void *data, Evas_Object *obj, const char *part);
static Eina_Bool _slides_list_state_get(const void *data, Evas_Object *obj, const char *part);
static void _slides_list_del(const void *data, Evas_Object *obj);
static void _slides_list_sel(void *data, Evas_Object *obj, void *event_info);
int _thumbnails_load_idle(void *data);
static void _slider_zoom_cb(void *data, Evas_Object *obj, void *event_info);

Elm_Genlist_Item_Class itc_slides;
Evas_Object *slides_list;

#define SLIDER_ZOOM_MAX 0.2

Evas_Object *slides_list_new()
{
    Evas_Object *vbox;

    vbox = elm_box_add(win);
    evas_object_size_hint_weight_set(vbox, -1.0, 1.0);
    evas_object_size_hint_align_set(vbox, -1.0, -1.0);
    evas_object_show(vbox);

    // The list of slides
    slides_list = elm_genlist_add(win);
    evas_object_size_hint_weight_set(slides_list, -1.0, 1.0);
    evas_object_size_hint_align_set(slides_list, -1.0, -1.0);
    evas_object_show(slides_list);
    elm_box_pack_end(vbox, slides_list);

    itc_slides.item_style     = "icon_top_text_bottom";
    itc_slides.func.label_get = _slides_list_label_get;
    itc_slides.func.icon_get  = _slides_list_icon_get;
    itc_slides.func.state_get = _slides_list_state_get;
    itc_slides.func.del       = _slides_list_del;

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


void slides_list_update()
{
    int i;

    for(i=0;_l_items && i<eyelight_object_size_get(pres);i++)
    {
        if(_l_items[i].thumb)
        {
            free(_l_items[i].thumb->thumb);
            free(_l_items[i].thumb);
        }
    }

    if(_l_items) free(_l_items);

    _l_items = calloc(eyelight_object_size_get(pres), sizeof(List_Item));
    if(_idle) ecore_idler_del(_idle);
    _i_item_idle = 0;

    elm_genlist_clear(slides_list);
    for(i=0;i<eyelight_object_size_get(pres);i++)
    {
        _l_items[i].id_slide = i;
        _l_items[i].item = elm_genlist_item_append(slides_list, &itc_slides,
                (void*)i,
                NULL,
                ELM_GENLIST_ITEM_NONE,
                _slides_list_sel,
                (void*)i);
    }

    _idle = ecore_idler_add(_thumbnails_load_idle,pres);
}

int _thumbnails_load_idle(void *data)
{
    Evas_Object* pres = (Evas_Object*)data;

    if(_i_item_idle>=eyelight_object_size_get(pres))
    {
        _idle = NULL;
        _i_item_idle = 0;
        return 0;
    }

    _l_items[_i_item_idle].thumb = eyelight_edit_thumbnails_get_new(eyelight_object_pres_get(pres),
            _i_item_idle);
    elm_genlist_item_update(_l_items[_i_item_idle].item);

    _i_item_idle++;
    return 1;
}


char *_slides_list_label_get(const void *data, Evas_Object *obj, const char *part)
{
    int i = (int) data;

    const char* title = eyelight_edit_slide_title_get(eyelight_object_pres_get(pres),i);

    return strdup(title);
}

Evas_Object *_slides_list_icon_get(const void *data, Evas_Object *obj, const char *part)
{
    int i = (int) data;
    char buf[PATH_MAX];

    snprintf(buf, sizeof(buf), "%s/theme.edj", PACKAGE_DATA_DIR);

    Evas_Object *o = edje_object_add(evas_object_evas_get(win));
    _l_items[i].icon = o;
    edje_object_file_set(o, buf, "list/slides/icon");

    double val = elm_slider_value_get(_sl);
    if(val<1)
    {
        elm_slider_value_set(_sl,1);
        val = 1;
    }
    val /= 100.;

    evas_object_size_hint_min_set(o,1024*val,768*val);
    evas_object_size_hint_max_set(o,1024*val,768*val);

    if(_l_items[i].thumb)
    {
        int w = _l_items[i].thumb->w;
        int h = _l_items[i].thumb->h;
        int *image = calloc(w*h, sizeof(int));
        memcpy(image, _l_items[i].thumb->thumb, sizeof(int)*w*h);

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
    int i =(int) data;
    eyelight_object_slide_goto(pres, i);
}

static void _slider_zoom_cb(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *sl = (Evas_Object*) data;
    int i;

    double val = elm_slider_value_get(sl);
    if(val<1)
    {
        elm_slider_value_set(sl,1);
        val = 1;
    }
    val /= 100.;

    for(i=0;i<eyelight_object_size_get(pres);i++)
    {
        evas_object_size_hint_min_set(_l_items[i].icon,1024*val,768*val);
        evas_object_size_hint_max_set(_l_items[i].icon,1024*val,768*val);
        elm_genlist_item_update(_l_items[i].item);
    }
}
