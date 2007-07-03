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

MODULE = Etk::Shadow	PACKAGE = Etk::Shadow	PREFIX = etk_shadow_

Etk_Shadow *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_SHADOW(etk_shadow_new());
	OUTPUT:
	RETVAL

void
etk_shadow_shadow_set(shadow, type, edges, radius, offset_x, offset_y, opacity)
	Etk_Shadow *shadow
	Etk_Shadow_Type type
	Etk_Shadow_Edges edges
	int radius
	int offset_x
	int offset_y 
	int opacity
	ALIAS:
	ShadowSet=1

void
etk_shadow_shadow_get(shadow)
	Etk_Shadow *shadow
	ALIAS:
	ShadowGet=1
	PPCODE:
	Etk_Shadow_Type type;
	Etk_Shadow_Edges edges;
	int radius;
	int offset_x;
	int offset_y;
	int opacity;

	etk_shadow_shadow_get(shadow, &type, &edges, &radius, &offset_x, &offset_y, &opacity);
	EXTEND(SP, 6);
	PUSHs(sv_2mortal(newSViv(type)));
	PUSHs(sv_2mortal(newSViv(edges)));
	PUSHs(sv_2mortal(newSViv(radius)));
	PUSHs(sv_2mortal(newSViv(offset_x)));
	PUSHs(sv_2mortal(newSViv(offset_y)));
	PUSHs(sv_2mortal(newSViv(opacity)));

void
etk_shadow_shadow_color_set(shadow, r, g, b)
	Etk_Shadow * shadow
	int r
	int g
	int b
	ALIAS:
	ShadowColorSet=1

void
etk_shadow_shadow_color_get(shadow)
	Etk_Shadow * shadow
	ALIAS:
	ShadowColorGet=1
	PPCODE:
	int r, g, b;

	etk_shadow_shadow_color_get(shadow, &r, &g, &b);

	EXTEND(SP, 3);
	PUSHs(sv_2mortal(newSViv(r)));
	PUSHs(sv_2mortal(newSViv(g)));
	PUSHs(sv_2mortal(newSViv(b)));

void 
etk_shadow_border_set(shadow, border_width)
	Etk_Shadow *shadow
	int border_width
	ALIAS:
	BorderSet=1

int
etk_shadow_border_get(shadow)
	Etk_Shadow * shadow
	ALIAS:
	BorderGet=1

void
etk_shadow_border_color_set(shadow, r, g, b, a)
	Etk_Shadow *shadow
	int r
	int g
	int b
	int a
	ALIAS:
	BorderColorSet=1

void
etk_shadow_border_color_get(shadow)
	Etk_Shadow *shadow
	ALIAS:
	BorderColorGet=1
	PPCODE:
	int r, g, b, a;
	etk_shadow_border_color_get(shadow, &r, &g, &b, &a);
	EXTEND(SP, 4);
	PUSHs(sv_2mortal(newSViv(r)));
	PUSHs(sv_2mortal(newSViv(g)));
	PUSHs(sv_2mortal(newSViv(b)));
	PUSHs(sv_2mortal(newSViv(a)));


