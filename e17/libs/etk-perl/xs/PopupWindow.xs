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


MODULE = Etk::PopupWindow	PACKAGE = Etk::PopupWindow	PREFIX = etk_popup_window_
	
Etk_Popup_Window *
etk_popup_window_focused_window_get()
      ALIAS:
	FocusedWindowGet=1

void
etk_popup_window_focused_window_set(popup_window)
	Etk_Popup_Window *	popup_window
      ALIAS:
	FocusedWindowSet=1

Etk_Bool
etk_popup_window_is_popped_up(popup_window)
	Etk_Popup_Window *	popup_window
      ALIAS:
	IsPoppedUp=1

void
etk_popup_window_popdown(popup_window)
	Etk_Popup_Window *	popup_window
      ALIAS:
	Popdown=1

void
etk_popup_window_popdown_all()
      ALIAS:
	PopdownAll=1

void
etk_popup_window_popup(popup_window)
	Etk_Popup_Window *	popup_window
      ALIAS:
	Popup=1

void
etk_popup_window_popup_at_xy(popup_window, x, y)
	Etk_Popup_Window *	popup_window
	int	x
	int	y
      ALIAS:
	PopupAtXy=1

void
etk_popup_window_parent_set(popup_window, parent)
	Etk_Popup_Window *      popup_window
	Etk_Popup_Window *      parent
	ALIAS:
	ParentSet=1

Etk_Popup_Window *
etk_popup_window_parent_get(popup_window)
	Etk_Popup_Window *      popup_window
	ALIAS:
	ParentGet=1

void
etk_popup_window_popup_in_direction(popup_window, direction)
	Etk_Popup_Window *      popup_window
	Etk_Popup_Direction	direction
	ALIAS:
	PopupInDirection=1

void
etk_popup_window_popup_at_xy_in_direction(popup_window, x, y, direction)
	Etk_Popup_Window *      popup_window
	int	x
	int	y
	Etk_Popup_Direction     direction
	ALIAS:
	PopupAtXYInDirection=1


