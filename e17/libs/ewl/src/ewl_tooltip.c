#include <Ewl.h>


/**
 * @parent: the widget this tooltip relates
 * @return Returns a new tooltip widget on success, NULL on failure.
 * @brief Creates a new tooltip widget for a specific parent.
 */
Ewl_Widget *ewl_tooltip_new (Ewl_Widget *parent)
{
	Ewl_Tooltip *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = NEW(Ewl_Tooltip, 1);
	if (!t)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_tooltip_init (t, parent)) {
		FREE(t);
	}

	DRETURN_PTR(EWL_WIDGET(t), DLEVEL_STABLE);
}

/**
 * @t: the tooltip widget to initialize to default values
 * @parent: the parent widget the tooltip is associated with
 * @return Returns TRUE on success, FALSE otherwise.
 * @brief Initialize the default values of the tooltip fields.
 */
int ewl_tooltip_init (Ewl_Tooltip *t, Ewl_Widget *parent)
{
	Ewl_Widget *w;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, FALSE);

	w = EWL_WIDGET (t);

	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_VERTICAL);
	ewl_widget_set_appearance (EWL_WIDGET (w), "tooltip");
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FLAG_FILL_NONE);
	ewl_widget_set_layer(w, 2000);

	t->text = ewl_text_new ("test tooltip");
	ewl_object_set_alignment (EWL_OBJECT(t->text),
			EWL_FLAG_ALIGN_TOP | EWL_FLAG_ALIGN_LEFT);
	ewl_container_append_child (EWL_CONTAINER(w), t->text);

	ewl_widget_show (t->text);

	t->delay = 1.5;
	t->hide = FALSE;

	if (parent) {
		ewl_callback_append (parent, EWL_CALLBACK_MOUSE_MOVE, 
				ewl_tooltip_parent_mouse_move_cb, t);
		ewl_callback_append (parent, EWL_CALLBACK_FOCUS_OUT,
				ewl_tooltip_parent_focus_out_cb, t);

		/* 
		 * If the parent is clicked we don't want to display 
		 * the tooltip.
		 */
		ewl_callback_append (parent, EWL_CALLBACK_MOUSE_DOWN,
				ewl_tooltip_parent_mouse_down_cb, t);
	}
	
	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param t: the tooltip to change displayed text
 * @param text: the new text to display in the tooltip
 * @return Returns no value.
 * @brief Change the text displayed in the tooltip.
 */
void ewl_tooltip_set_text (Ewl_Tooltip *t, char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	ewl_text_set_text (EWL_TEXT (t), text);
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: the tooltip to change the display delay
 * @param delay: the delay in fractions of seconds before displaying tooltip
 * @return Returns no value.
 * @brief Changes the delay value on the tooltip.
 */
void ewl_tooltip_set_delay (Ewl_Tooltip *t, double delay)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	if (delay > 0) 
		t->delay = delay;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

int ewl_tooltip_focus_timer (void *data)
{
	Ewl_Tooltip *t = data;

	if (t->hide)
		return FALSE;

	ewl_object_request_position (EWL_OBJECT(t), t->x, t->y);
	ewl_widget_show (EWL_WIDGET (t));

	printf ("Opening tooltip after %lf secs\n", t->delay);

	t->timer = NULL;

	return FALSE;
}

void
ewl_tooltip_parent_mouse_down_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Tooltip *t = user_data;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (t->hide)
		return;

	if (t->timer) {
		ecore_timer_del (t->timer);
		t->timer = NULL;
	}

	printf ("Mouse clicked the parent, don't display tooltip\n");
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tooltip_parent_mouse_move_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Tooltip *t = user_data;
	Ewl_Event_Mouse_Move *e = ev_data;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (t->hide)
		return;
	
	t->x = e->x;
	t->y = e->y;

	printf ("X: %d Y: %d\n", e->x, e->y);
	

	if (t->timer) {
		ecore_timer_del (t->timer);
		t->timer = NULL;
	}

	t->timer = ecore_timer_add(t->delay, ewl_tooltip_focus_timer, t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_tooltip_parent_focus_out_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Tooltip *t = user_data;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (t->hide)
		return;

	if (t->timer) {
		ecore_timer_del (t->timer);
		t->timer = NULL;
	}

	ewl_widget_hide (EWL_WIDGET (t));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
