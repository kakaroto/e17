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
    ecore_evas_borderless_set(ee, 0);
    ecore_evas_shaped_set(ee, 0);
    ecore_evas_show(ee);

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
    esmart_container_fill_policy_set(o, CONTAINER_FILL_POLICY_NONE);
    esmart_container_direction_set(o, CONTAINER_DIRECTION_VERTICAL);
    edje_object_part_swallow(edje, "feeds.list.items", o);
    evas_object_show(o);

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
    Evas_Object *part;
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
        edje_object_file_set(disp->menu_item, file, "feed.list.item");

        edje_object_size_min_get(disp->menu_item, &w, &h);
        if ((w > 0) && (h > 0))
            evas_object_resize(disp->menu_item, w, h);

        list = evas_object_name_find(evas, "feeds.list");
        esmart_container_element_append(list, disp->menu_item);
        evas_object_show(disp->menu_item);

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

    if((part = edje_object_part_swallow_get(eke->gui.edje.edje, 
                                                    "feed.body"))) {
        edje_object_part_unswallow(eke->gui.edje.edje, part);
    }
    edje_object_part_swallow(eke->gui.edje.edje, "feed.body", disp->body);
    edje_object_part_geometry_get(eke->gui.edje.edje, "feed.body",
                                                        NULL, NULL, &w, &h);

    esmart_container_empty(disp->body);
    ecore_list_goto_first(feed->items);
    edje_object_file_get(eke->gui.edje.edje, &file, NULL);
    while ((item = ecore_list_next(feed->items)) != NULL) {
        if((obj = eke_gui_edje_item_new(evas, eke->gui.edje.theme, "feed.body.item"))) {
            eke_gui_edje_item_init(obj, item->title, item->link, item->desc);
            evas_object_resize(obj, w, 150);
            evas_object_show(obj);
            esmart_container_element_append(disp->body, obj);
        }
    }
    edje_thaw();
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


