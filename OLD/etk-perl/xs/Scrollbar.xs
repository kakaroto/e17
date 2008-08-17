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


MODULE = Etk::VScrollbar	PACKAGE = Etk::VScrollbar	PREFIX = etk_vscrollbar_

Etk_VScrollbar *
new(class, lower, upper, value, step_increment, page_increment, page_size)
	SV * class
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment
	double	page_size
	CODE:
	RETVAL = ETK_VSCROLLBAR(etk_vscrollbar_new(lower, upper, value, 
				step_increment, page_increment, page_size));
	OUTPUT:
	RETVAL

MODULE = Etk::HScrollbar	PACKAGE = Etk::HScrollbar	PREFIX = etk_hscrollbar_

Etk_HScrollbar *
new(class, lower, upper, value, step_increment, page_increment, page_size)
	SV * class
	double	lower
	double	upper
	double	value
	double	step_increment
	double	page_increment
	double	page_size
	CODE:
	RETVAL = ETK_HSCROLLBAR(etk_hscrollbar_new(lower, upper, value, 
				step_increment, page_increment, page_size));
	OUTPUT:
	RETVAL

MODULE = Etk::Scrollbar		PACKAGE = Etk::Scrollbar
