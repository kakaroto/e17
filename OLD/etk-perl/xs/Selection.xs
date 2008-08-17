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


MODULE = Etk::Selection	PACKAGE = Etk::Selection	PREFIX = etk_selection_

void
etk_selection_text_request(selection, widget)
	Etk_Selection_Type selection
	Etk_Widget *	widget
      ALIAS:
	TextRequest=1

void
etk_selection_text_set(selection, text)
	Etk_Selection_Type selection
	char *	text
      ALIAS:
	TextSet=1

void
etk_selection_clear(selection)
	Etk_Selection_Type selection
	ALIAS:
	Clear=1
	
