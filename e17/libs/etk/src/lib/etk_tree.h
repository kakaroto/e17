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

/** @file etk_tree.h */
#ifndef _ETK_TREE_H_
#define _ETK_TREE_H_

#include <stdarg.h>

#include <Evas.h>
#include <Ecore_Job.h>

#include "etk_widget.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Tree Etk_Tree
 * @brief A tree is widget that displays rows of elements of different types (text, image, checkbox, etc), separated
 * into columns
 * @{
 */


#define ETK_TREE_MAX_MODELS_PER_COL 5
#define ETK_TREE_MAX_OBJECTS_PER_MODEL 5

/** Gets the type of a tree */
#define ETK_TREE_TYPE       (etk_tree_type_get())
/** Casts the object to an Etk_Tree */
#define ETK_TREE(obj)       (ETK_OBJECT_CAST((obj), ETK_TREE_TYPE, Etk_Tree))
/** Check if the object is an Etk_Tree */
#define ETK_IS_TREE(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TREE_TYPE))

/** Gets the type of a tree column */
#define ETK_TREE_COL_TYPE        (etk_tree_col_type_get())
/** Casts the object to an Etk_Tree_Col */
#define ETK_TREE_COL(obj)        (ETK_OBJECT_CAST((obj), ETK_TREE_COL_TYPE, Etk_Tree_Col))
/** Check if the object is an Etk_Tree_Col */
#define ETK_IS_TREE_COL(obj)     (ETK_OBJECT_CHECK_TYPE((obj), ETK_TREE_COL_TYPE))

extern int ETK_TREE_ALL_SELECTED_SIGNAL;
extern int ETK_TREE_ALL_UNSELECTED_SIGNAL;
extern int ETK_TREE_ROW_SELECTED_SIGNAL;
extern int ETK_TREE_ROW_UNSELECTED_SIGNAL;
extern int ETK_TREE_ROW_CLICKED_SIGNAL;
extern int ETK_TREE_ROW_ACTIVATED_SIGNAL;
extern int ETK_TREE_ROW_UNFOLDED_SIGNAL;
extern int ETK_TREE_ROW_FOLDED_SIGNAL;
extern int ETK_TREE_ROW_SHOWN_SIGNAL;
extern int ETK_TREE_ROW_HIDDEN_SIGNAL;

extern int ETK_TREE_COL_CELL_VALUE_CHANGED_SIGNAL;

/** @brief The different modes of the tree: List (rows can not have children) or tree (rows can have children) */
typedef enum
{
   ETK_TREE_MODE_LIST,          /**< The rows of a list can not have children (the rows can not be folded/unfolded) */
   ETK_TREE_MODE_TREE           /**< The rows of a tree can have children */
} Etk_Tree_Mode;


/**
 * @brief @object A column of a tree
 * @structinfo
 */
struct Etk_Tree_Col
{
   /* private: */
   /* Inherit form Etk_Object */
   Etk_Object object;

   int id;
   int num_models;
   int position;
   int xoffset;
   int min_width;
   int width;
   int visible_width;
   float align;

   Etk_Tree *tree;

   Etk_Tree_Model *models[ETK_TREE_MAX_MODELS_PER_COL];

   Etk_Widget *header;
   Evas_Object *header_over;
   Evas_Object *clip;
   Evas_Object *separator;

   struct
   {
      int (*compare_cb)(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data);
      int (*last_compare_cb)(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data);
      void *data;
   } sort;

   Etk_Bool resizable:1;
   Etk_Bool visible:1;
   Etk_Bool expand:1;
};

/**
 * @brief A row of a tree
 * @structinfo
 */
struct Etk_Tree_Row
{
   Etk_Tree *tree;

   Etk_Tree_Row *prev;
   Etk_Tree_Row *next;
   Etk_Tree_Row *parent;
   Etk_Tree_Row *first_child;
   Etk_Tree_Row *last_child;
   int num_children;
   int num_visible_children;

   void ***cells_data;
   void *data;
   void (*data_free_cb)(void *data);

   Etk_Bool delete_me:1;
   Etk_Bool unfolded:1;
   Etk_Bool selected:1;
};

/**
 * @brief @widget A widget that displays rows of elements of different types, separated into columns
 * @structinfo
 */
struct Etk_Tree
{
   /* private: */
   /* Inherit from Etk_Widget */
   Etk_Widget widget;

   Etk_Widget *scrolled_view;
   Etk_Widget *scroll_content;
   Etk_Widget *grid;
   Evas_Object *grid_clip;

   Etk_Tree_Col **columns;
   Etk_Color col_separator_color;

   Etk_Tree_Col *col_to_resize;
   Etk_Tree_Col *sorted_col;

   Etk_Tree_Row root;
   Etk_Tree_Row *last_selected_row;
   Evas_List *purge_pool;
   Evas_List *row_objects;

   int num_cols;
   int rows_height;
   int scroll_x;
   int scroll_y;
   int col_resize_orig_width;
   int col_resize_orig_mouse_x;
   int total_rows;

   Ecore_Job *purge_job;
   Etk_Tree_Mode mode;
   Etk_Bool tree_contains_headers:1;
   Etk_Bool col_resize_pointer_set:1;
   Etk_Bool sorted_asc:1;
   Etk_Bool multiple_select:1;
   Etk_Bool alternating_row_colors:1;
   Etk_Bool headers_visible:1;
   Etk_Bool col_separators_visible:1;
   Etk_Bool frozen:1;
   Etk_Bool built:1;
};


Etk_Type      *etk_tree_type_get(void);
Etk_Type      *etk_tree_col_type_get(void);

Etk_Widget    *etk_tree_new(void);
void           etk_tree_mode_set(Etk_Tree *tree, Etk_Tree_Mode mode);
Etk_Tree_Mode  etk_tree_mode_get(Etk_Tree *tree);
void           etk_tree_multiple_select_set(Etk_Tree *tree, Etk_Bool multiple_select);
Etk_Bool       etk_tree_multiple_select_get(Etk_Tree *tree);
void           etk_tree_headers_visible_set(Etk_Tree *tree, Etk_Bool headers_visible);
Etk_Bool       etk_tree_headers_visible_get(Etk_Tree *tree);
void           etk_tree_column_separators_visible_set(Etk_Tree *tree, Etk_Bool col_separators_visible);
Etk_Bool       etk_tree_column_separators_visible_get(Etk_Tree *tree);
void           etk_tree_alternating_row_colors_set(Etk_Tree *tree, Etk_Bool alternating_row_colors);
Etk_Bool       etk_tree_alternating_row_colors_get(Etk_Tree *tree);
void           etk_tree_rows_height_set(Etk_Tree *tree, int rows_height);
int            etk_tree_rows_height_get(Etk_Tree *tree);

void           etk_tree_build(Etk_Tree *tree);
void           etk_tree_freeze(Etk_Tree *tree);
void           etk_tree_thaw(Etk_Tree *tree);

Etk_Tree_Col  *etk_tree_col_new(Etk_Tree *tree, const char *title, int width, float alignment);
int            etk_tree_num_cols_get(Etk_Tree *tree);
Etk_Tree_Col  *etk_tree_nth_col_get(Etk_Tree *tree, int nth);

Etk_Tree      *etk_tree_col_tree_get(Etk_Tree_Col *col);
void           etk_tree_col_model_add(Etk_Tree_Col *col, Etk_Tree_Model *model);
Etk_Widget    *etk_tree_col_header_get(Etk_Tree_Col *col);
void           etk_tree_col_title_set(Etk_Tree_Col *col, const char *title);
const char    *etk_tree_col_title_get(Etk_Tree_Col *col);
void           etk_tree_col_width_set(Etk_Tree_Col *col, int width);
int            etk_tree_col_width_get(Etk_Tree_Col *col);
void           etk_tree_col_min_width_set(Etk_Tree_Col *col, int min_width);
int            etk_tree_col_min_width_get(Etk_Tree_Col *col);
void           etk_tree_col_resizable_set(Etk_Tree_Col *col, Etk_Bool resizable);
Etk_Bool       etk_tree_col_resizable_get(Etk_Tree_Col *col);
void           etk_tree_col_expand_set(Etk_Tree_Col *col, Etk_Bool expand);
Etk_Bool       etk_tree_col_expand_get(Etk_Tree_Col *col);
void           etk_tree_col_alignment_set(Etk_Tree_Col *col, float alignment);
float          etk_tree_col_alignment_get(Etk_Tree_Col *col);
void           etk_tree_col_visible_set(Etk_Tree_Col *col, Etk_Bool visible);
Etk_Bool       etk_tree_col_visible_get(Etk_Tree_Col *col);
void           etk_tree_col_position_set(Etk_Tree_Col *col, int position);
int            etk_tree_col_position_get(Etk_Tree_Col *col);
void           etk_tree_col_sort_set(Etk_Tree_Col *col, int (*compare_cb)(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data), void *data);
void           etk_tree_col_sort(Etk_Tree_Col *col, Etk_Bool ascending);
void           etk_tree_col_sort_full(Etk_Tree_Col *col, int (*compare_cb)(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data), void *data, Etk_Bool ascending);

Etk_Tree_Row  *etk_tree_row_prepend(Etk_Tree *tree, Etk_Tree_Row *parent, ...);
Etk_Tree_Row  *etk_tree_row_append(Etk_Tree *tree, Etk_Tree_Row *parent, ...);
Etk_Tree_Row  *etk_tree_row_insert(Etk_Tree *tree, Etk_Tree_Row *parent, Etk_Tree_Row *after, ...);
Etk_Tree_Row  *etk_tree_row_insert_sorted(Etk_Tree *tree, Etk_Tree_Row *parent, ...);
Etk_Tree_Row  *etk_tree_row_insert_valist(Etk_Tree *tree, Etk_Tree_Row *parent, Etk_Tree_Row *after, va_list args);
void           etk_tree_row_delete(Etk_Tree_Row *row);
void           etk_tree_clear(Etk_Tree *tree);
/* TODO: way to insert a separator... */

void           etk_tree_row_fields_set(Etk_Tree_Row *row, Etk_Bool emit_signal, ...);
void           etk_tree_row_fields_set_valist(Etk_Tree_Row *row, Etk_Bool emit_signal, va_list args);
void           etk_tree_row_fields_get(Etk_Tree_Row *row, ...);
void           etk_tree_row_fields_get_valist(Etk_Tree_Row *row, va_list args);

void           etk_tree_row_model_fields_set(Etk_Tree_Row *row, Etk_Bool emit_signal, ...);
void           etk_tree_row_model_fields_set_valist(Etk_Tree_Row *row, Etk_Bool emit_signal, va_list args);
void           etk_tree_row_model_fields_get(Etk_Tree_Row *row, ...);
void           etk_tree_row_model_fields_get_valist(Etk_Tree_Row *row, va_list args);

void           etk_tree_row_data_set(Etk_Tree_Row *row, void *data);
void           etk_tree_row_data_set_full(Etk_Tree_Row *row, void *data, void (*free_cb)(void *data));
void          *etk_tree_row_data_get(Etk_Tree_Row *row);

Etk_Tree_Row  *etk_tree_selected_row_get(Etk_Tree *tree);
void           etk_tree_select_all(Etk_Tree *tree);
void           etk_tree_unselect_all(Etk_Tree *tree);
void           etk_tree_row_select(Etk_Tree_Row *row);
void           etk_tree_row_unselect(Etk_Tree_Row *row);
Etk_Bool       etk_tree_row_is_selected(Etk_Tree_Row *row);

void           etk_tree_row_fold(Etk_Tree_Row *row);
void           etk_tree_row_unfold(Etk_Tree_Row *row);
Etk_Bool       etk_tree_row_is_folded(Etk_Tree_Row *row);

Etk_Tree_Row  *etk_tree_first_row_get(Etk_Tree *tree);
Etk_Tree_Row  *etk_tree_last_row_get(Etk_Tree *tree);
Etk_Tree_Row  *etk_tree_row_parent_get(Etk_Tree_Row *row);
Etk_Tree_Row  *etk_tree_row_first_child_get(Etk_Tree_Row *row);
Etk_Tree_Row  *etk_tree_row_last_child_get(Etk_Tree_Row *row);
Etk_Tree_Row  *etk_tree_row_prev_get(Etk_Tree_Row *row);
Etk_Tree_Row  *etk_tree_row_next_get(Etk_Tree_Row *row);
Etk_Tree_Row  *etk_tree_row_walk_prev(Etk_Tree_Row *row, Etk_Bool include_folded);
Etk_Tree_Row  *etk_tree_row_walk_next(Etk_Tree_Row *row, Etk_Bool include_folded);
Etk_Tree      *etk_tree_row_tree_get(Etk_Tree_Row *row);

Etk_Tree_Row  *etk_tree_row_get_at_xy(Etk_Tree *tree, int x, int y);

Etk_Scrolled_View *etk_tree_scrolled_view_get(Etk_Tree *tree);
void           etk_tree_row_scroll_to(Etk_Tree_Row *row, Etk_Bool center);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
