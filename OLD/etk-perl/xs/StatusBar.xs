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


MODULE = Etk::StatusBar	PACKAGE = Etk::StatusBar	PREFIX = etk_statusbar_

int
etk_statusbar_context_id_get(statusbar, context)
	Etk_Statusbar *	statusbar
	char *	context
      ALIAS:
	ContextIdGet=1

Etk_Bool
etk_statusbar_has_resize_grip_get(statusbar)
	Etk_Statusbar *	statusbar
      ALIAS:
	HasResizeGripGet=1

void
etk_statusbar_has_resize_grip_set(statusbar, has_resize_grip)
	Etk_Statusbar *	statusbar
	Etk_Bool	has_resize_grip
      ALIAS:
	HasResizeGripSet=1

Etk_Statusbar *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_STATUSBAR(etk_statusbar_new());
	OUTPUT:
	RETVAL

void
etk_statusbar_message_pop(statusbar, context_id)
	Etk_Statusbar *	statusbar
	int	context_id
      ALIAS:
	MessagePop=1

int
etk_statusbar_message_push(statusbar, message, context_id)
	Etk_Statusbar *	statusbar
	char *	message
	int	context_id
      ALIAS:
	MessagePush=1

void
etk_statusbar_message_remove(statusbar, message_id)
	Etk_Statusbar *	statusbar
	int	message_id
      ALIAS:
	MessageRemove=1

void
etk_statusbar_message_get(statusbar)
	Etk_Statusbar *	statusbar
      ALIAS:
	MessageGet=1
	PPCODE:
	const char ** message;
	int mid;
	int cid;
	etk_statusbar_message_get(statusbar, message, &mid, &cid);
	EXTEND(SP, 3);
	PUSHs(sv_2mortal(newSVpv(*message, strlen(*message))));
	PUSHs(sv_2mortal(newSViv(mid)));
	PUSHs(sv_2mortal(newSViv(cid)));
	
