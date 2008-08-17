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


MODULE = Etk::Dialog	PACKAGE = Etk::Dialog	PREFIX = etk_dialog_

Etk_Button *
etk_dialog_button_add(dialog, label, response_id)
	Etk_Dialog *	dialog
	char *	label
	int	response_id
      ALIAS:
	ButtonAdd=1
	CODE:
	RETVAL = ETK_BUTTON(etk_dialog_button_add(dialog, label, response_id));
	OUTPUT:
	RETVAL

Etk_Button *
etk_dialog_button_add_from_stock(dialog, stock_id, response_id)
	Etk_Dialog *	dialog
	int	stock_id
	int	response_id
      ALIAS:
	ButtonAddFromStock=1
	CODE:
	RETVAL = ETK_BUTTON(etk_dialog_button_add_from_stock(dialog, stock_id, response_id));
	OUTPUT:
	RETVAL
	
Etk_Bool
etk_dialog_action_area_homogeneous_get(dialog)
	Etk_Dialog *	dialog
	ALIAS:
	ActionAreaHomogeneousGet=1

void
etk_dialog_action_area_homogeneous_set(dialog, homogeneous)
	Etk_Dialog *    dialog
	Etk_Bool  	homogeneous
	ALIAS:
	ActionAreaHomogeneousSet=1

void
etk_dialog_action_area_alignment_set(dialog, align)
	Etk_Dialog *    dialog
	float	align
	ALIAS:
	ActionAreaAlignmentSet=1

float
etk_dialog_action_area_alignment_get(dialog)
	Etk_Dialog *    dialog
	ALIAS:
	ActionAreaAlignmentGet=1

Etk_Bool
etk_dialog_has_separator_get(dialog)
	Etk_Dialog *	dialog
      ALIAS:
	HasSeparatorGet=1

void
etk_dialog_has_separator_set(dialog, has_separator)
	Etk_Dialog *	dialog
	Etk_Bool	has_separator
      ALIAS:
	HasSeparatorSet=1

Etk_Widget *
etk_dialog_main_area_vbox_get(dialog)
	Etk_Dialog *    dialog
	ALIAS:
	MainAreaVboxGet=1

Etk_Widget *
etk_dialog_action_area_hbox_get(dialog)
	Etk_Dialog *    dialog
	ALIAS:
	MainAreaHboxGet=1

void
etk_dialog_button_response_id_set(dialog, button, response_id)
	Etk_Dialog *    dialog
	Etk_Button *	button
	int	response_id
	ALIAS:
	ButtonResponseIdSet=1

int
etk_dialog_button_response_id_get(button)
	Etk_Button *    button
	ALIAS:
	ButtonResponseIdGet=1
	
Etk_Dialog *
new(class)
	SV	* class
	CODE:
	RETVAL = ETK_DIALOG(etk_dialog_new());
	OUTPUT:
	RETVAL

void
etk_dialog_pack_button_in_action_area(dialog, button, response_id, group, fill_policy, padding)
	Etk_Dialog *	dialog
	Etk_Button *	button
	int	response_id
	Etk_Box_Group   group
	Etk_Box_Fill_Policy     fill_policy
	int	padding
      ALIAS:
	PackButtonInActionArea=1

void
etk_dialog_pack_in_main_area(dialog, widget, group, fill_policy, padding)
	Etk_Dialog *	dialog
	Etk_Widget *	widget
	Etk_Box_Group	group
	Etk_Box_Fill_Policy	fill_policy
	int	padding
      ALIAS:
	PackInMainArea=1

void
etk_dialog_pack_widget_in_action_area(dialog, widget, group, fill_policy, padding)
	Etk_Dialog *	dialog
	Etk_Widget *	widget
	Etk_Box_Group	group
	Etk_Box_Fill_Policy	fill_policy
	int	padding
      ALIAS:
	PackWidgetInActionArea=1


