/** @file etk_tree.h */
#ifndef _ETK_TREE_H_
#define _ETK_TREE_H_

#include "etk_widget.h"
#include <Ecore_Data.h>
#include <stdarg.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Tree Etk_Tree
 * @{
 */

/** @brief Gets the type of a tree */
#define ETK_TREE_TYPE       (etk_tree_type_get())
/** @brief Casts the object to an Etk_Tree */
#define ETK_TREE(obj)       (ETK_OBJECT_CAST((obj), ETK_TREE_TYPE, Etk_Tree))
/** @brief Check if the object is an Etk_Tree */
#define ETK_IS_TREE(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TREE_TYPE))

/**
 * @struct Etk_Tree
 * @brief A tree is a widget that displays objects (text, images, ...) into columns and rows, with eventually a hierarchy
 */
struct _Etk_Tree
{
   /* private: */
   /* Inherit form Etk_Widget */
   Etk_Widget widget;

   int num_cols;
   Etk_Tree_Col **columns;
   Ecore_List *rows;

   Ecore_List *items_objects;
   Etk_Bool built;

   float scroll_percent;

   int item_height;
   Etk_Color separator_color;
   Etk_Color row_color1;
   Etk_Color row_color2;
};

/**
 * @struct Etk_Tree_Col
 * @brief A column of a tree
 */
struct _Etk_Tree_Col
{
   /* private: */
   int id;
   Etk_Tree *tree;
   char *title;
   Etk_Tree_Col_Type type;

   int xoffset;
   int width;
   int place;
   Etk_Bool visible;
};

/**
 * @struct Etk_Tree_Row
 * @brief A row of a tree
 */
struct _Etk_Tree_Row
{
   /* private: */
   Etk_Tree *tree;
   Etk_Tree_Cell *cells;
   Ecore_List *child_rows;
};

/**
 * @union Etk_Tree_Cell
 * @brief The value of a cell of the tree
 */
union _Etk_Tree_Cell
{
   char *text_value;
   char *image_filename_value;
   struct
   {
      char *text;
      char *image_filename;
   } text_image_value;
   int int_value;
   double double_value;
};

Etk_Type *etk_tree_type_get();
Etk_Widget *etk_tree_new();

Etk_Tree_Col *etk_tree_col_new(Etk_Tree *tree, const char *title, Etk_Tree_Col_Type type);
void etk_tree_build(Etk_Tree *tree);

Etk_Tree_Row *etk_tree_append(Etk_Tree *tree, ...);

/** @} */

#endif
