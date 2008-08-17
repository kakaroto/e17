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

MODULE = Etk::VPaned	PACKAGE = Etk::VPaned	PREFIX = etk_vpaned_

Etk_VPaned *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_VPANED(etk_vpaned_new());
	OUTPUT:
	RETVAL

MODULE = Etk::HPaned	PACKAGE = Etk::HPaned	PREFIX = etk_hpaned_

Etk_HPaned *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_HPANED(etk_hpaned_new());
	OUTPUT:
	RETVAL

MODULE = Etk::Paned	PACKAGE = Etk::Paned	PREFIX = etk_paned_
	
Etk_Widget *
etk_paned_child1_get(paned)
	Etk_Paned *	paned
      ALIAS:
	Child1Get=1

void
etk_paned_child1_set(paned, child, expand)
	Etk_Paned *	paned
	Etk_Widget *	child
	Etk_Bool	expand
      ALIAS:
	Child1Set=1

Etk_Widget *
etk_paned_child2_get(paned)
	Etk_Paned *	paned
      ALIAS:
	Child2Get=1

void
etk_paned_child2_set(paned, child, expand)
	Etk_Paned *	paned
	Etk_Widget *	child
	Etk_Bool	expand
      ALIAS:
	Child2Set=1

int
etk_paned_position_get(paned)
	Etk_Paned *	paned
      ALIAS:
	PositionGet=1

void
etk_paned_position_set(paned, position)
	Etk_Paned *	paned
	int	position
      ALIAS:
	PositionSet=1

void
etk_paned_child1_expand_set(paned, expand)
	Etk_Paned *	paned
	Etk_Bool	expand
	ALIAS:
	Child1ExpandSet=1

void
etk_paned_child2_expand_set(paned, expand)
	Etk_Paned *	paned
	Etk_Bool	expand
	ALIAS:
	Child2ExpandSet=1

Etk_Bool
etk_paned_child1_expand_get(paned)
	Etk_Paned *	paned
	ALIAS:
	Child1ExpandGet=1

Etk_Bool
etk_paned_child2_expand_get(paned)
	Etk_Paned *	paned
	ALIAS:
	Child2ExpandGet=1


