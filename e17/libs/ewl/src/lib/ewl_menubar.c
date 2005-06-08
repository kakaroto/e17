#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

/**
 * @param orientation: the desirec orientation of the menubar
 * @return Returns NULL on failure, or a pointer to a new menubar on success.
 * @brief Allocate and initialize a new menubar widget
 */
Ewl_Widget *ewl_menubar_new(Ewl_Orientation orientation)
{
	Ewl_Menubar *mb = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	mb = NEW(Ewl_Menubar, 1);
	if (!mb)
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	
	if (!ewl_menubar_init(mb, orientation)) {
		ewl_widget_destroy(EWL_WIDGET(mb));
		mb = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(mb), DLEVEL_STABLE);
}

/**
 * @param mb: the menubar to initialize
 * @param orientation: the orientation for the menubar
 * @return Returns 1 on success and 0 on failure
 * @brief Initialize a menubar to default values
 */
int ewl_menubar_init(Ewl_Menubar *mb, Ewl_Orientation orientation)
{
	Ewl_Widget *w = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mb", mb, FALSE);

	w = EWL_WIDGET(mb);

	if (!ewl_box_init(EWL_BOX(mb), orientation)) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	if (orientation == EWL_ORIENTATION_HORIZONTAL) {
		ewl_object_fill_policy_set(EWL_OBJECT(w),
				EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);
		mb->inner_box = EWL_BOX(ewl_hbox_new());
		ewl_object_fill_policy_set(EWL_OBJECT(mb->inner_box),
				EWL_FLAG_FILL_HFILL | EWL_FLAG_FILL_VSHRINK);

	} else if (orientation == EWL_ORIENTATION_VERTICAL) {
		ewl_object_fill_policy_set(EWL_OBJECT(w),
				EWL_FLAG_FILL_VFILL | EWL_FLAG_FILL_HSHRINK);
		mb->inner_box = EWL_BOX(ewl_vbox_new());
		ewl_object_fill_policy_set(EWL_OBJECT(mb->inner_box),
				EWL_FLAG_FILL_VFILL | EWL_FLAG_FILL_HSHRINK);
	}
	ewl_container_child_append(EWL_CONTAINER(mb),
					EWL_WIDGET(mb->inner_box));

	/*
	ewl_object_fill_policy_set(EWL_OBJECT(mb->inner_box), 
					EWL_FLAG_FILL_SHRINK);
					*/
	ewl_widget_internal_set(EWL_WIDGET(mb->inner_box), TRUE);
	ewl_container_redirect_set(EWL_CONTAINER(mb),
					EWL_CONTAINER(mb->inner_box));

	ewl_widget_show(EWL_WIDGET(mb->inner_box));

	ewl_widget_appearance_set(w, "menubar");
	ewl_widget_inherit(w, "menubar");

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param mb: The Ewl_Menubar to add the menu too.
 * @param img: The image to place beside the menu.
 * @param title: The name to give the menu.
 * @return Returns NULL on failure and a new IMenu on success
 * @brief Creates a new menu and adds it to the menubar. Returns the menu to
 * be setup as required by the app.
 */
Ewl_Widget *ewl_menubar_menu_add(Ewl_Menubar *mb, char *img, char *title)
{
	Ewl_Widget *menu = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mb", mb, FALSE);

	menu = ewl_imenu_new(img, title);
	ewl_container_child_append(EWL_CONTAINER(mb), menu);
	ewl_object_fill_policy_set(EWL_OBJECT(menu), EWL_FLAG_FILL_NONE);
	ewl_widget_show(menu);

	DRETURN_PTR(EWL_WIDGET(menu), DLEVEL_STABLE);
}

/**
 * @param mb: The Ewl_Menubar to add the seperator too.
 * @return Returns NULL on failure and a new Seperator on success
 * @brief Creates a new seperator in the menubar and returns it to the app.
 */
Ewl_Widget *ewl_menubar_seperator_add(Ewl_Menubar *mb)
{
	Ewl_Widget *separator = NULL;
	Ewl_Orientation orient;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mb", mb, FALSE);

	orient = ewl_box_orientation_get(EWL_BOX(mb));
	if (orient == EWL_ORIENTATION_HORIZONTAL)
		orient = EWL_ORIENTATION_VERTICAL;
	else if (orient == EWL_ORIENTATION_VERTICAL)
		orient = EWL_ORIENTATION_HORIZONTAL;

	separator = ewl_separator_new(orient);
	ewl_container_child_append(EWL_CONTAINER(mb), separator);
	ewl_widget_show(separator);

	DRETURN_PTR(EWL_WIDGET(separator), DLEVEL_STABLE);
}



