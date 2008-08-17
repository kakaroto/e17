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


MODULE = Etk::Signal	PACKAGE = Etk::Signal	PREFIX = etk_signal_
	
void
etk_signal_shutdown()
      ALIAS:
	Shutdown=1


const char *
etk_signal_name_get(signal)
	Etk_Signal * signal
	ALIAS:
	NameGet=1


