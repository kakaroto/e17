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

MODULE = Etk::Spinner	PACKAGE = Etk::Spinner	PREFIX = etk_spinner_

Etk_Spinner *
new(class, lower, upper, value, step_increment, page_increment)
	SV * class
	double lower
	double upper
	double value
	double step_increment
	double page_increment
	CODE:
	RETVAL = ETK_SPINNER(etk_spinner_new(lower, upper, value, step_increment, page_increment));

void
etk_spinner_digits_set(spinner, digits)
	Etk_Spinner * spinner
	int digits
	ALIAS:
	DigitsSet=1

int
etk_spinner_digits_get(spinner)
	Etk_Spinner * spinner
	ALIAS:
	DigitsGet=1

void
etk_spinner_snap_to_ticks_set(spinner, snap)
	Etk_Spinner * spinner
	Etk_Bool	snap
	ALIAS:
	SnapToTicksSet=1

Etk_Bool
etk_spinner_snap_to_ticks_get(spinner)
	Etk_Spinner * spinner
	ALIAS:
	SnapToTicksGet=1
	
void
etk_spinner_wrap_set(spinner, wrap)
	Etk_Spinner * spinner
	Etk_Bool	wrap
	ALIAS:
	WrapSet=1

Etk_Bool
etk_spinner_wrap_get(spinner)
	Etk_Spinner * spinner
	ALIAS:
	WrapGet=1


