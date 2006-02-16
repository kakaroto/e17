#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @param menu: the menu item to initialize
 * @return Returns TRUE on success, FALSE otherwise.
 * @brief Initialize a menu item to default values
 *
 * Sets up the internal variables for the menu item and places the icon from
 * @a image and label from @a title in the menu item.
 */
int
ewl_menu_base_init(Ewl_Menu_Base *menu)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("menu", menu, FALSE);

	/*
	 * Initialize the defaults of the inherited fields.
	 */
	if (!ewl_menu_item_init(EWL_MENU_ITEM(menu)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(menu), EWL_MENU_BASE_TYPE);
	ewl_widget_inherit(EWL_WIDGET(menu), EWL_MENU_BASE_TYPE);

	ewl_callback_del(EWL_WIDGET(menu), EWL_CALLBACK_CLICKED,
					ewl_menu_item_clicked_cb);
	ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_FOCUS_IN,
					ewl_menu_base_expand_cb, NULL);
	
	/*ewl_callback_append(EWL_WIDGET(menu), EWL_CALLBACK_FOCUS_OUT,
					ewl_menu_base_collapse_cb, NULL);*/


	ewl_callback_prepend(EWL_WIDGET(menu), EWL_CALLBACK_DESTROY,
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

	DRETURN_INT(TRUE, DLEVEL_STABLE);
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
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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

	/*
	 * Give all the items in the submenu a reference to this popup.
	 */
	ecore_dlist_goto_first(pb->children);
	while ((child = ecore_dlist_next(pb->children))) {
		if (ewl_widget_type_is(child, "menu_item")) {
			item = EWL_MENU_ITEM(child);
			item->inmenu = menu->popup;
		}
	}

	ewl_widget_show(menu->popup);
	ewl_widget_focus_send(menu->popbox);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_base_collapse_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Menu_Base *menu;
	Ewl_Widget    *focused;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	menu = EWL_MENU_BASE(w);

	focused = ewl_widget_focused_get();
	if (!focused || !ewl_widget_parent_of(menu->popbox, focused))
		ewl_widget_hide(menu->popup);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_base_popup_hide_cb(Ewl_Widget *w __UNUSED__,
			void *ev_data __UNUSED__, void *user_data)
{
	Ewl_Widget *ppop;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	ppop = user_data;
	ewl_widget_hide(ppop);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_base_popup_show_cb(Ewl_Widget *w __UNUSED__,
			void *ev_data __UNUSED__, void *user_data)
{
	Ewl_Widget *ppop;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	ppop = user_data;
	ewl_widget_show(ppop);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_menu_base_popbox_key_down_cb(Ewl_Widget *w __UNUSED__, void *ev_data, 
					void *user_data __UNUSED__)
{
	Ewl_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	
	ev = ev_data;
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
ewl_menu_base_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Menu_Base *menu;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_MENU_BASE_TYPE);

	menu = EWL_MENU_BASE(w);
	if (menu->popup)
		ewl_widget_destroy(menu->popup);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

