#include <Ewl.h>

void __ewl_progressbar_configure(Ewl_Widget * w, void *ev_data, void *user_data);


/**
 * @return Returns NULL on failure, or a pointer to the new progressbar on success.
 * @brief Allocate and initialize a new progressbar
 */
Ewl_Widget *ewl_progressbar_new() 
{
	Ewl_Progressbar *p;

	DENTER_FUNCTION(DLEVEL_STABLE);

	p = NEW(Ewl_Progressbar, 1);
	if (!p)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ewl_progressbar_init(p);
	
	DRETURN_PTR(EWL_WIDGET(p), DLEVEL_STABLE);
}

/**
 * @param p: the progressbar to be initialized
 * @return Returns no value.
 * @brief Initialize the progressbar to some sane starting values
 */
void ewl_progressbar_init(Ewl_Progressbar * p)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);

	w = EWL_WIDGET(p);

	ewl_container_init(EWL_CONTAINER(w), "progressbar",
			NULL, NULL, NULL);

	p->bar = NEW(Ewl_Widget, 1);
	if (!p->bar)
		DRETURN(DLEVEL_STABLE);

	ewl_widget_init(p->bar, "bar");
	ewl_container_append_child(EWL_CONTAINER(p), p->bar);
	ewl_widget_show(p->bar);

	p->text = ewl_text_new("test");
	ewl_object_set_alignment(EWL_OBJECT(p->text),
			EWL_FLAG_ALIGN_CENTER);
	ewl_container_append_child(EWL_CONTAINER(p), p->text);
	ewl_widget_show(p->text);

	p->value = 0.0;
	
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, 
			__ewl_progressbar_configure, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param p: the progressbar whose value will be changed
 * @param v: the new value the statusbar
 * @return Returns no value.
 * @brief Set the value of the progressbars location
 */
void ewl_progressbar_set_value(Ewl_Progressbar * p, double v)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);

	if (v == p->value)
		DRETURN(DLEVEL_STABLE);

	if (v < 0)
		v = 0;

	p->value = v;

	ewl_widget_configure(EWL_WIDGET(p));
	ewl_callback_call(EWL_WIDGET(p), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param p: the progressbars to retrieve the value
 * @return Returns 0 on failure, the value of the progressbars location on success.
 * @brief Retrieve the current value of the progressbars
 */ 
double ewl_progressbar_get_value(Ewl_Progressbar * p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, -1);

	DRETURN_FLOAT(p->value, DLEVEL_STABLE);
}


/**
 * @param p: the progressbars whose text will be changed
 * @param text: the new text
 * @return Returns no value
 * @brief Sets the given text on the progressbar
 */
void ewl_progressbar_set_text (Ewl_Progressbar * p, char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);

	if (text)
		ewl_text_set_text(EWL_TEXT(p->text), text);

	DLEAVE_FUNCTION(DLEVEL_STABLE)
}


/*
 * On a configure event we need to adjust the progressbar to fit into it's new
 * coords and position as well as move the bar to the correct size and
 * position.
 */
void __ewl_progressbar_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Progressbar *p;
	int             dx, dy;
	int             dw, dh;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	p = EWL_PROGRESSBAR(w);

	dx = CURRENT_X(p);
	dy = CURRENT_Y(p);
	dw = CURRENT_W(p);
	dh = CURRENT_H(p);

	ewl_object_request_geometry(EWL_OBJECT(p->bar), dx, dy, dw * p->value, dh);
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

