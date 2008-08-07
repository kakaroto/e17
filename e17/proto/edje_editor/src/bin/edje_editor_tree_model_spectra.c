/*  Copyright (C) 2006-2008 Davide Andreoli (see AUTHORS)
 *
 *  This file is part of Edje_editor.
 *  Edje_editor is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Edje_editor is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Edje_editor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "edje_editor_tree_model_spectra.h"
#include <string.h>
#include <Evas.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include "config.h"
#include "main.h"

/* Structure of the "spectra" model */
typedef struct Etk_Tree_Model_Spectra
{
   Etk_Tree_Model model;
   Etk_Cache *cache;
} Etk_Tree_Model_Spectra;

/* Data associated to the "spectra" model */
typedef struct Etk_Tree_Model_Spectra_Data
{
   char *spectra;
} Etk_Tree_Model_Spectra_Data;

static void _spectra_model_free(Etk_Tree_Model *model);
static void _spectra_cell_data_free(Etk_Tree_Model *model, void *cell_data);
static void _spectra_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void _spectra_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args);
static void _spectra_objects_cache(Etk_Tree_Model *model, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL]);
static Etk_Bool _spectra_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL], Evas *evas);
static int _spectra_width_get(Etk_Tree_Model *model, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL]);
static void _spectra_cache_remove(Etk_Tree_Model *model, const char *filename, const char *key);

/* The function to create the model */
Etk_Tree_Model *etk_tree_model_spectra_new(void)
{
   Etk_Tree_Model *model;
   Etk_Tree_Model_Spectra *spectra_model;

   model = calloc(1, sizeof(Etk_Tree_Model_Spectra));
   spectra_model = (Etk_Tree_Model_Spectra *)model;

   model->cell_data_size = sizeof(Etk_Tree_Model_Spectra_Data);
   model->model_free = _spectra_model_free;
   model->cell_data_free = _spectra_cell_data_free;
   model->cell_data_set = _spectra_cell_data_set;
   model->cell_data_get = _spectra_cell_data_get;
   model->objects_cache = _spectra_objects_cache;
   model->render = _spectra_render;
   model->width_get = _spectra_width_get;
   model->cache_remove = _spectra_cache_remove;

   spectra_model->cache = etk_cache_new(50);

   return model;
}

/* Spectra: model_free() */
static void _spectra_model_free(Etk_Tree_Model *model)
{
   Etk_Tree_Model_Spectra *spectra_model;

   if (!(spectra_model = (Etk_Tree_Model_Spectra *)model))
      return;
   etk_cache_destroy(spectra_model->cache);
}

/* Spectra: cell_data_free() */
static void _spectra_cell_data_free(Etk_Tree_Model *model, void *cell_data)
{
   Etk_Tree_Model_Spectra_Data *spectra_data;

   if (!(spectra_data = cell_data))
      return;

   free(spectra_data->spectra);
}

/* Spectra: cell_data_set() */
static void _spectra_cell_data_set(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   Etk_Tree_Model_Spectra_Data *spectra_data;
   char *string;

   if (!(spectra_data = cell_data) || !args || !model)
      return;

   /* Get the spectra name from the args */
   string = va_arg(*args, char *);
   if (spectra_data->spectra != string)
   {
      free(spectra_data->spectra);
      spectra_data->spectra = string ? strdup(string) : NULL;
   }
}

/* Spectra: cell_data_get() */
static void _spectra_cell_data_get(Etk_Tree_Model *model, void *cell_data, va_list *args)
{
   Etk_Tree_Model_Spectra_Data *spectra_data;
   char **string;

   if (!(spectra_data = cell_data) || !args || !model)
      return;

   string = va_arg(*args, char **);
   if (string)
      *string = spectra_data->spectra;
}

/* Spectra: objects_cache() */
static void _spectra_objects_cache(Etk_Tree_Model *model, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL])
{
   Etk_Tree_Model_Spectra *spectra_model;
   Etk_Tree_Model_Spectra_Data *spectra_data;

   if (!(spectra_model = (Etk_Tree_Model_Spectra *)model) || !cell_objects[0])
      return;

   spectra_data = cell_data;
   if (spectra_data)
      etk_cache_add(spectra_model->cache, cell_objects[0], spectra_data->spectra, NULL);
   else
      evas_object_del(cell_objects[0]);

   cell_objects[0] = NULL;
}

/* Spectra: render() */
static Etk_Bool _spectra_render(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL], Evas *evas)
{
   Etk_Tree_Model_Spectra *spectra_model;
   Etk_Tree_Model_Spectra_Data *spectra_data;
   int i;

   if (!(spectra_model = (Etk_Tree_Model_Spectra *)model) ||
       !(spectra_data = cell_data) || !evas || !spectra_data->spectra)
      return ETK_FALSE;

   if (!(cell_objects[0] = etk_cache_find(spectra_model->cache, spectra_data->spectra, NULL)))
   {
      //printf("MODEL RENDER TRUE %s \n", spectra_data->spectra);
      cell_objects[0] = evas_object_gradient_add(evas);
      if (!cell_objects[0]) return ETK_FALSE;

      evas_object_gradient_type_set(cell_objects[0], "linear", NULL);
      evas_object_gradient_fill_angle_set(cell_objects[0], 90);

      for (i = 0; i < edje_edit_spectra_stop_num_get(edje_o, spectra_data->spectra); i++)
      {
         int r, g, b, a, d;
         edje_edit_spectra_stop_color_get(edje_o, spectra_data->spectra, i, &r, &g, &b, &a, &d);
         evas_object_gradient_color_stop_add(cell_objects[0], r, g, b, 255, d);
         evas_object_gradient_alpha_stop_add(cell_objects[0], a, d);
      }
   }

   evas_object_move(cell_objects[0], geometry.x, geometry.y);
   evas_object_resize(cell_objects[0], geometry.w, geometry.h);
   evas_object_gradient_fill_set(cell_objects[0], 0, 0, geometry.h, geometry.w);
   evas_object_show(cell_objects[0]);

   return ETK_TRUE;
}

/* Spectra: width_get() */
static int _spectra_width_get(Etk_Tree_Model *model, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL])
{
   Etk_Tree_Model_Spectra *spectra_model;
   int w;

   if (!(spectra_model = (Etk_Tree_Model_Spectra *)model) || !cell_objects[0])
      return 0;

   evas_object_geometry_get(cell_objects[0], NULL, NULL, &w, NULL);
   return w;
}

/* Spectra: delete a certain evas object from the model's cache */
static void _spectra_cache_remove(Etk_Tree_Model *model, const char *filename, const char *key)
{
   Etk_Tree_Model_Spectra *spectra_model;

   if (!(spectra_model = (Etk_Tree_Model_Spectra *)model))
     return;

   etk_cache_remove(spectra_model->cache, etk_cache_find(spectra_model->cache, filename, NULL));
}
