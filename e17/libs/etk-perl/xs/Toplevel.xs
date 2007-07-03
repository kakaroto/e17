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


MODULE = Etk::Toplevel	PACKAGE = Etk::Toplevel	PREFIX = etk_toplevel_

Evas *
etk_toplevel_evas_get(toplevel_widget)
	Etk_Toplevel *	toplevel_widget
      ALIAS:
	EvasGet=1

Etk_Widget *
etk_toplevel_focused_widget_get(toplevel_widget)
	Etk_Toplevel *	toplevel_widget
      ALIAS:
	FocusedWidgetGet=1

Etk_Widget *
etk_toplevel_focused_widget_next_get(toplevel_widget)
	Etk_Toplevel *	toplevel_widget
      ALIAS:
	FocusedWidgetNextGet=1

Etk_Widget *
etk_toplevel_focused_widget_prev_get(toplevel_widget)
	Etk_Toplevel *	toplevel_widget
      ALIAS:
	FocusedWidgetPrevGet=1

void
etk_toplevel_focused_widget_set(toplevel_widget, widget)
	Etk_Toplevel *	toplevel_widget
	Etk_Widget *	widget
      ALIAS:
	FocusedWidgetSet=1

void
etk_toplevel_pointer_pop(toplevel_widget, pointer_type)
	Etk_Toplevel *	toplevel_widget
	Etk_Pointer_Type	pointer_type
      ALIAS:
	PointerPop=1

void
etk_toplevel_pointer_push(toplevel_widget, pointer_type)
	Etk_Toplevel *	toplevel_widget
	Etk_Pointer_Type	pointer_type
      ALIAS:
	PointerPush=1

void
etk_toplevel_evas_position_get(toplevel)
	Etk_Toplevel *  toplevel
	ALIAS:
	EvasPositionGet=1
	PPCODE:
	int x, y;
	etk_toplevel_evas_position_get(toplevel, &x, &y);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));

void
etk_toplevel_screen_position_get(toplevel)
	Etk_Toplevel *  toplevel
	ALIAS:
	ScreenPositionGet=1
	PPCODE:
	int x, y;
	etk_toplevel_screen_position_get(toplevel, &x, &y);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));

void
etk_toplevel_size_get(toplevel)
	Etk_Toplevel *  toplevel
	ALIAS:
	SizeGet=1
	PPCODE:
	int w, h;
	etk_toplevel_size_get(toplevel, &w, &h);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));

Evas_List *
etk_toplevel_widgets_get()
	ALIAS:
	WidgetsGet=1


