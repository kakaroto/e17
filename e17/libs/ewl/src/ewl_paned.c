#include <Ewl.h>

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
 * @param orient: the desired orientation of the paned widget
 * @return Returns NULL on failure, or a pointer to a new paned widget on success.
 * @brief Allocate and initialize a new paned widget
 */
Ewl_Widget *
ewl_paned_new(Ewl_Orientation orient) 
{
	Ewl_Paned *pane = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	pane = NEW(Ewl_Paned, 1);
	if (!pane)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_paned_init(pane, orient)) {
		ewl_widget_destroy(EWL_WIDGET(pane));
		pane = NULL;
    	}

	DRETURN_PTR(pane, DLEVEL_STABLE);
}

/**
 * @param p: the paned widget to initialize
 * @param orient: the orientation of the paned widget
 * @return Returns 1 on success or 0 on failure
 * @brief Initialize a new paned widget to default values
 */
int
ewl_paned_init(Ewl_Paned *p, Ewl_Orientation orient)
{
	Ewl_Widget *w = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, FALSE);

	w = EWL_WIDGET(p);

	if (!ewl_box_init(EWL_BOX(p), orient)) {
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}
	p->orientation = orient;
	p->grabbed = FALSE;

	p->first = EWL_BOX(ewl_vbox_new());
	ewl_container_child_append(EWL_CONTAINER(p), EWL_WIDGET(p->first));
	ewl_widget_show(EWL_WIDGET(p->first));

	if (orient == EWL_ORIENTATION_HORIZONTAL)
		p->grabber = EWL_BOX(ewl_hbox_new());
	else
		p->grabber = EWL_BOX(ewl_vbox_new());

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
	ewl_widget_show(EWL_WIDGET(p->grabber));

	p->second = EWL_BOX(ewl_vbox_new());
	ewl_container_child_append(EWL_CONTAINER(p), EWL_WIDGET(p->second));
	ewl_widget_show(EWL_WIDGET(p->second));
    
	ewl_widget_internal_set(EWL_WIDGET(p->first), TRUE);
	ewl_widget_internal_set(EWL_WIDGET(p->second), TRUE);
	ewl_widget_internal_set(EWL_WIDGET(p->grabber), TRUE);

	p->active = EWL_POSITION_LEFT;
	ewl_container_redirect_set(EWL_CONTAINER(p), EWL_CONTAINER(p->first));

	ewl_widget_appearance_set(w, "paned");
	ewl_widget_appearance_set(EWL_WIDGET(p->first), "first");
	ewl_widget_appearance_set(EWL_WIDGET(p->second), "second");

	if (orient == EWL_ORIENTATION_HORIZONTAL)
		ewl_widget_appearance_set(EWL_WIDGET(p->grabber),
					"grabber_horizontal");
	else
		ewl_widget_appearance_set(EWL_WIDGET(p->grabber),
					"grabber_vertical");

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param p: the paned wiget to work on
 * @param pos: The Ewl_Position to set to the active partition
 * @returns Returns no value.
 * @brief Changes the current active pane of the paned widget
 */
void
ewl_paned_active_set(Ewl_Paned *p, Ewl_Position pos) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);

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
ewl_paned_active_get(Ewl_Paned *p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, FALSE);

	DRETURN_INT(p->active, DLEVEL_STABLE);
}

/**
 * @param p: the paned wiget to work against
 * @param w: the wiget to append
 * @return Returns no value.
 * @brief Appends the wiget @a w to the first pane in @a p.
 */
void
ewl_paned_first_pane_append(Ewl_Paned *p, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_PARAM_PTR("w", w);

	ewl_container_child_append(EWL_CONTAINER(p->first), w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: the paned wiget to work against
 * @param w: the wiget to append
 * @return Returns no value.
 * @brief Prepends the wiget @a w to the first pane in @a p.
 */
void
ewl_paned_first_pane_prepend(Ewl_Paned *p, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_PARAM_PTR("w", w);

	ewl_container_child_prepend(EWL_CONTAINER(p->first), w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: the paned wiget to work against
 * @param w: the wiget to append
 * @return Returns no value.
 * @brief Appends the wiget @a w to the second pane in @a p.
 */
void
ewl_paned_second_pane_append(Ewl_Paned *p, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_PARAM_PTR("w", w);

	ewl_container_child_append(EWL_CONTAINER(p->second), w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: the paned wiget to work against
 * @param w: the wiget to append
 * @return Returns no value.
 * @brief Prepends the wiget @a w to the second pane in @a p.
 */
void
ewl_paned_second_pane_prepend(Ewl_Paned *p, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_PARAM_PTR("w", w);

	ewl_container_child_prepend(EWL_CONTAINER(p->second), w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_mouse_down_cb(Ewl_Widget *w, void *ev, void *user_data)
{
	Ewl_Paned *p = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	p = EWL_PANED(user_data);
	p->grabbed = TRUE;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_mouse_up_cb(Ewl_Widget *w, void *ev, void *user_data)
{
	Ewl_Paned *p = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	p = EWL_PANED(user_data);
	p->grabbed = FALSE;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_mouse_move_cb(Ewl_Widget *w, void *ev, void *user_data)
{
	Ewl_Paned *p = NULL;
	Ewl_Event_Mouse_Move *mm = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev", ev);
	DCHECK_PARAM_PTR("user_data", user_data);
 
	p = EWL_PANED(user_data);
	if (!p->grabbed) 
		return;

	mm = (Ewl_Event_Mouse_Move *)ev;
	if (p->orientation == EWL_ORIENTATION_HORIZONTAL) {
		int x = mm->x;

		if (x < EWL_OBJECT_MIN_SIZE)
			x = EWL_OBJECT_MIN_SIZE;
		else if (x > EWL_OBJECT_MAX_SIZE)
			x = EWL_OBJECT_MAX_SIZE;

		ewl_object_x_request(EWL_OBJECT(p->grabber), x);
		ewl_object_w_request(EWL_OBJECT(p->first), x);

	} else {
		int y = mm->y;

		if (y < EWL_OBJECT_MIN_SIZE)
			y = EWL_OBJECT_MIN_SIZE;
		else if (y > EWL_OBJECT_MAX_SIZE)
			y = EWL_OBJECT_MAX_SIZE;

		ewl_object_y_request(EWL_OBJECT(p->grabber), y);
		ewl_object_h_request(EWL_OBJECT(p->first), y);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_focus_in_cb(Ewl_Widget *w, void *ev, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/* FIXME : Fiddle mouse pointer */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_paned_grabber_focus_out_cb(Ewl_Widget *w, void *ev, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	/* FIXME : Un-fiddle mouse pointer */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



