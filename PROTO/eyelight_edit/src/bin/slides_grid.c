#include "slides_grid.h"

#define SLIDER_ZOOM_MAX 0.5

static Evas_Object *grid = NULL;
static Evas_Object *_sc_rect;

static void _slider_zoom_cb(void *data, Evas_Object *obj, void *event_info);
static void _grid_region_cb(Evas_Object *obj, int *x, int *y, int *w, int *h, void *data);
static void _scroll_move_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _scroll_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _scroll_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _scroll_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _item_move_cb(Evas_Object *obj, int id, int id_after, void *data);
static void _item_menu_cb(void *data, Evas_Object *o, const char *emission, const char *source);


Evas_Object *slides_grid_create()
{
    Evas_Object *vbox, *sl, *sc;
    char buf[PATH_MAX];

    snprintf(buf, sizeof(buf), "%s/theme.edj", PACKAGE_DATA_DIR);

    vbox = elm_box_add(win);
    evas_object_size_hint_weight_set(vbox, 1.0, 1.0);
    evas_object_size_hint_align_set(vbox, -1.0, -1.0);

    //scroll which contains the presentation
    sc = elm_scroller_add(win);
    elm_scroller_content_min_limit(sc, 0, 0);
    evas_object_size_hint_weight_set(sc, 1.0, 1.0);
    evas_object_size_hint_align_set(sc, -1.0, -1.0);
    elm_scroller_policy_set(sc, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_ON);
    evas_object_show(sc);
    elm_box_pack_end(vbox, sc);
    evas_object_event_callback_add(sc, EVAS_CALLBACK_RESIZE, _scroll_move_resize_cb, sc);
    evas_object_event_callback_add(sc, EVAS_CALLBACK_MOVE, _scroll_move_resize_cb, sc);
    //

    //rectangle on top of the scroll which retrieves the mouse events
    _sc_rect = edje_object_add(evas_object_evas_get(win));
    edje_object_file_set(_sc_rect, buf, "presentation/size");
    evas_object_repeat_events_set(_sc_rect,1);
    evas_object_show(_sc_rect);
    evas_object_smart_member_add(_sc_rect, sc);
    evas_object_raise(_sc_rect);
    evas_object_event_callback_add(_sc_rect, EVAS_CALLBACK_MOUSE_DOWN, _scroll_mouse_down_cb, NULL);
    evas_object_event_callback_add(_sc_rect, EVAS_CALLBACK_MOUSE_UP, _scroll_mouse_up_cb, NULL);
    evas_object_event_callback_add(_sc_rect, EVAS_CALLBACK_MOUSE_MOVE, _scroll_mouse_move_cb, NULL);
    //

    //
    grid = grid_object_add(evas_object_evas_get(win));
    grid_object_items_size_set(grid, 1024*0.20, 768*0.20);
    grid_object_region_cb_set(grid, _grid_region_cb, sc);
    grid_object_move_cb_set(grid, _item_move_cb, NULL);
    evas_object_size_hint_weight_set(grid, 1.0, 1.0);
    evas_object_size_hint_align_set(grid, -1.0, -1.0);
    elm_scroller_content_set(sc, grid);
    evas_object_show(grid);
    //

    //the slider which zoom the presentation
    sl = elm_slider_add(win);
    elm_slider_label_set(sl, "Zoom");
    elm_slider_span_size_set(sl, 80);
    elm_slider_indicator_format_set(sl, "%3.0f");
    elm_slider_min_max_set(sl, 0, 100*SLIDER_ZOOM_MAX);
    elm_slider_value_set(sl, 20);
    elm_slider_unit_format_set(sl, "%4.0f");
    evas_object_size_hint_weight_set(sl, -1.0, -1.0);
    evas_object_size_hint_align_set(sl, -1.0, -1.0);
    evas_object_smart_callback_add(sl, "changed", _slider_zoom_cb, sl);
    evas_object_show(sl);
    elm_box_pack_end(vbox, sl);
    //


    evas_object_show(vbox);
    return vbox;
}

void slides_grid_update()
{
    Eina_List *l;
    List_Item *item;
    char buf[PATH_MAX];

    grid_object_freeze(grid, 1);
    grid_object_clear(grid);
    EINA_LIST_FOREACH(l_slides, l, item)
    {
        snprintf(buf, sizeof(buf), "%s/theme.edj", PACKAGE_DATA_DIR);

        Evas_Object *o = edje_object_add(evas_object_evas_get(win));
        item->grid_icon = o;
        edje_object_file_set(o, buf, "list/slides/icon");
        edje_object_signal_callback_add(o, "menu","item",_item_menu_cb, item);
        evas_object_show(o);

        if(item->thumb)
        {
            int w = item->thumb->w;
            int h = item->thumb->h;
            if(item->grid_icon_data) free(item->grid_icon_data);
            int *image = calloc(w*h, sizeof(int));
            item->grid_icon_data = image;
            memcpy(image, item->thumb->thumb, sizeof(int)*w*h);

            const Evas_Object *o_image = edje_object_part_object_get(o, "object.icon");
            evas_object_image_filled_set((Evas_Object*)o_image,1);
            evas_object_color_set((Evas_Object*)o_image,255,255,255,255);
            evas_object_image_size_set((Evas_Object*)o_image, w, h);
            evas_object_image_data_set((Evas_Object*)o_image, image);
        }
        grid_object_item_append(grid, o);
    }

    grid_object_freeze(grid, 0);
}

void slides_grid_append_relative(List_Item *item, List_Item *previous)
{
    char buf[PATH_MAX];

    Evas_Object *o = edje_object_add(evas_object_evas_get(win));
    item->grid_icon = o;
    edje_object_file_set(o, buf, "list/slides/icon");
    edje_object_signal_callback_add(o, "menu","item",_item_menu_cb, item);
    evas_object_show(o);

    if(item->thumb)
    {
        int w = item->thumb->w;
        int h = item->thumb->h;
        if(item->grid_icon_data) free(item->grid_icon_data);
        int *image = calloc(w*h, sizeof(int));
        item->grid_icon_data = image;
        memcpy(image, item->thumb->thumb, sizeof(int)*w*h);

        const Evas_Object *o_image = edje_object_part_object_get(o, "object.icon");
        evas_object_image_filled_set((Evas_Object*)o_image,1);
        evas_object_color_set((Evas_Object*)o_image,255,255,255,255);
        evas_object_image_size_set((Evas_Object*)o_image, w, h);
        evas_object_image_data_set((Evas_Object*)o_image, image);
    }

    if(previous)
        grid_object_item_append_relative(grid, item->grid_icon, previous->grid_icon);
    else
        grid_object_item_prepend(grid, item->grid_icon);
}

void slides_grid_remove(List_Item *item)
{
    grid_object_item_remove(grid, item->grid_icon);
}

void slides_grid_thumb_done_cb(int id_slide)
{
    char buf[PATH_MAX];

    snprintf(buf, sizeof(buf), "%s/theme.edj", PACKAGE_DATA_DIR);

    List_Item *item = eina_list_nth(l_slides, id_slide);

    Evas_Object *o = edje_object_add(evas_object_evas_get(win));
    edje_object_file_set(o, buf, "list/grid/icon");
    evas_object_show(o);
    edje_object_signal_callback_add(o, "menu","item",_item_menu_cb, item);

    item->grid_icon = o;
    if(item->thumb)
    {
        int w = item->thumb->w;
        int h = item->thumb->h;
        if(item->grid_icon_data) free(item->grid_icon_data);
        int *image = calloc(w*h, sizeof(int));
        item->grid_icon_data = image;
        memcpy(image, item->thumb->thumb, sizeof(int)*w*h);

        const Evas_Object *o_image = edje_object_part_object_get(o, "object.icon");
        evas_object_image_filled_set((Evas_Object*)o_image,1);
        evas_object_color_set((Evas_Object*)o_image,255,255,255,255);
        evas_object_image_size_set((Evas_Object*)o_image, w, h);
        evas_object_image_data_set((Evas_Object*)o_image, image);

        edje_object_signal_emit(o, "icon,show", "eyelight");
    }

    grid_object_item_replace(grid, id_slide, o);
}

static void _slider_zoom_cb(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *sl = (Evas_Object*) data;
    char buf[PATH_MAX];

    double val = elm_slider_value_get(sl);
    if(val<1)
    {
        elm_slider_value_set(sl,1);
        val = 1;
    }
    val /= 100.;

    grid_object_items_size_set(grid, 1024*val, 768*val);

    snprintf(buf,PATH_MAX,"%dx%d",(int)(1024*val), (int)(768*val));
    edje_object_part_text_set(_sc_rect,"object.text",buf);
    edje_object_signal_emit(_sc_rect, "text,set","eyelight");
}

static void _grid_region_cb(Evas_Object *obj, int *x, int *y, int *w, int *h, void *data)
{
    Evas_Object *sc = (Evas_Object*) data;

    elm_scroller_region_get(sc,x,y,w,h);
}


static void _scroll_move_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Object *sc = (Evas_Object*) data;
    int x,y,w,h;

    evas_object_geometry_get(sc,&x,&y,&w,&h);
    evas_object_resize(_sc_rect,w,h);
    evas_object_move(_sc_rect,x,y);
}

static void _scroll_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up*) event_info;
    ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
}

static void _scroll_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down*) event_info;
    ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
}

static void _scroll_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move*) event_info;
    ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
}

static void _item_move_cb(Evas_Object *obj, int id, int id_after, void *data)
{
    utils_slide_move(id, id_after);
}

static void _item_menu_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    List_Item *item = (List_Item*) data;
    int x,y,w,h;

    evas_object_geometry_get(item->grid_icon,&x,&y,&w,&h);
    slide_menu_show(item,x+w/2,y+h/2);
}
