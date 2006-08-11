#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_progressbar_child_handle(Ewl_Container *c, Ewl_Widget *w);

/**
 * @return Returns NULL on failure, or a pointer to the new progressbar on success.
 * @brief Allocate and initialize a new progressbar
 */
Ewl_Widget *
ewl_progressbar_new(void) 
{
	Ewl_Progressbar *p;

	DENTER_FUNCTION(DLEVEL_STABLE);

	p = NEW(Ewl_Progressbar, 1);
	if (!p)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	if (!ewl_progressbar_init(p)) {
		ewl_widget_destroy(EWL_WIDGET(p));
		p = NULL;
	}
	
	DRETURN_PTR(EWL_WIDGET(p), DLEVEL_STABLE);
}

/**
 * @param p: the progressbar to be initialized
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the progressbar to some sane starting values
 */
int
ewl_progressbar_init(Ewl_Progressbar *p)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("p", p, FALSE);

	w = EWL_WIDGET(p);

	if (!ewl_range_init(EWL_RANGE(w)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(w, EWL_PROGRESSBAR_TYPE);
	ewl_widget_inherit(w, EWL_PROGRESSBAR_TYPE);

	ewl_container_show_notify_set(EWL_CONTAINER(w),
				  ewl_progressbar_child_show_cb);
	ewl_container_resize_notify_set(EWL_CONTAINER(w),
				    ewl_progressbar_child_resize_cb);

	p->bar = NEW(Ewl_Widget, 1);
	if (!p->bar)
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	if (!ewl_widget_init(p->bar))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(p->bar, "progressbar_bar");
	ewl_container_child_append(EWL_CONTAINER(p), p->bar);
	ewl_widget_show(p->bar);

	p->label = ewl_text_new();
	ewl_text_text_set(EWL_TEXT(p->label), NULL);
	ewl_widget_layer_priority_set(p->label, 1);
	ewl_object_alignment_set(EWL_OBJECT(p->label), EWL_FLAG_ALIGN_CENTER);
	ewl_container_child_append(EWL_CONTAINER(p), p->label);
	ewl_widget_show(p->label);

	p->auto_label = TRUE;
	
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, 
			ewl_progressbar_configure_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_VALUE_CHANGED, 
			ewl_progressbar_value_changed_cb, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param p: the progressbars whose text will be changed
 * @param label: the new label
 * @return Returns no value
 * @brief Sets the given text on the progressbar
 */
void
ewl_progressbar_label_set(Ewl_Progressbar *p, char *label)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_PROGRESSBAR_TYPE);

	p->auto_label = FALSE;
	
	if (label)
		ewl_text_text_set(EWL_TEXT(p->label), label);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: the progressbars whose label will be changed
 * @param format_string: format string for the new label
 * @return Returns no value
 * @brief Sets the given format string on the progressbar (%lf of %lf beers)
 */
void
ewl_progressbar_custom_label_set(Ewl_Progressbar *p, char *format_string)
{
	Ewl_Range *r;
	char label[PATH_MAX];
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_PROGRESSBAR_TYPE);

	p->auto_label = FALSE;
	r = EWL_RANGE(p);

	if (format_string) {
		snprintf (label, PATH_MAX, format_string, r->value, r->max_val);
		ewl_text_text_set(EWL_TEXT(p->label), label);
	}
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: the progressbars whose label will be hidden
 * @return Returns no value
 * @brief Hides the given progressbars label
 */
void
ewl_progressbar_label_hide(Ewl_Progressbar *p) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_PROGRESSBAR_TYPE);
	
	p->auto_label = FALSE;
	ewl_text_text_set(EWL_TEXT(p->label), "");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param p: the progressbars whose label will be shown
 * @return Returns no value
 * @brief Shows the given progressbars label
 */
void
ewl_progressbar_label_show (Ewl_Progressbar *p) 
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("p", p);
	DCHECK_TYPE("p", p, EWL_PROGRESSBAR_TYPE);

	p->auto_label = TRUE;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief On a configure event we need to adjust the progressbar to fit 
 * into it's new coords and position as well as move the bar to the correct 
 * size and position.
 */
void
ewl_progressbar_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Progressbar *p;
	Ewl_Range *r;
	int dx, dy;
	int dw;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	p = EWL_PROGRESSBAR(w);
	r = EWL_RANGE(p);

	dx = CURRENT_X(p);
	dy = CURRENT_Y(p);
	dw = CURRENT_W(p);
	
	dw = dw * (r->value - r->min_val) / (r->max_val - r->min_val);
	
	if (r->invert){
		dx += CURRENT_W(p) - dw;
	}
	
	ewl_object_geometry_request(EWL_OBJECT(p->bar), dx, CURRENT_Y(p), 
						dw, CURRENT_H(p));
	ewl_object_place (EWL_OBJECT(p->label), CURRENT_X(p),CURRENT_Y(p), 
						CURRENT_W(p), CURRENT_H(p));
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief the value changed callback
 */
void
ewl_progressbar_value_changed_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Progressbar *p;
	Ewl_Range *r;
	
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	p = EWL_PROGRESSBAR(w);
	r = EWL_RANGE(p);

	if (p->auto_label) {
		char c[10];
		/* 
		 * Do a precentage calculation as a default label.
		 */
		snprintf (c, sizeof (c), "%.0lf%%", (r->value / 
					(r->max_val - r->min_val)) * 100);
		ewl_text_text_set(EWL_TEXT(p->label), c);
	}
	 
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_progressbar_child_handle(Ewl_Container *c,
				Ewl_Widget *w __UNUSED__)
{
	Ewl_Range *r;
	double value;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);

	r = EWL_RANGE(c);
	value = r->value / (r->max_val - r->min_val);

	if (value < 0.01)
		value = 0.01;

	ewl_object_preferred_inner_w_set (EWL_OBJECT(c),
			ewl_object_preferred_w_get(
				EWL_OBJECT(EWL_PROGRESSBAR(c)->bar)) / value);

	ewl_container_largest_prefer(EWL_CONTAINER(c),
			EWL_ORIENTATION_VERTICAL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child show callback
 */
void
ewl_progressbar_child_show_cb(Ewl_Container *c, Ewl_Widget *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_progressbar_child_handle(c, w);
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @param size: UNUSED
 * @param o: UNUSED
 * @return Returns no value
 * @brief The child resize callback
 */
void
ewl_progressbar_child_resize_cb(Ewl_Container *c, Ewl_Widget *w,
				int size __UNUSED__,
				Ewl_Orientation o __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("c", c, EWL_CONTAINER_TYPE);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	ewl_progressbar_child_handle(c, w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


