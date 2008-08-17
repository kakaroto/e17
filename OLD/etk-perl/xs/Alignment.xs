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

MODULE = Etk::Alignment		PACKAGE = Etk::Alignment	PREFIX = etk_alignment_

void
etk_alignment_get(alignment)
	Etk_Alignment *	alignment
      ALIAS:
	Get=1
      PPCODE:
	float xalign;
	float yalign;
	float xscale;
	float yscale;

	etk_alignment_get(alignment, &xalign, &yalign, &xscale, &yscale);
        EXTEND(SP, 4);
        PUSHs(sv_2mortal(newSVnv(xalign)));
        PUSHs(sv_2mortal(newSVnv(yalign)));
        PUSHs(sv_2mortal(newSVnv(xscale)));
        PUSHs(sv_2mortal(newSVnv(yscale)));	

Etk_Alignment *
new(class, xalign=0.5, yalign=0.5, xscale=1, yscale=1)
	SV	*class
	float	xalign
	float	yalign
	float	xscale
	float	yscale
	CODE:
	RETVAL = ETK_ALIGNMENT(etk_alignment_new(xalign, yalign, xscale, yscale));
	OUTPUT:
	RETVAL

void
etk_alignment_set(alignment, xalign, yalign, xscale, yscale)
	Etk_Alignment *	alignment
	float	xalign
	float	yalign
	float	xscale
	float	yscale
      ALIAS:
	Set=1

