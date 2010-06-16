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

#include "game.h"

enum { GAME_STOP, GAME_PLAY, GAME_WIN, GAME_LEVEL_NEXT, GAME_QUIT };

static Evas *evas = NULL;
static Level *level = NULL; //current level
static int play_state = GAME_STOP;
static Eina_List *no_physic_list = NULL;
static Eina_List *drawn_objects = NULL;

static PhysicObject *ball = NULL, *goal = NULL;

static Eina_List *levels_list = NULL, *level_nth = NULL;
static Game_Win_Cb win_callback = NULL;
static Evas_Object *win_dialog = NULL;
static int win_dialog_action = 0;

static Evas_Object *menu_button = NULL; //the menu button to quit the current level
static Eina_Bool want_menu_button = EINA_FALSE;

static void (*game_play_done)(void *data) = NULL;
static void *game_end_data = NULL;

static void _move_physic_object(PhysicObject *o, DrawPoint new_pos, DrawPoint center, double rot);
static void _on_new_object(Evas_Object *new_obj, Eina_List *point_list, void *data);
static void _collision_cb(PhysicObject *object1, PhysicObject *object2, void *data);

static void _mouse_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _add_ball(void);
static void _add_goal(void);

static void _win_callback(Level *level);
static void _win_dialog_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _menu_button_click(void *data, Evas_Object *obj, const char *emission, const char *source);

/**
 * @brief play a list of level
 * @param list list of eet files to be played
 * @param start_at start at the nth level
 */
void game_play_level_list(Evas *e, Eina_List *list, Eina_List *start_at, void (*play_done)(void *data), void *data)
{
        levels_list = list;
        level_nth = start_at;

        game_play_done = play_done;
        game_end_data = data;

        if (!level_nth) level_nth = levels_list;

        Level *l = NULL;
        if (!level_load_file(&l, eina_list_data_get(level_nth)))
        {
                ERR("Can't load level %s !", (char *)eina_list_data_get(level_nth));

                return;
        }

        game_play_level(e, l, _win_callback, EINA_TRUE);
}

/**
 * @brief play a level
 * @param evas the canvas
 * @param level the level we to play
 */
void game_play_level(Evas *e, Level *lvl, Game_Win_Cb cb, Eina_Bool _want_menu_button)
{
        if (!e)
        {
                ERR("evas is NULL");
                return;
        }
        if (!lvl)
        {
                ERR("level is NULL");
                return;
        }
        if (play_state != GAME_STOP)
        {
                WARN("Game is running, will stop first.");
                game_stop_level();
        }

        level = lvl;
        evas = e;
        win_callback = cb;

        draw_object_add(evas);

        Eina_List *l;
        LevelObject *lo;
        EINA_LIST_FOREACH(level->objects, l, lo)
        {
                //create evas object from points
                Evas_Object *eobj = draw_object_create(e, lo->point_list, lo->color);

                //Add object to physic world
                if (lo->has_physic)
                {
                        int x, y, w, h;
                        evas_object_geometry_get(eobj, &x, &y, &w, &h);

                        PhysicObject *pobj = physic_object_drawing_add(eobj, drawpoint(x + w / 2, y + h / 2), lo->point_list, lo->static_object);
//                         physic_object_bounding_box_show(pobj);
                        physic_object_move_callback_set(pobj, _move_physic_object, NULL);
                }
                else
                {
                        //Add object to non moveable list
                        no_physic_list = eina_list_append(no_physic_list, eobj);
                }
        }

        //Add ball to game.
        _add_ball();

        //Add goal to game
        _add_goal();

        draw_object_enabled_set(EINA_TRUE);
        draw_object_brush_set(default_colors[rand() % BRUSH_COUNT]);
        draw_object_new_callback_add(_on_new_object, NULL);

        physic_object_collision_callback_set(goal, ball, _collision_cb, NULL);

        physic_simulation_play_set(EINA_TRUE);

        play_state = GAME_PLAY;

        want_menu_button = _want_menu_button;
        if (want_menu_button)
        {
                int bg_width = 0, bg_height = 0;
                ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas), NULL, NULL, &bg_width, &bg_height);

                menu_button = edje_object_add(evas);
                edje_object_file_set(menu_button, PACKAGE_DATA_DIR "/theme/default.edj", "main/game/menu_button");
                evas_object_resize(menu_button, bg_width, bg_height);
                edje_object_signal_callback_add(menu_button, "click", "menu", _menu_button_click, NULL);
                evas_object_move(menu_button, 0, 0);
                evas_object_show(menu_button);

                edje_object_signal_emit(menu_button, "show", "eskiss");
        }
}

/**
 * @brief stop playing
 */
void game_stop_level(void)
{
        if (play_state != GAME_PLAY && play_state != GAME_WIN)
        {
                WARN("Game is not running");

                return;
        }

        draw_object_new_callback_del(_on_new_object, NULL);
        physic_simulation_play_set(EINA_FALSE);

        play_state = GAME_STOP;
        level = NULL;

        physic_simulation_clear();

        Evas_Object *o;
        EINA_LIST_FREE(no_physic_list, o)
                evas_object_del(o);

        if (menu_button)
                evas_object_del(menu_button);
        menu_button = NULL;
}

/**
 * @brief restart the current level
 */
void game_restart_level(void)
{
        if (play_state != GAME_PLAY && play_state != GAME_WIN)
        {
                WARN("Game is not running, can't restart level");
                return;
        }
        if (!evas || !level)
        {
                ERR("evas or level is NULL");
                return;
        }

        Evas *e = evas;
        Level *l = level;

        game_stop_level();
        game_play_level(e, l, win_callback, want_menu_button);
}

// Private -------------------------------------

static void _move_physic_object(PhysicObject *o, DrawPoint new_pos, DrawPoint center, double rot)
{
        if (!evas)
        {
                ERR("evas is NULL");
                return;
        }
        if (!level)
        {
                ERR("level is NULL");
                return;
        }

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

        int bg_width = 0, bg_height = 0;
        ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas), NULL, NULL, &bg_width, &bg_height);

        if (new_pos.x < 0 - w - 50 || new_pos.x > bg_width + 50 ||
            new_pos.y < 0 - h - 50 || new_pos.y > bg_height + 50)
        {
                INF("Object out of view, removing it.");

                physic_object_del(o);

                if (o == ball)
                {
                        INF("Redraw ball to start position.");

                        if (play_state != GAME_WIN)
                        {
                                _add_ball();

                                physic_object_collision_callback_set(goal, ball, _collision_cb, NULL);
                        }
                }
        }
}

static void _on_new_object(Evas_Object *new_obj, Eina_List *point_list, void *data)
{
        draw_object_brush_set(default_colors[rand() % BRUSH_COUNT]);

        int x, y, w, h;
        evas_object_geometry_get(new_obj, &x, &y, &w, &h);

        PhysicObject *pobj = physic_object_drawing_add(new_obj, drawpoint(x + w / 2, y + h / 2), point_list, EINA_FALSE);
//         physic_object_bounding_box_show(pobj);
        physic_object_move_callback_set(pobj, _move_physic_object, NULL);

        drawn_objects = eina_list_append(drawn_objects, pobj);

        evas_object_event_callback_add(new_obj, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down_cb, NULL);
}

static void _collision_cb(PhysicObject *object1, PhysicObject *object2, void *data)
{
        if (play_state == GAME_WIN) return; //can't win more than once !
        play_state = GAME_WIN;

        INF("You win !");
        if (win_callback)
                win_callback(level);

        if (goal)
                edje_object_signal_emit(goal->obj, "hide", "goal");
}

static void _mouse_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
        Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *)event_info;

        if (ev->flags & EVAS_BUTTON_DOUBLE_CLICK) //remove object if double click
        {
                Eina_List *l;
                PhysicObject *pobj;
                EINA_LIST_FOREACH(drawn_objects, l, pobj)
                {
                        if (pobj->obj == obj)
                        {
                                drawn_objects = eina_list_remove(drawn_objects, pobj);
                                physic_object_del(pobj);

                                INF("Object removed");

                                return;
                        }
                }
        }
}

void _add_ball(void)
{
        int w, h;

        Evas_Object *b = edje_object_add(evas);
        edje_object_file_set(b, PACKAGE_DATA_DIR "/theme/default.edj", "main/ball");
        edje_object_size_min_get(b, &w, &h);
        evas_object_resize(b, w, h);
        evas_object_move(b, level->ball.x, level->ball.y);
        evas_object_show(b);

        int radius = atoi(edje_object_data_get(b, "radius"));
        char **array = eina_str_split(edje_object_data_get(b, "center"), ",", 2);
        ball = physic_object_circle_add(b, drawpoint(level->ball.x + atoi(array[0]), level->ball.y + atoi(array[1])), radius, EINA_FALSE);
//         physic_object_bounding_box_show(ball);
        physic_object_move_callback_set(ball, _move_physic_object, NULL);
        free(array[0]);
        free(array);
}

void _add_goal(void)
{
        int w, h;

        Evas_Object *g = edje_object_add(evas);
        edje_object_file_set(g, PACKAGE_DATA_DIR "/theme/default.edj", "main/goal");
        edje_object_size_min_get(g, &w, &h);
        evas_object_resize(g, w, h);
        evas_object_move(g, level->goal.x, level->goal.y);
        evas_object_show(g);

        //read points from edje file
        char **array = eina_str_split(edje_object_data_get(g, "points"), " ", 999);

        Eina_List *plist = NULL;
        char *cvalue = array[0];
        int cpt = 0;
        while (cvalue)
        {
                char **array2 = eina_str_split(cvalue, ",", 2);
                DrawPoint *p = calloc(1, sizeof(DrawPoint));

                p->x = level->goal.x + atoi(array2[0]);
                p->y = level->goal.y + atoi(array2[1]);

                free(array2[0]);
                free(array2);

                plist = eina_list_append(plist, p);

                cpt++;
                cvalue = array[cpt];
        }
        free(array[0]);
        free(array);

        int thickness = atoi(edje_object_data_get(g, "thickness"));
        goal = physic_object_segment_add(g, drawpoint(level->goal.x + w / 2, level->goal.y + h / 2), plist, thickness, EINA_TRUE);
        //physic_object_bounding_box_show(goal);
        physic_object_move_callback_set(goal, _move_physic_object, NULL);

        DrawPoint *p;
        EINA_LIST_FREE(plist, p)
                free(p);
}

static void _win_callback(Level *level)
{
        draw_object_enabled_set(EINA_FALSE);

        int bg_width = 0, bg_height = 0;
        ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas), NULL, NULL, &bg_width, &bg_height);

        win_dialog = edje_object_add(evas);
        edje_object_file_set(win_dialog, PACKAGE_DATA_DIR "/theme/default.edj", "dialog/win");
        evas_object_resize(win_dialog, bg_width, bg_height);
        edje_object_signal_callback_add(win_dialog, "*", "*", _win_dialog_cb, NULL);
        evas_object_move(win_dialog, 0, 0);
        evas_object_show(win_dialog);

        edje_object_signal_emit(win_dialog, "show", "dialog");
}

static void _win_dialog_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
        if (!strcmp(emission, "select,menu") && !strcmp(source, "group:menu"))
        {
                edje_object_signal_emit(win_dialog, "hide", "dialog");
                win_dialog_action = GAME_QUIT;

                if (level) level_del(level);
                game_stop_level();
        }
        else if (!strcmp(emission, "select,next") && !strcmp(source, "group:menu"))
        {
                edje_object_signal_emit(win_dialog, "hide", "dialog");
                win_dialog_action = GAME_LEVEL_NEXT;

                if (level) level_del(level);
                game_stop_level();
        }
        else if (!strcmp(emission, "hidden") && !strcmp(source, "dialog"))
        {
                if (win_dialog_action == GAME_LEVEL_NEXT)
                {
                        level_nth = eina_list_next(level_nth);

                        if (!level_nth) level_nth = levels_list;

                        Level *l = NULL;
                        if (!level_load_file(&l, eina_list_data_get(level_nth)))
                        {
                                ERR("Can't load level %s !", (char *)eina_list_data_get(level_nth));

                                return;
                        }

                        game_play_level(evas, l, _win_callback, want_menu_button);
                }
                else
                {
                        if (game_play_done)
                                game_play_done(game_end_data);
                }
        }
}

static void _menu_button_click(void *data, Evas_Object *obj, const char *emission, const char *source)
{
        if (level) level_del(level);
        game_stop_level();

        if (game_play_done)
                game_play_done(game_end_data);
}

