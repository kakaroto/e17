#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "../ppport.h"

#ifdef _
#undef _
#endif

#include <Etk.h>
#include <Ecore.h>
#include <Ecore_Data.h>

#include "EtkTypes.h"
#include "EtkSignals.h"


MODULE = Etk::Table	PACKAGE = Etk::Table	PREFIX = etk_table_

void
etk_table_attach(table, child, left_attach, right_attach, top_attach, bottom_attach, x_padding, y_padding, fill_policy)
	Etk_Table *	table
	Etk_Widget *	child
	int	left_attach
	int	right_attach
	int	top_attach
	int	bottom_attach
	int	x_padding
	int	y_padding
	Etk_Table_Fill_Policy	fill_policy
      ALIAS:
	Attach=1

void
etk_table_attach_default(table, child, left_attach, right_attach, top_attach, bottom_attach)
	Etk_Table *	table
	Etk_Widget *	child
	int	left_attach
	int	right_attach
	int	top_attach
	int	bottom_attach
      ALIAS:
	AttachDefault=1

void
etk_table_cell_clear(table, col, row)
	Etk_Table *	table
	int	col
	int	row
      ALIAS:
	CellClear=1

Etk_Bool
etk_table_homogeneous_get(table)
	Etk_Table *	table
      ALIAS:
	HomogeneousGet=1

void
etk_table_homogeneous_set(table, homogeneous)
	Etk_Table *	table
	Etk_Bool	homogeneous
      ALIAS:
	HomogeneousSet=1

Etk_Table *
new(class, num_cols, num_rows, homogeneous)
	SV * class
	int	num_cols
	int	num_rows
	Etk_Bool	homogeneous
	CODE:
	RETVAL = ETK_TABLE(etk_table_new(num_cols, num_rows, homogeneous));
	OUTPUT:
	RETVAL

void
etk_table_resize(table, num_cols, num_rows)
	Etk_Table *	table
	int	num_cols
	int	num_rows
      ALIAS:
	Resize=1



