#include <Ewl.h>

/**
 * @param label: the initial label to display on the border
 * @return Returns a new border container on success, NULL on failure.
 * @brief Allocate and initialize a new border container
 */
Ewl_Widget     *ewl_border_new(char *label)
{
	Ewl_Border     *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = NEW(Ewl_Border, 1);
	if (!b)
		return NULL;

	ewl_border_init(b, label);

	DRETURN_PTR(EWL_WIDGET(b), DLEVEL_STABLE);
}

/**
 * @param b: the border container to initialize
 * @param label: the initial text to display on the border
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize a border container to default values
 */
int ewl_border_init(Ewl_Border * b, char *label)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("b", b, FALSE);

	w = EWL_WIDGET(b);

	if (!ewl_container_init(EWL_CONTAINER(w), "border"))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_container_show_notify(EWL_CONTAINER(w), ewl_border_child_size_cb);
	ewl_container_resize_notify(EWL_CONTAINER(w), 
				    (Ewl_Child_Resize)ewl_border_child_size_cb);

	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FLAG_FILL_FILL);

	b->label = ewl_text_new(label);
	ewl_container_append_child(EWL_CONTAINER(b), b->label);
	ewl_widget_show(b->label);

	b->box = ewl_vbox_new();
	ewl_container_append_child(EWL_CONTAINER(b), b->box);
	ewl_widget_show(b->box);

	ewl_container_set_redirect(EWL_CONTAINER(b), EWL_CONTAINER(b->box));

	/*
	 * Attach necessary callback mechanisms 
	 */
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_border_configure_cb, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param b: the border widget to change the text
 * @param t: the text to set for the border label
 * @return Returns no value.
 * @brief Set the text for an border label
 *
 * Change the text of the border label to the string @a t.
 */
void ewl_border_set_text(Ewl_Border * b, char *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("b", b);

	ewl_text_text_set(EWL_TEXT(b->label), t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the border to retrieve the label text
 * @return Returns the border label text on success, NULL on failure.
 * @brief Get the text label from a border widget
 */
char           *ewl_border_get_text(Ewl_Border * b)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("b", b, NULL);

	w = EWL_WIDGET(b);

	DRETURN_PTR(ewl_text_text_get(EWL_TEXT(b->label)), DLEVEL_STABLE);
}

/*
 * Layout the text and box within the border widget.
 */
void ewl_border_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Border     *b;
	int             yy, hh;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = EWL_BORDER(w);
	ewl_object_place(EWL_OBJECT(b->label), CURRENT_X(w),
				    CURRENT_Y(w), CURRENT_W(b), CURRENT_H(b));
	hh = ewl_object_get_current_h(EWL_OBJECT(w));
	yy = CURRENT_Y(w) + hh;
	hh = CURRENT_H(w) - hh;

	ewl_object_place(EWL_OBJECT(b->box), CURRENT_X(w), yy, CURRENT_W(w),
			 hh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_border_child_size_cb(Ewl_Container * c, Ewl_Widget * w)
{
	Ewl_Border *b;

	DENTER_FUNCTION(DLEVEL_STABLE);

	b = EWL_BORDER(c);
	ewl_container_prefer_largest(c, EWL_ORIENTATION_HORIZONTAL);
	ewl_container_prefer_sum(c, EWL_ORIENTATION_VERTICAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
