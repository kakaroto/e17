/** @file etk_tree2_model.h */
#ifndef _ETK_TREE2_MODEL_H_
#define _ETK_TREE2_MODEL_H_

#include <stdarg.h>
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Tree2_Model Etk_Tree2_Model
 * Etk_Tree2_Models are used by a tree to define the content of its columns
 * @{
 */

/**
 * @brief A tree model defines the content of a tree's column, and how it should be displayed
 * @structinfo
 */
struct Etk_Tree2_Model
{
   Etk_Tree2 *tree;
   Etk_Tree2_Col *col;
   int cell_data_size;
   
   void (*model_free)(Etk_Tree2_Model *model);
   void (*cell_data_init)(Etk_Tree2_Model *model, void *cell_data);
   void (*cell_data_free)(Etk_Tree2_Model *model, void *cell_data);
   void (*cell_data_set)(Etk_Tree2_Model *model, void *cell_data, va_list *args);
   void (*cell_data_get)(Etk_Tree2_Model *model, void *cell_data, va_list *args);
   void (*objects_create)(Etk_Tree2_Model *model, Evas_Object **cell_objects, Evas *evas);
   void (*render)(Etk_Tree2_Model *model, Etk_Tree2_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects, Evas *evas);
   int  (*width_get)(Etk_Tree2_Model *model, void *cell_data, Evas_Object **cell_objects);
};

Etk_Tree2_Model *etk_tree2_model_text_new(void);
Etk_Tree2_Model *etk_tree2_model_int_new(void);
Etk_Tree2_Model *etk_tree2_model_double_new(void);
Etk_Tree2_Model *etk_tree2_model_image_new(void);
Etk_Tree2_Model *etk_tree2_model_checkbox_new(void);
Etk_Tree2_Model *etk_tree2_model_progress_bar_new(void);

/* TODO: image_width_set() */

void etk_tree2_model_free(Etk_Tree2_Model *model);

/** @} */

#endif
