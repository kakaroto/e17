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


MODULE = Etk::Frame	PACKAGE = Etk::Frame	PREFIX = etk_frame_

const char *
etk_frame_label_get(frame)
	Etk_Frame *	frame
      ALIAS:
	LabelGet=1

void
etk_frame_label_set(frame, label)
	Etk_Frame *	frame
	char *	label
      ALIAS:
	LabelSet=1

Etk_Frame *
new(class, label)
	SV * class
	char *	label
	CODE:
	RETVAL = ETK_FRAME(etk_frame_new(label));
	OUTPUT:
	RETVAL


