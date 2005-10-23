/** @file etk_tree.c */
#include "etk_tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Evas.h>
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

enum _Etk_Tree_Property_Id
{
   ETK_TREE_MODE_PROPERTY,
   ETK_TREE_MULTIPLE_SELECT_PROPERTY,
   ETK_TREE_HEADERS_VISIBLE_PROPERTY
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

static void _etk_tree_expander_clicked_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree_row_pressed_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _etk_tree_header_mouse_down_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_up_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_move_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_in_cb(Etk_Object *object, void *event, void *data);
static void _etk_tree_header_mouse_out_cb(Etk_Object *object, void *event, void *data);

static void _etk_tree_update(Etk_Tree *tree);
static int _etk_tree_rows_draw(Etk_Tree *tree, Etk_Tree_Node *node, Ecore_List *items_objects,
   int x, int w, int h, int xoffset, int yoffset, int first_row_id, int first_row_color);
static Etk_Tree_Row *_etk_tree_row_new_valist(Etk_Tree *tree, Etk_Tree_Node *node, va_list args);
static Etk_Tree_Item_Objects *_etk_tree_item_objects_new(Etk_Tree *tree);
static void _etk_tree_item_objects_free(Etk_Tree_Item_Objects *item_objects, Etk_Tree *tree);
static void _etk_tree_row_free(Etk_Tree_Row *row);
static void _etk_tree_cell_clear(Etk_Tree_Cell *cell, Etk_Tree_Col_Type cell_type);
static void _etk_tree_col_realize(Etk_Tree *tree, int col_nth);
static Etk_Tree_Col *etk_tree_col_to_resize_get(Etk_Tree_Col *col, int x);

static void _etk_tree_node_unselect_all(Etk_Tree_Node *node);
static void _etk_tree_node_select(Etk_Tree *tree, Etk_Tree_Node *node, Evas_Modifier *modifiers);

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
 * @brief Creates a new column for a tree
 * @param tree a tree
 * @param title the tile of the column
 * @param type the type of the objects in the cells of the column
 * @param min_width the minimum width of the column
 * @param width the width of the column
 * @param resizable TRUE whether the column should be resizable
 * @return Returns the new column
 */
Etk_Tree_Col *etk_tree_col_new(Etk_Tree *tree, const char *title, Etk_Tree_Col_Type type, int min_width, int width, Etk_Bool resizable)
{
   Etk_Tree_Col *new_col;
   Etk_Widget *new_header;

   if (!tree)
      return NULL;

   tree->columns = realloc(tree->columns, sizeof(Etk_Tree_Col *) * (tree->num_cols + 1));
   new_col = malloc(sizeof(Etk_Tree_Col));
   tree->columns[tree->num_cols] = new_col;

   new_col->id = tree->num_cols;
   new_col->tree = tree;
   if (title)
      new_col->title = strdup(title);
   else
      new_col->title = NULL;
   new_col->type = type;

   new_col->min_width = ETK_MAX(0, min_width);
   new_col->width = ETK_MAX(new_col->min_width, width);
   new_col->place = new_col->id;
   new_col->visible = TRUE;
   new_col->resizable = resizable;

   /* Create the header widget */
   new_header = etk_widget_new(ETK_BUTTON_TYPE, "theme_group", "tree_header", "label", title, "xalign", 0.0, NULL);
   etk_signal_connect("mouse_down", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_down_cb), new_col);
   etk_signal_connect("mouse_up", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_up_cb), new_col);
   etk_signal_connect("mouse_move", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_move_cb), new_col);
   etk_signal_connect("mouse_in", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_in_cb), new_col);
   etk_signal_connect("mouse_out", ETK_OBJECT(new_header), ETK_CALLBACK(_etk_tree_header_mouse_out_cb), new_col);
   etk_widget_parent_set(new_header, ETK_CONTAINER(tree));
   ecore_dlist_append(ETK_CONTAINER(tree)->children, new_header);
   etk_widget_show(new_header);
   new_col->header = new_header;

   tree->num_cols++;
   _etk_tree_col_realize(tree, tree->num_cols - 1);

   return new_col;
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
   etk_widget_resize_queue(ETK_WIDGET(tree));
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
 * @brief Freezes the tree: it will not be updated until it is thawed (TODO: irregular verb?!) @n
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
   Etk_Tree_Row *row;

   if (!tree || (tree->mode != ETK_TREE_MODE_LIST) || !tree->multiple_select)
      return;

   ecore_list_goto_first(tree->root.child_rows);
   while ((row = ecore_list_next(tree->root.child_rows)))
      row->node.selected = TRUE;
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

   /* TODO */
   _etk_tree_node_unselect_all(&tree->root);
   etk_widget_redraw_queue(ETK_WIDGET(tree));
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

   if (!row->tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(row->tree));
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
 * @param ... an Etk_Tree_Col * followed by the value of the cell, then again, an Etk_Tree_Col * followed by its value... terminated by NULL
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
 * @brief Removes all the rows of the tree 
 * @param tree a tree 
 */
void etk_tree_clear(Etk_Tree *tree)
{
   if (!tree)
      return;

   ecore_list_goto_first(tree->root.child_rows);
   while (!ecore_list_is_empty(tree->root.child_rows))
      ecore_list_remove_destroy(tree->root.child_rows);
   
   ecore_list_clear(tree->root.child_rows);
   if (!tree->frozen)
      etk_widget_redraw_queue(ETK_WIDGET(tree));
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
   num_item_to_add = num_visible_items - ecore_list_nodes(tree->items_objects);

   if (num_item_to_add > 0)
   {
      Etk_Tree_Item_Objects *item_objects;

      for (i = 0; i < num_item_to_add; i++)
      {
         if (!(item_objects = _etk_tree_item_objects_new(tree)))
            break;
         ecore_list_append(tree->items_objects, item_objects);
      }
   }
   else if (num_item_to_add < 0)
   {
      for (i = 0; i < -num_item_to_add; i++)
      {
         if (!ecore_list_goto_last(tree->items_objects))
            break;
         _etk_tree_item_objects_free(ecore_list_current(tree->items_objects), tree);
         ecore_list_remove(tree->items_objects);
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
   ecore_dlist_append(ETK_CONTAINER(tree)->children, tree->grid);
   etk_widget_show(tree->grid);

   tree->num_cols = 0;
   tree->columns = NULL;
   tree->last_selected = NULL;
   tree->column_to_resize = NULL;
   tree->resize_pointer_shown = FALSE;
   tree->headers_visible = TRUE;

   tree->root.parent = NULL;
   tree->root.child_rows = ecore_list_new();
   ecore_list_set_free_cb(tree->root.child_rows, ECORE_FREE_CB(_etk_tree_row_free));
   tree->root.num_visible_children = 0;
   tree->root.num_parent_children = 0;
   tree->root.expanded = TRUE;

   tree->item_height = -1;
   tree->image_height = -1;
   tree->expander_size = -1;
   tree->items_objects = ecore_list_new();

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
   Etk_Tree_Item_Objects *item_objects;
   int i;

   if (!tree)
      return;

   ecore_list_goto_first(tree->items_objects);
   while ((item_objects = ecore_list_next(tree->items_objects)))
      _etk_tree_item_objects_free(item_objects, tree);
   ecore_list_destroy(tree->items_objects);
   ecore_list_destroy(tree->root.child_rows);
   for (i = 0; i < tree->num_cols; i++)
   {
      if (tree->columns[i])
      {
         free(tree->columns[i]->title);
         free(tree->columns[i]);
         /* TODO: etk_object_destroy("header"); */
      }
   }
   free(tree->columns);
   /* TODO: */
   /* etk_object_destroy(ETK_OBJECT(tree->grid)); */
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
      new_size = ETK_MAX(col->tree->column_to_resize->min_width, new_size);

      if (new_size != col->tree->column_to_resize->width)
      {
         col->tree->column_to_resize->width = new_size;
         etk_widget_resize_queue(ETK_WIDGET(col->tree));
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
            if (i < tree->num_cols - 1)
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

   ecore_list_goto_first(tree->items_objects);
   _etk_tree_rows_draw(tree, &tree->root, tree->items_objects, x, w, h, 0, y - delta, first_visible_nth, (first_visible_nth % 2));

   while ((item_objects = ecore_list_next(tree->items_objects)))
   {
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
static int _etk_tree_rows_draw(Etk_Tree *tree, Etk_Tree_Node *node, Ecore_List *items_objects,
   int x, int w, int h, int xoffset, int yoffset, int first_row_id, int first_row_color)
{
   Etk_Tree_Row *row;
   Etk_Tree_Item_Objects *item_objects;
   int i, j;
   int first_col_offset;

   if (!tree || !node || !items_objects)
      return 0;

   first_col_offset = xoffset + ((node->num_parent_children > 0) ? tree->expander_size : 4) + 4;

   i = 0;
   ecore_list_goto_first(node->child_rows);
   while ((row = ecore_list_next(node->child_rows)))
   {
      if (first_row_id <= 0)
      {
         int item_y;
   
         if (!(item_objects = ecore_list_next(items_objects)))
            break;
   
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
            if (row->node.child_rows && ecore_list_nodes(row->node.child_rows) > 0)
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

      if (row->node.child_rows && row->node.expanded && ecore_list_goto_first(row->node.child_rows))
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
   Etk_Tree_Col *col;
   Etk_Tree_Node *n;

   if (!tree || !tree->built || !node)
      return NULL;

   new_row = malloc(sizeof(Etk_Tree_Row));
   new_row->tree = tree;
   new_row->node.parent = node;
   new_row->node.child_rows = NULL;
   new_row->node.num_visible_children = 0;
   new_row->node.num_parent_children = 0;
   new_row->node.expanded = FALSE;
   new_row->node.selected = FALSE;

   new_row->cells = malloc(sizeof(Etk_Tree_Cell) * tree->num_cols);
   while ((col = va_arg(args, Etk_Tree_Col *)))
   {
      Etk_Tree_Cell *new_cell;

      new_cell = &new_row->cells[col->id];
      switch (col->type)
      {
         case ETK_TREE_COL_TEXT:
         {
            const char *text;

            text = va_arg(args, char *);
            if (text)
               new_cell->text_value = strdup(text);
            else
               new_cell->text_value = NULL;
            break;
         }
         case ETK_TREE_COL_IMAGE:
         {
            const char *image;

            image = va_arg(args, char *);
            if (image)
               new_cell->image_filename_value = strdup(image);
            else
               new_cell->image_filename_value = NULL;
            break;
         }
         case ETK_TREE_COL_ICON_TEXT:
         {
            const char *icon;
            const char *text;

            icon = va_arg(args, char *);
            text = va_arg(args, char *);

            if (icon)
               new_cell->icon_text_value.icon_filename = strdup(icon);
            else
               new_cell->icon_text_value.icon_filename = NULL;

            if (text)
               new_cell->icon_text_value.text = strdup(text);
            else
               new_cell->icon_text_value.text = NULL;

            break;
         }
         case ETK_TREE_COL_INT:
         {
            int value;

            value = va_arg(args, int);
            new_cell->int_value = value;

            break;
         }
         case ETK_TREE_COL_DOUBLE:
         {
            double value;

            value = va_arg(args, double);
            new_cell->double_value = value;

            break;
         }
         default:
            new_cell->int_value = 0;
            break;
      }
   }

   for (n = new_row->node.parent; n; n = n->parent)
   {
      n->num_visible_children++;
      if (!n->expanded)
         break;
   }

   if ((n = new_row->node.parent) && (n = n->parent))
      n->num_parent_children++;

   if (!node->child_rows)
   {
      node->child_rows = ecore_list_new();
      ecore_list_set_free_cb(node->child_rows, ECORE_FREE_CB(_etk_tree_row_free));
   }
   ecore_list_append(node->child_rows, new_row);
   etk_widget_redraw_queue(ETK_WIDGET(tree));
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

   if (row->node.child_rows)
      ecore_list_destroy(row->node.child_rows);

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

/* TODO: doc */
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

/* Unselects recursively all the rows of the node */
static void _etk_tree_node_unselect_all(Etk_Tree_Node *node)
{
   Etk_Tree_Row *row;

   if (!node)
      return;

   node->selected = FALSE;
   if (node->child_rows)
   {
      ecore_list_goto_first(node->child_rows);
      while ((row = ecore_list_next(node->child_rows)))
         _etk_tree_node_unselect_all(&row->node);
   }
}

/* TODO: doc */
static void _etk_tree_node_select(Etk_Tree *tree, Etk_Tree_Node *node, Evas_Modifier *modifiers)
{
   if (!tree || !node)
      return;

   if (tree->mode == ETK_TREE_MODE_TREE || !tree->multiple_select)
   {
      etk_tree_unselect_all(tree);
      node->selected = TRUE;
   }
   else
   {
      if (evas_key_modifier_is_set(modifiers, "Shift"))
      {
         if (!evas_key_modifier_is_set(modifiers, "Control"))
            etk_tree_unselect_all(tree);
   
         if (!tree->last_selected)
         {
            node->selected = TRUE;
            tree->last_selected = node;
         }
         else
         {
            Etk_Bool selected = FALSE;
            Etk_Tree_Row *row;

            ecore_list_goto_first(tree->root.child_rows);
            while ((row = ecore_list_next(tree->root.child_rows)))
            {
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
               tree->last_selected = node;
            }
         }
      }
      else if (evas_key_modifier_is_set(modifiers, "Control"))
      {
         if (node->selected)
            node->selected = FALSE;
         else
         {
            node->selected = TRUE;
            tree->last_selected = node;
         }
      }
      else
      {
         etk_tree_unselect_all(tree);
         node->selected = TRUE;
         tree->last_selected = node;
      }
   }

   etk_widget_redraw_queue(ETK_WIDGET(tree->grid));
}
