#include <Ewl.h>

void __ewl_progressbar_configure(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_progressbar_child_add (Ewl_Container *c, Ewl_Widget *w);
void __ewl_progressbar_child_resize(Ewl_Container *c, Ewl_Widget *w, int size, Ewl_Orientation o);
void __ewl_progressbar_child_handle (Ewl_Container *c, Ewl_Widget *w);


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
			__ewl_progressbar_child_add, 
			__ewl_progressbar_child_resize, 
			NULL);

	p->bar = NEW(Ewl_Widget, 1);
	if (!p->bar)
		DRETURN(DLEVEL_STABLE);

	ewl_widget_init(p->bar, "bar");
	ewl_container_append_child(EWL_CONTAINER(p), p->bar);
	ewl_widget_show(p->bar);

	p->label = ewl_text_new(NULL);
	ewl_object_set_alignment(EWL_OBJECT(p->label),
			EWL_FLAG_ALIGN_CENTER);
	ewl_container_append_child(EWL_CONTAINER(p), p->label);
	ewl_widget_show(p->label);

	p->value = 0.0;
	p->range = 100.0;
	p->auto_label = TRUE;
	
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, 
			__ewl_progressbar_configure, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * @param p: the progressbar whose value will be changed
 * @param v: the new value of the statusbar
 * @return Returns no value.
 * @brief Set the value of the progressbars location
 */
void ewl_progressbar_set_value(Ewl_Progressbar * p, double v)
{
	char c[10];
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);

	if (v == p->value)
		DRETURN(DLEVEL_STABLE);

	if (v < 0)
		v = 0;

	p->value = v;

	if (p->auto_label) {
		/* 
		 * Do a precentage calculation as a default label.
		 */
		snprintf (c, sizeof (c), "%.0lf%%", (p->value / p->range) * 100);
		ewl_text_set_text(EWL_TEXT(p->label), c);
	}

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
 * @param p: the progressbar whose range will be changed
 * @param r: the new range of the statusbar
 * @return Returns no value.
 * @brief Set the range of the progressbar. Cannot be less then 1.
 */    
void ewl_progressbar_set_range (Ewl_Progressbar * p, double r)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);

	if (r == p->range)
		DRETURN(DLEVEL_STABLE);

	if (r < 1)
		DRETURN(DLEVEL_STABLE);

	p->range = r;

	ewl_widget_configure(EWL_WIDGET(p));
	ewl_callback_call(EWL_WIDGET(p), EWL_CALLBACK_VALUE_CHANGED);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: the progressbars to retrieve the range
 * @return Returns 0 on failure, the value of the progressbars location on success.
 * @brief Retrieve the current range of the progressbars (default 100)
 */
double ewl_progressbar_get_range (Ewl_Progressbar * p)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, -1);
	
	DRETURN_FLOAT(p->range, DLEVEL_STABLE);
}


/**
 * @param p: the progressbars whose text will be changed
 * @param label: the new label
 * @return Returns no value
 * @brief Sets the given text on the progressbar
 */
void ewl_progressbar_set_label (Ewl_Progressbar * p, char *label)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);

	p->auto_label = FALSE;
	
	if (label)
		ewl_text_set_text(EWL_TEXT(p->label), label);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: the progressbars whose label will be changed
 * @param format_string: format string for the new label
 * @return Returns no value
 * @brief Sets the given format string on the progressbar (%lf of %lf beers)
 */
void ewl_progressbar_set_custom_label (Ewl_Progressbar * p, char *format_string)
{
	char label[PATH_MAX];
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);

	p->auto_label = FALSE;

	if (format_string) {
		snprintf (label, PATH_MAX, format_string, p->value, p->range);
		ewl_text_set_text(EWL_TEXT(p->label), label);
	}
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: the progressbars whose label will be hidden
 * @return Returns no value
 * @brief Hides the given progressbars label
 */
void ewl_progressbar_label_hide (Ewl_Progressbar * p) {
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	
	p->auto_label = FALSE;
	ewl_text_set_text(EWL_TEXT(p->label), "");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: the progressbars whose label will be shown
 * @return Returns no value
 * @brief Shows the given progressbars label
 */
void ewl_progressbar_label_show (Ewl_Progressbar * p) {
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);

	p->auto_label = TRUE;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
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

	ewl_object_request_geometry (EWL_OBJECT(p->bar), dx, dy, 
			dw * (p->value / p->range), dh);

	ewl_object_place (EWL_OBJECT(p->label), dx, dy, dw, dh);
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


void __ewl_progressbar_child_handle (Ewl_Container *c, Ewl_Widget *w)
{
	Ewl_Progressbar *p;
	double value;

	DENTER_FUNCTION(DLEVEL_STABLE);

	p = EWL_PROGRESSBAR(c);

	value = p->value / p->range;

	if (value < 0.01)
		value = 0.01;

	ewl_object_set_preferred_w (EWL_OBJECT(p),
			ewl_object_get_preferred_w (EWL_OBJECT(p->bar)) / value);

	ewl_container_prefer_largest (EWL_CONTAINER (c),
			EWL_ORIENTATION_VERTICAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_progressbar_child_add (Ewl_Container *c, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	__ewl_progressbar_child_handle (c, w);
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_progressbar_child_resize(Ewl_Container *c, Ewl_Widget *w, 
		int size, Ewl_Orientation o)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	__ewl_progressbar_child_handle (c, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


