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

MODULE = Etk::Colorpicker		PACKAGE = Etk::Colorpicker	PREFIX = etk_colorpicker_
	
Etk_Color
etk_colorpicker_current_color_get(cp)
	Etk_Colorpicker *	cp
      ALIAS:
	CurrentColorGet=1
	
void
etk_colorpicker_use_alpha_set(cp, use_alpha)
	Etk_Colorpicker *	cp
	Etk_Bool	use_alpha
	ALIAS:
	UseAlphaSet=1

Etk_Bool
etk_colorpicker_use_alpha_get(cp)
	Etk_Colorpicker *       cp
	ALIAS:
	UseAlphaGet=1

void
etk_colorpicker_current_color_set(cp, color)
	Etk_Colorpicker *	cp
	Etk_Color	color
      ALIAS:
	CurrentColorSet=1
	

Etk_Colorpicker_Mode
etk_colorpicker_mode_get(cp)
	Etk_Colorpicker *	cp
      ALIAS:
	ModeGet=1

void
etk_colorpicker_mode_set(cp, mode)
	Etk_Colorpicker *	cp
	Etk_Colorpicker_Mode	mode
      ALIAS:
	ModeSet=1

Etk_Colorpicker *
new(class)
	SV	*class
	CODE:
	RETVAL = ETK_COLORPICKER(etk_colorpicker_new());
	OUTPUT:
	RETVAL

