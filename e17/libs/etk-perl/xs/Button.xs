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


MODULE = Etk::RadioButton	PACKAGE = Etk::RadioButton	PREFIX = etk_radio_button_

Etk_Widget *
etk_radio_button_new(group)
	Evas_List **	group
      ALIAS:
	New=1

Etk_Radio_Button *
etk_radio_button_new_from_widget(radio_button)
	Etk_Radio_Button *	radio_button
      ALIAS:
	NewFromWidget=1
	CODE:
	RETVAL = ETK_RADIO_BUTTON(etk_radio_button_new_from_widget(radio_button));
	OUTPUT:
	RETVAL

Etk_Radio_Button *
etk_radio_button_new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_RADIO_BUTTON(etk_radio_button_new_with_label(label, NULL));
	OUTPUT:
	RETVAL
	
Etk_Radio_Button *
etk_radio_button_new_with_label_from_widget(label, radio_button)
	char *	label
	Etk_Radio_Button *	radio_button
      ALIAS:
	NewWithLabelFromWidget=1
	CODE:
	RETVAL = ETK_RADIO_BUTTON(etk_radio_button_new_with_label_from_widget(label, radio_button));
	OUTPUT:
	RETVAL



MODULE = Etk::CheckButton		PACKAGE = Etk::CheckButton	PREFIX = etk_check_button_
	
Etk_Check_Button *
new(class)
	SV	*class
	CODE:
	RETVAL = ETK_CHECK_BUTTON(etk_check_button_new());
	OUTPUT:
	RETVAL

Etk_Check_Button *
new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_CHECK_BUTTON(etk_check_button_new_with_label(label));
	OUTPUT:
	RETVAL


MODULE = Etk::ToggleButton	PACKAGE = Etk::ToggleButton	PREFIX = etk_toggle_button_

Etk_Bool
etk_toggle_button_active_get(toggle_button)
	Etk_Toggle_Button *	toggle_button
      ALIAS:
	ActiveGet=1

void
etk_toggle_button_active_set(toggle_button, active)
	Etk_Toggle_Button *	toggle_button
	Etk_Bool	active
      ALIAS:
	ActiveSet=1

Etk_Toggle_Button *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_TOGGLE_BUTTON(etk_toggle_button_new());
	OUTPUT:
	RETVAL

Etk_Toggle_Button *
etk_toggle_button_new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_TOGGLE_BUTTON(etk_toggle_button_new_with_label(label));
	OUTPUT:
	RETVAL

void
etk_toggle_button_toggle(toggle_button)
	Etk_Toggle_Button *	toggle_button
      ALIAS:
	Toggle=1



MODULE = Etk::Button		PACKAGE = Etk::Button	PREFIX = etk_button_

void
etk_button_alignment_get(button)
	Etk_Button *	button
      ALIAS:
	AlignmentGet=1
      PPCODE:	
       float xalign;
       float yalign;
       
       etk_button_alignment_get(button, &xalign, &yalign);
       EXTEND(SP, 2);
       PUSHs(sv_2mortal(newSVnv(xalign)));
       PUSHs(sv_2mortal(newSVnv(yalign)));

void
etk_button_alignment_set(button, xalign, yalign)
	Etk_Button *	button
	float	xalign
	float	yalign
      ALIAS:
	AlignmentSet=1

void
etk_button_click(button)
	Etk_Button *	button
      ALIAS:
	Click=1

Etk_Image *
etk_button_image_get(button)
	Etk_Button *	button
      ALIAS:
	ImageGet=1

void
etk_button_image_set(button, image)
	Etk_Button *	button
	Etk_Image *	image
      ALIAS:
	ImageSet=1

const char *
etk_button_label_get(button)
	Etk_Button *	button
      ALIAS:
	LabelGet=1

void
etk_button_label_set(button, label)
	Etk_Button *	button
	char *	label
      ALIAS:
	LabelSet=1

Etk_Button *
new(class)
	SV	*class
	CODE:
	RETVAL = ETK_BUTTON(etk_button_new());
	OUTPUT:
	RETVAL

Etk_Button *
new_from_stock(stock_id)
	Etk_Stock_Id	stock_id
      ALIAS:
	NewFromStock=1
	CODE:
	RETVAL = ETK_BUTTON(etk_button_new_from_stock(stock_id));
	OUTPUT:
	RETVAL

Etk_Button *
new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_BUTTON(etk_button_new_with_label(label));
	OUTPUT:
	RETVAL

void
etk_button_press(button)
	Etk_Button *	button
      ALIAS:
	Press=1

void
etk_button_release(button)
	Etk_Button *	button
      ALIAS:
	Release=1

void
etk_button_set_from_stock(button, stock_id)
	Etk_Button *	button
	Etk_Stock_Id	stock_id
      ALIAS:
	SetFromStock=1

void
etk_button_style_set(button, style)
	Etk_Button * 	button
	Etk_Button_Style	style
	ALIAS:
	StyleSet=1

Etk_Button_Style
etk_button_style_get(button)
	Etk_Button *	button
	ALIAS:
	StyleGet=1

void
etk_button_stock_size_set(button, size)
	Etk_Button *	button
	Etk_Stock_Size	size
	ALIAS:
	StockSizeSet=1

Etk_Stock_Size
etk_button_stock_size_get(button)
	Etk_Button *	button
	ALIAS:
	StockSizeGet=1

