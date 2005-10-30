/** @file etk_tree.c */
#include "etk_tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Edje.h>
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

#define ETK_TREE_GRID_TYPE       (_etk_grid_type_get())
#define ETK_TREE_GRID(obj)       (ETK_OBJECT_CAST((obj), ETK_TREE_GRID_TYPE, Etk_grid))
#define ETK_IS_TREE_GRID(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TREE_GRID_TYPE))

typedef struct _Etk_grid
{
   /* Inherit form Etk_Widget */
   Etk_Widget widget;

   Etk_Tree *tree;
} Etk_grid;

typedef struct _Etk_Tree_Item_Object
{
   Evas_Object *text_object;
   Evas_Object *image_object;
} Etk_Tree_Item_Object;

typedef struct _Etk_Tree_Item_Objects
{
   Evas_Object *expander;
   Evas_Object *rect_bg;
   Etk_Tree_Item_Object *objects;
} Etk_Tree_Item_Objects;

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
   ETK_TREE_COL_TYPE_PROPERTY,
   ETK_TREE_COL_WIDTH_PROPERTY,
   ETK_TREE_COL_MIN_WIDTH_PROPERTY,
   ETK_TREE_COL_VISIBLE_WIDTH_PROPERTY,
   ETK_TREE_COL_VISIBLE_PROPERTY,
   ETK_TREE_COL_RESIZABLE_PROPERTY,
   ETK_TREE_COL_PLACE_PROPERTY,
   ETK_TREE_COL_XALIGN_PROPERTY
};

static Etk_Type *_etk_grid_type_get();
static void _etk_grid_constructor(Etk_grid *grid);
static void _etk_grid_move_resize(Etk_Widget *widget, int x, int y, int w, int h);
static void _etk_grid_realize_cb(Etk_Object *object, void *data);
static void _etk_grid_mouse_wheel_cb(Etk_Object *object, void *event_info, void *data);

static void _etk_tree_constructor(Etk_Tree *tree);
static void _etk_tree_destructor(Etk_Tree *tree);
static void _etk_tree_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_tree_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_tree_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static void _etk_tree_col_constructor(Etk_Tree_Col *tree_col);
static void _etk_tree_col_destructor(Etk_Tree_Col *tree_col);
static void _etk_tree_col_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_tree_col_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

static void _etk_tree_expander_clicked_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree_row_pressed_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree_header_mouse_down_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_up_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_move_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_in_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_out_cb(Etk_Object *object, void *event, void *data);

static void _etk_tree_update(Etk_Tree *tree);
static int _etk_tree_rows_draw(Etk_Tree *tree, Etk_Tree_Node *node, Evas_List **items_objects,
   int x, int w, int h, int xoffset, int yoffset, int first_row_id, int first_row_color);
static Etk_Tree_Row *_etk_tree_row_new_valist(Etk_Tree *tree, Etk_Tree_Node *node, va_list args);
static Etk_Tree_Item_Objects *_etk_tree_item_objects_new(Etk_Tree *tree);
static void _etk_tree_item_objects_free(Etk_Tree_Item_Objects *item_objects, Etk_Tree *tree);
static void _etk_tree_row_free(Etk_Tree_Row *row);
static void _etk_tree_cell_clear(Etk_Tree_Cell *cell, Etk_Tree_Col_Type cell_type);
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
      tree_type = etk_type_new("Etk_Tree", ETK_CONTAINER_TYPE, sizeof(Etk_Tree), ETK_CONSTRUCTOR(_etk_tree_constructor), ETK_DESTRUCTOR(_etk_tree_destructor), NULL);

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
   return etk_widget_new(ETK_TREE_TYPE, NULL);
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
      tree_col_type = etk_type_new("Etk_Tree_Col", ETK_OBJECT_TYPE, sizeof(Etk_Tree_Col), ETK_CONSTRUCTOR(_etk_tree_col_constructor), ETK_DESTRUCTOR(_etk_tree_col_destructor), NULL);

      etk_type_property_add(tree_col_type, "title",         ETK_TREE_COL_TITLE_PROPERTY,           ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_string(NULL));
      etk_type_property_add(tree_col_type, "cell_type",     ETK_TREE_COL_TYPE_PROPERTY,            ETK_PROPERTY_INT,    ETK_PROPERTY_READABLE,           NULL);
      etk_type_property_add(tree_col_type, "width",         ETK_TREE_COL_WIDTH_PROPERTY,           ETK_PROPERTY_INT,    ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(ETK_TREE_MIN_HEADER_WIDTH));
      etk_type_property_add(tree_col_type, "min_width",     ETK_TREE_COL_MIN_WIDTH_PROPERTY,       ETK_PROPERTY_INT,    ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(-1));
      etk_type_property_add(tree_col_type, "visible_width", ETK_TREE_COL_VISIBLE_WIDTH_PROPERTY,   ETK_PROPERTY_INT,    ETK_PROPERTY_READABLE,           NULL);
      etk_type_property_add(tree_col_type, "visible",       ETK_TREE_COL_VISIBLE_PROPERTY,         ETK_PROPERTY_BOOL,   ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(TRUE));
      etk_type_property_add(tree_col_type, "resizable",     ETK_TREE_COL_RESIZABLE_PROPERTY,       ETK_PROPERTY_BOOL,   ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(TRUE));
      etk_type_property_add(tree_col_type, "place",         ETK_TREE_COL_PLACE_PROPERTY,           ETK_PROPERTY_INT,    ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(0));
      etk_type_property_add(tree_col_type, "xalign",        ETK_TREE_COL_XALIGN_PROPERTY,          ETK_PROPERTY_FLOAT,  ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_float(0.0));

      tree_col_type->property_set = _etk_tree_col_property_set;
      tree_col_type->property_get = _etk_tree_col_property_get;
   }

   return tree_col_type;
}

/**
 * @brief Creates a new column for a tree
 * @param tree a tree
 * @param title the tile of the column
 * @param type the type of the objects in the cells of the column
 * @param width the requested width of the column. It won't be necessary the visible width of the column since it can be expanded to fit the available space
 * @return Returns the new column
 */
Etk_Tree_Col *etk_tree_col_new(Etk_Tree *tree, const char *title, Etk_Tree_Col_Type type, int width)
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
   new_col->type = type;
   etk_object_notify(ETK_OBJECT(new_col), "col_type");
   switch (type)
   {
      case ETK_TREE_COL_INT:
      case ETK_TREE_COL_DOUBLE:
         etk_tree_col_xalign_set(new_col, 1.0);
         break;
      default:
         etk_tree_col_xalign_set(new_col, 0.0);
         break;
   }

   /* Create the header widget */
   new_header = etk_widget_new(ETK_BUTTON_TYPE, "theme_group", "tree_header", "label", title, "xalign", 0.0, NULL);
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
 * @brief Sets the horizontal alignment of the column
 * @param col a tree column
 * @param xalign the horizontal alignment to set (0.0 for left, 1.0 for right)
 */
void etk_tree_col_xalign_set(Etk_Tree_Col *col, float xalign)
{
   if (!col)
      return;

   col->xalign = ETK_CLAMP(0.0, 1.0, xalign);
   etk_object_notify(ETK_OBJECT(col), "xalign");
   if (col->tree)
      etk_widget_redraw_queue(ETK_WIDGET(col->tree));
}

/**
 * @brief Gets the horizontal alignment of the column
 * @param col a tree column
 * @return Returns the horizontal alignment of the column
 */
float etk_tree_col_xalign_get(Etk_Tree_Col *col)
{
   if (!col)
      return 0.0;
   return col->xalign;
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
 * @param Returns the place of the column
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
   tree->frozen = FALSE;
   etk_widget_redraw_queue(ETK_WIDGET(tree));
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

   if (!row)
      return;

   if ((l = evas_list_find_list(row->node.parent->child_rows, row)))
   {
      Etk_Tree_Node *n;
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
   }
   
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

   if (!row)
      return;

   while ((col = va_arg(args, Etk_Tree_Col *)))
   {
      Etk_Tree_Cell *cell;

      cell = &row->cells[col->id];
      switch (col->type)
      {
         case ETK_TREE_COL_TEXT:
         {
            const char *text;
            text = va_arg(args, char *);
            if (cell->text_value != text)
            {
               free(cell->text_value);
               if (text)
                  cell->text_value = strdup(text);
               else
                  cell->text_value = NULL;
            }
            break;
         }
         case ETK_TREE_COL_IMAGE:
         {
            const char *image;
            image = va_arg(args, char *);
            if (cell->image_filename_value != image)
            {
               free(cell->image_filename_value);
               if (image)
                  cell->image_filename_value = strdup(image);
               else
                  cell->image_filename_value = NULL;
            }
            break;
         }
         case ETK_TREE_COL_ICON_TEXT:
         {
            const char *icon;
            const char *text;

            icon = va_arg(args, char *);
            text = va_arg(args, char *);

            if (cell->icon_text_value.icon_filename != icon)
            {
               free(cell->icon_text_value.icon_filename);
               if (icon)
                  cell->icon_text_value.icon_filename = strdup(icon);
               else
                  cell->icon_text_value.icon_filename = NULL;
            }

            if (cell->icon_text_value.text != text)
            {
               free(cell->icon_text_value.text);
               if (text)
                  cell->icon_text_value.text = strdup(text);
               else
                  cell->icon_text_value.text = NULL;
            }
            break;
         }
         case ETK_TREE_COL_INT:
         {
            int value;
            value = va_arg(args, int);
            cell->int_value = value;
            break;
         }
         case ETK_TREE_COL_DOUBLE:
         {
            double value;
            value = va_arg(args, double);
            cell->double_value = value;
            break;
         }
         default:
            break;
      }   
   }

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

   if (!row)
      return;

   while ((col = va_arg(args, Etk_Tree_Col *)))
   {
      Etk_Tree_Cell *cell;

      cell = &row->cells[col->id];
      switch (col->type)
      {
         case ETK_TREE_COL_TEXT:
         {
            char **text;
            text = va_arg(args, char **);
            if (text)
               *text = cell->text_value;
            break;
         }
         case ETK_TREE_COL_IMAGE:
         {
            char **image;
            image = va_arg(args, char **);
            *image = cell->image_filename_value;
            break;
         }
         case ETK_TREE_COL_ICON_TEXT:
         {
            char **icon;
            char **text;
            icon = va_arg(args, char **);
            text = va_arg(args, char **);
            if (icon)
               *icon = cell->icon_text_value.icon_filename;
            if (text)
               *text = cell->icon_text_value.text;
            break;
         }
         case ETK_TREE_COL_INT:
         {
            int *value;
            value = va_arg(args, int *);
            if (value)
               *value = cell->int_value;
            break;
         }
         case ETK_TREE_COL_DOUBLE:
         {
            double *value;
            value = va_arg(args, double *);
            if (value)
               *value = cell->double_value;
            break;
         }
         default:
            break;
      }   
   }
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
   float offset;

   if (!row || row->node.expanded || !row->tree || (row->tree->mode != ETK_TREE_MODE_TREE))
      return;

   offset = row->tree->xscroll_percent * row->tree->item_height * row->tree->root.num_visible_children;
   row->node.expanded = TRUE;
   for (n = row->node.parent; n && n->expanded; n = n->parent)
      n->num_visible_children += row->node.num_visible_children;
   row->tree->xscroll_percent = offset / (row->tree->item_height * row->tree->root.num_visible_children);
   row->tree->xscroll_percent = ETK_CLAMP(row->tree->xscroll_percent, 0.0, 1.0);

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
   float offset;

   if (!row || !row->node.expanded || !row->tree || (row->tree->mode != ETK_TREE_MODE_TREE))
      return;

   offset = row->tree->xscroll_percent * row->tree->item_height * row->tree->root.num_visible_children;
   row->node.expanded = FALSE;
   for (n = row->node.parent; n && n->expanded; n = n->parent)
      n->num_visible_children -= row->node.num_visible_children;
   row->tree->xscroll_percent = offset / (row->tree->item_height * row->tree->root.num_visible_children);
   row->tree->xscroll_percent = ETK_CLAMP(row->tree->xscroll_percent, 0.0, 1.0);

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
static Etk_Type *_etk_grid_type_get()
{
   static Etk_Type *grid_type = NULL;

   if (!grid_type)
      grid_type = etk_type_new("Etk_grid", ETK_WIDGET_TYPE, sizeof(Etk_grid), ETK_CONSTRUCTOR(_etk_grid_constructor), NULL, NULL);

   return grid_type;
}

/* Initializes the default values of the tree grid */
static void _etk_grid_constructor(Etk_grid *grid)
{
   if (!grid)
      return;

   ETK_WIDGET(grid)->move_resize = _etk_grid_move_resize;
   etk_signal_connect_after("realize", ETK_OBJECT(grid), ETK_CALLBACK(_etk_grid_realize_cb), NULL);
   etk_signal_connect("mouse_wheel", ETK_OBJECT(grid), ETK_CALLBACK(_etk_grid_mouse_wheel_cb), NULL);
}

/* Moves and resizes the tree grid */
static void _etk_grid_move_resize(Etk_Widget *widget, int x, int y, int w, int h)
{
   Etk_Tree *tree;
   int num_visible_items;
   int num_item_to_add;
   int i;

   if (!widget || !(tree = ETK_TREE_GRID(widget)->tree) || !(tree->built))
      return;

   num_visible_items = ceil((float)h / tree->item_height) + 1;
   if (num_visible_items < 0)
      num_visible_items = 0;
   num_item_to_add = num_visible_items - evas_list_count(tree->items_objects);

   if (num_item_to_add > 0)
   {
      Etk_Tree_Item_Objects *item_objects;

      for (i = 0; i < num_item_to_add; i++)
      {
         if (!(item_objects = _etk_tree_item_objects_new(tree)))
            break;
         tree->items_objects = evas_list_append(tree->items_objects, item_objects);
      }
   }
   else if (num_item_to_add < 0)
   {
      for (i = 0; i < -num_item_to_add; i++)
      {
         if (!tree->items_objects)
            break;
         _etk_tree_item_objects_free(tree->items_objects->data, tree);
         tree->items_objects = evas_list_remove(tree->items_objects, tree->items_objects);
      }
   }

   evas_object_move(tree->clip, x, y);
   evas_object_resize(tree->clip, w, h);

   _etk_tree_update(tree);
}

/**************************
 * Tree
 **************************/

/* Initializes the default values of the tree */
static void _etk_tree_constructor(Etk_Tree *tree)
{
   if (!tree)
      return;

   tree->grid = etk_widget_new(ETK_TREE_GRID_TYPE, "theme_group", "tree", NULL);

   ETK_TREE_GRID(tree->grid)->tree = tree;
   etk_widget_parent_set(tree->grid, ETK_CONTAINER(tree));
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
   tree->root.num_parent_children = 0;
   tree->root.expanded = TRUE;

   tree->item_height = -1;
   tree->image_height = -1;
   tree->expander_size = -1;
   tree->items_objects = NULL;

   tree->built = FALSE;
   tree->frozen = FALSE;
   tree->mode = ETK_TREE_MODE_LIST;
   tree->multiple_select = FALSE;
   tree->xscroll_percent = 0.0;
   tree->yscroll_percent = 0.0;

   ETK_WIDGET(tree)->size_allocate = _etk_tree_size_allocate;
}

/* Destroys the tree */
static void _etk_tree_destructor(Etk_Tree *tree)
{
   Evas_List *l;
   int i;

   if (!tree)
      return;

   for (l = tree->items_objects; l; l = l->next)
      _etk_tree_item_objects_free(l->data, tree);
   tree->items_objects = evas_list_free(tree->items_objects);

   etk_tree_clear(tree);

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

   grid_width += tree->grid->left_padding + tree->grid->right_padding + tree->grid->left_inset + tree->grid->right_inset;
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
         child_geometry.x = geometry.x + tree->columns[i]->xoffset + tree->grid->left_padding;
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
   etk_widget_size_allocate(tree->grid, geometry);
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
   tree_col->id = 0;
   tree_col->place = 0;
   tree_col->xoffset = 0;
   tree_col->requested_width = 0;
   tree_col->width = 0;
   tree_col->visible_width = 0;
   tree_col->visible = TRUE;
   tree_col->resizable = TRUE;
   tree_col->xalign = 0.0;
   tree_col->header = NULL;
   tree_col->clip = NULL;
   tree_col->separator = NULL;
}

/* Destroys the tree column */
static void _etk_tree_col_destructor(Etk_Tree_Col *tree_col)
{
   if (!tree_col)
      return;

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
      case ETK_TREE_COL_XALIGN_PROPERTY:
         etk_tree_col_xalign_set(tree_col, etk_property_value_float_get(value));
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
      case ETK_TREE_COL_TYPE_PROPERTY:
         etk_property_value_int_set(value, tree_col->type);
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
      case ETK_TREE_COL_XALIGN_PROPERTY:
         etk_property_value_float_set(value, tree_col->xalign);
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
static void _etk_grid_realize_cb(Etk_Object *object, void *data)
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

   data_value = edje_object_data_get(grid->theme_object, "row_color1");
   if (!data_value || sscanf(data_value, "%d %d %d %d", &tree->row_color1.r, &tree->row_color1.g, &tree->row_color1.b, &tree->row_color1.a) != 4)
   {
      tree->row_color1.r = 255;
      tree->row_color1.g = 255;
      tree->row_color1.b = 255;
      tree->row_color1.a = 0;
   }

   data_value = edje_object_data_get(grid->theme_object, "row_color2");
   if (!data_value || sscanf(data_value, "%d %d %d %d", &tree->row_color2.r, &tree->row_color2.g, &tree->row_color2.b, &tree->row_color2.a) != 4)
   {
      tree->row_color2.r = tree->row_color1.r;
      tree->row_color2.g = tree->row_color1.g;
      tree->row_color2.b = tree->row_color1.b;
      tree->row_color2.a = tree->row_color1.a;
   }

   data_value = edje_object_data_get(grid->theme_object, "row_selected_color");
   if (!data_value || sscanf(data_value, "%d %d %d %d", &tree->row_selected_color.r, &tree->row_selected_color.g, &tree->row_selected_color.b, &tree->row_selected_color.a) != 4)
   {
      tree->row_selected_color.r = tree->row_color1.r - 50;
      tree->row_selected_color.g = tree->row_color1.g - 50;
      tree->row_selected_color.b = tree->row_color1.b - 50;
      tree->row_selected_color.a = tree->row_color1.a - 50;
   }

   if (tree->item_height < 10)
   {
      data_value = edje_object_data_get(grid->theme_object, "item_height");
      if (!data_value || sscanf(data_value, "%d", &tree->item_height) != 1 || tree->item_height < 10)
         tree->item_height = 10;
   }

   if (tree->image_height < 6)
   {
      data_value = edje_object_data_get(grid->theme_object, "image_height");
      if (!data_value || sscanf(data_value, "%d", &tree->image_height) != 1 || tree->image_height < 6 || tree->image_height > tree->item_height)
         tree->image_height = tree->item_height - 4;
   }

   if (tree->expander_size < 6)
   {
      data_value = edje_object_data_get(grid->theme_object, "expander_size");
      if (!data_value || sscanf(data_value, "%d", &tree->expander_size) != 1 || tree->expander_size < 6 || tree->expander_size > tree->item_height)
         tree->expander_size = tree->item_height - 4;
   }

   if ((evas = etk_widget_toplevel_evas_get(grid)))
   {
      tree->clip = evas_object_rectangle_add(evas);
      evas_object_show(tree->clip);
      etk_widget_member_object_add(grid, tree->clip);
   }

   for (i = 0; i < tree->num_cols; i++)
      _etk_tree_col_realize(tree, i);
}

/* Called when the mouse wheel is used over the tree grid. TODO: scrollbars */
static void _etk_grid_mouse_wheel_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Tree *tree;
   Etk_Event_Mouse_Wheel *event;

   if (!object || !(tree = ETK_TREE_GRID(object)->tree) || !(event = event_info))
      return;

   tree->xscroll_percent += event->z * ((float)3 / tree->root.num_visible_children);
   tree->xscroll_percent = ETK_CLAMP(tree->xscroll_percent, 0.0, 1.0);
   _etk_tree_update(tree);
}

/**************************
 * Tree
 **************************/

/* Called when an expander is clicked */
static void _etk_tree_expander_clicked_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree_Row *row;

   if (!(row = evas_object_data_get(obj, "etk_row")))
      return;

   if (row->node.expanded)
      etk_tree_row_collapse(row);
   else
      etk_tree_row_expand(row);
}

/* Called when the row is pressed */
static void _etk_tree_row_pressed_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree_Row *row;
   Evas_Event_Mouse_Down *event;

   if (!(row = evas_object_data_get(obj, "etk_row")))
      return;

   event = event_info;
   _etk_tree_node_select(row->tree, &row->node, event->modifiers);
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
   Etk_Tree_Item_Objects *item_objects;
   int invisible_height, offset, first_visible_nth, delta;
   int x, y, w, h;
   int i;

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

   invisible_height = (tree->root.num_visible_children * tree->item_height) - h;
   if (invisible_height < 0)
      invisible_height = 0;
   offset = invisible_height * tree->xscroll_percent;
   first_visible_nth = offset / tree->item_height;
   delta = offset - (first_visible_nth * tree->item_height);

   l = tree->items_objects;
   _etk_tree_rows_draw(tree, &tree->root, &l, x, w, h, 0, y - delta, first_visible_nth, (first_visible_nth % 2));

   for (; l; l = l->next)
   {
      item_objects = l->data;
      evas_object_hide(item_objects->rect_bg);
      evas_object_hide(item_objects->expander);
      for (i = 0; i < tree->num_cols; i++)
      {
         if (item_objects->objects[i].text_object)
            evas_object_hide(item_objects->objects[i].text_object);
         if (item_objects->objects[i].image_object)
            evas_object_hide(item_objects->objects[i].image_object);
      }
   }
}

/* Draws recursively a list of rows and their children */
static int _etk_tree_rows_draw(Etk_Tree *tree, Etk_Tree_Node *node, Evas_List **items_objects,
   int x, int w, int h, int xoffset, int yoffset, int first_row_id, int first_row_color)
{
   Etk_Tree_Row *row;
   Etk_Tree_Item_Objects *item_objects;
   int i, j;
   int first_col_offset;
   Evas_List *l;

   if (!tree || !node || !items_objects)
      return 0;

   first_col_offset = xoffset + ((node->num_parent_children > 0) ? tree->expander_size : 4) + 4;

   i = 0;
   for (l = node->child_rows; l; l = l->next)
   {
      row = l->data;
      if (first_row_id <= 0)
      {
         int item_y;
   
         if (!(*items_objects) || !(item_objects = (*items_objects)->data))
            break;
         *items_objects = (*items_objects)->next;
   
         item_y = yoffset + i * tree->item_height;
         evas_object_move(item_objects->rect_bg, x, item_y);
         evas_object_resize(item_objects->rect_bg, w, tree->item_height);
         evas_object_data_set(item_objects->rect_bg, "etk_row", row);
         evas_object_show(item_objects->rect_bg);
         if (row->node.selected)
            evas_object_color_set(item_objects->rect_bg, tree->row_selected_color.r, tree->row_selected_color.g, tree->row_selected_color.b, tree->row_selected_color.a);
         else
         {
            if ((first_row_color + i) % 2 == 0)
               evas_object_color_set(item_objects->rect_bg, tree->row_color1.r, tree->row_color1.g, tree->row_color1.b, tree->row_color1.a);
            else
               evas_object_color_set(item_objects->rect_bg, tree->row_color2.r, tree->row_color2.g, tree->row_color2.b, tree->row_color2.a);
         }
         
         if (node->num_parent_children > 0)
         {
            if (evas_list_count(row->node.child_rows) > 0)
            {
               evas_object_move(item_objects->expander, xoffset + x, item_y + (tree->item_height - tree->expander_size + 1) / 2);
               evas_object_resize(item_objects->expander, tree->expander_size, tree->expander_size);
               if (row->node.expanded)
                  edje_object_signal_emit(item_objects->expander, "expand", "");
               else
                  edje_object_signal_emit(item_objects->expander, "collapse", "");
               evas_object_data_set(item_objects->expander, "etk_row", row);
               evas_object_show(item_objects->expander);
            }
            else
               evas_object_hide(item_objects->expander);
         }
         else
            evas_object_hide(item_objects->expander);
   
         for (j = 0; j < tree->num_cols; j++)
         {
            Etk_Bool show_text = FALSE, show_image = FALSE;
            int image_offset = 0;
   
            if (!tree->columns[j] || !tree->columns[j]->visible)
               continue;
   
            switch (tree->columns[j]->type)
            {
               case ETK_TREE_COL_TEXT:
                  evas_object_text_text_set(item_objects->objects[j].text_object, row->cells[j].text_value);
                  show_text = TRUE;
                  break;
               case ETK_TREE_COL_INT:
               {
                  char string[256];
   
                  snprintf(string, 255, "%d", row->cells[j].int_value);
                  evas_object_text_text_set(item_objects->objects[j].text_object, string);
                  show_text = TRUE;
                  break;
               }
               case ETK_TREE_COL_DOUBLE:
               {
                  char string[256];
   
                  snprintf(string, 255, "%'.2f", row->cells[j].double_value);
                  evas_object_text_text_set(item_objects->objects[j].text_object, string);
                  show_text = TRUE;
                  break;
               }
               case ETK_TREE_COL_IMAGE:
                  evas_object_image_file_set(item_objects->objects[j].image_object, row->cells[j].image_filename_value, NULL);
                  show_image = TRUE;
                  break;
               case ETK_TREE_COL_ICON_TEXT:
                  evas_object_image_file_set(item_objects->objects[j].image_object, row->cells[j].icon_text_value.icon_filename, NULL);
                  show_image = TRUE;
                  evas_object_text_text_set(item_objects->objects[j].text_object, row->cells[j].icon_text_value.text);
                  show_text = TRUE;
                  break;
               default:
                  break;
            }
   
            if (show_image)
            {
               int iw, ih;
   
               evas_object_image_size_get(item_objects->objects[j].image_object, &iw, &ih);
               if (iw > tree->image_height)
               {
                  iw = tree->image_height * ((float)iw / ih);
                  ih = tree->image_height;
               }
               image_offset = iw + 8;
               evas_object_move(item_objects->objects[j].image_object,
                  x + ((j == 0) ? first_col_offset : tree->columns[j]->xoffset + 8),
                  item_y + (tree->item_height - ih + 1) / 2);
               evas_object_resize(item_objects->objects[j].image_object, iw, ih);
               evas_object_image_fill_set(item_objects->objects[j].image_object, 0, 0, iw, ih);
               evas_object_show(item_objects->objects[j].image_object);
            }
            else
               evas_object_hide(item_objects->objects[j].image_object);
   
            if (show_text)
            {
               Evas_Coord th;
   
               evas_object_geometry_get(item_objects->objects[j].text_object, NULL, NULL, NULL, &th);
               evas_object_move(item_objects->objects[j].text_object,
                  x + image_offset + ((j == 0) ? first_col_offset : tree->columns[j]->xoffset + 8),
                  item_y + (tree->item_height - th + 1) / 2);
               evas_object_show(item_objects->objects[j].text_object);
            }
            else
               evas_object_hide(item_objects->objects[j].text_object);
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

   if (!tree || !tree->built || !node)
      return NULL;

   new_row = malloc(sizeof(Etk_Tree_Row));
   new_row->tree = tree;
   new_row->node.row = new_row;
   new_row->node.parent = node;
   new_row->node.child_rows = NULL;
   new_row->node.num_visible_children = 0;
   new_row->node.num_parent_children = 0;
   new_row->node.expanded = FALSE;
   new_row->node.selected = FALSE;
   new_row->data = NULL;

   new_row->cells = calloc(1, sizeof(Etk_Tree_Cell) * tree->num_cols);
   etk_tree_row_fields_set_valist(new_row, args);

   for (n = new_row->node.parent; n; n = n->parent)
   {
      n->num_visible_children++;
      if (!n->expanded)
         break;
   }

   if ((n = new_row->node.parent) && (n = n->parent))
      n->num_parent_children++;

   node->child_rows = evas_list_append(node->child_rows, new_row);

   return new_row;
}

/* Creates the evas objects needed by a row */ 
static Etk_Tree_Item_Objects *_etk_tree_item_objects_new(Etk_Tree *tree)
{
   Etk_Tree_Item_Objects *new_item_objects;
   Evas *evas;
   int i;

   if (!tree || !tree->built || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(tree))))
      return NULL;

   new_item_objects = malloc(sizeof(Etk_Tree_Item_Objects));
   new_item_objects->rect_bg = evas_object_rectangle_add(evas);
   evas_object_clip_set(new_item_objects->rect_bg, tree->clip);
   evas_object_event_callback_add(new_item_objects->rect_bg, EVAS_CALLBACK_MOUSE_DOWN, _etk_tree_row_pressed_cb, NULL);
   etk_widget_member_object_add(tree->grid, new_item_objects->rect_bg);

   new_item_objects->expander = edje_object_add(evas);
   edje_object_file_set(new_item_objects->expander, ETK_WIDGET(tree)->theme_file, "tree_expander");
   if (tree->num_cols > 0)
      evas_object_clip_set(new_item_objects->expander, tree->columns[0]->clip);
   evas_object_event_callback_add(new_item_objects->expander, EVAS_CALLBACK_MOUSE_UP, _etk_tree_expander_clicked_cb, NULL);
   etk_widget_member_object_add(tree->grid, new_item_objects->expander);

   new_item_objects->objects = malloc(sizeof(Etk_Tree_Item_Object) * tree->num_cols);
   for (i = 0; i < tree->num_cols; i++)
   {
      if (!tree->columns[i])
         continue;

      switch (tree->columns[i]->type)
      {
         case ETK_TREE_COL_TEXT:
         case ETK_TREE_COL_INT:
         case ETK_TREE_COL_DOUBLE:
            new_item_objects->objects[i].text_object = evas_object_text_add(evas);
            evas_object_text_font_set(new_item_objects->objects[i].text_object, "Vera", 10);
            evas_object_color_set(new_item_objects->objects[i].text_object, 0, 0, 0, 255);
            evas_object_repeat_events_set(new_item_objects->objects[i].text_object, 1);
            evas_object_clip_set(new_item_objects->objects[i].text_object, tree->columns[i]->clip);
            etk_widget_member_object_add(tree->grid, new_item_objects->objects[i].text_object);
            new_item_objects->objects[i].image_object = NULL;
            break;
         case ETK_TREE_COL_IMAGE:
            new_item_objects->objects[i].text_object = NULL;
            new_item_objects->objects[i].image_object = evas_object_image_add(evas);
            evas_object_repeat_events_set(new_item_objects->objects[i].image_object, 1);
            evas_object_clip_set(new_item_objects->objects[i].image_object, tree->columns[i]->clip);
            etk_widget_member_object_add(tree->grid, new_item_objects->objects[i].image_object);
            break;
         case ETK_TREE_COL_ICON_TEXT:
            new_item_objects->objects[i].text_object = evas_object_text_add(evas);
            evas_object_text_font_set(new_item_objects->objects[i].text_object, "Vera", 10);
            evas_object_color_set(new_item_objects->objects[i].text_object, 0, 0, 0, 255);
            evas_object_repeat_events_set(new_item_objects->objects[i].text_object, 1);
            evas_object_clip_set(new_item_objects->objects[i].text_object, tree->columns[i]->clip);
            etk_widget_member_object_add(tree->grid, new_item_objects->objects[i].text_object);

            new_item_objects->objects[i].image_object = evas_object_image_add(evas);
            evas_object_repeat_events_set(new_item_objects->objects[i].image_object, 1);
            evas_object_clip_set(new_item_objects->objects[i].image_object, tree->columns[i]->clip);
            etk_widget_member_object_add(tree->grid, new_item_objects->objects[i].image_object);
            break;
         default:
            new_item_objects->objects[i].text_object = NULL;
            new_item_objects->objects[i].image_object = NULL;
            break;
      }

      etk_widget_member_object_raise(tree->grid, tree->columns[i]->separator);
   }

   return new_item_objects;
}

/* Frees all the evas objects of a row */
static void _etk_tree_item_objects_free(Etk_Tree_Item_Objects *item_objects, Etk_Tree *tree)
{
   int i;

   if (!item_objects || !tree || !tree->grid)
      return;

   if (item_objects->rect_bg)
   {
      etk_widget_member_object_del(tree->grid, item_objects->rect_bg);
      evas_object_del(item_objects->rect_bg);
   }
   if (item_objects->expander)
   {
      etk_widget_member_object_del(tree->grid, item_objects->expander);
      evas_object_del(item_objects->expander);
   }

   for (i = 0; i < tree->num_cols; i++)
   {
      if (item_objects->objects[i].text_object)
      {
         etk_widget_member_object_del(tree->grid, item_objects->objects[i].text_object);
         evas_object_del(item_objects->objects[i].text_object);
      }
      if (item_objects->objects[i].image_object)
      {
         etk_widget_member_object_del(tree->grid, item_objects->objects[i].image_object);
         evas_object_del(item_objects->objects[i].image_object);
      }
   }
   free(item_objects->objects);
   free(item_objects);
}

/* Frees a row */
static void _etk_tree_row_free(Etk_Tree_Row *row)
{
   Etk_Tree_Node *n;

   if (!row)
      return;

   if (row->tree && row->cells)
   {
      int i;

      for (i = 0; i < row->tree->num_cols; i++)
      {
         if (row->tree->columns[i])
            _etk_tree_cell_clear(&row->cells[i], row->tree->columns[i]->type);
      }
      free(row->cells);
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
   if ((n = row->node.parent) && (n = n->parent))
      n->num_parent_children--;

   free(row);
}

/* Clears a cell */
static void _etk_tree_cell_clear(Etk_Tree_Cell *cell, Etk_Tree_Col_Type cell_type)
{
   if (!cell)
      return;

   switch (cell_type)
   {
      case ETK_TREE_COL_TEXT:
         free(cell->text_value);
         break;
      case ETK_TREE_COL_IMAGE:
         free(cell->image_filename_value);
         break;
      case ETK_TREE_COL_ICON_TEXT:
         free(cell->icon_text_value.icon_filename);
         free(cell->icon_text_value.text);
         break;
      default:
         break;
   }
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
