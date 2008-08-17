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


MODULE = Etk::Drag	PACKAGE = Etk::Drag	PREFIX = etk_drag_
	
void
etk_drag_begin(drag)
	Etk_Drag *	drag
      ALIAS:
	Begin=1

void
etk_drag_data_set(drag, data, size)
	Etk_Drag *	drag
	SV *	data
      ALIAS:
	DataSet=1
	CODE:
	etk_drag_data_set(drag, newSVsv(data), sizeof(SV));

Etk_Drag *
new(class, widget)
	SV * class
	Etk_Widget *	widget
	CODE:
	RETVAL = ETK_DRAG(etk_drag_new(widget));
	OUTPUT:
	RETVAL

Etk_Widget *
etk_drag_parent_widget_get(drag)
	Etk_Drag *	drag
      ALIAS:
	ParentWidgetGet=1

void
etk_drag_parent_widget_set(drag, widget)
	Etk_Drag *	drag
	Etk_Widget *	widget
      ALIAS:
	ParentWidgetSet=1

void
etk_drag_types_set(drag, types)
	Etk_Drag *	drag
	AV * types
      ALIAS:
	TypesSet=1
	CODE:
	const char **	t;
	unsigned int	num_types;
	int i;
	
	num_types = (unsigned int) av_len(types) + 1;
	t = calloc(num_types, sizeof(char *));
	for (i=0; i<num_types; i++) {
	    SV ** val;
	    val = av_fetch(types, i, 0);
	    if (val) 
		t[i] = (char *)SvIV(*val);
	    else 
		t[i] = 0;
	}   
	
	etk_drag_types_set(drag, t, num_types);


