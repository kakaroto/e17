/** @file etk_tree.c */
#include "etk_tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Ecore_Data.h>
#include <Edje.h>
#include "etk_tree_model.h"
#include "etk_scrolled_view.h"
#include "etk_button.h"
#include "etk_toplevel_widget.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"
#include "etk_window.h"
#include "etk_drag.h"
#include "config.h"

/**
 * @addtogroup Etk_Tree
 * @{
 */

#define ETK_TREE_MIN_HEADER_WIDTH 10
#define ETK_TREE_MIN_ROW_HEIGHT 8
#define ETK_TREE_NUM_OBJECTS_PER_CELL 3

#define ETK_TREE_GRID_TYPE       (_etk_tree_grid_type_get())
#define ETK_TREE_GRID(obj)       (ETK_OBJECT_CAST((obj), ETK_TREE_GRID_TYPE, Etk_Tree_Grid))
#define ETK_IS_TREE_GRID(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TREE_GRID_TYPE))

typedef struct _Etk_Tree_Grid
{
   /* Inherit form Etk_Container */
   Etk_Container container;

   Etk_Tree *tree;
   Evas_Object *clip;
} Etk_Tree_Grid;

typedef struct _Etk_Tree_Cell_Objects
{
   Evas_Object *objects[ETK_TREE_NUM_OBJECTS_PER_CELL];
} Etk_Tree_Cell_Objects;

typedef struct _Etk_Tree_Row_Objects
{
   Evas_Object *expander;
   Evas_Object *background;
   Etk_Tree_Cell_Objects *cells_objects;
   Etk_Tree_Row *row;
} Etk_Tree_Row_Objects;

enum _Etk_Tree_Signal_Id
{
   ETK_TREE_ROW_SELECTED_SIGNAL,
   ETK_TREE_ROW_UNSELECTED_SIGNAL,
   ETK_TREE_ROW_CLICKED_SIGNAL,
   ETK_TREE_ROW_ACTIVATED_SIGNAL,
   ETK_TREE_ROW_EXPANDED_SIGNAL,
   ETK_TREE_ROW_COLLAPSED_SIGNAL,
   ETK_TREE_SELECT_ALL_SIGNAL,
   ETK_TREE_UNSELECT_ALL_SIGNAL,
   ETK_TREE_NUM_SIGNALS
};

enum _Etk_Tree_Property_Id
{
   ETK_TREE_MODE_PROPERTY,
   ETK_TREE_MULTIPLE_SELECT_PROPERTY,
   ETK_TREE_HEADERS_VISIBLE_PROPERTY,
   ETK_TREE_ROW_HEIGHT_PROPERTY
};

enum _Etk_Tree_Col_Signal_Id
{
   ETK_TREE_COL_CELL_VALUE_CHANGED,
   ETK_TREE_COL_NUM_SIGNALS
};

enum _Etk_Tree_Col_Property_Id
{
   ETK_TREE_COL_TITLE_PROPERTY,
   ETK_TREE_COL_WIDTH_PROPERTY,
   ETK_TREE_COL_MIN_WIDTH_PROPERTY,
   ETK_TREE_COL_VISIBLE_WIDTH_PROPERTY,
   ETK_TREE_COL_VISIBLE_PROPERTY,
   ETK_TREE_COL_RESIZABLE_PROPERTY,
   ETK_TREE_COL_PLACE_PROPERTY,
   ETK_TREE_COL_EXPAND_PROPERTY
};

static Etk_Type *_etk_tree_grid_type_get();
static void _etk_tree_grid_constructor(Etk_Tree_Grid *grid);
static void _etk_tree_grid_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_tree_grid_scroll(Etk_Widget *widget, int x, int y);
static void _etk_tree_grid_scroll_size_get(Etk_Widget *widget, Etk_Size *scroll_size);
static void _etk_tree_grid_scroll_margins_get(Etk_Widget *widget, Etk_Size *margins_size);
static void _etk_tree_grid_realize_cb(Etk_Object *object, void *data);

static void _etk_tree_constructor(Etk_Tree *tree);
static void _etk_tree_destructor(Etk_Tree *tree);
static void _etk_tree_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_tree_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_tree_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_tree_realize_cb(Etk_Object *object, void *data);
static void _etk_tree_unrealize_cb(Etk_Object *object, void *data);

static void _etk_tree_col_constructor(Etk_Tree_Col *tree_col);
static void _etk_tree_col_destructor(Etk_Tree_Col *tree_col);
static void _etk_tree_col_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_tree_col_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

static void _etk_tree_expander_clicked_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree_row_pressed_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree_row_clicked_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree_row_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree_focus_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_unfocus_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_key_down_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_down_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_up_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_move_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_in_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_out_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_drag_drop_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_drag_end_cb(Etk_Object *object, void *data);

static void _etk_tree_update(Etk_Tree *tree);
static int _etk_tree_rows_draw(Etk_Tree *tree, Etk_Tree_Row *first_row, Evas_List **items_objects,
   int x, int w, int h, int xoffset, int yoffset, int first_row_id, int first_row_color);
static Etk_Tree_Row *_etk_tree_row_new_valist(Etk_Tree *tree, Etk_Tree_Row *row, va_list args);
static void _etk_tree_row_free(Etk_Tree_Row *row);
static void _etk_tree_row_fields_set_valist_full(Etk_Tree_Row *row, va_list args, Etk_Bool emit_value_changed_signal);
static Etk_Tree_Row_Objects *_etk_tree_row_objects_new(Etk_Tree *tree);
static void _etk_tree_row_objects_free(Etk_Tree_Row_Objects *row_objects, Etk_Tree *tree);
static void _etk_tree_col_realize(Etk_Tree *tree, int col_nth);
static Etk_Tree_Col *etk_tree_col_to_resize_get(Etk_Tree_Col *col, int x);

static void _etk_tree_row_selected_rows_get(Etk_Tree_Row *row, Evas_List **selected_rows);
static void _etk_tree_row_select_all(Etk_Tree_Row *row);
static void _etk_tree_row_unselect_all(Etk_Tree_Row *row);
static void _etk_tree_row_select(Etk_Tree *tree, Etk_Tree_Row *row, Evas_Modifier *modifiers);
static void _etk_tree_heapify(Etk_Tree *tree, Etk_Tree_Row **heap, int root, int size, int (*compare_cb)(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data), int asc, Etk_Tree_Col *col, void *data);

static Etk_Tree_Row *_etk_tree_last_clicked_row = NULL;
static Etk_Signal *_etk_tree_signals[ETK_TREE_NUM_SIGNALS];
static Etk_Signal *_etk_tree_col_signals[ETK_TREE_COL_NUM_SIGNALS];
static Etk_Bool    _etk_tree_drag_started = ETK_FALSE;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Tree
 * @return Returns the type on an Etk_Tree
 */
Etk_Type *etk_tree_type_get()
{
   static Etk_Type *tree_type = NULL;

   if (!tree_type)
   {
      tree_type = etk_type_new("Etk_Tree", ETK_CONTAINER_TYPE, sizeof(Etk_Tree), ETK_CONSTRUCTOR(_etk_tree_constructor), ETK_DESTRUCTOR(_etk_tree_destructor));

      _etk_tree_signals[ETK_TREE_ROW_SELECTED_SIGNAL] = etk_signal_new("row_selected", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_ROW_UNSELECTED_SIGNAL] = etk_signal_new("row_unselected", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_ROW_CLICKED_SIGNAL] = etk_signal_new("row_clicked", tree_type, -1, etk_marshaller_VOID__POINTER_POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_ROW_ACTIVATED_SIGNAL] = etk_signal_new("row_activated", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_ROW_EXPANDED_SIGNAL] = etk_signal_new("row_expaned", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_ROW_COLLAPSED_SIGNAL] = etk_signal_new("row_collapsed", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_SELECT_ALL_SIGNAL] = etk_signal_new("select_all", tree_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_tree_signals[ETK_TREE_UNSELECT_ALL_SIGNAL] = etk_signal_new("unselect_all", tree_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);

      etk_type_property_add(tree_type, "mode", ETK_TREE_MODE_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(ETK_TREE_MODE_LIST));
      etk_type_property_add(tree_type, "multiple_select", ETK_TREE_MULTIPLE_SELECT_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_type, "headers_visible", ETK_TREE_HEADERS_VISIBLE_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_type, "row_height", ETK_TREE_ROW_HEIGHT_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(24));
      
      tree_type->property_set = _etk_tree_property_set;
      tree_type->property_get = _etk_tree_property_get;      
   }

   return tree_type;
}

/**
 * @brief Creates a new tree
 * @return Returns the new tree widget
 */
Etk_Widget *etk_tree_new()
{
   return etk_widget_new(ETK_TREE_TYPE, "focusable", ETK_TRUE, "focus_on_press", ETK_TRUE, NULL);
}

/**
 * @brief Gets the type of an Etk_Tree_Col
 * @return Returns the type on an Etk_Tree_Col
 */
Etk_Type *etk_tree_col_type_get()
{
   static Etk_Type *tree_col_type = NULL;

   if (!tree_col_type)
   {
      tree_col_type = etk_type_new("Etk_Tree_Col", ETK_OBJECT_TYPE, sizeof(Etk_Tree_Col), ETK_CONSTRUCTOR(_etk_tree_col_constructor), ETK_DESTRUCTOR(_etk_tree_col_destructor));

      _etk_tree_col_signals[ETK_TREE_COL_CELL_VALUE_CHANGED] = etk_signal_new("cell_value_changed", tree_col_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      
      etk_type_property_add(tree_col_type, "title",         ETK_TREE_COL_TITLE_PROPERTY,           ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_string(NULL));
      etk_type_property_add(tree_col_type, "width",         ETK_TREE_COL_WIDTH_PROPERTY,           ETK_PROPERTY_INT,    ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(ETK_TREE_MIN_HEADER_WIDTH));
      etk_type_property_add(tree_col_type, "min_width",     ETK_TREE_COL_MIN_WIDTH_PROPERTY,       ETK_PROPERTY_INT,    ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(-1));
      etk_type_property_add(tree_col_type, "visible_width", ETK_TREE_COL_VISIBLE_WIDTH_PROPERTY,   ETK_PROPERTY_INT,    ETK_PROPERTY_READABLE,           NULL);
      etk_type_property_add(tree_col_type, "visible",       ETK_TREE_COL_VISIBLE_PROPERTY,         ETK_PROPERTY_BOOL,   ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_col_type, "resizable",     ETK_TREE_COL_RESIZABLE_PROPERTY,       ETK_PROPERTY_BOOL,   ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_col_type, "place",         ETK_TREE_COL_PLACE_PROPERTY,           ETK_PROPERTY_INT,    ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(0));
      etk_type_property_add(tree_col_type, "expand",        ETK_TREE_COL_EXPAND_PROPERTY,          ETK_PROPERTY_BOOL,   ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(ETK_FALSE));
      
      tree_col_type->property_set = _etk_tree_col_property_set;
      tree_col_type->property_get = _etk_tree_col_property_get;
   }

   return tree_col_type;
}

/**
 * @brief Creates a new column for a tree
 * @param tree a tree
 * @param title the tile of the column
 * @param model the model to use for the column
 * @param width the requested width of the column. It won't be necessary the visible width of the column since it can be expanded to fit the available space
 * @return Returns the new column
 */
Etk_Tree_Col *etk_tree_col_new(Etk_Tree *tree, const char *title, Etk_Tree_Model *model, int width)
{
   Etk_Tree_Col *new_col;
   Etk_Widget *new_header;

   if (!tree || !model)
      return NULL;
   if (model->col)
   {
      ETK_WARNING("The tree model to use for that column is already used for another column");
      return NULL;
   }

   tree->columns = realloc(tree->columns, sizeof(Etk_Tree_Col *) * (tree->num_cols + 1));
   new_col = ETK_TREE_COL(etk_object_new(ETK_TREE_COL_TYPE, "title", title, "width", width, "visible", ETK_TRUE, "resizable", ETK_TRUE, NULL));
   tree->columns[tree->num_cols] = new_col;

   new_col->id = tree->num_cols;
   new_col->place = new_col->id;
   etk_object_notify(ETK_OBJECT(new_col), "place");
   new_col->tree = tree;
   new_col->model = model;
   new_col->model->col = new_col;

   /* Creates the header widget */
   new_header = etk_widget_new(ETK_BUTTON_TYPE, "theme_group", "tree_header", "label", title, "xalign", 0.0, "repeat_events", ETK_TRUE, "visibility_locked", ETK_TRUE, NULL);
   etk_signal_connect("mouse_down", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_down_cb), new_col);
   etk_signal_connect("mouse_up", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_up_cb), new_col);
   etk_signal_connect("mouse_move", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_move_cb), new_col);
   etk_signal_connect("mouse_in", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_in_cb), new_col);
   etk_signal_connect("mouse_out", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_out_cb), new_col);
   etk_widget_parent_set(new_header, ETK_CONTAINER(tree));
   etk_widget_show(new_header);
   if (tree->headers_clip)
      etk_widget_clip_set(new_header, tree->headers_clip);
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
 * @param nth the rank of the column you want to get
 * @return Returns the "nth" column of the tree
 */
Etk_Tree_Col *etk_tree_nth_col_get(Etk_Tree *tree, int nth)
{
   if (!tree || nth < 0 || nth >= tree->num_cols)
      return NULL;
   return tree->columns[nth];
}

/**
 * @brief Sets whether the column headers should be displayed
 * @param tree a tree
 * @param headers_visible ETK_TRUE if the column headers should be displayed
 */
void etk_tree_headers_visible_set(Etk_Tree *tree, Etk_Bool headers_visible)
{
   if (!tree || (tree->headers_visible == headers_visible))
      return;

   tree->headers_visible = headers_visible;
   etk_object_notify(ETK_OBJECT(tree), "headers_visible");
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Gets the visibility of the column headers
 * @param tree a tree
 * @return Returns ETK_TRUE whether the column headers are visible
 */
Etk_Bool etk_tree_headers_visible_get(Etk_Tree *tree)
{
   if (!tree)
      return ETK_FALSE;
   return tree->headers_visible;
}

/**
 * @brief Sets the title of the column
 * @param col a tree column
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
 * @param col a tree column
 * @return Returns the title of the column
 */
const char *etk_tree_col_title_get(Etk_Tree_Col *col)
{
   if (!col || !col->header)
      return NULL;
   return etk_button_label_get(ETK_BUTTON(col->header));
}

/**
 * @brief Sets the requested width of the column. It won't be necessary the visible width of the column since it can be expanded to fit the available space
 * @param col a tree column
 * @param width the requested width to set
 */
void etk_tree_col_width_set(Etk_Tree_Col *col, int width)
{
   if (!col)
      return;
   col->requested_width = ETK_MAX(width, ETK_TREE_MIN_HEADER_WIDTH);
   etk_object_notify(ETK_OBJECT(col), "width");
   if (col->tree)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(col->tree->grid), NULL);
      etk_widget_redraw_queue(ETK_WIDGET(col->tree->grid));
   }
}

/**
 * @brief Gets the requested width of the column
 * @param col a tree column
 * @return Returns the requested width of the column
 */
int etk_tree_col_width_get(Etk_Tree_Col *col)
{
   if (!col)
      return 0;
   return col->requested_width;
}

/**
 * @brief Sets the minimum width of the column. The column couldn't be smaller than this width
 * @param col a tree column
 * @param min_width the minimum width to set. -1 to make etk calculate the min_width
 */
void etk_tree_col_min_width_set(Etk_Tree_Col *col, int min_width)
{
   if (!col || (col->min_width == min_width))
      return;
   col->min_width = min_width;
   etk_object_notify(ETK_OBJECT(col), "min_width");
   if (col->tree)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(col->tree->grid), NULL);
      etk_widget_redraw_queue(ETK_WIDGET(col->tree->grid));
   }
}

/**
 * @brief Gets the minimum width of the column
 * @param col a tree column
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
 * @param col a tree column
 * @param resizable ETK_TRUE whether the column should be resizable
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
 * @param col a tree column
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
 * @param col a tree column
 * @param expand ETK_TRUE whether the column should be expandable
 */
void etk_tree_col_expand_set(Etk_Tree_Col *col, Etk_Bool expand)
{
   if (!col || (col->expand == expand))
      return;
   col->expand = expand;
   etk_object_notify(ETK_OBJECT(col), "expand");
   if (col->tree)
      etk_widget_redraw_queue(ETK_WIDGET(col->tree->grid));
}

/**
 * @brief Gets whether the column expands if there is some free space in the tree
 * @param col a tree column
 * @return Returns ETK_TRUE if the column is expandable
 */
Etk_Bool etk_tree_col_expand_get(Etk_Tree_Col *col)
{
   if (!col)
      return ETK_FALSE;
   return col->expand;
}

/**
 * @brief Sets whether the column is visible
 * @param col a tree column
 * @param visible ETK_TRUE whether the column should be visible
 */
void etk_tree_col_visible_set(Etk_Tree_Col *col, Etk_Bool visible)
{
   if (!col || (col->visible == visible))
      return;
   col->visible = visible;
   etk_object_notify(ETK_OBJECT(col), "visible");
   if (col->tree)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(col->tree->grid), NULL);
      etk_widget_redraw_queue(ETK_WIDGET(col->tree->grid));
   }
}

/**
 * @brief Gets whether the column is visible
 * @param col a tree column
 * @return Returns ETK_TRUE if the column is visible
 */
Etk_Bool etk_tree_col_visible_get(Etk_Tree_Col *col)
{
   if (!col)
      return ETK_FALSE;
   return col->visible;
}

/**
 * @brief Reorders the the column
 * @param col a tree column
 * @param new_place the new place that the column should take (0 is the first column on the left of the tree, etk_tree_num_cols_get(tree) - 1 is the last one on the right)
 */
void etk_tree_col_reorder(Etk_Tree_Col *col, int new_place)
{
   int i;

   if (!col || !col->tree || (col->place == new_place))
      return;

   new_place = ETK_CLAMP(new_place, 0, col->tree->num_cols - 1);
   if (new_place < col->place)
   {
      for (i = 0; i < col->tree->num_cols; i++)
      {
         if (col->tree->columns[i]->place >= new_place && col->tree->columns[i]->place < col->place)
         {
            col->tree->columns[i]->place++;
            etk_object_notify(ETK_OBJECT(col->tree->columns[i]), "place");
         }
      }
   }
   else if (new_place > col->place)
   {
      for (i = 0; i < col->tree->num_cols; i++)
      {
         if (col->tree->columns[i]->place > col->place && col->tree->columns[i]->place <= new_place)
         {
            col->tree->columns[i]->place--;
            etk_object_notify(ETK_OBJECT(col->tree->columns[i]), "place");
         }
      }
   }
   col->place = new_place;
   etk_object_notify(ETK_OBJECT(col), "place");
   etk_widget_redraw_queue(ETK_WIDGET(col->tree->grid));
}

/**
 * @brief Gets the place of the column (0 is the first column on the left of the tree, etk_tree_num_cols_get(tree) - 1 is the last one on the right)
 * @param col a tree column
 * @return Returns the place of the column
 */
int etk_tree_col_place_get(Etk_Tree_Col *col)
{
   if (!col)
      return 0;
   return col->place;
}

/**
 * @brief Sets the mode used by the tree. The tree has to be not built
 * @param tree a tree
 * @param mode the mode which will be used by the tree: ETK_TREE_MODE_LIST (rows can not have children) or ETK_TREE_MODE_TREE (rows can have children)
 */
void etk_tree_mode_set(Etk_Tree *tree, Etk_Tree_Mode mode)
{
   if (!tree || tree->built)
      return;
   tree->mode = mode;
   etk_object_notify(ETK_OBJECT(tree), "mode");
}

/**
 * @brief Gets the mode used by the tree
 * @param tree a tree
 * @return Returns the mode used by the tree: ETK_TREE_MODE_LIST (rows can not have children) or ETK_TREE_MODE_TREE (rows can have children)
 */
Etk_Tree_Mode etk_tree_mode_get(Etk_Tree *tree)
{
   if (!tree)
      return ETK_TREE_MODE_LIST;
   return tree->mode;
}

/**
 * @brief Builds the tree. This function to be called after having added all the columns and before being able to add rows to the tree
 * @param tree a tree
 */
void etk_tree_build(Etk_Tree *tree)
{
   if (!tree)
      return;
   tree->built = ETK_TRUE;
}

/**
 * @brief Freezes the tree: it will not be updated until it is thawed @n
 * This function is useful when you want to add a lot of rows quickly.
 * @param tree a tree
 */
void etk_tree_freeze(Etk_Tree *tree)
{
   if (!tree)
      return;
   tree->frozen = ETK_TRUE;
}

/**
 * @brief Thaws the tree: it will update the tree if it was frozen
 * @param tree a tree
 */
void etk_tree_thaw(Etk_Tree *tree)
{
   if (!tree || !tree->frozen)
      return;
   
   etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(tree->grid), NULL);
   etk_widget_redraw_queue(ETK_WIDGET(tree->grid));
   tree->frozen = ETK_FALSE;
}

/**
 * @brief Sets the height of the rows of the tree
 * @param tree a tree
 * @param row_height the new height of the rows. If @a row_height < 8, the tree will use the theme default value
 */
void etk_tree_row_height_set(Etk_Tree *tree, int row_height)
{
   if (!tree || (tree->row_height == row_height && !tree->use_default_row_height))
      return;
   
   if (row_height < ETK_TREE_MIN_ROW_HEIGHT)
   {
      tree->use_default_row_height = ETK_TRUE;
      if (etk_widget_theme_object_data_get(tree->grid, "row_height", "%d", &tree->row_height) != 1 || tree->row_height < ETK_TREE_MIN_ROW_HEIGHT)
         tree->row_height = ETK_TREE_MIN_ROW_HEIGHT;
      etk_range_increments_set(etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(tree->scrolled_view)), tree->row_height, 5 * tree->row_height);
   }
   else
   {
      tree->use_default_row_height = ETK_FALSE;
      tree->row_height = row_height;
   }
   
   etk_object_notify(ETK_OBJECT(tree), "row_height");
   etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(tree->grid), NULL);
   etk_widget_redraw_queue(ETK_WIDGET(tree->grid));
   etk_range_increments_set(etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(tree->scrolled_view)), tree->row_height, 5 * tree->row_height);
}

/**
 * @brief Gets the height of the rows of the tree
 * @param tree a tree
 * @return Returns the height of the rows of the tree
 */
int etk_tree_row_height_get(Etk_Tree *tree)
{
   if (!tree)
      return 0;
   return tree->row_height;
}

/**
 * @brief Sets whether several rows can be selected in the tree
 * @param tree a tree
 * @param multiple_select ETK_TRUE if several rows can be selected
 */
void etk_tree_multiple_select_set(Etk_Tree *tree, Etk_Bool multiple_select)
{
   if (!tree)
      return;

   if (!multiple_select)
      etk_tree_unselect_all(tree);
   tree->multiple_select = multiple_select;
   etk_object_notify(ETK_OBJECT(tree), "multiple_select");
}

/**
 * @brief Gets whether several rows can be selected in the tree.
 * @param tree a tree
 * @return ETK_TRUE if several rows can be selected
 */
Etk_Bool etk_tree_multiple_select_get(Etk_Tree *tree)
{
   if (!tree)
      return ETK_FALSE;
   return tree->multiple_select;
}

/**
 * @brief Selects all the rows of the tree. Multiple selection has to be enabled with etk_tree_multiple_select_set()
 * @param tree a tree
 */
void etk_tree_select_all(Etk_Tree *tree)
{
   if (!tree || !tree->multiple_select)
      return;

   _etk_tree_row_select_all(&tree->root);
   etk_signal_emit(_etk_tree_signals[ETK_TREE_SELECT_ALL_SIGNAL], ETK_OBJECT(tree), NULL);
   etk_widget_redraw_queue(ETK_WIDGET(tree->grid));
}

/**
 * @brief Unselects all the rows of the tree
 * @param tree a tree
 */
void etk_tree_unselect_all(Etk_Tree *tree)
{
   if (!tree)
      return;

   if (tree->last_selected && tree->last_selected->selected)
   {
      tree->last_selected->selected = ETK_FALSE;
      etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_UNSELECTED_SIGNAL], ETK_OBJECT(tree), NULL, tree->last_selected);
   }
   if (tree->multiple_select)
   {
      _etk_tree_row_unselect_all(&tree->root);
      etk_signal_emit(_etk_tree_signals[ETK_TREE_UNSELECT_ALL_SIGNAL], ETK_OBJECT(tree), NULL);
   }
   tree->num_selected_rows = 0;
   etk_widget_redraw_queue(ETK_WIDGET(tree->grid));
}

/**
 * @brief Appends a new row to the tree
 * @param tree a tree
 * @param ... an Etk_Tree_Col * followed by the value of the cell, then again, an Etk_Tree_Col * followed by its value... terminated by NULL
 * @return Returns the new row
 */
Etk_Tree_Row *etk_tree_append(Etk_Tree *tree, ...)
{
   Etk_Tree_Row *new_row;
   va_list args;

   if (!tree || !tree->built)
      return NULL;

   va_start(args, tree);
   new_row = _etk_tree_row_new_valist(tree, &tree->root, args);
   va_end(args);

   return new_row;
}

/**
 * @brief Appends a new row as a child of a another row of the tree. The tree has to be in the ETK_TREE_MODE_TREE mode
 * @param row a row
 * @param ... an Etk_Tree_Col * followed by the value(s) of the cell, then again, an Etk_Tree_Col * followed by its value(s)... terminated by NULL
 * @return Returns the new row
 */
Etk_Tree_Row *etk_tree_append_to_row(Etk_Tree_Row *row, ...)
{
   Etk_Tree_Row *new_row;
   va_list args;

   if (!row || !row->tree || !row->tree->built || (row->tree->mode != ETK_TREE_MODE_TREE))
      return NULL;

   va_start(args, row);
   new_row = _etk_tree_row_new_valist(row->tree, row, args);
   va_end(args);

   return new_row;
}

/**
 * @brief Removes a row from the tree
 * @param row the the row to remove
 */
void etk_tree_row_del(Etk_Tree_Row *row)
{
   Etk_Tree_Row *r;

   if (!row)
      return;

   for (r = row->tree->last_selected; r; r = r->parent)
   {
      if (row == r)
      {
         row->tree->last_selected = NULL;
         break;
      }
   }
   
   if (!row->tree->frozen)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(row->tree->grid), NULL);
      etk_widget_redraw_queue(ETK_WIDGET(row->tree->grid));
   }
   
   _etk_tree_row_free(row);
}

/**
 * @brief Removes all the rows of the tree 
 * @param tree a tree 
 */
void etk_tree_clear(Etk_Tree *tree)
{
   if (!tree)
      return;

   while (tree->root.first_child)
      _etk_tree_row_free(tree->root.first_child);
   tree->last_selected = NULL;
   
   etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(tree->grid), NULL);
   etk_widget_redraw_queue(ETK_WIDGET(tree->grid));
}

/**
 * @brief Sorts the tree
 * @param tree a tree
 * @param compare_cb the function to call to compare two rows. Must return a negative value if row1 is less than row2, @n
 * 0 is row1 is equal to row2, and a positive value is row1 is greater than row2.
 * @param ascendant ETK_TRUE if the lowest rows have to be the first ones in the sorted tree
 * @param col the column to pass to compare_cb when it's called
 * @param data the data to pass to compare_cb when it's called
 */
void etk_tree_sort(Etk_Tree *tree, int (*compare_cb)(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data), Etk_Bool ascendant, Etk_Tree_Col *col, void *data)
{
   int num_rows, i, pos, parent;
   Etk_Tree_Row *r, *first_row;
   Etk_Tree_Row **heap;
   int asc;
   
   if (!tree || !compare_cb)
      return;
   
   for (num_rows = 0, r = tree->root.first_child; r; r = r->next, num_rows++);
   if (num_rows <= 1)
      return;
   heap = malloc(num_rows * sizeof(Etk_Tree_Row *));
   asc = ascendant ? -1 : 1;
   
   /* We insert all the rows in the heap */
   heap[0] = tree->root.first_child;
   for (i = 1, r = tree->root.first_child->next; r; r = r->next, i++)
   {
      pos = i;
      parent = (pos - 1) / 2;
      
      while (parent >= 0 && (compare_cb(tree, heap[parent], r, col, data) * asc) < 0)
      {
         heap[pos] = heap[parent];
         pos = parent;
         parent = parent ? (pos - 1) / 2 : -1;
      }
      heap[pos] = r;
   }
   
   /* Then we extract them */
   first_row = heap[0];
   first_row->prev = NULL;
   first_row->next = NULL;
   r = first_row;
   heap[0] = heap[num_rows - 1];
   _etk_tree_heapify(tree, heap, 0, num_rows - 1, compare_cb, asc, col, data);
   for (i = num_rows - 2; i >= 0; i--)
   {
      r->next = heap[0];
      heap[0]->prev = r;
      r = heap[0];
      heap[0] = heap[i];
      _etk_tree_heapify(tree, heap, 0, i, compare_cb, asc, col, data);
   }
   r->next = NULL;
   
   tree->root.first_child = first_row;
   tree->root.last_child = r;
   free(heap);
   
   etk_widget_redraw_queue(ETK_WIDGET(tree->grid));
}

/**
 * @brief Gets the first row of the tree
 * @param tree a tree
 * @return Returns the first row of the tree
 */
Etk_Tree_Row *etk_tree_first_row_get(Etk_Tree *tree)
{
   if (!tree)
      return NULL;
   return etk_tree_row_first_child_get(&tree->root);
}

/**
 * @brief Gets the last row of the tree
 * @param tree a tree
 * @param walking_through_hierarchy if ETK_TRUE, the last row is not necessary on the first level of hierarchy. @n
 * This setting has no effect if the tree is in the list mode
 * @param include_collapsed_children if ETK_TRUE, the last row can be a child of a collapsed row. @n
 * This setting has no effect if the tree is in the list mode, or if @a walking_through_hierarchy is ETK_FALSE
 * @return Returns the last row of the tree
 */
Etk_Tree_Row *etk_tree_last_row_get(Etk_Tree *tree, Etk_Bool walking_through_hierarchy, Etk_Bool include_collapsed_children)
{
   if (!tree)
      return NULL;
   return etk_tree_row_last_child_get(&tree->root, walking_through_hierarchy, include_collapsed_children);
}

/**
 * @brief Gets the first child of the row
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
 * @brief Gets the last child of the row
 * @param row a row
 * @param walking_through_hierarchy if ETK_TRUE, the last child is not necessary directly a child of the row.
 * @param include_collapsed_children if ETK_TRUE, the last child can be a child of a collapsed row. @n
 * This setting has no effect if @a walking_through_hierarchy is ETK_FALSE
 * @return Returns the last child of the row
 */
Etk_Tree_Row *etk_tree_row_last_child_get(Etk_Tree_Row *row, Etk_Bool walking_through_hierarchy, Etk_Bool include_collapsed_children)
{
   if (!row)
      return NULL;
   
   if (!walking_through_hierarchy)
      return row->last_child;
   else
   {
      Etk_Tree_Row *last;
      
      for (last = row->last_child; last && (include_collapsed_children || last->expanded) && last->last_child; last = last->last_child);
      return last;
   }
}

/**
 * @brief Gets the previous row of the tree
 * @param row the current row
 * @param walking_through_hierarchy if ETK_TRUE, the previous row can be a row located on a different level in the hierarchy. @n
 * This setting has no effect if the tree is in the list mode
 * @param include_collapsed_children if ETK_TRUE, the previous row can be a child of a collapsed row. @n
 * This setting has no effect if the tree is in the list mode, or if @a walking_through_hierarchy is ETK_FALSE
 * @return Returns the previous row of the tree
 */
Etk_Tree_Row *etk_tree_prev_row_get(Etk_Tree_Row *row, Etk_Bool walking_through_hierarchy, Etk_Bool include_collapsed_children)
{
   if (!row)
      return NULL;
   
   if (row->tree->mode == ETK_TREE_MODE_LIST || !walking_through_hierarchy)
      return row->prev;
   else
   {
      if (!row->prev)
         return (row->parent != &row->tree->root) ? row->parent : NULL;
      else
      {
         Etk_Tree_Row *prev;
         
         for (prev = row->prev; prev && (include_collapsed_children || prev->expanded) && prev->last_child; prev = prev->last_child);
         return prev;
      }
   }
}

/**
 * @brief Gets the next row of the tree
 * @param row the current row
 * @param walking_through_hierarchy if ETK_TRUE, the next row can be a row located on a different level in the hierarchy. @n
 * This setting has no effect if the tree is in the list mode
 * @param include_collapsed_children if ETK_TRUE, the next row can be a child of a collapsed row. @n
 * This setting has no effect if the tree is in the list mode, or if @a walking_through_hierarchy is ETK_FALSE
 * @return Returns the next row of the tree
 */
Etk_Tree_Row *etk_tree_next_row_get(Etk_Tree_Row *row, Etk_Bool walking_through_hierarchy, Etk_Bool include_collapsed_children)
{
   if (!row)
      return NULL;
   
   if (row->tree->mode == ETK_TREE_MODE_LIST || !walking_through_hierarchy)
      return row->next;
   else
   {
      if ((include_collapsed_children || row->expanded) && row->first_child)
         return row->first_child;
      else
      {
         if (row->next)
            return row->next;
         else
         {
            Etk_Tree_Row *next;
            
            for (next = row->parent; next && !next->next; next = next->parent);
            return next ? next->next : NULL;
         }
      }
   }
}

/**
 * @brief Sets the different values of the cells of the row
 * @param row a row
 * @param ... an Etk_Tree_Col * followed by the value(s) of the cell, then again, an Etk_Tree_Col * followed by its value(s)... terminated by NULL
 */
void etk_tree_row_fields_set(Etk_Tree_Row *row, ...)
{
   va_list args;

   va_start(args, row);
   etk_tree_row_fields_set_valist(row, args);
   va_end(args);
}

/**
 * @brief Sets the different values of the cells of the row
 * @param row a row
 * @param args an Etk_Tree_Col * followed by the value(s) of the cell, then again, an Etk_Tree_Col * followed by its value(s)... terminated by NULL
 */
void etk_tree_row_fields_set_valist(Etk_Tree_Row *row, va_list args)
{
   _etk_tree_row_fields_set_valist_full(row, args, ETK_TRUE);
}
   
/**
 * @brief Gets the different values of the cells of the row
 * @param row a row
 * @param ... an Etk_Tree_Col * followed by the location where to store the value(s) of the cell, then again, an Etk_Tree_Col * followed by its location(s)... terminated by NULL
 */
void etk_tree_row_fields_get(Etk_Tree_Row *row, ...)
{
   va_list args;

   va_start(args, row);
   etk_tree_row_fields_get_valist(row, args);
   va_end(args);
}

/**
 * @brief Gets the different values of the cells of the row
 * @param row a row
 * @param args an Etk_Tree_Col * followed by the location where to store the value(s) of the cell, then again, an Etk_Tree_Col * followed by its location(s)... terminated by NULL
 */
void etk_tree_row_fields_get_valist(Etk_Tree_Row *row, va_list args)
{
   Etk_Tree_Col *col;
   va_list args2;

   if (!row)
      return;

   va_copy(args2, args);
   while ((col = va_arg(args2, Etk_Tree_Col *)))
   {
      if (col->model->cell_data_get)
         col->model->cell_data_get(col->model, row->cells_data[col->id], &args2);
   }
   va_end(args2);
}

/**
 * @brief Sets a value to the data member of a row. The date could be retrieved with @a etk_tree_row_data_get()
 * @param row a row
 * @param data the data to set
 */
void etk_tree_row_data_set(Etk_Tree_Row *row, void *data)
{
   etk_tree_row_data_set_full(row, data, NULL);
}

/**
 * @brief Sets a value to the data member of a row. The date could be retrieved with @a etk_tree_row_data_get()
 * @param row a row
 * @param data the data to set
 * @param free_cb the function to call to free the data
 */
void etk_tree_row_data_set_full(Etk_Tree_Row *row, void *data, void (*free_cb)(void *data))
{
   if (!row)
      return;
   
   if (row->data && row->data_free_cb)
      row->data_free_cb(row->data);
   
   row->data = data;
   row->data_free_cb = free_cb;
}

/**
 * @brief Gets the value of the data member of a row
 * @param row a row
 * @return Returns the value of the data member of a row
 */
void *etk_tree_row_data_get(Etk_Tree_Row *row)
{
   if (!row)
      return NULL;
   return row->data;
}

/**
 * @brief Makes the tree scroll to show the row
 * @param row the row up to which you want to scroll
 * @param center_the_row ETK_TRUE if you want the row to be centered
 */
void etk_tree_row_scroll_to(Etk_Tree_Row *row, Etk_Bool center_the_row)
{
   Etk_Tree *tree;
   Etk_Tree_Row *r;
   int row_offset;
   int tree_height;
   int i;
   int new_xoffset;
   
   if (!row || !(tree = row->tree))
      return;
   
   for (r = tree->root.first_child, i = 0; r; r = etk_tree_next_row_get(r, ETK_TRUE, ETK_FALSE), i++)
   {
      if (r == row)
      {
         row_offset = i * tree->row_height;
         tree_height = tree->grid->inner_geometry.h;
         
         /* If the row is already entirely visible, we do nothing */
         if (!center_the_row && (row_offset >= tree->yoffset && (row_offset + tree->row_height) <= (tree->yoffset + tree_height)))
            return;
         
         if (center_the_row)
            new_xoffset = row_offset + (tree->row_height - tree_height) / 2;
         else if (row_offset < tree->yoffset)
            new_xoffset = row_offset;
         else
            new_xoffset = row_offset - tree_height + tree->row_height;
         
         etk_range_value_set(etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(tree->scrolled_view)), new_xoffset);
         return;
      }
   }
}

/**
 * @brief Selects the row
 * @param row the row to select
 */
void etk_tree_row_select(Etk_Tree_Row *row)
{
   if (!row)
      return;
   _etk_tree_row_select(row->tree, row, NULL);
}

/**
 * @brief Unselects the row
 * @param row the row to unselect
 */
void etk_tree_row_unselect(Etk_Tree_Row *row)
{
   if (!row || !row->selected)
      return;
   
   row->selected = ETK_FALSE;
   etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_UNSELECTED_SIGNAL], ETK_OBJECT(row->tree), NULL, row);
   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree->grid));
}

/**
 * @brief Gets the selected row of the tree
 * @param tree a tree
 * @return Returns the selected row of the tree
 */
Etk_Tree_Row *etk_tree_selected_row_get(Etk_Tree *tree)
{
   if (!tree || !tree->last_selected || !tree->last_selected->selected)
      return NULL;
   return tree->last_selected;
}

/**
 * @brief Gets all the selected rows of the tree
 * @param tree a tree
 * @return Returns an Evas_List * containing the selected rows of the tree
 * @warning The returned Evas_List * should be freed with @a evas_list_free()
 */
Evas_List *etk_tree_selected_rows_get(Etk_Tree *tree)
{
   Evas_List *selected_rows = NULL;

   if (!tree)
      return NULL;

   if (!tree->multiple_select)
   {
      if (!tree->last_selected || !tree->last_selected->selected)
         return NULL;
      selected_rows = evas_list_append(selected_rows, tree->last_selected);
   }
   else
      _etk_tree_row_selected_rows_get(&tree->root, &selected_rows);

   return selected_rows;
}

/**
 * @brief Expands the row. The child rows of the row will be displayed. It will only affect a tree in the ETK_TREE_MODE_TREE mode
 * @param row a row
 */
void etk_tree_row_expand(Etk_Tree_Row *row)
{
   Etk_Tree_Row *r;

   if (!row || row->expanded || !row->tree || (row->tree->mode != ETK_TREE_MODE_TREE))
      return;

   row->expanded = ETK_TRUE;
   for (r = row->parent; r && r->expanded; r = r->parent)
      r->num_visible_children += row->num_visible_children;

   etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_EXPANDED_SIGNAL], ETK_OBJECT(row->tree), NULL, row);
   if (!row->tree->frozen)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(row->tree->grid), NULL);
      etk_widget_redraw_queue(ETK_WIDGET(row->tree->grid));
   }
}

/**
 * @brief Collapses the row. The child rows of the row are no longer displayed. It will only affect a tree in the ETK_TREE_MODE_TREE mode
 * @param row a row
 */
void etk_tree_row_collapse(Etk_Tree_Row *row)
{
   Etk_Tree_Row *r;

   if (!row || !row->expanded || !row->tree || (row->tree->mode != ETK_TREE_MODE_TREE))
      return;

   row->expanded = ETK_FALSE;
   for (r = row->parent; r && r->expanded; r = r->parent)
      r->num_visible_children -= row->num_visible_children;

   etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_COLLAPSED_SIGNAL], ETK_OBJECT(row->tree), NULL, row);
   if (!row->tree->frozen)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(row->tree->grid), NULL);
      etk_widget_redraw_queue(ETK_WIDGET(row->tree->grid));
   }
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/**************************
 * Tree Grid
 **************************/

/* Creates a new type for the tree grid widget (a tree is composed by a tree grid, column headers and scrollbars */
static Etk_Type *_etk_tree_grid_type_get()
{
   static Etk_Type *grid_type = NULL;

   if (!grid_type)
      grid_type = etk_type_new("Etk_Tree_Grid", ETK_CONTAINER_TYPE, sizeof(Etk_Tree_Grid), ETK_CONSTRUCTOR(_etk_tree_grid_constructor), NULL);

   return grid_type;
}

/* Initializes the default values of the tree grid */
static void _etk_tree_grid_constructor(Etk_Tree_Grid *grid)
{
   if (!grid)
      return;

   ETK_WIDGET(grid)->size_allocate = _etk_tree_grid_size_allocate;
   ETK_WIDGET(grid)->scroll = _etk_tree_grid_scroll;
   ETK_WIDGET(grid)->scroll_size_get = _etk_tree_grid_scroll_size_get;
   ETK_WIDGET(grid)->scroll_margins_get = _etk_tree_grid_scroll_margins_get;
   etk_signal_connect("realize", ETK_OBJECT(grid), ETK_CALLBACK(_etk_tree_grid_realize_cb), NULL);
}

/* Creates or destroys the rows according to the new height of the tree grid, and then updates the tree */
static void _etk_tree_grid_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Tree *tree;
   Etk_Tree_Grid *grid;
   Etk_Size child_request;
   Etk_Geometry child_geometry;
   int num_visible_cols = 0, num_expandable_cols = 0;
   int grid_width = 0, freespace;
   float extra_width = 0.0;
   Etk_Tree_Col *first_visible_col, *last_visible_col;
   int xoffset = 0;
   int max_header_height = 0;
   int num_visible_items;
   int num_item_to_add;
   int i, j, k;

   if (!(grid = ETK_TREE_GRID(widget)) || !(tree = grid->tree) || !(tree->built))
      return;
   
   first_visible_col = NULL;
   last_visible_col = NULL;
   for (i = 0; i < tree->num_cols; i++)
   {
      if (tree->columns[i]->visible)
      {
         if (!first_visible_col || first_visible_col->place > tree->columns[i]->place)
            first_visible_col = tree->columns[i];
         if (!last_visible_col || last_visible_col->place < tree->columns[i]->place)
            last_visible_col = tree->columns[i];
      }
   }
   if (!first_visible_col)
      return;

   /* Calculates the size of the cols */
   for (i = 0; i < tree->num_cols; i++)
   {
      if (tree->headers_visible)
      {
         if (tree->columns[i]->min_width < 0)
         {
            Etk_Size header_requested_size;
      
            etk_widget_size_request(tree->columns[i]->header, &header_requested_size);
            if (tree->columns[i] == first_visible_col)
               header_requested_size.w -= widget->left_inset;
            if (tree->columns[i] == last_visible_col)
               header_requested_size.w -= widget->right_inset;
            tree->columns[i]->width = ETK_MAX(header_requested_size.w, tree->columns[i]->requested_width);
         }
         else
            tree->columns[i]->width = ETK_MAX(tree->columns[i]->min_width, tree->columns[i]->requested_width);
         
         grid_width += tree->columns[i]->width;
         num_visible_cols++;
         if (tree->columns[i]->expand)
            num_expandable_cols++;
      }
   }
   
   freespace = ETK_MAX(0, geometry.w - grid_width);
   if (num_expandable_cols > 0)
      extra_width = (float)freespace / num_expandable_cols;

   k = 0;
   for (i = 0; i < tree->num_cols; i++)
   {
      for (j = 0; j < tree->num_cols; j++)
      {
         if (tree->columns[j]->place == i)
         {
            if (tree->columns[j]->visible)
            {
               tree->columns[j]->xoffset = xoffset - tree->xoffset;
               tree->columns[j]->visible_width = tree->columns[j]->width;
               if (num_expandable_cols > 0 && tree->columns[j]->expand)
                  tree->columns[j]->visible_width += extra_width;
               else if (num_expandable_cols == 0 && tree->columns[j] == last_visible_col)
                  tree->columns[j]->visible_width += freespace;
               etk_object_notify(ETK_OBJECT(tree->columns[j]), "visible_width");
               xoffset += tree->columns[j]->visible_width;
               k++;
            }
            break;
         }
      }
   }

   /* Allocates size for the headers */
   if (tree->headers_visible)
   {
      for (i = 0; i < tree->num_cols; i++)
      {
         etk_widget_size_request(tree->columns[i]->header, &child_request);
         if (child_request.h > max_header_height)
            max_header_height = child_request.h;
      }

      child_geometry.h = max_header_height;
      for (i = 0; i < tree->num_cols; i++)
      {
         child_geometry.x = ETK_WIDGET(tree)->inner_geometry.x + tree->columns[i]->xoffset;
         if (tree->columns[i] != first_visible_col)
            child_geometry.x += widget->left_inset;
         child_geometry.y = ETK_WIDGET(tree)->inner_geometry.y;

         if (tree->columns[i] == last_visible_col)
            child_geometry.w = ETK_WIDGET(tree)->geometry.x + ETK_WIDGET(tree)->geometry.w - child_geometry.x;
         else
         {
            child_geometry.w = tree->columns[i]->visible_width;
            if (tree->columns[i] == first_visible_col)
               child_geometry.w += tree->grid->left_inset;
         }
         
         etk_widget_size_allocate(tree->columns[i]->header, child_geometry);
      }
   }
   
   /* Create or delete row objects if necessary */
   num_visible_items = ceil((float)geometry.h / tree->row_height) + 1;
   if (num_visible_items < 0)
      num_visible_items = 0;
   num_item_to_add = num_visible_items - evas_list_count(tree->rows_widgets);

   if (num_item_to_add > 0)
   {
      Etk_Tree_Row_Objects *row_objects;

      for (i = 0; i < num_item_to_add; i++)
      {
         if (!(row_objects = _etk_tree_row_objects_new(tree)))
            break;
         tree->rows_widgets = evas_list_append(tree->rows_widgets, row_objects);
      }
   }
   else if (num_item_to_add < 0)
   {
      for (i = 0; i < -num_item_to_add; i++)
      {
         if (!tree->rows_widgets)
            break;
         _etk_tree_row_objects_free(tree->rows_widgets->data, tree);
         tree->rows_widgets = evas_list_remove_list(tree->rows_widgets, tree->rows_widgets);
      }
   }

   evas_object_move(grid->clip, geometry.x, geometry.y);
   evas_object_resize(grid->clip, geometry.w, geometry.h);

   /* Then, updates the tree */
   _etk_tree_update(tree);
}

/* Scrolls the tree grid */
static void _etk_tree_grid_scroll(Etk_Widget *widget, int x, int y)
{
   Etk_Tree *tree;
   
   if (!widget || !(tree = ETK_TREE_GRID(widget)->tree))
      return;
   
   tree->xoffset = x;
   tree->yoffset = y;
   etk_widget_redraw_queue(ETK_WIDGET(tree->grid));
}

/* Gets the scrolling size of the tree grid */
static void _etk_tree_grid_scroll_size_get(Etk_Widget *widget, Etk_Size *scroll_size)
{
   Etk_Tree *tree;
   int i;
   int width = 0;
   
   if (!widget || !(tree = ETK_TREE_GRID(widget)->tree) || !scroll_size)
      return;
   
   for (i = 0; i < tree->num_cols; i++)
   {
      if (tree->columns[i]->visible)
         width += tree->columns[i]->width;
   }
   scroll_size->w = width;
   scroll_size->h = tree->root.num_visible_children * tree->row_height;
}

/* Gets the scrolling margins size of the tree grid */
static void _etk_tree_grid_scroll_margins_get(Etk_Widget *widget, Etk_Size *margins_size)
{
   if (!widget || !margins_size)
      return;
   
   margins_size->w = widget->left_inset + widget->right_inset;
   margins_size->h = widget->top_inset + widget->bottom_inset;
}

/**************************
 * Tree
 **************************/

/* Initializes the default values of the tree */
static void _etk_tree_constructor(Etk_Tree *tree)
{
   if (!tree)
      return;
   
   tree->scrolled_view = etk_scrolled_view_new();
   etk_widget_visibility_locked_set(tree->scrolled_view, ETK_TRUE);
   etk_widget_repeat_events_set(tree->scrolled_view, ETK_TRUE);
   etk_widget_parent_set(tree->scrolled_view, ETK_CONTAINER(tree));
   etk_widget_show(tree->scrolled_view);
   
   tree->grid = etk_widget_new(ETK_TREE_GRID_TYPE, "theme_group", "tree", "repeat_events", ETK_TRUE, "visibility_locked", ETK_TRUE, NULL);
   ETK_TREE_GRID(tree->grid)->tree = tree;
   etk_container_add(ETK_CONTAINER(tree->scrolled_view), tree->grid);
   etk_widget_show(tree->grid);

   tree->num_cols = 0;
   tree->columns = NULL;
   tree->last_selected = NULL;
   tree->column_to_resize = NULL;
   tree->resize_pointer_shown = ETK_FALSE;
   tree->headers_visible = ETK_TRUE;

   tree->root.tree = tree;
   tree->root.parent = NULL;
   tree->root.prev = NULL;
   tree->root.next = NULL;
   tree->root.first_child = NULL;
   tree->root.last_child = NULL;
   tree->root.num_visible_children = 0;
   tree->root.num_parent_children = 0;
   tree->root.expanded = ETK_TRUE;
   tree->root.cells_data = NULL;
   tree->root.data = NULL;

   tree->row_height = 0;
   tree->use_default_row_height = ETK_TRUE;
   tree->cell_margins[0] = 0;
   tree->cell_margins[1] = 0;
   tree->cell_margins[2] = 0;
   tree->cell_margins[3] = 0;
   tree->expander_size = 0;
   tree->headers_clip = NULL;
   tree->rows_widgets = NULL;
   tree->num_selected_rows = 0;

   tree->built = ETK_FALSE;
   tree->frozen = ETK_FALSE;
   tree->mode = ETK_TREE_MODE_LIST;
   tree->multiple_select = ETK_FALSE;
   tree->xoffset = 0;
   tree->yoffset = 0.0;

   tree->dnd_event = ETK_FALSE;
   
   ETK_WIDGET(tree)->size_allocate = _etk_tree_size_allocate;
   etk_widget_dnd_internal_set(ETK_WIDGET(tree), ETK_TRUE);   
   (ETK_WIDGET(tree))->drag = etk_drag_new(ETK_WIDGET(tree));   
   
   etk_signal_connect("realize", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_unrealize_cb), NULL);
   etk_signal_connect("focus", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_focus_cb), NULL);
   etk_signal_connect("unfocus", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_unfocus_cb), NULL);
   etk_signal_connect("key_down", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_key_down_cb), NULL);
   etk_signal_connect("drag_drop", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_drag_drop_cb), NULL);
   etk_signal_connect("drag_end", ETK_OBJECT((ETK_WIDGET(tree))->drag), ETK_CALLBACK(_etk_tree_drag_end_cb), NULL);
   
}

/* Destroys the tree */
static void _etk_tree_destructor(Etk_Tree *tree)
{
   Evas_List *l;

   if (!tree)
      return;
   
   etk_tree_clear(tree);

   for (l = tree->rows_widgets; l; l = l->next)
      _etk_tree_row_objects_free(l->data, tree);
   tree->rows_widgets = evas_list_free(tree->rows_widgets);
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
      case ETK_TREE_ROW_HEIGHT_PROPERTY:
         etk_tree_row_height_set(tree, etk_property_value_int_get(value));
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
      case ETK_TREE_MULTIPLE_SELECT_PROPERTY:
         etk_property_value_bool_set(value, tree->multiple_select);
         break;
      case ETK_TREE_HEADERS_VISIBLE_PROPERTY:
         etk_property_value_bool_set(value, tree->headers_visible);
         break;
      case ETK_TREE_ROW_HEIGHT_PROPERTY:
         etk_property_value_int_set(value, tree->row_height);
         break;
      default:
         break;
   }
}

/* Resizes the tree to the size allocation */
static void _etk_tree_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Tree *tree;
   int max_header_height = 0;
   Etk_Size child_request;
   int i;

   if (!(tree = ETK_TREE(widget)))
      return;
   
   evas_object_move(tree->headers_clip, geometry.x, geometry.y);
   evas_object_resize(tree->headers_clip, geometry.w, geometry.h);
   
   /* Allocate size for the scrolled view */
   if (tree->headers_visible)
   {
      for (i = 0; i < tree->num_cols; i++)
      {
         etk_widget_size_request(tree->columns[i]->header, &child_request);
         if (child_request.h > max_header_height)
            max_header_height = child_request.h;
      }
      geometry.y += max_header_height;
      geometry.h -= max_header_height;
   }
   etk_widget_size_allocate(tree->scrolled_view, geometry);
}

/**************************
 * Tree Col
 **************************/

/* Initializes the default values of the tree column */
static void _etk_tree_col_constructor(Etk_Tree_Col *tree_col)
{
   if (!tree_col)
      return;

   tree_col->tree = NULL;
   tree_col->model = NULL;
   tree_col->id = 0;
   tree_col->place = 0;
   tree_col->xoffset = 0;
   tree_col->requested_width = 0;
   tree_col->min_width = -1;
   tree_col->width = 0;
   tree_col->visible_width = 0;
   tree_col->visible = ETK_TRUE;
   tree_col->resizable = ETK_TRUE;
   tree_col->expand = ETK_FALSE;
   tree_col->header = NULL;
   tree_col->clip = NULL;
   tree_col->separator = NULL;
}

/* Destroys the tree column */
static void _etk_tree_col_destructor(Etk_Tree_Col *tree_col)
{
   if (!tree_col)
      return;
   etk_tree_model_free(tree_col->model);
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
      case ETK_TREE_COL_WIDTH_PROPERTY:
         etk_tree_col_width_set(tree_col, etk_property_value_int_get(value));
         break;
      case ETK_TREE_COL_MIN_WIDTH_PROPERTY:
         etk_tree_col_min_width_set(tree_col, etk_property_value_int_get(value));
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
      case ETK_TREE_COL_PLACE_PROPERTY:
         etk_tree_col_reorder(tree_col, etk_property_value_int_get(value));
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
      case ETK_TREE_COL_WIDTH_PROPERTY:
         etk_property_value_int_set(value, tree_col->requested_width);
         break;
      case ETK_TREE_COL_MIN_WIDTH_PROPERTY:
         etk_property_value_int_set(value, etk_tree_col_min_width_get(tree_col));
         break;
      case ETK_TREE_COL_VISIBLE_WIDTH_PROPERTY:
         etk_property_value_int_set(value, tree_col->visible_width);
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
      case ETK_TREE_COL_PLACE_PROPERTY:
         etk_property_value_int_set(value, tree_col->place);
         break;
      default:
         break;
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/**************************
 * Tree Grid
 **************************/

/* Called when the tree grid is realized */
static void _etk_tree_grid_realize_cb(Etk_Object *object, void *data)
{
   Etk_Tree *tree;
   Etk_Widget *grid;
   Evas *evas;
   int i;

   if (!(grid = ETK_WIDGET(object)))
      return;
   tree = ETK_TREE_GRID(grid)->tree;

   if (etk_widget_theme_object_data_get(grid, "separator_color", "%d %d %d %d", &tree->separator_color.r, &tree->separator_color.g, &tree->separator_color.b, &tree->separator_color.a) != 4)
   {
      tree->separator_color.r = 255;
      tree->separator_color.g = 255;
      tree->separator_color.b = 255;
      tree->separator_color.a = 0;
   }

   if (tree->use_default_row_height || tree->row_height < ETK_TREE_MIN_ROW_HEIGHT)
   {
      if (etk_widget_theme_object_data_get(grid, "row_height", "%d", &tree->row_height) != 1 || tree->row_height < ETK_TREE_MIN_ROW_HEIGHT)
         tree->row_height = ETK_TREE_MIN_ROW_HEIGHT;
      etk_range_increments_set(etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(tree->scrolled_view)), tree->row_height, 5 * tree->row_height);
   }
   
   if (etk_widget_theme_object_data_get(grid, "cell_margins", "%d %d %d %d", &tree->cell_margins[0], &tree->cell_margins[1], &tree->cell_margins[2], &tree->cell_margins[3]) != 4)
   {
      tree->cell_margins[0] = 0;
      tree->cell_margins[1] = 0;
      tree->cell_margins[2] = 0;
      tree->cell_margins[3] = 0;
   }
   
   if (etk_widget_theme_object_data_get(grid, "expander_size", "%d", &tree->expander_size) != 1)
      tree->expander_size = tree->row_height - 4;
   tree->expander_size = ETK_CLAMP(tree->expander_size, 1, tree->row_height);

   if ((evas = etk_widget_toplevel_evas_get(grid)))
   {
      ETK_TREE_GRID(grid)->clip = evas_object_rectangle_add(evas);
      evas_object_show(ETK_TREE_GRID(grid)->clip);
      etk_widget_member_object_add(grid, ETK_TREE_GRID(grid)->clip);
   }

   for (i = 0; i < tree->num_cols; i++)
      _etk_tree_col_realize(tree, i);
}

/**************************
 * Tree
 **************************/

/* Called when the tree is realized */
static void _etk_tree_realize_cb(Etk_Object *object, void *data)
{
   Etk_Tree *tree;
   Evas *evas;
   int i;
   
   if (!(tree = ETK_TREE(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(tree))))
      return;
   
   tree->headers_clip = evas_object_rectangle_add(evas);
   evas_object_show(tree->headers_clip);
   etk_widget_member_object_add(ETK_WIDGET(tree), tree->headers_clip);
   
   for (i = 0; i < tree->num_cols; i++)
      etk_widget_clip_set(tree->columns[i]->header, tree->headers_clip);
}

/* Called when the tree is unrealized */
static void _etk_tree_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Tree *tree;
   
   if (!(tree = ETK_TREE(object)))
      return;
   
   while (tree->rows_widgets)
   {
      _etk_tree_row_objects_free(tree->rows_widgets->data, tree);
      tree->rows_widgets = evas_list_remove_list(tree->rows_widgets, tree->rows_widgets);
   }
}

/* Called when an expander is clicked */
static void _etk_tree_expander_clicked_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree_Row_Objects *row_objects;
   Evas_Event_Mouse_Up *event;
   Evas_Coord x, y, w, h;

   if (!(row_objects = data) || !row_objects->row)
      return;

   event = event_info;
   evas_object_geometry_get(obj, &x, &y, &w, &h);
   if (x <= event->canvas.x && x + w >= event->canvas.x && y <= event->canvas.y && y + h >= event->canvas.y)
   {
      if (row_objects->row->expanded)
         etk_tree_row_collapse(row_objects->row);
      else
         etk_tree_row_expand(row_objects->row);
   }
}

/* Called when the row is pressed */
static void _etk_tree_row_pressed_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree_Row_Objects *row_objects;
   Evas_Event_Mouse_Down *evas_event;
   Etk_Event_Mouse_Up_Down event;

   if (!(row_objects = data) || !row_objects->row)
      return;
   evas_event = event_info;
   
   /* Double or triple click */
   if (!(evas_event->flags & EVAS_BUTTON_NONE) && (_etk_tree_last_clicked_row == row_objects->row))
   {
      event.button = evas_event->button;
      event.canvas.x = evas_event->canvas.x;
      event.canvas.y = evas_event->canvas.y;
      event.widget.x = evas_event->canvas.x - ETK_WIDGET(row_objects->row->tree)->inner_geometry.x;
      event.widget.y = evas_event->canvas.y - ETK_WIDGET(row_objects->row->tree)->inner_geometry.y;
      event.modifiers = evas_event->modifiers;
      event.locks = evas_event->locks;
      event.flags = evas_event->flags;
      event.timestamp = evas_event->timestamp;      
      
      if(!row_objects->row->tree->dnd_event)
	{
	   etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_CLICKED_SIGNAL], ETK_OBJECT(row_objects->row->tree), NULL, row_objects->row, &event);
	}
      
      /* We have to check this again because the user can remove the row on the "clicked" signal */
      if (!row_objects->row)
         return;
      
      if (!(evas_event->flags & EVAS_BUTTON_TRIPLE_CLICK) && (evas_event->button == 1) && !row_objects->row->tree->dnd_event)
	{
	   etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_ACTIVATED_SIGNAL], ETK_OBJECT(row_objects->row->tree), NULL, row_objects->row);
	}
   }
   
   /* We have to check this again because the user can remove the row on the "activated" signal */
   if (!row_objects->row)
      return;

   if (evas_event->button == 1)
      _etk_tree_row_select(row_objects->row->tree, row_objects->row, evas_event->modifiers);
}

/* Called when a row is clicked */
static void _etk_tree_row_clicked_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree_Row_Objects *row_objects;
   Evas_Event_Mouse_Up *evas_event;
   Etk_Event_Mouse_Up_Down event;
   Evas_Coord x, y, w, h;

   if (!(row_objects = data) || !row_objects->row)
      return;

   evas_event = event_info;
   evas_object_geometry_get(obj, &x, &y, &w, &h);
   if (((evas_event->flags & EVAS_BUTTON_NONE) || (row_objects->row != _etk_tree_last_clicked_row)) &&
      x <= evas_event->canvas.x && x + w >= evas_event->canvas.x &&
      y <= evas_event->canvas.y && y + h >= evas_event->canvas.y)
   {
      event.button = evas_event->button;
      event.canvas.x = evas_event->canvas.x;
      event.canvas.y = evas_event->canvas.y;
      event.widget.x = evas_event->canvas.x - ETK_WIDGET(row_objects->row->tree)->inner_geometry.x;
      event.widget.y = evas_event->canvas.y - ETK_WIDGET(row_objects->row->tree)->inner_geometry.y;
      event.modifiers = evas_event->modifiers;
      event.locks = evas_event->locks;
      event.flags = EVAS_BUTTON_NONE;
      event.timestamp = evas_event->timestamp;

      if (!evas_key_modifier_is_set(event.modifiers, "Control") &&
	  !evas_key_modifier_is_set(event.modifiers, "Shift") &&
	  row_objects->row->selected && evas_event->button == 1 &&
	  row_objects->row->tree->num_selected_rows > 1)
      {
	 etk_tree_unselect_all(row_objects->row->tree);
	 _etk_tree_row_select(row_objects->row->tree, row_objects->row, evas_event->modifiers);
      }
      
      if(row_objects->row->tree->dnd_event)      	 
	row_objects->row->tree->dnd_event = ETK_FALSE;
      else
	etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_CLICKED_SIGNAL], ETK_OBJECT(row_objects->row->tree), NULL, row_objects->row, &event);
      
      _etk_tree_last_clicked_row = row_objects->row;
   }
}

/* Called when the mouse moves over a row */
static void _etk_tree_row_mouse_move_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Event_Mouse_Move *ev;
   Etk_Tree_Row_Objects *row_objects;
   
   ev = event_info;
   if (!(row_objects = data) || !row_objects->row)
     return;   
   
   if(ev->buttons & 0x001 && !_etk_tree_drag_started)
   {
      Etk_Drag *drag;
      
      _etk_tree_drag_started = ETK_TRUE;
      drag = (ETK_WIDGET(row_objects->row->tree))->drag;      
      etk_drag_begin(drag);      
   }
}

/* Called when the tree is focused */
static void _etk_tree_focus_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Tree *tree;
   
   if (!(tree = ETK_TREE(object)))
      return;
   etk_widget_theme_object_signal_emit(tree->grid, "focus");
}

/* Called when the tree is unfocused */
static void _etk_tree_unfocus_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Tree *tree;
   
   if (!(tree = ETK_TREE(object)))
      return;
   etk_widget_theme_object_signal_emit(tree->grid, "unfocus");
}

/* Called when the user presses a key */
static void _etk_tree_key_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Key_Up_Down *key_event = event;
   Etk_Range *hscrollbar_range;
   Etk_Range *vscrollbar_range;
   Etk_Bool propagate = ETK_FALSE;
   Etk_Tree *tree;

   if (!(tree = ETK_TREE(object)))
      return;
   
   hscrollbar_range = etk_scrolled_view_hscrollbar_get(ETK_SCROLLED_VIEW(tree->scrolled_view));
   vscrollbar_range = etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(tree->scrolled_view));

   /* Select the previous visible row */
   if (strcmp(key_event->key, "Up") == 0)
   {
      Etk_Tree_Row *row_to_select;
      
      if (!tree->last_selected)
         row_to_select = etk_tree_last_row_get(tree, ETK_TRUE, ETK_FALSE);
      else
         row_to_select = etk_tree_prev_row_get(tree->last_selected, ETK_TRUE, ETK_FALSE);
      
      _etk_tree_row_select(tree, row_to_select, key_event->modifiers);
      etk_tree_row_scroll_to(row_to_select, ETK_FALSE);
   }
   /* Select the next visible row */
   else if (strcmp(key_event->key, "Down") == 0)
   {
      Etk_Tree_Row *row_to_select;
      
      if (!tree->last_selected)
         row_to_select = etk_tree_first_row_get(tree);
      else
         row_to_select = etk_tree_next_row_get(tree->last_selected, ETK_TRUE, ETK_FALSE);
      
      _etk_tree_row_select(tree, row_to_select, key_event->modifiers);
      etk_tree_row_scroll_to(row_to_select, ETK_FALSE);
   }
   else if (strcmp(key_event->key, "Right") == 0 && tree->last_selected && tree->last_selected->selected)
      etk_tree_row_expand(tree->last_selected);
   else if (strcmp(key_event->key, "Left") == 0 && tree->last_selected && tree->last_selected->selected)
      etk_tree_row_collapse(tree->last_selected);
   else if (strcmp(key_event->key, "Home") == 0)
      etk_range_value_set(vscrollbar_range, vscrollbar_range->lower);
   else if (strcmp(key_event->key, "End") == 0)
      etk_range_value_set(vscrollbar_range, vscrollbar_range->upper);
   else if (strcmp(key_event->key, "Next") == 0)
      etk_range_value_set(vscrollbar_range, vscrollbar_range->value + vscrollbar_range->page_increment);
   else if (strcmp(key_event->key, "Prior") == 0)
      etk_range_value_set(vscrollbar_range, vscrollbar_range->value - vscrollbar_range->page_increment);
   else if (strcmp(key_event->key, "space") == 0)
   {
      if (tree->last_selected)
         etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_ACTIVATED_SIGNAL], ETK_OBJECT(tree), NULL, tree->last_selected);
   }
   else
      propagate = ETK_TRUE;
   
   if (!propagate)
      etk_widget_event_propagation_stop();
}

/* Called when the user presses a column header */
static void _etk_tree_header_mouse_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Tree_Col *col;
   Etk_Event_Mouse_Up_Down *down_event;

   if (!(col = data) || !(down_event = event))
      return;

   col->tree->column_to_resize = etk_tree_col_to_resize_get(col, down_event->widget.x);
   if (col->tree->column_to_resize)
   {
      col->tree->col_to_resize_initial_width = col->tree->column_to_resize->visible_width;
      col->tree->col_to_resize_initial_x = down_event->canvas.x;
   }
}

/* Called when the user presses a column header */
static void _etk_tree_header_mouse_up_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Tree_Col *col;

   if (!(col = data))
      return;

   if (col->tree->column_to_resize)
      col->tree->column_to_resize = NULL;
   else
   {
      /* TODO: sort */
   }
}

/* Called when the mouse is moved above a column header */ 
static void _etk_tree_header_mouse_move_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Tree_Col *col;
   Etk_Event_Mouse_Move *move_event;
   int new_size;

   if (!(col = data) || !(move_event = event))
      return;

   if (col->tree->column_to_resize)
   {
      new_size = col->tree->col_to_resize_initial_width + move_event->cur.canvas.x - col->tree->col_to_resize_initial_x;
      if (col->tree->column_to_resize->place == 0)
         new_size -= col->tree->grid->left_inset;

      new_size = ETK_MAX(new_size, ETK_TREE_MIN_HEADER_WIDTH);
      if (new_size != col->tree->column_to_resize->requested_width)
      {
         col->tree->column_to_resize->requested_width = new_size;
         etk_object_notify(ETK_OBJECT(col->tree->column_to_resize), "width");
         /* TODO */
         etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(col->tree->grid), NULL);
         etk_widget_redraw_queue(ETK_WIDGET(col->tree->grid));
      }
   }
   else
   {
      Etk_Bool show_resize_pointer = ETK_FALSE;

      show_resize_pointer = (etk_tree_col_to_resize_get(col, move_event->cur.widget.x) != NULL);
      if (show_resize_pointer && !col->tree->resize_pointer_shown)
      {
         etk_toplevel_widget_pointer_push(ETK_WIDGET(col->tree)->toplevel_parent, ETK_POINTER_H_DOUBLE_ARROW);
         col->tree->resize_pointer_shown = ETK_TRUE;
      }
      else if (!show_resize_pointer && col->tree->resize_pointer_shown)
      {
         etk_toplevel_widget_pointer_pop(ETK_WIDGET(col->tree)->toplevel_parent, ETK_POINTER_H_DOUBLE_ARROW);
         col->tree->resize_pointer_shown = ETK_FALSE;
      }
   }
}

/* Called when the mouse enters a column header */ 
static void _etk_tree_header_mouse_in_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Mouse_In_Out *in_event;
   Etk_Tree_Col *col;
   Etk_Bool show_resize_pointer = ETK_FALSE;

   if (!(col = data) || !(in_event = event))
      return;

   show_resize_pointer = (etk_tree_col_to_resize_get(col, in_event->widget.x) != NULL);
   if (show_resize_pointer && !col->tree->resize_pointer_shown)
   {
      etk_toplevel_widget_pointer_push(ETK_WIDGET(col->tree)->toplevel_parent, ETK_POINTER_H_DOUBLE_ARROW);
      col->tree->resize_pointer_shown = ETK_TRUE;
   }
   else if (!show_resize_pointer && col->tree->resize_pointer_shown)
   {
      etk_toplevel_widget_pointer_pop(ETK_WIDGET(col->tree)->toplevel_parent, ETK_POINTER_H_DOUBLE_ARROW);
      col->tree->resize_pointer_shown = ETK_FALSE;
   }
}

/* Called when the mouse leaves a column header */ 
static void _etk_tree_header_mouse_out_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Tree_Col *col;

   if (!(col = data) || !col->tree->resize_pointer_shown)
      return;
   
   etk_toplevel_widget_pointer_pop(ETK_WIDGET(col->tree)->toplevel_parent, ETK_POINTER_H_DOUBLE_ARROW);
   col->tree->resize_pointer_shown = ETK_FALSE;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Updates the tree */
static void _etk_tree_update(Etk_Tree *tree)
{
   Evas_List *l;
   Etk_Tree_Row_Objects *row_objects;
   Etk_Tree_Col *first_visible_col, *last_visible_col;
   int first_visible_nth, delta;
   int x, y, w, h;
   int i, j;

   if (!tree || !tree->grid)
      return;
   
   x = tree->grid->inner_geometry.x;
   y = tree->grid->inner_geometry.y;
   w = tree->grid->inner_geometry.w;
   h = tree->grid->inner_geometry.h;
   
   first_visible_col = NULL;
   last_visible_col = NULL;
   for (i = 0; i < tree->num_cols; i++)
   {
      if (tree->columns[i]->visible)
      {
         if (!first_visible_col || first_visible_col->place > tree->columns[i]->place)
            first_visible_col = tree->columns[i];
         if (!last_visible_col || last_visible_col->place < tree->columns[i]->place)
            last_visible_col = tree->columns[i];
      }
   }
   if (!first_visible_col)
      return;

   /* Move, resize, and show or hide the clips and the column separators */
   for (i = 0; i < tree->num_cols; i++)
   {
      if ((tree->columns[i]->xoffset + tree->columns[i]->visible_width <= 0) || tree->columns[i]->xoffset > w || !tree->columns[i]->visible)
      {
         evas_object_hide(tree->columns[i]->clip);
         evas_object_hide(tree->columns[i]->separator);
      }
      else
      {
         evas_object_show(tree->columns[i]->clip);

         if (tree->columns[i]->xoffset + tree->columns[i]->visible_width > w && tree->columns[i]->xoffset < 0)
         {
            evas_object_move(tree->columns[i]->clip, x, y);
            evas_object_resize(tree->columns[i]->clip, w, h);
            evas_object_hide(tree->columns[i]->separator);
         }
         else if (tree->columns[i]->xoffset + tree->columns[i]->visible_width > w)
         {
            evas_object_move(tree->columns[i]->clip, x + tree->columns[i]->xoffset, y);
            evas_object_resize(tree->columns[i]->clip, w - tree->columns[i]->xoffset, h);
            evas_object_hide(tree->columns[i]->separator);
         }
         else if (tree->columns[i]->xoffset < 0)
         {
            evas_object_move(tree->columns[i]->clip, x, y);
            evas_object_resize(tree->columns[i]->clip, tree->columns[i]->visible_width + tree->columns[i]->xoffset, h);
            if (tree->columns[i] != last_visible_col)
            {
               evas_object_move(tree->columns[i]->separator, x + tree->columns[i]->visible_width + tree->columns[i]->xoffset, y);
               evas_object_resize(tree->columns[i]->separator, 1, h);
               evas_object_show(tree->columns[i]->separator);
            }
            else
               evas_object_hide(tree->columns[i]->separator);
         }
         else
         {
            evas_object_move(tree->columns[i]->clip, x + tree->columns[i]->xoffset, y);
            evas_object_resize(tree->columns[i]->clip, tree->columns[i]->visible_width, h);
            if (tree->columns[i] != last_visible_col)
            {
               evas_object_show(tree->columns[i]->separator);
               evas_object_move(tree->columns[i]->separator, x + tree->columns[i]->xoffset + tree->columns[i]->visible_width, y);
               evas_object_resize(tree->columns[i]->separator, 1, h);
            }
            else
               evas_object_hide(tree->columns[i]->separator);
         }
      }
   }

   /* Render the rows */
   first_visible_nth = tree->yoffset / tree->row_height;
   delta = tree->yoffset - (first_visible_nth * tree->row_height);
   
   l = tree->rows_widgets;
   _etk_tree_rows_draw(tree, &tree->root, &l, x, w, h, first_visible_col->xoffset, y - delta, first_visible_nth, (first_visible_nth % 2));

   /* Hide the remaining row objects */
   for (; l; l = l->next)
   {
      row_objects = l->data;
      evas_object_hide(row_objects->background);
      if (row_objects->expander)
         evas_object_hide(row_objects->expander);
      for (i = 0; i < tree->num_cols; i++)
      {
         for (j = 0; j < ETK_TREE_NUM_OBJECTS_PER_CELL; j++)
            evas_object_hide(row_objects->cells_objects[i].objects[j]);
      }
   }
}

/* Draws recursively a list of rows and their children */
static int _etk_tree_rows_draw(Etk_Tree *tree, Etk_Tree_Row *parent_row, Evas_List **items_objects,
   int x, int w, int h, int xoffset, int yoffset, int first_row_id, int first_row_color)
{
   Etk_Tree_Row *row;
   Etk_Tree_Row_Objects *row_objects;
   Etk_Tree_Col *first_visible_col;
   int i, j;
   int first_col_offset;
   Etk_Geometry geometry;

   if (!tree || !parent_row || !items_objects)
      return 0;

   first_visible_col = NULL;
   for (i = 0; i < tree->num_cols; i++)
   {
      if (tree->columns[i]->visible && (!first_visible_col || first_visible_col->place > tree->columns[i]->place))
         first_visible_col = tree->columns[i];
   }
   if (!first_visible_col)
      return 0;
   
   first_col_offset = xoffset;
   if (&tree->root != parent_row || parent_row->num_parent_children > 0)
      first_col_offset += tree->expander_size;
   
   i = 0;
   for (row = parent_row->first_child; row; row = row->next)
   {
      if (first_row_id <= 0)
      {
         int item_y;
   
         if (!(*items_objects) || !(row_objects = (*items_objects)->data))
            break;
         *items_objects = (*items_objects)->next;
   
         item_y = yoffset + i * tree->row_height;
         row->row_objects = row_objects;
         row_objects->row = row;
         
         evas_object_move(row_objects->background, x, item_y);
         evas_object_resize(row_objects->background, w, tree->row_height);
         evas_object_show(row_objects->background);
         
         if ((first_row_color + i) % 2 == 0)
            edje_object_signal_emit(row_objects->background, "odd" , "");
         else
            edje_object_signal_emit(row_objects->background, "even" , "");
         
         if (row->selected)
            edje_object_signal_emit(row_objects->background, "selected" , "");
         else
            edje_object_signal_emit(row_objects->background, "unselected" , "");
         
         if (row_objects->expander)
         {
            if (row->first_child)
            {
               if (evas_object_clip_get(row_objects->expander) != first_visible_col->clip)
               {
                  evas_object_clip_unset(row_objects->expander);
                  evas_object_clip_set(row_objects->expander, first_visible_col->clip);
               }
               evas_object_move(row_objects->expander, x + xoffset, item_y + (tree->row_height - tree->expander_size + 1) / 2);
               evas_object_resize(row_objects->expander, tree->expander_size, tree->expander_size);
               edje_object_signal_emit(row_objects->expander, row->expanded ? "expand" : "collapse", "");
               evas_object_show(row_objects->expander);
            }
            else
               evas_object_hide(row_objects->expander);
         }
   
         for (j = 0; j < tree->num_cols; j++)
         {
            if (!tree->columns[j] || !tree->columns[j]->visible)
               continue;
            
            geometry.x = x + ((tree->columns[j] == first_visible_col) ? first_col_offset : tree->columns[j]->xoffset) + tree->cell_margins[0];
            geometry.y = item_y + tree->cell_margins[2];
            geometry.w = tree->columns[j]->visible_width - tree->cell_margins[0] - tree->cell_margins[1];
            geometry.h = tree->row_height - tree->cell_margins[2] - tree->cell_margins[3];
            tree->columns[j]->model->render(tree->columns[j]->model, row, geometry, row->cells_data[j], row_objects->cells_objects[j].objects);
         }
         i++;
      }
      first_row_id--;

      if (row->first_child && row->expanded)
      {
         i += _etk_tree_rows_draw(tree, row, items_objects, x, w, h, first_col_offset, yoffset + i * tree->row_height, first_row_id, (first_row_color + i) % 2);
         first_row_id -= row->num_visible_children;
      }
   }

   return i;
}

/* Creates a new row and append to the list of the children of the row */
static Etk_Tree_Row *_etk_tree_row_new_valist(Etk_Tree *tree, Etk_Tree_Row *row, va_list args)
{
   Etk_Tree_Row *new_row;
   Etk_Tree_Row *r;
   int i;

   if (!tree || !tree->built || !row)
      return NULL;

   new_row = malloc(sizeof(Etk_Tree_Row));
   new_row->tree = tree;
   new_row->parent = row;
   new_row->first_child = NULL;
   new_row->last_child = NULL;
   new_row->num_visible_children = 0;
   new_row->num_parent_children = 0;
   new_row->expanded = ETK_FALSE;
   new_row->selected = ETK_FALSE;
   new_row->data = NULL;
   new_row->data_free_cb = NULL;
   new_row->row_objects = NULL;

   new_row->cells_data = malloc(sizeof(void *) * tree->num_cols);
   for (i = 0; i < tree->num_cols; i++)
   {
      new_row->cells_data[i] = calloc(1, tree->columns[i]->model->cell_data_size);
      if (tree->columns[i]->model->cell_data_init)
         tree->columns[i]->model->cell_data_init(tree->columns[i]->model, new_row->cells_data[i]);
   }
   _etk_tree_row_fields_set_valist_full(new_row, args, ETK_FALSE);

   for (r = new_row->parent; r; r = r->parent)
   {
      r->num_visible_children++;
      if (!r->expanded)
         break;
   }
   if ((r = new_row->parent) && (r = r->parent))
      r->num_parent_children++;

   new_row->prev = row->last_child;
   new_row->next = ETK_FALSE;
   if (!row->first_child)
      row->first_child = new_row;
   if (row->last_child)
      row->last_child->next = new_row;
   row->last_child = new_row;
   
   if (!tree->frozen)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(tree->grid), NULL);
      etk_widget_redraw_queue(ETK_WIDGET(tree->grid));
   }
   return new_row;
}

/* Sets the different values of the cells of the row */
static void _etk_tree_row_fields_set_valist_full(Etk_Tree_Row *row, va_list args, Etk_Bool emit_value_changed_signal)
{
   Etk_Tree_Col *col;
   va_list args2;
   
   if (!row)
      return;

   va_copy(args2, args);
   while ((col = va_arg(args2, Etk_Tree_Col *)))
   {
      if (col->model->cell_data_set)
      {
         col->model->cell_data_set(col->model, row->cells_data[col->id], &args2);
         if (emit_value_changed_signal)
            etk_signal_emit(_etk_tree_col_signals[ETK_TREE_COL_CELL_VALUE_CHANGED], ETK_OBJECT(col), NULL, row);
      }
   }
   va_end(args2);

   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree->grid));
}

/* Frees a row */
static void _etk_tree_row_free(Etk_Tree_Row *row)
{
   Etk_Tree_Row *r;

   if (!row)
      return;

   if (row->tree && row->cells_data)
   {
      int i;

      for (i = 0; i < row->tree->num_cols; i++)
      {
         if (row->tree->columns[i]->model->cell_data_free)
            row->tree->columns[i]->model->cell_data_free(row->tree->columns[i]->model, row->cells_data[i]);
         free(row->cells_data[i]);
      }
      free(row->cells_data);
   }
   if (row->row_objects)
      ((Etk_Tree_Row_Objects *)row->row_objects)->row = NULL;
   
   if (row->data && row->data_free_cb)
      row->data_free_cb(row->data);
   
   if (row->parent)
   {
      if (row->parent->first_child == row)
         row->parent->first_child = row->next;
      if (row->parent->last_child == row)
         row->parent->last_child = row->prev;
      if (row->prev)
         row->prev->next = row->next;
      if (row->next)
         row->next->prev = row->prev;
   }

   while (row->first_child)
      _etk_tree_row_free(row->first_child);

   for (r = row->parent; r; r = r->parent)
   {
      r->num_visible_children--;
      if (!r->expanded)
         break;
   }
   if ((r = row->parent) && (r = r->parent))
      r->num_parent_children--;
   
   free(row);
}

/* Creates the evas objects needed by a row */ 
static Etk_Tree_Row_Objects *_etk_tree_row_objects_new(Etk_Tree *tree)
{
   Etk_Tree_Row_Objects *new_row_objects;
   Evas *evas;
   int i, j;

   if (!tree || !tree->built || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(tree))))
      return NULL;

   new_row_objects = malloc(sizeof(Etk_Tree_Row_Objects));
   new_row_objects->row = NULL;
   
   /* Creates the background object of the row */
   /* TODO: use etk_theme */
   new_row_objects->background = edje_object_add(evas);
   edje_object_file_set(new_row_objects->background, ETK_WIDGET(tree)->theme_file, "tree_row");
   evas_object_repeat_events_set(new_row_objects->background, 1);
   evas_object_clip_set(new_row_objects->background, ETK_TREE_GRID(tree->grid)->clip);
   evas_object_event_callback_add(new_row_objects->background, EVAS_CALLBACK_MOUSE_DOWN, _etk_tree_row_pressed_cb, new_row_objects);
   evas_object_event_callback_add(new_row_objects->background, EVAS_CALLBACK_MOUSE_UP, _etk_tree_row_clicked_cb, new_row_objects);
   evas_object_event_callback_add(new_row_objects->background, EVAS_CALLBACK_MOUSE_MOVE, _etk_tree_row_mouse_move_cb, new_row_objects);
   etk_widget_member_object_add(tree->grid, new_row_objects->background);
   
   /* Creates the expander of the row */
   if (tree->mode == ETK_TREE_MODE_TREE)
   {
      /* TODO: use etk_theme */
      new_row_objects->expander = edje_object_add(evas);
      edje_object_file_set(new_row_objects->expander, ETK_WIDGET(tree)->theme_file, "tree_expander");
      evas_object_event_callback_add(new_row_objects->expander, EVAS_CALLBACK_MOUSE_UP, _etk_tree_expander_clicked_cb, new_row_objects);
      etk_widget_member_object_add(tree->grid, new_row_objects->expander);
   }
   else
      new_row_objects->expander = NULL;
   
   /* Creates the objects of each cells of the row */ 
   new_row_objects->cells_objects = calloc(1, sizeof(Etk_Tree_Cell_Objects) * tree->num_cols);
   for (i = 0; i < tree->num_cols; i++)
   {
      if (tree->columns[i]->model->objects_create)
      {
         tree->columns[i]->model->objects_create(tree->columns[i]->model, new_row_objects->cells_objects[i].objects, evas);
         for (j = 0; j < ETK_TREE_NUM_OBJECTS_PER_CELL; j++)
         {
            etk_widget_member_object_add(ETK_WIDGET(tree->grid), new_row_objects->cells_objects[i].objects[j]);
            evas_object_clip_set(new_row_objects->cells_objects[i].objects[j], tree->columns[i]->clip);
         }
      }
      etk_widget_member_object_raise(tree->grid, tree->columns[i]->separator);
   }

   return new_row_objects;
}

/* Frees all the objects of a row */
static void _etk_tree_row_objects_free(Etk_Tree_Row_Objects *row_objects, Etk_Tree *tree)
{
   int i, j;

   if (!row_objects || !tree || !tree->grid)
      return;

   if (row_objects->background)
   {
      etk_widget_member_object_del(tree->grid, row_objects->background);
      evas_object_del(row_objects->background);
   }
   if (row_objects->expander)
   {
      etk_widget_member_object_del(tree->grid, row_objects->expander);
      evas_object_del(row_objects->expander);
   }

   for (i = 0; i < tree->num_cols; i++)
   {
      for (j = 0; j < ETK_TREE_NUM_OBJECTS_PER_CELL; j++)
      {
         etk_widget_member_object_del(tree->grid, row_objects->cells_objects[i].objects[j]);
         evas_object_del(row_objects->cells_objects[i].objects[j]);
      }
   }
   free(row_objects->cells_objects);
   free(row_objects);
}

/* Creates the evas_objects of the "col_nth" column */
static void _etk_tree_col_realize(Etk_Tree *tree, int col_nth)
{
   Evas *evas;

   if (!tree || !tree->grid || col_nth < 0 || col_nth >= tree->num_cols || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(tree))))
      return;

   tree->columns[col_nth]->clip = evas_object_rectangle_add(evas);
   etk_widget_member_object_add(tree->grid, tree->columns[col_nth]->clip);

   tree->columns[col_nth]->separator = evas_object_rectangle_add(evas);
   evas_object_color_set(tree->columns[col_nth]->separator, tree->separator_color.r, tree->separator_color.g, tree->separator_color.b, tree->separator_color.a);
   evas_object_pass_events_set(tree->columns[col_nth]->separator, 1);
   etk_widget_member_object_add(tree->grid, tree->columns[col_nth]->separator);
}

/* Returns the col to resize according to the x position of the mouse in the column "col" */
static Etk_Tree_Col *etk_tree_col_to_resize_get(Etk_Tree_Col *col, int x)
{
   int i;

   if (!col)
      return NULL;

   if ((x + col->header->left_inset <= 3) && col->place >= 1)
   {
      for (i = 0; i < col->tree->num_cols; i++)
      {
         if (col->tree->columns[i]->place == col->place - 1)
         {
            if (col->tree->columns[i]->resizable)
               return col->tree->columns[i];
            else
               return NULL;
         }
      }
   }
   else if ((col->header->geometry.w - (x + col->header->left_inset) <= 3) && col->resizable)
      return col;

   return NULL;
}

/* Gets the selected child rows of the row */
static void _etk_tree_row_selected_rows_get(Etk_Tree_Row *row, Evas_List **selected_rows)
{
   Etk_Tree_Row *r;
   
   if (!row || !selected_rows)
      return;
   
   for (r = row->first_child; r; r = r->next)
   {
      if (r->selected)
         *selected_rows = evas_list_append(*selected_rows, r);
      _etk_tree_row_selected_rows_get(r, selected_rows);
   }
}

/* Selects all the child rows of the row */
static void _etk_tree_row_select_all(Etk_Tree_Row *row)
{
   Etk_Tree_Row *r;
   
   if (!row)
      return;
   
   for (r = row->first_child; r; r = r->next)
   {
      r->selected = ETK_TRUE;
      if (r->expanded)
         _etk_tree_row_select_all(r);
   }
}

/* Unselects all the child rows of the row */
static void _etk_tree_row_unselect_all(Etk_Tree_Row *row)
{
   Etk_Tree_Row *r;
   
   if (!row)
      return;

   for (r = row->first_child; r; r = r->next)
   {
      r->selected = ETK_FALSE;
      _etk_tree_row_unselect_all(r);
   }
}

/* Selects rows in the tree according to the keyboard modifiers and the clicked row */
static void _etk_tree_row_select(Etk_Tree *tree, Etk_Tree_Row *row, Evas_Modifier *modifiers)
{
   if (!tree || !row)
      return;
  
   if (!tree->multiple_select || !modifiers)
   {
      tree->num_selected_rows = 1;
      etk_tree_unselect_all(tree);
      row->selected = ETK_TRUE;
      tree->last_selected = row;
      if(tree->dnd_event)      	
	etk_widget_theme_object_signal_emit(ETK_WIDGET(tree), "row_selected");
      else
	etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_SELECTED_SIGNAL], ETK_OBJECT(tree), NULL, row);
   }
   else
   {
      if (evas_key_modifier_is_set(modifiers, "Shift"))
      {
         if (!evas_key_modifier_is_set(modifiers, "Control"))
	 {
            etk_tree_unselect_all(tree);
	    tree->num_selected_rows = 1;
	 }
   
         if (!tree->last_selected)
	 {
            row->selected = ETK_TRUE;
	    ++tree->num_selected_rows;
	 }
         else
         {
            Etk_Bool selected = ETK_FALSE;
            Etk_Tree_Row *r;

            for (r = tree->root.first_child; r; r = etk_tree_next_row_get(r, ETK_TRUE, ETK_FALSE))
            {
               if (r == tree->last_selected || r == row)
               {
                  r->selected = ETK_TRUE;
		  ++tree->num_selected_rows;
                  selected = !selected;
               }
               else
	       {
                  r->selected |= selected;
		  if(!r->selected)
		    ++tree->num_selected_rows;
		  else
		    --tree->num_selected_rows;
	       }
            }
            if (selected)
            {
               etk_tree_unselect_all(tree);
               row->selected = ETK_TRUE;
	       tree->num_selected_rows = 1;
            }
         }
         tree->last_selected = row;
	 
	 if(tree->dnd_event)
	   etk_widget_theme_object_signal_emit(ETK_WIDGET(tree), "row_selected");
	 else
	   etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_SELECTED_SIGNAL], ETK_OBJECT(tree), NULL, row);
      }
      else if (evas_key_modifier_is_set(modifiers, "Control"))
      {
         if (row->selected)
         {
            row->selected = ETK_FALSE;
	    --tree->num_selected_rows;
            tree->last_selected = row;
	    if(tree->dnd_event)      
	      etk_widget_theme_object_signal_emit(ETK_WIDGET(tree), "row_selected");
	    else
	      etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_SELECTED_SIGNAL], ETK_OBJECT(tree), NULL, row);
         }
         else
         {
            row->selected = ETK_TRUE;
	    ++tree->num_selected_rows;
            tree->last_selected = row;
	    if(tree->dnd_event)      
	      etk_widget_theme_object_signal_emit(ETK_WIDGET(tree), "row_selected");
	    else
	      etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_SELECTED_SIGNAL], ETK_OBJECT(tree), NULL, row);
         }
      }
      else
      {
	 if(!row->selected)
	 {
	    etk_tree_unselect_all(tree);
	    tree->num_selected_rows = 1;
	    if(tree->dnd_event)      
	      etk_widget_theme_object_signal_emit(ETK_WIDGET(tree), "row_selected");
	    else
	      etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_SELECTED_SIGNAL], ETK_OBJECT(tree), NULL, row);	    
	 }
	 
         row->selected = ETK_TRUE;
         tree->last_selected = row;
      }
   }

   etk_widget_redraw_queue(ETK_WIDGET(tree->grid));
}

/* Restore the heap properties of the heap. Used to sort the tree */
static void _etk_tree_heapify(Etk_Tree *tree, Etk_Tree_Row **heap, int root, int size, int (*compare_cb)(Etk_Tree *tree, Etk_Tree_Row *row1, Etk_Tree_Row *row2, Etk_Tree_Col *col, void *data), int asc, Etk_Tree_Col *col, void *data)
{
   Etk_Tree_Row *tmp;
   int left, right, max;
   
   if (!heap)
      return;
   
   left = (root * 2) + 1;
   right = (root * 2) + 2;
   
   max = root;
   if (left < size && (compare_cb(tree, heap[left], heap[max], col, data) * asc) > 0)
      max = left;
   if (right < size && (compare_cb(tree, heap[right], heap[max], col, data) * asc) > 0)
      max = right;
   
   if (max != root)
   {
      tmp = heap[max];
      heap[max] = heap[root];
      heap[root] = tmp;
      _etk_tree_heapify(tree, heap, max, size, compare_cb, asc, col, data);
   }
}

static void _etk_tree_drag_drop_cb(Etk_Object *object, void *event, void *data)
{
#if HAVE_ECORE_X   
   Etk_Event_Selection_Request *ev;
   Etk_Widget *win;
      
   win = etk_widget_toplevel_parent_get(ETK_WIDGET(object));
   if(ETK_IS_WINDOW(win))
   {
      Etk_Tree *tree;
      
      tree = ETK_TREE(object);
      tree->dnd_event = ETK_TRUE;
            
      evas_event_feed_mouse_down(etk_toplevel_widget_evas_get(win), 1,
				 EVAS_BUTTON_NONE,
				 ecore_x_current_time_get(),
				 NULL);
      
      evas_event_feed_mouse_up(etk_toplevel_widget_evas_get(win), 1,
			       EVAS_BUTTON_NONE,
			       ecore_x_current_time_get(),
			       NULL);      
   }      
#endif   
}
  
static void _etk_tree_drag_end_cb(Etk_Object *object, void *data)
{
   _etk_tree_drag_started = ETK_FALSE;
}
