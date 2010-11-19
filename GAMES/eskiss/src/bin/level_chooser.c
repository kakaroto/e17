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
#include <limits.h>
#include <Ecore_File.h>

#define ITEM_CLICK(snum, num) else if (!strcmp(emission, "click,"snum) && !strcmp(source, "table:item")) _item_click(num);

static Evas *evas = NULL;
static Evas_Object *file_chooser = NULL;
static Eina_List *level_list = NULL;
static Eina_List *show_current = NULL;

static Eina_List *thumbnails = NULL;
static int current_page = 0;
static Eina_Bool quit_flag = EINA_FALSE;

static void *close_data = NULL;
static void (*close_cb)(char *level, void *data) = NULL;

static void _update_file_chooser(void);
static void _edje_cb(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _item_click(int num);

/**
 * @brief Show the level chooser
 * @param evas The canvas
 * @param cb callback called when user choose a level
 * @param data user data passed to callback
 */
void level_chooser_show(Evas *e, Eina_List *list, void (*cb)(char *level, void *data), void *data)
{
        if (!e)
        {
                ERR("evas is NULL");
                return;
        }
        if (!list)
        {
                ERR("level list is NULL");
        }

        evas = e;
        level_list = list;
        close_cb = cb;
        close_data = data;

        int bg_width = 0, bg_height = 0;
        ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas), NULL, NULL, &bg_width, &bg_height);

        file_chooser = edje_object_add(evas);
        edje_object_file_set(file_chooser, PACKAGE_DATA_DIR "/theme/default.edj", "dialog/level_list");
        evas_object_resize(file_chooser, bg_width, bg_height);
        evas_object_move(file_chooser, 0, 0);
        evas_object_show(file_chooser);

        edje_object_signal_callback_add(file_chooser, "*", "*", _edje_cb, NULL);

        edje_object_signal_emit(file_chooser, "show,right", "dialog");

        show_current = level_list;
        current_page = 0;
        quit_flag = EINA_FALSE;

        _update_file_chooser();
}

/**
 * @brief Close the level chooser
 */
void level_chooser_close(void)
{
        quit_flag = EINA_TRUE;
        edje_object_signal_emit(file_chooser, "hide,right", "dialog");

        Evas_Object *t;
        EINA_LIST_FREE(thumbnails, t)
                evas_object_del(t);
}

/**
 * @brief Show the "new" button to create new file
 */
void level_chooser_new_button_show()
{
        if (file_chooser)
                edje_object_signal_emit(file_chooser, "show,save", "dialog");
}

/* PRIVATE -------------------- */

static void _update_file_chooser(void)
{
        int cpt = 0;

        Evas_Object *t;
        EINA_LIST_FREE(thumbnails, t)
                evas_object_del(t);

        if (show_current != level_list)
                edje_object_signal_emit(file_chooser, "show,previous", "dialog");
        else
                edje_object_signal_emit(file_chooser, "hide,previous", "dialog");

        Evas_Object *table = (Evas_Object *)edje_object_part_object_get(file_chooser, "table");
        Eina_List *items = evas_object_table_children_get(table);

        Eina_List *l;
        char *level_file;
        EINA_LIST_FOREACH(show_current, l, level_file)
        {
                if (cpt >= 8) break;

                Level *level;
                level_load_file(&level, level_file);


#ifdef BUG_HERE___

/*
        There is a bug in evas here...
*/

                Evas_Object *thumb = level_get_thumb(evas, level);
                evas_object_resize(thumb, 500, 500);
                evas_object_move(thumb, 100, 100);
                evas_object_del(thumb);
#else
                Evas_Object *thumb = evas_object_image_filled_add(evas);
                evas_object_image_file_set(thumb, level_file, "eskiss/thumb");
                evas_object_show(thumb);
#endif
                level_del(level);

                thumbnails = eina_list_append(thumbnails, thumb);

                Evas_Object *item = eina_list_nth(items, cpt);
                if (!edje_object_part_swallow(item, "item.thumb", thumb))
                        ERR("swallow failed ! %p", item);

                edje_object_signal_emit(item, "load,done", "item");
                edje_object_signal_emit(item, "enable", "item");

                cpt++;
        }

        if (l)
                edje_object_signal_emit(file_chooser, "show,next", "dialog");
        else
                edje_object_signal_emit(file_chooser, "hide,next", "dialog");
}

static void _edje_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
        if (!strcmp(emission, "select,next") && !strcmp(source, "menu"))
        {
                edje_object_signal_emit(file_chooser, "hide,left", "dialog");

                Evas_Object *table = (Evas_Object *)edje_object_part_object_get(file_chooser, "table");
                Eina_List *items = evas_object_table_children_get(table);

                Eina_List *l;
                Evas_Object *item;
                EINA_LIST_FOREACH(items, l, item)
                        edje_object_signal_emit(item, "disable", "item");
        }
        else if (!strcmp(emission, "select,previous") && !strcmp(source, "menu"))
        {
                edje_object_signal_emit(file_chooser, "hide,right", "dialog");

                Evas_Object *table = (Evas_Object *)edje_object_part_object_get(file_chooser, "table");
                Eina_List *items = evas_object_table_children_get(table);

                Eina_List *l;
                Evas_Object *item;
                EINA_LIST_FOREACH(items, l, item)
                        edje_object_signal_emit(item, "disable", "item");
        }
        else if (!strcmp(emission, "select,save") && !strcmp(source, "menu"))
        {
                char level_file[PATH_MAX];
                int cpt = 0;

                do
                {
                        sprintf(level_file, "%s/.eskiss/levels/%03d_userlevel.eet", getenv("HOME"), cpt);
                        cpt++;
                }
                while (ecore_file_exists(level_file));

                level_chooser_close();

                if (close_cb)
                        close_cb(level_file, close_data);
        }
        else if (!strcmp(emission, "left,hidden") && !strcmp(source, "dialog"))
        {
                if (quit_flag) return;

                edje_object_signal_emit(file_chooser, "show,right", "dialog");

                int count = eina_list_count(level_list) / 8;

                current_page++;
                if (current_page >= count)
                        current_page = count;

                show_current = eina_list_nth_list(level_list, current_page * 8);

                _update_file_chooser();
        }
        else if (!strcmp(emission, "right,hidden") && !strcmp(source, "dialog"))
        {
                if (quit_flag) return;

                edje_object_signal_emit(file_chooser, "show,left", "dialog");

                int count = eina_list_count(level_list) / 8;

                current_page--;
                if (current_page < 0)
                        current_page = 0;

                show_current = eina_list_nth_list(level_list, current_page * 8);

                _update_file_chooser();
        }
        else if (!strcmp(emission, "close") && !strcmp(source, "dialog"))
        {
                level_chooser_close();

                if (close_cb)
                        close_cb(NULL, close_data);
        }
        ITEM_CLICK("0", 0)
        ITEM_CLICK("1", 1)
        ITEM_CLICK("2", 2)
        ITEM_CLICK("3", 3)
        ITEM_CLICK("4", 4)
        ITEM_CLICK("5", 5)
        ITEM_CLICK("6", 6)
        ITEM_CLICK("7", 7)
}

static void _item_click(int num)
{
        Evas_Object *table = (Evas_Object *)edje_object_part_object_get(file_chooser, "table");
        Eina_List *items = evas_object_table_children_get(table);

        char *level_file = eina_list_nth(level_list, current_page * 8 + num);

        level_chooser_close();

        if (close_cb)
                close_cb(level_file, close_data);
}
