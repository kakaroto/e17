/** @file etk_table.h */
#ifndef _ETK_TABLE_H_
#define _ETK_TABLE_H_

#include "etk_container.h"
#include <Evas.h>
#include "etk_types.h"

/**
 * @defgroup Etk_Table Etk_Table
 * @{
 */

/** @brief Gets the type of a table */
#define ETK_TABLE_TYPE       (etk_table_type_get())
/** @brief Casts the object to an Etk_Table */
#define ETK_TABLE(obj)       (ETK_OBJECT_CAST((obj), ETK_TABLE_TYPE, Etk_Table))
/** @brief Checks if the object is an Etk_Table */
#define ETK_IS_TABLE(obj)    (ETK_OBJECT_CHECK_TYPE((obj), ETK_TABLE_TYPE))

/* A cell of a table */
typedef struct _Etk_Table_Cell
{
   /* private: */
   int left_attach;
   int right_attach;
   int top_attach;
   int bottom_attach;
   int x_padding;
   int y_padding;
   Etk_Fill_Policy_Flags fill_policy;
   Etk_Widget *child;
} Etk_Table_Cell;

/* A column or a row of a table */
typedef struct _Etk_Table_Col_Row
{
   /* private: */
   int requested_size;
   int size;
   int offset;
   Etk_Bool expand;
} Etk_Table_Col_Row;

/**
 * @struct Etk_Table
 * @brief An Etk_Table is a container that can contain several children packed on a grid
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

void etk_table_attach(Etk_Table *table, Etk_Widget *child, int left_attach, int right_attach,
   int top_attach, int bottom_attach, int x_padding, int y_padding, Etk_Fill_Policy_Flags fill_policy);
void etk_table_attach_defaults(Etk_Table *table, Etk_Widget *child, int left_attach, int right_attach, int top_attach, int bottom_attach);

void etk_table_homogeneous_set(Etk_Table *table, Etk_Bool homogeneous);
Etk_Bool etk_table_homogeneous_get(Etk_Table *table);

/** @} */

#endif
