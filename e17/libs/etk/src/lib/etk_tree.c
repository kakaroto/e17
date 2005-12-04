/** @file etk_tree.c */
#include "etk_tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Edje.h>
#include "etk_tree_model.h"
#include "etk_scrolled_view.h"
#include "etk_button.h"
#include "etk_toplevel_widget.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Tree
 * @{
 */

#define ETK_TREE_MIN_HEADER_WIDTH 10
#define ETK_TREE_NUM_OBJECTS_PER_CELL 5

#define ETK_TREE_GRID_TYPE       (_etk_tree_grid_type_get())
#define ETK_TREE_GRID(obj)       (ETK_OBJECT_CAST((obj), ETK_TREE_GRID_TYPE, Etk_Tree_Grid))
#define ETK_IS_TREE_GRID(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TREE_GRID_TYPE))

typedef struct _Etk_Tree_Grid
{
   /* Inherit form Etk_Container */
   Etk_Container container;

   Etk_Tree *tree;
} Etk_Tree_Grid;

typedef struct _Etk_Tree_Cell_Objects
{
   Evas_Object *objects[ETK_TREE_NUM_OBJECTS_PER_CELL];
} Etk_Tree_Cell_Objects;

typedef struct _Etk_Tree_Row_Objects
{
   Etk_Widget *expander;
   Etk_Widget *background;
   Etk_Tree_Cell_Objects *cells_objects;
} Etk_Tree_Row_Objects;

enum _Etk_Tree_Signal_Id
{
   ETK_TREE_ROW_SELECTED_SIGNAL,
   ETK_TREE_ROW_UNSELECTED_SIGNAL,
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
   ETK_TREE_HEADERS_VISIBLE_PROPERTY
};

enum _Etk_Tree_Col_Property_Id
{
   ETK_TREE_COL_TITLE_PROPERTY,
   ETK_TREE_COL_WIDTH_PROPERTY,
   ETK_TREE_COL_MIN_WIDTH_PROPERTY,
   ETK_TREE_COL_VISIBLE_WIDTH_PROPERTY,
   ETK_TREE_COL_VISIBLE_PROPERTY,
   ETK_TREE_COL_RESIZABLE_PROPERTY,
   ETK_TREE_COL_PLACE_PROPERTY
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

static void _etk_tree_col_constructor(Etk_Tree_Col *tree_col);
static void _etk_tree_col_destructor(Etk_Tree_Col *tree_col);
static void _etk_tree_col_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_tree_col_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

static void _etk_tree_expander_clicked_cb(Etk_Object *object, void *event_info, void *data);
static void _etk_tree_row_pressed_cb(Etk_Object *object, void *event_info, void *data);
static void _etk_tree_focus_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_unfocus_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_down_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_up_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_move_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_in_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_out_cb(Etk_Object *object, void *event, void *data);

static void _etk_tree_update(Etk_Tree *tree);
static int _etk_tree_rows_draw(Etk_Tree *tree, Etk_Tree_Node *node, Evas_List **items_objects,
   int x, int w, int h, int xoffset, int yoffset, int first_row_id, int first_row_color);
static Etk_Tree_Row *_etk_tree_row_new_valist(Etk_Tree *tree, Etk_Tree_Node *node, va_list args);
static Etk_Tree_Row_Objects *_etk_tree_row_objects_new(Etk_Tree *tree);
static void _etk_tree_row_objects_free(Etk_Tree_Row_Objects *row_objects, Etk_Tree *tree);
static void _etk_tree_row_free(Etk_Tree_Row *row);
static void _etk_tree_col_realize(Etk_Tree *tree, int col_nth);
static Etk_Tree_Col *etk_tree_col_to_resize_get(Etk_Tree_Col *col, int x);

static void _etk_tree_node_select(Etk_Tree *tree, Etk_Tree_Node *node, Evas_Modifier *modifiers);

static Etk_Signal *_etk_tree_signals[ETK_TREE_NUM_SIGNALS];

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
      _etk_tree_signals[ETK_TREE_ROW_EXPANDED_SIGNAL] = etk_signal_new("row_expaned", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_ROW_COLLAPSED_SIGNAL] = etk_signal_new("row_collapsed", tree_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree_signals[ETK_TREE_SELECT_ALL_SIGNAL] = etk_signal_new("select_all", tree_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_tree_signals[ETK_TREE_UNSELECT_ALL_SIGNAL] = etk_signal_new("unselect_all", tree_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);

      etk_type_property_add(tree_type, "mode", ETK_TREE_MODE_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(ETK_TREE_MODE_LIST));
      etk_type_property_add(tree_type, "multiple_select", ETK_TREE_MULTIPLE_SELECT_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(TRUE));
      etk_type_property_add(tree_type, "headers_visible", ETK_TREE_HEADERS_VISIBLE_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(TRUE));
   
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
   return etk_widget_new(ETK_TREE_TYPE, "focusable", TRUE, "focus_on_press", TRUE, NULL);
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

      etk_type_property_add(tree_col_type, "title",         ETK_TREE_COL_TITLE_PROPERTY,           ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_string(NULL));
      etk_type_property_add(tree_col_type, "width",         ETK_TREE_COL_WIDTH_PROPERTY,           ETK_PROPERTY_INT,    ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(ETK_TREE_MIN_HEADER_WIDTH));
      etk_type_property_add(tree_col_type, "min_width",     ETK_TREE_COL_MIN_WIDTH_PROPERTY,       ETK_PROPERTY_INT,    ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(-1));
      etk_type_property_add(tree_col_type, "visible_width", ETK_TREE_COL_VISIBLE_WIDTH_PROPERTY,   ETK_PROPERTY_INT,    ETK_PROPERTY_READABLE,           NULL);
      etk_type_property_add(tree_col_type, "visible",       ETK_TREE_COL_VISIBLE_PROPERTY,         ETK_PROPERTY_BOOL,   ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(TRUE));
      etk_type_property_add(tree_col_type, "resizable",     ETK_TREE_COL_RESIZABLE_PROPERTY,       ETK_PROPERTY_BOOL,   ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(TRUE));
      etk_type_property_add(tree_col_type, "place",         ETK_TREE_COL_PLACE_PROPERTY,           ETK_PROPERTY_INT,    ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(0));
      
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

   if (!tree)
      return NULL;

   tree->columns = realloc(tree->columns, sizeof(Etk_Tree_Col *) * (tree->num_cols + 1));
   new_col = ETK_TREE_COL(etk_object_new(ETK_TREE_COL_TYPE, "title", title, "width", width, "visible", TRUE, "resizable", TRUE, NULL));
   tree->columns[tree->num_cols] = new_col;

   new_col->id = tree->num_cols;
   new_col->place = new_col->id;
   etk_object_notify(ETK_OBJECT(new_col), "place");
   new_col->tree = tree;
   new_col->model = model;

   /* Create the header widget */
   new_header = etk_widget_new(ETK_BUTTON_TYPE, "theme_group", "tree_header", "label", title, "xalign", 0.0, "repeat_events", TRUE, "visibility_locked", TRUE, NULL);
   etk_signal_connect("mouse_down", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_down_cb), new_col);
   etk_signal_connect("mouse_up", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_up_cb), new_col);
   etk_signal_connect("mouse_move", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_move_cb), new_col);
   etk_signal_connect("mouse_in", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_in_cb), new_col);
   etk_signal_connect("mouse_out", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_out_cb), new_col);
   etk_widget_parent_set(new_header, ETK_CONTAINER(tree));
   etk_widget_show(new_header);
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
 * @param headers_visible TRUE if the column headers should be displayed
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
 * @return Returns TRUE whether the column headers are visible
 */
Etk_Bool etk_tree_headers_visible_get(Etk_Tree *tree)
{
   if (!tree)
      return FALSE;
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
      etk_widget_redraw_queue(ETK_WIDGET(col->tree));
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
   if (!col || !col->header)
      return;
   etk_widget_size_request_set(col->header, min_width, -1);
   etk_object_notify(ETK_OBJECT(col), "min_width");
}

/**
 * @brief Gets the minimum width of the column
 * @param col a tree column
 * @return Returns the minimum width of the column
 */
int etk_tree_col_min_width_get(Etk_Tree_Col *col)
{
   Etk_Size size;

   if (!col || !col->header)
      return 0;
   etk_widget_size_request(col->header, &size);
   return size.w;
}

/**
 * @brief Sets whether the column can be resized by the user
 * @param col a tree column
 * @param resizable TRUE whether the column should be resizable
 */
void etk_tree_col_resizable_set(Etk_Tree_Col *col, Etk_Bool resizable)
{
   if (!col)
      return;
   col->resizable = resizable;
   etk_object_notify(ETK_OBJECT(col), "resizable");
   if (col->tree)
      etk_widget_redraw_queue(ETK_WIDGET(col->tree));
}

/**
 * @brief Gets whether the column can be resized by the user
 * @param col a tree column
 * @return Returns TRUE if the column is resizable
 */
Etk_Bool etk_tree_col_resizable_get(Etk_Tree_Col *col)
{
   if (!col)
      return FALSE;
   return col->resizable;
}

/**
 * @brief Sets whether the column is visible
 * @param col a tree column
 * @param visible TRUE whether the column should be visible
 */
void etk_tree_col_visible_set(Etk_Tree_Col *col, Etk_Bool visible)
{
   if (!col)
      return;
   col->visible = visible;
   etk_object_notify(ETK_OBJECT(col), "visible");
   if (col->tree)
      etk_widget_redraw_queue(ETK_WIDGET(col->tree));
}

/**
 * @brief Gets whether the column is visible
 * @param col a tree column
 * @return Returns TRUE if the column is visible
 */
Etk_Bool etk_tree_col_visible_get(Etk_Tree_Col *col)
{
   if (!col)
      return FALSE;
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
            col->tree->columns[i]->place++;
      }
   }
   else if (new_place > col->place)
   {
      for (i = 0; i < col->tree->num_cols; i++)
      {
         if (col->tree->columns[i]->place > col->place && col->tree->columns[i]->place <= new_place)
            col->tree->columns[i]->place--;
      }
   }
   col->place = new_place;
   etk_object_notify(ETK_OBJECT(col), "place");
   etk_widget_redraw_queue(ETK_WIDGET(col->tree));
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
   if (mode != ETK_TREE_MODE_LIST)
      etk_tree_multiple_select_set(tree, FALSE);
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
   tree->built = TRUE;
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
   tree->frozen = TRUE;
}

/**
 * @brief Thaws the tree: it will update the tree if it was frozen
 * @param tree a tree
 */
void etk_tree_thaw(Etk_Tree *tree)
{
   if (!tree || !tree->frozen)
      return;
   
   etk_widget_redraw_queue(ETK_WIDGET(tree));
   tree->frozen = FALSE;
}

/**
 * @brief Sets whether several rows can be selected in the tree. The tree has to be in the ETK_TREE_MODE_LIST mode 
 * @param tree a tree
 * @param multiple_select TRUE if several rows can be selected
 */
void etk_tree_multiple_select_set(Etk_Tree *tree, Etk_Bool multiple_select)
{
   if (!tree || (tree->mode != ETK_TREE_MODE_LIST))
      return;

   if (!multiple_select)
      etk_tree_unselect_all(tree);
   tree->multiple_select = multiple_select;
   etk_object_notify(ETK_OBJECT(tree), "multiple_select");
}

/**
 * @brief Gets whether several rows can be selected in the tree.
 * @param tree a tree
 * @return TRUE if several rows can be selected
 */
Etk_Bool etk_tree_multiple_select_get(Etk_Tree *tree)
{
   if (!tree)
      return FALSE;
   return tree->multiple_select;
}

/**
 * @brief Selects all the node of the tree. Multiple selection has to be enabled (with etk_tree_multiple_select_set()), @n
 * so it will only work with a tree in the ETK_TREE_MODE_LIST mode.
 * @param tree a tree
 */
void etk_tree_select_all(Etk_Tree *tree)
{
   Evas_List *l;
   Etk_Tree_Row *row;

   if (!tree || (tree->mode != ETK_TREE_MODE_LIST) || !tree->multiple_select)
      return;

   for (l = tree->root.child_rows; l; l = l->next)
   {
      row = l->data;
      row->node.selected = TRUE;
   }
   etk_signal_emit(_etk_tree_signals[ETK_TREE_SELECT_ALL_SIGNAL], ETK_OBJECT(tree), NULL);
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Unselects all the node of the tree
 * @param tree a tree
 */
void etk_tree_unselect_all(Etk_Tree *tree)
{
   if (!tree)
      return;

   if (tree->last_selected)
   {
      tree->last_selected->selected = FALSE;
      etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_UNSELECTED_SIGNAL], ETK_OBJECT(tree), NULL, tree->last_selected->row);
   }
   if (tree->mode == ETK_TREE_MODE_LIST && tree->multiple_select)
   {
      Evas_List *l;
      Etk_Tree_Row *row;

      for (l = tree->root.child_rows; l; l = l->next)
      {
         row = l->data;
         row->node.selected = FALSE;
      }
      etk_signal_emit(_etk_tree_signals[ETK_TREE_UNSELECT_ALL_SIGNAL], ETK_OBJECT(tree), NULL);
   }
   etk_widget_redraw_queue(ETK_WIDGET(tree));
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
   new_row = _etk_tree_row_new_valist(row->tree, &row->node, args);
   va_end(args);

   return new_row;
}

/**
 * @brief Removes a row from the tree
 * @param row the the row to remove
 */
void etk_tree_row_del(Etk_Tree_Row *row)
{
   Evas_List *l;
   Etk_Tree_Node *n;

   if (!row || !(l = evas_list_find_list(row->node.parent->child_rows, row)))
      return;

   for (n = row->tree->last_selected; n; n = n->parent)
   {
      if (&row->node == n)
      {
         row->tree->last_selected = NULL;
         break;
      }
   }
   
   _etk_tree_row_free(l->data);
   row->node.parent->child_rows = evas_list_remove_list(row->node.parent->child_rows, l);
   
   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
}

/**
 * @brief Removes all the rows of the tree 
 * @param tree a tree 
 */
void etk_tree_clear(Etk_Tree *tree)
{
   if (!tree)
      return;

   while (tree->root.child_rows)
   {
      _etk_tree_row_free(tree->root.child_rows->data);
      tree->root.child_rows = evas_list_remove_list(tree->root.child_rows, tree->root.child_rows);
   }
   tree->last_selected = NULL;
   
   if (!tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(tree));
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
   Etk_Tree_Col *col;
   va_list args2;
   
   if (!row)
      return;

   va_copy(args2, args);
   while ((col = va_arg(args2, Etk_Tree_Col *)))
   {
      if (col->model->cell_data_set)
         col->model->cell_data_set(col->model, row->cells_data[col->id], &args2);
   }
   va_end(args2);

   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
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
   if (!row)
      return;
   row->data = data;
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
 * @brief Selects the row
 * @param row the row to select
 */
void etk_tree_row_select(Etk_Tree_Row *row)
{
   if (!row)
      return;
   _etk_tree_node_select(row->tree, &row->node, NULL);
}

/**
 * @brief Unselects the row
 * @param row the row to unselect
 */
void etk_tree_row_unselect(Etk_Tree_Row *row)
{
   if (!row || !row->node.selected)
      return;
   
   row->node.selected = FALSE;
   etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_UNSELECTED_SIGNAL], ETK_OBJECT(row->tree), NULL, row);
   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
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
   return tree->last_selected->row;
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

   if (tree->mode == ETK_TREE_MODE_TREE || !tree->multiple_select)
   {
      if (!tree->last_selected || !tree->last_selected->selected)
         return NULL;
      
      selected_rows = evas_list_append(selected_rows, tree->last_selected);
   }
   else
   {
      Evas_List *l;
      Etk_Tree_Row *row;

      for (l = tree->root.child_rows; l; l = l->next)
      {
         row = l->data;
         if (row->node.selected)
            selected_rows = evas_list_append(selected_rows, row);
      }
   }

   return selected_rows;
}

/**
 * @brief Expands the row. The child rows of the row will be displayed. It will only take effect with a tree in the ETK_TREE_MODE_TREE mode
 * @param row a row
 */
void etk_tree_row_expand(Etk_Tree_Row *row)
{
   Etk_Tree_Node *n;

   if (!row || row->node.expanded || !row->tree || (row->tree->mode != ETK_TREE_MODE_TREE))
      return;

   row->node.expanded = TRUE;
   for (n = row->node.parent; n && n->expanded; n = n->parent)
      n->num_visible_children += row->node.num_visible_children;

   etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_EXPANDED_SIGNAL], ETK_OBJECT(row->tree), NULL, row);
   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
}

/**
 * @brief Collapses the row. The child rows of the row are no longer displayed. It will only take effect with a tree in the ETK_TREE_MODE_TREE mode
 * @param row a row
 */
void etk_tree_row_collapse(Etk_Tree_Row *row)
{
   Etk_Tree_Node *n;

   if (!row || !row->node.expanded || !row->tree || (row->tree->mode != ETK_TREE_MODE_TREE))
      return;

   row->node.expanded = FALSE;
   for (n = row->node.parent; n && n->expanded; n = n->parent)
      n->num_visible_children -= row->node.num_visible_children;

   etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_COLLAPSED_SIGNAL], ETK_OBJECT(row->tree), NULL, row);
   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
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
   int num_visible_items;
   int num_item_to_add;
   int i;

   if (!widget || !(tree = ETK_TREE_GRID(widget)->tree) || !(tree->built))
      return;

   num_visible_items = ceil((float)geometry.h / tree->item_height) + 1;
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

   evas_object_move(tree->clip, geometry.x, geometry.y);
   evas_object_resize(tree->clip, geometry.w, geometry.h);

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
   etk_widget_redraw_queue(widget);
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
   scroll_size->h = tree->root.num_visible_children * tree->item_height;
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
   etk_widget_visibility_locked_set(tree->scrolled_view, TRUE);
   etk_widget_repeat_events_set(tree->scrolled_view, TRUE);
   etk_widget_parent_set(tree->scrolled_view, ETK_CONTAINER(tree));
   etk_widget_show(tree->scrolled_view);
   
   tree->grid = etk_widget_new(ETK_TREE_GRID_TYPE, "theme_group", "tree", "repeat_events", TRUE, "visibility_locked", TRUE, NULL);
   ETK_TREE_GRID(tree->grid)->tree = tree;
   etk_container_add(ETK_CONTAINER(tree->scrolled_view), tree->grid);
   etk_widget_show(tree->grid);

   tree->num_cols = 0;
   tree->columns = NULL;
   tree->last_selected = NULL;
   tree->column_to_resize = NULL;
   tree->resize_pointer_shown = FALSE;
   tree->headers_visible = TRUE;

   tree->root.parent = NULL;
   tree->root.child_rows = NULL;
   tree->root.num_visible_children = 0;
   tree->root.expanded = TRUE;

   tree->item_height = 0;
   tree->cell_margins[0] = 0;
   tree->cell_margins[1] = 0;
   tree->cell_margins[2] = 0;
   tree->cell_margins[3] = 0;
   tree->expander_size = 0;
   tree->rows_widgets = NULL;

   tree->built = FALSE;
   tree->frozen = FALSE;
   tree->mode = ETK_TREE_MODE_LIST;
   tree->multiple_select = FALSE;
   tree->xoffset = 0;
   tree->yoffset = 0.0;

   ETK_WIDGET(tree)->size_allocate = _etk_tree_size_allocate;
   
   etk_signal_connect("focus", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_focus_cb), NULL);
   etk_signal_connect("unfocus", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree_unfocus_cb), NULL);
}

/* Destroys the tree */
static void _etk_tree_destructor(Etk_Tree *tree)
{
   Evas_List *l;
   int i;

   if (!tree)
      return;
   
   etk_tree_clear(tree);

   for (l = tree->rows_widgets; l; l = l->next)
      _etk_tree_row_objects_free(l->data, tree);
   tree->rows_widgets = evas_list_free(tree->rows_widgets);

   for (i = 0; i < tree->num_cols; i++)
   {
      if (tree->columns[i])
         etk_object_destroy(ETK_OBJECT(tree->columns[i]));
   }
   free(tree->columns);
   /* TODO: etk_object_destroy(ETK_OBJECT(tree->grid)); */
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
      default:
         break;
   }
}

/* Resizes the tree to the size allocation */
static void _etk_tree_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Tree *tree;
   Etk_Size child_request;
   Etk_Geometry child_geometry;
   int num_visible_cols = 0;
   int grid_width = 0, freespace;
   int max_header_height = 0;
   Etk_Tree_Col *first_visible_col, *last_visible_col;
   int xoffset = 0;
   float extra_width = 0;
   int i, j, k;

   if (!(tree = ETK_TREE(widget)))
      return;

   /* Calculate the size of the cols */
   for (i = 0; i < tree->num_cols; i++)
   {
      if (tree->headers_visible)
      {
         Etk_Size header_requested_size;

         etk_widget_size_request(tree->columns[i]->header, &header_requested_size);
         if (tree->columns[i]->place == 0)
            header_requested_size.w -= tree->grid->left_inset;
         tree->columns[i]->width = ETK_MAX(header_requested_size.w, tree->columns[i]->requested_width);
      }
      else
         tree->columns[i]->width = tree->columns[i]->requested_width;
   }

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

         grid_width += tree->columns[i]->width;
         num_visible_cols++;
      }
   }
   if (num_visible_cols <= 0)
      return;

   grid_width += tree->grid->left_inset + tree->grid->right_inset;
   freespace = ETK_MAX(0, geometry.w - grid_width);
   extra_width = (float)freespace / num_visible_cols;

   k = 0;
   for (i = 0; i < tree->num_cols; i++)
   {
      for (j = 0; j < tree->num_cols; j++)
      {
         if (tree->columns[j]->place == i)
         {
            if (!tree->columns[j]->visible)
               break;

            tree->columns[j]->xoffset = xoffset;
            tree->columns[j]->visible_width = tree->columns[j]->width;
            if (tree->columns[j] == last_visible_col)
               tree->columns[j]->visible_width += freespace;
            etk_object_notify(ETK_OBJECT(tree->columns[j]), "visible_width");
            xoffset += tree->columns[j]->visible_width;
            k++;
            break;
         }
      }
   }

   /* Allocate size for the headers */
   if (tree->headers_visible)
   {
      for (i = 0; i < tree->num_cols; i++)
      {
         etk_widget_size_request(tree->columns[i]->header, &child_request);
         if (child_request.h > max_header_height)
            max_header_height = child_request.h;
      }

      for (i = 0; i < tree->num_cols; i++)
      {
         child_geometry.x = geometry.x + tree->columns[i]->xoffset;
         if (tree->columns[i] != first_visible_col)
            child_geometry.x += tree->grid->left_inset;
         child_geometry.y = geometry.y;

         child_geometry.w = tree->columns[i]->visible_width;
         if (tree->columns[i] == first_visible_col)
            child_geometry.w += tree->grid->left_inset;
         if (tree->columns[i] == last_visible_col)
            child_geometry.w += tree->grid->right_inset;

         child_geometry.h = max_header_height;
         etk_widget_size_allocate(tree->columns[i]->header, child_geometry);
      }
   }

   /* Allocate size for the tree grid */
   geometry.y += max_header_height;
   geometry.h -= max_header_height;
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
   tree_col->width = 0;
   tree_col->visible_width = 0;
   tree_col->visible = TRUE;
   tree_col->resizable = TRUE;
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
   /* TODO: etk_object_destroy("header"); */
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

/* Called when the tree is realized */
static void _etk_tree_grid_realize_cb(Etk_Object *object, void *data)
{
   const char *data_value;
   Etk_Tree *tree;
   Etk_Widget *grid;
   Evas *evas;
   int i;

   if (!(grid = ETK_WIDGET(object)) || !grid->theme_object)
      return;
   tree = ETK_TREE_GRID(grid)->tree;

   data_value = edje_object_data_get(grid->theme_object, "separator_color");
   if (!data_value || sscanf(data_value, "%d %d %d %d", &tree->separator_color.r, &tree->separator_color.g, &tree->separator_color.b, &tree->separator_color.a) != 4)
   {
      tree->separator_color.r = 255;
      tree->separator_color.g = 255;
      tree->separator_color.b = 255;
      tree->separator_color.a = 0;
   }

   if (tree->item_height < 10)
   {
      data_value = edje_object_data_get(grid->theme_object, "item_height");
      if (!data_value || sscanf(data_value, "%d", &tree->item_height) != 1 || tree->item_height < 10)
         tree->item_height = 10;
      /* TODO */
      etk_range_increments_set(ETK_RANGE(etk_scrolled_view_vscrollbar_get(ETK_SCROLLED_VIEW(tree->scrolled_view))), tree->item_height, 5 * tree->item_height);
   }
   
   data_value = edje_object_data_get(grid->theme_object, "cell_margins");
   if (!data_value || sscanf(data_value, "%d %d %d %d", &tree->cell_margins[0], &tree->cell_margins[0], &tree->cell_margins[1], &tree->cell_margins[2]) != 4)
   {
      tree->cell_margins[0] = 0;
      tree->cell_margins[1] = 0;
      tree->cell_margins[2] = 0;
      tree->cell_margins[3] = 0;
   }
   
   data_value = edje_object_data_get(grid->theme_object, "expander_size");
   if (!data_value || sscanf(data_value, "%d", &tree->expander_size) != 1)
      tree->expander_size = tree->item_height - 4;
   tree->expander_size = ETK_CLAMP(tree->expander_size, 1, tree->item_height);

   if ((evas = etk_widget_toplevel_evas_get(grid)))
   {
      tree->clip = evas_object_rectangle_add(evas);
      evas_object_show(tree->clip);
      etk_widget_member_object_add(grid, tree->clip);
   }

   for (i = 0; i < tree->num_cols; i++)
      _etk_tree_col_realize(tree, i);
}

/**************************
 * Tree
 **************************/

/* Called when an expander is clicked */
static void _etk_tree_expander_clicked_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Tree_Row *row;

   if (!(row = etk_object_data_get(object, "_Etk_Tree::row")))
      return;

   if (row->node.expanded)
      etk_tree_row_collapse(row);
   else
      etk_tree_row_expand(row);
}

/* Called when the row is pressed */
static void _etk_tree_row_pressed_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Tree_Row *row;
   Etk_Event_Mouse_Up_Down *event;

   if (!(row = etk_object_data_get(object, "_Etk_Tree::row")))
      return;

   event = event_info;
   _etk_tree_node_select(row->tree, &row->node, event->modifiers);
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

/* Called when the user presses a column header */
static void _etk_tree_header_mouse_down_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Tree_Col *col;
   Etk_Event_Mouse_Up_Down *down_event;

   if (!(col = data) || !(down_event = event))
      return;

   col->tree->column_to_resize = etk_tree_col_to_resize_get(col, down_event->widget.x);
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
      new_size = move_event->cur.canvas.x - col->tree->column_to_resize->header->geometry.x;
      if (col->tree->column_to_resize->place == 0)
         new_size -= col->tree->grid->left_inset;

      if (new_size != col->tree->column_to_resize->requested_width)
      {
         col->tree->column_to_resize->requested_width = ETK_MAX(new_size, ETK_TREE_MIN_HEADER_WIDTH);
         etk_widget_redraw_queue(ETK_WIDGET(col->tree));
      }
   }
   else
   {
      Etk_Bool show_resize_pointer = FALSE;

      show_resize_pointer = (etk_tree_col_to_resize_get(col, move_event->cur.widget.x) != NULL);

      if (show_resize_pointer && !col->tree->resize_pointer_shown)
      {
         etk_toplevel_widget_pointer_push(ETK_WIDGET(col->tree)->toplevel_parent, ETK_POINTER_H_DOUBLE_ARROW);
         col->tree->resize_pointer_shown = TRUE;
      }
      else if (!show_resize_pointer && col->tree->resize_pointer_shown)
      {
         etk_toplevel_widget_pointer_pop(ETK_WIDGET(col->tree)->toplevel_parent, ETK_POINTER_H_DOUBLE_ARROW);
         col->tree->resize_pointer_shown = FALSE;
      }
   }
}

/* Called when the mouse enters a column header */ 
static void _etk_tree_header_mouse_in_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Event_Mouse_In_Out *in_event;
   Etk_Tree_Col *col;
   Etk_Bool show_resize_pointer = FALSE;

   if (!(col = data) || !(in_event = event))
      return;

   show_resize_pointer = (etk_tree_col_to_resize_get(col, in_event->widget.x) != NULL);

   if (show_resize_pointer && !col->tree->resize_pointer_shown)
   {
      etk_toplevel_widget_pointer_push(ETK_WIDGET(col->tree)->toplevel_parent, ETK_POINTER_H_DOUBLE_ARROW);
      col->tree->resize_pointer_shown = TRUE;
   }
   else if (!show_resize_pointer && col->tree->resize_pointer_shown)
   {
      etk_toplevel_widget_pointer_pop(ETK_WIDGET(col->tree)->toplevel_parent, ETK_POINTER_H_DOUBLE_ARROW);
      col->tree->resize_pointer_shown = FALSE;
   }
}

/* Called when the mouse leaves a column header */ 
static void _etk_tree_header_mouse_out_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Tree_Col *col;

   if (!(col = data) || !col->tree->resize_pointer_shown)
      return;
   
   etk_toplevel_widget_pointer_pop(ETK_WIDGET(col->tree)->toplevel_parent, ETK_POINTER_H_DOUBLE_ARROW);
   col->tree->resize_pointer_shown = FALSE;
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
   int first_visible_nth, delta;
   int x, y, w, h;
   int i, j;

   if (!tree || !tree->grid)
      return;
   
   x = tree->grid->inner_geometry.x;
   y = tree->grid->inner_geometry.y;
   w = tree->grid->inner_geometry.w;
   h = tree->grid->inner_geometry.h;

   for (i = 0; i < tree->num_cols; i++)
   {
      if (tree->columns[i]->xoffset > w || !tree->columns[i]->visible)
      {
         evas_object_hide(tree->columns[i]->clip);
         evas_object_hide(tree->columns[i]->separator);
      }
      else
      {
         evas_object_show(tree->columns[i]->clip);
         evas_object_move(tree->columns[i]->clip, x + tree->columns[i]->xoffset, y);

         if (tree->columns[i]->xoffset + tree->columns[i]->visible_width > w)
         {
            evas_object_resize(tree->columns[i]->clip, w - tree->columns[i]->xoffset, h);
            evas_object_hide(tree->columns[i]->separator);
         }
         else
         {
            evas_object_resize(tree->columns[i]->clip, tree->columns[i]->visible_width, h);
            if (tree->columns[i]->place < tree->num_cols - 1)
               evas_object_show(tree->columns[i]->separator);
            else
               evas_object_hide(tree->columns[i]->separator);
            evas_object_move(tree->columns[i]->separator, x + tree->columns[i]->xoffset + tree->columns[i]->visible_width, y);
            evas_object_resize(tree->columns[i]->separator, 1, h);
         }
      }
   }

   first_visible_nth = tree->yoffset / tree->item_height;
   delta = tree->yoffset - (first_visible_nth * tree->item_height);

   l = tree->rows_widgets;
   _etk_tree_rows_draw(tree, &tree->root, &l, x, w, h, 0, y - delta, first_visible_nth, (first_visible_nth % 2));

   for (; l; l = l->next)
   {
      row_objects = l->data;
      etk_widget_hide(row_objects->background);
      etk_widget_hide(row_objects->expander);
      for (i = 0; i < tree->num_cols; i++)
      {
         for (j = 0; j < ETK_TREE_NUM_OBJECTS_PER_CELL; j++)
            evas_object_hide(row_objects->cells_objects[i].objects[j]);
      }
   }
}

/* Draws recursively a list of rows and their children */
static int _etk_tree_rows_draw(Etk_Tree *tree, Etk_Tree_Node *node, Evas_List **items_objects,
   int x, int w, int h, int xoffset, int yoffset, int first_row_id, int first_row_color)
{
   Etk_Tree_Row *row;
   Etk_Tree_Row_Objects *row_objects;
   int i, j;
   int first_col_offset;
   Evas_List *l;
   Etk_Geometry geometry;

   if (!tree || !node || !items_objects)
      return 0;

   i = 0;
   first_col_offset = xoffset + ((evas_list_count(node->child_rows) > 0) ? tree->expander_size : 0);
   for (l = node->child_rows; l; l = l->next)
   {
      row = l->data;
      if (first_row_id <= 0)
      {
         int item_y;
   
         if (!(*items_objects) || !(row_objects = (*items_objects)->data))
            break;
         *items_objects = (*items_objects)->next;
   
         item_y = yoffset + i * tree->item_height;
         geometry.x = x;
         geometry.y = item_y;
         geometry.w = w;
         geometry.h = tree->item_height;
         etk_widget_size_allocate(row_objects->background, geometry);
         etk_object_data_set(ETK_OBJECT(row_objects->background), "_Etk_Tree::row", row);
         etk_widget_show(row_objects->background);
         
         if ((first_row_color + i) % 2 == 0)
            etk_widget_theme_object_signal_emit(row_objects->background, "odd");
         else
            etk_widget_theme_object_signal_emit(row_objects->background, "even");
         
         if (row->node.selected)
            etk_widget_theme_object_signal_emit(row_objects->background, "selected");
         else
            etk_widget_theme_object_signal_emit(row_objects->background, "unselected");
         
         if (evas_list_count(row->node.child_rows) > 0)
         {
            geometry.x = xoffset + x;
            geometry.y = item_y + (tree->item_height - tree->expander_size + 1) / 2;
            geometry.w = tree->expander_size;
            geometry.h = tree->expander_size;
            
            etk_widget_size_allocate(row_objects->expander, geometry);
            etk_widget_theme_object_signal_emit(row_objects->expander, row->node.expanded ? "expand" : "collapse");
            etk_object_data_set(ETK_OBJECT(row_objects->expander), "_Etk_Tree::row", row);
            etk_widget_show(row_objects->expander);
         }
         else
            etk_widget_hide(row_objects->expander);
   
         for (j = 0; j < tree->num_cols; j++)
         {
            if (!tree->columns[j] || !tree->columns[j]->visible)
               continue;
            
            geometry.x = x + ((j == 0) ? first_col_offset : tree->columns[j]->xoffset) + tree->cell_margins[0];
            geometry.y = item_y + tree->cell_margins[2];
            geometry.w = tree->columns[j]->visible_width - tree->cell_margins[0] - tree->cell_margins[1];
            geometry.h = tree->item_height - tree->cell_margins[2] - tree->cell_margins[3];
            tree->columns[j]->model->render(tree->columns[j]->model, geometry, row->cells_data[j], row_objects->cells_objects[j].objects);
         }
         i++;
      }
      first_row_id--;

      if (row->node.child_rows && row->node.expanded)
      {
         i += _etk_tree_rows_draw(tree, &row->node, items_objects, x, w, h, first_col_offset, yoffset + i * tree->item_height, first_row_id, (first_row_color + i) % 2);
         first_row_id -= row->node.num_visible_children;
      }
   }

   return i;
}

/* Creates a new row and add it to the node */
static Etk_Tree_Row *_etk_tree_row_new_valist(Etk_Tree *tree, Etk_Tree_Node *node, va_list args)
{
   Etk_Tree_Row *new_row;
   Etk_Tree_Node *n;
   int i;

   if (!tree || !tree->built || !node)
      return NULL;

   new_row = malloc(sizeof(Etk_Tree_Row));
   new_row->tree = tree;
   new_row->node.row = new_row;
   new_row->node.parent = node;
   new_row->node.child_rows = NULL;
   new_row->node.num_visible_children = 0;
   new_row->node.expanded = FALSE;
   new_row->node.selected = FALSE;
   new_row->data = NULL;

   new_row->cells_data = malloc(sizeof(void *) * tree->num_cols);
   for (i = 0; i < tree->num_cols; i++)
   {
      new_row->cells_data[i] = calloc(1, tree->columns[i]->model->cell_data_size);
      if (tree->columns[i]->model->cell_data_init)
         tree->columns[i]->model->cell_data_init(tree->columns[i]->model, new_row->cells_data[i]);
   }
   etk_tree_row_fields_set_valist(new_row, args);

   for (n = new_row->node.parent; n; n = n->parent)
   {
      n->num_visible_children++;
      if (!n->expanded)
         break;
   }

   node->child_rows = evas_list_append(node->child_rows, new_row);
   
   if (!tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(tree));
   return new_row;
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
   new_row_objects->background = etk_widget_new(ETK_WIDGET_TYPE, "theme_group", "tree_row", NULL);
   etk_widget_visibility_locked_set(new_row_objects->background, TRUE);
   etk_widget_clip_set(new_row_objects->background, tree->clip);
   etk_signal_connect("mouse_down", ETK_OBJECT(new_row_objects->background), ETK_CALLBACK(_etk_tree_row_pressed_cb), NULL);
   etk_widget_parent_set(new_row_objects->background, ETK_CONTAINER(tree->grid));
   
   new_row_objects->expander = etk_widget_new(ETK_WIDGET_TYPE, "theme_group", "tree_expander", NULL);
   etk_widget_visibility_locked_set(new_row_objects->expander, TRUE);
   if (tree->num_cols > 0)
      etk_widget_clip_set(new_row_objects->expander, tree->columns[0]->clip);
   etk_widget_parent_set(new_row_objects->expander, ETK_CONTAINER(tree->grid));
   etk_signal_connect("mouse_click", ETK_OBJECT(new_row_objects->expander), ETK_CALLBACK(_etk_tree_expander_clicked_cb), NULL);

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
      etk_object_destroy(ETK_OBJECT(row_objects->background));
   if (row_objects->expander)
      etk_object_destroy(ETK_OBJECT(row_objects->expander));

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

/* Frees a row */
static void _etk_tree_row_free(Etk_Tree_Row *row)
{
   Etk_Tree_Node *n;

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

   while (row->node.child_rows)
   {
      _etk_tree_row_free(row->node.child_rows->data);
      row->node.child_rows = evas_list_remove_list(row->node.child_rows, row->node.child_rows);
   }

   for (n = row->node.parent; n; n = n->parent)
   {
      n->num_visible_children--;
      if (!n->expanded)
         break;
   }
   
   free(row);
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

/* Selects rows in the tree according to the keyboard modifiers and the clicked node */
static void _etk_tree_node_select(Etk_Tree *tree, Etk_Tree_Node *node, Evas_Modifier *modifiers)
{
   if (!tree || !node)
      return;

   if (tree->mode == ETK_TREE_MODE_TREE || !tree->multiple_select || !modifiers)
   {
      etk_tree_unselect_all(tree);
      node->selected = TRUE;
      tree->last_selected = node;
      etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_SELECTED_SIGNAL], ETK_OBJECT(tree), NULL, node->row);
   }
   else
   {
      if (evas_key_modifier_is_set(modifiers, "Shift"))
      {
         if (!evas_key_modifier_is_set(modifiers, "Control"))
            etk_tree_unselect_all(tree);
   
         if (!tree->last_selected)
            node->selected = TRUE;
         else
         {
            Etk_Bool selected = FALSE;
            Evas_List *l;
            Etk_Tree_Row *row;

            for (l = tree->root.child_rows; l; l = l->next)
            {
               row = l->data;
               if (&row->node == tree->last_selected || &row->node == node)
               {
                  row->node.selected = TRUE;
                  selected = !selected;
               }
               else
                  row->node.selected |= selected;
            }
            if (selected)
            {
               etk_tree_unselect_all(tree);
               node->selected = TRUE;
            }
         }
         tree->last_selected = node;
         etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_SELECTED_SIGNAL], ETK_OBJECT(tree), NULL, node->row);
      }
      else if (evas_key_modifier_is_set(modifiers, "Control"))
      {
         if (node->selected)
         {
            node->selected = FALSE;
            tree->last_selected = node;
            etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_UNSELECTED_SIGNAL], ETK_OBJECT(tree), NULL, node->row);
         }
         else
         {
            node->selected = TRUE;
            tree->last_selected = node;
            etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_SELECTED_SIGNAL], ETK_OBJECT(tree), NULL, node->row);
         }
      }
      else
      {
         etk_tree_unselect_all(tree);
         node->selected = TRUE;
         tree->last_selected = node;
         etk_signal_emit(_etk_tree_signals[ETK_TREE_ROW_SELECTED_SIGNAL], ETK_OBJECT(tree), NULL, node->row);
      }
   }

   etk_widget_redraw_queue(ETK_WIDGET(tree->grid));
}
