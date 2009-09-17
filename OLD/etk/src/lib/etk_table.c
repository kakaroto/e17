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

/** @file etk_table.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_table.h"

#include <stdlib.h>

#include "etk_signal.h"
#include "etk_signal_callback.h"
#include "etk_utils.h"

/**
 * @addtogroup Etk_Table
 * @{
 */

#define CELL_INDEX(table, col, row)   ((row) * (table)->num_cols + (col))
#define CELL_MIN_SIZE 10

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
static Eina_List *_etk_table_children_get(Etk_Container *container);
static void _etk_table_size_request(Etk_Widget *widget, Etk_Size *size_requisition);
static void _etk_table_size_allocate(Etk_Widget *widget, Etk_Geometry geometry);

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Gets the type of an Etk_Table
 * @return Returns the type of an Etk_Table
 */
Etk_Type *etk_table_type_get(void)
{
   static Etk_Type *table_type = NULL;

   if (!table_type)
   {
      table_type = etk_type_new("Etk_Table", ETK_CONTAINER_TYPE, sizeof(Etk_Table),
            ETK_CONSTRUCTOR(_etk_table_constructor), ETK_DESTRUCTOR(_etk_table_destructor), NULL);

      etk_type_property_add(table_type, "num-cols", ETK_TABLE_NUM_COLS_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(0));
      etk_type_property_add(table_type, "num-rows", ETK_TABLE_NUM_ROWS_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(0));
      etk_type_property_add(table_type, "homogeneous", ETK_TABLE_HOMOGENEOUS_PROPERTY,
            ETK_PROPERTY_INT, ETK_PROPERTY_READABLE_WRITABLE,  etk_property_value_int(ETK_TABLE_NOT_HOMOGENEOUS));

      table_type->property_set = _etk_table_property_set;
      table_type->property_get = _etk_table_property_get;
   }

   return table_type;
}

/**
 * @brief Creates a new table
 * @param num_cols the number of columns of the new table
 * @param num_rows the number of rows of the new table
 * @param homogeneous a flag describing whether the table should be homogenous horizontally, vertically,
 * in both directions or not at all
 * @return Returns the new table
 */
Etk_Widget *etk_table_new(int num_cols, int num_rows, Etk_Table_Homogeneous homogeneous)
{
   return etk_widget_new(ETK_TABLE_TYPE, "homogeneous", homogeneous,
         "num-cols", num_cols, "num-rows", num_rows, NULL);
}

/**
 * @brief Clears the cell ( @a col, @a row ): it removes from the table the widget that is in this cell
 * @param table a table
 * @param col the column in which the widget to remove is (starting from 0)
 * @param row the row in which the widget to remove is (starting from 0)
 */
void etk_table_cell_clear(Etk_Table *table, int col, int row)
{
   Etk_Table_Cell *cell;
   Etk_Widget *child;
   int i, j;

   if (!table || !table->cells || col < 0 || col > table->num_cols - 1 || row < 0 || row > table->num_rows - 1)
      return;
   if (!(cell = table->cells[CELL_INDEX(table, col, row)]) || !(child = cell->child))
      return;

   for (i = cell->left_attach; i <= cell->right_attach; i++)
   {
      for (j = cell->top_attach; j <= cell->bottom_attach; j++)
         table->cells[CELL_INDEX(table, i, j)] = NULL;
   }

   table->cells_list = eina_list_remove_list(table->cells_list, cell->node);
   free(cell);

   etk_object_data_set(ETK_OBJECT(child), "_Etk_Table::Cell", NULL);
   etk_widget_parent_set(child, NULL);
   etk_signal_emit(ETK_CONTAINER_CHILD_REMOVED_SIGNAL, ETK_OBJECT(table), child);
}

/**
 * @brief Resizes the table. The children that are attached to a row or a column that is removed will be unparented
 * @param table a table
 * @param num_rows the new number of rows
 * @param num_cols the new number of cols
 */
void etk_table_resize(Etk_Table *table, int num_cols, int num_rows)
{
   Eina_List *l, *next;
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

   for (l = table->cells_list; l; l = next)
   {
      next = l->next;
      cell = l->data;
      child = cell->child;

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
   etk_object_notify(ETK_OBJECT(table), "num-cols");
   etk_object_notify(ETK_OBJECT(table), "num-rows");
}

/**
 * @brief Attachs a widget to the table
 * @param table a table
 * @param child the widget to attach
 * @param left_attach the column where the left side of the child will be attached (starting from 0)
 * @param right_attach the column where the right side of the child will be attached (starting from 0)
 * @param top_attach the row where the top side of the child will be attached (starting from 0)
 * @param bottom_attach the row where the bottom side of the child will be attached (starting from 0)
 * @param fill_policy The fill policy of the child
 * @param x_padding the amount of free space on the left and on the right sides of the child widget
 * @param y_padding the amount of free space on the top and on the bottom sides of the child widget
 */
void etk_table_attach(Etk_Table *table, Etk_Widget *child, int left_attach, int right_attach, int top_attach, int bottom_attach, Etk_Table_Fill_Policy fill_policy, int x_padding, int y_padding)
{
   Etk_Table_Cell *cell;
   int i, j;

   if (!table || !table->cells || !child)
      return;

   left_attach = ETK_CLAMP(left_attach, 0, table->num_cols - 1);
   right_attach = ETK_CLAMP(right_attach, left_attach, table->num_cols - 1);
   top_attach = ETK_CLAMP(top_attach, 0, table->num_rows - 1);
   bottom_attach = ETK_CLAMP(bottom_attach, top_attach, table->num_rows - 1);

   cell = malloc(sizeof(Etk_Table_Cell));
   cell->left_attach = left_attach;
   cell->right_attach = right_attach;
   cell->top_attach = top_attach;
   cell->bottom_attach = bottom_attach;
   cell->x_padding = x_padding;
   cell->y_padding = y_padding;
   cell->fill_policy = fill_policy;
   cell->child = child;

   for (i = left_attach; i <= right_attach; i++)
   {
      for (j = top_attach; j <= bottom_attach; j++)
      {
         etk_table_cell_clear(table, i, j);
         table->cells[CELL_INDEX(table, i, j)] = cell;
      }
   }

   table->cells_list = eina_list_append(table->cells_list, cell);
   cell->node = eina_list_last(table->cells_list);

   etk_object_data_set(ETK_OBJECT(child), "_Etk_Table::Cell", cell);
   etk_widget_parent_set(child, ETK_WIDGET(table));
   etk_signal_emit(ETK_CONTAINER_CHILD_ADDED_SIGNAL, ETK_OBJECT(table), child);
}

/**
 * @brief Same as etk_table_attach() but with default settings: padding is set to 0 and
 * the fill policy is ETK_TABLE_EXPAND_FILL.
 * @param table a table
 * @param child the widget to attach
 * @param left_attach the column where the left side of the child will be attached (starting from 0)
 * @param right_attach the column where the right side of the child will be attached (starting from 0)
 * @param top_attach the row where the top side of the child will be attached (starting from 0)
 * @param bottom_attach the row where the bottom side of the child will be attached (starting from 0)
 */
void etk_table_attach_default(Etk_Table *table, Etk_Widget *child, int left_attach, int right_attach, int top_attach, int bottom_attach)
{
   etk_table_attach(table, child, left_attach, right_attach, top_attach, bottom_attach, ETK_TABLE_EXPAND_FILL, 0, 0);
}

/**
 * @brief Changes the homogenous property of the table
 * @param table a table
 * @param homogeneous a flag describing whether the table should be homogenous horizontally, vertically,
 * in both directions or not at all
 */
void etk_table_homogeneous_set(Etk_Table *table, Etk_Table_Homogeneous homogeneous)
{
   if (!table)
      return;

   table->homogeneous = homogeneous;
   etk_widget_size_recalc_queue(ETK_WIDGET(table));
   etk_object_notify(ETK_OBJECT(table), "homogeneous");
}

/**
 * @brief Gets whether the table is homogenous horizontally, vertically, in both directions or not at all
 * @param table a table
 * @return Returns the homogenous property of the table
 */
Etk_Table_Homogeneous etk_table_homogeneous_get(Etk_Table *table)
{
   if (!table)
      return ETK_TABLE_NOT_HOMOGENEOUS;
   return table->homogeneous;
}

/**
 * @brief Gets the col and row of the given child
 * @param table a table
 * @param left_attach the variable to store the left_attach
 * @param right_attach the variable to store the right_attach
 * @param top_attach the variable to store the top_attach
 * @param bottom_attach the variable to store the bottom_attach
 */
void etk_table_child_position_get(Etk_Table *table, Etk_Widget *child, int *left_attach, int *right_attach, int *top_attach, int *bottom_attach)
{
   int i, j;

   if (left_attach)
      *left_attach = -1;
   if (right_attach)
      *right_attach = -1;
   if (top_attach)
      *top_attach = -1;
   if (bottom_attach)
      *bottom_attach = -1;

   if (!table || !child)
      return;

   for (i = 0; i < table->num_cols; i++)
   {
      for (j = 0; j < table->num_rows; j++)
      {
	 Etk_Table_Cell *cell = NULL;

         cell = table->cells[CELL_INDEX(table, i, j)];
	 if (cell && cell->child == child)
	 {
	    if (left_attach)
	       *left_attach = cell->left_attach;
	    if (right_attach)
	       *right_attach = cell->right_attach;
	    if (top_attach)
	       *top_attach = cell->top_attach;
	    if (bottom_attach)
	       *bottom_attach = cell->bottom_attach;
	    return;
	 }
      }
   }
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

   table->cells = NULL;
   table->cells_list = NULL;
   table->cols = NULL;
   table->rows = NULL;
   table->num_cols = 0;
   table->num_rows = 0;
   table->homogeneous = ETK_TABLE_NOT_HOMOGENEOUS;

   ETK_CONTAINER(table)->child_add = _etk_table_child_add;
   ETK_CONTAINER(table)->child_remove = _etk_table_child_remove;
   ETK_CONTAINER(table)->children_get = _etk_table_children_get;
   ETK_WIDGET(table)->size_request = _etk_table_size_request;
   ETK_WIDGET(table)->size_allocate = _etk_table_size_allocate;
}

/* Destroys the table */
static void _etk_table_destructor(Etk_Table *table)
{
   Etk_Table_Cell *cell;

   if (!table)
      return;

   while (table->cells_list)
   {
      cell = table->cells_list->data;
      etk_table_cell_clear(table, cell->left_attach, cell->top_attach);
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
         etk_table_homogeneous_set(table, etk_property_value_int_get(value));
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
         etk_property_value_int_set(value, table->homogeneous);
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
   Etk_Size child_size;
   Etk_Bool hexpand = ETK_FALSE, vexpand = ETK_FALSE;
   Eina_List *l;
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
      /*********************************************
       * Phase 1: We calculate the width of the cols of the table
       *********************************************/

      /* Horizontally-homogeneous table */
      if (table->homogeneous & ETK_TABLE_HHOMOGENEOUS)
      {
         int max_col_width = CELL_MIN_SIZE;
         int cell_size;

         /* We calculate the maximum size of a cell */
         for (l = table->cells_list; l; l = l->next)
         {
            cell = l->data;
            child = cell->child;
            etk_widget_size_request(child, &child_size);

            cell_size = (2 * cell->x_padding + child_size.w) / (cell->right_attach - cell->left_attach + 1);
            if (max_col_width < cell_size)
               max_col_width = cell_size;
            hexpand |= (cell->fill_policy & ETK_TABLE_HEXPAND);
         }

         for (i = 0; i < table->num_cols; i++)
         {
            table->cols[i].requested_size = max_col_width;
            table->cols[i].expand = !!hexpand;
         }

         size_requisition->w = table->num_cols * max_col_width;
      }
      /* Non horizontally-homogeneous table */
      else
      {
         for (i = 0; i < table->num_cols; i++)
         {
            table->cols[i].requested_size = CELL_MIN_SIZE;
            table->cols[i].expand = ETK_FALSE;
         }

         /* We first treat the children that span only one column */
         for (l = table->cells_list; l; l = l->next)
         {
            cell = l->data;
            child = cell->child;

            if (cell->left_attach == cell->right_attach)
            {
               etk_widget_size_request(child, &child_size);
               if (table->cols[cell->left_attach].requested_size < child_size.w + 2 * cell->x_padding)
                  table->cols[cell->left_attach].requested_size = child_size.w + 2 * cell->x_padding;
               table->cols[cell->left_attach].expand |= ((cell->fill_policy & ETK_TABLE_HEXPAND) != 0);
               hexpand |= (cell->fill_policy & ETK_TABLE_HEXPAND);
            }
         }
         /* Then, we treat the children that span several columns */
         for (l = table->cells_list; l; l = l->next)
         {
            int cells_size;
            int num_expandable_cells;
            int free_space;
            float delta;

            cell = l->data;
            child = cell->child;

            if (cell->left_attach < cell->right_attach)
            {
               cells_size = 0;
               num_expandable_cells = 0;

               etk_widget_size_request(child, &child_size);
               for (i = cell->left_attach; i <= cell->right_attach; i++)
               {
                  cells_size += table->cols[i].requested_size;
                  if (!hexpand && (cell->fill_policy & ETK_TABLE_HEXPAND))
                     table->cols[i].expand = ETK_TRUE;
                  if (table->cols[i].expand)
                     num_expandable_cells++;
               }

               free_space = child_size.w - cells_size;
               if (free_space > 0)
               {
                  if (num_expandable_cells <= 0)
                  {
                     delta = (float)free_space / (cell->right_attach - cell->left_attach + 1);
                     for (i = cell->left_attach; i <= cell->right_attach; i++)
                        table->cols[i].requested_size += (int)(delta * (i + 1)) - (int)(delta * i);
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
         }

         size_requisition->w = 0;
         for (i = 0; i < table->num_cols; i++)
            size_requisition->w += table->cols[i].requested_size;
      }


      /*********************************************
       * Phase 2: We calculate the height of the rows of the table
       *********************************************/

      /* Vertically-homogeneous table */
      if (table->homogeneous & ETK_TABLE_VHOMOGENEOUS)
      {
         int max_row_height = CELL_MIN_SIZE;
         int cell_size;

         /* We calculate the maximum size of a cell */
         for (l = table->cells_list; l; l = l->next)
         {
            cell = l->data;
            child = cell->child;
            etk_widget_size_request(child, &child_size);

            cell_size = (2 * cell->y_padding + child_size.h) / (cell->bottom_attach - cell->top_attach + 1);
            if (max_row_height < cell_size)
               max_row_height = cell_size;
            vexpand |= (cell->fill_policy & ETK_TABLE_VEXPAND);
         }

         for (i = 0; i < table->num_rows; i++)
         {
            table->rows[i].requested_size = max_row_height;
            table->rows[i].expand = !!vexpand;
         }

         size_requisition->h = table->num_rows * max_row_height;
      }
      /* Non vertically-homogeneous table */
      else
      {
         for (i = 0; i < table->num_rows; i++)
         {
            table->rows[i].requested_size = CELL_MIN_SIZE;
            table->rows[i].expand = ETK_FALSE;
         }

         /* We first treat the children that span only one row */
         for (l = table->cells_list; l; l = l->next)
         {
            cell = l->data;
            child = cell->child;

            if (cell->top_attach == cell->bottom_attach)
            {
               etk_widget_size_request(child, &child_size);
               if (table->rows[cell->top_attach].requested_size < child_size.h + 2 * cell->y_padding)
                  table->rows[cell->top_attach].requested_size = child_size.h + 2 * cell->y_padding;
               table->rows[cell->top_attach].expand |= ((cell->fill_policy & ETK_TABLE_VEXPAND) != 0);
               vexpand |= (cell->fill_policy & ETK_TABLE_VEXPAND);
            }
         }
         /* Then, we treat the children that span several rows */
         for (l = table->cells_list; l; l = l->next)
         {
            int cells_size;
            int num_expandable_cells;
            int free_space;
            float delta;

            cell = l->data;
            child = cell->child;

            if (cell->top_attach < cell->bottom_attach)
            {
               cells_size = 0;
               num_expandable_cells = 0;

               etk_widget_size_request(child, &child_size);
               for (i = cell->top_attach; i <= cell->bottom_attach; i++)
               {
                  cells_size += table->rows[i].requested_size;
                  if (!vexpand && (cell->fill_policy & ETK_TABLE_VEXPAND))
                     table->rows[i].expand = ETK_TRUE;
                  if (table->rows[i].expand)
                     num_expandable_cells++;
               }

               free_space = child_size.h - cells_size;
               if (free_space > 0)
               {
                  if (num_expandable_cells <= 0)
                  {
                     delta = (float)free_space / (cell->bottom_attach - cell->top_attach + 1);
                     for (i = cell->top_attach; i <= cell->bottom_attach; i++)
                        table->rows[i].requested_size += (int)(delta * (i + 1)) - (int)(delta * i);
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
   Eina_List *l;
   int i;

   if (!(table = ETK_TABLE(widget)) || !table->cells)
      return;
   container = ETK_CONTAINER(table);

   _etk_table_size_request(widget, &requested_inner_size);
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
   for (l = table->cells_list; l; l = l->next)
   {
      cell = l->data;
      child = cell->child;

      child_geometry.x = geometry.x + table->cols[cell->left_attach].offset + cell->x_padding;
      child_geometry.y = geometry.y + table->rows[cell->top_attach].offset + cell->y_padding;
      child_geometry.w = table->cols[cell->right_attach].offset - table->cols[cell->left_attach].offset +
            table->cols[cell->right_attach].size - 2 * cell->x_padding;
      child_geometry.h = table->rows[cell->bottom_attach].offset - table->rows[cell->top_attach].offset +
            table->rows[cell->bottom_attach].size - 2 * cell->y_padding;

      etk_container_child_space_fill(child, &child_geometry, cell->fill_policy & ETK_TABLE_HFILL,
            cell->fill_policy & ETK_TABLE_VFILL, 0.5, 0.5);
      etk_widget_size_allocate(child, child_geometry);
   }
}

/* Adds a child to the table */
static void _etk_table_child_add(Etk_Container *container, Etk_Widget *widget)
{
   etk_table_attach_default(ETK_TABLE(container), widget, 0, 0, 0, 0);
}

/* Removes the child from the table */
static void _etk_table_child_remove(Etk_Container *container, Etk_Widget *widget)
{
   Etk_Table *table;
   Etk_Table_Cell *cell;

   if (!(table = ETK_TABLE(container)) || !widget)
      return;
   if (!(cell = etk_object_data_get(ETK_OBJECT(widget), "_Etk_Table::Cell")))
      return;
   etk_table_cell_clear(table, cell->left_attach, cell->top_attach);
}

/* Gets the list of the children */
static Eina_List *_etk_table_children_get(Etk_Container *container)
{
   Etk_Table *table;
   Etk_Table_Cell *cell;
   Eina_List *children, *l;

   if (!(table = ETK_TABLE(container)))
      return NULL;

   children = NULL;
   for (l = table->cells_list; l; l = l->next)
   {
      cell = l->data;
      children = eina_list_append(children, cell->child);
   }
   return children;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Table
 *
 * To create a table, you have to call etk_table_new(). It requires the number of columns and rows of the table, and
 * also the homogeneous property of the table. A table can be homogeneous horizontally, vertically, in both directions
 * or not at all. If a table is homogeneous in a direction, it means that all its cells will have the same size in this
 * direction. For example, ETK_TABLE_VHOMOGENEOUS would mean that all the cells of the table have the same height. @n
 *
 * To pack a widget into the table, you have to call etk_table_attach(). It takes four attach-points that indicates
 * where the widget should be packed (one attach-point for each side of the widget: left, right, top, bottom). For
 * example, to attach a widget in the cell (2, 1), you'll have to call etk_table_attach(table, widget, 2, 2, 1, 1, ...).
 * To make the widget span three columns (from column 2 to column 4), you'll have to call
 * etk_table_attach(table, widget, 2, 4, 1, 1, ...). @n
 *
 * A table can also be resized with etk_table_resize().
 *
 * \par Object Hierarchy:
 * - Etk_Object
 *   - Etk_Widget
 *     - Etk_Container
 *       - Etk_Table
 *
 * \par Properties:
 * @prop_name "num-cols": The number of columns of the table
 * @prop_type Integer
 * @prop_rw
 * @prop_val 0
 * \par
 * @prop_name "num-rows": The number of rows of the table
 * @prop_type Integer
 * @prop_rw
 * @prop_val 0
 * \par
 * @prop_name "homogeneous": A flag describing whether the table is homogenous horizontally, vertically,
 * in both directions or not at all
 * @prop_type Integer (Etk_Table_Homogeneous)
 * @prop_rw
 * @prop_val ETK_TABLE_NOT_HOMOGENEOUS
 */
