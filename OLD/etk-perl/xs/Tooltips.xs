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


MODULE = Etk::Tooltips	PACKAGE = Etk::Tooltips	PREFIX = etk_tooltips_

void
etk_tooltips_disable()
      ALIAS:
	Disable=1

void
etk_tooltips_enable()
      ALIAS:
	Enable=1

void
etk_tooltips_init()
      ALIAS:
	Init=1

void
etk_tooltips_pop_down()
      ALIAS:
	PopDown=1

void
etk_tooltips_pop_up(widget)
	Etk_Widget *	widget
      ALIAS:
	PopUp=1

void
etk_tooltips_shutdown()
      ALIAS:
	Shutdown=1

const char *
etk_tooltips_tip_get(widget)
	Etk_Widget *	widget
      ALIAS:
	TipGet=1

void
etk_tooltips_tip_set(widget, text)
	Etk_Widget *	widget
	const char *	text
      ALIAS:
	TipSet=1

Etk_Bool
etk_tooltips_tip_visible()
      ALIAS:
	TipVisible=1


