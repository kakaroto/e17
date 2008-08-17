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


MODULE = Etk::ScrolledView	PACKAGE = Etk::ScrolledView	PREFIX = etk_scrolled_view_

void
etk_scrolled_view_add_with_viewport(scrolled_view, child)
	Etk_Scrolled_View *	scrolled_view
	Etk_Widget *	child
      ALIAS:
	AddWithViewport=1

Etk_Range *
etk_scrolled_view_hscrollbar_get(scrolled_view)
	Etk_Scrolled_View *	scrolled_view
      ALIAS:
	HScrollbarGet=1

Etk_Scrolled_View *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_SCROLLED_VIEW(etk_scrolled_view_new());
	OUTPUT:
	RETVAL

void
etk_scrolled_view_policy_get(scrolled_view)
	Etk_Scrolled_View *	scrolled_view
      ALIAS:
	PolicyGet=1
	PPCODE:
	Etk_Scrolled_View_Policy hpolicy;
	Etk_Scrolled_View_Policy vpolicy;

	etk_scrolled_view_policy_get(scrolled_view, &hpolicy, &vpolicy);
	XPUSHs(sv_2mortal(newSViv(hpolicy)));
	XPUSHs(sv_2mortal(newSViv(vpolicy)));

void
etk_scrolled_view_policy_set(scrolled_view, hpolicy, vpolicy)
	Etk_Scrolled_View *	scrolled_view
	Etk_Scrolled_View_Policy	hpolicy
	Etk_Scrolled_View_Policy	vpolicy
      ALIAS:
	PolicySet=1

Etk_Range *
etk_scrolled_view_vscrollbar_get(scrolled_view)
	Etk_Scrolled_View *	scrolled_view
      ALIAS:
	VScrollbarGet=1


