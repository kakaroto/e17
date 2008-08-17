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


MODULE = Etk::Label	PACKAGE = Etk::Label	PREFIX = etk_label_

void
etk_label_alignment_get(label)
	Etk_Label *	label
      ALIAS:
	AlignmentGet=1
	PPCODE:
	float xalign;
	float yalign;
	etk_label_alignment_get(label, &xalign, &yalign);

	XPUSHs(sv_2mortal(newSVnv(xalign)));
	XPUSHs(sv_2mortal(newSVnv(yalign)));

void
etk_label_alignment_set(label, xalign, yalign)
	Etk_Label *	label
	float	xalign
	float	yalign
      ALIAS:
	AlignmentSet=1

const char *
etk_label_get(label)
	Etk_Label *	label
      ALIAS:
	Get=1

Etk_Label *
new(class, text)
	SV * class
	char *	text
	CODE:
	RETVAL = ETK_LABEL(etk_label_new(text));
	OUTPUT:
	RETVAL

void
etk_label_set(label, text)
	Etk_Label *	label
	const char *	text
      ALIAS:
	Set=1


