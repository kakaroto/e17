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

MODULE = Etk::MessageDialog	PACKAGE = Etk::MessageDialog	PREFIX = etk_message_dialog_

Etk_Message_Dialog_Buttons
etk_message_dialog_buttons_get(dialog)
	Etk_Message_Dialog *	dialog
      ALIAS:
	ButtonsGet=1

void
etk_message_dialog_buttons_set(dialog, buttons)
	Etk_Message_Dialog *	dialog
	Etk_Message_Dialog_Buttons	buttons
      ALIAS:
	ButtonsSet=1

Etk_Message_Dialog_Type
etk_message_dialog_message_type_get(dialog)
	Etk_Message_Dialog *	dialog
      ALIAS:
	MessageTypeGet=1

void
etk_message_dialog_message_type_set(dialog, type)
	Etk_Message_Dialog *	dialog
	Etk_Message_Dialog_Type	type
      ALIAS:
	MessageTypeSet=1

Etk_Message_Dialog *
new(class, message_type, buttons, text)
	SV * class
	Etk_Message_Dialog_Type	message_type
	Etk_Message_Dialog_Buttons	buttons
	char *	text
	CODE:
	RETVAL = ETK_MESSAGE_DIALOG(etk_message_dialog_new(message_type, buttons, text));
	OUTPUT:
	RETVAL

const char *
etk_message_dialog_text_get(dialog)
	Etk_Message_Dialog *	dialog
      ALIAS:
	TextGet=1

void
etk_message_dialog_text_set(dialog, text)
	Etk_Message_Dialog *	dialog
	char *	text
      ALIAS:
	TextSet=1


