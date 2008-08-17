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

MODULE = Etk::Bin		PACKAGE = Etk::Bin	PREFIX = etk_bin_

Etk_Widget *
etk_bin_child_get(bin)
	Etk_Bin *	bin
      ALIAS:
	ChildGet=1

void
etk_bin_child_set(bin, child)
	Etk_Bin *	bin
	Etk_Widget *	child
      ALIAS:
	ChildSet=1

