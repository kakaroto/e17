#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @return Returns NULL on failure, or a pointer to a new menubar on success.
 * @brief Allocate and initialize a new menubar widget
 */
Ewl_Widget *
ewl_menubar_new(void)
{
	Ewl_Menubar *mb = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	mb = NEW(Ewl_Menubar, 1);
	if (!mb)
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	
	if (!ewl_menubar_init(mb)) {
		ewl_widget_destroy(EWL_WIDGET(mb));
		mb = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(mb), DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure or a pointer to a new horizontal menubar
 * on success
 * @brief Creates a new Ewl_Menubar in a horizontal orientation
 */
Ewl_Widget *
ewl_hmenubar_new(void)
{
	Ewl_Widget *mb = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	mb = ewl_menubar_new();
	if (!mb)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_menubar_orientation_set(EWL_MENUBAR(mb), 
				EWL_ORIENTATION_HORIZONTAL);

	DRETURN_PTR(mb, DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure or a pointer to a new vertical menubar on
 * success
 * @brief Creates a new Ewl_Menubar in a vertical orientation 
 */
Ewl_Widget *
ewl_vmenubar_new(void)
{
	Ewl_Widget *mb = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	mb = ewl_menubar_new();
	if (!mb)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_menubar_orientation_set(EWL_MENUBAR(mb), 
				EWL_ORIENTATION_VERTICAL);

	DRETURN_PTR(mb, DLEVEL_STABLE);
}

/**
 * @param mb: the menubar to initialize
 * @return Returns TRUE on success and FALSE on failure
 * @brief Initialize a menubar to default values
 */
int
ewl_menubar_init(Ewl_Menubar *mb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mb", mb, FALSE);

	if (!ewl_box_init(EWL_BOX(mb))) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	ewl_widget_appearance_set(EWL_WIDGET(mb), EWL_MENUBAR_TYPE);
	ewl_widget_inherit(EWL_WIDGET(mb), EWL_MENUBAR_TYPE);

	mb->inner_box = ewl_hbox_new();
	ewl_container_child_append(EWL_CONTAINER(mb),
					EWL_WIDGET(mb->inner_box));
	ewl_widget_internal_set(EWL_WIDGET(mb->inner_box), TRUE);
	ewl_widget_show(EWL_WIDGET(mb->inner_box));

	ewl_container_redirect_set(EWL_CONTAINER(mb),
					EWL_CONTAINER(mb->inner_box));

	ewl_container_add_notify_set(EWL_CONTAINER(mb->inner_box),
					ewl_menubar_cb_child_add);

	ewl_menubar_orientation_set(mb, EWL_ORIENTATION_HORIZONTAL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param mb: The menubar to set the orientation on
 * @param o: The orientation to set onto the menubar
 * @return Returns no value.
 * @brief Sets the orientation of the menubar
 */
void
ewl_menubar_orientation_set(Ewl_Menubar *mb, Ewl_Orientation o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("mb", mb);
	DCHECK_TYPE("mb", mb, EWL_MENUBAR_TYPE);

	ewl_box_orientation_set(EWL_BOX(mb), o);
	if (o == EWL_ORIENTATION_HORIZONTAL) {
		ewl_object_fill_policy_set(EWL_OBJECT(mb),
				EWL_FLAG_FILL_HFILL);
		ewl_box_orientation_set(EWL_BOX(mb->inner_box), 
				EWL_ORIENTATION_HORIZONTAL);
		ewl_object_fill_policy_set(EWL_OBJECT(mb->inner_box),
				EWL_FLAG_FILL_HFILL);

	} else if (o == EWL_ORIENTATION_VERTICAL) {
		ewl_object_fill_policy_set(EWL_OBJECT(mb),
				EWL_FLAG_FILL_VFILL);
		ewl_box_orientation_set(EWL_BOX(mb->inner_box),
				EWL_ORIENTATION_VERTICAL);
		ewl_object_fill_policy_set(EWL_OBJECT(mb->inner_box),
				EWL_FLAG_FILL_VFILL);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param mb: The menubar to get the orientation from
 * @return Returns the orientation of the menubar
 * @brief Gets the orientation of the menubar
 */
Ewl_Orientation
ewl_menubar_orientation_get(Ewl_Menubar *mb)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("mb", mb, EWL_ORIENTATION_HORIZONTAL);
	DCHECK_TYPE_RET("mb", mb, EWL_MENUBAR_TYPE, EWL_ORIENTATION_HORIZONTAL);

	DRETURN_INT(ewl_box_orientation_get(EWL_BOX(mb)), DLEVEL_STABLE);
}


/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child add callback
 */
void
ewl_menubar_cb_child_add(Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Menubar *mb;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	mb = EWL_MENUBAR(c);
	if (ewl_widget_type_is(w, "menu")) 
		EWL_MENU(w)->menubar_parent = EWL_WIDGET(mb);

	DLEAVE_FUNCTION(DLEVEL_STABLE);

}

