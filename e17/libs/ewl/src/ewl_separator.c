#include <Ewl.h>

static void __ewl_separator_init(Ewl_Separator * s, Ewl_Orientation o);
static void __ewl_separator_realize(Ewl_Widget * w, void *ev_data,
				    void *user_data);

Ewl_Widget *
ewl_separator_new(Ewl_Orientation o)
{
	Ewl_Separator *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Separator, 1);
	memset(s, 0, sizeof(Ewl_Separator));

	__ewl_separator_init(s, o);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

static void
__ewl_separator_init(Ewl_Separator * s, Ewl_Orientation o)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = EWL_WIDGET(s);

	if (o == EWL_ORIENTATION_HORIZONTAL)
		ewl_widget_init(w, "/appearance/separator/horizontal");
	else
		ewl_widget_init(w, "/appearance/separator/vertical");

	ewl_object_set_alignment(EWL_OBJECT(s), EWL_ALIGNMENT_LEFT);
	ewl_object_set_fill_policy(EWL_OBJECT(s), EWL_FILL_POLICY_FILL);

	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_separator_realize, NULL);

	s->orientation = o;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_separator_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (w->ebits_object) {
		int ww, hh;

		ebits_get_max_size(w->ebits_object, &ww, &hh);

		ewl_object_set_maximum_size(EWL_OBJECT(w), ww, hh);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
