#include <Ewl.h>

static void __ewl_separator_init(Ewl_Separator * s, Ewl_Orientation o);
static void __ewl_separator_configure(Ewl_Widget * w, void *ev_data,
				      void *user_data);

Ewl_Widget *
ewl_separator_new(Ewl_Orientation o)
{
	Ewl_Separator *s;

	DENTER_FUNCTION;

	s = NEW(Ewl_Separator, 1);
	memset(s, 0, sizeof(Ewl_Separator));

	__ewl_separator_init(s, o);

	DRETURN_PTR(EWL_WIDGET(s));
}

void
ewl_separator_set_padding(Ewl_Widget * w, int l, int r, int t, int b)
{
	Ewl_Separator *s;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SEPARATOR(w);

	s->padd.l = l;
	s->padd.r = r;
	s->padd.t = t;
	s->padd.b = b;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_separator_init(Ewl_Separator * s, Ewl_Orientation o)
{
	Ewl_Widget *w;

	DENTER_FUNCTION;

	w = EWL_WIDGET(s);

	if (o == EWL_ORIENTATION_HORIZONTAL)
		ewl_widget_init(EWL_CONTAINER(s),
				"appearance/separator/horizontal");
	else
		ewl_widget_init(EWL_CONTAINER(s),
				"appearance/separator/vertical");

	ewl_object_set_minimum_size(EWL_OBJECT(s), 10, 5);
	ewl_object_set_alignment(EWL_OBJECT(s), EWL_ALIGNMENT_CENTER);
	ewl_object_set_fill_policy(EWL_OBJECT(s), EWL_FILL_POLICY_FILL);

	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_separator_theme_update, NULL);

	s->orientation = o;

	DLEAVE_FUNCTION;
}

static void
__ewl_separator_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DCHECK_PARAM_PTR("w", w);
}
