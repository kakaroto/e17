/** @file etk_table.c */
#include "etk_table.h"
#include <stdlib.h>
#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Table
* @{
 */

/* Gets the array index of the cell located at column "col" and row "row" in table "table" */
#define ETK_TABLE_CELL_INDEX(table, col, row)   ((row) * (table)->num_cols + (col))
#define ETK_TABLE_CELL_MIN_SIZE 10

enum _Etk_Table_Property_Id
{
   ETK_TABLE_NUM_COLS_PROPERTY,
   ETK_TABLE_NUM_ROWS_PROPERTY,
   ETK_TABLE_HOMOGENEOUS_PROPERTY
};

static void _etk_table_constructor(Etk_Table *table);
static void _etk_table_destructor(Etk_Table *table);
static void _etk_table_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_table_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value);
static void _etk_table_child_add(Etk_Container *container, Etk_Widget *widget);
static void _etk_table_child_remove(Etk_Container *container, Etk_Widget *widget);
static Evas_List *_etk_table_children_get(Etk_Container *container);
static void _etk_table_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_table_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Table
 * @return Returns the type on an Etk_Table
 */
Etk_Type *etk_table_type_get()
{
   static Etk_Type *table_type = NULL;

   if (!table_type)
   {
      table_type = etk_type_new("Etk_Table", ETK_CONTAINER_TYPE, sizeof(Etk_Table), ETK_CONSTRUCTOR(_etk_table_constructor), ETK_DESTRUCTOR(_etk_table_destructor));
   
      etk_type_property_add(table_type, "num_cols", ETK_TABLE_NUM_COLS_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(0));
      etk_type_property_add(table_type, "num_rows", ETK_TABLE_NUM_ROWS_PROPERTY, ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(0));
      etk_type_property_add(table_type, "homogeneous", ETK_TABLE_HOMOGENEOUS_PROPERTY, ETK_PROPERTY_BOOL, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_bool(ETK_FALSE));
      
      table_type->property_set = _etk_table_property_set;
      table_type->property_get = _etk_table_property_get;
   }

   return table_type;
}

/**
 * @brief Creates a new table
 * @param num_cols the number of columns of the new table
 * @param num_rows the number of rows of the new table
 * @param homogeneous if homogeneous != 0, all the cells will have the same size
 */
Etk_Widget *etk_table_new(int num_cols, int num_rows, Etk_Bool homogeneous)
{
   Etk_Widget *new_widget;
   Etk_Table *new_table;

   new_widget = etk_widget_new(ETK_TABLE_TYPE, NULL);
   new_table = ETK_TABLE(new_widget);
   etk_table_resize(new_table, num_cols, num_rows);
   etk_table_homogeneous_set(new_table, homogeneous);

   return new_widget;
}

/**
 * @brief Clears the cell (@a col, @a row): removes the widget that is in this cell @n
 * (and in the neighbour cells if the the widget is attach to several cells)
 * @param table a table
 * @param col the column in which the cell is
 * @param row the row in which the cell is
 */
void etk_table_cell_clear(Etk_Table *table, int col, int row)
{
   Evas_List *l;
   Etk_Table_Cell *cell;
   Etk_Widget *child;
   int i, j;

   if (!table || !table->cells || col < 0 || col > table->num_cols - 1 || row < 0 || row > table->num_rows - 1)
      return;
   if (!(cell = table->cells[ETK_TABLE_CELL_INDEX(table, col, row)]) || !(child = cell->child))
      return;
   if (!(l = evas_list_find_list(table->children, child)))
      return;

   for (i = cell->left_attach; i <= cell->right_attach; i++)
   {
      for (j = cell->top_attach; j <= cell->bottom_attach; j++)
         table->cells[ETK_TABLE_CELL_INDEX(table, i, j)] = NULL;
   }
   free(cell);

   table->children = evas_list_remove_list(table->children, l);
   etk_widget_parent_set(child, NULL);
   etk_signal_emit_by_name("child_removed", ETK_OBJECT(table), NULL, child);
}

/**
 * @brief Resizes the table
 * @param table a table
 * @param num_rows the new number of rows
 * @param num_cols the new number of cols 
 */
void etk_table_resize(Etk_Table *table, int num_cols, int num_rows)
{
   Evas_List *l;
   Etk_Table_Cell **new_cells;
   Etk_Table_Cell *cell;
   Etk_Table_Col_Row *new_cols, *new_rows;
   Etk_Widget *child;
   int i, j;

   if (!table)
      return;

   if (num_cols < 0)
      num_cols = 0;
   if (num_rows < 0)
      num_rows = 0;

   if (num_cols == 0 && num_rows == 0)
   {
      new_cells = NULL;
      new_cols = NULL;
      new_rows = NULL;
   }
   else
   {
      new_cells = calloc(num_cols * num_rows, sizeof(Etk_Table_Cell *));
      new_cols = malloc(num_cols * sizeof(Etk_Table_Col_Row));
      new_rows = malloc(num_rows * sizeof(Etk_Table_Col_Row));
   }
   
   for (l = table->children; l; )
   {
      child = ETK_WIDGET(l->data);
      l = l->next;
      if (!(cell = child->child_properties))
         continue;
      
      /* The child is in the old table but not in the new one: we remove it */
      if (cell->left_attach >= num_cols || cell->top_attach >= num_rows)
         etk_table_cell_clear(table, cell->left_attach, cell->top_attach);
      /* The child is in the new table: we copy it to the new table */
      else
      {
         cell->right_attach = ETK_MIN(num_cols - 1, cell->right_attach);
         cell->bottom_attach = ETK_MIN(num_rows - 1, cell->bottom_attach);
         for (i = cell->left_attach; i <= cell->right_attach; i++)
         {
            for (j = cell->top_attach; j <= cell->bottom_attach; j++)
               new_cells[j * num_cols + i] = cell;
         }
      }
   }

   free(table->cells);
   free(table->cols);
   free(table->rows);

   table->cells = new_cells;
   table->cols = new_cols;
   table->rows = new_rows;
   table->num_cols = num_cols;
   table->num_rows = num_rows;

   etk_widget_size_recalc_queue(ETK_WIDGET(table));
   etk_object_notify(ETK_OBJECT(table), "num_cols");
   etk_object_notify(ETK_OBJECT(table), "num_rows");
}

/**
 * @brief Attachs a child widget to the table
 * @param table a table
 * @param child the child widget to attach
 * @param left_attach the column where the left size of the child will be attached
 * @param right_attach the column where the right size of the child will be attached
 * @param top_attach the row where the top size of the child will be attached
 * @param bottom_attach the row where the bottom size of the child will be attached
 * @param x_padding the space on the left and the right of the child widget
 * @param y_padding the space on the top and the bottom of the child widget
 * @param fill_policy The fill policy of the child
 */
void etk_table_attach(Etk_Table *table, Etk_Widget *child, int left_attach, int right_attach,
   int top_attach, int bottom_attach, int x_padding, int y_padding, Etk_Fill_Policy_Flags fill_policy)
{
   Etk_Table_Cell *cell;
   int i, j;

   if (!table || !table->cells || !child)
      return;

   left_attach = ETK_CLAMP(left_attach, 0, table->num_cols - 1);
   right_attach = ETK_CLAMP(right_attach, left_attach, table->num_cols - 1);
   top_attach = ETK_CLAMP(top_attach, 0, table->num_rows - 1);
   bottom_attach = ETK_CLAMP(bottom_attach, top_attach, table->num_rows - 1);

   if (child->parent && ETK_IS_CONTAINER(child->parent))
      etk_container_remove(ETK_CONTAINER(child->parent), child);
   
   cell = malloc(sizeof(Etk_Table_Cell));
   cell->left_attach = left_attach;
   cell->right_attach = right_attach;
   cell->top_attach = top_attach;
   cell->bottom_attach = bottom_attach;
   cell->x_padding = x_padding;
   cell->y_padding = y_padding;
   cell->fill_policy = fill_policy;
   cell->child = child;
   child->child_properties = cell;

   for (i = left_attach; i <= right_attach; i++)
   {
      for (j = top_attach; j <= bottom_attach; j++)
      {
         etk_table_cell_clear(table, i, j);
         table->cells[ETK_TABLE_CELL_INDEX(table, i, j)] = cell;
      }
   }

   table->children = evas_list_append(table->children, child);
   etk_widget_parent_set(child, ETK_WIDGET(table));
   etk_signal_emit_by_name("child_added", ETK_OBJECT(table), NULL, child);
}

/**
 * @brief Simpler version of etk_table_attach: padding is set to 0 and fill policy is FILL and EXPAND in the both directions.
 * @param table a table
 * @param child the child widget to attach
 * @param left_attach the column where the left size of the child will be attached
 * @param right_attach the column where the right size of the child will be attached
 * @param top_attach the row where the top size of the child will be attached
 * @param bottom_attach the row where the bottom size of the child will be attached
 */
void etk_table_attach_defaults(Etk_Table *table, Etk_Widget *child, int left_attach, int right_attach, int top_attach, int bottom_attach)
{
   etk_table_attach(table, child, left_attach, right_attach, top_attach, bottom_attach,
      0, 0, ETK_FILL_POLICY_HFILL | ETK_FILL_POLICY_VFILL | ETK_FILL_POLICY_HEXPAND | ETK_FILL_POLICY_VEXPAND);
}

/**
 * @brief Changes the homogenous property of the table
 * @param table a table
 * @param homogeneous if homogeneous != 0, all the cells will have the same size
 */
void etk_table_homogeneous_set(Etk_Table *table, Etk_Bool homogeneous)
{
   if (!table)
      return;

   table->homogeneous = homogeneous;
   etk_widget_size_recalc_queue(ETK_WIDGET(table));
   etk_object_notify(ETK_OBJECT(table), "homogeneous");
}

/**
 * @brief Checks if the table is homogenous
 * @param table a table
 * @return Returns ETK_TRUE if the table is homogeneous, ETK_FALSE otherwise
 */
Etk_Bool etk_table_homogeneous_get(Etk_Table *table)
{
   if (!table)
      return ETK_FALSE;
   return table->homogeneous;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the table */
static void _etk_table_constructor(Etk_Table *table)
{
   if (!table)
      return;

   table->children = NULL;
   table->cells = NULL;
   table->cols = NULL;
   table->rows = NULL;
   table->num_cols = 0;
   table->num_rows = 0;
   table->homogeneous = ETK_FALSE;

   ETK_CONTAINER(table)->child_add = _etk_table_child_add;
   ETK_CONTAINER(table)->child_remove = _etk_table_child_remove;
   ETK_CONTAINER(table)->children_get = _etk_table_children_get;
   ETK_WIDGET(table)->size_request = _etk_table_size_request;
   ETK_WIDGET(table)->size_allocate = _etk_table_size_allocate;
}

/* Destroys the table */
static void _etk_table_destructor(Etk_Table *table)
{
   Etk_Widget *child;
   
   if (!table)
      return;
   
   while (table->children)
   {
      child = ETK_WIDGET(table->children->data);
      free(child->child_properties);
      child->child_properties = NULL;
      table->children = evas_list_remove_list(table->children, table->children);
   }
   free(table->cells);
   free(table->cols);
   free(table->rows);
}

/* Sets the property whose id is "property_id" to the value "value" */
static void _etk_table_property_set(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Table *table;

   if (!(table = ETK_TABLE(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TABLE_NUM_COLS_PROPERTY:
         etk_table_resize(table, etk_property_value_int_get(value), table->num_rows);
         break;
      case ETK_TABLE_NUM_ROWS_PROPERTY:
         etk_table_resize(table, table->num_cols, etk_property_value_int_get(value));
         break;
      case ETK_TABLE_HOMOGENEOUS_PROPERTY:
         etk_table_homogeneous_set(table, etk_property_value_bool_get(value));
         break;
      default:
         break;
   }
}

/* Gets the value of the property whose id is "property_id" */
static void _etk_table_property_get(Etk_Object *object, int property_id, Etk_Property_Value *value)
{
   Etk_Table *table;

   if (!(table = ETK_TABLE(object)) || !value)
      return;

   switch (property_id)
   {
      case ETK_TABLE_NUM_COLS_PROPERTY:
         etk_property_value_int_set(value, table->num_cols);
         break;
      case ETK_TABLE_NUM_ROWS_PROPERTY:
         etk_property_value_int_set(value, table->num_rows);
         break;
      case ETK_TABLE_HOMOGENEOUS_PROPERTY:
         etk_property_value_bool_set(value, table->homogeneous);
         break;
      default:
         break;
   }
}

/* Calculates the ideal size of the table */
static void _etk_table_size_request(Etk_Widget *widget, Etk_Size *size_requisition)
{
   Etk_Table *table;
   Etk_Widget *child;
   Etk_Table_Cell *cell;
   Etk_Size child_requisition;
   Evas_List *l;
   int i;

   if (!(table = ETK_TABLE(widget)) || !size_requisition)
      return;

   if (!table->cells)
   {
      size_requisition->w = 0;
      size_requisition->h = 0;
   }
   else
   {
      if (table->homogeneous)
      {
         int max_col_width = ETK_TABLE_CELL_MIN_SIZE, max_row_height = ETK_TABLE_CELL_MIN_SIZE;
         int cell_size;
         Etk_Bool hexpand = ETK_FALSE, vexpand = ETK_FALSE;
   
         /* We calculate the maximum size of a cell */
         for (l = table->children; l; l = l->next)
         {
            child = ETK_WIDGET(l->data);
            if (!(cell = child->child_properties))
               continue;
            etk_widget_size_request(child, &child_requisition);

            cell_size = (2 * cell->x_padding + child_requisition.w) / (cell->right_attach - cell->left_attach + 1);
            if (max_col_width < cell_size)
               max_col_width = cell_size;
            hexpand |= (cell->fill_policy & ETK_FILL_POLICY_HEXPAND);

            cell_size = (2 * cell->y_padding + child_requisition.h) / (cell->bottom_attach - cell->top_attach + 1);
            if (max_row_height < cell_size)
               max_row_height = cell_size;
            vexpand |= (cell->fill_policy & ETK_FILL_POLICY_VEXPAND);
         }

         for (i = 0; i < table->num_cols; i++)
         {
            table->cols[i].requested_size = max_col_width;
            table->cols[i].expand = hexpand;
         }
         for (i = 0; i < table->num_rows; i++)
         {
            table->rows[i].requested_size = max_row_height;
            table->rows[i].expand = vexpand;
         }
   
         size_requisition->w = table->num_cols * max_col_width;
         size_requisition->h = table->num_rows * max_row_height;
      }
      /* Non homogeneous table */
      else
      {
         for (i = 0; i < table->num_cols; i++)
         {
            table->cols[i].requested_size = ETK_TABLE_CELL_MIN_SIZE;
            table->cols[i].expand = ETK_FALSE;
         }
         for (i = 0; i < table->num_rows; i++)
         {
            table->rows[i].requested_size = ETK_TABLE_CELL_MIN_SIZE;
            table->rows[i].expand = ETK_FALSE;
         }

         /* We first treat the children that span only one column or one row */
         for (l = table->children; l; l = l->next)
         {
            child = ETK_WIDGET(l->data);
            if (!(cell = child->child_properties))
               continue;
            etk_widget_size_request(child, &child_requisition);

            /* Cols */
            if (cell->left_attach == cell->right_attach)
            {
               if (table->cols[cell->left_attach].requested_size < child_requisition.w + 2 * cell->x_padding)
                  table->cols[cell->left_attach].requested_size = child_requisition.w + 2 * cell->x_padding;
               table->cols[cell->left_attach].expand |= (cell->fill_policy & ETK_FILL_POLICY_HEXPAND);
            }

            /* Rows */
            if (cell->top_attach == cell->bottom_attach)
            {
               if (table->rows[cell->top_attach].requested_size < child_requisition.h + 2 * cell->y_padding)
                  table->rows[cell->top_attach].requested_size = child_requisition.h + 2 * cell->y_padding;
               table->rows[cell->top_attach].expand |= (cell->fill_policy & ETK_FILL_POLICY_VEXPAND);
            }
         }
         /* Then, we treat the children that span multiple columns or rows */
         for (l = table->children; l; l = l->next)
         {
            int cells_size;
            int num_expandable_cells;
            int free_space;
            float delta;
            Etk_Bool a_cell_already_expands;

            child = ETK_WIDGET(l->data);
            if (!(cell = child->child_properties))
               continue;
            etk_widget_size_request(child, &child_requisition);

            /* Cols */
            if (cell->left_attach < cell->right_attach)
            {
               cells_size = 0;
               num_expandable_cells = 0;
               a_cell_already_expands = ETK_FALSE;
               
               for (i = cell->left_attach; i <= cell->right_attach; i++)
               {
                  if (table->cols[i].expand)
                  {
                     a_cell_already_expands = ETK_TRUE;
                     break;
                  }
               }
               if (!a_cell_already_expands)
               {
                  for (i = cell->left_attach; i <= cell->right_attach; i++)
                     table->cols[i].expand = ETK_TRUE;
               }
                  
               for (i = cell->left_attach; i <= cell->right_attach; i++)
               {
                  cells_size += table->cols[i].requested_size;
                  if (table->cols[i].expand)
                     num_expandable_cells++;;
               }

               free_space = child_requisition.w - cells_size;
               if (free_space > 0)
               {
                  if (num_expandable_cells <= 0)
                  {
                     delta = (float)free_space / (cell->right_attach - cell->left_attach + 1);
                     for (i = cell->left_attach; i <= cell->right_attach; i++)
                     {
                        table->cols[i].expand |= (cell->fill_policy & ETK_FILL_POLICY_HEXPAND);
                        table->cols[i].requested_size += (int)(delta * (i + 1)) - (int)(delta * i);
                     }
                  }
                  else
                  {
                     delta = (float)free_space / num_expandable_cells;
                     for (i = cell->left_attach; i <= cell->right_attach; i++)
                     {
                        if (table->cols[i].expand)
                           table->cols[i].requested_size += (int)(delta * (i + 1)) - (int)(delta * i);
                     }
                  }
               }
            }

            /* Rows */
            if (cell->top_attach < cell->bottom_attach)
            {
               cells_size = 0;
               num_expandable_cells = 0;
               a_cell_already_expands = ETK_FALSE;
               
               for (i = cell->top_attach; i <= cell->bottom_attach; i++)
               {
                  if (table->rows[i].expand)
                  {
                     a_cell_already_expands = ETK_TRUE;
                     break;
                  }
               }
               if (!a_cell_already_expands)
               {
                  for (i = cell->top_attach; i <= cell->bottom_attach; i++)
                     table->rows[i].expand = ETK_TRUE;
               }
               
               for (i = cell->top_attach; i <= cell->bottom_attach; i++)
               {
                  cells_size += table->rows[i].requested_size;
                  if (table->rows[i].expand)
                     num_expandable_cells++;;
               }

               free_space = child_requisition.h - cells_size;
               if (free_space > 0)
               {
                  if (num_expandable_cells <= 0)
                  {
                     delta = (float)free_space / (cell->bottom_attach - cell->top_attach + 1);
                     for (i = cell->top_attach; i <= cell->bottom_attach; i++)
                     {
                        table->rows[i].expand |= (cell->fill_policy & ETK_FILL_POLICY_VEXPAND);
                        table->rows[i].requested_size += (int)(delta * (i + 1)) - (int)(delta * i);
                     }
                  }
                  else
                  {
                     delta = (float)free_space / num_expandable_cells;
                     for (i = cell->top_attach; i <= cell->bottom_attach; i++)
                     {
                        if (table->rows[i].expand)
                           table->rows[i].requested_size += (int)(delta * (i + 1)) - (int)(delta * i);
                     }
                  }
               }
            }
         }

         size_requisition->w = 0;
         for (i = 0; i < table->num_cols; i++)
            size_requisition->w += table->cols[i].requested_size;
         size_requisition->h = 0;
         for (i = 0; i < table->num_rows; i++)
            size_requisition->h += table->rows[i].requested_size;
      }
   }

   size_requisition->w += 2 * ETK_CONTAINER(table)->border_width;
   size_requisition->h += 2 * ETK_CONTAINER(table)->border_width;
}

/* Resizes the table to the size allocation */
static void _etk_table_size_allocate(Etk_Widget *widget, Etk_Geometry geometry)
{
   Etk_Table *table;
   Etk_Container *container;
   Etk_Widget *child;
   Etk_Table_Cell *cell;
   Etk_Size requested_inner_size;
   Etk_Size allocated_inner_size;
   Etk_Geometry child_geometry;
   float offset, size;
   Evas_List *l;
   int i;

   if (!(table = ETK_TABLE(widget)) || !table->cells)
      return;
   container = ETK_CONTAINER(table);

   etk_widget_size_request(widget, &requested_inner_size);
   requested_inner_size.w -= 2 * container->border_width;
   requested_inner_size.h -= 2 * container->border_width;
   allocated_inner_size.w = geometry.w - 2 * container->border_width;
   allocated_inner_size.h = geometry.h - 2 * container->border_width;

   /* We calculate the size of the cols */
   offset = 0;
   if (requested_inner_size.w >= allocated_inner_size.w)
   {
      float ratio;

      ratio = (float)allocated_inner_size.w / requested_inner_size.w;
      for (i = 0; i < table->num_cols; i++)
      {
         size = table->cols[i].requested_size * ratio;
         table->cols[i].size = size;
         table->cols[i].offset = offset;
         offset += size;
      }
   }
   else
   {
      int num_cols_to_expand = 0;
      int free_space;

      for (i = 0; i < table->num_cols; i++)
      {
         if (table->cols[i].expand)
            num_cols_to_expand++;
      }
      free_space = allocated_inner_size.w - requested_inner_size.w;

      for (i = 0; i < table->num_cols; i++)
      {
         if (table->cols[i].expand)
            size = table->cols[i].requested_size + ((float)free_space / num_cols_to_expand);
         else
            size = table->cols[i].requested_size;

         table->cols[i].size = size;
         table->cols[i].offset = offset;
         offset += size;
      }
   }

   /* We calculate the size of the rows */
   offset = 0;
   if (requested_inner_size.h >= allocated_inner_size.h)
   {
      float ratio;

      ratio = (float)allocated_inner_size.h / requested_inner_size.h;
      for (i = 0; i < table->num_rows; i++)
      {
         size = table->rows[i].requested_size * ratio;
         table->rows[i].size = size;
         table->rows[i].offset = offset;
         offset += size;
      }
   }
   else
   {
      int num_rows_to_expand = 0;
      int free_space;

      for (i = 0; i < table->num_rows; i++)
      {
         if (table->rows[i].expand)
            num_rows_to_expand++;
      }
      free_space = allocated_inner_size.h - requested_inner_size.h;

      for (i = 0; i < table->num_rows; i++)
      {
         if (table->rows[i].expand)
            size = table->rows[i].requested_size + ((float)free_space / num_rows_to_expand);
         else
            size = table->rows[i].requested_size;
         
         table->rows[i].size = size;
         table->rows[i].offset = offset;
         offset += size;
      }
   }

   /* We allocate the size for the children */
   for (l = table->children; l; l = l->next)
   {
      child = ETK_WIDGET(l->data);
      if (!(cell = child->child_properties))
         continue;
      
      child_geometry.x = geometry.x + table->cols[cell->left_attach].offset + cell->x_padding;
      child_geometry.y = geometry.y + table->rows[cell->top_attach].offset + cell->y_padding;
      child_geometry.w = table->cols[cell->right_attach].offset - table->cols[cell->left_attach].offset +
         table->cols[cell->right_attach].size - 2 * cell->x_padding;
      child_geometry.h = table->rows[cell->bottom_attach].offset - table->rows[cell->top_attach].offset +
         table->rows[cell->bottom_attach].size - 2 * cell->y_padding;

      etk_container_child_space_fill(child, &child_geometry, cell->fill_policy & ETK_FILL_POLICY_HFILL, cell->fill_policy & ETK_FILL_POLICY_VFILL);
      etk_widget_size_allocate(child, child_geometry);
   }
}

/**************************
 *
 * Callbacks and handlers
 *
 **************************/

/* Adds a child to the table */
static void _etk_table_child_add(Etk_Container *container, Etk_Widget *widget)
{
   etk_table_attach_defaults(ETK_TABLE(container), widget, 0, 0, 0, 0);
}

/* Removes the child from the table */
static void _etk_table_child_remove(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Table *table;
   Etk_Table_Cell *cell;

   if (!(table = ETK_TABLE(container)) || !widget || (widget->parent != ETK_WIDGET(container)))
      return;
   if (!(cell = widget->child_properties))
      return;

   etk_table_cell_clear(table, cell->left_attach, cell->top_attach);
}

/* Gets the list of the children */
static Evas_List *_etk_table_children_get(Etk_Container *container)
{
   Etk_Table *table;
   
   if (!(table = ETK_TABLE(container)))
      return NULL;
   return table->children;
}

/** @} */
