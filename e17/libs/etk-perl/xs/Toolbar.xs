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

MODULE = Etk::ToolButton	PACKAGE = Etk::ToolButton	PREFIX = etk_tool_button_

Etk_Tool_Button *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_TOOL_BUTTON(etk_tool_button_new());
	OUTPUT:
	RETVAL

Etk_Tool_Button *
new_from_stock(stock_id)
	Etk_Stock_Id	stock_id
      ALIAS:
	NewFromStock=1
	CODE:
	RETVAL = ETK_TOOL_BUTTON(etk_tool_button_new_from_stock(stock_id));
	OUTPUT:
	RETVAL

Etk_Tool_Button *
new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_TOOL_BUTTON(etk_tool_button_new_with_label(label));
	OUTPUT:
	RETVAL

MODULE = Etk::ToolToggleButton	PACKAGE = Etk::ToolToggleButton	PREFIX = etk_tool_toggle_button_

Etk_Tool_Toggle_Button *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_TOOL_TOGGLE_BUTTON(etk_tool_toggle_button_new());
	OUTPUT:
	RETVAL

Etk_Tool_Toggle_Button *
new_from_stock(stock_id)
	Etk_Stock_Id	stock_id
      ALIAS:
	NewFromStock=1
	CODE:
	RETVAL = ETK_TOOL_TOGGLE_BUTTON(etk_tool_toggle_button_new_from_stock(stock_id));
	OUTPUT:
	RETVAL

Etk_Tool_Toggle_Button *
new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_TOOL_TOGGLE_BUTTON(etk_tool_toggle_button_new_with_label(label));
	OUTPUT:
	RETVAL

MODULE = Etk::Toolbar	PACKAGE = Etk::Toolbar	PREFIX = etk_toolbar_

Etk_Toolbar *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_TOOLBAR(etk_toolbar_new());
	OUTPUT:
	RETVAL

void
etk_toolbar_append(toolbar, widget, group=ETK_BOX_START)
	Etk_Toolbar * toolbar
	Etk_Widget * widget
	Etk_Box_Group	group
	ALIAS:
	Append=1

void
etk_toolbar_prepend(toolbar, widget, group=ETK_BOX_START)
	Etk_Toolbar * toolbar
	Etk_Widget * widget
	Etk_Box_Group	group
	ALIAS:
	Prepend=1

void
etk_toolbar_orientation_set(toolbar, orientation)
	Etk_Toolbar * toolbar
	Etk_Toolbar_Orientation orientation
	ALIAS:
	OrientationSet=1

Etk_Toolbar_Orientation
etk_toolbar_orientation_get(toolbar)
	Etk_Toolbar * toolbar
	ALIAS:
	OrientationGet=1

void
etk_toolbar_style_set(toolbar, style)
	Etk_Toolbar * toolbar
	Etk_Toolbar_Style style
	ALIAS:
	StyleSet=1

Etk_Toolbar_Style
etk_toolbar_style_get(toolbar)
	Etk_Toolbar * toolbar
	ALIAS:
	StyleGet=1

void
etk_toolbar_stock_size_set(toolbar, size)
	Etk_Toolbar * toolbar
	Etk_Stock_Size size
	ALIAS:
	StockSizeSet=1

Etk_Stock_Size
etk_toolbar_stock_size_get(toolbar)
	Etk_Toolbar * toolbar
	ALIAS:
	StockSizeGet=1


