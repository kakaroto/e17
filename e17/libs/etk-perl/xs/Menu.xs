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


MODULE = Etk::Menu::Bar	PACKAGE = Etk::Menu::Bar	PREFIX = etk_menu_bar_
	
Etk_Menu_Bar *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MENU_BAR(etk_menu_bar_new());
	OUTPUT:
	RETVAL

MODULE = Etk::Menu::Item	PACKAGE = Etk::Menu::Item	PREFIX = etk_menu_item_

void
etk_menu_item_activate(menu_item)
	Etk_Menu_Item *	menu_item
      ALIAS:
	Activate=1

void
etk_menu_item_deselect(menu_item)
	Etk_Menu_Item *	menu_item
      ALIAS:
	Deselect=1

const char *
etk_menu_item_label_get(menu_item)
	Etk_Menu_Item *	menu_item
      ALIAS:
	LabelGet=1

void
etk_menu_item_label_set(menu_item, label)
	Etk_Menu_Item *	menu_item
	char *	label
      ALIAS:
	LabelSet=1

Etk_Menu_Item *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MENU_ITEM(etk_menu_item_new());
	OUTPUT:
	RETVAL

Etk_Menu_Item *
etk_menu_item_new_from_stock(stock_id)
	Etk_Stock_Id	stock_id
      ALIAS:
	NewFromStock=1
	CODE:
	RETVAL = ETK_MENU_ITEM(etk_menu_item_new_from_stock(stock_id));
	OUTPUT:
	RETVAL

Etk_Menu_Item *
etk_menu_item_new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_MENU_ITEM(etk_menu_item_new_with_label(label));
	OUTPUT:
	RETVAL

void
etk_menu_item_select(menu_item)
	Etk_Menu_Item *	menu_item
      ALIAS:
	Select=1

void
etk_menu_item_set_from_stock(menu_item, stock_id)
	Etk_Menu_Item *	menu_item
	Etk_Stock_Id	stock_id
      ALIAS:
	SetFromStock=1

void
etk_menu_item_submenu_set(menu_item, submenu)
	Etk_Menu_Item *	menu_item
	Etk_Menu *	submenu
      ALIAS:
	SubmenuSet=1

Etk_Menu *
etk_menu_item_submenu_get(menu_item)
	Etk_Menu_Item * menu_item
	ALIAS:
	SubmenuGet=1


MODULE = Etk::Menu::Item::Check	PACKAGE = Etk::Menu::Item::Check	PREFIX = etk_menu_item_check_
	
Etk_Menu_Item_Check *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MENU_ITEM_CHECK(etk_menu_item_check_new());
	OUTPUT:
	RETVAL

Etk_Menu_Item_Check *
etk_menu_item_check_new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_MENU_ITEM_CHECK(etk_menu_item_check_new_with_label(label));
	OUTPUT:
	RETVAL

Etk_Bool
etk_menu_item_check_active_get(check_item)
	Etk_Menu_Item_Check *	check_item
      ALIAS:
	ActiveGet=1

void
etk_menu_item_check_active_set(check_item, active)
	Etk_Menu_Item_Check *	check_item
	Etk_Bool	active
      ALIAS:
	ActiveSet=1


MODULE = Etk::Menu::Item::Image	PACKAGE = Etk::Menu::Item::Image	PREFIX = etk_menu_item_image_

Etk_Menu_Item_Image *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MENU_ITEM_IMAGE(etk_menu_item_image_new());
	OUTPUT:
	RETVAL

Etk_Menu_Item_Image *
etk_menu_item_image_new_from_stock(stock_id)
	Etk_Stock_Id	stock_id
      ALIAS:
	NewFromStock=1
	CODE:
	RETVAL = ETK_MENU_ITEM_IMAGE(etk_menu_item_image_new_from_stock(stock_id));
	OUTPUT:
	RETVAL

Etk_Menu_Item_Image *
etk_menu_item_image_new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_MENU_ITEM_IMAGE(etk_menu_item_image_new_with_label(label));
	OUTPUT:
	RETVAL

void
etk_menu_item_image_set(image_item, image)
	Etk_Menu_Item_Image *	image_item
	Etk_Image *	image
      ALIAS:
	Set=1


MODULE = Etk::Menu::Item::Radio	PACKAGE = Etk::Menu::Item::Radio	PREFIX = etk_menu_item_radio_
	
Etk_Menu_Item_Radio *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MENU_ITEM_RADIO(etk_menu_item_radio_new(NULL));
	OUTPUT:
	RETVAL
	
Etk_Menu_Item_Radio *
etk_menu_item_radio_new_from_widget(radio_item)
	Etk_Menu_Item_Radio *	radio_item
      ALIAS:
	NewFromWidget=1
	CODE:
	RETVAL = ETK_MENU_ITEM_RADIO(etk_menu_item_radio_new_from_widget(radio_item));
	OUTPUT:
	RETVAL

Etk_Menu_Item_Radio *
etk_menu_item_radio_new_with_label(label)
	char *	label
      ALIAS:
	NewWithLabel=1
	CODE:
	RETVAL = ETK_MENU_ITEM_RADIO(etk_menu_item_radio_new_with_label(label, NULL));
	OUTPUT:
	RETVAL	

Etk_Menu_Item_Radio *
etk_menu_item_radio_new_with_label_from_widget(label, radio_item)
	char *	label
	Etk_Menu_Item_Radio *	radio_item
      ALIAS:
	NewWithLabelFromWidget=1
	CODE:
	RETVAL = ETK_MENU_ITEM_RADIO(etk_menu_item_radio_new_with_label_from_widget(label, 
				radio_item));
	OUTPUT:
	RETVAL

MODULE = Etk::Menu::Item::Separator	PACKAGE = Etk::Menu::Item::Separator	PREFIX = etk_menu_item_separator_

Etk_Menu_Item_Separator *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MENU_ITEM_SEPARATOR(etk_menu_item_separator_new());
	OUTPUT:
	RETVAL


MODULE = Etk::Menu::Shell	PACKAGE = Etk::Menu::Shell	PREFIX = etk_menu_shell_

void
etk_menu_shell_append(menu_shell, item)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
      ALIAS:
	Append=1

void
etk_menu_shell_append_relative(menu_shell, item, relative)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
	Etk_Menu_Item *	relative
      ALIAS:
	AppendRelative=1

void
etk_menu_shell_insert(menu_shell, item, position)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
	int	position
      ALIAS:
	Insert=1

Evas_List *
etk_menu_shell_items_get(menu_shell)
	Etk_Menu_Shell *	menu_shell
      ALIAS:
	ItemsGet=1

void
etk_menu_shell_prepend(menu_shell, item)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
      ALIAS:
	Prepend=1

void
etk_menu_shell_prepend_relative(menu_shell, item, relative)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
	Etk_Menu_Item *	relative
      ALIAS:
	PrependRelative=1

void
etk_menu_shell_remove(menu_shell, item)
	Etk_Menu_Shell *	menu_shell
	Etk_Menu_Item *	item
      ALIAS:
	Remove=1

MODULE = Etk::Menu	PACKAGE = Etk::Menu	PREFIX = etk_menu_

Etk_Menu *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_MENU(etk_menu_new());
	OUTPUT:
	RETVAL

void
etk_menu_popdown(menu)
	Etk_Menu *	menu
      ALIAS:
	Popdown=1

void
etk_menu_popup(menu)
	Etk_Menu *	menu
      ALIAS:
	Popup=1

void
etk_menu_popup_at_xy(menu, x, y)
	Etk_Menu *	menu
	int	x
	int	y
      ALIAS:
	PopupAtXy=1

void
etk_menu_popup_in_direction(menu, direction)
	Etk_Menu *menu
	Etk_Popup_Direction direction
	ALIAS:
	PopupInDirection=1
	
void
etk_menu_popup_at_xy_in_direction(menu, x, y, direction)
	Etk_Menu *menu
	int	x
	int	y
	Etk_Popup_Direction direction
	ALIAS:
	PopupAtXYInDirection=1

Etk_Menu_Item *
etk_menu_parent_item_get(menu)
	Etk_Menu *menu
	ALIAS:
	ParentItemGet=1


