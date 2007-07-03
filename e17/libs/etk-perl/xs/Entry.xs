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


MODULE = Etk::Entry	PACKAGE = Etk::Entry	PREFIX = etk_entry_
	
Etk_Entry *
new(class)
	SV *	class
	CODE:
	RETVAL = ETK_ENTRY(etk_entry_new());
	OUTPUT:
	RETVAL

Etk_Bool
etk_entry_password_mode_get(entry)
	Etk_Entry *	entry
      ALIAS:
	PasswordModeGet=1

void
etk_entry_password_mode_set(entry, on)
	Etk_Entry *	entry
	Etk_Bool	on
      ALIAS:
	PasswordModeSet=1

const char *
etk_entry_text_get(entry)
	Etk_Entry *	entry
      ALIAS:
	TextGet=1

void
etk_entry_text_set(entry, text)
	Etk_Entry *	entry
	char *	text
      ALIAS:
	TextSet=1

void
etk_entry_clear(entry)
	Etk_Entry *	entry
	ALIAS:
	Clear=1

void
etk_entry_image_set(entry, position, image)
	Etk_Entry *	entry
	Etk_Entry_Image_Position position
	Etk_Image *	image
	ALIAS:
	ImageSet=1

Etk_Image *
etk_entry_image_get(entry, position)
	Etk_Entry *	entry
	Etk_Entry_Image_Position position
	ALIAS:
	ImageGet=1

void
etk_entry_image_highlight_set(entry, position, highlight)
	Etk_Entry *	entry
	Etk_Entry_Image_Position position
	Etk_Bool	highlight
	ALIAS:
	ImageHighlightSet=1

void
etk_entry_clear_button_add(entry)
	Etk_Entry *     entry
	ALIAS:
	ClearButtonAdd=1
	

