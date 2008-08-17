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


MODULE = Etk::Range	PACKAGE = Etk::Range	PREFIX = etk_range_

void
etk_range_increments_set(range, step, page)
	Etk_Range *	range
	double	step
	double	page
      ALIAS:
	IncrementsSet=1

void
etk_range_increments_get(range)
	Etk_Range * range
      ALIAS:
	IncrementsGet=1
	PPCODE:
	double step, page;
	etk_range_increments_get(range, &step, &page);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSVnv(step)));
	PUSHs(sv_2mortal(newSVnv(page)));

double
etk_range_page_size_get(range)
	Etk_Range *	range
      ALIAS:
	PageSizeGet=1

void
etk_range_page_size_set(range, page_size)
	Etk_Range *	range
	double	page_size
      ALIAS:
	PageSizeSet=1

void
etk_range_range_set(range, lower, upper)
	Etk_Range *	range
	double	lower
	double	upper
      ALIAS:
	RangeSet=1

void
etk_range_range_get(range)
	Etk_Range * range
      ALIAS:
	RangeGet=1
	PPCODE:
	double lower, upper;
	etk_range_range_get(range, &lower, &upper);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSVnv(lower)));
	PUSHs(sv_2mortal(newSVnv(upper)));

double
etk_range_value_get(range)
	Etk_Range *	range
      ALIAS:
	ValueGet=1

Etk_Bool
etk_range_value_set(range, value)
	Etk_Range *	range
	double	value
      ALIAS:
	ValueSet=1


