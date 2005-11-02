/** @file etk_tree.h */
#ifndef _ETK_TREE_H_
#define _ETK_TREE_H_

#include "etk_container.h"
#include <Evas.h>
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

/** @brief Gets the type of a tree column */
#define ETK_TREE_COL_TYPE        (etk_tree_col_type_get())
/** @brief Casts the object to an Etk_Tree_Col */
#define ETK_TREE_COL(obj)        (ETK_OBJECT_CAST((obj), ETK_TREE_COL_TYPE, Etk_Tree_Col))
/** @brief Check if the object is an Etk_Tree_Col */
#define ETK_IS_TREE_COL(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_TREE_COL_TYPE))

/**
 * @enum Etk_Tree_Col_Type
 * @brief The type of the objects of a column of a tree
 */
enum _Etk_Tree_Col_Type
{
   ETK_TREE_COL_TEXT,
   ETK_TREE_COL_IMAGE,
   ETK_TREE_COL_ICON_TEXT,
   ETK_TREE_COL_INT,
   ETK_TREE_COL_DOUBLE
};

/**
 * @enum Etk_Tree_Mode
 * @brief The mode of the tree: List (rows can not have children) or tree (rows can have children)
 */
enum _Etk_Tree_Mode
{
   ETK_TREE_MODE_LIST,
   ETK_TREE_MODE_TREE
};

/**
 * @struct Etk_Tree_Node
 * @brief A node of a tree contains several rows
 */
struct _Etk_Tree_Node
{
   Etk_Tree_Row *row;
   Etk_Tree_Node *parent;
   Evas_List *child_rows;
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
   Etk_Container container;

   Etk_Widget *scrolled_view;
   Etk_Widget *grid;

   int num_cols;
   Etk_Tree_Col **columns;
   Etk_Tree_Col *column_to_resize;
   Etk_Bool resize_pointer_shown;
   Etk_Bool headers_visible;
   
   Etk_Tree_Node root;
   Etk_Tree_Node *last_selected;

   Evas_Object *clip;
   Evas_List *items_objects;

   Etk_Tree_Mode mode;
   Etk_Bool multiple_select;
   Etk_Bool frozen;
   Etk_Bool built;
   int xoffset;
   int yoffset;

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
   /* Inherit form Etk_Object */
   Etk_Object object;

   int id;
   Etk_Tree *tree;
   Etk_Tree_Col_Type type;

   int xoffset;
   int requested_width;
   int width;
   int visible_width;
   int place;
   Etk_Bool resizable;
   Etk_Bool visible;
   float xalign;
   
   Evas_Object *clip;
   Evas_Object *separator;

   Etk_Widget *header;
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
   void *data;
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

Etk_Type *etk_tree_col_type_get();
Etk_Tree_Col *etk_tree_col_new(Etk_Tree *tree, const char *title, Etk_Tree_Col_Type type, int width);

int etk_tree_num_cols_get(Etk_Tree *tree);
Etk_Tree_Col *etk_tree_nth_col_get(Etk_Tree *tree, int nth);
void etk_tree_headers_visible_set(Etk_Tree *tree, Etk_Bool headers_visible);
Etk_Bool etk_tree_headers_visible_get(Etk_Tree *tree);

void etk_tree_col_title_set(Etk_Tree_Col *col, const char *title);
const char *etk_tree_col_title_get(Etk_Tree_Col *col);
void etk_tree_col_width_set(Etk_Tree_Col *col, int width);
int etk_tree_col_width_get(Etk_Tree_Col *col);
void etk_tree_col_min_width_set(Etk_Tree_Col *col, int min_width);
int etk_tree_col_min_width_get(Etk_Tree_Col *col);
void etk_tree_col_resizable_set(Etk_Tree_Col *col, Etk_Bool resizable);
Etk_Bool etk_tree_col_resizable_get(Etk_Tree_Col *col);
void etk_tree_col_visible_set(Etk_Tree_Col *col, Etk_Bool visible);
Etk_Bool etk_tree_col_visible_get(Etk_Tree_Col *col);
void etk_tree_col_reorder(Etk_Tree_Col *col, int new_place);
int etk_tree_col_place_get(Etk_Tree_Col *col);
void etk_tree_col_xalign_set(Etk_Tree_Col *col, float xalign);
float etk_tree_col_xalign_get(Etk_Tree_Col *col);

void etk_tree_build(Etk_Tree *tree);
void etk_tree_freeze(Etk_Tree *tree);
void etk_tree_thaw(Etk_Tree *tree);

void etk_tree_multiple_select_set(Etk_Tree *tree, Etk_Bool multiple_select);
Etk_Bool etk_tree_multiple_select_get(Etk_Tree *tree);
void etk_tree_select_all(Etk_Tree *tree);
void etk_tree_unselect_all(Etk_Tree *tree);

Etk_Tree_Row *etk_tree_append(Etk_Tree *tree, ...);
Etk_Tree_Row *etk_tree_append_to_row(Etk_Tree_Row *row, ...);
void etk_tree_row_fields_set(Etk_Tree_Row *row, ...);
void etk_tree_row_fields_set_valist(Etk_Tree_Row *row, va_list args);
void etk_tree_row_fields_get(Etk_Tree_Row *row, ...);
void etk_tree_row_fields_get_valist(Etk_Tree_Row *row, va_list args);
void etk_tree_row_del(Etk_Tree_Row *row);
void etk_tree_clear(Etk_Tree *tree);

void etk_tree_row_data_set(Etk_Tree_Row *row, void *data);
void *etk_tree_row_data_get(Etk_Tree_Row *row);

void etk_tree_row_select(Etk_Tree_Row *row);
void etk_tree_row_unselect(Etk_Tree_Row *row);
Etk_Tree_Row *etk_tree_selected_row_get(Etk_Tree *tree);
Evas_List *etk_tree_selected_rows_get(Etk_Tree *tree);
void etk_tree_row_expand(Etk_Tree_Row *row);
void etk_tree_row_collapse(Etk_Tree_Row *row);

/** @} */

#endif
