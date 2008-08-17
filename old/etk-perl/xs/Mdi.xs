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


MODULE = Etk::Mdi::Window	PACKAGE = Etk::Mdi::Window	PREFIX = etk_mdi_window_
	
Etk_Mdi_Window *
new(class)
	SV * class;
	CODE:
	RETVAL = ETK_MDI_WINDOW(etk_mdi_window_new());
	OUTPUT:
	RETVAL

void
etk_mdi_window_delete_request(window)
	Etk_Mdi_Window * window
	ALIAS:
	DeleteRequest=1

void
etk_mdi_window_title_set(window, title)
	Etk_Mdi_Window * window
	const char *title
	ALIAS:
	TitleSet=1

const char *
etk_mdi_window_title_get(window)
	Etk_Mdi_Window * window
	ALIAS:
	TitleGet=1
	
void
etk_mdi_window_move(window, x, y)
	Etk_Mdi_Window * window
	int x
	int y
	ALIAS:
	Move=1

void
etk_mdi_window_maximized_set(window, maximized)
	Etk_Mdi_Window * window
	Etk_Bool maximized
	ALIAS:
	MaximizedSet=1

Etk_Bool
etk_mdi_window_maximized_get(window)
	Etk_Mdi_Window * window
	ALIAS:
	MaximizedGet=1

Etk_Bool
etk_mdi_window_hide_on_delete(window, data)
	Etk_Object * window
	SV * data
	ALIAS:
	HideOnDelete=1
	CODE:
	RETVAL = etk_mdi_window_hide_on_delete(window, newSVsv(data));
	OUTPUT:
	RETVAL

MODULE = Etk::Mdi::Area	PACKAGE = Etk::Mdi::Area	PREFIX = etk_mdi_area_

Etk_Mdi_Area * 
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MDI_AREA(etk_mdi_area_new());
	OUTPUT:
	RETVAL

void
etk_mdi_area_put(area, widget, x, y)
	Etk_Mdi_Area * area
	Etk_Widget * widget
	int x
	int y
	ALIAS:
	Put=1

void
etk_mdi_area_move(area, widget, x, y)
	Etk_Mdi_Area * area
	Etk_Widget * widget
	int x
	int y
	ALIAS:
	Move=1

void
etk_mdi_area_child_position_get(area, widget)
	Etk_Mdi_Area * area
	Etk_Widget * widget
	ALIAS:
	ChildPositionGet=1
	PPCODE:
	int x, y;
	etk_mdi_area_child_position_get(area, widget, &x, &y);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));


MODULE = Etk::Mdi       PACKAGE = Etk::Mdi
