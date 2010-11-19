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

#include "level_chooser.h"
#include "level_editor.h"
#include "main.h"

#define COLOR_SET_FROM_SIGNAL(signal, val) \
else if (!strcmp(emission, signal)) { \
if (draw_mode == MODE_COLOR) { \
        brush_color = default_colors[val]; \
        edje_object_signal_emit(editor_menu_color, "hide", "menu"); } }

enum { MODE_DRAW, MODE_DRAW_STATIC, MODE_DRAW_NOPHYSIC, MODE_MOVE, MODE_COLOR, MODE_PLAY, MODE_MENU };

static Evas *evas = NULL;
static Eina_Bool editor_opened = EINA_FALSE;
static Evas_Object *editor_menu = NULL;
static Evas_Object *editor_menu_running = NULL;
static Evas_Object *editor_menu_color = NULL;
static Level *level = NULL;
static void *cb_data = NULL;
static void (*close_cb)(Level *level, void *data) = NULL;

static void _on_new_object(Evas_Object *new_obj, Eina_List *point_list, void *data);
static void _menu_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _unselect_buttons(void);
static void _mouse_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _mouse_move_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void _mouse_up_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info);

static void _load_level_cb(char *level, void *data);
static void _save_level_cb(char *level, void *data);

static BrushColor brush_color = BRUSH_GREEN;
static int draw_mode = MODE_DRAW;
static Eina_Bool is_clicked = EINA_FALSE;
static Evas_Object *moving_object = NULL;

static DrawPoint move_start;
static DrawPoint move_offset;

static Eina_List *object_list = NULL;

static Evas_Object *ball = NULL, *goal = NULL;

static Eina_List *search_levels = NULL;

/**
 * @brief open the level editor to edit a level
 * @param evas the main canvas
 * @param level level to edit
 * @param callback called when editor is closed
 * @param data user pointer passed to callback
 */
void level_editor_edit(Evas *e, Level *l, void (*edit_done)(Level *level, void *data), void *data)
{
        if (editor_opened)
        {
                WARN("Editor is already opened, closing.");
                level_editor_close();
        }

        editor_opened = EINA_TRUE;
        evas = e;
        cb_data = data;
        close_cb = edit_done;
        level = l;
        draw_mode = MODE_DRAW;

        draw_object_add(evas);
        draw_object_new_callback_add(_on_new_object, NULL);
        draw_object_brush_set(brush_color);

        int bg_width = 0, bg_height = 0;
        ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas), NULL, NULL, &bg_width, &bg_height);

        //create the editor edje, and show it
        editor_menu = edje_object_add(evas);
        edje_object_file_set(editor_menu, PACKAGE_DATA_DIR "/theme/default.edj", "editor/menu");
        edje_object_signal_callback_add(editor_menu, "*", "menu", _menu_cb, NULL);
        evas_object_resize(editor_menu, bg_width, bg_height);
        evas_object_move(editor_menu, 0, 0);
        evas_object_show(editor_menu);

        ball = edje_object_add(evas);
        edje_object_file_set(ball, PACKAGE_DATA_DIR "/theme/default.edj", "main/ball");
        int w, h;
        edje_object_size_min_get(ball, &w, &h);
        evas_object_resize(ball, w, h);
        evas_object_move(ball, level->ball.x, level->ball.y);
        evas_object_show(ball);

        evas_object_event_callback_add(ball, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down_cb, NULL);
        evas_object_event_callback_add(ball, EVAS_CALLBACK_MOUSE_UP, _mouse_up_cb, NULL);
        evas_object_event_callback_add(ball, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move_cb, NULL);

        goal = edje_object_add(evas);
        edje_object_file_set(goal, PACKAGE_DATA_DIR "/theme/default.edj", "main/goal");
        edje_object_size_min_get(goal, &w, &h);
        evas_object_resize(goal, w, h);
        evas_object_move(goal, level->goal.x, level->goal.y);
        evas_object_show(goal);

        evas_object_event_callback_add(goal, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down_cb, NULL);
        evas_object_event_callback_add(goal, EVAS_CALLBACK_MOUSE_UP, _mouse_up_cb, NULL);
        evas_object_event_callback_add(goal, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move_cb, NULL);

        edje_object_signal_emit(editor_menu, "item,select,draw", "menu");
}

/**
 * @brief close the editor
 */
void level_editor_close(void)
{
        if (!editor_opened)
        {
                WARN("Editor is not opened.");
                return;
        }

        editor_opened = EINA_FALSE;

        draw_object_del();

        char *l;
        EINA_LIST_FREE(search_levels, l)
                free(l);

        if (ball)
                edje_object_signal_emit(ball, "hide", "ball");
        if (goal)
                edje_object_signal_emit(ball, "hide", "goal");

        if (editor_menu)
                edje_object_signal_emit(editor_menu, "hide", "menu");
}


// Private ----------------------------

static void _on_new_object(Evas_Object *new_obj, Eina_List *point_list, void *data)
{
        if (draw_mode != MODE_DRAW && draw_mode != MODE_DRAW_STATIC
            && draw_mode != MODE_COLOR && draw_mode != MODE_DRAW_NOPHYSIC)
        {
                WARN("You're not in drawing mode ! Removing object.");

                evas_object_del(new_obj);

                return;
        }

        //Add object to level
        LevelObject *o = calloc(1, sizeof(LevelObject));

        o->color = brush_color;
        o->has_physic = EINA_TRUE;
        if (draw_mode == MODE_DRAW_STATIC)
                o->static_object = EINA_TRUE;
        if (draw_mode == MODE_DRAW_NOPHYSIC)
                o->has_physic = EINA_FALSE;

        //copy point list
        Eina_List *l;
        DrawPoint *p;
        EINA_LIST_FOREACH(point_list, l, p)
        {
                DrawPoint *p2 = calloc(1, sizeof(DrawPoint));
                *p2 = *p;
                o->point_list = eina_list_append(o->point_list, p2);
        }

        level_object_add(level, o);

        EditorObject *eo = calloc(1, sizeof(EditorObject));
        eo->level_obj = o;
        eo->obj = new_obj;

        object_list = eina_list_append(object_list, eo);

        evas_object_event_callback_add(new_obj, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down_cb, NULL);
        evas_object_event_callback_add(new_obj, EVAS_CALLBACK_MOUSE_UP, _mouse_up_cb, NULL);
        evas_object_event_callback_add(new_obj, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move_cb, NULL);
}

static void _menu_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
        if (strcmp(source, "menu") != 0) return;

        if (!strcmp(emission, "clicked,draw"))
        {
                _unselect_buttons();
                edje_object_signal_emit(editor_menu, "item,select,draw", "menu");

                draw_mode = MODE_DRAW;

                draw_object_enabled_set(EINA_TRUE);
        }
        else if (!strcmp(emission, "clicked,draw_static"))
        {
                _unselect_buttons();
                edje_object_signal_emit(editor_menu, "item,select,draw_static", "menu");

                draw_mode = MODE_DRAW_STATIC;

                draw_object_enabled_set(EINA_TRUE);
        }
        else if (!strcmp(emission, "clicked,draw_nophysic"))
        {
                _unselect_buttons();
                edje_object_signal_emit(editor_menu, "item,select,draw_nophysic", "menu");

                draw_mode = MODE_DRAW_NOPHYSIC;

                draw_object_enabled_set(EINA_TRUE);
        }
        else if (!strcmp(emission, "clicked,move"))
        {
                _unselect_buttons();
                edje_object_signal_emit(editor_menu, "item,select,move", "menu");

                draw_mode = MODE_MOVE;

                draw_object_enabled_set(EINA_FALSE);
        }
        else if (!strcmp(emission, "clicked,color"))
        {
                _unselect_buttons();
                edje_object_signal_emit(editor_menu, "item,select,color", "menu");

                draw_mode = MODE_COLOR;

                edje_object_signal_emit(editor_menu, "hide", "menu");
        }
        else if (!strcmp(emission, "clicked,play"))
        {
                _unselect_buttons();
                edje_object_signal_emit(editor_menu, "item,select,play", "menu");

                edje_object_signal_emit(editor_menu, "hide", "menu");

                draw_mode = MODE_PLAY;

                //hide all objects that we edit now.
                Eina_List *l;
                EditorObject *eo;
                EINA_LIST_FOREACH(object_list, l, eo)
                {
                        evas_object_hide(eo->obj);
                }

                evas_object_hide(ball);
                evas_object_hide(goal);

                draw_object_new_callback_del(_on_new_object, NULL);

                game_play_level(evas, level, NULL, EINA_FALSE);
        }
        else if (!strcmp(emission, "clicked,stop"))
        {
                game_stop_level();

                //hide all objects that we edit now.
                Eina_List *l;
                EditorObject *eo;
                EINA_LIST_FOREACH(object_list, l, eo)
                {
                        evas_object_show(eo->obj);
                }

                evas_object_show(ball);
                evas_object_show(goal);

                draw_object_new_callback_add(_on_new_object, NULL);

                edje_object_signal_emit(editor_menu_running, "hide", "menu");
        }
        else if (!strcmp(emission, "clicked,menu"))
        {
                _unselect_buttons();
                edje_object_signal_emit(editor_menu, "item,select,menu", "menu");

                draw_mode = MODE_MENU;

                level_editor_close();
        }
        else if (!strcmp(emission, "clicked,open"))
        {
                char *l;
                EINA_LIST_FREE(search_levels, l)
                        free(l);
                search_levels = level_search_level_files();

                level_chooser_show(evas, search_levels, _load_level_cb, NULL);

                draw_object_enabled_set(EINA_FALSE);
        }
        else if (!strcmp(emission, "clicked,save"))
        {
                char *le;
                EINA_LIST_FREE(search_levels, le)
                        free(le);
                search_levels = level_search_level_files();

                Eina_List *l, *ll;
                EINA_LIST_FOREACH_SAFE(search_levels, l, ll, le)
                {
                        if (!strstr(le, ".eskiss"))
                        {
                                search_levels = eina_list_remove_list(search_levels, l);
                                free(le);
                        }
                }

                level_chooser_show(evas, search_levels, _save_level_cb, NULL);

                level_chooser_new_button_show();

                draw_object_enabled_set(EINA_FALSE);
        }
        else if (!strcmp(emission, "hidden"))
        {
                if (draw_mode == MODE_PLAY)
                {
                        int bg_width = 0, bg_height = 0;
                        ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas), NULL, NULL, &bg_width, &bg_height);

                        editor_menu_running = edje_object_add(evas);
                        edje_object_file_set(editor_menu_running, PACKAGE_DATA_DIR "/theme/default.edj", "editor/menu/running");
                        edje_object_signal_callback_add(editor_menu_running, "*", "menu", _menu_cb, NULL);
                        evas_object_resize(editor_menu_running, bg_width, bg_height);
                        evas_object_move(editor_menu_running, 0, 0);
                        evas_object_show(editor_menu_running);
                }
                else if (draw_mode == MODE_COLOR)
                {
                        int bg_width = 0, bg_height = 0;
                        ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas), NULL, NULL, &bg_width, &bg_height);

                        editor_menu_color = edje_object_add(evas);
                        edje_object_file_set(editor_menu_color, PACKAGE_DATA_DIR "/theme/default.edj", "editor/menu/color");
                        edje_object_signal_callback_add(editor_menu_color, "*", "menu", _menu_cb, NULL);
                        evas_object_resize(editor_menu_color, bg_width, bg_height);
                        evas_object_move(editor_menu_color, 0, 0);
                        evas_object_show(editor_menu_color);

                        int i;
                        for (i = 0;i < BRUSH_COUNT;i++)
                        {
                                Evas_Object *c = evas_object_rectangle_add(evas);
                                evas_object_color_set(c, default_colors[i].r, default_colors[i].g, default_colors[i].b, default_colors[i].a);
                                char part[50];
                                sprintf(part, "item_swallow_%d", i);
                                edje_object_part_swallow(editor_menu_color, part, c);
                                evas_object_pass_events_set(c, EINA_TRUE);
                                evas_object_show(c);
                        }
                }
                else if (draw_mode == MODE_MENU)
                {
                        //remove all objects
                        EditorObject *eo;
                        EINA_LIST_FREE(object_list, eo)
                        {
                                evas_object_del(eo->obj);

                                free(eo);
                        }

                        evas_object_del(ball);
                        ball = NULL;
                        evas_object_del(goal);
                        goal = NULL;

                        if (close_cb) close_cb(level, cb_data);

                        level = NULL;
                        cb_data = NULL;
                        close_cb = NULL;
                        evas = NULL;
                }
        }
        else if (!strcmp(emission, "running,hidden") ||
                 !strcmp(emission, "color,hidden"))
        {
                edje_object_signal_emit(editor_menu, "show", "menu");
                edje_object_signal_emit(editor_menu, "item,select,draw", "menu");
                draw_mode = MODE_DRAW;

                if (editor_menu_running)
                {
                        evas_object_del(editor_menu_running);
                        editor_menu_running = NULL;
                }

                if (editor_menu_color)
                {
                        evas_object_del(editor_menu_color);
                        editor_menu_color = NULL;
                }

                draw_object_brush_set(brush_color);
        }
        COLOR_SET_FROM_SIGNAL("clicked,0", 0)
        COLOR_SET_FROM_SIGNAL("clicked,1", 1)
        COLOR_SET_FROM_SIGNAL("clicked,2", 2)
        COLOR_SET_FROM_SIGNAL("clicked,3", 3)
        COLOR_SET_FROM_SIGNAL("clicked,4", 4)
        COLOR_SET_FROM_SIGNAL("clicked,5", 5)
        COLOR_SET_FROM_SIGNAL("clicked,6", 6)
        COLOR_SET_FROM_SIGNAL("clicked,7", 7)
}

static void _unselect_buttons(void)
{
        edje_object_signal_emit(editor_menu, "item,unselect,draw", "menu");
        edje_object_signal_emit(editor_menu, "item,unselect,draw_static", "menu");
        edje_object_signal_emit(editor_menu, "item,unselect,draw_nophysic", "menu");
        edje_object_signal_emit(editor_menu, "item,unselect,move", "menu");
        edje_object_signal_emit(editor_menu, "item,unselect,color", "menu");
        edje_object_signal_emit(editor_menu, "item,unselect,play", "menu");
        edje_object_signal_emit(editor_menu, "item,unselect,menu", "menu");
}

static void _mouse_down_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
        Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *)event_info;

        if (ev->flags & EVAS_BUTTON_DOUBLE_CLICK && obj != ball && obj != goal) //remove object if double click
        {
                Eina_List *l;
                EditorObject *eo;
                EINA_LIST_FOREACH(object_list, l, eo)
                {
                        if (eo->obj == obj)
                        {
                                if (!eo->level_obj)
                                {
                                        ERR("LevelObject is NULL !");
                                        continue;
                                }

                                level_object_del(level, eo->level_obj);
                                evas_object_del(eo->obj);

                                object_list = eina_list_remove(object_list, eo);
                                free(eo);

                                INF("Object removed");

                                return;
                        }
                }
        }

        if (draw_mode != MODE_MOVE) return;

        move_start = drawpoint(ev->canvas.x, ev->canvas.y);

        int x, y;
        evas_object_geometry_get(obj, &x, &y, NULL, NULL);
        move_offset = drawpoint(move_start.x - x, move_start.y - y);

        is_clicked = EINA_TRUE;
        moving_object = obj;
}

static void _mouse_move_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
        if (draw_mode != MODE_MOVE || !is_clicked) return;

        if (moving_object != obj) return; //drop if wrong object

        Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move *)event_info;

        evas_object_move(obj, ev->cur.canvas.x - move_offset.x, ev->cur.canvas.y - move_offset.y);
}

static void _mouse_up_cb(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
        if (draw_mode != MODE_MOVE) return;

        Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up *)event_info;

        is_clicked = EINA_FALSE;
        moving_object = NULL;

        if (obj == ball)
        {
                level->ball.x = ev->canvas.x - move_offset.x;
                level->ball.y = ev->canvas.y - move_offset.y;

                INF("Ball moved");

                return;
        }
        if (obj == goal)
        {
                level->goal.x = ev->canvas.x - move_offset.x;
                level->goal.y = ev->canvas.y - move_offset.y;

                INF("Goal moved");

                return;
        }

        Eina_List *l;
        EditorObject *eo;
        EINA_LIST_FOREACH(object_list, l, eo)
        {
                if (eo->obj == obj)
                {
                        if (!eo->level_obj)
                        {
                                ERR("LevelObject is NULL !");
                                continue;
                        }

                        Eina_List *l2;
                        DrawPoint *p;
                        EINA_LIST_FOREACH(eo->level_obj->point_list, l2, p)
                        {
                                p->x += ev->canvas.x - move_start.x;
                                p->y += ev->canvas.y - move_start.y;
                        }

                        INF("Object moved");
                }
        }
}

static void _load_level_cb(char *level_file, void *data)
{
        Level *ll = NULL;

        draw_object_enabled_set(EINA_TRUE);

        if (!level_load_file(&ll, level_file))
        {
                ERR("Can't open level !");
                return;
        }

        //remove all objects
        level_del(level);
        EditorObject *eo;
        EINA_LIST_FREE(object_list, eo)
        {
                evas_object_del(eo->obj);

                free(eo);
        }

        level = ll;

        Eina_List *l;
        LevelObject *lo;
        EINA_LIST_FOREACH(level->objects, l, lo)
        {
                //create evas object from points
                Evas_Object *eobj = draw_object_create(evas, lo->point_list, lo->color);

                evas_object_event_callback_add(eobj, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down_cb, NULL);
                evas_object_event_callback_add(eobj, EVAS_CALLBACK_MOUSE_UP, _mouse_up_cb, NULL);
                evas_object_event_callback_add(eobj, EVAS_CALLBACK_MOUSE_MOVE, _mouse_move_cb, NULL);

                EditorObject *eo = calloc(1, sizeof(EditorObject));
                eo->level_obj = lo;
                eo->obj = eobj;

                object_list = eina_list_append(object_list, eo);
        }

        evas_object_move(ball, level->ball.x, level->ball.y);
        evas_object_move(goal, level->goal.x, level->goal.y);
}

static void _save_level_cb(char *level_file, void *data)
{
        draw_object_enabled_set(EINA_TRUE);

        if (!level_file)
        {
                ERR("Filename is NULL !");
                return;
        }

        if (!level_save_file(level, level_file))
        {
                ERR("Can't save level !");
                return;
        }
}
