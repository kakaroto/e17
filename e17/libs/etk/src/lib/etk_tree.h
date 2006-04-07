/** @file etk_tree.h */
#ifndef _ETK_TREE_H_
#define _ETK_TREE_H_

#include "etk_widget.h"
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
 * @enum Etk_Tree_Mode
 * @brief The mode of the tree: List (rows can not have children) or tree (rows can have children)
 */
enum _Etk_Tree_Mode
{
   ETK_TREE_MODE_LIST,
   ETK_TREE_MODE_TREE
};

/**
 * @struct Etk_Tree_Row
 * @brief A row of a tree
 */
struct _Etk_Tree_Row
{
   Etk_Tree *tree;

   Etk_Tree_Row *prev;
   Etk_Tree_Row *next;
   Etk_Tree_Row *parent;
   Etk_Tree_Row *first_child;
   Etk_Tree_Row *last_child;
   
   void **cells_data;
   void *data;
   void (*data_free_cb)(void *data);
   void *row_objects;
   
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
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Widget *scrolled_view;
   Etk_Widget *grid;

   int num_cols;
   Etk_Tree_Col **columns;
   Etk_Bool headers_visible;
   
   Etk_Tree_Col *column_to_resize;
   int col_to_resize_initial_width;
   int col_to_resize_initial_x;
   Etk_Bool resize_pointer_shown;
   
   Etk_Tree_Row root;
   Etk_Tree_Row *last_selected;
   int num_selected_rows;
   
   Etk_Tree_Col *last_sorted_col;
   Etk_Bool last_sorted_ascendant;

   Evas_Object *headers_clip;
   Evas_List *rows_widgets;

   Etk_Tree_Mode mode;
   Etk_Bool multiple_select;
   Etk_Bool frozen;
   Etk_Bool built;
   int xoffset;
   int yoffset;

   int row_height;
   Etk_Bool use_default_row_height;
   /* Left, right, top, bottom margins */
   int cell_margins[4];
   int expander_size;
   Etk_Color separator_color;
   
   Etk_Bool dnd_event;
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
   Etk_Tree_Model *model;

   int xoffset;
   int min_width;
   int requested_width;
   int width;
   int visible_width;
   int place;
   Etk_Bool resizable;
   Etk_Bool visible;
   Etk_Bool expand;
   
   Evas_Object *clip;
   Evas_Object *separator;

   Etk_Widget *header;
   
   struct
   {
      int (*compare_cb)(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data);
      void *data;
   } sort;
};

Etk_Type *etk_tree_type_get();
Etk_Widget *etk_tree_new();

void etk_tree_mode_set(Etk_Tree *tree, Etk_Tree_Mode mode);
Etk_Tree_Mode etk_tree_mode_get(Etk_Tree *tree);

Etk_Type *etk_tree_col_type_get();
Etk_Tree_Col *etk_tree_col_new(Etk_Tree *tree, const char *title, Etk_Tree_Model *model, int width);

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
void etk_tree_col_expand_set(Etk_Tree_Col *col, Etk_Bool expand);
Etk_Bool etk_tree_col_expand_get(Etk_Tree_Col *col);
void etk_tree_col_visible_set(Etk_Tree_Col *col, Etk_Bool visible);
Etk_Bool etk_tree_col_visible_get(Etk_Tree_Col *col);
void etk_tree_col_reorder(Etk_Tree_Col *col, int new_place);
int etk_tree_col_place_get(Etk_Tree_Col *col);
void etk_tree_col_sort_func_set(Etk_Tree_Col *col, int (*compare_cb)(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data), void *data);

void etk_tree_build(Etk_Tree *tree);
void etk_tree_freeze(Etk_Tree *tree);
void etk_tree_thaw(Etk_Tree *tree);

void etk_tree_row_height_set(Etk_Tree *tree, int row_height);
int etk_tree_row_height_get(Etk_Tree *tree);
void etk_tree_multiple_select_set(Etk_Tree *tree, Etk_Bool multiple_select);
Etk_Bool etk_tree_multiple_select_get(Etk_Tree *tree);
void etk_tree_select_all(Etk_Tree *tree);
void etk_tree_unselect_all(Etk_Tree *tree);

Etk_Tree_Row *etk_tree_append(Etk_Tree *tree, ...);
Etk_Tree_Row *etk_tree_append_valist(Etk_Tree *tree, va_list args);
Etk_Tree_Row *etk_tree_append_to_row(Etk_Tree_Row *row, ...);
void etk_tree_row_del(Etk_Tree_Row *row);
void etk_tree_clear(Etk_Tree *tree);

void etk_tree_sort(Etk_Tree *tree, int (*compare_cb)(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data), Etk_Bool ascendant, Etk_Tree_Col *col, void *data);

Etk_Tree_Row *etk_tree_first_row_get(Etk_Tree *tree);
Etk_Tree_Row *etk_tree_last_row_get(Etk_Tree *tree, Etk_Bool walking_through_hierarchy, Etk_Bool include_collapsed_children);
Etk_Tree_Row *etk_tree_row_first_child_get(Etk_Tree_Row *row);
Etk_Tree_Row *etk_tree_row_last_child_get(Etk_Tree_Row *row, Etk_Bool walking_through_hierarchy, Etk_Bool include_collapsed_children);
Etk_Tree_Row *etk_tree_prev_row_get(Etk_Tree_Row *row, Etk_Bool walking_through_hierarchy, Etk_Bool include_collapsed_children);
Etk_Tree_Row *etk_tree_next_row_get(Etk_Tree_Row *row, Etk_Bool walking_through_hierarchy, Etk_Bool include_collapsed_children);

void etk_tree_row_fields_set(Etk_Tree_Row *row, ...);
void etk_tree_row_fields_set_valist(Etk_Tree_Row *row, va_list args);
void etk_tree_row_fields_get(Etk_Tree_Row *row, ...);
void etk_tree_row_fields_get_valist(Etk_Tree_Row *row, va_list args);
void etk_tree_row_data_set(Etk_Tree_Row *row, void *data);
void etk_tree_row_data_set_full(Etk_Tree_Row *row, void *data, void (*free_cb)(void *data));
void *etk_tree_row_data_get(Etk_Tree_Row *row);

void etk_tree_row_scroll_to(Etk_Tree_Row *row, Etk_Bool center_the_row);
void etk_tree_row_select(Etk_Tree_Row *row);
void etk_tree_row_unselect(Etk_Tree_Row *row);
Etk_Tree_Row *etk_tree_selected_row_get(Etk_Tree *tree);
Evas_List *etk_tree_selected_rows_get(Etk_Tree *tree);

void etk_tree_row_expand(Etk_Tree_Row *row);
void etk_tree_row_collapse(Etk_Tree_Row *row);

/** @} */

#endif
