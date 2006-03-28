/** @file etk_tree_model.h */
#ifndef _ETK_TREE_COL_MODEL_H_
#define _ETK_TREE_COL_MODEL_H_

#include <stdarg.h>
#include "etk_types.h"
#include "etk_widget.h"

/**
 * @defgroup Etk_Tree_Model Etk_Tree_Model
 * @{
 */

/**
 * @enum Etk_Tree_Model_Image_Type
 * @brief The types of the images that you could have in an "image" or "icon_text" model
 */ 
enum _Etk_Tree_Model_Image_Type
{
   ETK_TREE_FROM_FILE,
   ETK_TREE_FROM_EDJE
};

/**
 * @struct Etk_Tree_Model
 * @brief A tree model describes how the cells of a column of a tree should be displayed and manipulated
 */
struct _Etk_Tree_Model
{
   Etk_Tree *tree;
   Etk_Tree_Col *col;
   int cell_data_size;
   float xalign;
   float yalign;
   
   void (*model_free)(Etk_Tree_Model *model);
   void (*cell_data_init)(Etk_Tree_Model *model, void *cell_data);
   void (*cell_data_free)(Etk_Tree_Model *model, void *cell_data);
   void (*cell_data_set)(Etk_Tree_Model *model, void *cell_data, va_list *args);
   void (*cell_data_get)(Etk_Tree_Model *model, void *cell_data, va_list *args);
   void (*objects_create)(Etk_Tree_Model *model, Evas_Object **cell_objects, Evas *evas);
   void (*render)(Etk_Tree_Model *model, Etk_Tree_Row *row, Etk_Geometry geometry, void *cell_data, Evas_Object **cell_objects);
   void (*ideal_size_calc)(Etk_Tree_Model *model, void *cell_data, int *w, int *h);
};

Etk_Tree_Model *etk_tree_model_text_new(Etk_Tree *tree);
Etk_Tree_Model *etk_tree_model_int_new(Etk_Tree *tree);
Etk_Tree_Model *etk_tree_model_double_new(Etk_Tree *tree);
Etk_Tree_Model *etk_tree_model_image_new(Etk_Tree *tree, Etk_Tree_Model_Image_Type image_type);
Etk_Tree_Model *etk_tree_model_icon_text_new(Etk_Tree *tree, Etk_Tree_Model_Image_Type icon_type);
Etk_Tree_Model *etk_tree_model_checkbox_new(Etk_Tree *tree);
void etk_tree_model_free(Etk_Tree_Model *model);

void etk_tree_model_alignment_set(Etk_Tree_Model *model, float xalign, float yalign);
void etk_tree_model_alignment_get(Etk_Tree_Model *model, float *xalign, float *yalign);

void etk_tree_model_icon_text_icon_width_set(Etk_Tree_Model *model, int icon_width);
int etk_tree_model_icon_text_icon_width_get(Etk_Tree_Model *model);

/** @} */

#endif
