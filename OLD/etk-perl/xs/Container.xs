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


MODULE = Etk::Container	PACKAGE = Etk::Container	PREFIX = etk_container_

void
etk_container_remove_all(container)
	Etk_Container * container
	ALIAS:
	RemoveAll=1

void
etk_container_add(container, widget)
	Etk_Container *	container
	Etk_Widget *	widget
      ALIAS:
	Add=1

int
etk_container_border_width_get(container)
	Etk_Container *	container
      ALIAS:
	BorderWidthGet=1

void
etk_container_border_width_set(container, border_width)
	Etk_Container *	container
	int	border_width
      ALIAS:
	BorderWidthSet=1

void
etk_container_child_space_fill(child, child_space, hfill, vfill, xalign, yalign)
	Etk_Widget *	child
	Etk_Geometry *	child_space
	Etk_Bool	hfill
	Etk_Bool	vfill
	float	xalign
	float	yalign
      ALIAS:
	ChildSpaceFill=1
	
Evas_List *
etk_container_children_get(container)
	Etk_Container	*container
      ALIAS:
	ChildrenGet=1

Etk_Bool
etk_container_is_child(container, widget)
	Etk_Container * container
	Etk_Widget * widget
      ALIAS:
	IsChild=1

void
etk_container_remove(widget)
	Etk_Widget *	widget
      ALIAS:
	Remove=1


