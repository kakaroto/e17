#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <Esmart/Esmart_Container.h>

#include "config.h"
#include "eke_gui.h"
#include "eke_gui_edje.h"
#include "eke_gui_edje_item.h"

static void eke_gui_edje_win_del_cb(Ecore_Evas *ee);
static void eke_gui_edje_win_resize_cb(Ecore_Evas *ee);
static void eke_gui_edje_feed_container_scroll_cb(void *data, 
                        Evas_Object *o, const char *src, const char *em);
static void eke_gui_edje_feed_select_cb(void *data, Evas_Object *o, 
                        const char *em, const char *src);
static void eke_gui_edje_quit_cb(void *data, Evas_Object *obj, 
                        const char *em, const char *src);
static void eke_gui_edje_feed_add_cb(void *data, Evas_Object *obj, 
                        const char *em, const char *src);
static void eke_gui_edje_feed_del_cb(void *data, Evas_Object *obj, 
                        const char *em, const char *src);
static void eke_gui_edje_feed_refresh_cb(void *data, Evas_Object *obj, 
                        const char *em, const char *src);

void eke_gui_edje_feed_swap(Eke *eke, Eke_Feed *feed);

typedef struct Eke_Gui_Edje_Feed Eke_Gui_Edje_Feed;
struct Eke_Gui_Edje_Feed
{
    Evas_Object *menu_item;
    Evas_Object *body;
};

int
eke_gui_edje_init(void)
{
    if (!ecore_init()) return 0;
    if (!ecore_evas_init()) return 0;
    if (!edje_init()) return 0;
    return 1;
}

void
eke_gui_edje_shutdown(void)
{
    edje_shutdown();
    ecore_evas_shutdown();
    ecore_shutdown();
}

void
eke_gui_edje_create(Eke *eke)
{
    Ecore_Evas *ee;
    Evas *evas;
    Evas_Object *edje, *o;

    edje_freeze();
    edje_frametime_set(1.0 / 30.0);

    ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, 
                                EKE_GUI_WIDTH, EKE_GUI_HEIGHT);
    ecore_evas_title_set(ee, PACKAGE);
    ecore_evas_name_class_set(ee, PACKAGE, PACKAGE);
    ecore_evas_borderless_set(ee, 0);
    ecore_evas_shaped_set(ee, 0);
    ecore_evas_show(ee);
    eke->gui.edje.ee = ee;

    ecore_evas_callback_delete_request_set(ee, eke_gui_edje_win_del_cb);
    ecore_evas_callback_resize_set(ee, eke_gui_edje_win_resize_cb);

    evas = ecore_evas_get(ee);

    /* the main edje */
    edje = edje_object_add(evas);
    if (!edje_object_file_set(edje, eke->gui.edje.theme, "Main")) {
        printf("Unable to load theme (%s)\n", eke->gui.edje.theme);
        exit(1);
    }
    evas_object_move(edje, 0, 0);
    evas_object_resize(edje, EKE_GUI_WIDTH, EKE_GUI_HEIGHT);
    evas_object_name_set(edje, "ui");
    evas_object_layer_set(edje, 0);
    evas_object_show(edje);

    eke->gui.edje.edje = edje;

    /* the feed list */
    o = esmart_container_new(evas);
    evas_object_name_set(o, "feeds.list");
    esmart_container_padding_set(o, 0, 0, 0, 0);
    esmart_container_spacing_set(o, 0);
    esmart_container_move_button_set(o, 2);
    esmart_container_fill_policy_set(o, CONTAINER_FILL_POLICY_FILL_X);
    esmart_container_direction_set(o, CONTAINER_DIRECTION_VERTICAL);
    edje_object_part_swallow(edje, "feeds.list.items", o);
    evas_object_show(o);

    edje_object_signal_callback_add(edje, "eke,feed,add", "",
                                        eke_gui_edje_feed_add_cb, NULL);
    edje_object_signal_callback_add(edje, "eke,feed,del", "",
                                        eke_gui_edje_feed_del_cb, eke);
    edje_object_signal_callback_add(edje, "eke,feed,refresh", "",
                                        eke_gui_edje_feed_refresh_cb, eke);
    edje_object_signal_callback_add(edje, "eke,quit", "",
                                        eke_gui_edje_quit_cb, NULL);

    edje_thaw();
}

void
eke_gui_edje_feed_register(Eke *eke, Eke_Feed *feed)
{
    Eke_Gui_Edje_Feed *disp;

    disp = NEW(Eke_Gui_Edje_Feed, 1);
    ecore_hash_set(eke->feeds, feed, disp);
}

void
eke_gui_edje_feed_change(Eke *eke, Eke_Feed *feed)
{
    Eke_Gui_Edje_Feed *disp;
    const char *file;
    Eke_Feed_Item *item;
    Evas *evas;
    Evas_Coord w, h;
    Evas_Object *obj = NULL; 
    edje_freeze();

    evas = evas_object_evas_get(eke->gui.edje.edje);
    edje_object_file_get(eke->gui.edje.edje, &file, NULL);

    disp = ecore_hash_get(eke->feeds, feed);
    if (!disp->menu_item) {
        Evas_Object *list;

        disp->menu_item = edje_object_add(evas);
        if (edje_object_file_set(disp->menu_item, file, "feed.list.item")) {
            edje_object_size_min_get(disp->menu_item, &w, &h);
            if ((w > 0) && (h > 0))
                evas_object_resize(disp->menu_item, w, h);
            
            evas_object_data_set(disp->menu_item, "feed", feed);
            edje_object_signal_callback_add(disp->menu_item,
                                "eke,feed,select", "",
                                 eke_gui_edje_feed_select_cb, eke);

            if ((list = evas_object_name_find(evas, "feeds.list")))
                esmart_container_element_append(list, disp->menu_item);
            
            evas_object_show(disp->menu_item);
        } else {
            evas_object_del(disp->menu_item);
            disp->menu_item = NULL;
        }

        disp->body = esmart_container_new(evas);
        esmart_container_padding_set(disp->body, 0, 0, 0, 0);
        esmart_container_spacing_set(disp->body, 0);
        esmart_container_move_button_set(disp->body, 2);
        esmart_container_fill_policy_set(disp->body,
                                            CONTAINER_FILL_POLICY_FILL_X);
        esmart_container_alignment_set(disp->body, CONTAINER_ALIGN_TOP);
        esmart_container_direction_set(disp->body, CONTAINER_DIRECTION_VERTICAL);
        evas_object_show(disp->body);
    }
    edje_object_part_text_set(disp->menu_item, "label", feed->title);
    edje_object_signal_emit(disp->menu_item, "feed,list,item,updated", "");

    /* if there is no current feed displayed, then show this one */
    if ((!edje_object_part_swallow_get(eke->gui.edje.edje, "feed.body"))) 
        eke_gui_edje_feed_swap(eke, feed);

    esmart_container_empty(disp->body);
    ecore_list_first_goto(feed->items);
    edje_object_file_get(eke->gui.edje.edje, &file, NULL);
    while ((item = ecore_list_next(feed->items)) != NULL) {
        if ((obj = eke_gui_edje_item_new(evas, eke->gui.edje.theme, "feed.body.item"))) {
            eke_gui_edje_item_init(obj, item->title, item->date, item->link, item->desc);
            eke_gui_edje_item_size_min_get(obj, &w, &h);
            if ((w > 0) && (h > 0))
                evas_object_resize(obj, w, h);
            evas_object_show(obj);
            esmart_container_element_append(disp->body, obj);
        }
    }
    edje_thaw();
}

void
eke_gui_edje_feed_swap(Eke *eke, Eke_Feed *feed)
{
    char buf[128];
    Evas_Object *part;
    Eke_Gui_Edje_Feed *disp;

    snprintf(buf, sizeof(buf), PACKAGE " -- %s", feed->title);
    ecore_evas_title_set(eke->gui.edje.ee, buf);

    if ((part = edje_object_part_swallow_get(eke->gui.edje.edje, 
                                                    "feed.body"))) {
        edje_object_part_unswallow(eke->gui.edje.edje, part);
        evas_object_hide(part);
    }

    disp = ecore_hash_get(eke->feeds, feed);
    edje_object_signal_emit(disp->menu_item, "feed,list,item,default", "");
    edje_object_part_swallow(eke->gui.edje.edje, "feed.body", disp->body);
    evas_object_show(disp->body);

    edje_object_signal_callback_del(eke->gui.edje.edje, "drag",
                                    "feed.body.scroll", 
                                    eke_gui_edje_feed_container_scroll_cb);
    edje_object_signal_callback_add(eke->gui.edje.edje, "drag",
                                    "feed.body.scroll",
                                    eke_gui_edje_feed_container_scroll_cb,
                                    disp->body);

    eke->current_feed = feed;
}

void
eke_gui_edje_feed_error(Eke *eke, Eke_Feed *feed)
{

    return;
    eke = NULL;
    feed = NULL;
}

void
eke_gui_edje_begin(void)
{
    ecore_main_loop_begin();
}

static void
eke_gui_edje_win_del_cb(Ecore_Evas *ee)
{
    ecore_main_loop_quit();

    return;
    ee = NULL;
}

static void
eke_gui_edje_win_resize_cb(Ecore_Evas *ee)
{
    Evas_Object *o;

    if ((o = evas_object_name_find(ecore_evas_get(ee), "ui"))) {
        int w, h;

        ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
        evas_object_resize(o, w, h);
    }
}

static void
eke_gui_edje_quit_cb(void *data, Evas_Object *obj, 
                        const char *em, const char *src)
{
    ecore_main_loop_quit();

    return;
    data = NULL;
    obj = NULL;
    em = NULL;
    src = NULL;
}

static void
eke_gui_edje_feed_add_cb(void *data, Evas_Object *obj, 
                        const char *em, const char *src)
{
    printf("add cb\n");

    return;
    data = NULL;
    obj = NULL;
    em = NULL;
    src = NULL;
}

static void
eke_gui_edje_feed_del_cb(void *data, Evas_Object *obj, 
                        const char *em, const char *src)
{
    Eke *eke;
    Eke_Feed *feed;
    Eke_Gui_Edje_Feed *disp;
    Evas_Object *list;
    Evas *evas;
    Ecore_List *key_list;

    eke = data;
    feed = eke->current_feed;
    disp = ecore_hash_remove(eke->feeds, feed);

    key_list = ecore_hash_keys(eke->feeds);
    if (key_list) {
        Eke_Feed *new_feed;

        new_feed = ecore_list_first_goto(key_list);
        if (new_feed) eke_gui_edje_feed_swap(eke, new_feed);

        ecore_list_destroy(key_list);
    }

    evas = evas_object_evas_get(eke->gui.edje.edje);
    if ((list = evas_object_name_find(evas, "feeds.list")))
        esmart_container_element_remove(list, disp->menu_item);

    if (disp->menu_item) evas_object_del(disp->menu_item);
    disp->menu_item = NULL;

    if (disp->body) evas_object_del(disp->body);
    disp->body = NULL;

    if (feed) eke_feed_del(feed);
    feed = NULL;

    FREE(disp);

    return;
    data = NULL;
    obj = NULL;
    em = NULL;
    src = NULL;
}

static void
eke_gui_edje_feed_refresh_cb(void *data, Evas_Object *obj, 
                        const char *em, const char *src)
{
    Eke *eke;

    eke = data;
    if (eke->current_feed)
        eke_feed_update_execute(eke->current_feed);

    return;
    obj = NULL;
    em = NULL;
    src = NULL;
}

static void
eke_gui_edje_feed_select_cb(void *data, Evas_Object *o, 
                            const char *em, const char *src)
{
    Eke *eke = NULL;
    Eke_Feed *feed = NULL;

    if ((eke = (Eke*)data)) {
        if ((feed = evas_object_data_get(o, "feed")))
            eke_gui_edje_feed_swap(eke, feed);
    }

    return;
    em = NULL;
    src = NULL;
}

static void
eke_gui_edje_feed_container_scroll_cb(void *data, Evas_Object *o, 
                                    const char *em, const char *src)
{
    double sx = 0.0, sy = 0.0;
    double container_length = 0.0;
    Evas_Object *container = NULL;
    Evas_Coord cw = (Evas_Coord)0.0, ch = (Evas_Coord)0.0;

    if ((container = (Evas_Object*)data)) {
        edje_object_part_geometry_get(o, "feed.body", NULL, NULL, &cw, &ch);
        container_length = esmart_container_elements_length_get(container);
        edje_object_part_drag_value_get(o, src, &sx, &sy);

        switch (esmart_container_direction_get(container)) {
            case CONTAINER_DIRECTION_HORIZONTAL:
                if(container_length > cw) {
                    container_length -= cw;
                    esmart_container_scroll_offset_set(container,
                                              - (int) (sx * container_length));
                }
                break;
            case CONTAINER_DIRECTION_VERTICAL:
                if(container_length > ch) {
                    container_length -= ch;
                    esmart_container_scroll_offset_set(container,
                                              - (int) (sy * container_length));
                }
                break;
            default:
                fprintf(stderr, "Unknown Container Orientation\n");
                break;
        }
    }

    return;
    em = NULL;
}

