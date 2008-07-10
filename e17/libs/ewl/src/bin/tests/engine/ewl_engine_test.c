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

static int create_test(Ewl_Container *win);
static void cb_configure(Ewl_Widget *w, void *ev, void *data);
static void cb_mouse_down(Ewl_Widget *w, void *ev, void *data);
static void cb_mouse_up(Ewl_Widget *w, void *ev, void *data);
static void cb_mouse_move(Ewl_Widget *w, void *ev, void *data);

static int engine_test_set_get(char *buf, int len);

#define WIN_NAME "buffer_engine_window"

static Ewl_Unit_Test engine_unit_tests[] = {
                {"engine set/get", engine_test_set_get, NULL, -1, 1},
                {NULL, NULL, NULL, -1, 0}
        };

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

/*
static void *window_funcs[EWL_ENGINE_WINDOW_MAX] =
        {
                engine_window_new,
                engine_window_destroy,
                engine_window_move,
                engine_window_resize,
                engine_window_min_max_size_set,
                engine_window_show,
                engine_window_hide,
                engine_window_title_set,
                engine_window_name_class_set,
                engine_window_borderless_set,
                engine_window_dialog_set,
                engine_window_states_set,
                engine_window_hints_set,
                engine_window_transient_for,
                engine_window_leader_set,
                engine_window_raise,
                engine_window_lower,
                engine_keyboard_grab,
                engine_keyboard_ungrab,
                engine_pointer_grab,
                engine_pointer_ungrab,
                engine_window_selection_text_set,
                engine_window_geometry_set,
                engine_dnd_aware_set,
                engine_desktop_size_get,
                engine_dnd_drag_types_set,
                engine_dnd_drag_begin,
                engine_dnd_drag_drop,
                engine_dnd_drag_data_send,
        };

static void *pointer_funcs[EWL_ENGINE_POINTER_MAX] =
        {
                engine_pointer_data_new,
                engine_pointer_free,
                engine_pointer_get,
                engine_pointer_set,
        };
        */

static Ewl_Widget *
create_unit_test_window(void)
{
        Ewl_Widget *window;

        window = ewl_window_new();
        ewl_embed_engine_name_set(EWL_EMBED(window), "ewl_engine_test");
        ewl_widget_show(window);

        return window;
}

static int
engine_test_set_get(char *buf, int len)
{
        int ret = 0;
        const char *engine_name;
        Ewl_Widget *window;

        ecore_path_group_add(ewl_engines_path, PACKAGE_LIB_DIR "/ewl/tests");

        window = create_unit_test_window();
        engine_name = ewl_embed_engine_name_get(EWL_EMBED(window));

        if (strcmp(engine_name, "ewl_engine_test"))
                LOG_FAILURE(buf, len, "engine name doesn't match");
        else
                ret = 1;

        ewl_widget_destroy(window);

        return ret;
}

/*
static int
engine_init(Ewl_Engine *engine, int *argc, char **argv)
{
        info = NEW(Ewl_Engine_Info, 1);
        info->shutdown = engine_shutdown;
        info->hooks.window = window_funcs;
        info->hooks.pointer = pointer_funcs;

        engine->functions = info;

        return TRUE;
}

static void
ee_shutdown(Ewl_Engine *engine)
{
}
*/
