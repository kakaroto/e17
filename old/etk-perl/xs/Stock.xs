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


MODULE = Etk::Stock	PACKAGE = Etk::Stock	PREFIX = etk_stock_
	
const char *
etk_stock_key_get(stock_id, size)
	Etk_Stock_Id	stock_id
	Etk_Stock_Size	size
      ALIAS:
	KeyGet=1

const char *
etk_stock_label_get(stock_id)
	Etk_Stock_Id	stock_id
      ALIAS:
	LabelGet=1


