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


MODULE = Etk::VBox	PACKAGE = Etk::VBox	PREFIX = etk_vbox_
	
Etk_VBox *
new(class, homogeneous=ETK_FALSE, spacing=0)
	SV	*class
	Etk_Bool	homogeneous
	int	spacing
	CODE:
	RETVAL = ETK_VBOX(etk_vbox_new(homogeneous, spacing));
	OUTPUT:
	RETVAL

MODULE = Etk::HBox	PACKAGE = Etk::HBox	PREFIX = etk_hbox_
	
Etk_HBox *
new(class, homogeneous=ETK_FALSE, spacing=0)
	SV	*class
	Etk_Bool	homogeneous
	int	spacing
	CODE:
	RETVAL = ETK_HBOX(etk_hbox_new(homogeneous, spacing));
	OUTPUT:
	RETVAL

MODULE = Etk::Box		PACKAGE = Etk::Box	PREFIX = etk_box_

void
etk_box_child_packing_get(box, child)
	Etk_Box *	box
	Etk_Widget *	child
      ALIAS:
	ChildPackingGet=1
     PPCODE:
       Etk_Box_Fill_Policy   	fill;
       int 	        padding;
       
       etk_box_child_packing_get(box, child, &fill, &padding);
       EXTEND(SP, 2);
       PUSHs(sv_2mortal(newSViv(fill)));
       PUSHs(sv_2mortal(newSViv(padding)));

void
etk_box_child_packing_set(box, child, fill, padding=0)
	Etk_Box *	box
	Etk_Widget *	child
	Etk_Box_Fill_Policy	fill
	int	padding
      ALIAS:
	ChildPackingSet=1

void
etk_box_child_position_get(box, child)
	Etk_Box *	box
	Etk_Widget *	child
      ALIAS:
	ChildPositionGet=1
     PPCODE:
       Etk_Box_Group   	group;
       int 	        pos;
       
       etk_box_child_position_get(box, child, &group, &pos);
       EXTEND(SP, 2);
       PUSHs(sv_2mortal(newSViv(group)));
       PUSHs(sv_2mortal(newSViv(pos)));

void
etk_box_child_position_set(box, child, group, pos)
	Etk_Box *	box
	Etk_Widget *	child
	Etk_Box_Group	group
	int	pos
	ALIAS:
	ChildPositionSet=1

Etk_Bool
etk_box_homogeneous_get(box)
	Etk_Box *	box
      ALIAS:
	HomogeneousGet=1

void
etk_box_homogeneous_set(box, homogeneous)
	Etk_Box *	box
	Etk_Bool	homogeneous
      ALIAS:
	HomogeneousSet=1

void
etk_box_prepend(box, child, group=ETK_BOX_START, fill=ETK_BOX_NONE, padding=0)
	Etk_Box *	box
	Etk_Widget *	child
	Etk_Box_Group	group
	Etk_Box_Fill_Policy fill
	int	padding
     ALIAS:
	Prepend=1

void
etk_box_append(box, child, group=ETK_BOX_START, fill=ETK_BOX_NONE, padding=0)
	Etk_Box *	box
	Etk_Widget *	child
	Etk_Box_Group	group
	Etk_Box_Fill_Policy fill
	int	padding
     ALIAS:
	Append=1

void
etk_box_insert(box, child, group, after, fill=ETK_BOX_NONE, padding=0)
	Etk_Box *	box
	Etk_Widget *	child
	Etk_Box_Group	group
	Etk_Widget *	after
	Etk_Box_Fill_Policy fill
	int	padding
     ALIAS:
	Insert=1

void
etk_box_insert_at(box, child, group, pos, fill=ETK_BOX_NONE, padding=0)
	Etk_Box *	box
	Etk_Widget *	child
	Etk_Box_Group	group
	int 	pos
	Etk_Box_Fill_Policy fill
	int	padding
     ALIAS:
	InsertAt=1

Etk_Widget *
etk_box_child_get_at(box, group, pos)
	Etk_Box *	box
	Etk_Box_Group	group
	int	pos
	ALIAS:
	ChildGetAt=1
	
int
etk_box_spacing_get(box)
	Etk_Box *	box
      ALIAS:
	SpacingGet=1

void
etk_box_spacing_set(box, spacing)
	Etk_Box *	box
	int	spacing
      ALIAS:
	SpacingSet=1

