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


MODULE = Etk::Fixed	PACKAGE	= Etk::Fixed	PREFIX = etk_fixed_

Etk_Fixed *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_FIXED(etk_fixed_new());
	OUTPUT:
	RETVAL

void
etk_fixed_put(fixed, widget, x, y)
	Etk_Fixed * fixed
	Etk_Widget * widget
	int x
	int y
	ALIAS:
	Put=1

void
etk_fixed_move(fixed, widget, x, y)
	Etk_Fixed * fixed
	Etk_Widget * widget
	int x
	int y
	ALIAS:
	Move=1

void
etk_fixed_child_position_get(fixed, widget)
	Etk_Fixed * fixed
	Etk_Widget * widget
	ALIAS:
	ChildPositionGet=1
	PPCODE:
	
	int x, y;
	etk_fixed_child_position_get(fixed, widget, &x, &y);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));



