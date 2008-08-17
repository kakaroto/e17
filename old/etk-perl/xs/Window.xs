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

MODULE = Etk::Window	PACKAGE = Etk::Window	PREFIX = etk_window_

void
etk_window_has_alpha_set(window, has_alpha)
	Etk_Window *	window
	Etk_Bool	has_alpha
	ALIAS:
	HasAlphaSet=1

Etk_Bool
etk_window_has_alpha_get(window)
	Etk_Window *    window
	ALIAS:
	HasAlphaGet=1

void
etk_window_raise(window)
	Etk_Window * window
      ALIAS:
	Raise=1

void
etk_window_lower(window)
	Etk_Window * window
      ALIAS:
	Lower=1

void
etk_window_modal_for_window(window_to_modal, window)
	Etk_Window *window_to_modal
	Etk_Window *window
      ALIAS:
	ModalForWindow=1

void
etk_window_center_on_window(window_to_center, window)
	Etk_Window *	window_to_center
	Etk_Window *	window
      ALIAS:
	CenterOnWindow=1

Etk_Bool
etk_window_decorated_get(window)
	Etk_Window *	window
      ALIAS:
	DecoratedGet=1

void
etk_window_decorated_set(window, decorated)
	Etk_Window *	window
	Etk_Bool	decorated
      ALIAS:
	DecoratedSet=1

void
etk_window_iconified_set(window, iconified)
	Etk_Window *	window
	Etk_Bool	iconified
      ALIAS:
	IconifiedSet=1

Etk_Bool
etk_window_iconified_get(window)
	Etk_Window *	window
      ALIAS:
	IconifiedGet=1

Etk_Bool
etk_window_focused_get(window)
	Etk_Window *	window
      ALIAS:
	FocusedGet=1

void
etk_window_focused_set(window, focused)
	Etk_Window *	window
	Etk_Bool	focused
      ALIAS:
	FocusedSet=1

Etk_Bool
etk_window_fullscreen_get(window)
	Etk_Window *	window
      ALIAS:
	FullscreenGet=1

void
etk_window_fullscreen_set(window, fullscreen)
	Etk_Window *	window
	Etk_Bool	fullscreen
      ALIAS:
	FullscreenSet=1

void
etk_window_geometry_get(window)
	Etk_Window *	window
      ALIAS:
	GeometryGet=1
	PPCODE:
	int 	x;
	int 	y;
	int 	w;
	int 	h;
	etk_window_geometry_get(window, &x, &y, &w, &h);
	EXTEND(SP, 4);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));

Etk_Bool
etk_window_hide_on_delete(window, data)
	Etk_Object *	window
	void *	data
      ALIAS:
	HideOnDelete=1

Etk_Bool
etk_window_maximized_get(window)
	Etk_Window *	window
      ALIAS:
	MaximizedGet=1

void
etk_window_maximized_set(window, maximized)
	Etk_Window *	window
	Etk_Bool	maximized
      ALIAS:
	MaximizedSet=1

void
etk_window_move(window, x, y)
	Etk_Window *	window
	int	x
	int	y
      ALIAS:
	Move=1

void
etk_window_move_to_mouse(window)
	Etk_Window *	window
      ALIAS:
	MoveToMouse=1

Etk_Window *
new(class)
	SV	* class
	CODE:
	RETVAL = ETK_WINDOW(etk_window_new());
	OUTPUT:
	RETVAL

void
etk_window_resize(window, w, h)
	Etk_Window *	window
	int	w
	int	h
      ALIAS:
	Resize=1

Etk_Bool
etk_window_shaped_get(window)
	Etk_Window *	window
      ALIAS:
	ShapedGet=1

void
etk_window_shaped_set(window, shaped)
	Etk_Window *	window
	Etk_Bool	shaped
      ALIAS:
	ShapedSet=1

Etk_Bool
etk_window_skip_pager_hint_get(window)
	Etk_Window *	window
      ALIAS:
	SkipPagerHintGet=1

void
etk_window_skip_pager_hint_set(window, skip_pager_hint)
	Etk_Window *	window
	Etk_Bool	skip_pager_hint
      ALIAS:
	SkipPagerHintSet=1

Etk_Bool
etk_window_skip_taskbar_hint_get(window)
	Etk_Window *	window
      ALIAS:
	SkipTaskbarHintGet=1

void
etk_window_skip_taskbar_hint_set(window, skip_taskbar_hint)
	Etk_Window *	window
	Etk_Bool	skip_taskbar_hint
      ALIAS:
	SkipTaskbarHintSet=1

Etk_Bool
etk_window_sticky_get(window)
	Etk_Window *	window
      ALIAS:
	StickyGet=1

void
etk_window_sticky_set(window, sticky)
	Etk_Window *	window
	Etk_Bool	sticky
      ALIAS:
	StickySet=1

const char *
etk_window_title_get(window)
	Etk_Window *	window
      ALIAS:
	TitleGet=1

void
etk_window_title_set(window, title)
	Etk_Window *	window
	char *	title
      ALIAS:
	TitleSet=1

void
etk_window_wmclass_set(window, window_name, window_class)
	Etk_Window *	window
	char *	window_name
	char *	window_class
      ALIAS:
	WmclassSet=1

void
etk_window_delete_request(window)
	Etk_Window *	window
	ALIAS:
	DeleteRequest=1

void
etk_window_stacking_set(window, stacking)
	Etk_Window *    window
	Etk_Window_Stacking stacking
	ALIAS:
	StackingSet=1

Etk_Window_Stacking
etk_window_stacking_get(window)
	Etk_Window *    window
	ALIAS:
	StackingGet=1


