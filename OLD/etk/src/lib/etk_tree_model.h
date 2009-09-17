/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_tree_model.h */
#ifndef _ETK_TREE_MODEL_H_
#define _ETK_TREE_MODEL_H_

#include <stdarg.h>

#include <Evas.h>

#include "etk_tree.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Tree_Model Etk_Tree_Model
 * Etk_Tree_Models are used by a tree to define the content of its columns
 * @{
 */


/**
 * @brief A tree model defines the content of a tree's column, and how it should be displayed
 * @structinfo
 */
struct Etk_Tree_Model
{
   Etk_Tree *tree;
   Etk_Tree_Col *col;
   int cell_data_size;
   int index;

   void (*model_free)(Etk_Tree_Model *model);
   void (*cell_data_init)(Etk_Tree_Model *model, void *cell_data);
   void (*cell_data_free)(Etk_Tree_Model *model, void *cell_data);
   void (*cell_data_set)(Etk_Tree_Model *model, void *cell_data, va_list *args);
   void (*cell_data_get)(Etk_Tree_Model *model, void *cell_data, va_list *args);
   void (*objects_create)(Etk_Tree_Model *model, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL], Evas *evas);
   void (*objects_cache)(Etk_Tree_Model *model, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL]);
   Etk_Bool (*render)(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL], Evas *evas);
   int (*width_get)(Etk_Tree_Model *model, void *cell_data, Evas_Object *cell_objects[ETK_TREE_MAX_OBJECTS_PER_MODEL]);
   void (*cache_remove)(Etk_Tree_Model *model, const char *filename, const char *key);
};


Etk_Tree_Model *etk_tree_model_text_new(void);
Etk_Tree_Model *etk_tree_model_int_new(void);
Etk_Tree_Model *etk_tree_model_double_new(void);
Etk_Tree_Model *etk_tree_model_image_new(void);
Etk_Tree_Model *etk_tree_model_checkbox_new(void);
Etk_Tree_Model *etk_tree_model_progress_bar_new(void);
void            etk_tree_model_free(Etk_Tree_Model *model);
void            etk_tree_model_cache_remove(Etk_Tree_Model *model, const char *filename, const char *key);

void            etk_tree_model_image_width_set(Etk_Tree_Model *model, int width, float alignment);
void            etk_tree_model_image_width_get(Etk_Tree_Model *model, int *width, float *alignment);

/* TODO: checkbox-model: use a third value to actually hide the checkbox */

/** @} */

#ifdef __cplusplus
}
#endif

#endif
