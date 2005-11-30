#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_paned_grabber_mouse_down_cb(Ewl_Widget *w, void *ev, 
						void *user_data);
static void ewl_paned_grabber_mouse_up_cb(Ewl_Widget *w, void *ev, 
						void *user_data);
static void ewl_paned_grabber_mouse_move_cb(Ewl_Widget *w, void *ev, 
						void *user_data);
static void ewl_paned_grabber_focus_in_cb(Ewl_Widget *w, void *ev, 
						void *user_data);
static void ewl_paned_grabber_focus_out_cb(Ewl_Widget *w, void *ev, 
						void *user_data);

/**
 * @return Returns NULL on failure, or a pointer to a new paned widget on success.
 * @brief Allocate and initialize a new paned widget
 */
Ewl_Widget *
ewl_paned_new(void) 
{
	Ewl_Paned *pane = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	pane = NEW(Ewl_Paned, 1);
	if (!pane)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_paned_init(pane)) {
		ewl_widget_destroy(EWL_WIDGET(pane));
		pane = NULL;
	}

	DRETURN_PTR(pane, DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure, or a pointer to a new paned widget on success.
 * @brief Allocate and initialize a new paned widget with horizontal orientation
 */
Ewl_Widget *
ewl_hpaned_new(void)
{
	Ewl_Widget *pane = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	pane = ewl_paned_new();
	if (pane)
		ewl_box_orientation_set(EWL_BOX(pane),
					EWL_ORIENTATION_HORIZONTAL);

	DRETURN_PTR(pane, DLEVEL_STABLE);
}

/**
 * @return Returns NULL on failure, or a pointer to a new paned widget on success.
 * @brief Allocate and initialize a new paned widget with vertical orientation
 */
Ewl_Widget *
ewl_vpaned_new(void)
{
	Ewl_Widget *pane = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	pane = ewl_paned_new();
	if (pane)
		ewl_box_orientation_set(EWL_BOX(pane),
					EWL_ORIENTATION_VERTICAL);

	DRETURN_PTR(pane, DLEVEL_STABLE);
}

/**
 * @param p: the paned widget to initialize
 * @return Returns 1 on success or 0 on failure
 * @brief Initialize a new paned widget to default values
 */
int
ewl_paned_init(Ewl_Paned *p)
{
	Ewl_Widget *w = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, FALSE);

	w = EWL_WIDGET(p);

	if (!ewl_box_init(EWL_BOX(p))) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	ewl_widget_appearance_set(w, "paned");
	ewl_widget_inherit(w, "paned");

	ewl_callback_prepend(EWL_WIDGET(p), EWL_CALLBACK_CONFIGURE, 
						ewl_paned_configure_cb, NULL);

	p->grabbed = FALSE;

	p->first = EWL_BOX(ewl_vbox_new());
	ewl_container_child_append(EWL_CONTAINER(p), EWL_WIDGET(p->first));
	ewl_object_fill_policy_set(EWL_OBJECT(p->first), EWL_FLAG_FILL_ALL);
	ewl_widget_show(EWL_WIDGET(p->first));

	p->grabber = ewl_vseparator_new();
	ewl_object_custom_w_set(EWL_OBJECT(p->grabber), 10);

	ewl_container_child_append(EWL_CONTAINER(p), EWL_WIDGET(p->grabber));
	ewl_callback_append(EWL_WIDGET(p->grabber), EWL_CALLBACK_MOUSE_DOWN, 
						ewl_paned_grabber_mouse_down_cb, p);
	ewl_callback_append(EWL_WIDGET(p->grabber), EWL_CALLBACK_MOUSE_UP, 
						ewl_paned_grabber_mouse_up_cb, p);
	ewl_callback_append(EWL_WIDGET(p->grabber), EWL_CALLBACK_MOUSE_MOVE, 
						ewl_paned_grabber_mouse_move_cb, p);
	ewl_callback_append(EWL_WIDGET(p->grabber), EWL_CALLBACK_FOCUS_IN, 
						ewl_paned_grabber_focus_in_cb, p);
	ewl_callback_append(EWL_WIDGET(p->grabber), EWL_CALLBACK_FOCUS_OUT, 
						ewl_paned_grabber_focus_out_cb, p);
	ewl_widget_layer_set(EWL_WIDGET(p->grabber), 100);
	ewl_widget_show(EWL_WIDGET(p->grabber));

	p->second = EWL_BOX(ewl_vbox_new());
	ewl_container_child_append(EWL_CONTAINER(p), EWL_WIDGET(p->second));
	ewl_object_fill_policy_set(EWL_OBJECT(p->second), EWL_FLAG_FILL_ALL);
	ewl_widget_show(EWL_WIDGET(p->second));
    
	ewl_widget_internal_set(EWL_WIDGET(p->first), TRUE);
	ewl_widget_internal_set(EWL_WIDGET(p->second), TRUE);
	ewl_widget_internal_set(EWL_WIDGET(p->grabber), TRUE);

	p->active = EWL_POSITION_LEFT;
	ewl_container_redirect_set(EWL_CONTAINER(p), EWL_CONTAINER(p->first));

	ewl_widget_appearance_set(EWL_WIDGET(p->first), "first");
	ewl_widget_appearance_set(EWL_WIDGET(p->second), "second");

	/*Looks like we only do horizontally-split panes right now*/
	ewl_object_fill_policy_set(p->second, EWL_FLAG_FILL_NORMAL);
	ewl_object_fill_policy_set(p->first, EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_VFILL );
	ewl_object_fill_policy_set(EWL_OBJECT(p), EWL_FLAG_FILL_NORMAL);



	// ewl_widget_appearance_set(EWL_WIDGET(p->grabber), "grabber_horizontal");
	/*ewl_object_fill_policy_set(EWL_OBJECT(p), 
				   EWL_FLAG_FILL_HSHRINK | EWL_FLAG_FILL_VFILL);*/

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param p: the paned wiget to work on
 * @param pos: The Ewl_Position to set to the active partition
 * @returns Returns no value.
 * @brief Changes the current active pane of the paned widget
 */
void
ewl_paned_active_area_set(Ewl_Paned *p, Ewl_Position pos) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, "paned");

	if ((pos == EWL_POSITION_LEFT) || (pos == EWL_POSITION_TOP))
		ewl_container_redirect_set(EWL_CONTAINER(p),
						EWL_CONTAINER(p->first));
	else
		ewl_container_redirect_set(EWL_CONTAINER(p),
						EWL_CONTAINER(p->second));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: the paned wiget to work against
 * @return Returns the Ewl_Position specifing the active position
 * @brief Gets the current active partiton from the paned wiget
 */
Ewl_Position
ewl_paned_active_area_get(Ewl_Paned *p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, FALSE);
	DCHECK_TYPE("p", p, "paned");

	DRETURN_INT(p->active, DLEVEL_STABLE);
}

void
ewl_paned_configure_cb(Ewl_Widget *w, void *ev __UNUSED__,
		       void *user_data __UNUSED__)
{
	Ewl_Orientation o;
	Ewl_Paned *p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, "widget");

	p = EWL_PANED(w);

	/*
	 * Swap the orientation on the internal boxes if they are changed
	 */
	o = ewl_box_orientation_get(EWL_BOX(p));
	if (o == ewl_separator_orientation_get(EWL_SEPARATOR(p->grabber))) {
		if (o == EWL_ORIENTATION_HORIZONTAL) {
			ewl_separator_orientation_set(EWL_SEPARATOR(p->grabber),
					EWL_ORIENTATION_VERTICAL);
		}
		else {
			ewl_separator_orientation_set(EWL_SEPARATOR(p->grabber),
					EWL_ORIENTATION_HORIZONTAL);
		}
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_mouse_down_cb(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
							void *user_data)
{
	Ewl_Paned *p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	p = EWL_PANED(user_data);
	p->grabbed = TRUE;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_mouse_up_cb(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
							void *user_data)
{
	Ewl_Paned *p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("user_data", user_data);

	p = EWL_PANED(user_data);
	p->grabbed = FALSE;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_mouse_move_cb(Ewl_Widget *w __UNUSED__, void *ev, void *user_data)
{
	Ewl_Paned *p = NULL;
	Ewl_Event_Mouse_Move *mm = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ev", ev);
	DCHECK_PARAM_PTR("user_data", user_data);
 
	p = EWL_PANED(user_data);
	if (!p->grabbed) 
		DRETURN(DLEVEL_STABLE);

	mm = (Ewl_Event_Mouse_Move *)ev;
	if (ewl_box_orientation_get(EWL_BOX(p)) == EWL_ORIENTATION_HORIZONTAL) {
		int x = mm->x;

		// ewl_object_x_request(EWL_OBJECT(p->grabber), x);
		if (mm->x > CURRENT_X(p) && mm->x < CURRENT_X(p) + CURRENT_W(p)) {
			ewl_object_preferred_inner_w_set(EWL_OBJECT(p->first),
							 x);
		}

	} else {
		int y = mm->y;

		// ewl_object_y_request(EWL_OBJECT(p->grabber), y);
		if (mm->y > CURRENT_Y(p) && mm->y < CURRENT_Y(p) + CURRENT_H(p)) {
			ewl_object_preferred_inner_h_set(EWL_OBJECT(p->first),
							 y);
		}
	}

	ewl_widget_configure(EWL_WIDGET(p));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_focus_in_cb(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
					void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/* FIXME : Fiddle mouse pointer */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_focus_out_cb(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
						void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/* FIXME : Un-fiddle mouse pointer */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



