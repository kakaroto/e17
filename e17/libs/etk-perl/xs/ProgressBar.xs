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


MODULE = Etk::ProgressBar	PACKAGE = Etk::ProgressBar	PREFIX = etk_progress_bar_
	
double
etk_progress_bar_fraction_get(progress_bar)
	Etk_Progress_Bar *	progress_bar
      ALIAS:
	FractionGet=1

void
etk_progress_bar_fraction_set(progress_bar, fraction)
	Etk_Progress_Bar *	progress_bar
	double	fraction
      ALIAS:
	FractionSet=1

Etk_Progress_Bar *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_PROGRESS_BAR(etk_progress_bar_new());
	OUTPUT:
	RETVAL

Etk_Progress_Bar *
etk_progress_bar_new_with_text(label)
	char *	label
      ALIAS:
	NewWithText=1
	CODE:
	RETVAL = ETK_PROGRESS_BAR(etk_progress_bar_new_with_text(label));
	OUTPUT:
	RETVAL

void
etk_progress_bar_pulse(progress_bar)
	Etk_Progress_Bar *	progress_bar
      ALIAS:
	Pulse=1

double
etk_progress_bar_pulse_step_get(progress_bar)
	Etk_Progress_Bar *	progress_bar
      ALIAS:
	PulseStepGet=1

void
etk_progress_bar_pulse_step_set(progress_bar, pulse_step)
	Etk_Progress_Bar *	progress_bar
	double	pulse_step
      ALIAS:
	PulseStepSet=1

const char *
etk_progress_bar_text_get(progress_bar)
	Etk_Progress_Bar *	progress_bar
      ALIAS:
	TextGet=1

void
etk_progress_bar_text_set(progress_bar, label)
	Etk_Progress_Bar *	progress_bar
	char *	label
      ALIAS:
	TextSet=1

void
etk_progress_bar_direction_set(progress_bar, direction)
	Etk_Progress_Bar * progress_bar
	Etk_Progress_Bar_Direction direction
      ALIAS:
	DirectionSet=1

Etk_Progress_Bar_Direction
etk_progress_bar_direction_get(progress_bar)
	Etk_Progress_Bar * progress_bar
      ALIAS:
	DirectionGet=1


