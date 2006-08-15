/** @file etk_table.h */
#ifndef _ETK_TABLE_H_
#define _ETK_TABLE_H_

#include "etk_container.h"
#include <Evas.h>
#include "etk_types.h"

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

/** @brief Describes how a child of the table should fill and expand the space allocated to it */ 
typedef enum Etk_Table_Fill_Policy
{
   ETK_TABLE_NONE = 1 << 0,       /**< The child doesn't fill or expand */
   ETK_TABLE_HFILL = 1 << 1,      /**< The child fills all the horizontal space allocated to it */
   ETK_TABLE_VFILL = 1 << 2,      /**< The child fills all the vertical space allocated to it */
   ETK_TABLE_HEXPAND = 1 << 3,    /**< The child expands horizontally: the cell containing the child */
                                  /**< will take as much place as possible in the horizontal direction */
   ETK_TABLE_VEXPAND = 1 << 4,    /**< The child expands vertically: the cell containing the child */
                                  /**< will take as much place as possible in the vertical direction */
   ETK_TABLE_FILL = ETK_TABLE_HFILL | ETK_TABLE_VFILL,         /**< Equivalent to ETK_TABLE_HFILL | ETK_TABLE_VFILL */
   ETK_TABLE_EXPAND = ETK_TABLE_HEXPAND | ETK_TABLE_VEXPAND,   /**< Equivalent to ETK_TABLE_HEXPAND | ETK_TABLE_VEXPAND */
   ETK_TABLE_EXPAND_FILL = ETK_TABLE_EXPAND | ETK_TABLE_FILL,  /**< Equivalent to ETK_TABLE_EXPAND | ETK_TABLE_FILL */
} Etk_Table_Fill_Policy;

/* A cell of a table */
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
} Etk_Table_Cell;

/* A column or a row of a table */
typedef struct Etk_Table_Col_Row
{
   /* private: */
   int requested_size;
   int size;
   int offset;
   Etk_Bool expand;
} Etk_Table_Col_Row;

/**
 * @struct Etk_Table
 */
struct _Etk_Table
{
   /* private: */
   /* Inherit from Etk_Container */
   Etk_Container container;

   Etk_Table_Cell **cells;
   Etk_Table_Col_Row *cols;
   Etk_Table_Col_Row *rows;
   
   Evas_List *children;

   int num_cols;
   int num_rows;
   Etk_Bool homogeneous;
};

Etk_Type *etk_table_type_get();

Etk_Widget *etk_table_new(int num_cols, int num_rows, Etk_Bool homogeneous);

void etk_table_cell_clear(Etk_Table *table, int col, int row);
void etk_table_resize(Etk_Table *table, int num_cols, int num_rows);

void etk_table_attach(Etk_Table *table, Etk_Widget *child, int left_attach, int right_attach, int top_attach, int bottom_attach, int x_padding, int y_padding, Etk_Table_Fill_Policy fill_policy);
void etk_table_attach_default(Etk_Table *table, Etk_Widget *child, int left_attach, int right_attach, int top_attach, int bottom_attach);

void etk_table_homogeneous_set(Etk_Table *table, Etk_Bool homogeneous);
Etk_Bool etk_table_homogeneous_get(Etk_Table *table);

/** @} */

#endif
