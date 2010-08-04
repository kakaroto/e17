#include "main.h"

static Evas_Object *hover = NULL;
static Evas_Object *icon_hover_placement = NULL;
static Evas_Object *icon = NULL;

static void _delete_cb(void *data, Evas_Object *obj, void *event_info);
static void _insert_before_cb(void *data, Evas_Object *obj, void *event_info);
static void _insert_after_cb(void *data, Evas_Object *obj, void *event_info);

void slide_menu_show(List_Item *item, int x, int y)
{
    char buf[PATH_MAX];
    snprintf(buf, sizeof(buf), "%s/theme.edj", PACKAGE_DATA_DIR);

    //center
    icon = edje_object_add(evas_object_evas_get(win));
    edje_object_file_set(icon, buf, "list/slides/icon");

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
    //

    // left
    Evas_Object *left = elm_box_add(win);
    Evas_Object *btn = elm_button_add(win);
    evas_object_smart_callback_add(btn, "clicked", _delete_cb, item);
    elm_button_label_set(btn, "Delete");
    evas_object_show(btn);
    elm_box_pack_end(left, btn);
    evas_object_color_set(left, 255, 0, 0, 255);
    //

    // bottom
    Evas_Object *bottom = elm_box_add(win);
    btn = elm_button_add(win);
    elm_button_label_set(btn, "Insert after");
    evas_object_smart_callback_add(btn, "clicked", _insert_after_cb, item);
    evas_object_show(btn);
    elm_box_pack_end(bottom, btn);
    //

    // top
    Evas_Object *top = elm_box_add(win);
    btn = elm_button_add(win);
    elm_button_label_set(btn, "Insert before");
    evas_object_smart_callback_add(btn, "clicked", _insert_before_cb, item);
    evas_object_show(btn);
    elm_box_pack_end(top, btn);
    //

    //set the width of the hover
    int w_top,w_bottom,w_center,h_center;
    int h_top, h_bottom;
    evas_object_geometry_get(top,NULL,NULL,&w_top,&h_top);
    evas_object_geometry_get(bottom,NULL,NULL,&w_bottom,&h_bottom);
    w_center = (w_top>w_bottom?w_top:w_bottom);
    h_center = 768/(1024/w_center);
    evas_object_size_hint_min_set(icon,w_center,h_center);
    evas_object_size_hint_max_set(icon,w_center,h_center);
    evas_object_size_hint_min_set(top,w_center,h_top);
    evas_object_size_hint_min_set(bottom,w_center,h_bottom);

    //now move the hover to prevent a button to be out of the window
    int w_left, h_left;
    evas_object_geometry_get(left, NULL, NULL, &w_left, &h_left);
    int w_win, h_win;
    evas_object_geometry_get(win, NULL, NULL, &w_win, &h_win);

    //left
    if(x-w_center/2-w_left < 0)
        x = x-(x-w_center/2-w_left);
    //right
    if(x+w_center/2>w_win)
        x = x - (x+w_center/2-w_win);
    //top
    if(y-h_center/2-h_top<0)
        y = y - (y-h_center/2-h_top);
    //bottom
    if(y+h_center/2+h_bottom>h_win)
        y = y - (y+h_center/2+h_bottom - h_win);

    // where the hover will be placed
    icon_hover_placement = elm_icon_add(win);
    evas_object_move(icon_hover_placement, x-1,y-1);
    evas_object_resize(icon_hover_placement, 2, 2);
    //



    // hover
    hover = elm_hover_add(win);
    elm_hover_parent_set(hover, win);
    elm_hover_target_set(hover, icon_hover_placement);
    elm_object_style_set(hover, "popout");
    elm_hover_content_set(hover, "middle", icon);
    elm_hover_content_set(hover, "left", left);
    elm_hover_content_set(hover, "bottom", bottom);
    elm_hover_content_set(hover, "top", top);
    evas_object_show(hover);
    //
}

static void _delete_cb(void *data, Evas_Object *obj, void *event_info)
{
    List_Item *item = (List_Item*) data;
    evas_object_del(hover);
    evas_object_del(icon_hover_placement);
    evas_object_del(icon);

    utils_slide_delete(item);
}

static void _insert_before_cb(void *data, Evas_Object *obj, void *event_info)
{
    List_Item *item = (List_Item*) data;
    evas_object_del(hover);
    evas_object_del(icon_hover_placement);
    evas_object_del(icon);

    Eina_List *l;
    List_Item *_item;
    List_Item *item_prev = NULL;
    EINA_LIST_FOREACH(l_slides, l, _item)
    {
        if(_item == item)
            break;
        else
            item_prev = _item;
    }

    utils_slide_insert(item_prev);
}

static void _insert_after_cb(void *data, Evas_Object *obj, void *event_info)
{
    List_Item *item = (List_Item*) data;
    evas_object_del(hover);
    evas_object_del(icon_hover_placement);
    evas_object_del(icon);

    utils_slide_insert(item);
}

