/******************************************************************************
**  Copyright (c) 2006-2010, Calaos. All Rights Reserved.
**
**  This is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 3 of the License, or
**  (at your option) any later version.
**
**  This is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this; if not, write to the Free Software
**  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
**
******************************************************************************/

#include "common.h"
#include "main.h"
#include "draw_object.h"
#include "physic_object.h"
#include "level.h"
#include "level_editor.h"
#include "level_chooser.h"

#ifdef HAVE_ECORE_X
#include <Ecore_X.h>
#endif

int _drawin_log_domain = -1;

static Application *application = NULL;
static Eina_Bool static_drawing = EINA_FALSE;

static const Ecore_Getopt options = 
{
        "eskiss",
        "%prog [options]",
        "0.0.2",
        "(C) 2010 Raoul Hecky",
        "GPL",
        "Eskiss physics game",
        EINA_TRUE,
        {
                ECORE_GETOPT_STORE_STR
                        ('e', "engine", "ecore-evas engine to use."),
                ECORE_GETOPT_CALLBACK_NOARGS
                        ('E', "list-engines", "list ecore-evas engines.",
                        ecore_getopt_callback_ecore_evas_list_engines, NULL),
                ECORE_GETOPT_STORE_DEF_BOOL
                        ('F', "fullscreen", "fullscreen mode.", 1),
                ECORE_GETOPT_CALLBACK_ARGS
                        ('g', "geometry", "geometry to use in x:y:w:h form.", "X:Y:W:H",
                        ecore_getopt_callback_geometry_parse, NULL),
                ECORE_GETOPT_VERSION
                        ('V', "version"),
                ECORE_GETOPT_COPYRIGHT
                        ('R', "copyright"),
                ECORE_GETOPT_LICENSE
                        ('L', "license"),
                ECORE_GETOPT_HELP
                        ('h', "help"),
                ECORE_GETOPT_STORE_STR
                        ('l', "level", "level file to play."),
                ECORE_GETOPT_STORE_DEF_BOOL
                        ('c', "cursor", "show/hide cursor. Usefull for touchscreens", 1),
                ECORE_GETOPT_SENTINEL
        }
};

static void on_ecore_evas_resize(Ecore_Evas* ee)
{
        int w, h;
        ecore_evas_geometry_get(application->ee, NULL, NULL, &w, &h);
        evas_object_image_fill_set(application->background, 0, 0, w, h);
        evas_object_resize(application->background, w, h);
        evas_object_resize(application->main_menu, w, h);
        draw_object_resize(w, h);
}

static void on_close_window(Ecore_Evas* ee)
{
        ecore_main_loop_quit();
}

static Eina_Bool main_signal_exit(void* data, int ev_type, void* ev)
{
        ecore_main_loop_quit();

        return 0;
}

static void move_physic_object(PhysicObject *o, DrawPoint new_pos, DrawPoint center, double rot)
{
        Evas_Object *obj = o->obj;

        //Position
        evas_object_move(obj, new_pos.x, new_pos.y);

        int w, h;
        evas_object_geometry_get(obj, NULL, NULL, &w, &h);

        //Rotation
        Evas_Map *m = (Evas_Map *)evas_object_map_get(obj);
        if (!m) m = evas_map_new(4);

        evas_map_util_points_populate_from_geometry(m, new_pos.x, new_pos.y, w, h, 0);

        evas_map_util_rotate(m, rot, center.x, center.y);

        evas_object_map_enable_set(obj, 1);
        evas_object_map_set(obj, m);
}

static void on_new_object(Evas_Object *new_obj, Eina_List *point_list, void *data)
{
        BrushColor color = default_colors[rand() % BRUSH_COUNT];
        draw_object_color_set(color.r, color.g, color.b, color.a);

        int x, y, w, h;
        evas_object_geometry_get(new_obj, &x, &y, &w, &h);

        PhysicObject *pobj = physic_object_drawing_add(new_obj, drawpoint(x + w / 2, y + h / 2), point_list, static_drawing);

//         physic_object_bounding_box_show(pobj);
        physic_object_move_callback_set(pobj, move_physic_object, NULL);
}

static void key_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
        Evas_Event_Key_Down *ev = (Evas_Event_Key_Down *)event_info;

        if (!ev->string) return;

        if (!strcmp(ev->string, " ") || !strcmp(ev->string, "p"))
                physic_simulation_play_set(!physic_simulation_play_get());

        if (!strcmp(ev->string, "s"))
                static_drawing = !static_drawing;

        if (static_drawing)
                INF("Object will be static");
        else
                INF("Object will be normal");
}

static void key_up_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
//         Evas_Event_Key_Up *ev = (Evas_Event_Key_Down *)event_info;
}

static void editor_done(Level *level, void *data)
{
        edje_object_signal_emit(application->main_menu, "show", "menu");
}

static void game_done(void *data)
{
        edje_object_signal_emit(application->main_menu, "show", "menu");
}

static void play_selected_level(char *level, void *data)
{
        if (!level)
        {
                ERR("Error loading level !");
                game_done(NULL);

                return;
        }

        game_play_level_list(application->evas, application->levels, eina_list_data_find_list(application->levels, level), game_done, NULL);
}

static void main_menu_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
        if (strcmp(source, "menu") != 0) return;

        if (!strcmp(emission, "select,quit"))
        {
                edje_object_signal_emit(application->main_menu, "pin,select,quit", "menu");
                edje_object_signal_emit(application->main_menu, "pin,unselect,play", "menu");
                edje_object_signal_emit(application->main_menu, "pin,unselect,choose_level", "menu");
                edje_object_signal_emit(application->main_menu, "pin,unselect,editor", "menu");
        }
        else if (!strcmp(emission, "select,play"))
        {
                edje_object_signal_emit(application->main_menu, "pin,unselect,quit", "menu");
                edje_object_signal_emit(application->main_menu, "pin,select,play", "menu");
                edje_object_signal_emit(application->main_menu, "pin,unselect,choose_level", "menu");
                edje_object_signal_emit(application->main_menu, "pin,unselect,editor", "menu");
        }
        else if (!strcmp(emission, "select,choose_level"))
        {
                edje_object_signal_emit(application->main_menu, "pin,unselect,quit", "menu");
                edje_object_signal_emit(application->main_menu, "pin,unselect,play", "menu");
                edje_object_signal_emit(application->main_menu, "pin,select,choose_level", "menu");
                edje_object_signal_emit(application->main_menu, "pin,unselect,editor", "menu");
        }
        else if (!strcmp(emission, "select,editor"))
        {
                edje_object_signal_emit(application->main_menu, "pin,unselect,quit", "menu");
                edje_object_signal_emit(application->main_menu, "pin,unselect,play", "menu");
                edje_object_signal_emit(application->main_menu, "pin,unselect,choose_level", "menu");
                edje_object_signal_emit(application->main_menu, "pin,select,editor", "menu");
        }

        if (!strcmp(emission, "clicked,quit"))
        {
                ecore_main_loop_quit();
        }
        else if (!strcmp(emission, "clicked,editor"))
        {
                edje_object_signal_emit(application->main_menu, "hide", "menu");

                Level *level = level_add();

                level_editor_edit(application->evas, level, editor_done, NULL);
        }
        else if (!strcmp(emission, "clicked,play"))
        {
                edje_object_signal_emit(application->main_menu, "hide", "menu");

                char *l;
                EINA_LIST_FREE(application->levels, l)
                        free(l);

                INF("Searching levels...");
                application->levels = level_search_level_files();

                game_play_level_list(application->evas, application->levels, application->levels, game_done, NULL);
        }
        else if (!strcmp(emission, "clicked,choose_level"))
        {
                edje_object_signal_emit(application->main_menu, "hide", "menu");

                char *l;
                EINA_LIST_FREE(application->levels, l)
                        free(l);

                INF("Searching levels...");
                application->levels = level_search_level_files();

                level_chooser_show(application->evas, application->levels, play_selected_level, NULL);
        }
}

int main(int argc, char **argv) 
{
        char *engine = NULL, *level_to_play = NULL;
        int args;
        unsigned char quit_option = 0;
        Eina_Rectangle geometry = {0, 0, 0, 0};
        unsigned char is_fullscreen = 0, has_cursor = 1;

        Ecore_Getopt_Value values[] = 
        {
                ECORE_GETOPT_VALUE_STR(engine),
                ECORE_GETOPT_VALUE_BOOL(quit_option),
                ECORE_GETOPT_VALUE_BOOL(is_fullscreen),
                ECORE_GETOPT_VALUE_PTR_CAST(geometry),
                ECORE_GETOPT_VALUE_BOOL(quit_option),
                ECORE_GETOPT_VALUE_BOOL(quit_option),
                ECORE_GETOPT_VALUE_BOOL(quit_option),
                ECORE_GETOPT_VALUE_BOOL(quit_option),
                ECORE_GETOPT_VALUE_STR(level_to_play),
                ECORE_GETOPT_VALUE_BOOL(has_cursor),
                ECORE_GETOPT_VALUE_NONE
        };

        eina_init();

        eina_init();
        _drawin_log_domain = eina_log_domain_register("eskiss", EINA_COLOR_CYAN);
        if(_drawin_log_domain < 0)
        {
                EINA_LOG_ERR("Enable to create a log domain.");
                exit(1);
        }

        if (!ecore_evas_init())
                return EXIT_FAILURE;

        ecore_app_args_set(argc, (const char **) argv);
        args = ecore_getopt_parse(&options, values, argc, argv);

        application = calloc(1, sizeof(Application));

        if (args < 0)
        {
                ERR("ERROR: could not parse options.\n");
                return EXIT_FAILURE;
        }

        if (quit_option)
                return EXIT_SUCCESS;

        if ((geometry.w <= 0) && (geometry.h <= 0)) 
        {
                geometry.w = DEFAULT_WIDTH;
                geometry.h = DEFAULT_HEIGHT;
        }

        application->ee = ecore_evas_new(engine, 0, 0, geometry.w, geometry.h, NULL);

        if (!application->ee)
        {
                ERR("ERROR: could not construct ecore-evas.\n");
                return EXIT_FAILURE;
        }

        if (is_fullscreen)
                ecore_evas_fullscreen_set(application->ee, EINA_TRUE);

#ifdef HAVE_ECORE_X
        if (!has_cursor && !strcmp(ecore_evas_engine_name_get(application->ee), "software_x11"))
        {
                Ecore_X_Window ewin = ecore_evas_software_x11_window_get(application->ee);
                ecore_x_window_cursor_show(ewin, 0);
        }
#endif
        if (has_cursor)
          ecore_evas_cursor_set(application->ee, PACKAGE_DATA_DIR "/brushes/cursor.png", 2000, 0, 32);

        ecore_evas_show(application->ee);

        ecore_evas_title_set(application->ee, "Eskiss");
        ecore_evas_callback_resize_set(application->ee, on_ecore_evas_resize);
        ecore_evas_callback_delete_request_set(application->ee, on_close_window);
        ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, main_signal_exit, NULL);

        application->evas = ecore_evas_get(application->ee);

        //Create background
        application->background = evas_object_image_add(application->evas);
//         evas_object_color_set(background, 80, 147, 147, 255);
        evas_object_image_file_set(application->background, PACKAGE_DATA_DIR "/backgrounds/background_02.jpg", NULL);
        evas_object_image_fill_set(application->background, 0, 0, geometry.w, geometry.h);
        evas_object_resize(application->background, geometry.w, geometry.h);
        evas_object_move(application->background, 0, 0);
        evas_object_show(application->background);

        application->key_catcher = evas_object_rectangle_add(application->evas);
        evas_object_move(application->key_catcher, 0, 0);
        evas_object_resize(application->key_catcher, geometry.w, geometry.h);
        evas_object_layer_set(application->key_catcher, 1000);
        evas_object_repeat_events_set(application->key_catcher, EINA_TRUE);
        evas_object_color_set(application->key_catcher, 0, 0, 0, 0);
        evas_object_focus_set(application->key_catcher, EINA_TRUE);
        evas_object_show(application->key_catcher);

        evas_object_event_callback_add(application->key_catcher, EVAS_CALLBACK_KEY_DOWN, key_down_cb, NULL);
        evas_object_event_callback_add(application->key_catcher, EVAS_CALLBACK_KEY_UP, key_up_cb, NULL);

        srand(time(NULL));

        physic_init();

        level_init();

        edje_init();
        edje_frametime_set(1.0 / 60.0);

        application->main_menu = edje_object_add(application->evas);
        edje_object_file_set(application->main_menu, PACKAGE_DATA_DIR "/theme/default.edj", "main/menu");
        edje_object_signal_callback_add(application->main_menu, "*", "menu", main_menu_cb, NULL);
        evas_object_resize(application->main_menu, geometry.w, geometry.h);
        evas_object_move(application->main_menu, 0, 0);

        if (level_to_play)
        {
                Level *level;
                level_load_file(&level, level_to_play);
                game_play_level(application->evas, level, NULL, EINA_FALSE);
        }
        else
        {
                evas_object_show(application->main_menu);
        }

        ecore_main_loop_begin();

        char *l;
        EINA_LIST_FREE(application->levels, l)
                free(l);

        level_shutdown();

        physic_uninit();
        draw_object_del();

        ecore_evas_free(application->ee);

        edje_shutdown();
        ecore_evas_shutdown();
        ecore_shutdown();
        evas_shutdown();
        free(application);

        return EXIT_SUCCESS;
}
