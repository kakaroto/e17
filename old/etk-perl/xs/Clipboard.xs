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

MODULE = Etk::Clipboard		PACKAGE = Etk::Clipboard	PREFIX = etk_clipboard_

void
etk_clipboard_text_request(widget)
	Etk_Widget *	widget
      ALIAS:
	TextRequest=1

void
etk_clipboard_text_set(widget, data, length)
	Etk_Widget *	widget
	char *	data
	int	length
      ALIAS:
	TextSet=1


