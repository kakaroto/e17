/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_button.h"
#include "ewl_entry.h"
#include "ewl_image.h"

#include <Evas.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIN_NAME "buffer_engine_window"

static int create_test(Ewl_Container *win);
static void cb_configure(Ewl_Widget *w, void *ev, void *data);
static void cb_mouse_down(Ewl_Widget *w, void *ev, void *data);
static void cb_mouse_up(Ewl_Widget *w, void *ev, void *data);
static void cb_mouse_move(Ewl_Widget *w, void *ev, void *data);

extern Ewl_Unit_Test engine_unit_tests[];

void
test_info(Ewl_Test *test)
{
        test->name = "Engine";
        test->tip = "Overrides the engine on a window.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_ADVANCED;
        test->unit_tests = engine_unit_tests;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *pointer_win;
        Ewl_Widget *img;
        Ewl_Widget *o;

        o = ewl_entry_new();
        ewl_text_text_set(EWL_TEXT(o), "The image below is copied from an\n"
                       "evas buffer engine, and reacts because of events\n"
                       "fed to it from the image widget.");
        ewl_entry_editable_set(EWL_ENTRY(o), FALSE);
        ewl_object_fill_policy_set(EWL_OBJECT(o), EWL_FLAG_FILL_HFILL);
        ewl_container_child_append(EWL_CONTAINER(box), o);
        ewl_widget_show(o);

        img = ewl_image_new();
        ewl_container_child_append(EWL_CONTAINER(box), img);
        ewl_object_fill_policy_set(EWL_OBJECT(img), EWL_FLAG_FILL_ALL);
        ewl_callback_append(img, EWL_CALLBACK_MOUSE_DOWN, cb_mouse_down, NULL);
        ewl_callback_append(img, EWL_CALLBACK_MOUSE_UP, cb_mouse_up, NULL);
        ewl_callback_append(img, EWL_CALLBACK_MOUSE_MOVE, cb_mouse_move, NULL);
        ewl_widget_show(img);

        pointer_win = ewl_window_new();
        ewl_widget_name_set(pointer_win, WIN_NAME);
        ewl_object_fill_policy_set(EWL_OBJECT(pointer_win), EWL_FLAG_FILL_ALL);
        ewl_object_size_request(EWL_OBJECT(pointer_win), 64, 64);
        ewl_embed_engine_name_set(EWL_EMBED(pointer_win), "evas_buffer");
        ewl_callback_append(pointer_win, EWL_CALLBACK_CONFIGURE, cb_configure,
                        img);
        ewl_callback_append(pointer_win, EWL_CALLBACK_VALUE_CHANGED,
                        cb_configure, img);
        ewl_widget_show(pointer_win);

        o = ewl_button_new();
        ewl_container_child_append(EWL_CONTAINER(pointer_win), o);
        ewl_widget_show(o);

        return 1;
}

static void
cb_configure(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Evas_Object *eimg;
        int width, height;
        Ewl_Embed *emb = EWL_EMBED(w);
        Ewl_Image *img = EWL_IMAGE(data);

        eimg = img->image;
        width = ewl_object_current_w_get(EWL_OBJECT(emb));
        height = ewl_object_current_h_get(EWL_OBJECT(emb));
        evas_object_image_size_set(eimg, width, height);
        evas_object_image_data_set(eimg, emb->canvas_window);
        evas_object_image_data_update_add(eimg, 0, 0, width, height);
        ewl_object_preferred_inner_size_set(EWL_OBJECT(img), width, height);
}

static void
cb_mouse_down(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        int x, y;
        Ewl_Widget *pointer_win;
        Ewl_Event_Mouse_Down *event = ev;

        pointer_win = ewl_widget_name_find(WIN_NAME);

        x = (int)((double)(event->base.x - CURRENT_X(w)) *
                        ((double)CURRENT_W(pointer_win) / (double)CURRENT_W(w)));
        y = (int)((double)(event->base.y - CURRENT_Y(w)) *
                        ((double)CURRENT_H(pointer_win) / (double)CURRENT_H(w)));

        ewl_embed_mouse_down_feed(EWL_EMBED(pointer_win), event->button,
                        event->clicks, x, y, event->base.modifiers);
}

static void
cb_mouse_up(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        int x, y;
        Ewl_Widget *pointer_win;
        Ewl_Event_Mouse_Up *event = ev;

        pointer_win = ewl_widget_name_find(WIN_NAME);

        x = (int)((double)(event->base.x - CURRENT_X(w)) *
                        ((double)CURRENT_W(pointer_win) / (double)CURRENT_W(w)));
        y = (int)((double)(event->base.y - CURRENT_Y(w)) *
                        ((double)CURRENT_H(pointer_win) / (double)CURRENT_H(w)));

        ewl_embed_mouse_up_feed(EWL_EMBED(pointer_win), event->button, x, y,
                        event->base.modifiers);
}

static void
cb_mouse_move(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        int x, y;
        Ewl_Widget *pointer_win;
        Ewl_Event_Mouse *event = ev;

        pointer_win = ewl_widget_name_find(WIN_NAME);

        x = (int)((double)(event->x - CURRENT_X(w)) *
                        ((double)CURRENT_W(pointer_win) / (double)CURRENT_W(w)));
        y = (int)((double)(event->y - CURRENT_Y(w)) *
                        ((double)CURRENT_H(pointer_win) / (double)CURRENT_H(w)));

        ewl_embed_mouse_move_feed(EWL_EMBED(pointer_win), x, y, event->modifiers);
}
