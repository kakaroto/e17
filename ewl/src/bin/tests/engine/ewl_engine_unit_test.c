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

#define BUFFER_ENGINE_NAME "evas_buffer"

static int test_engine_set_get(char *buf, int len);

Ewl_Unit_Test engine_unit_tests[] = {
                {"engine set/get", test_engine_set_get, NULL, -1, 1},
                {NULL, NULL, NULL, -1, 0}
        };

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

static int
test_engine_set_get(char *buf, int len)
{
        int ret = 0;
        const char *engine_name;
        Ewl_Widget *window;

        ecore_path_group_add(ewl_engines_path, PACKAGE_LIB_DIR "/ewl/tests");

        window = ewl_window_new();
        ewl_embed_engine_name_set(EWL_EMBED(window), BUFFER_ENGINE_NAME);
        ewl_widget_show(window);
        engine_name = ewl_embed_engine_name_get(EWL_EMBED(window));

        if (strcmp(engine_name, BUFFER_ENGINE_NAME))
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
