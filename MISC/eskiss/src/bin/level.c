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
#include "level.h"
#include <limits.h>
#include <Eet.h>
#include <Ecore_File.h>

static Eet_Data_Descriptor *edd_drawpoint = NULL, *edd_levelobject = NULL;
static Eet_Data_Descriptor *edd_level = NULL;

/**
 * @brief init data descriptors for level loading/saving
 */
void level_init(void)
{
        if (edd_level)
        {
                WARN("level_init already done");
                return;
        }

        eet_init();

        Eet_Data_Descriptor_Class edc;

        EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&edc, DrawPoint);

        edd_drawpoint = eet_data_descriptor_file_new(&edc);

        EET_DATA_DESCRIPTOR_ADD_BASIC(edd_drawpoint, DrawPoint, "x", x, EET_T_INT);
        EET_DATA_DESCRIPTOR_ADD_BASIC(edd_drawpoint, DrawPoint, "y", y, EET_T_INT);

        EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&edc, LevelObject);

        edd_levelobject = eet_data_descriptor_file_new(&edc);

        EET_DATA_DESCRIPTOR_ADD_BASIC(edd_levelobject, LevelObject, "color.r", color.r, EET_T_INT);
        EET_DATA_DESCRIPTOR_ADD_BASIC(edd_levelobject, LevelObject, "color.g", color.g, EET_T_INT);
        EET_DATA_DESCRIPTOR_ADD_BASIC(edd_levelobject, LevelObject, "color.b", color.b, EET_T_INT);
        EET_DATA_DESCRIPTOR_ADD_BASIC(edd_levelobject, LevelObject, "color.a", color.a, EET_T_INT);
        EET_DATA_DESCRIPTOR_ADD_BASIC(edd_levelobject, LevelObject, "has_physic", has_physic, EET_T_UCHAR);
        EET_DATA_DESCRIPTOR_ADD_BASIC(edd_levelobject, LevelObject, "static_object", static_object, EET_T_UCHAR);
        EET_DATA_DESCRIPTOR_ADD_LIST(edd_levelobject, LevelObject, "point_list", point_list, edd_drawpoint);

        EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&edc, Level);

        edd_level = eet_data_descriptor_file_new(&edc);

        EET_DATA_DESCRIPTOR_ADD_LIST(edd_level, Level, "objects", objects, edd_levelobject);
        EET_DATA_DESCRIPTOR_ADD_BASIC(edd_level, Level, "ball.x", ball.x, EET_T_INT);
        EET_DATA_DESCRIPTOR_ADD_BASIC(edd_level, Level, "ball.y", ball.y, EET_T_INT);
        EET_DATA_DESCRIPTOR_ADD_BASIC(edd_level, Level, "goal.x", goal.x, EET_T_INT);
        EET_DATA_DESCRIPTOR_ADD_BASIC(edd_level, Level, "goal.y", goal.y, EET_T_INT);
}

/**
 * @brief free data descriptors for level loading/saving
 */
void level_shutdown(void)
{
        if (!edd_level)
        {
                WARN("call level_init first");
                return;
        }

        eet_data_descriptor_free(edd_level);
        eet_data_descriptor_free(edd_levelobject);
        eet_data_descriptor_free(edd_drawpoint);

        edd_level = NULL;
        edd_levelobject = NULL;
        edd_drawpoint = NULL;

        eet_shutdown();
}

/**
 * @brief create a new level
 * @return the new level
 */
Level *level_add(void)
{
        Level *l = calloc(1, sizeof(Level));

        //Set default position for ball and goal
        l->ball = drawpoint(218, 308);
        l->goal = drawpoint(767, 395);

        INF("New level");

        return l;
}

/**
 * @brief delete a level
 * @param level the level to delete
 */
void level_del(Level *level)
{
        if (level->objects)
        {
                LevelObject *lo;
                EINA_LIST_FREE(level->objects, lo)
                {
                        DrawPoint *p;
                        if (lo->point_list)
                        {
                                EINA_LIST_FREE(lo->point_list, p)
                                        free(p);
                        }

                        free(lo);
                }
        }

        if (level->eet_file)
                free(level->eet_file);

        free(level);

        INF("level deleted");
}

/**
 * @brief Load a level from an eet file
 * @param level the level to load. Must be created first with level_add and empty
 * @param file the file to load
 * @return false if load failed
 */
Eina_Bool level_load_file(Level **level, const char *file)
{
        if (!edd_level)
        {
                ERR("call level_init first");
                return EINA_FALSE;
        }

        Eet_File *ef;

        ef = eet_open(file, EET_FILE_MODE_READ);
        if (!ef)
        {
                ERR("eet_open(%s) failed !", file);
                return EINA_FALSE;
        }

        Level *l;
        l = eet_data_read(ef, edd_level, "eskiss/level");
        if (!l)
        {
                ERR("eet_read(%s) failed !", file);
                eet_close(ef);
                return EINA_FALSE;
        }

        eet_close(ef);

        l->eet_file = strdup(file);

        *level = l;

        INF("Level %s successfully opened.", file);

        return EINA_TRUE;
}

/**
 * @brief Save a level to an eet file
 * @param level the level to save
 * @param file the file to write data to
 * @return false if save failed
 */
Eina_Bool level_save_file(Level *level, const char *file)
{
        if (!edd_level)
        {
                ERR("call level_init first");
                return EINA_FALSE;
        }

        if (level->eet_file)
                free(level->eet_file);
        level->eet_file = strdup(file);

        Eet_File *ef;

        ef = eet_open(file, EET_FILE_MODE_WRITE);
        if (!ef)
        {
                ERR("eet_open(%s) failed !", file);
                return EINA_FALSE;
        }

        if (!eet_data_write(ef, edd_level, "eskiss/level", level, 1))
        {
                ERR("eet_write(%s) failed !", file);
                eet_close(ef);
                return EINA_FALSE;
        }

        eet_close(ef);

        /****************
        Remove this when evas_object_image_fill bug is corrected
        and use level_get_thumb()
        **************/
        INF("No cache, generating thumb for level %s", level->eet_file);

        int bg_width = 1024, bg_height = 768;

        Ecore_Evas *ee_buffer = ecore_evas_buffer_new(bg_width, bg_height);
        Evas_Object *image_buffer = ecore_evas_object_image_new(ee_buffer);

        evas_object_move(image_buffer, 0, 0);
        evas_object_resize(image_buffer, bg_width, bg_height);
        evas_object_image_fill_set(image_buffer, 0, 0, bg_width, bg_height);
        evas_object_image_size_set(image_buffer, bg_width, bg_height);
        evas_object_show(image_buffer);

        Ecore_Evas *ee_im = (Ecore_Evas *)evas_object_data_get(image_buffer, "Ecore_Evas");
        Evas *evas_im = ecore_evas_get(ee_im);

        Eina_List *l;
        LevelObject *lo;
        EINA_LIST_FOREACH(level->objects, l, lo)
        {
                //create evas object from points
                Evas_Object *eobj = draw_object_create(evas_im, lo->point_list, lo->color);
        }

        Evas_Object *ball = evas_object_image_filled_add(evas_im);
        evas_object_image_file_set(ball, PACKAGE_DATA_DIR "/brushes/ball_01.png", NULL);
        int w, h;
        evas_object_image_size_get(ball, &w, &h);
        evas_object_resize(ball, w, h);
        evas_object_move(ball, level->ball.x, level->ball.y);
        evas_object_show(ball);

        Evas_Object *goal = evas_object_image_filled_add(evas_im);
        evas_object_image_file_set(goal, PACKAGE_DATA_DIR "/brushes/goal_01.png", NULL);
        evas_object_image_size_get(goal, &w, &h);
        evas_object_resize(goal, w, h);
        evas_object_move(goal, level->goal.x, level->goal.y);
        evas_object_show(goal);

        void *data = (void *)ecore_evas_buffer_pixels_get(ee_buffer);
        INF("Saving thumbnail for level %s", level->eet_file);
        ef = eet_open(level->eet_file, EET_FILE_MODE_READ_WRITE);
        if (!ef)
        {
                ERR("eet_open(%s) failed !", level->eet_file);
        }

        if (ef)
        {
                if(!eet_data_image_write(ef, "eskiss/thumb", data, bg_width, bg_height, 1, 9, 0, 0))
                {
                        ERR("eet_data_image_write(%s) failed !", level->eet_file);
                }

                eet_close(ef);
        }

        evas_object_del(image_buffer);
        ecore_evas_free(ee_buffer);
        /*************************************************/

        return EINA_TRUE;
}

/**
 * @brief Get the thumbnail of the level from the eet file. Create it if it does exist
 * @param level the level
 * @return the image object
 */
Evas_Object *level_get_thumb(Evas *evas, Level *level)
{
        Eet_File *ef;
        void *data;
        unsigned int w, h;
        Evas_Object *image = NULL;

        INF("Getting thumbnail for level %s", level->eet_file);

        int bg_width = 0, bg_height = 0;
        ecore_evas_geometry_get(ecore_evas_ecore_evas_get(evas), NULL, NULL, &bg_width, &bg_height);

        if (level->eet_file)
        {
                INF("Trying to load cache thumbnail for level %s", level->eet_file);
                ef = eet_open(level->eet_file, EET_FILE_MODE_READ);
                if (!ef)
                {
                        ERR("eet_open(%s) failed !", level->eet_file);
                }

                if (ef)
                {
                        data = eet_data_image_read(ef, "eskiss/thumb", &w, &h, NULL, NULL, NULL, NULL);

                        //create image object
                        Evas_Object *image = evas_object_image_filled_add(evas);

                        evas_object_move(image, 0, 0);
                        evas_object_image_fill_set(image, 0, 0, bg_width, bg_height);
                        evas_object_image_size_set(image, bg_width, bg_height);
                        evas_object_resize(image, bg_width, bg_height);
                        evas_object_image_alpha_set(image, 1);
                        evas_object_image_data_copy_set(image, data);
                        evas_object_show(image);

                        eet_close(ef);
                }
        }

        //No data in eet, create the picture
        if (!data)
        {
                INF("No cache, generating thumb for level %s", level->eet_file);

                Ecore_Evas *ee_buffer = ecore_evas_buffer_new(bg_width, bg_height);
                Evas_Object *image_buffer = ecore_evas_object_image_new(ee_buffer);

                evas_object_move(image_buffer, 0, 0);
                evas_object_resize(image_buffer, bg_width, bg_height);
                evas_object_image_fill_set(image_buffer, 0, 0, bg_width, bg_height);
                evas_object_image_size_set(image_buffer, bg_width, bg_height);
                evas_object_show(image_buffer);

                Ecore_Evas *ee_im = (Ecore_Evas *)evas_object_data_get(image_buffer, "Ecore_Evas");
                Evas *evas_im = ecore_evas_get(ee_im);

                Eina_List *l;
                LevelObject *lo;
                EINA_LIST_FOREACH(level->objects, l, lo)
                {
                        //create evas object from points
                        Evas_Object *eobj = draw_object_create(evas_im, lo->point_list, lo->color);
                }

                Evas_Object *ball = evas_object_image_filled_add(evas_im);
                evas_object_image_file_set(ball, PACKAGE_DATA_DIR "/brushes/ball_01.png", NULL);
                int w, h;
                evas_object_image_size_get(ball, &w, &h);
                evas_object_resize(ball, w, h);
                evas_object_move(ball, level->ball.x, level->ball.y);
                evas_object_show(ball);

                Evas_Object *goal = evas_object_image_filled_add(evas_im);
                evas_object_image_file_set(goal, PACKAGE_DATA_DIR "/brushes/goal_01.png", NULL);
                evas_object_image_size_get(goal, &w, &h);
                evas_object_resize(goal, w, h);
                evas_object_move(goal, level->goal.x, level->goal.y);
                evas_object_show(goal);

                data = (void *)ecore_evas_buffer_pixels_get(ee_buffer);
                w = bg_width;
                h = bg_height;
//                 evas_object_image_save(image_buffer, "tmp.png", NULL, NULL);

                //create image object
                //it's time to save our buffer image to a new one on the current Evas surface
                image = evas_object_image_filled_add(evas);

                evas_object_move(image, 0, 0);
                evas_object_image_fill_set(image, 0, 0, bg_width, bg_height);
                evas_object_image_size_set(image, bg_width, bg_height);
                evas_object_resize(image, bg_width, bg_height);
                evas_object_image_alpha_set(image, 1);
                evas_object_image_data_copy_set(image, data);
                evas_object_show(image);

                if (level->eet_file)
                {
                        INF("Saving thumbnail for level %s", level->eet_file);

                        ef = eet_open(level->eet_file, EET_FILE_MODE_READ_WRITE);
                        if (!ef)
                        {
                                ERR("eet_open(%s) failed !", level->eet_file);
                        }

                        if (ef)
                        {
                                if(!eet_data_image_write(ef, "eskiss/thumb", data, w, h, 1, 9, 0, 0))
                                {
                                        ERR("eet_data_image_write(%s) failed !", level->eet_file);
                                }

                                eet_close(ef);
                        }
                }

                evas_object_del(image_buffer);
                ecore_evas_free(ee_buffer);
        }

        INF("New thumbnail image added to evas");

        return image;
}

/**
 * @brief add an object to the level
 * @param level the level to modify
 * @param obj the object to add
 */
void level_object_add(Level *level, LevelObject *obj)
{
        if (!level)
        {
                ERR("level is NULL!");
                return;
        }
        if (!obj)
        {
                ERR("obj is NULL!");
                return;
        }

        level->objects = eina_list_append(level->objects, obj);

        INF("New object added to level");
}

/**
 * @brief remove an object from the level
 * @param level the level to modify
 * @param obj the object to remove
 * @note obj is also deleted
 */
void level_object_del(Level *level, LevelObject *obj)
{
        if (!level)
        {
                ERR("level is NULL!");
                return;
        }
        if (!obj)
        {
                ERR("obj is NULL!");
                return;
        }

        level->objects = eina_list_remove(level->objects, obj);

        DrawPoint *p;
        if (obj->point_list)
        {
                EINA_LIST_FREE(obj->point_list, p)
                free(p);
        }

        free(obj);

        INF("object removed from level");
}

/**
 * @brief search all levels in system dir and home dir
 * @return list of filenames
 */
Eina_List *level_search_level_files(void)
{
        Eina_List *list = NULL, *ls = NULL, *l;

        ls = ecore_file_ls(PACKAGE_DATA_DIR "/levels");

        char *file, filepath[PATH_MAX];
        EINA_LIST_FOREACH(ls, l, file)
        {
                if (ecore_file_is_dir(file))
                {
                        free(file);
                        continue;
                }

                sprintf(filepath, "%s/levels/%s", PACKAGE_DATA_DIR, file);
                free(file);

                Level *level = NULL;
                if (level_load_file(&level, filepath))
                {
                        level_del(level);
                        list = eina_list_append(list, strdup(filepath));

                        INF("level found: %s", filepath);
                }
        }

        ls = eina_list_free(ls);

        char home[PATH_MAX];
        sprintf(home, "%s/.eskiss/levels", getenv("HOME"));
        ls = ecore_file_ls(home);

        EINA_LIST_FOREACH(ls, l, file)
        {
                if (ecore_file_is_dir(file))
                {
                        free(file);
                        continue;
                }

                sprintf(filepath, "%s/.eskiss/levels/%s", getenv("HOME"), file);
                free(file);

                Level *level = NULL;
                if (level_load_file(&level, filepath))
                {
                        level_del(level);
                        list = eina_list_append(list, strdup(filepath));

                        INF("level found: %s", filepath);
                }
        }

        ls = eina_list_free(ls);

        return list;
}
