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


MODULE = Etk::VSeparator	PACKAGE = Etk::VSeparator	PREFIX = etk_vseparator_

Etk_VSeparator *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_VSEPARATOR(etk_vseparator_new());
	OUTPUT:
	RETVAL

MODULE = Etk::HSeparator	PACKAGE = Etk::HSeparator	PREFIX = etk_hseparator_

Etk_HSeparator *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_HSEPARATOR(etk_hseparator_new());
	OUTPUT:
	RETVAL

MODULE = Etk::Separator		PACKAGE = Etk::Separator

