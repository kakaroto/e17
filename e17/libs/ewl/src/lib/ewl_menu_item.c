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
	if (!ewl_button_init(EWL_BUTTON(item)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

//	ewl_box_orientation_set(EWL_BOX(item), EWL_ORIENTATION_HORIZONTAL);
	ewl_button_fill_policy_set(EWL_BUTTON(item), EWL_FLAG_FILL_HFILL);
	ewl_button_alignment_set(EWL_BUTTON(item), EWL_FLAG_ALIGN_LEFT);
	ewl_button_label_set(EWL_BUTTON(item), "");
	ewl_button_image_scale_to(EWL_BUTTON(item), 16, 16);
	ewl_widget_appearance_set(EWL_WIDGET(item), EWL_MENU_ITEM_TYPE);
	ewl_widget_inherit(EWL_WIDGET(item), EWL_MENU_ITEM_TYPE);

	ewl_object_fill_policy_set(EWL_OBJECT(item), EWL_FLAG_FILL_HFILL);
	ewl_object_fill_policy_set(EWL_OBJECT(EWL_BUTTON(item)->label_object),
			EWL_FLAG_FILL_HFILL);

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
