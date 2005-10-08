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
 * @struct Etk_Tree_Node
 * @brief A node of a tree contains several rows
 */
struct _Etk_Tree_Node
{
   Etk_Tree_Node *parent;
   Ecore_List *child_rows;
   int num_visible_children;
   int num_parent_children;
   Etk_Bool expanded;
   Etk_Bool selected;
};

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
   Ecore_List *cols_objects;
   
   Etk_Tree_Node root;
   Etk_Tree_Node *last_selected;

   Evas_Object *clip;
   Ecore_List *items_objects;

   Etk_Tree_Mode mode;
   Etk_Bool multiple_select;
   float scroll_percent;
   Etk_Bool frozen;
   Etk_Bool built;

   int item_height;
   int image_height;
   int expander_size;
   Etk_Color separator_color;
   Etk_Color row_color1;
   Etk_Color row_color2;
   Etk_Color row_selected_color;
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
   
   Evas_Object *clip;
   Evas_Object *separator;
};

/**
 * @struct Etk_Tree_Row
 * @brief A row of a tree
 */
struct _Etk_Tree_Row
{
   /* private: */
   Etk_Tree *tree;
   Etk_Tree_Node node;

   Etk_Tree_Cell *cells;
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
      char *icon_filename;
      char *text;
   } icon_text_value;
   int int_value;
   double double_value;
};

Etk_Type *etk_tree_type_get();
Etk_Widget *etk_tree_new();

void etk_tree_mode_set(Etk_Tree *tree, Etk_Tree_Mode mode);
Etk_Tree_Mode etk_tree_mode_get(Etk_Tree *tree);

Etk_Tree_Col *etk_tree_col_new(Etk_Tree *tree, const char *title, Etk_Tree_Col_Type type);

void etk_tree_build(Etk_Tree *tree);
void etk_tree_freeze(Etk_Tree *tree);
void etk_tree_thaw(Etk_Tree *tree);

void etk_tree_multiple_select_set(Etk_Tree *tree, Etk_Bool multiple_select);
Etk_Bool etk_tree_multiple_select_get(Etk_Tree *tree);
void etk_tree_select_all(Etk_Tree *tree);
void etk_tree_unselect_all(Etk_Tree *tree);

void etk_tree_row_fold(Etk_Tree_Row *row);
void etk_tree_row_unfold(Etk_Tree_Row *row);

Etk_Tree_Row *etk_tree_append(Etk_Tree *tree, ...);
Etk_Tree_Row *etk_tree_append_to_row(Etk_Tree_Row *row, ...);

/** @} */

#endif
