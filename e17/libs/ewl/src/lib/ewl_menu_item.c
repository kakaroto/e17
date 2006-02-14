#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns a pointer to a new menu item on success, NULL on failure.
 * @brief Create a new menu item to place in a menu
 */
Ewl_Widget *
ewl_menu_item_new(void)
{       
	Ewl_Menu_Item *item;

	DENTER_FUNCTION(DLEVEL_STABLE);

	item = NEW(Ewl_Menu_Item, 1);
	if (!item)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_menu_item_init(item)) {
		ewl_widget_destroy(EWL_WIDGET(item));
		item = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(item), DLEVEL_STABLE);
}

/**
 * @param item: the item to be initialized
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initialize the fields of a menu item to their defaults
 */ 
int
ewl_menu_item_init(Ewl_Menu_Item *item)
{       
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("item", item, FALSE);

	/*
	 * Initialize the inherited container fields.
	 */
	if (!ewl_box_init(EWL_BOX(item)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_box_orientation_set(EWL_BOX(item), EWL_ORIENTATION_HORIZONTAL);
	ewl_widget_appearance_set(EWL_WIDGET(item), EWL_MENU_ITEM_TYPE);
	ewl_widget_inherit(EWL_WIDGET(item), EWL_MENU_ITEM_TYPE);

	ewl_object_fill_policy_set(EWL_OBJECT(item), EWL_FLAG_FILL_HFILL);

	ewl_callback_append(EWL_WIDGET(item), EWL_CALLBACK_CLICKED,
			ewl_menu_item_clicked_cb, NULL);

	/*
	 * Intercept mouse events this will cause callbacks to children of
	 * this widget.
	 */
	ewl_container_callback_intercept(EWL_CONTAINER(item),
			EWL_CALLBACK_FOCUS_IN);
	ewl_container_callback_intercept(EWL_CONTAINER(item),
			EWL_CALLBACK_FOCUS_OUT);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @return Returns the text currently used by a menu item
 * @param item: the menu item
 * @return Returns the text of the item, or NULL otherwise
 * @brief Get the text of a menu item
 */
const char *
ewl_menu_item_text_get(Ewl_Menu_Item *item)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("item", item, NULL);
	DCHECK_TYPE_RET("item", item, EWL_MENU_ITEM_TYPE, NULL);

	if (item->text)
		DRETURN_PTR(ewl_label_text_get(EWL_LABEL(item->text)), 
				DLEVEL_STABLE);

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @param item: the menu item of which to set the text
 * @param text: the text in string form
 * @brief Sets the text of a menu item
 */
void
ewl_menu_item_text_set(Ewl_Menu_Item *item, const char *text)
{
	Ewl_Container *redirect;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("item", item);
	DCHECK_TYPE("item", item, EWL_MENU_ITEM_TYPE);

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
			item->text = ewl_label_new();
			ewl_container_child_append(EWL_CONTAINER(item),
					item->text);
			ewl_object_fill_policy_set(EWL_OBJECT(item->text),
					EWL_FLAG_FILL_HFILL);
			ewl_object_alignment_set(EWL_OBJECT(item->text),
					EWL_FLAG_ALIGN_LEFT);
			ewl_widget_show(item->text);
		}

		/*
		 * Set the request text.
		 */
		if (item->text) {
			ewl_label_text_set(EWL_LABEL(item->text), text);
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
const char *
ewl_menu_item_image_get(Ewl_Menu_Item *item)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("item", item, NULL);
	DCHECK_TYPE_RET("item", item, EWL_MENU_ITEM_TYPE, NULL);

	if (item->icon && ewl_widget_type_is(item->icon, "image"))
		DRETURN_PTR(ewl_image_file_path_get(EWL_IMAGE(item->icon)), 
				DLEVEL_STABLE);

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @param item: the menu item
 * @param image: the image filename
 * @brief Set the image of a menu item
 */
void
ewl_menu_item_image_set(Ewl_Menu_Item *item, const char *image)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("item", item);
	DCHECK_TYPE("item", item, EWL_MENU_ITEM_TYPE);

	/*
	 * Destroy the icon if it's the wrong type.
	 */
	if (item->icon && ((image && !ewl_widget_type_is(item->icon, "image")) 
			|| (!image && 
				!ewl_widget_type_is(item->icon, "spacer"))))  
	{
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
			ewl_object_minimum_size_set(EWL_OBJECT(item->icon),
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

void
ewl_menu_item_clicked_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
		void *user_data __UNUSED__)
{       
	Ewl_Menu_Item *item;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	item = EWL_MENU_ITEM(w); 
	if (item->inmenu)
		ewl_widget_hide(item->inmenu);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


