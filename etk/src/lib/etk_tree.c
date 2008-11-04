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

/** @file etk_tree.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_tree.h"

#include <stdlib.h>
#include <string.h>

#include <Edje.h>

#include "etk_button.h"
#include "etk_event.h"
#include "etk_scrolled_view.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_theme.h"
#include "etk_toplevel.h"
#include "etk_tree_model.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Tree
 * @{
 */

#define TREE_GET(widget) \
   ETK_TREE(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Tree::Tree"))

#define COL_MIN_WIDTH 24
#define COL_RESIZE_THRESHOLD 3
#define MIN_ROW_HEIGHT 12
#define DEFAULT_ROW_HEIGHT 24
#define CELL_HMARGINS 4
#define CELL_VMARGINS 2
#define MODEL_INTERSPACE 8

int ETK_TREE_ALL_SELECTED_SIGNAL;
int ETK_TREE_ALL_UNSELECTED_SIGNAL;
int ETK_TREE_ROW_SELECTED_SIGNAL;
int ETK_TREE_ROW_UNSELECTED_SIGNAL;
int ETK_TREE_ROW_CLICKED_SIGNAL;
int ETK_TREE_ROW_ACTIVATED_SIGNAL;
int ETK_TREE_ROW_UNFOLDED_SIGNAL;
int ETK_TREE_ROW_FOLDED_SIGNAL;
int ETK_TREE_ROW_SHOWN_SIGNAL;
int ETK_TREE_ROW_HIDDEN_SIGNAL;

#define ETK_TREE_COL_NUM_SIGNALS 1
int ETK_TREE_COL_CELL_VALUE_CHANGED_SIGNAL;


typedef struct Etk_Tree_Cell_Objects
{
   Evas_Object *objects[ETK_TREE_MAX_MODELS_PER_COL][ETK_TREE_MAX_OBJECTS_PER_MODEL];
} Etk_Tree_Cell_Objects;

typedef struct Etk_Tree_Row_Object
{
   Evas_Object *expander;
   Evas_Object *background;
   Etk_Tree_Cell_Objects *cells;
   Etk_Tree_Row *row;
} Etk_Tree_Row_Object;

enum Etk_Tree_Property_Id
{
   ETK_TREE_MODE_PROPERTY,
   ETK_TREE_MULTIPLE_SELECT_PROPERTY,
   ETK_TREE_HEADERS_VISIBLE_PROPERTY,
   ETK_TREE_EXPANDERS_VISIBLE_PROPERTY,
   ETK_TREE_DEPTH_INDENT_PROPERTY,
   ETK_TREE_COLUMN_SEPARATORS_VISIBLE_PROPERTY,
   ETK_TREE_ALTERNATING_ROW_COLORS_PROPERTY,
   ETK_TREE_ROWS_HEIGHT_PROPERTY
};

enum Etk_Tree_Col_Property_Id
{
   ETK_TREE_COL_TITLE_PROPERTY,
   ETK_TREE_COL_VISIBLE_PROPERTY,
   ETK_TREE_COL_RESIZABLE_PROPERTY,
   ETK_TREE_COL_POSITION_PROPERTY,
   ETK_TREE_COL_EXPAND_PROPERTY,
   ETK_TREE_COL_WIDTH_PROPERTY,
   ETK_TREE_COL_MIN_WIDTH_PROPERTY,
   ETK_TREE_COL_ALIGN_PROPERTY
};


static void _etk_tree_constructor(Etk_Tree *tree);
static void _etk_tree_destructor(Etk_Tree *tree);
static void _etk_tree_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_tree_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_tree_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_tree_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static Etk_Bool _etk_tree_realized_cb(Etk_Object *object, void *data);

static void _etk_tree_col_constructor(Etk_Tree_Col *tree_col);
static void _etk_tree_col_destructor(Etk_Tree_Col *tree_col);
static void _etk_tree_col_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_tree_col_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

static void _etk_tree_headers_size_allocate(Etk_Tree *tree, Etk_Geometry geometry);
static void _etk_tree_scroll_content_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_tree_scroll_content_scroll(Etk_Widget *widget, int x, int y);
static void _etk_tree_scroll_content_scroll_size_get(Etk_Widget *widget, Etk_Size scrollview_size, Etk_Size scrollbar_size, Etk_Size *scroll_size);
static void _etk_tree_grid_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_tree_columns_geometry_calc(Etk_Tree *tree, int grid_width, Etk_Tree_Col **first_visible_col, Etk_Tree_Col **last_visible_col);
static void _etk_tree_columns_render(Etk_Tree *tree, Etk_Geometry grid_geometry, Etk_Tree_Col *last_visible_col);
static void _etk_tree_row_objects_cache(Etk_Tree *tree);
static void _etk_tree_row_objects_create_destroy(Etk_Tree *tree, Etk_Geometry grid_geometry);
static void _etk_tree_row_objects_update(Etk_Tree *tree, Etk_Geometry grid_geometry, Etk_Tree_Col *first_visible_col, Eina_List **prev_visible_rows, Eina_List **new_visible_rows);

static Etk_Bool _etk_tree_focused_cb(Etk_Object *object, void *event, void *data);
static Etk_Bool _etk_tree_unfocused_cb(Etk_Object *object, void *event, void *data);
static Etk_Bool _etk_tree_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static Etk_Bool _etk_tree_grid_realized_cb(Etk_Object *object, void *data);
static Etk_Bool _etk_tree_grid_unrealized_cb(Etk_Object *object, void *data);

static void _etk_tree_header_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree_header_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree_header_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree_header_mouse_in_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree_header_mouse_out_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

static void _etk_tree_row_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree_row_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree_row_expander_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

static void _etk_tree_purge_job(void *data);
static void _etk_tree_purge(Etk_Tree *tree);
static void _etk_tree_row_move_to_purge_pool(Etk_Tree_Row *row);

static void _etk_tree_col_realize(Etk_Tree *tree, int col_nth);
static Etk_Tree_Col *etk_tree_col_to_resize_get(Etk_Tree_Col *col, int x);

static Etk_Tree_Row *_etk_tree_row_next_to_render_get(Etk_Tree_Row *row, int *depth);
static Etk_Tree_Row_Object *_etk_tree_row_object_create(Etk_Tree *tree);
static void _etk_tree_row_object_destroy(Etk_Tree *tree, Etk_Tree_Row_Object *row_object);
static void _etk_tree_row_signal_emit(Etk_Tree_Row *row, Etk_Tree_Row_Object *row_object, const char *signal);
static void _etk_tree_expanders_clip(Etk_Tree *tree);

static void _etk_tree_row_select(Etk_Tree *tree, Etk_Tree_Row *row, Etk_Modifiers modifiers);
static void _etk_tree_heapify(Etk_Tree *tree, Etk_Tree_Row **heap, int root, int size, int (*compare_cb)(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data), int asc, Etk_Tree_Col *col, void *data);

static void _etk_tree_qsort(Etk_Tree_Row *first, Etk_Tree_Row *last, Etk_Tree_Col *col,
                            int (*compare_cb)(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data),
                            void *data, Etk_Bool ascending);
static void _etk_tree_partition(Etk_Tree_Row **first, Etk_Tree_Row **last, Etk_Tree_Row *pivot, Etk_Tree_Col *col,
                                int (*compare_cb)(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data),
                                void *data, Etk_Bool ascending);
static void _etk_tree_row_move_relatively(Etk_Tree_Row *center, Etk_Tree_Row *row, Etk_Bool after);
static void _etk_tree_reverse(Etk_Tree *tree);
static void _etk_tree_reverse_rows(Etk_Tree_Row *first);


/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Tree
 * @return Returns the type of an Etk_Tree
 */
Etk_Type *etk_tree_type_get(void)
{
   static Etk_Type *tree_type = NULL;

   if (!tree_type)
   {
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_TREE_ALL_SELECTED_SIGNAL,
            "all-selected", etk_marshaller_VOID),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_TREE_ALL_UNSELECTED_SIGNAL,
            "all-unselected", etk_marshaller_VOID),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_TREE_ROW_SELECTED_SIGNAL,
            "row-selected", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_TREE_ROW_UNSELECTED_SIGNAL,
            "row-unselected", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_TREE_ROW_CLICKED_SIGNAL,
            "row-clicked", etk_marshaller_POINTER_POINTER),

         /* TODO: do we really need "row-activated" */
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_TREE_ROW_ACTIVATED_SIGNAL,
            "row-activated", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_TREE_ROW_UNFOLDED_SIGNAL,
            "row-unfolded", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_TREE_ROW_FOLDED_SIGNAL,
            "row-folded", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_TREE_ROW_SHOWN_SIGNAL,
            "row-shown", etk_marshaller_POINTER),
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_TREE_ROW_HIDDEN_SIGNAL,
            "row-hidden", etk_marshaller_POINTER),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      tree_type = etk_type_new("Etk_Tree", ETK_WIDGET_TYPE, sizeof(Etk_Tree),
         ETK_CONSTRUCTOR(_etk_tree_constructor),
         ETK_DESTRUCTOR(_etk_tree_destructor), signals);

      etk_type_property_add(tree_type, "mode", ETK_TREE_MODE_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_TREE_MODE_LIST));
      etk_type_property_add(tree_type, "multiple-select", ETK_TREE_MULTIPLE_SELECT_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_type, "headers-visible", ETK_TREE_HEADERS_VISIBLE_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_type, "expanders-visible", ETK_TREE_EXPANDERS_VISIBLE_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_type, "depth-indent", ETK_TREE_DEPTH_INDENT_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(0));
      etk_type_property_add(tree_type, "column-separators-visible", ETK_TREE_COLUMN_SEPARATORS_VISIBLE_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_type, "alternating-row-colors", ETK_TREE_ALTERNATING_ROW_COLORS_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_type, "rows-height", ETK_TREE_ROWS_HEIGHT_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(DEFAULT_ROW_HEIGHT));

      tree_type->property_set = _etk_tree_property_set;
      tree_type->property_get = _etk_tree_property_get;
   }

   return tree_type;
}

/**
 * @internal
 * @brief Gets the type of an Etk_Tree_Col
 * @return Returns the type of an Etk_Tree_Col
 */
Etk_Type *etk_tree_col_type_get(void)
{
   static Etk_Type *tree_col_type = NULL;

   if (!tree_col_type)
   {
      const Etk_Signal_Description signals[] = {
         ETK_SIGNAL_DESC_NO_HANDLER(ETK_TREE_COL_CELL_VALUE_CHANGED_SIGNAL,
            "cell-value-changed", etk_marshaller_POINTER),
         ETK_SIGNAL_DESCRIPTION_SENTINEL
      };

      tree_col_type = etk_type_new("Etk_Tree_Col", ETK_OBJECT_TYPE,
         sizeof(Etk_Tree_Col), ETK_CONSTRUCTOR(_etk_tree_col_constructor),
         ETK_DESTRUCTOR(_etk_tree_col_destructor), signals);

      etk_type_property_add(tree_col_type, "title", ETK_TREE_COL_TITLE_PROPERTY,
            ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(tree_col_type, "visible", ETK_TREE_COL_VISIBLE_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_col_type, "resizable", ETK_TREE_COL_RESIZABLE_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_col_type, "position", ETK_TREE_COL_POSITION_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(0));
      etk_type_property_add(tree_col_type, "expand", ETK_TREE_COL_EXPAND_PROPERTY,
            ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(tree_col_type, "width", ETK_TREE_COL_WIDTH_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(COL_MIN_WIDTH));
      etk_type_property_add(tree_col_type, "min-width", ETK_TREE_COL_MIN_WIDTH_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(COL_MIN_WIDTH));
      etk_type_property_add(tree_col_type, "align", ETK_TREE_COL_ALIGN_PROPERTY,
            ETK_PROPERTY_FLOAT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_float(0.0));

      tree_col_type->property_set = _etk_tree_col_property_set;
      tree_col_type->property_get = _etk_tree_col_property_get;
   }

   return tree_col_type;
}

/**
 * @brief Creates a new tree
 * @return Returns the new tree
 */
Etk_Widget *etk_tree_new(void)
{
   return etk_widget_new(ETK_TREE_TYPE, "theme-group", "tree",
         "focusable", ETK_TRUE, "focus-on-click", ETK_TRUE, NULL);
}

/**
 * @brief Sets the mode of the tree. The tree must not be built
 * @param tree a tree
 * @param mode the mode the tree should use (ETK_TREE_MODE_LIST or ETK_TREE_MODE_TREE)
 */
void etk_tree_mode_set(Etk_Tree *tree, Etk_Tree_Mode mode)
{
   if (!tree)
      return;
   if (tree->built)
   {
      ETK_WARNING("Unable to change the mode of the tree because the tree is already built");
      return;
   }

   tree->mode = mode;
   etk_object_notify(ETK_OBJECT(tree), "mode");
}

/**
 * @brief Gets the mode of tree
 * @param tree a tree
 * @return Returns the mode of tree (ETK_TREE_MODE_LIST or ETK_TREE_MODE_TREE)
 */
Etk_Tree_Mode etk_tree_mode_get(Etk_Tree *tree)
{
   if (!tree)
      return ETK_TREE_MODE_LIST;
   return tree->mode;
}

/**
 * @brief Sets whether the user can select several rows of the tree
 * @param tree a tree
 * @param multiple_select ETK_TRUE to allow the user to select several rows, ETK_FALSE otherwise
 */
void etk_tree_multiple_select_set(Etk_Tree *tree, Etk_Bool multiple_select)
{
   if (!tree || tree->multiple_select == multiple_select)
      return;

   if (!multiple_select)
      etk_tree_unselect_all(tree);
   tree->multiple_select = multiple_select;
   etk_object_notify(ETK_OBJECT(tree), "multiple-select");
}

/**
 * @brief Gets whether or not several rows can be selected in the tree
 * @param tree a tree
 * @return ETK_TRUE if several rows can be selected, ETK_FALSE otherwise
 */
Etk_Bool etk_tree_multiple_select_get(Etk_Tree *tree)
{
   if (!tree)
      return ETK_FALSE;
   return tree->multiple_select;
}

/**
 * @brief Sets whether the column-headers should be visible or not
 * @param tree a tree
 * @param headers_visible ETK_TRUE to show the column-headers, ETK_FALSE to hide them
 */
void etk_tree_headers_visible_set(Etk_Tree *tree, Etk_Bool headers_visible)
{
   if (!tree || (tree->headers_visible == headers_visible))
      return;

   tree->headers_visible = headers_visible;
   etk_object_notify(ETK_OBJECT(tree), "headers-visible");
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Gets whether or not the column-headers are visible
 * @param tree a tree
 * @return Returns ETK_TRUE if the column-headers are visible, ETK_FALSE otherwise
 */
Etk_Bool etk_tree_headers_visible_get(Etk_Tree *tree)
{
   if (!tree)
      return ETK_FALSE;
   return tree->headers_visible;
}

/**
 * @brief Sets whether the row expanders should be visible or not. Default indentation will also be disabled, if expanders are not visible. To add custom depth indentation, etk_tree_depth_indent_set can be used()
 * @param tree a tree
 * @param expanders_visible ETK_TRUE to show the row expanders, ETK_FALSE to hide them
 * @see etk_tree_depth_indent_set()
 */
void etk_tree_expanders_visible_set(Etk_Tree *tree, Etk_Bool expanders_visible)
{
   if (!tree || (tree->expanders_visible == expanders_visible))
      return;

   tree->expanders_visible = expanders_visible;
   etk_object_notify(ETK_OBJECT(tree), "expanders-visible");
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Gets whether or not the row expanders are visible
 * @param tree a tree
 * @return Returns ETK_TRUE if the row expanders are visible, ETK_FALSE otherwise
 */
Etk_Bool etk_tree_expanders_visible_get(Etk_Tree *tree)
{
   if (!tree)
      return ETK_FALSE;
   return tree->expanders_visible;
}

/**
 * @brief Adds additional indentation per depth. Does not affect indentation of first-level rows.
 * @param tree a tree
 * @param indent_level ETK_TRUE to show the row expanders, ETK_FALSE to hide them
 */
void etk_tree_depth_indent_set(Etk_Tree *tree, Etk_Bool depth_indent)
{
   if (!tree || (tree->depth_indent == depth_indent))
      return;

   tree->depth_indent = depth_indent;
   etk_object_notify(ETK_OBJECT(tree), "depth-indent");
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Gets the amount of additional indent, in pixels, of child rows
 * @param tree a tree
 * @return Returns the amount of indentation in pixels
 */
Etk_Bool etk_tree_depth_indent_get(Etk_Tree *tree)
{
   if (!tree)
      return ETK_FALSE;
   return tree->depth_indent;
}

/**
 * @brief Sets whether the columns should be separated by a visible vertical separator
 * @param tree a tree
 * @param col_separators_visible ETK_TRUE to show the vertical separators, ETK_FALSE to hide them
 */
void etk_tree_column_separators_visible_set(Etk_Tree *tree, Etk_Bool col_separators_visible)
{
   if (!tree || (tree->col_separators_visible == col_separators_visible))
      return;

   tree->col_separators_visible = col_separators_visible;
   etk_object_notify(ETK_OBJECT(tree), "column-separators-visible");
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Gets whether or not the columns are separated by a visible vertical separator
 * @param tree a tree
 * @return Returns ETK_TRUE if the vertical separators are shown, ETK_FALSE otherwise
 */
Etk_Bool etk_tree_column_separators_visible_get(Etk_Tree *tree)
{
   if (!tree)
      return ETK_FALSE;
   return tree->col_separators_visible;
}

/**
 * @brief Sets whether the row-colors alternate (e.g. the odd rows are darker than the even rows)
 * @param tree a tree
 * @param alternating_row_colors ETK_TRUE to make the row-colors alternate, ETK_FALSE otherwise
 */
void etk_tree_alternating_row_colors_set(Etk_Tree *tree, Etk_Bool alternating_row_colors)
{
   if (!tree || (tree->alternating_row_colors == alternating_row_colors))
      return;

   tree->alternating_row_colors = alternating_row_colors;
   etk_object_notify(ETK_OBJECT(tree), "alternating-row-colors");
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Gets whether the row-colors of the tree alternate
 * @param tree a tree
 * @return Returns ETK_TRUE if the row-colors alternate, ETK_FALSE otherwise
 */
Etk_Bool etk_tree_alternating_row_colors_get(Etk_Tree *tree)
{
   if (!tree)
      return ETK_FALSE;
   return tree->alternating_row_colors;
}

/**
 * @brief Sets the height of the rows of the tree
 * @param tree a tree
 * @param rows_height the rows height to set. The minimum value is 12
 */
void etk_tree_rows_height_set(Etk_Tree *tree, int rows_height)
{
   if (!tree)
      return;

   rows_height = ETK_MAX(rows_height, MIN_ROW_HEIGHT);
   if (tree->rows_height != rows_height)
   {
      Etk_Range *vscrollbar;

      tree->rows_height = rows_height;
      vscrollbar = etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(tree->scrolled_view));
      etk_range_increments_set(vscrollbar, rows_height, 5 * rows_height);

      etk_object_notify(ETK_OBJECT(tree), "rows-height");
      etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(tree->scroll_content));
      etk_widget_redraw_queue(ETK_WIDGET(tree));
   }
}

/**
 * @brief Gets the height of the rows of the tree
 * @param tree a tree
 * @return Returns the height of the rows of the tree
 */
int etk_tree_rows_height_get(Etk_Tree *tree)
{
   if (!tree)
      return 0;
   return tree->rows_height;
}

/**
 * @brief Builds the tree. You have to call this function after you have added all the columns
 * and before you add the rows to the tree. Once the tree is built, it can not be unbuilt,
 * and you could not add new columns anymore
 * @param tree a tree
 */
void etk_tree_build(Etk_Tree *tree)
{
   if (!tree || tree->built)
      return;
   tree->built = ETK_TRUE;
}

/**
 * @brief Freezes the tree: the won't be updated until it is thawed. It is useful to add
 * a lot (> 1000) of rows efficiently. To add a few rows, freeze/thaw() is not necessary
 * @param tree a tree
 * @see etk_tree_thaw()
 */
void etk_tree_freeze(Etk_Tree *tree)
{
   if (!tree || tree->frozen)
      return;
   tree->frozen = ETK_TRUE;
}

/**
 * @brief Thaws the tree: the tree will be updated the tree if it was frozen
 * @param tree a tree
 * @see etk_tree_freeze()
 */
void etk_tree_thaw(Etk_Tree *tree)
{
   if (!tree || !tree->frozen)
      return;

   tree->frozen = ETK_FALSE;
   etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(tree->scroll_content));
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Inserts a new column into a tree
 * @param tree a tree
 * @param title the tile of the column
 * @param width the requested width of the column. It won't be necessary the visible width
 * of the column since it can be expanded to fit the available space
 * @param alignment the horizontal alignment of the objects inside the column, from 0.0 (left alignment)
 * to 1.0 (right alignment)
 * @return Returns the new column
 */
Etk_Tree_Col *etk_tree_col_new(Etk_Tree *tree, const char *title, int width, float alignment)
{
   Etk_Tree_Col *new_col;
   Etk_Widget *new_header;

   if (!tree)
      return NULL;
   if (tree->built)
   {
      ETK_WARNING("The tree is built, you can not add a new column");
      return NULL;
   }

   tree->columns = realloc(tree->columns, sizeof(Etk_Tree_Col *) * (tree->num_cols + 1));
   new_col = ETK_TREE_COL(etk_object_new(ETK_TREE_COL_TYPE, "title", title,
         "width", width, "visible", ETK_TRUE, "resizable", ETK_TRUE, "align", alignment, NULL));
   tree->columns[tree->num_cols] = new_col;

   new_col->id = tree->num_cols;
   new_col->tree = tree;
   new_col->position = tree->num_cols;

   /* Creates the header widget */
   new_header = etk_widget_new(ETK_BUTTON_TYPE, "theme-group", "header", "theme-parent", tree,
         "label", title, "xalign", 0.0, "internal", ETK_TRUE, NULL);
   if (tree->tree_contains_headers)
      etk_widget_parent_set(new_header, ETK_WIDGET(tree));
   else
      etk_widget_parent_set(new_header, tree->scroll_content);
   new_col->header = new_header;

   tree->num_cols++;
   _etk_tree_col_realize(tree, tree->num_cols - 1);

   return new_col;
}

/**
 * @brief Gets the number of columns of the tree
 * @param tree a tree
 * @return Returns the number of columns of the tree
 */
int etk_tree_num_cols_get(Etk_Tree *tree)
{
   if (!tree)
      return 0;
   return tree->num_cols;
}

/**
 * @brief Gets the "nth" column of the tree
 * @param tree a tree
 * @param nth the index of the column to get. Since the columns can be reordered or hidden,
 * @a nth corresponds to the "nth" created column, which is not necessarily the "nth" visible column
 * @return Returns the "nth" column of the tree
 */
Etk_Tree_Col *etk_tree_nth_col_get(Etk_Tree *tree, int nth)
{
   if (!tree || nth < 0 || nth >= tree->num_cols)
      return NULL;
   return tree->columns[nth];
}

/**
 * @brief Gets the tree which contains the given column
 * @param col a column
 * @return Returns the tree which contains the given column, or NULL on failure
 */
Etk_Tree *etk_tree_col_tree_get(Etk_Tree_Col *col)
{
   if (!col)
      return NULL;
   return col->tree;
}

/**
 * @brief Adds a model to a column of the tree. You can add several models to the same column in order to combine them.
 * For example, if you want the column's content to be an icon followed by a text, add the "image" model and then the
 * "text" model
 * @param col a column
 * @param model the model to add to the column @a col
 * @warning the number of models per column is limited to 5
 */
void etk_tree_col_model_add(Etk_Tree_Col *col, Etk_Tree_Model *model)
{
   if (!col || !model || !col->tree)
      return;
   if (col->tree->built)
   {
      ETK_WARNING("You cannot add a model to a column once the tree is built");
      return;
   }
   if (col->num_models >= ETK_TREE_MAX_MODELS_PER_COL)
   {
      ETK_WARNING("The number of models per column is limited to %d. Unable to add the model", ETK_TREE_MAX_MODELS_PER_COL);
      return;
   }
   if (model->col)
   {
      ETK_WARNING("The tree-model to add to that column is already used by another column");
      return;
   }

   col->models[col->num_models] = model;
   model->tree = col->tree;
   model->col = col;
   model->index = col->num_models;
   col->num_models++;
}

/**
 * @brief Gets the column's header widget. This can be used if you want to detect clicks on it for example.
 * The header widget is an Etk_Button
 * @param col a column
 * @return Returns the column's header widget
 */
Etk_Widget *etk_tree_col_header_get(Etk_Tree_Col *col)
{
   if (!col)
      return NULL;
   return col->header;
}

/**
 * @brief Sets the title of the column
 * @param col a column of a tree
 * @param title the title to set
 */
void etk_tree_col_title_set(Etk_Tree_Col *col, const char *title)
{
   if (!col || !col->header)
      return;

   etk_button_label_set(ETK_BUTTON(col->header), title);
   etk_object_notify(ETK_OBJECT(col), "title");
}

/**
 * @brief Gets the title of the column
 * @param col a column of a tree
 * @return Returns the title of the column
 */
const char *etk_tree_col_title_get(Etk_Tree_Col *col)
{
   if (!col || !col->header)
      return NULL;
   return etk_button_label_get(ETK_BUTTON(col->header));
}

/**
 * @brief Sets the width of the column. It won't be necessarily the visible width of the column since
 * the column can expand
 * @param col a column of a tree
 * @param width the width to set
 */
void etk_tree_col_width_set(Etk_Tree_Col *col, int width)
{
   if (!col || col->width == width)
      return;

   col->width = ETK_MAX(width, col->min_width);
   etk_object_notify(ETK_OBJECT(col), "width");

   if (col->tree)
   {
      etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(col->tree->scroll_content));
      etk_widget_redraw_queue(ETK_WIDGET(col->tree));
   }
}

/**
 * @brief Gets the width of the column
 * @param col a column of a tree
 * @return Returns the width of the column
 * @note This function doesn't take into account the extra-width if the column expands.
 */
int etk_tree_col_width_get(Etk_Tree_Col *col)
{
   if (!col)
      return 0;
   return col->width;
}

/**
 * @brief Sets the minimum width of the column. The column can not be smaller than this width
 * @param col a column of a tree
 * @param min_width the minimum width to set. -1 to use the default value
 */
void etk_tree_col_min_width_set(Etk_Tree_Col *col, int min_width)
{
   if (!col || (col->min_width == min_width))
      return;

   col->min_width = ETK_MAX(min_width, COL_MIN_WIDTH);
   if (col->width < col->min_width)
      etk_tree_col_width_set(col, col->min_width);

   etk_object_notify(ETK_OBJECT(col), "min-width");
}

/**
 * @brief Gets the minimum width of the column
 * @param col a column of a tree
 * @return Returns the minimum width of the column
 */
int etk_tree_col_min_width_get(Etk_Tree_Col *col)
{
   if (!col)
      return 0;
   return col->min_width;
}

/**
 * @brief Sets whether the column can be resized by the user
 * @param col a column of a tree
 * @param resizable ETK_TRUE to make the column resizable, ETK_FALSE otherwise
 */
void etk_tree_col_resizable_set(Etk_Tree_Col *col, Etk_Bool resizable)
{
   if (!col || (col->resizable == resizable))
      return;

   col->resizable = resizable;
   etk_object_notify(ETK_OBJECT(col), "resizable");
}

/**
 * @brief Gets whether the column can be resized by the user
 * @param col a column of a tree
 * @return Returns ETK_TRUE if the column is resizable
 */
Etk_Bool etk_tree_col_resizable_get(Etk_Tree_Col *col)
{
   if (!col)
      return ETK_FALSE;
   return col->resizable;
}

/**
 * @brief Sets whether the column should expand if there is some free space in the tree
 * @param col a column of a tree
 * @param expand ETK_TRUE to make the column expand, ETK_FALSE otherwise
 */
void etk_tree_col_expand_set(Etk_Tree_Col *col, Etk_Bool expand)
{
   if (!col || (col->expand == expand))
      return;

   col->expand = expand;
   etk_object_notify(ETK_OBJECT(col), "expand");
   etk_widget_redraw_queue(ETK_WIDGET(col->tree));
}

/**
 * @brief Gets whether the column expands
 * @param col a column of a tree
 * @return Returns ETK_TRUE if the column expands, ETK_FALSE otherwise
 */
Etk_Bool etk_tree_col_expand_get(Etk_Tree_Col *col)
{
   if (!col)
      return ETK_FALSE;
   return col->expand;
}

/**
 * @brief Sets the horizontal alignment of the objects inside the column
 * @param col a column of a tree
 * @param alignment the horizontal alignment to use, from 0.0 (left alignment) to 1.0 (right alignment)
 */
void etk_tree_col_alignment_set(Etk_Tree_Col *col, float alignment)
{
   if (!col || col->align == alignment)
      return;

   col->align = ETK_CLAMP(alignment, 0.0, 1.0);
   etk_object_notify(ETK_OBJECT(col), "align");

   if (col->tree)
      etk_widget_redraw_queue(ETK_WIDGET(col->tree));
}

/**
 * @brief Gets the horizontal alignment of the objects inside the column
 * @param col a column of a tree
 * @return Returns the horizontal alignment of the column, from 0.0 (left alignment) to 1.0 (right alignment)
 */
float etk_tree_col_alignment_get(Etk_Tree_Col *col)
{
   if (!col)
      return 0.0;
   return col->align;
}

/**
 * @brief Sets whether or not the column is visible
 * @param col a column of a tree
 * @param visible ETK_TRUE to show the column, ETK_FALSE to hide it
 */
void etk_tree_col_visible_set(Etk_Tree_Col *col, Etk_Bool visible)
{
   if (!col || (col->visible == visible))
      return;

   col->visible = visible;
   etk_object_notify(ETK_OBJECT(col), "visible");

   if (col->tree)
   {
      _etk_tree_expanders_clip(col->tree);
      etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(col->tree->scroll_content));
      etk_widget_redraw_queue(ETK_WIDGET(col->tree));
   }
}

/**
 * @brief Gets whether or not the column is visible
 * @param col a column of a tree
 * @return Returns ETK_TRUE if the column is visible
 */
Etk_Bool etk_tree_col_visible_get(Etk_Tree_Col *col)
{
   if (!col)
      return ETK_FALSE;
   return col->visible;
}

/**
 * @brief Changes the position of the column
 * @param col a column of a tree
 * @param position the new position the column should take (0 is the first column on the left of the tree,
 * "etk_tree_num_cols_get(tree) - 1" is the last one on the right)
 */
void etk_tree_col_position_set(Etk_Tree_Col *col, int position)
{
   int i;

   if (!col || !col->tree || (col->position == position))
      return;

   position = ETK_CLAMP(position, 0, col->tree->num_cols - 1);
   if (position < col->position)
   {
      for (i = 0; i < col->tree->num_cols; i++)
      {
         if (col->tree->columns[i]->position >= position && col->tree->columns[i]->position < col->position)
         {
            col->tree->columns[i]->position++;
            etk_object_notify(ETK_OBJECT(col->tree->columns[i]), "position");
         }
      }
   }
   else if (position > col->position)
   {
      for (i = 0; i < col->tree->num_cols; i++)
      {
         if (col->tree->columns[i]->position > col->position && col->tree->columns[i]->position <= position)
         {
            col->tree->columns[i]->position--;
            etk_object_notify(ETK_OBJECT(col->tree->columns[i]), "position");
         }
      }
   }
   col->position = position;
   etk_object_notify(ETK_OBJECT(col), "place");

   _etk_tree_expanders_clip(col->tree);
   etk_widget_redraw_queue(ETK_WIDGET(col->tree));
}

/**
 * @brief Gets the position of the column (0 is the first column on the left of the tree,
 * "etk_tree_num_cols_get(tree) - 1" is the last one on the right)
 * @param col a column of a tree
 * @return Returns the position of the column
 */
int etk_tree_col_position_get(Etk_Tree_Col *col)
{
   if (!col)
      return 0;
   return col->position;
}

/**
 * @brief Sets the sorting function of a column. This function will be called when
 * the header of the column is clicked or when you call etk_tree_col_sort()
 * @param col a column of a tree
 * @param compare_cb the function to call to compare two rows. It should return a negative
 * value if the cell of "row1" has a lower value than the cell of "row2", 0 if they have the
 * same value, and a positive value if the cell of "row2" has a greater value than the cell of "row1"
 * @param data a pointer that will be passed to @a compare_cb when it is called
 */
void etk_tree_col_sort_set(Etk_Tree_Col *col, int (*compare_cb)(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data), void *data)
{
   if (!col)
      return;

   col->sort.compare_cb = compare_cb;
   col->sort.data = data;
}

/**
 * @brief Sorts the rows of the tree according to the comparison function of the given column. You can set the
 * comparison function with etk_tree_col_sort_set(). If no comparison function is associated to the column, this
 * function will have no effect
 * @param col the column according to which the tree's rows should be sorted
 * @param ascending Etk_True to perform an ascendant sort, ETK_FALSE to perform a descendant sort
 */
void etk_tree_col_sort(Etk_Tree_Col *col, Etk_Bool ascending)
{
   if (!col)
      return;
   etk_tree_col_sort_full(col, col->sort.compare_cb, col->sort.data, ascending);
}

/**
 * @brief Sorts the rows of the tree according to the given comparison function
 * @param col the column that will be passed to @a compare_cb when it is called
 * @param compare_cb the function to call to compare two rows. It should return a negative
 * value if the cell of "row1" has a lower value than the cell of "row2", 0 if they have the
 * same value, and a positive value if the cell of "row1" has a greater value than the cell of "row2"
 * @param data a pointer that will be passed to @a compare_cb when it is called
 * @param ascending Etk_True to perform an ascendant sort, ETK_FALSE to perform a descendant sort
 */
void etk_tree_col_sort_full(Etk_Tree_Col *col, int (*compare_cb)(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data), void *data, Etk_Bool ascending)
{
   Etk_Tree *tree;
   Etk_Tree_Row *row, *to_sort;

   if (!col || !compare_cb || !(tree = col->tree) || (tree->sorted_col == col && tree->sorted_asc == ascending && col->sort.last_compare_cb == compare_cb))
      return;

   if (tree->sorted_col == col && tree->sorted_asc != ascending)
     {
       _etk_tree_reverse(col->tree);
       return;
     }
   col->sort.last_compare_cb = compare_cb;
   row = tree->root.first_child;

   to_sort = row;
   while (row || to_sort)
     {
       Etk_Tree_Row *next;
       if (to_sort)
         {
           Etk_Tree_Row *first = to_sort->parent->first_child;
           Etk_Tree_Row *last  = to_sort->parent->last_child;
           _etk_tree_qsort(first, last, col, compare_cb, data, ascending);
           to_sort = NULL;
           row = row->parent->first_child;
         }
       next = etk_tree_row_walk_next(row, ETK_TRUE);
       if (next && next->parent == row) to_sort = next;
       row = next;
     }

   tree->sorted_col = col;
   tree->sorted_asc = ascending;

   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * Inserts a new row at the beginning of the tree (if @a parent is NULL) or as the first child of an existing row
 * (if @a parent is not NULL and if the tree is in the "tree" mode)
 * @param tree a tree
 * @param parent the parent row of the row to insert. NULL means the row will be inserted at the tree's root
 * @param ... an "Etk_Tree_Col *" followed by the value of the cell, then any number of "Etk_Tree_Col *"/Value pairs,
 * and terminated by NULL. Note that, according to the model used by the column, a cell value can use several parameters
 * @return Returns the new row, or NULL on failure
 */
Etk_Tree_Row *etk_tree_row_prepend(Etk_Tree *tree, Etk_Tree_Row *parent, ...)
{
   Etk_Tree_Row *row;
   va_list args;

   if (!tree)
      return NULL;

   va_start(args, parent);
   row = etk_tree_row_insert_valist(tree, parent, NULL, args);
   va_end(args);

   return row;
}

/**
 * Inserts a new row at the end of the tree (if @a parent is NULL) or as the last child of an existing row
 * (if @a parent is not NULL and if the tree is in the "tree" mode)
 * @param tree a tree
 * @param parent the parent row of the row to insert. NULL means the row will be inserted at the tree's root
 * @param ... an "Etk_Tree_Col *" followed by the value of the cell, then any number of "Etk_Tree_Col *"/Value pairs,
 * and terminated by NULL. Note that, according to the model used by the column, a cell value can use several parameters
 * @return Returns the new row, or NULL on failure
 */
Etk_Tree_Row *etk_tree_row_append(Etk_Tree *tree, Etk_Tree_Row *parent, ...)
{
   Etk_Tree_Row *row;
   va_list args;

   if (!tree)
      return NULL;

   va_start(args, parent);
   if (!parent)
      parent = &tree->root;
   row = etk_tree_row_insert_valist(tree, parent, parent->last_child, args);
   va_end(args);

   return row;
}

/**
 * @brief Inserts a new row after an existing row
 * @param tree a tree
 * @param parent the parent row of the row to insert. If @a after is not NULL, @a parent is optionnal (the parent of the
 * new row will be the parent of @a after)
 * @param after the row after which the new row will be inserted. NULL means the new row will be inserted at the
 * beginning of the tree (if @a parent is NULL) or as the first child of @a parent (if @a parent is not NULL and if the
 * tree is in the "tree" mode)
 * @param ... an "Etk_Tree_Col *" followed by the value of the cell, then any number of "Etk_Tree_Col *"/Value pairs,
 * and terminated by NULL. Note that, according to the model used by the column, a cell value can use several parameters
 * @return Returns the new row, or NULL on failure
 */
Etk_Tree_Row *etk_tree_row_insert(Etk_Tree *tree, Etk_Tree_Row *parent, Etk_Tree_Row *after, ...)
{
   Etk_Tree_Row *row;
   va_list args;

   if (!tree)
      return NULL;

   va_start(args, after);
   row = etk_tree_row_insert_valist(tree, parent, after, args);
   va_end(args);

   return row;
}

/**
 * @brief Insert a new row in a sorted column at the right place
 * @param tree a tree
 * @param parent the parent row of the row to insert. If @a after is not NULL, @a parent is optionnal (the parent of the
 * new row will be the parent of @a after)
 * @param ... an "Etk_Tree_Col *" followed by the value of the cell, then any number of "Etk_Tree_Col *"/Value pairs,
 * and terminated by NULL. Note that, according to the model used by the column, a cell value can use several parameters
 * @return Returns the new row, or NULL on failure
 * @warning It will only work if the tree is already sorted.
 */
Etk_Tree_Row *etk_tree_row_insert_sorted(Etk_Tree *tree, Etk_Tree_Row *parent, ...)
{
   Etk_Tree_Row *row, *after;
   Etk_Tree_Col *col;
   va_list args;
   int i, j, direction;

   if (!tree || !(col = tree->sorted_col))
     return NULL;

   va_start(args, parent);
   if (!parent)
      parent = &tree->root;
   row = etk_tree_row_insert_valist(tree, parent, parent->last_child, args);
   va_end(args);

   after = parent->first_child;
   j = parent->num_children;
   direction = 1;
   do
     {
       j = j / 2 + j % 2;

       if (direction == 1)
         for (i = 0; after && i < j; after = after->next, i++);
       else if (direction == -1)
         for (i = 0; after && i < j; after = after->prev, i++);
       else
         break;

       direction = col->sort.compare_cb(col, row, after, col->sort.data) * (tree->sorted_asc ? 1 : -1);
     }
   while (j != 1);

   if (direction == -1)
     _etk_tree_row_move_relatively(after, row, ETK_FALSE);
   else
     _etk_tree_row_move_relatively(after, row, ETK_TRUE);

   tree->sorted_col = col;

   return row;
}

/**
 * @brief Inserts a new row after an existing row
 * @param tree a tree
 * @param parent the parent row of the row to insert. If @a after is not NULL, @a parent is optionnal (the parent of the
 * new row will be the parent of @a after)
 * @param after the row after which the new row will be inserted. NULL means the new row will be inserted at the
 * beginning of the tree (if @a parent is NULL) or as the first child of @a parent (if @a parent is not NULL and if the
 * tree is in the "tree" mode)
 * @param args an "Etk_Tree_Col *" followed by the value of the cell, then any number of "Etk_Tree_Col *"/Value pairs,
 * and terminated by NULL. Note that, according to the model used by the column, a cell value can use several parameters
 * @return Returns the new row, or NULL on failure
 */
Etk_Tree_Row *etk_tree_row_insert_valist(Etk_Tree *tree, Etk_Tree_Row *parent, Etk_Tree_Row *after, va_list args)
{
   Etk_Tree_Row *new_row;
   Etk_Tree_Col *col;
   va_list args2;
   int i, j;

   if (!tree)
      return NULL;
   if (!tree->built)
   {
      ETK_WARNING("The tree is not built yet, you can not add a row to the tree");
      return NULL;
   }

   if (after)
      parent = after->parent;
   if (!parent)
      parent = &tree->root;

   new_row = malloc(sizeof(Etk_Tree_Row));
   new_row->data = NULL;
   new_row->data_free_cb = NULL;
   new_row->delete_me = ETK_FALSE;
   new_row->selected = ETK_FALSE;
   new_row->unfolded = ETK_FALSE;

   /* Insert the row in the tree hierarchy */
   new_row->tree = tree;
   new_row->parent = parent;
   new_row->first_child = NULL;
   new_row->last_child = NULL;
   new_row->num_children = 0;
   new_row->num_visible_children = 0;
   if (after)
   {
      new_row->prev = after;
      new_row->next = after->next;
      if (after->next)
         after->next->prev = new_row;
      after->next = new_row;
      if (after == parent->last_child)
         parent->last_child = new_row;
   }
   else
   {
      new_row->prev = NULL;
      new_row->next = parent->first_child;
      if (parent->first_child)
         parent->first_child->prev = new_row;
      parent->first_child = new_row;
      if (!parent->last_child)
         parent->last_child = new_row;
   }
   parent->num_children++;
   for ( ; parent && parent->unfolded; parent = parent->parent)
      parent->num_visible_children++;
   tree->total_rows++;

   /* Initializes the data of the row's cells */
   new_row->cells_data = malloc(sizeof(void **) * tree->num_cols);
   for (i = 0; i < tree->num_cols; i++)
   {
      col = tree->columns[i];

      new_row->cells_data[i] = malloc(sizeof(void *) * col->num_models);
      for (j = 0; j < col->num_models; j++)
      {
         new_row->cells_data[i][j] = calloc(1, col->models[j]->cell_data_size);
         if (col->models[j]->cell_data_init)
            col->models[j]->cell_data_init(col->models[j], new_row->cells_data[i][j]);
      }
   }
   va_copy(args2, args);
   etk_tree_row_fields_set_valist(new_row, ETK_FALSE, args2);
   va_end(args2);


   if (tree->sorted_col) tree->sorted_col = NULL;
   if (!tree->frozen)
   {
      etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(tree->scroll_content));
      etk_widget_redraw_queue(ETK_WIDGET(tree));
   }
   return new_row;
}

/**
 * @brief Deletes an existing row and all its children. Note that the row will just be marked as deleted.
 * It will be effectively deleted only during the next iteration of Ecore's main loop. Thus, you can still manipulate
 * safely a row marked as deleted.
 * @param row the row to delete
 */
void etk_tree_row_delete(Etk_Tree_Row *row)
{
   if (!row || row->delete_me)
      return;

   _etk_tree_row_move_to_purge_pool(row);

   if (!row->tree->frozen)
   {
      etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(row->tree->scroll_content));
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
   }
}

/**
 * @brief Deletes all the rows of the tree. Note that the rows will just be marked as deleted. They will be effectively
 * deleted during the next iteration of Ecore's main loop. Thus, you can still manipulate safely the rows immediately
 * after a clear.
 */
void etk_tree_clear(Etk_Tree *tree)
{
   if (!tree)
      return;

   while (tree->root.first_child)
      _etk_tree_row_move_to_purge_pool(tree->root.first_child);

   etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(tree->scroll_content));
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Sets the values of the cells of the row
 * @param row a row of the tree
 * @param emit_signal whether or not the "cell-value-changed" signal should be emitted on the modified columns.
 * Most of the time, the signal don't need to be emitted (so @a emit_signal should be ETK_FALSE), except if you have a
 * callback connected on this signal
 * @param ... an "Etk_Tree_Col *" followed by the value of the cell,
 * then any number of "Etk_Tree_Col *"/Value pairs, and terminated by NULL.
 * Note that, according to the models used by the column, a cell value can use several parameters
 */
void etk_tree_row_fields_set(Etk_Tree_Row *row, Etk_Bool emit_signal, ...)
{
   va_list args;

   if (!row)
      return;

   va_start(args, emit_signal);
   etk_tree_row_fields_set_valist(row, emit_signal, args);
   va_end(args);
}

/**
 * @brief Sets the values of the cells of the row
 * @param row a row of the tree
 * @param emit_signal whether or not the "cell-value-changed" signal should be emitted on the modified columns.
 * Most of the time, the signal don't need to be emitted (so @a emit_signal should be ETK_FALSE), except if you have a
 * callback connected on this signal
 * @param args an "Etk_Tree_Col *" followed by the value of the cell,
 * then any number of "Etk_Tree_Col *"/Value pairs, and terminated by NULL.
 * Note that, according to the models used by the column, a cell value can use several parameters
 */
void etk_tree_row_fields_set_valist(Etk_Tree_Row *row, Etk_Bool emit_signal, va_list args)
{
   Etk_Tree_Col *col;
   va_list args2;
   int i;

   if (!row)
      return;

   va_copy(args2, args);
   while ((col = va_arg(args2, Etk_Tree_Col *)))
   {
      for (i = 0; i < col->num_models; i++)
      {
         if (col->models[i]->cell_data_set)
            col->models[i]->cell_data_set(col->models[i], row->cells_data[col->id][i], &args2);
      }
      if (col == row->tree->sorted_col)
         row->tree->sorted_col = NULL;
      if (emit_signal)
         etk_signal_emit(ETK_TREE_COL_CELL_VALUE_CHANGED_SIGNAL, ETK_OBJECT(col), row);
   }
   va_end(args2);

   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
}

/**
 * @brief Gets the values of the cells of the row
 * @param row a row of the tree
 * @param ... an "Etk_Tree_Col *" followed by the location where to store the value of the cell,
 * then any number of "Etk_Tree_Col *"/Location pairs, and terminated by NULL.
 * Note that some models may require several locations to store the cell value
 */
void etk_tree_row_fields_get(Etk_Tree_Row *row, ...)
{
   va_list args;

   if (!row)
      return;

   va_start(args, row);
   etk_tree_row_fields_get_valist(row, args);
   va_end(args);
}

/**
 * @brief Gets the values of the cells of the row
 * @param row a row of the tree
 * @param args an "Etk_Tree_Col *" followed by the location where to store the value of the cell,
 * then any number of "Etk_Tree_Col *"/Location pairs, and terminated by NULL.
 * Note that some models may require several locations to store the cell value
 */
void etk_tree_row_fields_get_valist(Etk_Tree_Row *row, va_list args)
{
   Etk_Tree_Col *col;
   va_list args2;
   int i;

   if (!row)
      return;

   va_copy(args2, args);
   while ((col = va_arg(args2, Etk_Tree_Col *)))
   {
      for (i = 0; i < col->num_models; i++)
      {
         if (col->models[i]->cell_data_get)
            col->models[i]->cell_data_get(col->models[i], row->cells_data[col->id][i], &args2);
      }
   }
   va_end(args2);
}

/**
 * @brief Sets the values of the models of the row
 * @param row a row of the tree
 * @param emit_signal whether or not the "cell-value-changed" signal should be emitted on the modified columns.
 * Most of the time, the signal don't need to be emitted (so @a emit_signal should be ETK_FALSE), except if you have a
 * callback connected on this signal
 * @param ... an "Etk_Tree_Model *" followed by the value of the model, then any number of
 * "Etk_Tree_Model *"/Value pairs, and terminated by NULL.
 * Note that, according to the models, a cell value can use several parameters
 */
void etk_tree_row_model_fields_set(Etk_Tree_Row *row, Etk_Bool emit_signal, ...)
{
   va_list args;

   if (!row)
      return;

   va_start(args, emit_signal);
   etk_tree_row_model_fields_set_valist(row, emit_signal, args);
   va_end(args);
}

/**
 * @brief Sets the values of the models of the row
 * @param row a row of the tree
 * @param emit_signal whether or not the "cell-value-changed" signal should be emitted on the modified columns.
 * Most of the time, the signal don't need to be emitted (so @a emit_signal should be ETK_FALSE), except if you have a
 * callback connected on this signal
 * @param args an "Etk_Tree_Model *" followed by the value of the model, then any number of
 * "Etk_Tree_Model *"/Value pairs, and terminated by NULL.
 * Note that, according to the models, a cell value can use several parameters
 */
void etk_tree_row_model_fields_set_valist(Etk_Tree_Row *row, Etk_Bool emit_signal, va_list args)
{
   Etk_Tree_Model *model;
   va_list args2;

   if (!row)
      return;

   va_copy(args2, args);
   while ((model = va_arg(args2, Etk_Tree_Model *)))
   {
      if (model->cell_data_set)
         model->cell_data_set(model, row->cells_data[model->col->id][model->index], &args2);
      if (emit_signal)
         etk_signal_emit(ETK_TREE_COL_CELL_VALUE_CHANGED_SIGNAL, ETK_OBJECT(model->col), row);
   }
   va_end(args2);

   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
}

/**
 * @brief Gets the values of the models of the row
 * @param row a row of the tree
 * @param ... an "Etk_Tree_Model *" followed by the location where to store the value of this model,
 * then any number of "Etk_Tree_Model *"/Location pairs, and terminated by NULL.
 * Note that some models may require several locations to store their value
 */
void etk_tree_row_model_fields_get(Etk_Tree_Row *row, ...)
{
   va_list args;

   if (!row)
      return;

   va_start(args, row);
   etk_tree_row_model_fields_get_valist(row, args);
   va_end(args);
}

/**
 * @brief Gets the values of the models of the row
 * @param row a row of the tree
 * @param args an "Etk_Tree_Model *" followed by the location where to store the value of this model,
 * then any number of "Etk_Tree_Model *"/Location pairs, and terminated by NULL.
 * Note that some models may require several locations to store their value
 */
void etk_tree_row_model_fields_get_valist(Etk_Tree_Row *row, va_list args)
{
   Etk_Tree_Model *model;
   va_list args2;

   if (!row)
      return;

   va_copy(args2, args);
   while ((model = va_arg(args2, Etk_Tree_Model *)))
   {
      if (model->cell_data_get)
         model->cell_data_get(model, row->cells_data[model->col->id][model->index], &args2);
   }
   va_end(args2);
}

/**
 * @brief Sets the user data associated to the row. The data could be retrieved later with etk_tree_row_data_get()
 * @param row a row of the tree
 * @param data the data to associate to the row
 * @note This is equivalent to etk_tree_row_data_set_full(row, data, NULL);
 */
void  etk_tree_row_data_set(Etk_Tree_Row *row, void *data)
{
   etk_tree_row_data_set_full(row, data, NULL);
}

/**
 * @brief Sets the user data associated to the row. The data could be retrieved later with etk_tree_row_data_get().
 * You can also set a function to call to free automatically the data when the row is deleted
 * @param row a row of the tree
 * @param data the data to associate to the row
 * @param free_cb a function to call to free the data automatically when the row is deleted
 */
void  etk_tree_row_data_set_full(Etk_Tree_Row *row, void *data, void (*free_cb)(void *data))
{
   if (!row)
      return;

   if (row->data && row->data_free_cb)
      row->data_free_cb(row->data);

   row->data = data;
   row->data_free_cb = free_cb;
}

/**
 * @brief Gets the user data associated to the row (previously set with etk_tree_row_data_set()).
 * @param row a row of the tree
 * @return Returns the user data associated to the row
 */
void *etk_tree_row_data_get(Etk_Tree_Row *row)
{
   if (!row)
      return NULL;
   return row->data;
}

/**
 * @brief Gets the row that was selected at last
 * @param tree a tree
 * @return Returns the row that was selected at last, or NULL if no row is selected
 * @note If you want to get all the selected rows (if multiple-selection is enabled), you have to walk through
 * all the rows, and see if they are selected with etk_tree_row_is_selected()
 */
Etk_Tree_Row *etk_tree_selected_row_get(Etk_Tree *tree)
{
   if (!tree || !tree->last_selected_row || !tree->last_selected_row->selected)
      return NULL;
   return tree->last_selected_row;
}

/**
 * @brief Selects all the rows of the tree
 * @param tree a tree
 * @note When you call etk_tree_select_all(), for performance reasons, the signal "row-selected" is not emitted for
 * each row of the tree. Only the signal "all-selected" is emitted on the tree.
 */
void etk_tree_select_all(Etk_Tree *tree)
{
   Etk_Tree_Row *row;

   if (!tree)
      return;

   for (row = tree->root.first_child; row; row = etk_tree_row_walk_next(row, ETK_TRUE))
      row->selected = ETK_TRUE;

   etk_signal_emit(ETK_TREE_ALL_SELECTED_SIGNAL, ETK_OBJECT(tree));
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Unselects all the rows of the tree
 * @param tree a tree
 * @note When you call etk_tree_unselect_all(), for performance reasons, the signal "row-unselected" is not emitted for
 * each row of the tree. Only the signal "all-unselected" is emitted on the tree.
 */
void etk_tree_unselect_all(Etk_Tree *tree)
{
   Etk_Tree_Row *row;

   if (!tree)
      return;

   for (row = tree->root.first_child; row; row = etk_tree_row_walk_next(row, ETK_TRUE))
      row->selected = ETK_FALSE;

   etk_signal_emit(ETK_TREE_ALL_UNSELECTED_SIGNAL, ETK_OBJECT(tree));
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Selects a row of the tree
 * @param row the row to select
 */
void etk_tree_row_select(Etk_Tree_Row *row)
{
   if (!row || row->selected || row->delete_me)
      return;
   _etk_tree_row_select(row->tree, row, ETK_MODIFIER_NONE);
}

/**
 * @brief Unselects a row of the tree
 * @param row the row to unselect
 */
void etk_tree_row_unselect(Etk_Tree_Row *row)
{
   if (!row || !row->selected || row->delete_me)
      return;

   row->selected = ETK_FALSE;
   etk_signal_emit(ETK_TREE_ROW_UNSELECTED_SIGNAL, ETK_OBJECT(row->tree), row);

   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
}

/**
 * @brief Gets whether or not the row is selected
 * @param row a row
 * @return Returns ETK_TRUE if the row is selected, ETK_FALSE otherwise
 */
Etk_Bool etk_tree_row_is_selected(Etk_Tree_Row *row)
{
   if (!row)
      return ETK_FALSE;
   return row->selected;
}

/**
 * @brief Folds a row of the tree: if the tree is in the "tree" mode, the children of the row won't be visible anymore
 * @param row the row to fold
 */
void etk_tree_row_fold(Etk_Tree_Row *row)
{
   Etk_Tree_Row *parent;

   if (!row || !row->unfolded || row->tree->mode != ETK_TREE_MODE_TREE)
      return;

   for (parent = row->parent; parent && parent->unfolded; parent = parent->parent)
      parent->num_visible_children -= row->num_visible_children;
   row->num_visible_children = 0;
   row->unfolded = ETK_FALSE;

   etk_signal_emit(ETK_TREE_ROW_FOLDED_SIGNAL, ETK_OBJECT(row->tree), row);
   if (!row->tree->frozen)
   {
      etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(row->tree->scroll_content));
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
   }
}

/**
 * @brief Unfolds a row of the tree: if the tree is in the "tree" mode, the children of the row will be visible
 * @param row the row to unfold
 */
void etk_tree_row_unfold(Etk_Tree_Row *row)
{
   Etk_Tree_Row *parent, *child;

   if (!row || row->unfolded || row->tree->mode != ETK_TREE_MODE_TREE)
      return;

   for (child = row->first_child; child; child = child->next)
      row->num_visible_children += 1 + child->num_visible_children;
   for (parent = row->parent; parent && parent->unfolded; parent = parent->parent)
      parent->num_visible_children += row->num_visible_children;
   row->unfolded = ETK_TRUE;

   etk_signal_emit(ETK_TREE_ROW_UNFOLDED_SIGNAL, ETK_OBJECT(row->tree), row);
   if (!row->tree->frozen)
   {
      etk_signal_emit(ETK_WIDGET_SCROLL_SIZE_CHANGED_SIGNAL, ETK_OBJECT(row->tree->scroll_content));
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
   }
}

/**
 * @brief Gets whether the row is folded or not
 * @param row a row
 * @return Returns ETK_TRUE if the row is folded, ETK_FALSE otherwise
 */
Etk_Bool etk_tree_row_is_folded(Etk_Tree_Row *row)
{
   if (!row)
      return ETK_TRUE;
   return !row->unfolded;
}

/**
 * @brief Gets the first row of the tree
 * @param tree a tree
 * @return Returns the first row of the tree, or NULL if the tree is empty
 */
Etk_Tree_Row *etk_tree_first_row_get(Etk_Tree *tree)
{
   if (!tree)
      return NULL;
   return tree->root.first_child;
}

/**
 * @brief Gets the last row of the tree
 * @param tree a tree
 * @return Returns the last row of the tree, or NULL if the tree is empty
 */
Etk_Tree_Row *etk_tree_last_row_get(Etk_Tree *tree)
{
   if (!tree)
      return NULL;
   return tree->root.last_child;
}

/**
 * @brief Gets the parent row of the specified row
 * @param row a row
 * @return Returns the parent row of the specified row, or NULL if the row is at the tree's root
 */
Etk_Tree_Row *etk_tree_row_parent_get(Etk_Tree_Row *row)
{
   if (!row)
      return NULL;
   if (!row->parent || row->parent == &row->tree->root)
      return NULL;
   return row->parent;
}

/**
 * @brief Gets the first child of the specified row
 * @param row a row
 * @return Returns the first child of the row
 */
Etk_Tree_Row *etk_tree_row_first_child_get(Etk_Tree_Row *row)
{
   if (!row)
      return NULL;
   return row->first_child;
}

/**
 * @brief Gets the last child of the specified row
 * @param row a row
 * @return Returns the last child of the row
 */
Etk_Tree_Row *etk_tree_row_last_child_get(Etk_Tree_Row *row)
{
   if (!row)
      return NULL;
   return row->last_child;
}

/**
 * @brief Gets the previous row before the specified row
 * @param row a row
 * @return Returns the previous row before the specified row, or NULL if the row is the first row of its parent
 * @note This function does not return the previous visible row, but only the previous row that is on the same
 * level as the specified row. So if some rows of the tree have children, you'll have to use etk_tree_row_walk_prev()
 * if you want to walk through all the rows of the tree.
 * @see etk_tree_row_walk_prev()
 */
Etk_Tree_Row *etk_tree_row_prev_get(Etk_Tree_Row *row)
{
   if (!row)
      return NULL;
   return row->prev;
}

/**
 * @brief Gets the next row after the specified row
 * @param row a row
 * @return Returns the next row after the specified row, or NULL if the row is the last row of its parent
 * @note This function does not return the next visible row, but only the next row that is on the same
 * level as the specified row. So if some rows of the tree have children, you'll have to use etk_tree_row_walk_next()
 * if you want to walk through all the rows of the tree.
 * @see etk_tree_row_walk_next()
 */
Etk_Tree_Row *etk_tree_row_next_get(Etk_Tree_Row *row)
{
   if (!row)
      return NULL;
   return row->next;
}

/**
 * @brief Walks to the previous "visible" row. Unlike etk_tree_row_prev_get(), etk_tree_row_walk_prev() can return a
 * row that is not on the same level as the specified row. It is useful if you want to walk easily through all the rows
 * of the tree.
 * @param row a row
 * @param include_folded if @a include_folded is ETK_TRUE, the function will return a row, even if this row is a child
 * of a folded row
 * @return Returns the previous "visible" row
 * @note if the tree is in the list mode, this is equivalent to etk_tree_row_prev_get(row)
 */
Etk_Tree_Row *etk_tree_row_walk_prev(Etk_Tree_Row *row, Etk_Bool include_folded)
{
   if (!row)
      return NULL;

   if (row->prev)
   {
      row = row->prev;
      while ((row->unfolded || include_folded) && row->last_child)
         row = row->last_child;
      return row;
   }
   else if (row->parent && row->parent != &row->tree->root)
      return row->parent;

   return NULL;
}

/**
 * @brief Walks to the next "visible" row. Unlike etk_tree_row_next_get(), etk_tree_row_walk_next() can return a row
 * that is not on the same level as the specified row. It is useful if you want to walk easily through all the rows of
 * the tree.
 * @param row a row
 * @param include_folded if @a include_folded is ETK_TRUE, the function will return a row, even if this row is a child
 * of a folded row
 * @return Returns the next "visible" row
 * @note if the tree is in the list mode, this is equivalent to etk_tree_row_next_get(row)
 */
Etk_Tree_Row *etk_tree_row_walk_next(Etk_Tree_Row *row, Etk_Bool include_folded)
{
   if (!row)
      return NULL;

   if ((row->unfolded || include_folded) && row->first_child)
      return row->first_child;
   else
   {
      while (row && !row->next)
         row = row->parent;
      return row ? row->next : NULL;
   }
}

/**
 * @brief Gets the tree which contains the given row
 * @param row a row
 * @return Returns the tree which contains the given row, or NULL on failure
 */
Etk_Tree *etk_tree_row_tree_get(Etk_Tree_Row *row)
{
   if (!row)
      return NULL;
   return row->tree;
}

/**
 * @brief Gets the row that is at the absolute position ( @a x, @a y )
 * @param tree a tree
 * @param x the x component of the position, relative to the canvas' top-left corner
 * @param y the y component of the position, relative to the canvas' top-left corner
 * @return Returns the row of the tree that is at the given position, or NULL if there is no row here
 */
Etk_Tree_Row  *etk_tree_row_get_at_xy(Etk_Tree *tree, int x, int y)
{
   Etk_Tree_Row *row;
   int gx, gy, gw, gh;
   int row_num, i;

   if (!tree)
      return NULL;

   etk_widget_inner_geometry_get(tree->grid, &gx, &gy, &gw, &gh);
   if (!ETK_INSIDE(x, y, gx, gy, gw, gh))
      return NULL;

   y = y - gy + tree->scroll_y;
   row_num = y / tree->rows_height;
   for (i = 0, row = etk_tree_first_row_get(tree); i < row_num && row; i++)
      row = etk_tree_row_walk_next(row, ETK_FALSE);

   return row;
}

/**
 * @brief Gets the scrolled view of the tree.
 * It can be used to change the scrollbars' policy, or to get the scroll-value
 * @param tree a tree
 * @return Returns the scrolled view of the tree
 */
Etk_Scrolled_View *etk_tree_scrolled_view_get(Etk_Tree *tree)
{
   if (!tree)
      return NULL;
   return ETK_SCROLLED_VIEW(tree->scrolled_view);
}

/**
 * @brief Scrolls to the given row of the tree, in order to make it visible
 * @param row the row to scroll to
 * @param center whether or not the row should be centered in the view
 */
void etk_tree_row_scroll_to(Etk_Tree_Row *row, Etk_Bool center)
{
   Etk_Tree *tree;
   Etk_Tree_Row *r;
   int i;

   if (!row || !(tree = row->tree))
      return;

   for (r = etk_tree_first_row_get(tree), i = 0; r; r = etk_tree_row_walk_next(r, ETK_FALSE), i++)
   {
      if (r == row)
      {
         int row_offset, new_offset;
         int tree_height;

         row_offset = i * tree->rows_height;
         etk_widget_inner_geometry_get(tree->grid, NULL, NULL, NULL, &tree_height);

         /* If the row is already entirely visible and the row should not be centered, we do nothing */
         if (center || (row_offset < tree->scroll_y)
            || ((row_offset + tree->rows_height) > (tree->scroll_y + tree_height)))
         {
            if (center)
               new_offset = row_offset + (tree->rows_height - tree_height) / 2;
            else if (row_offset < tree->scroll_y)
               new_offset = row_offset;
            else
               new_offset = row_offset - tree_height + tree->rows_height;

            etk_range_value_set(etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(tree->scrolled_view)), new_offset);
         }

         return;
      }
   }
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/**************************
 * Tree
 **************************/

/* Initializes the tree */
static void _etk_tree_constructor(Etk_Tree *tree)
{
   Etk_Range *vscrollbar;

   if (!tree)
      return;

   tree->scrolled_view = etk_scrolled_view_new();
   vscrollbar = etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(tree->scrolled_view));
   etk_range_increments_set(vscrollbar, DEFAULT_ROW_HEIGHT, 5 * DEFAULT_ROW_HEIGHT);
   etk_widget_theme_parent_set(tree->scrolled_view, ETK_WIDGET(tree));
   etk_widget_parent_set(tree->scrolled_view, ETK_WIDGET(tree));
   etk_widget_internal_set(tree->scrolled_view, ETK_TRUE);
   etk_widget_repeat_mouse_events_set(tree->scrolled_view, ETK_TRUE);
   etk_widget_show(tree->scrolled_view);

   tree->scroll_content = etk_widget_new(ETK_WIDGET_TYPE, "repeat-mouse-events", ETK_TRUE,
         "internal", ETK_TRUE, "visible", ETK_TRUE, NULL);
   etk_container_add(ETK_CONTAINER(tree->scrolled_view), tree->scroll_content);
   etk_object_data_set(ETK_OBJECT(tree->scroll_content), "_Etk_Tree::Tree", tree);
   tree->scroll_content->size_allocate = _etk_tree_scroll_content_size_allocate;
   tree->scroll_content->scroll = _etk_tree_scroll_content_scroll;
   tree->scroll_content->scroll_size_get = _etk_tree_scroll_content_scroll_size_get;

   tree->grid = etk_widget_new(ETK_WIDGET_TYPE, "theme-group", "grid", "theme-parent", tree,
         "repeat-mouse-events", ETK_TRUE, "internal", ETK_TRUE, "visible", ETK_TRUE, NULL);
   etk_widget_parent_set(tree->grid, tree->scroll_content);
   etk_object_data_set(ETK_OBJECT(tree->grid), "_Etk_Tree::Tree", tree);
   tree->grid->size_allocate = _etk_tree_grid_size_allocate;
   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(tree->grid),
         ETK_CALLBACK(_etk_tree_grid_realized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_UNREALIZED_SIGNAL, ETK_OBJECT(tree->grid),
         ETK_CALLBACK(_etk_tree_grid_unrealized_cb), NULL);

   tree->num_cols = 0;
   tree->columns = NULL;
   tree->col_to_resize = NULL;
   tree->sorted_col = NULL;
   tree->sorted_asc = ETK_TRUE;
   tree->headers_visible = ETK_TRUE;
   tree->expanders_visible = ETK_TRUE;
   tree->depth_indent = 0;
   tree->col_separators_visible = ETK_TRUE;
   tree->grid_clip = NULL;

   tree->root.tree = tree;
   tree->root.delete_me = ETK_FALSE;
   tree->root.selected = ETK_FALSE;
   tree->root.unfolded = ETK_TRUE;
   tree->root.data = NULL;
   tree->root.data_free_cb = NULL;
   tree->root.cells_data = NULL;

   tree->root.prev = NULL;
   tree->root.next = NULL;
   tree->root.parent = NULL;
   tree->root.first_child = NULL;
   tree->root.last_child = NULL;
   tree->root.num_children = 0;
   tree->root.num_visible_children = 0;

   tree->total_rows = 0;
   tree->last_selected_row = NULL;
   tree->purge_pool = NULL;
   tree->row_objects = NULL;

   tree->rows_height = -1;
   tree->scroll_x = 0;
   tree->scroll_y = 0;

   tree->purge_job = NULL;
   tree->mode = ETK_TREE_MODE_LIST;
   tree->multiple_select = ETK_FALSE;
   tree->alternating_row_colors = ETK_TRUE;
   tree->tree_contains_headers = ETK_TRUE;
   tree->col_resize_pointer_set = ETK_FALSE;

   tree->frozen = ETK_FALSE;
   tree->built = ETK_FALSE;


   ETK_WIDGET(tree)->size_request = _etk_tree_size_request;
   ETK_WIDGET(tree)->size_allocate = _etk_tree_size_allocate;

   etk_signal_connect_by_code(ETK_WIDGET_REALIZED_SIGNAL, ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_realized_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_FOCUSED_SIGNAL, ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_focused_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_UNFOCUSED_SIGNAL, ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_unfocused_cb), NULL);
   etk_signal_connect_by_code(ETK_WIDGET_KEY_DOWN_SIGNAL, ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_key_down_cb), NULL);
}

/* Destroys the tree */
static void _etk_tree_destructor(Etk_Tree *tree)
{
   if (!tree)
      return;

   /* TODO: more efficient cleaning?? */
   etk_tree_clear(tree);
   _etk_tree_purge(tree);

   if (tree->purge_job)
      ecore_job_del(tree->purge_job);

   free(tree->columns);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_tree_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Tree *tree;

   if (!(tree = ETK_TREE(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TREE_MODE_PROPERTY:
         etk_tree_mode_set(tree, etk_property_value_int_get(value));
         break;
      case ETK_TREE_MULTIPLE_SELECT_PROPERTY:
         etk_tree_multiple_select_set(tree, etk_property_value_bool_get(value));
         break;
      case ETK_TREE_HEADERS_VISIBLE_PROPERTY:
         etk_tree_headers_visible_set(tree, etk_property_value_bool_get(value));
         break;
      case ETK_TREE_EXPANDERS_VISIBLE_PROPERTY:
         etk_tree_expanders_visible_set(tree, etk_property_value_bool_get(value));
         break;
      case ETK_TREE_DEPTH_INDENT_PROPERTY:
         etk_tree_depth_indent_set(tree, etk_property_value_int_get(value));
         break;
      case ETK_TREE_COLUMN_SEPARATORS_VISIBLE_PROPERTY:
         etk_tree_column_separators_visible_set(tree, etk_property_value_bool_get(value));
         break;
      case ETK_TREE_ALTERNATING_ROW_COLORS_PROPERTY:
         etk_tree_alternating_row_colors_set(tree, etk_property_value_bool_get(value));
         break;
      case ETK_TREE_ROWS_HEIGHT_PROPERTY:
         etk_tree_rows_height_set(tree, etk_property_value_int_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_tree_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Tree *tree;

   if (!(tree = ETK_TREE(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TREE_MODE_PROPERTY:
         etk_property_value_int_set(value, tree->mode);
         break;
      case ETK_TREE_COLUMN_SEPARATORS_VISIBLE_PROPERTY:
         etk_property_value_bool_set(value, tree->col_separators_visible);
         break;
      case ETK_TREE_MULTIPLE_SELECT_PROPERTY:
         etk_property_value_bool_set(value, tree->multiple_select);
         break;
      case ETK_TREE_HEADERS_VISIBLE_PROPERTY:
         etk_property_value_bool_set(value, tree->headers_visible);
         break;
      case ETK_TREE_EXPANDERS_VISIBLE_PROPERTY:
         etk_property_value_bool_set(value, tree->expanders_visible);
         break;
      case ETK_TREE_DEPTH_INDENT_PROPERTY:
         etk_property_value_int_set(value, tree->depth_indent);
         break;
      case ETK_TREE_ALTERNATING_ROW_COLORS_PROPERTY:
         etk_property_value_bool_set(value, tree->alternating_row_colors);
         break;
      case ETK_TREE_ROWS_HEIGHT_PROPERTY:
         etk_property_value_int_set(value, tree->rows_height);
         break;
      default:
         break;
   }
}

/* Calculates the ideal size of the tree */
static void _etk_tree_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Tree *tree;
   Etk_Size header_size;
   int headers_height = 0;
   int i;

   if (!(tree = ETK_TREE(widget)) || !size)
      return;

   etk_widget_size_request(tree->scrolled_view, size);

   for (i = 0; i < tree->num_cols; i++)
   {
      etk_widget_size_request_full(tree->columns[i]->header, &header_size, ETK_FALSE);
      headers_height = ETK_MAX(headers_height, header_size.h);
   }
   size->h += headers_height;
}

/* Resizes the tree to the allocated size */
static void _etk_tree_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Tree *tree;
   Etk_Size header_size;
   Etk_Geometry view_geometry;
   int max_header_height;
   int i;

   if (!(tree = ETK_TREE(widget)))
      return;

   view_geometry = geometry;
   max_header_height = 0;

   /* Allocate size for the scrolled view */
   if (tree->tree_contains_headers && tree->headers_visible)
   {
      for (i = 0; i < tree->num_cols; i++)
      {
         etk_widget_size_request_full(tree->columns[i]->header, &header_size, ETK_FALSE);
         if (header_size.h > max_header_height)
            max_header_height = header_size.h;
      }

      view_geometry.y += max_header_height;
      view_geometry.h -= max_header_height;
   }
   etk_widget_size_allocate(tree->scrolled_view, view_geometry);

   /* Allocate size for the headers */
   if (tree->tree_contains_headers)
   {
      geometry.h = max_header_height;
      _etk_tree_headers_size_allocate(tree, geometry);
   }
}

/**************************
 * Tree Col
 **************************/

/* Initializes the tree column */
static void _etk_tree_col_constructor(Etk_Tree_Col *tree_col)
{
   if (!tree_col)
      return;

   tree_col->tree = NULL;
   tree_col->num_models = 0;
   tree_col->id = 0;
   tree_col->position = 0;
   tree_col->xoffset = 0;
   tree_col->min_width = COL_MIN_WIDTH;
   tree_col->width = COL_MIN_WIDTH;
   tree_col->visible_width = COL_MIN_WIDTH;
   tree_col->visible = ETK_TRUE;
   tree_col->resizable = ETK_TRUE;
   tree_col->expand = ETK_FALSE;
   tree_col->align = 0.0;
   tree_col->header = NULL;
   tree_col->header_over = NULL;
   tree_col->clip = NULL;
   tree_col->separator = NULL;
   tree_col->sort.compare_cb = NULL;
   tree_col->sort.data = NULL;
}

/* Destroys the tree column */
static void _etk_tree_col_destructor(Etk_Tree_Col *tree_col)
{
   int i;

   if (!tree_col)
      return;

   for (i = 0; i < tree_col->num_models; i++)
      etk_tree_model_free(tree_col->models[i]);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_tree_col_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Tree_Col *tree_col;

   if (!(tree_col = ETK_TREE_COL(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TREE_COL_TITLE_PROPERTY:
         etk_tree_col_title_set(tree_col, etk_property_value_string_get(value));
         break;
      case ETK_TREE_COL_RESIZABLE_PROPERTY:
         etk_tree_col_resizable_set(tree_col, etk_property_value_bool_get(value));
         break;
      case ETK_TREE_COL_EXPAND_PROPERTY:
         etk_tree_col_expand_set(tree_col, etk_property_value_bool_get(value));
         break;
      case ETK_TREE_COL_VISIBLE_PROPERTY:
         etk_tree_col_visible_set(tree_col, etk_property_value_bool_get(value));
         break;
      case ETK_TREE_COL_POSITION_PROPERTY:
         etk_tree_col_position_set(tree_col, etk_property_value_int_get(value));
         break;
      case ETK_TREE_COL_WIDTH_PROPERTY:
         etk_tree_col_width_set(tree_col, etk_property_value_int_get(value));
         break;
      case ETK_TREE_COL_MIN_WIDTH_PROPERTY:
         etk_tree_col_min_width_set(tree_col, etk_property_value_int_get(value));
         break;
      case ETK_TREE_COL_ALIGN_PROPERTY:
         etk_tree_col_alignment_set(tree_col, etk_property_value_float_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_tree_col_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Tree_Col *tree_col;

   if (!(tree_col = ETK_TREE_COL(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TREE_COL_TITLE_PROPERTY:
         etk_property_value_string_set(value, etk_tree_col_title_get(tree_col));
         break;
      case ETK_TREE_COL_RESIZABLE_PROPERTY:
         etk_property_value_bool_set(value, tree_col->resizable);
         break;
      case ETK_TREE_COL_EXPAND_PROPERTY:
         etk_property_value_bool_set(value, tree_col->expand);
         break;
      case ETK_TREE_COL_VISIBLE_PROPERTY:
         etk_property_value_bool_set(value, tree_col->visible);
         break;
      case ETK_TREE_COL_POSITION_PROPERTY:
         etk_property_value_int_set(value, tree_col->position);
         break;
      case ETK_TREE_COL_WIDTH_PROPERTY:
         etk_property_value_int_set(value, tree_col->width);
         break;
      case ETK_TREE_COL_MIN_WIDTH_PROPERTY:
         etk_property_value_int_set(value, tree_col->min_width);
         break;
      case ETK_TREE_COL_ALIGN_PROPERTY:
         etk_property_value_float_set(value, tree_col->align);
         break;
      default:
         break;
   }
}

/**************************
 * Tree Headers
 **************************/

/* Allocates size for the headers of the tree */
static void _etk_tree_headers_size_allocate(Etk_Tree *tree, Etk_Geometry geometry)
{
   int col_iter;

   if (!tree)
      return;

   if (tree->headers_visible)
   {
      Etk_Tree_Col *col;
      Etk_Tree_Col *first_visible_col;
      Etk_Tree_Col *last_visible_col;
      Etk_Geometry header_geometry;
      Etk_Geometry header_bar_geometry;
      int header_bar_xoffset;

      etk_widget_inner_geometry_get(tree->grid, &header_bar_geometry.x, NULL, &header_bar_geometry.w, NULL);
      header_bar_geometry.y = geometry.y;
      header_bar_geometry.h = geometry.h;
      header_bar_xoffset = header_bar_geometry.x - geometry.x;

      first_visible_col = NULL;
      last_visible_col = NULL;
      for (col_iter = 0; col_iter < tree->num_cols; col_iter++)
      {
         col = tree->columns[col_iter];
         if (col->visible
               && header_bar_geometry.x + col->xoffset <= geometry.x + geometry.w
               && header_bar_geometry.x + col->xoffset + col->visible_width >= geometry.x)
         {
            if (!first_visible_col || first_visible_col->position > col->position)
               first_visible_col = col;
            if (!last_visible_col || last_visible_col->position < col->position)
               last_visible_col = col;
         }
      }

      if (!first_visible_col || !last_visible_col)
          return;

      header_geometry.y = header_bar_geometry.y;
      header_geometry.h = header_bar_geometry.h;

      for (col_iter = 0; col_iter < tree->num_cols; col_iter++)
      {
         col = tree->columns[col_iter];
         if (col->visible
               && first_visible_col->position <= col->position
               && last_visible_col->position >= col->position)
         {
            header_geometry.x = header_bar_geometry.x + col->xoffset;
            header_geometry.w = col->visible_width;
            if (col == first_visible_col)
            {
               header_geometry.w += (header_geometry.x - geometry.x);
               header_geometry.x = geometry.x;
            }
            if (col == last_visible_col)
               header_geometry.w = geometry.x + geometry.w - header_geometry.x;

            /* TODO: what if the group doesn't exist... */
            if (col == first_visible_col && col == last_visible_col)
               etk_widget_theme_group_set(col->header, "header_unique");
            else if (col == first_visible_col)
               etk_widget_theme_group_set(col->header, "header_first");
            else if (col == last_visible_col)
               etk_widget_theme_group_set(col->header, "header_last");
            else
               etk_widget_theme_group_set(col->header, "header");

            etk_widget_show(col->header);
            etk_widget_raise(col->header);
            etk_widget_size_allocate(col->header, header_geometry);

            evas_object_show(col->header_over);
            evas_object_raise(col->header_over);
            evas_object_move(col->header_over, header_geometry.x, header_geometry.y);
            evas_object_resize(col->header_over, header_geometry.w, header_geometry.h);
         }
         else
         {
            etk_widget_hide(col->header);
            evas_object_hide(col->header_over);
         }
      }
   }
   else
   {
      for (col_iter = 0; col_iter < tree->num_cols; col_iter++)
      {
         etk_widget_hide(tree->columns[col_iter]->header);
         evas_object_hide(tree->columns[col_iter]->header_over);
      }
   }
}

/**************************
 * Tree Scroll Content
 **************************/

/* Allocates size for the tree's grid, and for the headers if they are not contained by the tree widget */
static void _etk_tree_scroll_content_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Tree *tree;
   Etk_Size header_size;
   Etk_Geometry grid_geometry;
   int max_header_height;
   int i;

   if (!(tree = TREE_GET(widget)))
      return;

   grid_geometry = geometry;
   max_header_height = 0;

   /* Allocate size for the grid */
   if (!tree->tree_contains_headers)
   {
      if (tree->headers_visible)
      {
         for (i = 0; i < tree->num_cols; i++)
         {
            etk_widget_size_request(tree->columns[i]->header, &header_size);
            if (header_size.h > max_header_height)
               max_header_height = header_size.h;
         }

         grid_geometry.y += max_header_height;
         grid_geometry.h -= max_header_height;
      }
   }
   etk_widget_size_allocate(tree->grid, grid_geometry);

   /* Allocate size for the headers */
   if (!tree->tree_contains_headers)
   {
      geometry.h = max_header_height;
      _etk_tree_headers_size_allocate(tree, geometry);
   }
}

/* Sets the scroll offset of the tree scroll-content */
static void _etk_tree_scroll_content_scroll(Etk_Widget *widget, int x, int y)
{
   Etk_Tree *tree;

   if (!(tree = TREE_GET(widget)))
      return;

   tree->scroll_x = x;
   tree->scroll_y = y;
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/* Gets the scrolling size of the tree scroll-content */
static void _etk_tree_scroll_content_scroll_size_get(Etk_Widget *widget, Etk_Size scrollview_size, Etk_Size scrollbar_size, Etk_Size *scroll_size)
{
   Etk_Tree *tree;
   int i;

   if (!(tree = TREE_GET(widget)) || !scroll_size)
      return;

   scroll_size->w = 0;
   for (i = 0; i < tree->num_cols; i++)
   {
      if (tree->columns[i]->visible)
         scroll_size->w += tree->columns[i]->width;
   }
   scroll_size->h = tree->root.num_visible_children * tree->rows_height;
}

/**************************
 * Tree Grid
 **************************/

/* Allocates size for the grid: it first calculates the width of the visible cols,
 * then creates/destroys the rows objects according to the new height of the tree grid,
 * and finally it updates the content of the rows */
static void _etk_tree_grid_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Tree *tree;
   Etk_Tree_Col *first_visible_col;
   Etk_Tree_Col *last_visible_col;
   Eina_List *prev_visible_rows;
   Eina_List *new_visible_rows;
   Eina_List *l;
   Evas *evas;

   if (!(tree = TREE_GET(widget)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(tree))))
      return;

   /* First, we calculate the size of the visible cols */
   _etk_tree_columns_geometry_calc(tree, geometry.w, &first_visible_col, &last_visible_col);

   /* Move/Resize the clipper of the grid */
   if (tree->root.num_children > 0)
   {
      evas_object_move(tree->grid_clip, geometry.x, geometry.y);
      evas_object_resize(tree->grid_clip, geometry.w, geometry.h);
      evas_object_show(tree->grid_clip);
   }
   else
      evas_object_hide(tree->grid_clip);

   if (!tree->built)
      return;

   /* Remove the rows marked as "deleted" */
   if (tree->purge_job)
   {
      _etk_tree_purge(tree);
      tree->purge_job = NULL;
   }

   /* Save the rows currently rendered so we could emit the "row-shown/hidden" signals later */
   prev_visible_rows = NULL;
   new_visible_rows = NULL;
   for (l = tree->row_objects; l; l = l->next)
   {
      Etk_Tree_Row_Object *row_object;

      row_object = l->data;
      if (row_object->row)
         prev_visible_rows = eina_list_append(prev_visible_rows, row_object->row);
   }

   /* Move, resize, and show/hide the column separators and clips */
   _etk_tree_columns_render(tree, geometry, last_visible_col);

   /* Cache the row-objects */
   _etk_tree_row_objects_cache(tree);

   /* Create or destroy row objects if the height of the grid has changed */
   _etk_tree_row_objects_create_destroy(tree, geometry);

   /* Update the position and the content of the row objects */
   _etk_tree_row_objects_update(tree, geometry, first_visible_col, &prev_visible_rows, &new_visible_rows);


   /* Emit the "row-shown/hidden" signals */
   while (prev_visible_rows)
   {
      etk_signal_emit(ETK_TREE_ROW_HIDDEN_SIGNAL, ETK_OBJECT(tree), prev_visible_rows->data);
      prev_visible_rows = eina_list_remove_list(prev_visible_rows, prev_visible_rows);
   }
   while (new_visible_rows)
   {
      etk_signal_emit(ETK_TREE_ROW_SHOWN_SIGNAL, ETK_OBJECT(tree), new_visible_rows->data);
      new_visible_rows = eina_list_remove_list(new_visible_rows, new_visible_rows);
   }
}

/* Recalculates the width and the position of each columns of the tree. Used by etk_tree_grid_size_allocate() only */
static void _etk_tree_columns_geometry_calc(Etk_Tree *tree, int grid_width, Etk_Tree_Col **first_visible_col, Etk_Tree_Col **last_visible_col)
{
   Etk_Tree_Col *col;
   Etk_Tree_Col *first_visible;
   Etk_Tree_Col *last_visible;
   int num_expand_cols;
   int columns_width;
   int freespace;
   int xoffset;
   int col_iter;
   int i;

   if (!tree)
      return;

   first_visible = NULL;
   last_visible = NULL;
   columns_width = 0;
   num_expand_cols = 0;

   /* Gets the first and the last visible columns */
   for (col_iter = 0; col_iter < tree->num_cols; col_iter++)
   {
      col = tree->columns[col_iter];
      if (col->visible)
      {
         if (!first_visible || first_visible->position > col->position)
            first_visible = col;
         if (!last_visible || last_visible->position < col->position)
            last_visible = col;

         col->visible_width = col->width;
         columns_width += col->width;

         if (col->expand)
            num_expand_cols++;
      }
   }

   /* At least one column is visible */
   if (first_visible)
   {
      /* Calculate the width of the visible columns */
      freespace = grid_width - columns_width;
      if (freespace > 0)
      {
         if (num_expand_cols > 0)
         {
            for (col_iter = 0; col_iter < tree->num_cols; col_iter++)
            {
               col = tree->columns[col_iter];
               if (col->visible && col->expand)
               {
                  col->width += freespace / num_expand_cols;
                  columns_width += freespace / num_expand_cols;
                  col->visible_width += freespace / num_expand_cols;
               }
            }
         }

         /* Give the last column the remaining space */
         last_visible->visible_width += (grid_width - columns_width);
      }

      /* Calculate the horizontal position of the visible columns */
      xoffset = -tree->scroll_x;
      for (i = 0; i < tree->num_cols; i++)
      {
         for (col_iter = 0; col_iter < tree->num_cols; col_iter++)
         {
            col = tree->columns[col_iter];
            if (col->position == i)
            {
               if (col->visible)
               {
                  col->xoffset = xoffset;
                  xoffset += col->visible_width;
               }
               break;
            }
         }
      }
   }

   if (first_visible_col)
      *first_visible_col = first_visible;
   if (last_visible_col)
      *last_visible_col = last_visible;
}

/* Move, resize, and show/hide the column separators and clips. Used by etk_tree_grid_size_allocate() only */
static void _etk_tree_columns_render(Etk_Tree *tree, Etk_Geometry grid_geometry, Etk_Tree_Col *last_visible_col)
{
   Etk_Tree_Col *col;
   int col_iter;

   for (col_iter = 0; col_iter < tree->num_cols; col_iter++)
   {
      col = tree->columns[col_iter];

      if (!col->visible)
      {
         evas_object_hide(col->clip);
         evas_object_hide(col->separator);
      }
      else
      {
         int col_x1, col_x2;

         col_x1 = ETK_MAX(0, col->xoffset);
         col_x2 = ETK_MIN(grid_geometry.w, col->xoffset + col->visible_width);

         if (col_x1 <= col_x2)
         {
            evas_object_move(col->clip, grid_geometry.x + col_x1, grid_geometry.y);
            evas_object_resize(col->clip, col_x2 - col_x1 + 1, grid_geometry.h);
            evas_object_show(col->clip);

            if (col_x2 >= 0 && col_x2 < grid_geometry.w && col != last_visible_col && tree->col_separators_visible)
            {
               evas_object_move(col->separator, grid_geometry.x + col_x2, grid_geometry.y);
               evas_object_resize(col->separator, 1, grid_geometry.h);
               evas_object_show(col->separator);
            }
            else
               evas_object_hide(col->separator);
         }
         else
         {
            evas_object_hide(col->clip);
            evas_object_hide(col->separator);
         }
      }
   }
}

/* Cache the sub-objects of the visible rows of the tree. Used by etk_tree_grid_size_allocate() only */
static void _etk_tree_row_objects_cache(Etk_Tree *tree)
{
   Eina_List *l;
   Etk_Tree_Col *col;
   Etk_Tree_Model *model;
   void *cell_data;
   int col_iter, model_iter;

   for (col_iter = 0; col_iter < tree->num_cols; col_iter++)
   {
      Etk_Tree_Row_Object *row_object;

      col = tree->columns[col_iter];
      for (model_iter = 0; model_iter < col->num_models; model_iter++)
      {
         model = col->models[model_iter];

         if (model->objects_cache)
         {
            for (l = tree->row_objects; l; l = l->next)
            {
               row_object = l->data;

               if (row_object->row)
                  cell_data = row_object->row->cells_data[col_iter][model_iter];
               else
                  cell_data = NULL;

               model->objects_cache(model, cell_data, row_object->cells[col_iter].objects[model_iter]);
            }
         }
      }
   }
}

/* Create or destroy row-objects if the height of the grid has changed. Used by etk_tree_grid_size_allocate() only */
static void _etk_tree_row_objects_create_destroy(Etk_Tree *tree, Etk_Geometry grid_geometry)
{
   int num_visible_rows;
   int current_num_rows;
   int i;

   num_visible_rows = (grid_geometry.h / tree->rows_height) + 2;
   current_num_rows = eina_list_count(tree->row_objects);

   if (current_num_rows < num_visible_rows)
   {
      /* We add more row objects */
      for (i = 0; i < (num_visible_rows - current_num_rows); i++)
      {
         Etk_Tree_Row_Object *row_object;

         if ((row_object = _etk_tree_row_object_create(tree)))
            tree->row_objects = eina_list_append(tree->row_objects, row_object);
      }
   }
   else if (current_num_rows > num_visible_rows)
   {
      Eina_List *last, *prev;

      /* We destroy the row objects that are no longer needed */
      last = eina_list_last(tree->row_objects);
      for (i = 0; i < (current_num_rows - num_visible_rows) && last; i++)
      {
         _etk_tree_row_object_destroy(tree, last->data);
         prev = last->prev;
         tree->row_objects = eina_list_remove_list(tree->row_objects, last);
         last = prev;
      }
   }
}

/* Update the position and the content of the row objects. Used by etk_tree_grid_size_allocate() only */
static void _etk_tree_row_objects_update(Etk_Tree *tree, Etk_Geometry grid_geometry, Etk_Tree_Col *first_visible_col, Eina_List **prev_visible_rows, Eina_List **new_visible_rows)
{
   Evas *evas;
   Evas_Object *obj;
   Eina_List *l, *l2;
   Etk_Tree_Col *col;
   Etk_Tree_Row *row;
   Etk_Tree_Row_Object *row_object;
   Etk_Geometry cell_geometry, model_geometry;
   Etk_Bool show_expanders;
   Etk_Bool objects_created;
   int expander_w, expander_h;
   int x, y;
   int total_width, w;
   int row_id;
   int row_y;
   int depth;
   int col_iter, model_iter;
   int i;

   if (!tree || !first_visible_col || !new_visible_rows || !prev_visible_rows)
      return;
   if (!(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(tree))))
      return;

   depth = 0;
   row_id = tree->scroll_y / tree->rows_height;
   row_y = -(tree->scroll_y % tree->rows_height);
   for (row = tree->root.first_child, i = 0; i < row_id && row; i++)
      row = _etk_tree_row_next_to_render_get(row, &depth);
   show_expanders = (tree->expanders_visible && tree->total_rows > tree->root.num_children && first_visible_col);

   for (l = tree->row_objects; l; l = l->next)
   {
      row_object = l->data;

      if (row)
      {
         /* If there is still a row to render, we render the content of the row */
         evas_object_move(row_object->background, grid_geometry.x, grid_geometry.y + row_y);
         evas_object_resize(row_object->background, grid_geometry.w, tree->rows_height);
         evas_object_show(row_object->background);

         if ((l2 = eina_list_data_find_list(*prev_visible_rows, row)))
            *prev_visible_rows = eina_list_remove_list(*prev_visible_rows, l2);
         else
            *new_visible_rows = eina_list_append(*new_visible_rows, row);

         /* Show the expander if needed */
         if (show_expanders && row->num_children > 0)
         {
            edje_object_signal_emit(row_object->expander,
                  row->unfolded ? "etk,action,unfold" : "etk,action,fold", "etk");
            edje_object_message_signal_process(row_object->expander);
            evas_object_show(row_object->expander);
         }
         else
            evas_object_hide(row_object->expander);

         /* Render the sub-objects of the row's cells */
         for (col_iter = 0; col_iter < tree->num_cols; col_iter++)
         {
            col = tree->columns[col_iter];
            if (col->visible)
            {
               cell_geometry.x = grid_geometry.x + col->xoffset + CELL_HMARGINS;
               cell_geometry.y = grid_geometry.y + row_y + CELL_VMARGINS;
               cell_geometry.w = col->visible_width - (2 * CELL_HMARGINS);
               cell_geometry.h = tree->rows_height - (2 * CELL_VMARGINS);

               /* Render the expander of the row */
               if (col == first_visible_col)
               {
                  if (show_expanders)
                  {
                     edje_object_size_min_get(row_object->expander, &expander_w, &expander_h);
                     if (row->num_children > 0)
                     {
                        evas_object_move(row_object->expander,
                              cell_geometry.x + (depth * expander_w), cell_geometry.y);
                        evas_object_resize(row_object->expander, expander_w, expander_h);
                     }
                     cell_geometry.x += ((depth + 1) * expander_w) + CELL_HMARGINS;
                     cell_geometry.w -= ((depth + 1) * expander_w) + CELL_HMARGINS;
                  }
                  if (tree->depth_indent)
                  {
                     cell_geometry.x += (depth * tree->depth_indent);
                     cell_geometry.w -= (depth * tree->depth_indent);
                  }
               }

               /* Render the sub-objects of the cell */
               total_width = 0;
               model_geometry = cell_geometry;
               for (model_iter = 0; model_iter < col->num_models; model_iter++)
               {
                  if (col->models[model_iter]->render)
                  {
                     objects_created = col->models[model_iter]->render(col->models[model_iter], row, model_geometry,
                           row->cells_data[col_iter][model_iter], row_object->cells[col_iter].objects[model_iter], evas);

                     /* Objects have been created by the render() method, we add them to the tree */
                     if (objects_created)
                     {
                        for (i = 0; i < ETK_TREE_MAX_OBJECTS_PER_MODEL; i++)
                        {
                           if ((obj = row_object->cells[col_iter].objects[model_iter][i])
                                 && !evas_object_smart_parent_get(obj))
                           {
                              evas_object_clip_set(obj, col->clip);
                              etk_widget_member_object_add(tree->grid, obj);
                           }
                        }
                     }

                     if (col->models[model_iter]->width_get)
                     {
                        w = col->models[model_iter]->width_get(col->models[model_iter],
                              row->cells_data[col_iter][model_iter], row_object->cells[col_iter].objects[model_iter]);
                     }
                     else
                        w = 0;

                     if ((model_iter + 1) != col->num_models && w > 0)
                        w += MODEL_INTERSPACE;

                     model_geometry.x += w;
                     model_geometry.w -= w;
                     total_width += w;
                  }
               }

               /* Align the cell objects */
               if (col->align != 0.0)
               {
                  for (model_iter = 0; model_iter < col->num_models; model_iter++)
                  {
                     for (i = 0; i < ETK_TREE_MAX_OBJECTS_PER_MODEL; i++)
                     {
                        if ((obj = row_object->cells[col_iter].objects[model_iter][i]))
                        {
                           evas_object_geometry_get(obj, &x, &y, NULL, NULL);
                           evas_object_move(obj, x + (col->align * (cell_geometry.w - total_width)), y);
                        }
                     }
                  }
               }
            }
            /* If the column is not visible, we hide the corresponding row-objects */
            else
            {
               for (model_iter = 0; model_iter < col->num_models; model_iter++)
               {
                  for (i = 0; i < ETK_TREE_MAX_OBJECTS_PER_MODEL; i++)
                     evas_object_hide(row_object->cells[col_iter].objects[model_iter][i]);
               }
            }
         }

         if ((row_id % 2 == 0) && tree->alternating_row_colors)
            _etk_tree_row_signal_emit(row, row_object, "etk,state,odd");
         else
            _etk_tree_row_signal_emit(row, row_object, "etk,state,even");
         _etk_tree_row_signal_emit(row, row_object, row->selected ? "etk,state,selected" : "etk,state,unselected");
         edje_object_message_signal_process(row_object->background);

         evas_object_lower(row_object->background);
         row_object->row = row;
      }
      else
      {
         /* If there is no more row to render, we hide the row objects */
         for (col_iter = 0; col_iter < tree->num_cols; col_iter++)
         {
            for (model_iter = 0; model_iter < tree->columns[col_iter]->num_models; model_iter++)
            {
               for (i = 0; i < ETK_TREE_MAX_OBJECTS_PER_MODEL; i++)
               {
                  if (row_object->cells[col_iter].objects[model_iter][i])
                     evas_object_hide(row_object->cells[col_iter].objects[model_iter][i]);
               }
            }
         }
         evas_object_hide(row_object->expander);
         evas_object_hide(row_object->background);
         row_object->row = NULL;
      }

      row = _etk_tree_row_next_to_render_get(row, &depth);
      row_y += tree->rows_height;
      row_id++;
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/**************************
 * Tree
 **************************/

/* Called when the tree is realized */
static Etk_Bool _etk_tree_realized_cb(Etk_Object *object, void *data)
{
   Etk_Tree *tree;
   Evas *evas;
   int tree_contains_headers;
   int i;

   if (!(tree = ETK_TREE(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(tree))))
      return ETK_TRUE;

   /* Read the data from the theme */
   if (etk_widget_theme_data_get(ETK_WIDGET(tree), "tree_contains_headers", "%d", &tree_contains_headers) != 1)
      tree_contains_headers = 1;
   if (etk_widget_theme_data_get(ETK_WIDGET(tree), "separator_color", "%d %d %d %d", &tree->col_separator_color.r,
      &tree->col_separator_color.g, &tree->col_separator_color.b, &tree->col_separator_color.a) != 4)
   {
      tree->col_separator_color.r = 0;
      tree->col_separator_color.g = 0;
      tree->col_separator_color.b = 0;
      tree->col_separator_color.a = 0;
   }
   else
   {
      evas_color_argb_premul(tree->col_separator_color.a, &tree->col_separator_color.r,
            &tree->col_separator_color.g, &tree->col_separator_color.b);
   }
   /* if rows_height is not set */
   if (tree->rows_height == -1)
   {
      /* try to get the value from theme or fallback to default value */
      if (etk_widget_theme_data_get(ETK_WIDGET(tree), "row_height", "%d", &tree->rows_height) != 1)
         tree->rows_height = DEFAULT_ROW_HEIGHT;
      else
      {
         Etk_Range *vscrollbar;
         vscrollbar = etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(tree->scrolled_view));
         etk_range_increments_set(vscrollbar, tree->rows_height, 5 * tree->rows_height);
      }
   }

   /* Reparent the column headers if the "tree-contains-headers" setting has changed */
   if (tree->tree_contains_headers != tree_contains_headers)
   {
      for (i = 0; i < tree->num_cols; i++)
      {
         if (tree_contains_headers)
            etk_widget_parent_set(tree->columns[i]->header, ETK_WIDGET(tree));
         else
            etk_widget_parent_set(tree->columns[i]->header, tree->scroll_content);
      }
      tree->tree_contains_headers = (tree_contains_headers != 0);
   }

   return ETK_TRUE;
}

/* Called when the tree is focused */
static Etk_Bool _etk_tree_focused_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Tree *tree;

   if (!(tree = ETK_TREE(object)))
      return ETK_TRUE;

   etk_widget_theme_signal_emit(tree->scrolled_view, "etk,state,focused", ETK_FALSE);
   etk_widget_theme_signal_emit(tree->grid, "etk,state,focused", ETK_FALSE);
   return ETK_TRUE;
}

/* Called when the tree is unfocused */
static Etk_Bool _etk_tree_unfocused_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Tree *tree;

   if (!(tree = ETK_TREE(object)))
      return ETK_TRUE;

   etk_widget_theme_signal_emit(tree->scrolled_view, "etk,state,unfocused", ETK_FALSE);
   etk_widget_theme_signal_emit(tree->grid, "etk,state,unfocused", ETK_FALSE);
   return ETK_TRUE;
}

/* TODO: row_clicked event: sometimes it's a Etk_Event_Mouse_Up, sometimes a Etk_Event_Mouse_Down... */

/* Called when a key is pressed while the tree is focused */
static Etk_Bool _etk_tree_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Tree *tree;
   Etk_Tree_Row *selected_row, *row_to_select;

   if (!(tree = ETK_TREE(object)))
      return ETK_TRUE;

   /* CTRL + A: Select all the rows */
   if (strcmp(event->keyname, "a") == 0 && (event->modifiers & ETK_MODIFIER_CTRL))
   {
      etk_tree_select_all(tree);
   }
   /* Up arrow: Select the previous row */
   else if (strcmp(event->keyname, "Up") == 0)
   {
      if (!(selected_row = etk_tree_selected_row_get(tree)))
      {
         row_to_select = etk_tree_last_row_get(tree);
         while (!etk_tree_row_is_folded(row_to_select) && etk_tree_row_last_child_get(row_to_select))
            row_to_select = etk_tree_row_last_child_get(row_to_select);
      }
      else
         row_to_select = etk_tree_row_walk_prev(selected_row, ETK_FALSE);

      _etk_tree_row_select(tree, row_to_select, event->modifiers);
      etk_tree_row_scroll_to(row_to_select, ETK_FALSE);
   }
   /* Down arrow: Select the next row */
   else if (strcmp(event->keyname, "Down") == 0)
   {
      if (!(selected_row = etk_tree_selected_row_get(tree)))
         row_to_select = etk_tree_first_row_get(tree);
      else
         row_to_select = etk_tree_row_walk_next(selected_row, ETK_FALSE);

      _etk_tree_row_select(tree, row_to_select, event->modifiers);
      etk_tree_row_scroll_to(row_to_select, ETK_FALSE);
   }
   /* Left arrow: Fold the selected row */
   else if (strcmp(event->keyname, "Left") == 0)
   {
      if ((selected_row = etk_tree_selected_row_get(tree)))
         etk_tree_row_fold(selected_row);
   }
   /* Right arrow: Unfold the selected row */
   else if (strcmp(event->keyname, "Right") == 0)
   {
      if ((selected_row = etk_tree_selected_row_get(tree)))
         etk_tree_row_unfold(selected_row);
   }
   else
   {
      return ETK_TRUE;
   }

   return ETK_FALSE;
}

/**************************
 * Tree Grid
 **************************/

/* Called when the tree grid is realized */
static Etk_Bool _etk_tree_grid_realized_cb(Etk_Object *object, void *data)
{
   Etk_Tree *tree;
   Evas *evas;
   int i;

   if (!(tree = TREE_GET(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(object))))
      return ETK_TRUE;

   for (i = 0; i < tree->num_cols; i++)
      _etk_tree_col_realize(tree, i);

   tree->grid_clip = evas_object_rectangle_add(evas);
   etk_widget_member_object_add(tree->grid, tree->grid_clip);
   return ETK_TRUE;
}

/* Called when the tree grid is unrealized */
static Etk_Bool _etk_tree_grid_unrealized_cb(Etk_Object *object, void *data)
{
   Etk_Tree *tree;
   int i;

   if (!(tree = TREE_GET(object)))
      return ETK_TRUE;

   tree->grid_clip = NULL;

   for (i = 0; i < tree->num_cols; i++)
   {
      tree->columns[i]->header_over = NULL;
      tree->columns[i]->clip = NULL;
      tree->columns[i]->separator = NULL;
   }

   while (tree->row_objects)
   {
      _etk_tree_row_object_destroy(tree, tree->row_objects->data);
      tree->row_objects = eina_list_remove_list(tree->row_objects, tree->row_objects);
   }

   return ETK_TRUE;
}

/**************************
 * Tree Headers
 **************************/

/* Called when the mouse presses a column's header */
static void _etk_tree_header_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree *tree;
   Etk_Tree_Col *col;
   Evas_Event_Mouse_Down *event = event_info;

   if (!(col = ETK_TREE_COL(data)) || !(tree = col->tree) || event->button != 1)
      return;

   /* Checks if we should resize a column */
   if ((tree->col_to_resize = etk_tree_col_to_resize_get(col, event->canvas.x)))
   {
      tree->col_resize_orig_width = tree->col_to_resize->width;
      tree->col_resize_orig_mouse_x = event->canvas.x;
   }
}

/* Called when the mouse releases a column's header */
static void _etk_tree_header_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree *tree;
   Etk_Tree_Col *col;
   Evas_Event_Mouse_Up *event = event_info;
   int hx, hy, hw, hh;
   Etk_Bool ascending;

   if (!(col = ETK_TREE_COL(data)) || !(tree = col->tree) || event->button != 1)
      return;

   if (event->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
      return;
   
   if (tree->col_to_resize)
      tree->col_to_resize = NULL;
   else
   {
      /* If the column header has been clicked, we sort the column */
      evas_object_geometry_get(obj, &hx, &hy, &hw, &hh);
      if (ETK_INSIDE(event->canvas.x, event->canvas.y, hx, hy, hw, hh))
      {
         if (tree->sorted_col == col)
            ascending = !tree->sorted_asc;
         else
            ascending = ETK_TRUE;
         etk_tree_col_sort(col, ascending);
      }
   }
}

/* Called when the mouse moves over a column's header */
static void _etk_tree_header_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree *tree;
   Etk_Tree_Col *col;
   Etk_Toplevel *toplevel;
   Evas_Event_Mouse_Move *event = event_info;

   if (!(col = ETK_TREE_COL(data)) || !(tree = col->tree)
         || !(toplevel = etk_widget_toplevel_parent_get(ETK_WIDGET(tree))))
      return;

   if (tree->col_to_resize)
   {
      int delta;

      /* Resize the column to resize */
      delta = event->cur.canvas.x - tree->col_resize_orig_mouse_x;
      etk_tree_col_width_set(tree->col_to_resize, tree->col_resize_orig_width + delta);
   }
   else
   {
      Etk_Tree_Col *col_to_resize;

      /* Set/Unset the resize mouse-pointer if the pointer is/was between two column headers */
      col_to_resize = etk_tree_col_to_resize_get(col, event->cur.canvas.x);
      if (col_to_resize && !tree->col_resize_pointer_set)
      {
         etk_toplevel_pointer_push(toplevel, ETK_POINTER_H_DOUBLE_ARROW);
         tree->col_resize_pointer_set = ETK_TRUE;
      }
      else if (!col_to_resize && tree->col_resize_pointer_set)
      {
         etk_toplevel_pointer_pop(toplevel, ETK_POINTER_H_DOUBLE_ARROW);
         tree->col_resize_pointer_set = ETK_FALSE;
      }
   }
}

/* Called when the mouse enters a column's header */
static void _etk_tree_header_mouse_in_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree *tree;
   Etk_Tree_Col *col, *col_to_resize;
   Etk_Toplevel *toplevel;
   Evas_Event_Mouse_In *event = event_info;

   if (!(col = ETK_TREE_COL(data)) || !(tree = col->tree)
         || !(toplevel = etk_widget_toplevel_parent_get(ETK_WIDGET(tree))))
      return;

   /* Set the resize mouse pointer if the pointer is between two column headers */
   col_to_resize = etk_tree_col_to_resize_get(col, event->canvas.x);
   if (col_to_resize && !tree->col_resize_pointer_set)
   {
      etk_toplevel_pointer_push(toplevel, ETK_POINTER_H_DOUBLE_ARROW);
      tree->col_resize_pointer_set = ETK_TRUE;
   }
}

/* Called when the mouse leaves a column's header */
static void _etk_tree_header_mouse_out_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree *tree;
   Etk_Tree_Col *col;
   Etk_Toplevel *toplevel;

   if (!(col = ETK_TREE_COL(data)) || !(tree = col->tree)
         || !(toplevel = etk_widget_toplevel_parent_get(ETK_WIDGET(tree))))
      return;

   /* Unset the resize mouse pointer if it was set */
   if (tree->col_resize_pointer_set)
   {
      etk_toplevel_pointer_pop(toplevel, ETK_POINTER_H_DOUBLE_ARROW);
      tree->col_resize_pointer_set = ETK_FALSE;
   }
}

/* Called when the background of a row is pressed by the mouse */
static void _etk_tree_row_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree_Row_Object *row_object;
   Etk_Event_Mouse_Down event;

   if (!(row_object = data) || !row_object->row)
      return;

   etk_event_mouse_down_wrap(ETK_WIDGET(row_object->row->tree), event_info, &event);

   /* We do not select a row on the "key-down" event if the row is already selected
    * to allow the user to drag and drop several rows. The selection will be done on "key-up" */
   if (!row_object->row->selected || event.modifiers != ETK_MODIFIER_NONE)
      _etk_tree_row_select(row_object->row->tree, row_object->row, event.modifiers);

   _etk_tree_row_signal_emit(row_object->row, row_object, "etk,state,clicked");
   
   if (event.flags != ETK_MOUSE_NONE)
   {
      etk_signal_emit(ETK_TREE_ROW_CLICKED_SIGNAL, ETK_OBJECT(row_object->row->tree),
            row_object->row, &event);
   }
}

/* Called when the background of a row is released by the mouse */
static void _etk_tree_row_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree_Row_Object *row_object;
   Etk_Event_Mouse_Up event;
   int x, y, w, h;

   if (!(row_object = data) || !row_object->row)
      return;

   evas_object_geometry_get(obj, &x, &y, &w, &h);
   etk_event_mouse_up_wrap(ETK_WIDGET(row_object->row->tree), event_info, &event);

   if (event.modifiers & ETK_MODIFIER_ON_HOLD)
      return;
   
   /* We make sure the mouse button has been released over the row */
   if (ETK_INSIDE(event.canvas.x, event.canvas.y, x, y, w, h))
   {
      if (row_object->row->selected && event.modifiers == ETK_MODIFIER_NONE)
         _etk_tree_row_select(row_object->row->tree, row_object->row, ETK_MODIFIER_NONE);

      if (event.flags == ETK_MOUSE_NONE)
      {
         etk_signal_emit(ETK_TREE_ROW_CLICKED_SIGNAL, ETK_OBJECT(row_object->row->tree),
               row_object->row, &event);
      }
      if (event.flags == ETK_MOUSE_DOUBLE_CLICK)
      {
         etk_signal_emit(ETK_TREE_ROW_ACTIVATED_SIGNAL, ETK_OBJECT(row_object->row->tree),
               row_object->row, &event);
      }
   }
}

/* Called when the expander of a row is released by the mouse */
static void _etk_tree_row_expander_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree_Row_Object *row_object;
   Evas_Event_Mouse_Up *event = event_info;
   Evas_Coord x, y, w, h;

   if (!(row_object = data) || !row_object->row)
      return;

   evas_object_geometry_get(obj, &x, &y, &w, &h);
   if (ETK_INSIDE(event->canvas.x, event->canvas.y, x, y, w, h))
   {
      if (row_object->row->unfolded)
         etk_tree_row_fold(row_object->row);
      else
         etk_tree_row_unfold(row_object->row);
   }
}

/**************************
 *
 * Private functions
 *
 **************************/

/* A job called when rows need to be deleted */
static void _etk_tree_purge_job(void *data)
{
   Etk_Tree *tree;

   if (!(tree = ETK_TREE(data)))
      return;

   _etk_tree_purge(tree);
   tree->purge_job = NULL;
}

/* Deletes effectively all the rows marked as deleted */
static void _etk_tree_purge(Etk_Tree *tree)
{
   Eina_List *l;
   Etk_Tree_Row *row, *r, *next;
   Etk_Tree_Col *col;
   int i, j;

   if (!tree)
      return;

   /* First we dissociate the row objects from the rows that will be deleted,
    * and we cache their row objects */
   for (l = tree->row_objects; l; l = l->next)
   {
      Etk_Tree_Row_Object *row_object;

      row_object = l->data;
      if (row_object->row && row_object->row->delete_me)
      {
         for (i = 0; i < tree->num_cols; i++)
         {
            col = tree->columns[i];
            for (j = 0; j < col->num_models; j++)
            {
               if (col->models[j]->objects_cache)
               {
                  col->models[j]->objects_cache(col->models[j], row_object->row->cells_data[i][j],
                        row_object->cells[i].objects[j]);
               }
            }
         }

         etk_signal_emit(ETK_TREE_ROW_HIDDEN_SIGNAL, ETK_OBJECT(tree), row_object->row);
         row_object->row = NULL;
      }
   }

   /* Then we delete all the rows of the purge pool */
   while (tree->purge_pool)
   {
      row = tree->purge_pool->data;
      row->next = NULL;
      for (r = row; r; r = next)
      {
         /* Get the next row to free after this one */
         if (r->first_child)
         {
            r->last_child->next = r->next;
            r->next = r->first_child;
         }
         next = r->next;

         /* Free the row */
         if (r->cells_data)
         {
            for (i = 0; i < tree->num_cols; i++)
            {
               col = tree->columns[i];
               for (j = 0; j < col->num_models; j++)
               {
                  if (col->models[j]->cell_data_free)
                     col->models[j]->cell_data_free(col->models[j], r->cells_data[i][j]);
                  free(r->cells_data[i][j]);
               }
               free(r->cells_data[i]);
            }
            free(r->cells_data);
         }

         if (r->data && r->data_free_cb)
            r->data_free_cb(r->data);

         free(r);
      }
      tree->purge_pool = eina_list_remove_list(tree->purge_pool, tree->purge_pool);
   }
}

/* Moves a row of the tree to the list of rows to delete during the next iteration */
static void _etk_tree_row_move_to_purge_pool(Etk_Tree_Row *row)
{
   Etk_Tree *tree;
   Etk_Tree_Row *r;

   if (!row || row->delete_me)
      return;

   tree = row->tree;

   /* Mark the row and all its children as deleted */
   row->delete_me = ETK_TRUE;
   tree->total_rows--;
   for (r = row->first_child; r; )
   {
      r->delete_me = ETK_TRUE;
      tree->total_rows--;

      if (r->first_child)
         r = r->first_child;
      else
      {
         while (r && !r->next)
         {
            if (r->parent == row)
               r = NULL;
            else
               r = r->parent;
         }
         r = r ? r->next : NULL;
      }
   }

   /* Move the row to the purge_pool */
   if (row->prev)
      row->prev->next = row->next;
   if (row->next)
      row->next->prev = row->prev;
   if (row->parent)
   {
      if (row->parent->first_child == row)
         row->parent->first_child = row->next;
      if (row->parent->last_child == row)
         row->parent->last_child = row->prev;
      row->parent->num_children--;
   }
   for (r = row->parent; r && r->unfolded; r = r->parent)
      r->num_visible_children -= row->num_visible_children + 1;

   if (tree->last_selected_row == row)
      tree->last_selected_row = NULL;
   tree->purge_pool = eina_list_append(tree->purge_pool, row);

   if (!tree->purge_job)
      tree->purge_job = ecore_job_add(_etk_tree_purge_job, tree);
}

/* Creates the Evas objects of the corresponding column */
static void _etk_tree_col_realize(Etk_Tree *tree, int col_nth)
{
   Evas *evas;
   Etk_Tree_Col *col;

   if (!tree || col_nth < 0 || col_nth >= tree->num_cols)
      return;
   if (!(evas = etk_widget_toplevel_evas_get(tree->grid)))
      return;
   col = tree->columns[col_nth];

   col->header_over = evas_object_rectangle_add(evas);
   evas_object_color_set(col->header_over, 0, 0, 0, 0);
   evas_object_repeat_events_set(col->header_over, 1);
   /* TODO: is the tree a good parent for the "header_over" ? */
   etk_widget_member_object_add(ETK_WIDGET(tree), col->header_over);

   col->clip = evas_object_rectangle_add(evas);
   etk_widget_member_object_add(tree->grid, col->clip);

   col->separator = evas_object_rectangle_add(evas);
   evas_object_color_set(col->separator, tree->col_separator_color.r,
         tree->col_separator_color.g, tree->col_separator_color.b, tree->col_separator_color.a);
   evas_object_pass_events_set(col->separator, 1);
   evas_object_clip_set(col->separator, col->clip);
   etk_widget_member_object_add(tree->grid, col->separator);


   evas_object_event_callback_add(col->header_over, EVAS_CALLBACK_MOUSE_DOWN,
         _etk_tree_header_mouse_down_cb, col);
   evas_object_event_callback_add(col->header_over, EVAS_CALLBACK_MOUSE_UP,
         _etk_tree_header_mouse_up_cb, col);
   evas_object_event_callback_add(col->header_over, EVAS_CALLBACK_MOUSE_MOVE,
         _etk_tree_header_mouse_move_cb, col);
   evas_object_event_callback_add(col->header_over, EVAS_CALLBACK_MOUSE_IN,
         _etk_tree_header_mouse_in_cb, col);
   evas_object_event_callback_add(col->header_over, EVAS_CALLBACK_MOUSE_OUT,
         _etk_tree_header_mouse_out_cb, col);
}

/* Returns the column to resize according to the position of the mouse pointer */
static Etk_Tree_Col *etk_tree_col_to_resize_get(Etk_Tree_Col *col, int x)
{
   Etk_Tree_Col *prev_visible_col, *col2;
   int header_x, header_w;
   int i;

   if (!col || !col->tree->headers_visible)
      return NULL;

   /* We look if the mouse pointer is over one of the two edges of the column header */
   etk_widget_geometry_get(col->header, &header_x, NULL, &header_w, NULL);
   x -= header_x;
   if (x <= COL_RESIZE_THRESHOLD)
   {
      prev_visible_col = NULL;
      for (i = 0; i < col->tree->num_cols; i++)
      {
         col2 = col->tree->columns[i];
         if (!col2->visible)
            continue;

         if ((col2->position < col->position) && (!prev_visible_col || (prev_visible_col->position < col2->position)))
            prev_visible_col = col2;
      }

      return (prev_visible_col && prev_visible_col->resizable) ? prev_visible_col : NULL;
   }
   else if (header_w - x <= COL_RESIZE_THRESHOLD)
      return (col && col->resizable) ? col : NULL;

   return NULL;
}

/* Gets the next row to render */
static Etk_Tree_Row *_etk_tree_row_next_to_render_get(Etk_Tree_Row *row, int *depth)
{
   if (!row)
      return NULL;

   if (row->first_child && row->unfolded)
   {
      if (depth)
         (*depth)++;
      return row->first_child;
   }
   else
   {
      while (row && !row->next)
      {
         if (depth)
            (*depth)--;
         row = row->parent;
      }
      return row ? row->next : NULL;
   }
}

/* Creates a new row-object and its subobjects */
static Etk_Tree_Row_Object *_etk_tree_row_object_create(Etk_Tree *tree)
{
   Etk_Tree_Row_Object *row_object;
   Etk_Tree_Col *col, *first_visible_col;
   Evas *evas;
   int i, j, k;

   if (!tree || !tree->built || !(evas = etk_widget_toplevel_evas_get(tree->grid)))
      return NULL;

   row_object = malloc(sizeof(Etk_Tree_Row_Object));
   row_object->row = NULL;

   /* Create the background object of the row */
   row_object->background = edje_object_add(evas);
   etk_theme_edje_object_set_from_parent(row_object->background, "row", ETK_WIDGET(tree));
   evas_object_repeat_events_set(row_object->background, 1);
   evas_object_clip_set(row_object->background, tree->grid_clip);
   etk_widget_member_object_add(tree->grid, row_object->background);
   evas_object_event_callback_add(row_object->background, EVAS_CALLBACK_MOUSE_DOWN,
         _etk_tree_row_mouse_down_cb, row_object);
   evas_object_event_callback_add(row_object->background, EVAS_CALLBACK_MOUSE_UP,
         _etk_tree_row_mouse_up_cb, row_object);

   /* Create the expander object of the row */
   if (tree->mode == ETK_TREE_MODE_TREE)
   {
      row_object->expander = edje_object_add(evas);
      etk_theme_edje_object_set_from_parent(row_object->expander, "expander", ETK_WIDGET(tree));

      /* Clip it to the first visible row */
      first_visible_col = NULL;
      for (i = 0; i < tree->num_cols; i++)
      {
         col = tree->columns[i];
         if (col->visible)
         {
            if (!first_visible_col || first_visible_col->position > col->position)
               first_visible_col = col;
         }
      }
      if (first_visible_col)
         evas_object_clip_set(row_object->expander, first_visible_col->clip);

      etk_widget_member_object_add(tree->grid, row_object->expander);
      evas_object_event_callback_add(row_object->expander, EVAS_CALLBACK_MOUSE_UP,
            _etk_tree_row_expander_mouse_up_cb, row_object);
   }
   else
      row_object->expander = NULL;

   /* Create the sub-objects of the row's cells */
   row_object->cells = calloc(tree->num_cols, sizeof(Etk_Tree_Cell_Objects));
   for (i = 0; i < tree->num_cols; i++)
   {
      col = tree->columns[i];
      for (j = 0; j < col->num_models; j++)
      {
         if (col->models[j]->objects_create)
         {
            col->models[j]->objects_create(col->models[j], row_object->cells[i].objects[j], evas);
            for (k = 0; k < ETK_TREE_MAX_OBJECTS_PER_MODEL; k++)
            {
               if (row_object->cells[i].objects[j][k]
                     && !evas_object_smart_parent_get(row_object->cells[i].objects[j][k]))
               {
                  evas_object_clip_set(row_object->cells[i].objects[j][k], col->clip);
                  etk_widget_member_object_add(tree->grid, row_object->cells[i].objects[j][k]);
               }
            }
         }
      }
      evas_object_raise(col->separator);
   }

   return row_object;
}

/* Destroys the row object and its subobjects */
static void _etk_tree_row_object_destroy(Etk_Tree *tree, Etk_Tree_Row_Object *row_object)
{
   int i, j, k;

   if (!tree || !row_object)
      return;

   for (i = 0; i < tree->num_cols; i++)
   {
      for (j = 0; j < tree->columns[i]->num_models; j++)
      {
         for (k = 0; k < ETK_TREE_MAX_OBJECTS_PER_MODEL; k++)
            evas_object_del(row_object->cells[i].objects[j][k]);
      }
   }
   free(row_object->cells);

   evas_object_del(row_object->expander);
   evas_object_del(row_object->background);
   free(row_object);
}

/* Emits a theme-signal to all the row objects (background, expander, model-objects) */
static void _etk_tree_row_signal_emit(Etk_Tree_Row *row, Etk_Tree_Row_Object *row_object, const char *signal)
{
   int i, j, k;

   if (!row || !row_object || !signal)
      return;

   if (row_object->background)
      edje_object_signal_emit(row_object->background, signal, "etk");
   if (row_object->expander)
      edje_object_signal_emit(row_object->expander, signal, "etk");

   for (i = 0; i < row->tree->num_cols; i++)
   {
      for (j = 0; j < row->tree->columns[i]->num_models; j++)
      {
         for (k = 0; k < ETK_TREE_MAX_OBJECTS_PER_MODEL; k++)
            edje_object_signal_emit(row_object->cells[i].objects[j][k], signal, "etk");
      }
   }

}

/* Clips all the expanders against the clip object of the first visible column */
static void _etk_tree_expanders_clip(Etk_Tree *tree)
{
   Etk_Tree_Row_Object *row_object;
   Etk_Tree_Col *col, *first_visible_col;
   Eina_List *l;
   int i;

   if (!tree || tree->mode != ETK_TREE_MODE_TREE || !tree->built)
      return;

   /* Clip it to the first visible row */
   first_visible_col = NULL;
   for (i = 0; i < tree->num_cols; i++)
   {
      col = tree->columns[i];
      if (col->visible)
      {
         if (!first_visible_col || first_visible_col->position > col->position)
            first_visible_col = col;
      }
   }

   if (!first_visible_col)
      return;

   for (l = tree->row_objects; l; l = l->next)
   {
      row_object = l->data;
      if (row_object->expander)
         evas_object_clip_set(row_object->expander, first_visible_col->clip);
   }
}

/* Selects/Unselects the corresponding rows according to the modifiers */
static void _etk_tree_row_select(Etk_Tree *tree, Etk_Tree_Row *row, Etk_Modifiers modifiers)
{
   if (!tree || !row || row->delete_me)
      return;

   if (!tree->multiple_select)
   {
      if (row->selected)
      {
         /* Unselect the row */
         if (modifiers & ETK_MODIFIER_CTRL)
            etk_tree_row_unselect(row);
      }
      else
      {
         /* Unselect the selected row and select only the given row */
         if (tree->last_selected_row && tree->last_selected_row->selected)
            etk_tree_row_unselect(tree->last_selected_row);
         else
            etk_tree_unselect_all(tree);

         row->selected = ETK_TRUE;
         row->tree->last_selected_row = row;
         etk_signal_emit(ETK_TREE_ROW_SELECTED_SIGNAL, ETK_OBJECT(row->tree), row);
      }
   }
   else
   {
      if ((modifiers & ETK_MODIFIER_SHIFT) && tree->last_selected_row)
      {
         Etk_Tree_Row *r;
         int cnt = 0;

         etk_tree_unselect_all(tree);

         /* Walk through all the rows of the tree and select the rows
          * between the last selected row and the given row */
         for (r = tree->root.first_child; r; r = etk_tree_row_walk_next(r, ETK_TRUE))
         {
            if (r == tree->last_selected_row)
               cnt++;
            if (r == row)
               cnt++;

            if (cnt >= 1)
               r->selected = ETK_TRUE;
            if (cnt >= 2)
               break;
         }

         etk_signal_emit(ETK_TREE_ROW_SELECTED_SIGNAL, ETK_OBJECT(row->tree), row);
      }
      /* Invert the selected state of the given row */
      else if (modifiers & ETK_MODIFIER_CTRL)
      {
         if (row->selected)
            etk_tree_row_unselect(row);
         else
         {
            row->selected = ETK_TRUE;
            row->tree->last_selected_row = row;
            etk_signal_emit(ETK_TREE_ROW_SELECTED_SIGNAL, ETK_OBJECT(row->tree), row);
         }
      }
      else
      {
         /* Unselect all the rows and select only the given row */
         etk_tree_unselect_all(tree);
         row->selected = ETK_TRUE;
         if (row->tree->last_selected_row != row)
         {
            row->tree->last_selected_row = row;
            etk_signal_emit(ETK_TREE_ROW_SELECTED_SIGNAL, ETK_OBJECT(row->tree), row);
         }
      }
   }

   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
}

/* Restore the heap-property of the heap. Used to sort the tree */
static void _etk_tree_heapify(Etk_Tree *tree, Etk_Tree_Row **heap, int root, int size, int (*compare_cb)(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data), int asc, Etk_Tree_Col *col, void *data)
{
   Etk_Tree_Row *tmp;
   int left, right, max;

   if (!heap)
      return;

   left = (root * 2) + 1;
   right = (root * 2) + 2;

   max = root;
   if (left < size && (compare_cb(col, heap[left], heap[max], data) * asc) > 0)
      max = left;
   if (right < size && (compare_cb(col, heap[right], heap[max], data) * asc) > 0)
      max = right;

   if (max != root)
   {
      tmp = heap[max];
      heap[max] = heap[root];
      heap[root] = tmp;
      _etk_tree_heapify(tree, heap, max, size, compare_cb, asc, col, data);
   }
}

/* Do a quick sort on a list of rows */
static void _etk_tree_qsort(Etk_Tree_Row *first, Etk_Tree_Row *last, Etk_Tree_Col *col,
                            int (*compare_cb)(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data),
                            void *data, Etk_Bool ascending)
{
   Etk_Tree_Row *pivot;

   if (!first || !last || first == last || last->next == first) return;

   pivot = last;

   _etk_tree_partition(&first, &last, pivot, col, compare_cb, data, ascending);
   _etk_tree_qsort(first, pivot->prev, col, compare_cb, data, ascending);
   _etk_tree_qsort(pivot->next, last, col, compare_cb, data, ascending);
}

/* Quick sort partition for rows lists */
static void _etk_tree_partition(Etk_Tree_Row **first, Etk_Tree_Row **last, Etk_Tree_Row *pivot, Etk_Tree_Col *col,
                                int (*compare_cb)(Etk_Tree_Col *col, Etk_Tree_Row *row1, Etk_Tree_Row *row2, void *data),
                                void *data, Etk_Bool ascending)
{
   Etk_Tree_Row *row;

   row = *first;

   while (row != pivot)
     {
       Etk_Tree_Row *next;
       next = row->next;
       if (compare_cb(col, row, pivot, data) * (ascending ? 1 : -1) > 0)
         {
           if (row == *first) *first = row->next;
           if (pivot == *last) *last = row;
           _etk_tree_row_move_relatively(pivot, row, ETK_TRUE);
         }
       row = next;
     }
}

/* Move row before or after center */
static void _etk_tree_row_move_relatively(Etk_Tree_Row *center, Etk_Tree_Row *row, Etk_Bool after)
{
   if (!center || !row || row == center) return;

   if (row->prev) row->prev->next = row->next;
   else row->parent->first_child = row->next;
   if (row->next) row->next->prev = row->prev;
   else row->parent->last_child = row->prev;

   if (after)
     {
       if (center->next) center->next->prev = row;
       else center->parent->last_child = row;
       row->next = center->next;
       center->next = row;
       row->prev = center;
     }
   else
     {
       if (center->prev) center->prev->next = row;
       else center->parent->first_child = row;
       row->prev = center->prev;
       center->prev = row;
       row->next = center;
     }
}

/* Reverse the whole tree */
static void _etk_tree_reverse(Etk_Tree *tree)
{
   Etk_Tree_Row *row, *to_reverse;

   if (!tree)
     return;

   to_reverse = row = tree->root.first_child;
   while (row || to_reverse)
     {
       Etk_Tree_Row *next;
       if (to_reverse)
         {
           _etk_tree_reverse_rows(to_reverse);
           to_reverse = NULL;
           row = row->parent->first_child;
         }
       next = etk_tree_row_walk_next(row, ETK_TRUE);
       if (next && next->parent == row) to_reverse = next;
       row = next;
     }

   tree->sorted_asc = !(tree->sorted_asc);
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/* Reverse a list of rows */
static void _etk_tree_reverse_rows(Etk_Tree_Row *first)
{
   Etk_Tree_Row *row = first;

   if (!first || first->parent->num_children < 2) return;

   row->parent->first_child = row->parent->last_child;
   row->parent->last_child = row;

   while (row)
     {
        Etk_Tree_Row *next;
        next = row->next;
        row->next = row->prev;
        row->prev = next;
        row = next;
     }
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Tree
 *
 * @image html widgets/tree.png
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Tree
 *
 * <hr>
 *
 * \par Signals of Etk_Tree:
 * @signal_name "row-selected": Emitted when a row is selected
 * @signal_cb Etk_Bool callback(Etk_Tree *tree, Etk_Tree_Row *row, void *data)
 * @signal_arg tree: the tree containing the row that has been selected
 * @signal_arg row: the row that has been selected
 * @signal_data
 * \par
 * @signal_name "row-unselected": Emitted when a row is unselected
 * @signal_cb Etk_Bool callback(Etk_Tree *tree, Etk_Tree_Row *row, void *data)
 * @signal_arg tree: the tree containing the row that has been unselected
 * @signal_arg row: the row that has been unselected
 * @signal_data
 * \par
 * @signal_name "all-selected": Emitted when all the rows of a tree are selected with etk_tree_select_all()
 * @signal_cb Etk_Bool callback(Etk_Tree *tree, void *data)
 * @signal_arg tree: the tree whose all rows have been selected
 * @signal_data
 * \par
 * @signal_name "all-unselected": Emitted when all the rows of a tree are unselected with etk_tree_unselect_all()
 * @signal_cb Etk_Bool callback(Etk_Tree *tree, void *data)
 * @signal_arg tree: the tree whose all rows have been unselected
 * @signal_data
 * \par
 * @signal_name "row-clicked": Emitted when a row of the tree has been clicked
 * @signal_cb Etk_Bool callback(Etk_Tree *tree, Etk_Tree_Row *row, Etk_Event_Mouse_Up *event, void *data)
 * @signal_arg tree: the tree containing the row that has been clicked
 * @signal_arg row: the row that has been clicked
 * @signal_arg event: the event-structure corresponding to the click event
 * @signal_data
 * \par
 * @signal_name "row-activated": Emitted when a row is double-clicked or when the user presses "space" on a selected row
 * @signal_cb Etk_Bool callback(Etk_Tree *tree, Etk_Tree_Row *row, void *data)
 * @signal_arg tree: the tree containing the row that has been activated
 * @signal_arg row: the row that has been activated
 * @signal_data
 * \par
 * @signal_name "row-unfolded": Emitted when a row is unfolded
 * @signal_cb Etk_Bool callback(Etk_Tree *tree, Etk_Tree_Row *row, void *data)
 * @signal_arg tree: the tree containing the row that has been unfolded
 * @signal_arg row: the row that has been unfolded
 * @signal_data
 * \par
 * @signal_name "row-folded": Emitted when a row is folded
 * @signal_cb Etk_Bool callback(Etk_Tree *tree, Etk_Tree_Row *row, void *data)
 * @signal_arg tree: the tree containing the row that has been folded
 * @signal_arg row: the row that has been folded
 * @signal_data
 * \par
 * @signal_name "row-shown": Emitted when a row becomes visible, i.e when the row enters in the tree's viewport
 * @signal_cb Etk_Bool callback(Etk_Tree *tree, Etk_Tree_Row *row, void *data)
 * @signal_arg tree: the tree containing the row that has hecome visible
 * @signal_arg row: the row that has hecome visible
 * @signal_data
 * \par
 * @signal_name "row-hidden": Emitted when a row becomes invisible, i.e when the row leaves in the tree's viewport
 * @signal_cb Etk_Bool callback(Etk_Tree *tree, Etk_Tree_Row *row, void *data)
 * @signal_arg tree: the tree containing the row that has hecome invisible
 * @signal_arg row: the row that has hecome invisible
 * @signal_data
 *
 * \par Properties of Etk_Tree:
 * @prop_name "mode": The current mode of the tree ("tree" or "list" mode)
 * @prop_type Integer (Etk_Tree_Mode)
 * @prop_rw
 * @prop_val ETK_TREE_MODE_LIST
 * \par
 * @prop_name "multiple-select": Whether or not several rows of the tree can be selected
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * \par
 * @prop_name "headers-visible": Whether or not the headers of the columns are visible
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * \par
 * @prop_name "expanders-visible": Whether or not the row expanders are visible
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * \par
 * @prop_name "depth-indent": The additional indent of child rows
 * @prop_type Integer
 * @prop_rw
 * @prop_val 0
 * \par
 * @prop_name "column-separators-visible": Whether or not the columns are separated by a visible vertical separator
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * \par
 * @prop_name "alternating-row-colors": Whether or not the row-colors alternate (e.g darker even rows)
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * \par
 * @prop_name "rows-height": The height of the rows of the tree
 * @prop_type Integer
 * @prop_rw
 * @prop_val 24
 *
 * <hr>
 *
 * \par Signals of Etk_Tree_Col:
 * @signal_name "cell-value-changed": Emitted when the value of a cell of the column is changed
 * @signal_cb Etk_Bool callback(Etk_Tree_Col *col, Etk_Tree_Row *row, void *data)
 * @signal_arg col: the column containing the cell whose value has been changed
 * @signal_arg row: the row containing the cell whose value has been changed
 * @signal_data
 *
 * \par Properties of Etk_Tree_Col:
 * @prop_name "title": The title of the column
 * @prop_type String (char *)
 * @prop_rw
 * @prop_val NULL
 * \par
 * @prop_name "visible": Whether or not the column is visible
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * \par
 * @prop_name "resizable": Whether or not the column can be resized by the user
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_TRUE
 * \par
 * @prop_name "position": The position of the column inside the tree (between 0 and (num_cols - 1))
 * @prop_type Integer
 * @prop_rw
 * @prop_val 0
 * \par
 * @prop_name "expand": Whether the column should expand to take up all available width
 * @prop_type Boolean
 * @prop_rw
 * @prop_val ETK_FALSE
 * \par
 * @prop_name "width": The current width of the column
 * @prop_type Boolean
 * @prop_rw
 * @prop_val 24
 * \par
 * @prop_name "min-width": The minimum width of the column
 * @prop_type Boolean
 * @prop_rw
 * @prop_val 24
 * \par
 * @prop_name "align": The horizontal alignment of the objects inside the cells of the column, from 0.0 (left) to 1.0 (right)
 * @prop_type Float
 * @prop_rw
 * @prop_val 0.0
 *
 * <hr>
 */
