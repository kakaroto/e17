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

	if (w->ebits_object)
	  {
		  int ww, hh;

		  ebits_get_max_size(w->ebits_object, &ww, &hh);

		  if (s->orientation == EWL_ORIENTATION_HORIZONTAL)
		    {
			    MAXIMUM_W(w) = ww + l + r;
			    MINIMUM_W(w) = MAXIMUM_W(w);
		    }
		  else
		    {
			    MAXIMUM_H(w) = hh + t + b;
			    MINIMUM_H(w) = MAXIMUM_H(w);
		    }
	  }

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
		ewl_widget_init(w, "/appearance/separator/horizontal");
	else
		ewl_widget_init(w, "/appearance/separator/vertical");

	ewl_object_set_alignment(EWL_OBJECT(s), EWL_ALIGNMENT_LEFT);
	ewl_object_set_fill_policy(EWL_OBJECT(s), EWL_FILL_POLICY_FILL);

	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_separator_configure, NULL);
	s->orientation = o;

	DLEAVE_FUNCTION;
}

static void
__ewl_separator_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Separator *s;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SEPARATOR(w);

	if (w->ebits_object)
	  {
		  int ww, hh;

		  if (s->orientation == EWL_ORIENTATION_HORIZONTAL)
		    {
			    ww = MAXIMUM_W(w) - s->padd.l - s->padd.r;
			    hh = REQUEST_H(w) - s->padd.t - s->padd.b;
		    }
		  else
		    {
			    ww = REQUEST_W(w) - s->padd.l - s->padd.r;
			    hh = MAXIMUM_H(w) - s->padd.t - s->padd.b;
		    }

		  ebits_move(w->ebits_object, REQUEST_X(w) + s->padd.l,
			     REQUEST_Y(w) + s->padd.t);
		  ebits_resize(w->ebits_object, ww, hh);
	  }

	DLEAVE_FUNCTION;
}
