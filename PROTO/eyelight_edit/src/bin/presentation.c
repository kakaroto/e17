/*
 * =====================================================================================
 *
 *       Filename:  presentation.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/29/2009 02:41:24 PM EDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "presentation.h"

#define SLIDER_ZOOM_MAX 2 //100*2

static void _slider_zoom_cb(void *data, Evas_Object *obj, void *event_info);
static void _scroll_mouse_wheel_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _scroll_key_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _scroll_move_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _pres_move_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

Evas_Object *_pres_shadow = NULL;
Evas_Object *_pres_rect;
Evas_Object *_pres_vbox;

Evas_Object *presentation_create()
{
    Evas_Object *vbox, *sl, *sc, *box;
    char buf[PATH_MAX];
    int x, y, w, h;

    snprintf(buf, sizeof(buf), "%s/theme.edj", PACKAGE_DATA_DIR);

    vbox = elm_box_add(win);
    evas_object_size_hint_weight_set(vbox, 1.0, 1.0);
    evas_object_size_hint_align_set(vbox, -1.0, -1.0);

    //scroll which contains the presentation
    sc = elm_scroller_add(win);
    elm_scroller_content_min_limit(sc, 0, 0);
    evas_object_size_hint_weight_set(sc, 1.0, 1.0);
    evas_object_size_hint_align_set(sc, -1.0, -1.0);
    elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_ON, ELM_SCROLLER_POLICY_ON);
    evas_object_show(sc);

    //the slider which zoom the presentation
    sl = elm_slider_add(win);
    elm_slider_label_set(sl, "Zoom");
    elm_slider_span_size_set(sl, 80);
    elm_slider_indicator_format_set(sl, "%3.0f");
    elm_slider_min_max_set(sl, 0, 100*SLIDER_ZOOM_MAX);
    elm_slider_value_set(sl, 50);
    elm_slider_unit_format_set(sl, "%4.0f");
    evas_object_size_hint_weight_set(sl, -1.0, -1.0);
    evas_object_size_hint_align_set(sl, -1.0, -1.0);
    evas_object_smart_callback_add(sl, "delay,changed", _slider_zoom_cb, sl);
    evas_object_show(sl);
    //

    //rectangle on top of the scroll which retrieves the mouse wheel  and keys events
    _pres_rect = edje_object_add(evas_object_evas_get(win));
    edje_object_file_set(_pres_rect, buf, "presentation/size");
    evas_object_repeat_events_set(_pres_rect,1);
    evas_object_show(_pres_rect);
    evas_object_smart_member_add(_pres_rect, sc);
    evas_object_raise(_pres_rect);
    evas_object_event_callback_add(_pres_rect, EVAS_CALLBACK_MOUSE_WHEEL, _scroll_mouse_wheel_cb, sl);
    evas_object_event_callback_add(_pres_rect, EVAS_CALLBACK_KEY_DOWN, _scroll_key_down_cb, NULL);

        //TODO: free cpl
    Evas_Object_Couple *cpl = calloc(1,sizeof(Evas_Object_Couple));
    cpl->left = sc;
    cpl->right = _pres_rect;
    evas_object_event_callback_add(sc, EVAS_CALLBACK_RESIZE, _scroll_move_resize_cb, cpl);
    evas_object_event_callback_add(sc, EVAS_CALLBACK_MOVE, _scroll_move_resize_cb, cpl);
    //

    //the presentation
    box = elm_box_add(win);
    _pres_vbox = box;
    evas_object_size_hint_weight_set(box, 1.0, 1.0);
    evas_object_size_hint_align_set(box, -1.0, -1.0);
    evas_object_show(box);
    elm_scroller_content_set(sc, box);

    pres = eyelight_object_add(evas_object_evas_get(win));
    eyelight_object_event_set(pres,1);
    evas_object_size_hint_min_set(pres, 1024/2,768/2);
    evas_object_size_hint_max_set(pres, 1024/2,768/2);
    evas_object_size_hint_align_set(pres, 0.5, 0.5);
    evas_object_event_callback_add(pres, EVAS_CALLBACK_RESIZE, _pres_move_resize_cb, pres);
    evas_object_event_callback_add(pres, EVAS_CALLBACK_MOVE, _pres_move_resize_cb, pres);
    evas_object_show(pres);
    elm_box_pack_end(box, pres);
    //

    //shadow
        // TODO: enable the shadow
    _pres_shadow = edje_object_add(evas_object_evas_get(win));
    edje_object_file_set(_pres_shadow, buf, "presentation/shadow");
    evas_object_smart_member_add(_pres_shadow, pres);
    evas_object_geometry_get(pres,&x,&y,&w,&h);
    evas_object_move(_pres_shadow,x,y);
    evas_object_resize(_pres_shadow,w,h);
    evas_object_show(_pres_shadow);
    evas_object_lower(_pres_shadow);
    evas_object_clip_set(_pres_shadow, box);
    evas_object_repeat_events_set(_pres_shadow,1);
    //

    evas_object_focus_set(_pres_rect, 1);
    elm_box_pack_end(vbox, sc);
    elm_box_pack_end(vbox, sl);

    evas_object_show(vbox);
    return vbox;
}

void presentation_resize(int w, int h)
{
    char buf[PATH_MAX];

    evas_object_size_hint_min_set(pres, w,h);
    evas_object_size_hint_max_set(pres, w,h);

    snprintf(buf,PATH_MAX,"%dx%d",w,h);
    edje_object_part_text_set(_pres_rect,"object.text",buf);
    edje_object_signal_emit(_pres_rect, "text,set","eyelight");
}

static void _slider_zoom_cb(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *sl = (Evas_Object*) data;

    double val = elm_slider_value_get(sl);
    if(val<1)
    {
        elm_slider_value_set(sl,1);
        val = 1;
    }
    val /= 100.;
    presentation_resize(1024*val, 768*val);
}

static void _pres_move_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Object *pres = (Evas_Object*) data;
    int x,y,w,h;

    evas_object_geometry_get(pres,&x,&y,&w,&h);
    evas_object_resize(_pres_shadow,w,h);
    evas_object_move(_pres_shadow,x,y);
}


static void _scroll_move_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Object_Couple *cpl = (Evas_Object_Couple*) data;
    int x,y,w,h;

    evas_object_geometry_get(cpl->left,&x,&y,&w,&h);
    evas_object_resize(cpl->right,w,h);
    evas_object_move(cpl->right,x,y);
}

static void _scroll_key_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Wheel *ev = (Evas_Event_Mouse_Wheel*) event_info;

    //send the event to the presentation
    eyelight_object_event_send(pres, event_info);

    //unset the event
    ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
}

static void _scroll_mouse_wheel_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
     Evas_Event_Mouse_Wheel *ev = (Evas_Event_Mouse_Wheel*) event_info;
     Evas_Object *sl = (Evas_Object*) data;

    //unset the mouse wheel
    ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;

    double value = elm_slider_value_get(sl) + (5*ev->z);
    if(value>SLIDER_ZOOM_MAX*100) value = SLIDER_ZOOM_MAX*100;
    if(value<1) value = 1;
    else value = (int)value - (int)value%5;

    elm_slider_value_set(sl,value);
    presentation_resize(1024*(value/100),768*(value/100));
}
