/** @file etk_tree2.c */
#include "etk_tree2.h"
#include <stdlib.h>
#include <string.h>
#include <Edje.h>
#include "etk_tree2_model.h"
#include "etk_scrolled_view.h"
#include "etk_button.h"
#include "etk_toplevel.h"
#include "etk_event.h"
#include "etk_theme.h"
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Tree2
 * @{
 */

#define TREE_GET(widget) \
   ETK_TREE2(etk_object_data_get(ETK_OBJECT(widget), "_Etk_Tree2::Tree"))

#define COL_MIN_WIDTH 24
#define COL_RESIZE_THRESHOLD 3
#define MIN_ROW_HEIGHT 12
#define DEFAULT_ROW_HEIGHT 24
#define MAX_OBJECTS_PER_CELL 5
#define CELL_HMARGINS 4
#define CELL_VMARGINS 2


typedef struct Etk_Tree2_Cell_Objects
{
   Evas_Object *objects[MAX_OBJECTS_PER_CELL];
} Etk_Tree2_Cell_Objects;

typedef struct Etk_Tree2_Row_Object
{
   Evas_Object *expander;
   Evas_Object *background;
   Etk_Tree2_Cell_Objects *cells;
   Etk_Tree2_Row *row;
} Etk_Tree2_Row_Object;

enum Etk_Tree2_Signal_Id
{
   ETK_TREE2_ALL_SELECTED_SIGNAL,
   ETK_TREE2_ALL_UNSELECTED_SIGNAL,
   ETK_TREE2_ROW_SELECTED_SIGNAL,
   ETK_TREE2_ROW_UNSELECTED_SIGNAL,
   ETK_TREE2_ROW_CLICKED_SIGNAL,
   ETK_TREE2_ROW_ACTIVATED_SIGNAL,
   ETK_TREE2_ROW_UNFOLDED_SIGNAL,
   ETK_TREE2_ROW_FOLDED_SIGNAL,
   ETK_TREE2_ROW_SHOWN_SIGNAL,
   ETK_TREE2_ROW_HIDDEN_SIGNAL,
   ETK_TREE2_NUM_SIGNALS
};

enum Etk_Tree2_Property_Id
{
   ETK_TREE2_MODE_PROPERTY,
   ETK_TREE2_MULTIPLE_SELECT_PROPERTY,
   ETK_TREE2_HEADERS_VISIBLE_PROPERTY,
   ETK_TREE2_ROWS_HEIGHT_PROPERTY
};

enum Etk_Tree2_Col_Signal_Id
{
   ETK_TREE2_COL_CELL_VALUE_CHANGED,
   ETK_TREE2_COL_NUM_SIGNALS
};

enum Etk_Tree2_Col_Property_Id
{
   ETK_TREE2_COL_TITLE_PROPERTY,
   ETK_TREE2_COL_VISIBLE_PROPERTY,
   ETK_TREE2_COL_RESIZABLE_PROPERTY,
   ETK_TREE2_COL_POSITION_PROPERTY,
   ETK_TREE2_COL_EXPAND_PROPERTY,
   ETK_TREE2_COL_WIDTH_PROPERTY,
   ETK_TREE2_COL_MIN_WIDTH_PROPERTY
};


static void _etk_tree2_constructor(Etk_Tree2 *tree);
static void _etk_tree2_destructor(Etk_Tree2 *tree);
static void _etk_tree2_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_tree2_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_tree2_size_request(Etk_Widget *widget, Etk_Size *size);
static void _etk_tree2_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_tree2_realize_cb(Etk_Object *object, void *data);

static void _etk_tree2_col_constructor(Etk_Tree2_Col *tree_col);
static void _etk_tree2_col_destructor(Etk_Tree2_Col *tree_col);
static void _etk_tree2_col_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_tree2_col_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);

static void _etk_tree2_headers_size_allocate(Etk_Tree2 *tree, Etk_Geometry geometry);
static void _etk_tree2_scroll_content_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);
static void _etk_tree2_scroll_content_scroll(Etk_Widget *widget, int x, int y);
static void _etk_tree2_scroll_content_scroll_size_get(Etk_Widget *widget, Etk_Size scrollview_size, Etk_Size scrollbar_size, Etk_Size *scroll_size);
static void _etk_tree2_grid_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

static void _etk_tree2_realize_cb(Etk_Object *object, void *data);
static void _etk_tree2_focus_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree2_unfocus_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree2_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data);
static void _etk_tree2_grid_realize_cb(Etk_Object *object, void *data);
static void _etk_tree2_grid_unrealize_cb(Etk_Object *object, void *data);

static void _etk_tree2_header_mouse_down_cb(Etk_Object *object, void *event_info, void *data);
static void _etk_tree2_header_mouse_up_cb(Etk_Object *object, void *event_info, void *data);
static void _etk_tree2_header_mouse_move_cb(Etk_Object *object, void *event_info, void *data);
static void _etk_tree2_header_mouse_in_cb(Etk_Object *object, void *event_info, void *data);
static void _etk_tree2_header_mouse_out_cb(Etk_Object *object, void *event_info, void *data);

static void _etk_tree2_row_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree2_row_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree2_row_expander_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

static void _etk_tree2_purge_job(void *data);
static void _etk_tree2_purge(Etk_Tree2 *tree);
static void _etk_tree2_row_move_to_purge_pool(Etk_Tree2_Row *row);

/* TODO: static void _etk_tree2_sort(Etk_Tree2 *tree); */

static void _etk_tree2_col_realize(Etk_Tree2 *tree, int col_nth);
static Etk_Tree2_Col *etk_tree2_col_to_resize_get(Etk_Tree2_Col *col, int x);

static Etk_Tree2_Row *_etk_tree2_row_next_to_render_get(Etk_Tree2_Row *row, int *depth);
static void _etk_tree2_row_fields_set_valist_full(Etk_Tree2_Row *row, va_list args, Etk_Bool emit_signal);
static Etk_Tree2_Row_Object *_etk_tree2_row_object_create(Etk_Tree2 *tree);
static void _etk_tree2_row_object_destroy(Etk_Tree2 *tree, Etk_Tree2_Row_Object *row_object);

static void _etk_tree2_row_select(Etk_Tree2 *tree, Etk_Tree2_Row *row, Etk_Modifiers modifiers);


static Etk_Signal *_etk_tree2_signals[ETK_TREE2_NUM_SIGNALS];
static Etk_Signal *_etk_tree2_col_signals[ETK_TREE2_COL_NUM_SIGNALS];

/* TODO: better doc of row_next_get()... (with a note about deleted rows...) */


/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Tree2
 * @return Returns the type of an Etk_Tree2
 */
Etk_Type *etk_tree2_type_get()
{
   static Etk_Type *tree2_type = NULL;

   if (!tree2_type)
   {
      tree2_type = etk_type_new("Etk_Tree2", ETK_WIDGET_TYPE, sizeof(Etk_Tree2),
         ETK_CONSTRUCTOR(_etk_tree2_constructor), ETK_DESTRUCTOR(_etk_tree2_destructor));
      
      _etk_tree2_signals[ETK_TREE2_ALL_SELECTED_SIGNAL] = etk_signal_new("all_selected",
         tree2_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_tree2_signals[ETK_TREE2_ALL_UNSELECTED_SIGNAL] = etk_signal_new("all_unselected",
         tree2_type, -1, etk_marshaller_VOID__VOID, NULL, NULL);
      _etk_tree2_signals[ETK_TREE2_ROW_SELECTED_SIGNAL] = etk_signal_new("row_selected",
         tree2_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree2_signals[ETK_TREE2_ROW_UNSELECTED_SIGNAL] = etk_signal_new("row_unselected",
         tree2_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree2_signals[ETK_TREE2_ROW_CLICKED_SIGNAL] = etk_signal_new("row_clicked",
         tree2_type, -1, etk_marshaller_VOID__POINTER_POINTER, NULL, NULL);
      _etk_tree2_signals[ETK_TREE2_ROW_ACTIVATED_SIGNAL] = etk_signal_new("row_activated",
         tree2_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree2_signals[ETK_TREE2_ROW_UNFOLDED_SIGNAL] = etk_signal_new("row_unfolded",
         tree2_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree2_signals[ETK_TREE2_ROW_FOLDED_SIGNAL] = etk_signal_new("row_folded",
         tree2_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree2_signals[ETK_TREE2_ROW_SHOWN_SIGNAL] = etk_signal_new("row_shown",
         tree2_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      _etk_tree2_signals[ETK_TREE2_ROW_HIDDEN_SIGNAL] = etk_signal_new("row_hidden",
         tree2_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);

      etk_type_property_add(tree2_type, "mode", ETK_TREE2_MODE_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(ETK_TREE2_MODE_LIST));
      etk_type_property_add(tree2_type, "multiple_select", ETK_TREE2_MULTIPLE_SELECT_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree2_type, "headers_visible", ETK_TREE2_HEADERS_VISIBLE_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree2_type, "rows_height", ETK_TREE2_ROWS_HEIGHT_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(DEFAULT_ROW_HEIGHT));
      
      tree2_type->property_set = _etk_tree2_property_set;
      tree2_type->property_get = _etk_tree2_property_get;      
   }

   return tree2_type;
}

/**
 * @internal
 * @brief Gets the type of an Etk_Tree2_Col
 * @return Returns the type of an Etk_Tree2_Col
 */
Etk_Type *etk_tree2_col_type_get()
{
   static Etk_Type *tree_col_type = NULL;

   if (!tree_col_type)
   {
      tree_col_type = etk_type_new("Etk_Tree2_Col", ETK_OBJECT_TYPE, sizeof(Etk_Tree2_Col),
         ETK_CONSTRUCTOR(_etk_tree2_col_constructor), ETK_DESTRUCTOR(_etk_tree2_col_destructor));

      _etk_tree2_col_signals[ETK_TREE2_COL_CELL_VALUE_CHANGED] = etk_signal_new("cell_value_changed",
         tree_col_type, -1, etk_marshaller_VOID__POINTER, NULL, NULL);
      
      etk_type_property_add(tree_col_type, "title", ETK_TREE2_COL_TITLE_PROPERTY,
         ETK_PROPERTY_STRING, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_string(NULL));
      etk_type_property_add(tree_col_type, "visible", ETK_TREE2_COL_VISIBLE_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_col_type, "resizable", ETK_TREE2_COL_RESIZABLE_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_TRUE));
      etk_type_property_add(tree_col_type, "position", ETK_TREE2_COL_POSITION_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(0));
      etk_type_property_add(tree_col_type, "expand", ETK_TREE2_COL_EXPAND_PROPERTY,
         ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_bool(ETK_FALSE));
      etk_type_property_add(tree_col_type, "width", ETK_TREE2_COL_WIDTH_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(COL_MIN_WIDTH));
      etk_type_property_add(tree_col_type, "min_width", ETK_TREE2_COL_MIN_WIDTH_PROPERTY,
         ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE, etk_property_value_int(COL_MIN_WIDTH));
      
      tree_col_type->property_set = _etk_tree2_col_property_set;
      tree_col_type->property_get = _etk_tree2_col_property_get;
   }

   return tree_col_type;
}

/**
 * @brief Creates a new tree
 * @return Returns the new tree
 */
Etk_Widget *etk_tree2_new()
{
   return etk_widget_new(ETK_TREE2_TYPE, "theme_group", "tree",
      "focusable", ETK_TRUE, "focus_on_click", ETK_TRUE, NULL);
}

/**
 * @brief Sets the mode of the tree. The tree must not be built
 * @param tree a tree
 * @param mode the mode the tree should use (ETK_TREE2_MODE_LIST or ETK_TREE2_MODE_TREE)
 */
void etk_tree2_mode_set(Etk_Tree2 *tree, Etk_Tree2_Mode mode)
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
 * @return Returns the mode of tree (ETK_TREE2_MODE_LIST or ETK_TREE2_MODE_TREE)
 */
Etk_Tree2_Mode etk_tree2_mode_get(Etk_Tree2 *tree)
{
   if (!tree)
      return ETK_TREE2_MODE_LIST;
   return tree->mode;
}

/**
 * @brief Sets whether the user can select several rows of the tree
 * @param tree a tree
 * @param multiple_select ETK_TRUE to allow the user to select several rows, ETK_FALSE otherwise
 */
void etk_tree2_multiple_select_set(Etk_Tree2 *tree, Etk_Bool multiple_select)
{
   if (!tree || tree->multiple_select == multiple_select)
      return;

   if (!multiple_select)
      etk_tree2_unselect_all(tree);
   tree->multiple_select = multiple_select;
   etk_object_notify(ETK_OBJECT(tree), "multiple_select");
}

/**
 * @brief Gets whether or not several rows can be selected in the tree
 * @param tree a tree
 * @return ETK_TRUE if several rows can be selected, ETK_FALSE otherwise
 */
Etk_Bool etk_tree2_multiple_select_get(Etk_Tree2 *tree)
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
void etk_tree2_headers_visible_set(Etk_Tree2 *tree, Etk_Bool headers_visible)
{
   if (!tree || (tree->headers_visible == headers_visible))
      return;

   tree->headers_visible = headers_visible;
   etk_object_notify(ETK_OBJECT(tree), "headers_visible");
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Gets whether or not the column-headers are visible
 * @param tree a tree
 * @return Returns ETK_TRUE if the column-headers are visible, ETK_FALSE otherwise
 */
Etk_Bool etk_tree2_headers_visible_get(Etk_Tree2 *tree)
{
   if (!tree)
      return ETK_FALSE;
   return tree->headers_visible;
}

/**
 * @brief Sets the height of the rows of the tree
 * @param tree a tree
 * @param rows_height the rows height to set. The minimum value is 12
 */
void etk_tree2_rows_height_set(Etk_Tree2 *tree, int rows_height)
{
   if (!tree)
      return;
   
   rows_height = ETK_MAX(rows_height, MIN_ROW_HEIGHT);
   if (tree->rows_height != rows_height)
   {
      tree->rows_height = rows_height;
      etk_object_notify(ETK_OBJECT(tree), "rows_height");
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(tree->scroll_content), NULL);
      etk_widget_redraw_queue(ETK_WIDGET(tree));
   }
}

/**
 * @brief Gets the height of the rows of the tree
 * @param tree a tree
 * @return Returns the height of the rows of the tree
 */
int etk_tree2_rows_height_get(Etk_Tree2 *tree)
{
   if (!tree)
      return DEFAULT_ROW_HEIGHT;
   return tree->rows_height;
}

/**
 * @brief Builds the tree. You have to call this function after you have added all the columns
 * and before you add the rows to the tree. Once the tree is built, it can not be unbuilt,
 * and you could not add new columns anymore
 * @param tree a tree
 */
void etk_tree2_build(Etk_Tree2 *tree)
{
   if (!tree || tree->built)
      return;
   tree->built = ETK_TRUE;
}

/**
 * @brief Freezes the tree: the won't be updated until it is thawed. It is useful to add
 * a lot (> 1000) of rows efficiently. To add a few rows, freeze/thaw() is not necessary
 * @param tree a tree
 * @see etk_tree2_thaw()
 */
void etk_tree2_freeze(Etk_Tree2 *tree)
{
   if (!tree || tree->frozen)
      return;
   tree->frozen = ETK_TRUE;
}

/**
 * @brief Thaws the tree: the tree will be updated the tree if it was frozen
 * @param tree a tree
 * @see etk_tree2_freeze()
 */
void etk_tree2_thaw(Etk_Tree2 *tree)
{
   if (!tree || !tree->frozen)
      return;
   
   tree->frozen = ETK_FALSE;
   etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(tree->scroll_content), NULL);
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Gets the scrolled view of the tree.
 * It can be used to change the scrollbars' policy, or to get the scroll-value
 * @param tree a tree
 * @return Returns the scrolled view of the tree
 */
Etk_Scrolled_View *etk_tree2_scrolled_view_get(Etk_Tree2 *tree)
{
   if (!tree)
      return NULL;
   return ETK_SCROLLED_VIEW(tree->scrolled_view);
}

/**
 * @brief Inserts a new column into a tree
 * @param tree a tree
 * @param title the tile of the column
 * @param model the model to use for the column
 * @param width the requested width of the column. It won't be necessary the visible width
 * of the column since it can be expanded to fit the available space
 * @return Returns the new column
 */
Etk_Tree2_Col *etk_tree2_col_new(Etk_Tree2 *tree, const char *title, Etk_Tree2_Model *model, int width)
{
   Etk_Tree2_Col *new_col;
   Etk_Widget *new_header;

   if (!tree || !model)
      return NULL;
   if (tree->built)
   {
      ETK_WARNING("The tree is built, you can not add a new column");
      return NULL;
   }
   if (model->col)
   {
      ETK_WARNING("The tree-model to use for that column is already used by another column");
      return NULL;
   }

   tree->columns = realloc(tree->columns, sizeof(Etk_Tree2_Col *) * (tree->num_cols + 1));
   new_col = ETK_TREE2_COL(etk_object_new(ETK_TREE2_COL_TYPE, "title", title,
      "width", width, "visible", ETK_TRUE, "resizable", ETK_TRUE, NULL));
   tree->columns[tree->num_cols] = new_col;

   new_col->id = tree->num_cols;
   new_col->tree = tree;
   new_col->model = model;
   new_col->model->col = new_col;
   new_col->position = tree->num_cols;

   /* Creates the header widget */
   new_header = etk_widget_new(ETK_BUTTON_TYPE, "theme_group", "header","theme_parent", tree,
      "label", title, "xalign", 0.0, "internal", ETK_TRUE, NULL);
   if (tree->tree_contains_headers)
      etk_widget_parent_set(new_header, ETK_WIDGET(tree));
   else
      etk_widget_parent_set(new_header, tree->scroll_content);
   new_col->header = new_header;
   
   etk_signal_connect("mouse_down", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree2_header_mouse_down_cb), new_col);
   etk_signal_connect("mouse_up", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree2_header_mouse_up_cb), new_col);
   etk_signal_connect("mouse_move", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree2_header_mouse_move_cb), new_col);
   etk_signal_connect("mouse_in", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree2_header_mouse_in_cb), new_col);
   etk_signal_connect("mouse_out", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree2_header_mouse_out_cb), new_col);

   tree->num_cols++;
   _etk_tree2_col_realize(tree, tree->num_cols - 1);

   return new_col;
}

/**
 * @brief Gets the number of columns of the tree
 * @param tree a tree
 * @return Returns the number of columns of the tree
 */
int etk_tree2_num_cols_get(Etk_Tree2 *tree)
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
Etk_Tree2_Col *etk_tree2_nth_col_get(Etk_Tree2 *tree, int nth)
{
   if (!tree || nth < 0 || nth >= tree->num_cols)
      return NULL;
   return tree->columns[nth];
}

/**
 * @brief Sets the title of the column
 * @param col a column of a tree
 * @param title the title to set
 */
void etk_tree2_col_title_set(Etk_Tree2_Col *col, const char *title)
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
const char *etk_tree2_col_title_get(Etk_Tree2_Col *col)
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
void etk_tree2_col_width_set(Etk_Tree2_Col *col, int width)
{
   if (!col || col->width == width)
      return;
   
   col->width = ETK_MAX(width, col->min_width);
   etk_object_notify(ETK_OBJECT(col), "width");
   
   if (col->tree)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(col->tree->scroll_content), NULL);
      etk_widget_redraw_queue(ETK_WIDGET(col->tree));
   }
}

/**
 * @brief Gets the width of the column. TODO: (what if the col expands??)
 * @param col a column of a tree
 * @return Returns the width of the column
 */
int etk_tree2_col_width_get(Etk_Tree2_Col *col)
{
   if (!col)
      return 0;
   return col->width;
}

/**
 * @brief Sets the minimum width of the column. The column can not be smaller than this width
 * @param col a column of a tree
 * @param min_width the minimum width to set. -1 to make etk calculate the min_width
 */
void etk_tree2_col_min_width_set(Etk_Tree2_Col *col, int min_width)
{
   if (!col || (col->min_width == min_width))
      return;
   
   col->min_width = ETK_MAX(min_width, COL_MIN_WIDTH);
   if (col->width < col->min_width)
      etk_tree2_col_width_set(col, col->min_width);
   
   etk_object_notify(ETK_OBJECT(col), "min_width");
}

/**
 * @brief Gets the minimum width of the column
 * @param col a column of a tree
 * @return Returns the minimum width of the column
 */
int etk_tree2_col_min_width_get(Etk_Tree2_Col *col)
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
void etk_tree2_col_resizable_set(Etk_Tree2_Col *col, Etk_Bool resizable)
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
Etk_Bool etk_tree2_col_resizable_get(Etk_Tree2_Col *col)
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
void etk_tree2_col_expand_set(Etk_Tree2_Col *col, Etk_Bool expand)
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
Etk_Bool etk_tree2_col_expand_get(Etk_Tree2_Col *col)
{
   if (!col)
      return ETK_FALSE;
   return col->expand;
}

/**
 * @brief Sets whether or not the column is visible
 * @param col a column of a tree
 * @param visible ETK_TRUE to show the column, ETK_FALSE to hide it
 */
void etk_tree2_col_visible_set(Etk_Tree2_Col *col, Etk_Bool visible)
{
   if (!col || (col->visible == visible))
      return;
   
   col->visible = visible;
   etk_object_notify(ETK_OBJECT(col), "visible");
   
   if (col->tree)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(col->tree->scroll_content), NULL);
      etk_widget_redraw_queue(ETK_WIDGET(col->tree));
   }
}

/**
 * @brief Gets whether or not the column is visible
 * @param col a column of a tree
 * @return Returns ETK_TRUE if the column is visible
 */
Etk_Bool etk_tree2_col_visible_get(Etk_Tree2_Col *col)
{
   if (!col)
      return ETK_FALSE;
   return col->visible;
}

/**
 * @brief Changes the position of the column
 * @param col a column of a tree
 * @param position the new position the column should take (0 is the first column on the left of the tree,
 * "etk_tree2_num_cols_get(tree) - 1" is the last one on the right)
 */
void etk_tree2_col_position_set(Etk_Tree2_Col *col, int position)
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
   etk_widget_redraw_queue(ETK_WIDGET(col->tree));
}

/**
 * @brief Gets the position of the column (0 is the first column on the left of the tree,
 * "etk_tree2_num_cols_get(tree) - 1" is the last one on the right)
 * @param col a column of a tree
 * @return Returns the position of the column
 */
int etk_tree2_col_position_get(Etk_Tree2_Col *col)
{
   if (!col)
      return 0;
   return col->position;
}

/**
 * @brief Sets the sorting function of a column. This function will be called when
 * the header of the column is clicked
 * @param col a column of a tree
 * @param compare_cb the function to call to compare two rows. It should return a negative
 * value if the cell of "row1" has a lower value than the cell of "row2", 0 if they have the
 * same value, and a position value if the cell of "row2" has a greater value than the cell of "row1"
 * @param data a pointer that will be passed to @a compare_cb when it is called
 */
void etk_tree2_col_sort_set(Etk_Tree2_Col *col, int (*compare_cb)(Etk_Tree2 *tree, Etk_Tree2_Row *row1, Etk_Tree2_Row *row2, Etk_Tree2_Col *col, void *data), void *data)
{  
   if (!col)
      return;
   
   col->sort.compare_cb = compare_cb;
   col->sort.data = data;
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
Etk_Tree2_Row *etk_tree2_row_prepend(Etk_Tree2 *tree, Etk_Tree2_Row *parent, ...)
{
   Etk_Tree2_Row *row;
   va_list args;
   
   if (!tree)
      return NULL;
   
   va_start(args, parent);
   row = etk_tree2_row_insert_valist(tree, parent, NULL, args);
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
Etk_Tree2_Row *etk_tree2_row_append(Etk_Tree2 *tree, Etk_Tree2_Row *parent, ...)
{
   Etk_Tree2_Row *row;
   va_list args;
   
   if (!tree)
      return NULL;
   
   va_start(args, parent);
   if (!parent)
      parent = &tree->root;
   row = etk_tree2_row_insert_valist(tree, parent, parent->last_child, args);
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
Etk_Tree2_Row *etk_tree2_row_insert(Etk_Tree2 *tree, Etk_Tree2_Row *parent, Etk_Tree2_Row *after, ...)
{
   Etk_Tree2_Row *row;
   va_list args;
   
   if (!tree)
      return NULL;
   
   va_start(args, after);
   row = etk_tree2_row_insert_valist(tree, parent, after, args);
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
 * @param args an "Etk_Tree_Col *" followed by the value of the cell, then any number of "Etk_Tree_Col *"/Value pairs,
 * and terminated by NULL. Note that, according to the model used by the column, a cell value can use several parameters
 * @return Returns the new row, or NULL on failure
 */
Etk_Tree2_Row *etk_tree2_row_insert_valist(Etk_Tree2 *tree, Etk_Tree2_Row *parent, Etk_Tree2_Row *after, va_list args)
{
   Etk_Tree2_Row *new_row;
   va_list args2;
   int i;

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
   
   new_row = malloc(sizeof(Etk_Tree2_Row));
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
      if (!parent->first_child)
         parent->first_child = new_row;
      else
         parent->first_child->prev = new_row;
      if (!parent->last_child)
         parent->last_child = new_row;
   }
   parent->num_children++;
   for ( ; parent && parent->unfolded; parent = parent->parent)
      parent->num_visible_children++;
   tree->total_rows++;

   /* Initializes the data of the row's cells */
   new_row->cells_data = malloc(sizeof(void *) * tree->num_cols);
   for (i = 0; i < tree->num_cols; i++)
   {
      new_row->cells_data[i] = calloc(1, tree->columns[i]->model->cell_data_size);
      if (tree->columns[i]->model->cell_data_init)
         tree->columns[i]->model->cell_data_init(tree->columns[i]->model, new_row->cells_data[i]);
   }
   va_copy(args2, args);
   _etk_tree2_row_fields_set_valist_full(new_row, args2, ETK_FALSE);
   va_end(args2);
   
   
   if (!tree->frozen)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(tree->scroll_content), NULL);
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
void etk_tree2_row_delete(Etk_Tree2_Row *row)
{
   if (!row || row->delete_me)
      return;
   
   _etk_tree2_row_move_to_purge_pool(row);
   
   if (!row->tree->frozen)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(row->tree->scroll_content), NULL);
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
   }
}

/**
 * @brief Deletes all the rows of the tree. Note that the rows will just be marked as deleted. They will be effectively
 * deleted during the next iteration of Ecore's main loop. Thus, you can still manipulate safely the rows immediately
 * after a clear.
 */
void etk_tree2_clear(Etk_Tree2 *tree)
{
   if (!tree)
      return;
   
   while (tree->root.first_child)
   _etk_tree2_row_move_to_purge_pool(tree->root.first_child);
   
   etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(tree->scroll_content), NULL);
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Sets the values of the cells of the row
 * @param row a row of the tree
 * @param ... an "Etk_Tree_Col *" followed by the value of the cell, then any number of "Etk_Tree_Col *"/Value pairs,
 * and terminated by NULL. Note that, according to the model used by the column, a cell value can use several parameters
 */
void etk_tree2_row_fields_set(Etk_Tree2_Row *row, ...)
{
   va_list args;
   
   if (!row)
      return;
   
   va_start(args, row);
   etk_tree2_row_fields_set_valist(row, args);
   va_end(args);
}

/**
 * @brief Sets the values of the cells of the row
 * @param row a row of the tree
 * @param args an "Etk_Tree_Col *" followed by the value of the cell,
 * then any number of "Etk_Tree_Col *"/Value pairs, and terminated by NULL.
 * Note that some models may require several parameters for the cell value
 */
void etk_tree2_row_fields_set_valist(Etk_Tree2_Row *row, va_list args)
{
   va_list args2;
   
   if (!row)
      return;
   
   va_copy(args2, args);
   _etk_tree2_row_fields_set_valist_full(row, args2, ETK_TRUE);
   va_end(args2);
}

/**
 * @brief Gets the values of the cells of the row
 * @param row a row of the tree
 * @param ... an "Etk_Tree_Col *" followed by the location where to store the value of the cell,
 * then any number of "Etk_Tree_Col *"/Location pairs, and terminated by NULL.
 * Note that some models may require several locations to store the cell value
 */
void etk_tree2_row_fields_get(Etk_Tree2_Row *row, ...)
{
   va_list args;
   
   if (!row)
      return;
   
   va_start(args, row);
   etk_tree2_row_fields_get_valist(row, args);
   va_end(args);
}

/**
 * @brief Gets the values of the cells of the row
 * @param row a row of the tree
 * @param args an "Etk_Tree_Col *" followed by the location where to store the value of the cell,
 * then any number of "Etk_Tree_Col *"/Location pairs, and terminated by NULL.
 * Note that some models may require several locations to store the cell value
 */
void etk_tree2_row_fields_get_valist(Etk_Tree2_Row *row, va_list args)
{
   Etk_Tree2_Col *col;
   va_list args2;
   
   if (!row)
      return;
   
   va_copy(args2, args);
   while ((col = va_arg(args, Etk_Tree2_Col *)))
   {
      if (col->model->cell_data_get)
         col->model->cell_data_get(col->model, row->cells_data[col->id], &args);
   }
   va_end(args2);
}

/**
 * @brief Sets the user data associated to the row. The data could be retrieved later with etk_tree2_row_data_get()
 * @param row a row of the tree
 * @param data the data to associate to the row
 * @note This is equivalent to etk_tree2_row_data_set_full(row, data, NULL);
 */
void  etk_tree2_row_data_set(Etk_Tree2_Row *row, void *data)
{
   etk_tree2_row_data_set_full(row, data, NULL);
}

/**
 * @brief Sets the user data associated to the row. The data could be retrieved later with etk_tree2_row_data_get().
 * You can also set a function to call to free automatically the data when the row is deleted
 * @param row a row of the tree
 * @param data the data to associate to the row
 * @param free_cb a function to call to free the data automatically when the row is deleted
 */
void  etk_tree2_row_data_set_full(Etk_Tree2_Row *row, void *data, void (*free_cb)(void *data))
{
   if (!row)
      return;
   
   if (row->data && row->data_free_cb)
      row->data_free_cb(row->data);
   
   row->data = data;
   row->data_free_cb = free_cb;
}

/**
 * @brief Gets the user data associated to the row (previously set with etk_tree2_row_data_set()).
 * @param row a row of the tree
 * @return Returns the user data associated to the row
 */
void *etk_tree2_row_data_get(Etk_Tree2_Row *row)
{
   if (!row)
      return NULL;
   return row->data;
}

/**
 * @brief Selects all the rows of the tree
 * @param tree a tree
 * @note When you call etk_tree2_select_all(), for performance reasons, the signal "row_selected" is not emitted for
 * each row of the tree. Only the signal "all_selected" is emitted on the tree.
 */
void etk_tree2_select_all(Etk_Tree2 *tree)
{
   Etk_Tree2_Row *row;
   
   if (!tree)
      return;
   
   for (row = tree->root.first_child; row; row = etk_tree2_row_walk_next(row, ETK_TRUE))
      row->selected = ETK_TRUE;
   
   etk_signal_emit(_etk_tree2_signals[ETK_TREE2_ALL_SELECTED_SIGNAL], ETK_OBJECT(tree), NULL);
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Unselects all the rows of the tree
 * @param tree a tree
 * @note When you call etk_tree2_unselect_all(), for performance reasons, the signal "row_unselected" is not emitted for
 * each row of the tree. Only the signal "all_unselected" is emitted on the tree.
 */
void etk_tree2_unselect_all(Etk_Tree2 *tree)
{
   Etk_Tree2_Row *row;
   
   if (!tree)
      return;
   
   for (row = tree->root.first_child; row; row = etk_tree2_row_walk_next(row, ETK_TRUE))
      row->selected = ETK_FALSE;
   
   etk_signal_emit(_etk_tree2_signals[ETK_TREE2_ALL_SELECTED_SIGNAL], ETK_OBJECT(tree), NULL);
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/**
 * @brief Selects a row of the tree
 * @param row the row to select
 */
void etk_tree2_row_select(Etk_Tree2_Row *row)
{
   if (!row || row->selected)
      return;
   _etk_tree2_row_select(row->tree, row, ETK_MODIFIER_NONE);
}

/**
 * @brief Unselects a row of the tree
 * @param row the row to unselect
 */
void etk_tree2_row_unselect(Etk_Tree2_Row *row)
{
   if (!row || !row->selected)
      return;
   
   row->selected = ETK_FALSE;
   etk_signal_emit(_etk_tree2_signals[ETK_TREE2_ROW_UNSELECTED_SIGNAL], ETK_OBJECT(row->tree), NULL, row);
   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
}

/**
 * @brief Gets whether or not the row is selected
 * @param row a row
 * @return Returns ETK_TRUE if the row is selected, ETK_FALSE otherwise
 */
Etk_Bool etk_tree2_row_is_selected(Etk_Tree2_Row *row)
{
   if (!row)
      return ETK_FALSE;
   return row->selected;
}

/**
 * @brief Folds a row of the tree: if the tree is in the "tree" mode, the children of the row won't be visible anymore
 * @param row the row to fold
 */
void etk_tree2_row_fold(Etk_Tree2_Row *row)
{
   Etk_Tree2_Row *parent;
   
   if (!row || !row->unfolded || row->tree->mode != ETK_TREE2_MODE_TREE)
      return;
   
   for (parent = row->parent; parent && parent->unfolded; parent = parent->parent)
      parent->num_visible_children -= row->num_visible_children;
   row->num_visible_children = 0;
   row->unfolded = ETK_FALSE;
   
   etk_signal_emit(_etk_tree2_signals[ETK_TREE2_ROW_FOLDED_SIGNAL], ETK_OBJECT(row->tree), NULL, row);
   if (!row->tree->frozen)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(row->tree->scroll_content), NULL);
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
   }
}

/**
 * @brief Unfolds a row of the tree: if the tree is in the "tree" mode, the children of the row will be visible
 * @param row the row to unfold
 */
void etk_tree2_row_unfold(Etk_Tree2_Row *row)
{
   Etk_Tree2_Row *parent, *child;
   
   if (!row || row->unfolded || row->tree->mode != ETK_TREE2_MODE_TREE)
      return;
   
   for (child = row->first_child; child; child = child->next)
      row->num_visible_children += 1 + child->num_visible_children;
   for (parent = row->parent; parent && parent->unfolded; parent = parent->parent)
      parent->num_visible_children += row->num_visible_children;
   row->unfolded = ETK_TRUE;
   
   etk_signal_emit(_etk_tree2_signals[ETK_TREE2_ROW_UNFOLDED_SIGNAL], ETK_OBJECT(row->tree), NULL, row);
   if (!row->tree->frozen)
   {
      etk_signal_emit_by_name("scroll_size_changed", ETK_OBJECT(row->tree->scroll_content), NULL);
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
   }
}

/**
 * @brief Gets whether the row is folded or not
 * @param row a row
 * @return Returns ETK_TRUE if the row is folded, ETK_FALSE otherwise
 */
Etk_Bool etk_tree2_row_is_folded(Etk_Tree2_Row *row)
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
Etk_Tree2_Row *etk_tree2_first_row_get(Etk_Tree2 *tree)
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
Etk_Tree2_Row *etk_tree2_last_row_get(Etk_Tree2 *tree)
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
Etk_Tree2_Row *etk_tree2_row_parent_get(Etk_Tree2_Row *row)
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
Etk_Tree2_Row *etk_tree2_row_first_child_get(Etk_Tree2_Row *row)
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
Etk_Tree2_Row *etk_tree2_row_last_child_get(Etk_Tree2_Row *row)
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
 * level as the specified row. So if some rows of the tree have children, you'll have to use etk_tree2_row_walk_prev()
 * if you want to walk through all the rows of the tree.
 * @see etk_tree2_row_walk_prev()
 */
Etk_Tree2_Row *etk_tree2_row_prev_get(Etk_Tree2_Row *row)
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
 * level as the specified row. So if some rows of the tree have children, you'll have to use etk_tree2_row_walk_next()
 * if you want to walk through all the rows of the tree.
 * @see etk_tree2_row_walk_next()
 */
Etk_Tree2_Row *etk_tree2_row_next_get(Etk_Tree2_Row *row)
{
   if (!row)
      return NULL;
   return row->next;
}

/**
 * @brief Walks to the previous "visible" row. Unlike etk_tree2_row_prev_get(), etk_tree2_row_walk_prev() can return a
 * row that is not on the same level as the specified row. It is useful if you want to walk easily through all the rows
 * of the tree.
 * @param row a row
 * @param include_folded if @a include_folded is ETK_TRUE, the function will return a row, even if this row is a child
 * of a folded row
 * @return Returns the previous "visible" row
 * @note if the tree is in the list mode, this is equivalent to etk_tree2_row_prev_get(row)
 */
Etk_Tree2_Row *etk_tree2_row_walk_prev(Etk_Tree2_Row *row, Etk_Bool include_folded)
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
 * @brief Walks to the next "visible" row. Unlike etk_tree2_row_next_get(), etk_tree2_row_walk_next() can return a row
 * that is not on the same level as the specified row. It is useful if you want to walk easily through all the rows of
 * the tree.
 * @param row a row
 * @param include_folded if @a include_folded is ETK_TRUE, the function will return a row, even if this row is a child
 * of a folded row
 * @return Returns the next "visible" row
 * @note if the tree is in the list mode, this is equivalent to etk_tree2_row_next_get(row)
 */
Etk_Tree2_Row *etk_tree2_row_walk_next(Etk_Tree2_Row *row, Etk_Bool include_folded)
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

/**************************
 *
 * Etk specific functions
 *
 **************************/

/**************************
 * Tree
 **************************/

/* Initializes the tree */
static void _etk_tree2_constructor(Etk_Tree2 *tree)
{
   if (!tree)
      return;
   
   tree->scrolled_view = etk_scrolled_view_new();
   etk_widget_theme_parent_set(tree->scrolled_view, ETK_WIDGET(tree));
   etk_widget_parent_set(tree->scrolled_view, ETK_WIDGET(tree));
   etk_widget_internal_set(tree->scrolled_view, ETK_TRUE);
   etk_widget_repeat_mouse_events_set(tree->scrolled_view, ETK_TRUE);
   etk_widget_show(tree->scrolled_view);
   
   tree->scroll_content = etk_widget_new(ETK_WIDGET_TYPE, "repeat_mouse_events", ETK_TRUE,
      "internal", ETK_TRUE, "visible", ETK_TRUE, NULL);
   etk_container_add(ETK_CONTAINER(tree->scrolled_view), tree->scroll_content);
   etk_object_data_set(ETK_OBJECT(tree->scroll_content), "_Etk_Tree2::Tree", tree);
   tree->scroll_content->size_allocate = _etk_tree2_scroll_content_size_allocate;
   tree->scroll_content->scroll = _etk_tree2_scroll_content_scroll;
   tree->scroll_content->scroll_size_get = _etk_tree2_scroll_content_scroll_size_get;
   
   tree->grid = etk_widget_new(ETK_WIDGET_TYPE, "theme_group", "grid", "theme_parent", tree,
       "repeat_mouse_events", ETK_TRUE, "internal", ETK_TRUE, "visible", ETK_TRUE, NULL);
   etk_widget_parent_set(tree->grid, tree->scroll_content);
   etk_object_data_set(ETK_OBJECT(tree->grid), "_Etk_Tree2::Tree", tree);
   tree->grid->size_allocate = _etk_tree2_grid_size_allocate;
   etk_signal_connect("realize", ETK_OBJECT(tree->grid),
      ETK_CALLBACK(_etk_tree2_grid_realize_cb), NULL);
   etk_signal_connect("unrealize", ETK_OBJECT(tree->grid),
      ETK_CALLBACK(_etk_tree2_grid_unrealize_cb), NULL);
   
   tree->num_cols = 0;
   tree->columns = NULL;
   tree->col_to_resize = NULL;
   tree->headers_visible = ETK_TRUE;
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
   
   tree->rows_height = DEFAULT_ROW_HEIGHT;
   tree->scroll_x = 0;
   tree->scroll_y = 0;
   
   tree->purge_job = NULL;
   tree->mode = ETK_TREE2_MODE_LIST;
   tree->multiple_select = ETK_FALSE;
   tree->tree_contains_headers = ETK_TRUE;
   tree->col_resize_pointer_set = ETK_FALSE;
   tree->frozen = ETK_FALSE;
   tree->built = ETK_FALSE;
   
   ETK_WIDGET(tree)->size_request = _etk_tree2_size_request;
   ETK_WIDGET(tree)->size_allocate = _etk_tree2_size_allocate;
   
   etk_signal_connect("realize", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree2_realize_cb), NULL);
   etk_signal_connect("focus", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree2_focus_cb), NULL);
   etk_signal_connect("unfocus", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree2_unfocus_cb), NULL);
   etk_signal_connect("key_down", ETK_OBJECT(tree), ETK_CALLBACK(_etk_tree2_key_down_cb), NULL);
}

/* TODO: last selected when deleted... */

/* Destroys the tree */
static void _etk_tree2_destructor(Etk_Tree2 *tree)
{
   if (!tree)
      return;

   /* TODO: more efficient cleaning?? */
   etk_tree2_clear(tree);
   _etk_tree2_purge(tree);
   
   if (tree->purge_job)
      ecore_job_del(tree->purge_job);

   free(tree->columns);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_tree2_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Tree2 *tree;

   if (!(tree = ETK_TREE2(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TREE2_MODE_PROPERTY:
         etk_tree2_mode_set(tree, etk_property_value_int_get(value));
         break;
      case ETK_TREE2_MULTIPLE_SELECT_PROPERTY:
         etk_tree2_multiple_select_set(tree, etk_property_value_bool_get(value));
         break;
      case ETK_TREE2_HEADERS_VISIBLE_PROPERTY:
         etk_tree2_headers_visible_set(tree, etk_property_value_bool_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_tree2_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Tree2 *tree;

   if (!(tree = ETK_TREE2(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TREE2_MODE_PROPERTY:
         etk_property_value_int_set(value, tree->mode);
         break;
      case ETK_TREE2_MULTIPLE_SELECT_PROPERTY:
         etk_property_value_bool_set(value, tree->multiple_select);
         break;
      case ETK_TREE2_HEADERS_VISIBLE_PROPERTY:
         etk_property_value_bool_set(value, tree->headers_visible);
         break;
      default:
         break;
   }
}

/* Calculates the ideal size of the tree */
static void _etk_tree2_size_request(Etk_Widget *widget, Etk_Size *size)
{
   Etk_Tree2 *tree;
   Etk_Size header_size;
   int headers_height = 0;
   int i;
   
   if (!(tree = ETK_TREE2(widget)) || !size)
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
static void _etk_tree2_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Tree2 *tree;
   Etk_Size header_size;
   Etk_Geometry view_geometry;
   int max_header_height;
   int i;
   
   if (!(tree = ETK_TREE2(widget)))
      return;
   
   view_geometry = geometry;
   max_header_height = 0;
   
   /* Allocate size for the scrolled view */
   if (tree->tree_contains_headers)
   {
      if (tree->headers_visible)
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
   }
   etk_widget_size_allocate(tree->scrolled_view, view_geometry);
   
   /* Allocate size for the headers */
   if (tree->tree_contains_headers)
   {
      geometry.h = max_header_height;
      _etk_tree2_headers_size_allocate(tree, geometry);
   }
}

/**************************
 * Tree Col
 **************************/

/* Initializes the tree column */
static void _etk_tree2_col_constructor(Etk_Tree2_Col *tree_col)
{
   if (!tree_col)
      return;

   tree_col->tree = NULL;
   tree_col->model = NULL;
   tree_col->id = 0;
   tree_col->position = 0;
   tree_col->xoffset = 0;
   tree_col->min_width = COL_MIN_WIDTH;
   tree_col->width = COL_MIN_WIDTH;
   tree_col->visible_width = COL_MIN_WIDTH;
   tree_col->visible = ETK_TRUE;
   tree_col->resizable = ETK_TRUE;
   tree_col->expand = ETK_FALSE;
   tree_col->header = NULL;
   tree_col->clip = NULL;
   tree_col->separator = NULL;
   tree_col->sort.compare_cb = NULL;
   tree_col->sort.data = NULL;      
}

/* Destroys the tree column */
static void _etk_tree2_col_destructor(Etk_Tree2_Col *tree_col)
{
   if (!tree_col)
      return;
   etk_tree2_model_free(tree_col->model);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_tree2_col_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Tree2_Col *tree_col;

   if (!(tree_col = ETK_TREE2_COL(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TREE2_COL_TITLE_PROPERTY:
         etk_tree2_col_title_set(tree_col, etk_property_value_string_get(value));
         break;
      case ETK_TREE2_COL_RESIZABLE_PROPERTY:
         etk_tree2_col_resizable_set(tree_col, etk_property_value_bool_get(value));
         break;
      case ETK_TREE2_COL_EXPAND_PROPERTY:
         etk_tree2_col_expand_set(tree_col, etk_property_value_bool_get(value));
         break;
      case ETK_TREE2_COL_VISIBLE_PROPERTY:
         etk_tree2_col_visible_set(tree_col, etk_property_value_bool_get(value));
         break;
      case ETK_TREE2_COL_POSITION_PROPERTY:
         etk_tree2_col_position_set(tree_col, etk_property_value_int_get(value));
         break;
      case ETK_TREE2_COL_WIDTH_PROPERTY:
         etk_tree2_col_width_set(tree_col, etk_property_value_int_get(value));
         break;
      case ETK_TREE2_COL_MIN_WIDTH_PROPERTY:
         etk_tree2_col_min_width_set(tree_col, etk_property_value_int_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_tree2_col_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Tree2_Col *tree_col;

   if (!(tree_col = ETK_TREE2_COL(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TREE2_COL_TITLE_PROPERTY:
         etk_property_value_string_set(value, etk_tree2_col_title_get(tree_col));
         break;
      case ETK_TREE2_COL_RESIZABLE_PROPERTY:
         etk_property_value_bool_set(value, tree_col->resizable);
         break;
      case ETK_TREE2_COL_EXPAND_PROPERTY:
         etk_property_value_bool_set(value, tree_col->expand);
         break;
      case ETK_TREE2_COL_VISIBLE_PROPERTY:
         etk_property_value_bool_set(value, tree_col->visible);
         break;
      case ETK_TREE2_COL_POSITION_PROPERTY:
         etk_property_value_int_set(value, tree_col->position);
         break;
      case ETK_TREE2_COL_WIDTH_PROPERTY:
         etk_property_value_int_set(value, tree_col->width);
         break;
      case ETK_TREE2_COL_MIN_WIDTH_PROPERTY:
         etk_property_value_int_set(value, tree_col->min_width);
         break;
      default:
         break;
   }
}

/**************************
 * Tree Headers
 **************************/

/* Allocates size for the headers of the tree */
static void _etk_tree2_headers_size_allocate(Etk_Tree2 *tree, Etk_Geometry geometry)
{
   int i;
   
   if (!tree)
      return;
   
   if (tree->headers_visible)
   {
      Etk_Tree2_Col *col;
      Etk_Tree2_Col *first_visible_col;
      Etk_Tree2_Col *last_visible_col;
      Etk_Geometry header_geometry;
      Etk_Geometry header_bar_geometry;
      int header_bar_xoffset;
      
      etk_widget_inner_geometry_get(tree->grid, &header_bar_geometry.x, NULL, &header_bar_geometry.w, NULL);
      header_bar_geometry.y = geometry.y;
      header_bar_geometry.h = geometry.h;
      header_bar_xoffset = header_bar_geometry.x - geometry.x;
      
      first_visible_col = NULL;
      last_visible_col = NULL;
      for (i = 0; i < tree->num_cols; i++)
      {
         col = tree->columns[i];
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
      
      header_geometry.y = header_bar_geometry.y;
      header_geometry.h = header_bar_geometry.h;
      
      for (i = 0; i < tree->num_cols; i++)
      {
         if (tree->columns[i]->visible
            && first_visible_col->position <= tree->columns[i]->position
            && last_visible_col->position >= tree->columns[i]->position)
         {
            header_geometry.x = header_bar_geometry.x + tree->columns[i]->xoffset;
            header_geometry.w = tree->columns[i]->visible_width;
            if (tree->columns[i] == first_visible_col)
            {
               header_geometry.w += (header_geometry.x - geometry.x);
               header_geometry.x = geometry.x;
            }
            if (tree->columns[i] == last_visible_col)
               header_geometry.w = geometry.x + geometry.w - header_geometry.x;
            
            /* TODO: what if the group doesn't exist... */
            if (tree->columns[i] == first_visible_col && tree->columns[i] == last_visible_col)
               etk_widget_theme_group_set(tree->columns[i]->header, "header_unique");
            else if (tree->columns[i] == first_visible_col)
               etk_widget_theme_group_set(tree->columns[i]->header, "header_first");
            else if (tree->columns[i] == last_visible_col)
               etk_widget_theme_group_set(tree->columns[i]->header, "header_last");
            else
               etk_widget_theme_group_set(tree->columns[i]->header, "header");
            
            etk_widget_show(tree->columns[i]->header);
            etk_widget_raise(tree->columns[i]->header);
            etk_widget_size_allocate(tree->columns[i]->header, header_geometry);
         }
         else
            etk_widget_hide(tree->columns[i]->header);
      }
   }
   else
   {
      for (i = 0; i < tree->num_cols; i++)
         etk_widget_hide(tree->columns[i]->header);
   }
}

/**************************
 * Tree Scroll Content
 **************************/

/* Allocates size for the tree's grid, and for the headers if they are not contained by the tree widget */
static void _etk_tree2_scroll_content_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Tree2 *tree;
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
      _etk_tree2_headers_size_allocate(tree, geometry);
   }
}

/* Sets the scroll offset of the tree scroll-content */
static void _etk_tree2_scroll_content_scroll(Etk_Widget *widget, int x, int y)
{
   Etk_Tree2 *tree;
   
   if (!(tree = TREE_GET(widget)))
      return;
   
   tree->scroll_x = x;
   tree->scroll_y = y;
   etk_widget_redraw_queue(ETK_WIDGET(tree));
}

/* Gets the scrolling size of the tree scroll-content */
static void _etk_tree2_scroll_content_scroll_size_get(Etk_Widget *widget, Etk_Size scrollview_size, Etk_Size scrollbar_size, Etk_Size *scroll_size)
{
   Etk_Tree2 *tree;
   int i;
   
   if (!(tree = TREE_GET(widget)) || !scroll_size)
      return;
   
   scroll_size->w = 0;
   for (i = 0; i < tree->num_cols; i++)
      scroll_size->w += tree->columns[i]->width;
   scroll_size->h = tree->root.num_visible_children * tree->rows_height;
}

/**************************
 * Tree Grid
 **************************/

/* Allocates size for the grid: it first calculates the width of the visible cols,
 * then creates/destroys the rows objects according to the new height of the tree grid,
 * and finally it updates the content of the rows */
static void _etk_tree2_grid_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Tree2 *tree;
   Etk_Tree2_Col *first_visible_col;
   Etk_Tree2_Col *last_visible_col;
   Etk_Tree2_Col *col;
   Evas_List *prev_visible_rows;
   Evas_List *new_visible_rows;
   Evas_List *l;
   int i, j;
   
   if (!(tree = TREE_GET(widget)))
      return;
   
   /* First, we calculate the size of the visible cols */
   {
      int num_visible_cols;
      int num_expand_cols;
      int columns_width;
      int freespace;
      int xoffset;
      
      first_visible_col = NULL;
      last_visible_col = NULL;
      columns_width = 0;
      num_visible_cols = 0;
      num_expand_cols = 0;
      for (i = 0; i < tree->num_cols; i++)
      {
         col = tree->columns[i];
         if (col->visible)
         {
            if (!first_visible_col || first_visible_col->position > col->position)
               first_visible_col = col;
            if (!last_visible_col || last_visible_col->position < col->position)
               last_visible_col = col;
            
            col->visible_width = col->width;
            columns_width += col->width;
            num_visible_cols++;
            if (col->expand)
               num_expand_cols++;
         }
      }
      
      /* At least one column is visible */
      if (first_visible_col)
      {
         /* Calculate the width of the visible columns */
         freespace = geometry.w - columns_width;
         if (freespace > 0)
         {
            if (num_expand_cols > 0)
            {
               for (i = 0; i < tree->num_cols; i++)
               {
                  col = tree->columns[i];
                  if (col->visible && col->expand)
                     col->visible_width += (float)freespace / (float)num_expand_cols;
               }
            }
            else
               last_visible_col->visible_width += freespace;
         }
         
         /* Calculate the horizontal position of the visible columns */
         xoffset = -tree->scroll_x;
         for (i = 0; i < tree->num_cols; i++)
         {
            for (j = 0; j < tree->num_cols; j++)
            {
               col = tree->columns[j];
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
   }
   
   /* Move, resize, and show/hide the column separators and clips */
   for (i = 0; i < tree->num_cols; i++)
   {
      col = tree->columns[i];
      
      if (!col->visible)
      {
         evas_object_hide(col->clip);
         evas_object_hide(col->separator);
      }
      else
      {
         int col_x1, col_x2;
         
         col_x1 = ETK_MAX(0, col->xoffset);
         col_x2 = ETK_MIN(geometry.w, col->xoffset + col->visible_width);
         
         if (col_x1 < col_x2)
         {
            evas_object_move(col->clip, geometry.x + col_x1, geometry.y);
            evas_object_resize(col->clip, col_x2 - col_x1 + 1, geometry.h);
            evas_object_show(col->clip);
            
            if (col_x2 >= 0 && col_x2 < geometry.w && col != last_visible_col)
            {
               evas_object_move(col->separator, geometry.x + col_x2, geometry.y);
               evas_object_resize(col->separator, 1, geometry.h);
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
      _etk_tree2_purge(tree);
      tree->purge_job = NULL;
   }
   
   /* Save the rows currently rendered so we could emit the "row_shown/hidden" signals later */
   prev_visible_rows = NULL;
   new_visible_rows = NULL;
   for (l = tree->row_objects; l; l = l->next)
   {
      Etk_Tree2_Row_Object *row_object;
      
      row_object = l->data;
      if (row_object->row)
         prev_visible_rows = evas_list_append(prev_visible_rows, row_object->row);
   }
   
   /* Create or destroy row objects if the height of the grid has changed */
   {
      int num_visible_rows;
      int current_num_rows;
      
      num_visible_rows = (geometry.h / tree->rows_height) + 2;
      current_num_rows = evas_list_count(tree->row_objects);
      
      if (current_num_rows < num_visible_rows)
      {
         /* We add more row objects */
         for (i = 0; i < (num_visible_rows - current_num_rows); i++)
         {
            Etk_Tree2_Row_Object *row_object;
            
            if ((row_object = _etk_tree2_row_object_create(tree)))
               tree->row_objects = evas_list_append(tree->row_objects, row_object);
         }
      }
      else if (current_num_rows > num_visible_rows)
      {
         Evas_List *last, *prev;
         
         /* We destroy the row objects that are no longer needed */
         last = evas_list_last(tree->row_objects);
         for (i = 0; i < (current_num_rows - num_visible_rows) && last; i++)
         {
            _etk_tree2_row_object_destroy(tree, last->data);
            prev = last->prev;
            tree->row_objects = evas_list_remove_list(tree->row_objects, last);
            last = prev;
         }
      }
   }
   
   /* Update the position and the content of the row objects */
   {
      Etk_Tree2_Row *row;
      Etk_Tree2_Row_Object *row_object;
      Etk_Geometry cell_geometry;
      Etk_Bool show_expanders;
      Evas_List *l2;
      int row_id;
      int row_y;
      int depth;
      
      depth = 0;
      row_id = tree->scroll_y / tree->rows_height;
      row_y = -(tree->scroll_y % tree->rows_height);
      for (row = tree->root.first_child, i = 0; i < row_id && row; i++)
         row = _etk_tree2_row_next_to_render_get(row, &depth);
      show_expanders = (tree->total_rows > tree->root.num_children && first_visible_col);
      
      for (l = tree->row_objects; l; l = l->next)
      {
         row_object = l->data;
         
         if (row)
         {
            /* If there is still a row to render, we render the content of the row */
            evas_object_move(row_object->background, geometry.x, geometry.y + row_y);
            evas_object_resize(row_object->background, geometry.w, tree->rows_height);
            evas_object_show(row_object->background);
            
            edje_object_signal_emit(row_object->background,
               (row_id % 2 == 0) ? "etk,state,odd" : "etk,state,even", "etk");
            edje_object_signal_emit(row_object->background,
               row->selected ? "etk,state,selected" : "etk,state,unselected", "etk");
            edje_object_message_signal_process(row_object->background);
            
            if ((l2 = evas_list_find_list(prev_visible_rows, row)))
               prev_visible_rows = evas_list_remove_list(prev_visible_rows, l2);
            else
               new_visible_rows = evas_list_append(new_visible_rows, row);
            
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
            for (i = 0; i < tree->num_cols; i++)
            {
               col = tree->columns[i];
               if (col->visible)
               {
                  cell_geometry.x = geometry.x + col->xoffset + CELL_HMARGINS;
                  cell_geometry.y = geometry.y + row_y + CELL_VMARGINS;
                  cell_geometry.w = col->width - (2 * CELL_HMARGINS);
                  cell_geometry.h = tree->rows_height - (2 * CELL_VMARGINS);
                  
                  /* Render the expander of the row */
                  if (col == first_visible_col && show_expanders)
                  {
                     /* TODO: 18 shouldn't be hardcoded... */
                     if (row->num_children > 0)
                     {
                        evas_object_move(row_object->expander,
                           cell_geometry.x + (depth * 18), cell_geometry.y);
                        evas_object_resize(row_object->expander, 18, 18);
                     }
                     cell_geometry.x += ((depth + 1) * 18) + CELL_HMARGINS;
                     cell_geometry.w -= ((depth + 1) * 18) + CELL_HMARGINS;
                  }
                  
                  /* Render the sub-objects of the cell */
                  if (col->model->render)
                     col->model->render(col->model, row, cell_geometry, row->cells_data[i], row_object->cells[i].objects);
               }
               else
               {
                  for (j = 0; j < MAX_OBJECTS_PER_CELL; j++)
                     evas_object_hide(row_object->cells[i].objects[j]);
               }
            }
            
            row_object->row = row;
         }
         else
         {
            /* If there is no more row to render, we hide the row objects */
            for (i = 0; i < tree->num_cols; i++)
            {
               for (j = 0; j < MAX_OBJECTS_PER_CELL; j++)
                  evas_object_hide(row_object->cells[i].objects[j]);
            }
            evas_object_hide(row_object->expander);
            evas_object_hide(row_object->background);
            row_object->row = NULL;
         }
         
         row = _etk_tree2_row_next_to_render_get(row, &depth);
         row_y += tree->rows_height;
         row_id++;
      }
   }
   
   /* Emit the "row_shown/hidden" signals */
   while (prev_visible_rows)
   {
      etk_signal_emit(_etk_tree2_signals[ETK_TREE2_ROW_HIDDEN_SIGNAL], ETK_OBJECT(tree), NULL, prev_visible_rows->data);
      prev_visible_rows = evas_list_remove_list(prev_visible_rows, prev_visible_rows);
   }
   while (new_visible_rows)
   {
      etk_signal_emit(_etk_tree2_signals[ETK_TREE2_ROW_SHOWN_SIGNAL], ETK_OBJECT(tree), NULL, new_visible_rows->data);
      new_visible_rows = evas_list_remove_list(new_visible_rows, new_visible_rows);
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
static void _etk_tree2_realize_cb(Etk_Object *object, void *data)
{
   Etk_Tree2 *tree;
   Evas *evas;
   int tree_contains_headers;
   int i;
   
   if (!(tree = ETK_TREE2(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(tree))))
      return;
   
   /* Read the data from the theme */
   if (etk_widget_theme_data_get(ETK_WIDGET(tree), "tree_contains_headers", "%d", &tree_contains_headers) != 1)
      tree_contains_headers = 1;
   if (etk_widget_theme_data_get(ETK_WIDGET(tree), "separator_color", "%d %d %d %d", &tree->separator_color.r,
      &tree->separator_color.g, &tree->separator_color.b, &tree->separator_color.a) != 4)
   {
      tree->separator_color.r = 0;
      tree->separator_color.g = 0;
      tree->separator_color.b = 0;
      tree->separator_color.a = 0;
   }
   else
   {
      evas_color_argb_premul(tree->separator_color.a, &tree->separator_color.r,
         &tree->separator_color.g, &tree->separator_color.b);
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
}

/* Called when the tree is focused */
static void _etk_tree2_focus_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Tree2 *tree;
   
   if (!(tree = ETK_TREE2(object)))
      return;
   
   etk_widget_theme_signal_emit(tree->scrolled_view, "etk,state,focused", ETK_FALSE);
   etk_widget_theme_signal_emit(tree->grid, "etk,state,focused", ETK_FALSE);
}

/* Called when the tree is unfocused */
static void _etk_tree2_unfocus_cb(Etk_Object *object, void *event, void *data)
{
   Etk_Tree2 *tree;
   
   if (!(tree = ETK_TREE2(object)))
      return;
   
   etk_widget_theme_signal_emit(tree->scrolled_view, "etk,state,unfocused", ETK_FALSE);
   etk_widget_theme_signal_emit(tree->grid, "etk,state,unfocused", ETK_FALSE);
}

/* Called when a key is pressed while the tree is focused */
static void _etk_tree2_key_down_cb(Etk_Object *object, Etk_Event_Key_Down *event, void *data)
{
   Etk_Tree2 *tree;
   
   if (!(tree = ETK_TREE2(object)))
      return;
   
   if (event->modifiers == ETK_MODIFIER_NONE)
   {
   }
   else if (event->modifiers & ETK_MODIFIER_CTRL)
   {
      /* CTRL + A: Select all the rows */
      if (strcmp(event->keyname, "a") == 0)
      {
         etk_tree2_select_all(tree);
         etk_signal_stop();
      }
   }
}

/**************************
 * Tree Grid
 **************************/

/* Called when the tree grid is realized */
static void _etk_tree2_grid_realize_cb(Etk_Object *object, void *data)
{
   Etk_Tree2 *tree;
   Evas *evas;
   int i;
   
   if (!(tree = TREE_GET(object)) || !(evas = etk_widget_toplevel_evas_get(ETK_WIDGET(object))))
      return;
   
   for (i = 0; i < tree->num_cols; i++)
      _etk_tree2_col_realize(tree, i);
   
   tree->grid_clip = evas_object_rectangle_add(evas);
   etk_widget_member_object_add(tree->grid, tree->grid_clip);
}

/* Called when the tree grid is unrealized */
static void _etk_tree2_grid_unrealize_cb(Etk_Object *object, void *data)
{
   Etk_Tree2 *tree;
   int i;
   
   if (!(tree = TREE_GET(object)))
      return;
   
   tree->grid_clip = NULL;
   
   for (i = 0; i < tree->num_cols; i++)
   {
      tree->columns[i]->clip = NULL;
      tree->columns[i]->separator = NULL;
   }
   
   while (tree->row_objects)
   {
      _etk_tree2_row_object_destroy(tree, tree->row_objects->data);
      tree->row_objects = evas_list_remove_list(tree->row_objects, tree->row_objects);
   }
}

/**************************
 * Tree Headers
 **************************/

/* Called when the mouse presses the column header */
static void _etk_tree2_header_mouse_down_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Event_Mouse_Down *event = event_info;
   Etk_Tree2_Col *col;
   
   if (!(col = ETK_TREE2_COL(data)) || event->button != 1)
      return;
   
   /* Checks if we should resize a column */
   if ((col->tree->col_to_resize = etk_tree2_col_to_resize_get(col, event->widget.x)))
   {
      col->tree->col_resize_orig_width = col->tree->col_to_resize->width;
      col->tree->col_resize_orig_mouse_x = event->canvas.x;
      etk_signal_stop();
   }
}

/* Called when the mouse releases the column header */
static void _etk_tree2_header_mouse_up_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Event_Mouse_Up *event = event_info;
   Etk_Tree2_Col *col;
   
   if (!(col = ETK_TREE2_COL(data)) || event->button != 1)
      return;
   
   if (col->tree->col_to_resize)
      col->tree->col_to_resize = NULL;
   /* TODO: else: sort the column if we are still above the header */
}

/* Called when the mouse moves over the column header */
static void _etk_tree2_header_mouse_move_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Event_Mouse_Move *event = event_info;
   Etk_Tree2_Col *col, *col_to_resize;
   Etk_Toplevel *toplevel;
   
   if (!(col = ETK_TREE2_COL(data)) || !(toplevel = etk_widget_toplevel_parent_get(ETK_WIDGET(object))))
      return;
   
   if (col->tree->col_to_resize)
   {
      /* Resize the column to resize */
      etk_tree2_col_width_set(col->tree->col_to_resize,
         col->tree->col_resize_orig_width + event->cur.canvas.x - col->tree->col_resize_orig_mouse_x);
   }
   else
   {
      /* Set/Unset the resize mouse pointer if the pointer is/was between two column headers */
      col_to_resize = etk_tree2_col_to_resize_get(col, event->cur.widget.x);
      if (col_to_resize && !col->tree->col_resize_pointer_set)
      {
         etk_toplevel_pointer_push(toplevel, ETK_POINTER_H_DOUBLE_ARROW);
         col->tree->col_resize_pointer_set = ETK_TRUE;
      }
      else if (!col_to_resize && col->tree->col_resize_pointer_set)
      {
         etk_toplevel_pointer_pop(toplevel, ETK_POINTER_H_DOUBLE_ARROW);
         col->tree->col_resize_pointer_set = ETK_FALSE;
      }
   }
}

/* Called when the mouse enters the column header */
static void _etk_tree2_header_mouse_in_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Event_Mouse_In *event = event_info;
   Etk_Tree2_Col *col, *col_to_resize;
   Etk_Toplevel *toplevel;
   
   if (!(col = ETK_TREE2_COL(data)) || !(toplevel = etk_widget_toplevel_parent_get(ETK_WIDGET(object))))
      return;
   
   /* Set the resize mouse pointer if the pointer is between two column headers */
   col_to_resize = etk_tree2_col_to_resize_get(col, event->widget.x);
   if (col_to_resize && !col->tree->col_resize_pointer_set)
   {
      etk_toplevel_pointer_push(toplevel, ETK_POINTER_H_DOUBLE_ARROW);
      col->tree->col_resize_pointer_set = ETK_TRUE;
   }
}

/* Called when the mouse leaves the column header */
static void _etk_tree2_header_mouse_out_cb(Etk_Object *object, void *event_info, void *data)
{
   Etk_Tree2_Col *col;
   Etk_Toplevel *toplevel;
   
   if (!(col = ETK_TREE2_COL(data)) || !(toplevel = etk_widget_toplevel_parent_get(ETK_WIDGET(object))))
      return;
   
   /* Unset the resize mouse pointer if it was set */
   if (col->tree->col_resize_pointer_set)
   {
      etk_toplevel_pointer_pop(toplevel, ETK_POINTER_H_DOUBLE_ARROW);
      col->tree->col_resize_pointer_set = ETK_FALSE;
   }
}

/* Called when the background of a row is pressed by the mouse */
static void _etk_tree2_row_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree2_Row_Object *row_object;
   Etk_Event_Mouse_Down event;
   
   if (!(row_object = data) || !row_object->row)
      return;
   
   etk_event_mouse_down_wrap(ETK_WIDGET(row_object->row->tree), event_info, &event);
   
   /* We do not select a row on the "key-down" event if the row is already selected
    * to allow the user to drag and drop several rows. The selection will be done on "key-up" */
   if (!row_object->row->selected || event.modifiers != ETK_MODIFIER_NONE)
      _etk_tree2_row_select(row_object->row->tree, row_object->row, event.modifiers);
   
   if (event.flags != ETK_MOUSE_NONE)
   {
      etk_signal_emit(_etk_tree2_signals[ETK_TREE2_ROW_CLICKED_SIGNAL], ETK_OBJECT(row_object->row->tree),
         NULL, row_object->row, &event);
   }
}

/* Called when the background of a row is released by the mouse */
static void _etk_tree2_row_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree2_Row_Object *row_object;
   Etk_Event_Mouse_Up event;
   int x, y, w, h;
   
   if (!(row_object = data) || !row_object->row)
      return;
   
   evas_object_geometry_get(obj, &x, &y, &w, &h);
   etk_event_mouse_up_wrap(ETK_WIDGET(row_object->row->tree), event_info, &event);
   
   /* We make sure the mouse button has been released over the row */
   if (ETK_INSIDE(event.canvas.x, event.canvas.y, x, y, w, h))
   {
      if (row_object->row->selected && event.modifiers == ETK_MODIFIER_NONE)
         _etk_tree2_row_select(row_object->row->tree, row_object->row, ETK_MODIFIER_NONE);
      
      if (event.flags == ETK_MOUSE_NONE)
      {
         etk_signal_emit(_etk_tree2_signals[ETK_TREE2_ROW_CLICKED_SIGNAL], ETK_OBJECT(row_object->row->tree),
            NULL, row_object->row, &event);
      }
   }
}

/* Called when the expander of a row is released by the mouse */
static void _etk_tree2_row_expander_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Etk_Tree2_Row_Object *row_object;
   Evas_Event_Mouse_Up *event = event_info;
   Evas_Coord x, y, w, h;
   
   if (!(row_object = data) || !row_object->row)
      return;
   
   evas_object_geometry_get(obj, &x, &y, &w, &h);
   if (ETK_INSIDE(event->canvas.x, event->canvas.y, x, y, w, h))
   {
      if (row_object->row->unfolded)
         etk_tree2_row_fold(row_object->row);
      else
         etk_tree2_row_unfold(row_object->row);
   }
}

/**************************
 *
 * Private functions
 *
 **************************/

/* A job called when rows need to be deleted */
static void _etk_tree2_purge_job(void *data)
{
   Etk_Tree2 *tree;
   
   if (!(tree = ETK_TREE2(data)))
      return;
   
   _etk_tree2_purge(tree);
   tree->purge_job = NULL;
}

/* Deletes effectively all the rows marked as deleted */
static void _etk_tree2_purge(Etk_Tree2 *tree)
{
   Evas_List *l;
   Etk_Tree2_Row *row, *r, *next;
   int i;
   
   if (!tree)
      return;
   
   /* First we dissociate the row objects from the rows that will be deleted */
   for (l = tree->row_objects; l; l = l->next)
   {
      Etk_Tree2_Row_Object *row_object;
      
      row_object = l->data;
      if (row_object->row && row_object->row->delete_me)
      {
         etk_signal_emit(_etk_tree2_signals[ETK_TREE2_ROW_HIDDEN_SIGNAL], ETK_OBJECT(tree), NULL, row_object->row);
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
               if (tree->columns[i]->model->cell_data_free)
                  tree->columns[i]->model->cell_data_free(tree->columns[i]->model, r->cells_data[i]);
               free(r->cells_data[i]);
            }
            free(r->cells_data);
         }
         
         if (r->data && r->data_free_cb)
            r->data_free_cb(r->data);
         
         free(r);
      }
      tree->purge_pool = evas_list_remove_list(tree->purge_pool, tree->purge_pool);
   }
}

/* Moves a row of the tree to the list of rows to delete during the next iteration */
static void _etk_tree2_row_move_to_purge_pool(Etk_Tree2_Row *row)
{
   Etk_Tree2 *tree;
   Etk_Tree2_Row *r;
   
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
   
   tree->purge_pool = evas_list_append(tree->purge_pool, row);
   
   if (!tree->purge_job)
      tree->purge_job = ecore_job_add(_etk_tree2_purge_job, tree);
}

/* Creates the Evas objects of the corresponding column */
static void _etk_tree2_col_realize(Etk_Tree2 *tree, int col_nth)
{
   Evas *evas;
   Etk_Tree2_Col *col;

   if (!tree || col_nth < 0 || col_nth >= tree->num_cols)
      return;
   if (!(evas = etk_widget_toplevel_evas_get(tree->grid)))
      return;

   col = tree->columns[col_nth];
   col->clip = evas_object_rectangle_add(evas);
   etk_widget_member_object_add(tree->grid, col->clip);

   col->separator = evas_object_rectangle_add(evas);
   evas_object_color_set(col->separator, tree->separator_color.r,
      tree->separator_color.g, tree->separator_color.b, tree->separator_color.a);
   evas_object_pass_events_set(col->separator, 1);
   evas_object_clip_set(col->separator, col->clip);
   etk_widget_member_object_add(tree->grid, col->separator);
}

/* Returns the column to resize according to the position of the mouse in the column "col" */
static Etk_Tree2_Col *etk_tree2_col_to_resize_get(Etk_Tree2_Col *col, int x)
{
   if (!col)
      return NULL;
   
   if ((x + col->header->inset.left <= COL_RESIZE_THRESHOLD))
   {
      Etk_Tree2_Col *prev_visible_col, *col2;
      int i;
      
      prev_visible_col = NULL;
      for (i = 0; i < col->tree->num_cols; i++)
      {
         col2 = col->tree->columns[i];
         if ((col2->position < col->position) && (!prev_visible_col || (prev_visible_col->position < col2->position)))
            prev_visible_col = col2;
      }
      
      return (prev_visible_col && prev_visible_col->resizable) ? prev_visible_col : NULL;
   }
   else if ((col->header->geometry.w - (x + col->header->inset.left) <= COL_RESIZE_THRESHOLD))
      return (col && col->resizable) ? col : NULL;

   return NULL;
}

/* Gets the next row to render */
static Etk_Tree2_Row *_etk_tree2_row_next_to_render_get(Etk_Tree2_Row *row, int *depth)
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

/* Sets the fields of a row */
static void _etk_tree2_row_fields_set_valist_full(Etk_Tree2_Row *row, va_list args, Etk_Bool emit_signal)
{
   Etk_Tree2_Col *col;
   va_list args2;
   
   if (!row)
      return;
   
   va_copy(args2, args);
   while ((col = va_arg(args2, Etk_Tree2_Col *)))
   {
      if (col->model->cell_data_set)
      {
         col->model->cell_data_set(col->model, row->cells_data[col->id], &args2);
         if (emit_signal)
            etk_signal_emit(_etk_tree2_col_signals[ETK_TREE2_COL_CELL_VALUE_CHANGED], ETK_OBJECT(col), NULL, row);
      }
   }
   va_end(args2);
   
   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
}

/* Creates a new row object and its subobjects */
static Etk_Tree2_Row_Object *_etk_tree2_row_object_create(Etk_Tree2 *tree)
{
   Etk_Tree2_Row_Object *row_object;
   Etk_Tree2_Col *col;
   Evas *evas;
   int i, j;
   
   if (!tree || !tree->built || !(evas = etk_widget_toplevel_evas_get(tree->grid)))
      return NULL;
   
   row_object = malloc(sizeof(Etk_Tree2_Row_Object));
   row_object->row = NULL;
   
   /* Create the background object of the row */
   row_object->background = edje_object_add(evas);
   etk_theme_edje_object_set_from_parent(row_object->background, "row", ETK_WIDGET(tree));
   evas_object_repeat_events_set(row_object->background, 1);
   evas_object_clip_set(row_object->background, tree->grid_clip);
   etk_widget_member_object_add(tree->grid, row_object->background);
   evas_object_event_callback_add(row_object->background, EVAS_CALLBACK_MOUSE_DOWN,
      _etk_tree2_row_mouse_down_cb, row_object);
   evas_object_event_callback_add(row_object->background, EVAS_CALLBACK_MOUSE_UP,
      _etk_tree2_row_mouse_up_cb, row_object);
   
   /* Create the expander object of the row */
   if (tree->mode == ETK_TREE2_MODE_TREE)
   {
      row_object->expander = edje_object_add(evas);
      etk_theme_edje_object_set_from_parent(row_object->expander, "expander", ETK_WIDGET(tree));
      /* TODO: the expander should be clipped to the clip of the first visible col */
      evas_object_clip_set(row_object->expander, tree->grid_clip);
      etk_widget_member_object_add(tree->grid, row_object->expander);
      evas_object_event_callback_add(row_object->expander, EVAS_CALLBACK_MOUSE_UP,
         _etk_tree2_row_expander_mouse_up_cb, row_object);
   }
   else
      row_object->expander = NULL;
   
   /* Create the sub-objects of the row's cells */
   row_object->cells = calloc(tree->num_cols, sizeof(Etk_Tree2_Cell_Objects));
   for (i = 0; i < tree->num_cols; i++)
   {
      col = tree->columns[i];
      if (col->model->objects_create)
      {
         col->model->objects_create(col->model, row_object->cells[i].objects, evas);
         for (j = 0; j < MAX_OBJECTS_PER_CELL; j++)
         {
            if (row_object->cells[i].objects[j])
            {
               evas_object_clip_set(row_object->cells[i].objects[j], col->clip);
               etk_widget_member_object_add(tree->grid, row_object->cells[i].objects[j]);
            }
         }
      }
      evas_object_raise(col->separator);
   }
   
   return row_object;
}

/* Destroys the row object and its subobjects */
static void _etk_tree2_row_object_destroy(Etk_Tree2 *tree, Etk_Tree2_Row_Object *row_object)
{
   int i, j;
   
   if (!tree || !row_object)
      return;
   
   for (i = 0; i < tree->num_cols; i++)
   {
      for (j = 0; j < MAX_OBJECTS_PER_CELL; j++)
         evas_object_del(row_object->cells[i].objects[j]);
   }
   free(row_object->cells);
   
   evas_object_del(row_object->expander);
   evas_object_del(row_object->background);
   free(row_object);
}

/* Selects/Unselects the corresponding rows according to the modifiers */
static void _etk_tree2_row_select(Etk_Tree2 *tree, Etk_Tree2_Row *row, Etk_Modifiers modifiers)
{
   if (!tree || !row)
      return;
   
   if (!tree->multiple_select)
   {
      if (row->selected)
      {
         /* Unselect the row */
         if (modifiers & ETK_MODIFIER_CTRL)
            etk_tree2_row_unselect(row);
      }
      else
      {
         /* Unselect the selected row and select only the given row */
         if (tree->last_selected_row && tree->last_selected_row->selected)
            etk_tree2_row_unselect(tree->last_selected_row);
         else
            etk_tree2_unselect_all(tree);
         
         row->selected = ETK_TRUE;
         row->tree->last_selected_row = row;
         etk_signal_emit(_etk_tree2_signals[ETK_TREE2_ROW_SELECTED_SIGNAL], ETK_OBJECT(row->tree), NULL, row);
      }
   }
   else
   {
      if ((modifiers & ETK_MODIFIER_SHIFT) && tree->last_selected_row)
      {
         Etk_Tree2_Row *r;
         int cnt = 0;
         
         etk_tree2_unselect_all(tree);
         
         /* Walk through all the rows of the tree and select the rows
          * between the last selected row and the given row */
         for (r = tree->root.first_child; r; r = etk_tree2_row_walk_next(r, ETK_TRUE))
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
         
         etk_signal_emit(_etk_tree2_signals[ETK_TREE2_ROW_SELECTED_SIGNAL], ETK_OBJECT(row->tree), NULL, row);
      }
      /* Invert the selected state of the given row */
      else if (modifiers & ETK_MODIFIER_CTRL)
      {
         if (row->selected)
            etk_tree2_row_unselect(row);
         else
         {
            row->selected = ETK_TRUE;
            row->tree->last_selected_row = row;
            etk_signal_emit(_etk_tree2_signals[ETK_TREE2_ROW_SELECTED_SIGNAL], ETK_OBJECT(row->tree), NULL, row);
         }
      }
      else
      {
         /* Unselect all the rows and select only the given row */
         etk_tree2_unselect_all(tree);
         row->selected = ETK_TRUE;
         row->tree->last_selected_row = row;
         etk_signal_emit(_etk_tree2_signals[ETK_TREE2_ROW_SELECTED_SIGNAL], ETK_OBJECT(row->tree), NULL, row);
      }
   }
   
   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
}

/** @} */
