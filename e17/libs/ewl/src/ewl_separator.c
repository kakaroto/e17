#include <Ewl.h>

static void __ewl_separator_init (Ewl_Separator * s, Ewl_Orientation o);
static void __ewl_separator_configure (Ewl_Widget * w, void *ev_data,
				       void *user_data);

Ewl_Widget *
ewl_separator_new (Ewl_Orientation o)
{
	Ewl_Separator *s;

	DENTER_FUNCTION;

	s = NEW (Ewl_Separator, 1);
	memset (s, 0, sizeof (Ewl_Separator));

	__ewl_separator_init (s, o);

	DRETURN_PTR (EWL_WIDGET (s));
}

void
ewl_separator_set_padding (Ewl_Widget * w, int u, int l)
{
	Ewl_Separator *s;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	s = EWL_SEPARATOR (w);

	s->u_padding = u;
	s->l_padding = l;

	ewl_widget_configure (w);

	DLEAVE_FUNCTION;
}

void
ewl_separator_set_fill_percentage (Ewl_Widget * w, double p)
{
	Ewl_Separator *s;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR ("w", w);

	s = EWL_SEPARATOR (w);

	s->fill_percentage = p;

	ewl_widget_configure (w);

	DLEAVE_FUNCTION;
}

static void
__ewl_separator_init (Ewl_Separator * s, Ewl_Orientation o)
{
	Ewl_Widget *w;

	DENTER_FUNCTION;

	w = EWL_WIDGET (s);

	if (o == EWL_ORIENTATION_HORIZONTAL)
		ewl_widget_init (EWL_CONTAINER (s),
				 "appearance/separator/horizontal");
	else
		ewl_widget_init (EWL_CONTAINER (s),
				 "appearance/separator/vertical");

	ewl_object_set_minimum_size (EWL_OBJECT (s), 10, 5);
	ewl_object_set_alignment (EWL_OBJECT (s), EWL_ALIGNMENT_CENTER);
	ewl_object_set_fill_policy (EWL_OBJECT (s), EWL_FILL_POLICY_FILL);

	ewl_callback_append (w, EWL_CALLBACK_THEME_UPDATE,
			     __ewl_separator_theme_update, NULL);

	s->orientation = o;
	s->fill_percentage = 80;
	s->u_padding = 2;
	s->l_padding = 2;

	DLEAVE_FUNCTION;
}

static void
__ewl_separator_configure (Ewl_Widget * w, void *ev_data, void *user_data)
{
	DCHECK_PARAM_PTR ("w", w);
}
