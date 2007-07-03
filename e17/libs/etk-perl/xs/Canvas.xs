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

MODULE = Etk::Canvas		PACKAGE = Etk::Canvas	PREFIX = etk_canvas_

Etk_Widget *
etk_canvas_new()
      ALIAS:
	New=1

Etk_Bool
etk_canvas_object_add(canvas, object)
	Etk_Widget *	canvas
	Evas_Object *	object
      ALIAS:
	ObjectAdd=1
	CODE:
	Etk_Bool var;
	var = etk_canvas_object_add(ETK_CANVAS(canvas), object);
	RETVAL = var;
	OUTPUT:
	RETVAL

void
etk_canvas_object_remove(canvas, object)
	Etk_Widget *	canvas
	Evas_Object *	object
      ALIAS:
	ObjectRemove=1
	CODE:
	etk_canvas_object_remove(ETK_CANVAS(canvas), object);

void
etk_canvas_object_move(canvas, object, x, y)
	Etk_Widget *    canvas
	Evas_Object *   object
	int	x
	int	y
	ALIAS:
	ObjectMove=1
	CODE:
	etk_canvas_object_move(ETK_CANVAS(canvas), object, x, y);

void
etk_canvas_object_geometry_get(canvas, object)
	Etk_Widget *    canvas
	Evas_Object *   object
	ALIAS:
	GeometryGet=1
	PPCODE:
	int x, y, w, h;
	etk_canvas_object_geometry_get(ETK_CANVAS(canvas), object, &x, &y, &w, &h);
	EXTEND(SP, 4);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));


