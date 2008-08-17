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

MODULE = Etk::Viewport	PACKAGE = Etk::Viewport	PREFIX = etk_viewport_

Etk_Widget *
new(class)
	SV * class
	CODE:
	RETVAL = etk_viewport_new();
	OUTPUT:
	RETVAL


