#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @param menu: the menu item to initialize
 * @return Returns nothing.
 * @brief Initialize a menu item to default values
 *
 * Sets up the internal variables for the menu item and places the icon from
 * @a image and label from @a title in the menu item.
 */
void ewl_menu_base_init(Ewl_Menu_Base * menu)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("menu", menu);

	/*
	 * Initialize the defaults of the inherited fields.
	 */
	ewl_menu_item_init(EWL_MENU_ITEM(menu));
	ewl_widget_appearance_set(EWL_WIDGET(menu), "menu_base");
	ewl_widget_inherit(EWL_WIDGET(menu), "menu_base");

	ewl_callback_del(EWL_WIDGET(menu), EWL_CALLBACK_CLICKED,
			 ewl_menu_item_clicked_cb);

	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_SELECT,
			    ewl_menu_base_expand_cb, NULL);

	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_DESELECT,
			    ewl_menu_base_collapse_cb, NULL);

	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_DESTROY,
			    ewl_menu_base_destroy_cb, NULL);

	/*
	 * The popbox actually holds the children, and is simply added to the
	 * popup part of the menu when the menu is expanded.
	 */
	menu->popbox = ewl_vbox_new();
	ewl_object_alignment_set(EWL_OBJECT(menu->popbox),
				 EWL_FLAG_ALIGN_LEFT | EWL_FLAG_ALIGN_TOP);
	ewl_callback_append(menu->popbox, EWL_CALLBACK_KEY_DOWN,
			    ewl_menu_base_popbox_key_down_cb, NULL);
	ewl_widget_show(menu->popbox);

	/*
	 * Redirect the container so that newly added children go in the popup
	 * menu.
	 */
	ewl_container_redirect_set(EWL_CONTAINER(menu),
				   EWL_CONTAINER(menu->popbox));
	ewl_widget_internal_set(menu->popbox, TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns a pointer to a new menu item on success, NULL on failure.
 * @brief Create a new menu item to place in a menu
 */
Ewl_Widget *ewl_menu_item_new(void)
{
	Ewl_Menu_Item  *item;

	DENTER_FUNCTION(DLEVEL_STABLE);

	item = NEW(Ewl_Menu_Item, 1);
	if (!item)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_menu_item_init(item);

	DRETURN_PTR(EWL_WIDGET(item), DLEVEL_STABLE);
}

/**
 * @param item: the item to be initialized
 * @return Returns no value.
 * @brief Initialize the fields of a menu item to their defaults
 */
int ewl_menu_item_init(Ewl_Menu_Item * item)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("item", item, FALSE);

	/*
	 * Initialize the inherited container fields.
	 */
	if (!ewl_box_init(EWL_BOX(item)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_box_orientation_set(EWL_BOX(item), EWL_ORIENTATION_HORIZONTAL);
	ewl_widget_appearance_set(EWL_WIDGET(item), "menuitem");
	ewl_widget_inherit(EWL_WIDGET(item), "menuitem");

	ewl_object_fill_policy_set(EWL_OBJECT(item), EWL_FLAG_FILL_HFILL);

	ewl_callback_append(EWL_WIDGET(item), EWL_CALLBACK_CONFIGURE,
			    ewl_menu_item_configure_cb, NULL);
	ewl_callback_append(EWL_WIDGET(item), EWL_CALLBACK_CLICKED,
			    ewl_menu_item_clicked_cb, NULL);

	/*
	 * Intercept mouse events this will cause callbacks to children of
	 * this widget.
	 */
	ewl_container_callback_intercept(EWL_CONTAINER(item),
					 EWL_CALLBACK_SELECT);
	ewl_container_callback_intercept(EWL_CONTAINER(item),
					 EWL_CALLBACK_DESELECT);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @return Returns the text currently used by a menu item
 * @param item: the menu item
 * @brief Get the text of a menu item
 */
char *
ewl_menu_item_text_get( Ewl_Menu_Item *item )
{
	if (item->text)
		DRETURN_PTR(ewl_text_text_get(EWL_TEXT(item->text)), DLEVEL_STABLE);
	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @param item: the menu item of which to set the text
 * @param text: the text in string form
 * @brief Sets the text of a menu item
 */
void
ewl_menu_item_text_set( Ewl_Menu_Item *item, char *text )
{
	Ewl_Container *redirect;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("item", item);

	/*
	 * Save and restore after we've made our changes.
	 */
	redirect = ewl_container_redirect_get(EWL_CONTAINER(item));
	ewl_container_redirect_set(EWL_CONTAINER(item), NULL);

	if (text) {

		/*
		 * Setup the text object and add it to the menu.
		 */
		if (!item->text) {
			item->text = ewl_text_new();
			ewl_container_child_append(EWL_CONTAINER(item),
						   item->text);
			ewl_widget_show(item->text);
		}

		/*
		 * Set the request text.
		 */
		if (item->text) {
			ewl_text_text_set(EWL_TEXT(item->text), text);
			if (!item->icon)
				ewl_menu_item_image_set(item, NULL);
		}
	}
	else if (item->text) {
		ewl_widget_destroy(item->text);
		item->text = NULL;
	}

	ewl_container_redirect_set(EWL_CONTAINER(item), redirect);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns the image currently used by a menu item
 * @param item: the menu item
 * @brief Get the image of a menu item
 */
char *
ewl_menu_item_image_get(Ewl_Menu_Item *item)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("item", item, NULL);

	if (item->icon && ewl_widget_type_is(item->icon, "image"))
		DRETURN_PTR(ewl_image_file_get(EWL_IMAGE(item->icon)), DLEVEL_STABLE);
	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @param item: the menu item
 * @param image: the image filename
 * @brief Set the image of a menu item
 */
void
ewl_menu_item_image_set( Ewl_Menu_Item *item, char *image )
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("item", item);

	/*
	 * Destroy the icon if it's the wrong type.
	 */
	if (item->icon && (
		(image && !ewl_widget_type_is(item->icon, "image")) || 			 
 		(!image && !ewl_widget_type_is(item->icon, "spacer"))))  {
		ewl_widget_destroy(item->icon);
		item->icon = NULL;
	}

	/*
	 * Create the appropriate widget if necessary.
	 */
	if (!item->icon) {
		Ewl_Container *redirect;

		/*
		 * Save the current redirect and override to avoid issues with
		 * submenus
		 */
		redirect = ewl_container_redirect_get(EWL_CONTAINER(item));
		ewl_container_redirect_set(EWL_CONTAINER(item), NULL);

		/*
		 * Create the icon if one is requested, or a spacer if not, but
		 * there is text to be displayed.
		 */
		if (image)
			item->icon = ewl_image_new();
		else if (item->text)
			item->icon = ewl_spacer_new();

		/*
		 * Setup display prperties on icon if created.
		 */
		if (item->icon) {
			ewl_object_alignment_set(EWL_OBJECT(item->icon),
						 EWL_FLAG_ALIGN_CENTER);
			ewl_object_maximum_size_set(EWL_OBJECT(item->icon),
						    20, 20);
			ewl_container_child_prepend(EWL_CONTAINER(item),
						    item->icon);
			ewl_widget_show(item->icon);
		}

		ewl_container_redirect_set(EWL_CONTAINER(item), redirect);
	}

	if (image && item->icon)
		ewl_image_file_set(EWL_IMAGE(item->icon), image, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @return Returns a new menu item separator on success, NULL on failure.
 * @brief Create a separator menu item
 */
Ewl_Widget *ewl_menu_separator_new()
{
	Ewl_Menu_Separator *sep;

	DENTER_FUNCTION(DLEVEL_STABLE);

	sep = NEW(Ewl_Menu_Separator, 1);
	if (!sep)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_menu_separator_init(sep);

	DRETURN_PTR(EWL_WIDGET(sep), DLEVEL_STABLE);
}

/**
 * @param sep: the menu separator item to initialize
 * @return Returns no value.
 * @brief Initialize a menu separator item
 *
 * Sets up the internal fields of the menu separator item to
 * some sane defaults.
 */
void ewl_menu_separator_init(Ewl_Menu_Separator *sep)
{
	Ewl_Widget *separator;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("sep", sep);

	ewl_menu_item_init(EWL_MENU_ITEM(sep));

	separator = ewl_separator_new(EWL_ORIENTATION_HORIZONTAL);
	if (!separator)
		DRETURN(DLEVEL_STABLE);

	ewl_container_child_append(EWL_CONTAINER(sep), separator);
	ewl_widget_show(separator);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_item_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	int x;
	Ewl_Container *c;
	Ewl_Widget *child;

	DENTER_FUNCTION(DLEVEL_STABLE);

	c = EWL_CONTAINER(w);
	x = CURRENT_X(w);
	ecore_list_goto_first(c->children);
	while ((child = ecore_list_next(c->children))) {
		int width;

		width = ewl_object_preferred_w_get(EWL_OBJECT(child));
		ewl_object_place(EWL_OBJECT(child), x, CURRENT_Y(w), width,
				 CURRENT_H(w));
		x += width;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_item_clicked_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Menu_Item *item = EWL_MENU_ITEM(w);
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (item->inmenu)
		ewl_widget_hide(item->inmenu);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_base_expand_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Widget *child;
	Ewl_Container *pb;
	Ewl_Menu_Base *menu = EWL_MENU_BASE(w);
	Ewl_Menu_Item *item = EWL_MENU_ITEM(w);

	DENTER_FUNCTION(DLEVEL_STABLE);

	if (!menu->popup)
		DRETURN(DLEVEL_STABLE);

	if (!REALIZED(menu->popup))
		ewl_container_child_append(EWL_CONTAINER(menu->popup),
					   menu->popbox);

	if (item->inmenu) {
		if (!REALIZED(menu->popup)) {
			ewl_callback_append(menu->popup, EWL_CALLBACK_SHOW,
					    ewl_menu_base_popup_show_cb,
					    item->inmenu);
			ewl_callback_append(menu->popup, EWL_CALLBACK_HIDE,
					    ewl_menu_base_popup_hide_cb,
					    item->inmenu);
		}
		ewl_object_minimum_w_set(EWL_OBJECT(menu->popup),
					 CURRENT_W(menu));
	}

	pb = EWL_CONTAINER(menu->popbox);

	ecore_list_goto_first(pb->children);

	/*
	 * Give all the items in the submenu a reference to this popup.
	 */
	while ((child = ecore_list_next(pb->children))) {
		if (ewl_widget_type_is(child, "menuitem")) {
			item = EWL_MENU_ITEM(child);
			item->inmenu = menu->popup;
		}
	}

	ewl_widget_show(menu->popup);
	ewl_widget_focus_send(menu->popbox);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_base_collapse_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Menu_Base *menu;
	Ewl_Widget    *focused;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = EWL_MENU_BASE(w);

	focused = ewl_widget_focused_get();
	if (!focused || !ewl_container_parent_of(menu->popbox, focused))
		ewl_widget_hide(menu->popup);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_base_popup_hide_cb(Ewl_Widget * w __UNUSED__,
			void *ev_data __UNUSED__, void *user_data)
{
	Ewl_Widget *ppop = user_data;
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_widget_hide(ppop);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_base_popup_show_cb(Ewl_Widget * w __UNUSED__,
			void *ev_data __UNUSED__, void *user_data)
{
	Ewl_Widget *ppop = user_data;
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_widget_show(ppop);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_base_popbox_key_down_cb(Ewl_Widget *w, void *ev_data, void *user_data)
{
	Ewl_Event_Key_Down *ev = ev_data;
	DENTER_FUNCTION(DLEVEL_STABLE);

	printf("Menu item: %s\n", ev->keyname);
	if (!strcmp(ev->keyname, "Down")) {
		printf("Menu item down\n");
	}
	else if (!strcmp(ev->keyname, "Up")) {
		printf("Menu item up\n");
	}
	else if (!strcmp(ev->keyname, "Left")) {
		printf("Menu item collapse\n");
	}
	else if (!strcmp(ev->keyname, "Right")) {
		printf("Menu item expand\n");
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_base_destroy_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Menu_Base      *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);

	menu = EWL_MENU_BASE(w);

	if (menu->popup)
		ewl_widget_destroy(menu->popup);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
