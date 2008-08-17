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

/** @file etk_table.h */
#ifndef _ETK_TABLE_H_
#define _ETK_TABLE_H_

#include <Evas.h>

#include "etk_container.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Table Etk_Table
 * @brief The Etk_Table widget is a container that can contain several children aligned on a grid
 * @{
 */

/** Gets the type of a table */
#define ETK_TABLE_TYPE       (etk_table_type_get())
/** Casts the object to an Etk_Table */
#define ETK_TABLE(obj)       (ETK_OBJECT_CAST((obj), ETK_TABLE_TYPE, Etk_Table))
/** Checks if the object is an Etk_Table */
#define ETK_IS_TABLE(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TABLE_TYPE))


/** @brief Describes whether the table should be homogenous horizontally, vertically,
 * in both directions or not at all */
typedef enum
{
   ETK_TABLE_NOT_HOMOGENEOUS = 0,       /**< The table is not homogeneous: its cells could have different size */
   ETK_TABLE_HHOMOGENEOUS = 1 << 0,     /**< The table is homogeneous horizontally. all its cells have the same width */
   ETK_TABLE_VHOMOGENEOUS = 1 << 1,     /**< The table is homogeneous vertically. all its cells have the same height */
   ETK_TABLE_HOMOGENEOUS = ETK_TABLE_HHOMOGENEOUS | ETK_TABLE_VHOMOGENEOUS   /**< Equivalent to ETK_TABLE_HHOMOGENEOUS | ETK_TABLE_VHOMOGENEOUS */
} Etk_Table_Homogeneous;

/** @brief Describes how a child of the table should fill and expand the space allocated for it */
typedef enum
{
   ETK_TABLE_NONE = 0,            /**< The child doesn't fill or expand */
   ETK_TABLE_HFILL = 1 << 0,      /**< The child fills all the horizontal space allocated to it */
   ETK_TABLE_VFILL = 1 << 1,      /**< The child fills all the vertical space allocated to it */
   ETK_TABLE_HEXPAND = 1 << 2,    /**< The cell containing the child will expand horizontally */
   ETK_TABLE_VEXPAND = 1 << 3,    /**< The cell containing the child will expand vertically */
   ETK_TABLE_FILL = ETK_TABLE_HFILL | ETK_TABLE_VFILL,         /**< Equivalent to ETK_TABLE_HFILL | ETK_TABLE_VFILL */
   ETK_TABLE_EXPAND = ETK_TABLE_HEXPAND | ETK_TABLE_VEXPAND,   /**< Equivalent to ETK_TABLE_HEXPAND | ETK_TABLE_VEXPAND */
   ETK_TABLE_EXPAND_FILL = ETK_TABLE_EXPAND | ETK_TABLE_FILL   /**< Equivalent to ETK_TABLE_EXPAND | ETK_TABLE_FILL */
} Etk_Table_Fill_Policy;


/**
 * @internal
 * @brief A cell of a table
 */
typedef struct Etk_Table_Cell
{
   /* private: */
   int left_attach;
   int right_attach;
   int top_attach;
   int bottom_attach;
   int x_padding;
   int y_padding;
   Etk_Table_Fill_Policy fill_policy;
   Etk_Widget *child;
   Evas_List *node;
} Etk_Table_Cell;

/**
 * @internal
 * @brief A column or a row of a table
 */
typedef struct Etk_Table_Col_Row
{
   /* private: */
   int requested_size;
   int size;
   int offset;
   Etk_Bool expand:1;
} Etk_Table_Col_Row;

/**
 * @brief @widget a container that can contain several children aligned on a grid
 * @structinfo
 */
struct Etk_Table
{
   /* private: */
   /* Inherit from Etk_Container */
   Etk_Container container;

   Etk_Table_Cell **cells;
   Evas_List *cells_list;
   Etk_Table_Col_Row *cols;
   Etk_Table_Col_Row *rows;

   int num_cols;
   int num_rows;
   Etk_Table_Homogeneous homogeneous;
};


Etk_Type   *etk_table_type_get(void);
Etk_Widget *etk_table_new(int num_cols, int num_rows, Etk_Table_Homogeneous homogeneous);

void        etk_table_cell_clear(Etk_Table *table, int col, int row);
void        etk_table_resize(Etk_Table *table, int num_cols, int num_rows);

void        etk_table_attach(Etk_Table *table, Etk_Widget *child, int left_attach, int right_attach, int top_attach, int bottom_attach, Etk_Table_Fill_Policy fill_policy, int x_padding, int y_padding);
void        etk_table_attach_default(Etk_Table *table, Etk_Widget *child, int left_attach, int right_attach, int top_attach, int bottom_attach);

void        etk_table_homogeneous_set(Etk_Table *table, Etk_Table_Homogeneous homogeneous);
Etk_Table_Homogeneous etk_table_homogeneous_get(Etk_Table *table);

void etk_table_child_position_get(Etk_Table *table, Etk_Widget *child, int *left_attach, int *right_attach, int *top_attach, int *bottom_attach);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
