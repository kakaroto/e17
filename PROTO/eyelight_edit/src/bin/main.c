/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:      The main source file of eyelight-elm
 *                      Eyelight-elm is an elementary front-end for the library eyelight
 *                      which is a presentation viewer using edje.
 *
 *        Version:  1.0
 *        Created:  06/29/2009 11:23:09 AM EDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "main.h"



Eina_List* l_slides = NULL;

static void win_del(void *data, Evas_Object *obj, void *event_info)
{
    elm_exit();
}

static void win_main(void)
{
    Evas_Object *bg, *tb;

    win = elm_win_add(NULL, "Eyelight", ELM_WIN_BASIC);
    elm_win_title_set(win, "Eyelight - presentation viewer");

    evas_object_smart_callback_add(win, "delete-request", win_del, NULL);

    bg = elm_bg_add(win);
    evas_object_size_hint_weight_set(bg, 1.0, 1.0);
    elm_win_resize_object_add(win, bg);
    evas_object_show(bg);

    tb = elm_table_add(win);
    elm_win_resize_object_add(win, tb);
    evas_object_size_hint_weight_set(tb, 1.0, 1.0);
    evas_object_show(tb);

    Evas_Object *slides_list =slides_list_new();
    elm_table_pack(tb, slides_list, 0, 1, 1, 1);

    Evas_Object *toolbar = toolbar_create();
    elm_table_pack(tb, toolbar, 0, 0, 3, 1);

    Evas_Object *tabpanel = tabpanel_create();
    elm_table_pack(tb, tabpanel, 1, 1, 1, 1);

    Evas_Object *rightpanel = rightpanel_create();
    elm_table_pack(tb, rightpanel, 2, 1, 1, 1);

    evas_object_resize(win, 900, 700);
    evas_object_move(win, 0, 0);
    evas_object_show(win);
}

int elm_main(int argc, char **argv)
{
    Eina_List *l;
    List_Item *item;

    win_main();

    elm_run();

    EINA_LIST_FOREACH(l_slides,l,item)
    {
        if(item->icon_data) free(item->icon_data);
        if(item->grid_icon_data) free(item->grid_icon_data);
        free(item);
    }
    elm_shutdown();
    return 0;
}

ELM_MAIN()
