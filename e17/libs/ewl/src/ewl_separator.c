#include <Ewl.h>

void            __ewl_separator_realize(Ewl_Widget * w, void *ev_data,
					void *user_data);

/**
 * ewl_separator_new - allocate a new separator widget
 * @o: the orientation of the newly allocated separator widget
 *
 * Returns a pointer to the newly alloacted separator widget on success, NULL
 * on failure.
 */
Ewl_Widget     *ewl_separator_new(Ewl_Orientation o)
{
	Ewl_Separator  *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Separator, 1);
	ZERO(s, Ewl_Separator, 1);

	ewl_separator_init(s, o);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

/**
 * ewl_separator_init - initialize the separator and inherited fields
 * @s: the separator to initialize
 * @o: the orientation of the separator to be initialized
 *
 * Returns no value. Clears the contents of the separator and stores the
 * default values along with the orientation specified by @o.
 */
void ewl_separator_init(Ewl_Separator * s, Ewl_Orientation o)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = EWL_WIDGET(s);

	ewl_widget_init(w, "separator");

	ewl_object_set_alignment(EWL_OBJECT(s), EWL_ALIGNMENT_LEFT);
	ewl_object_set_fill_policy(EWL_OBJECT(s), EWL_FILL_POLICY_FILL);

	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_separator_realize, NULL);

	s->orientation = o;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Setup the ewl object's maximum size based on the max size of the ebits
 * object.
 */
void __ewl_separator_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->ebits_object) {
		int             ww, hh;

		ebits_get_max_size(w->ebits_object, &ww, &hh);

		/* ewl_object_set_preferred_size(EWL_OBJECT(w), ww, hh); */
		ewl_object_set_maximum_size(EWL_OBJECT(w), ww, hh);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
