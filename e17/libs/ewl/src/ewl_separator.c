#include <Ewl.h>

static void __ewl_separator_init(Ewl_Separator * s);
static void __ewl_separator_realize(Ewl_Widget * w, void * ev_data, void * user_data);
static void __ewl_separator_show(Ewl_Widget * w, void * ev_data, void * user_data);
static void __ewl_separator_hide(Ewl_Widget * w, void * ev_data, void * user_data);
static void __ewl_separator_destroy(Ewl_Widget * w, void * ev_data, void * user_data);
static void __ewl_separator_configure(Ewl_Widget * w, void * ev_data, void * user_data);
static void __ewl_separator_theme_update(Ewl_Widget * w, void * ev_data, void * user_data);

Ewl_Widget *
ewl_separator_new(Ewl_Orientation o)
{
	Ewl_Separator * s;

	DENTER_FUNCTION;

	s = NEW(Ewl_Separator, 1);

	__ewl_separator_init(s);

	s->orientation = o;

	DRETURN_PTR(EWL_WIDGET(s));
}

void
ewl_separator_set_padding(Ewl_Widget * w, int u, int l)
{
	Ewl_Separator * s;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SEPARATOR(w);

	s->u_padding = u;
	s->l_padding = l;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

void
ewl_separator_set_fill_percentage(Ewl_Widget * w, double p)
{
	Ewl_Separator * s;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SEPARATOR(w);

	s->fill_percentage = p;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_separator_init(Ewl_Separator * s)
{
	Ewl_Widget * w;

	DENTER_FUNCTION;

	w = EWL_WIDGET(s);

	memset(s, 0, sizeof(Ewl_Separator));
	ewl_container_init(EWL_CONTAINER(s), 10, 5,
		EWL_FILL_POLICY_FILL, EWL_ALIGNMENT_CENTER);

	w->recursive = FALSE;

	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			__ewl_separator_realize, NULL);
        ewl_callback_append(w, EWL_CALLBACK_SHOW,
                        __ewl_separator_show, NULL);
        ewl_callback_append(w, EWL_CALLBACK_HIDE,
                        __ewl_separator_hide, NULL);
        ewl_callback_append(w, EWL_CALLBACK_DESTROY,
                        __ewl_separator_destroy, NULL);
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
                        __ewl_separator_configure, NULL);
        ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
                        __ewl_separator_theme_update, NULL);

	s->fill_percentage = 80;
	s->u_padding = 2;
	s->l_padding = 2;

	DLEAVE_FUNCTION;
}

static void
__ewl_separator_realize(Ewl_Widget * w, void * ev_data, void * user_data)
{
	Ewl_Separator * s;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SEPARATOR(w);

	w->fx_clip_box = evas_add_rectangle(w->evas);
	evas_set_color(w->evas, w->fx_clip_box, 255, 255, 255, 255);
	evas_set_layer(w->evas, w->fx_clip_box, LAYER(w) - 1);
	if (w->parent && EWL_CONTAINER(w->parent)->clip_box)
		evas_set_clip(w->evas, w->fx_clip_box,
				EWL_CONTAINER(w->parent)->clip_box);

	ewl_widget_theme_update(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_separator_show(Ewl_Widget * w, void * ev_data, void * user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	evas_show(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_separator_hide(Ewl_Widget * w, void * ev_data, void * user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	evas_hide(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_separator_destroy(Ewl_Widget * w, void * ev_data, void * user_data)
{
	Ewl_Separator * s;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SEPARATOR(w);

	if (w->ebits_object)
	  {
		ebits_hide(w->ebits_object);
		ebits_unset_clip(w->ebits_object);
		ebits_free(w->ebits_object);
	  }

	if (w->fx_clip_box)
	  {
		evas_hide(w->evas, w->fx_clip_box);
		evas_unset_clip(w->evas, w->fx_clip_box);
		evas_del_object(w->evas, w->fx_clip_box);
	  }

	ewl_theme_deinit_widget(w);

	ewl_callback_clear(w);

	FREE(s);

	DLEAVE_FUNCTION;
}

static void
__ewl_separator_configure(Ewl_Widget * w, void * ev_data, void * user_data)
{
	Ewl_Separator * s;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SEPARATOR(w);

	ewl_object_apply_requested(w);

	if (w->fx_clip_box)
	  {
		evas_move(w->evas, w->fx_clip_box, CURRENT_X(w), CURRENT_Y(w));
		evas_resize(w->evas, w->fx_clip_box, CURRENT_W(w),CURRENT_H(w));
	  }

	if (w->ebits_object)
	  {
		int req_w, req_h;
		double p;

		req_w = CURRENT_W(w);
		req_h = CURRENT_H(w);

		p = s->fill_percentage / 100;

		ebits_move(w->ebits_object, CURRENT_X(w), CURRENT_Y(w));

		if (s->orientation == EWL_ORIENTATION_HORIZONTAL)
			req_h *= p;
		else
			req_w *= p;
		
		ebits_resize(w->ebits_object, req_w, req_h);
	  }

	DLEAVE_FUNCTION;
}

static void
__ewl_separator_theme_update(Ewl_Widget * w, void * ev_data, void * user_data)
{
	Ewl_Separator * s;
	char * v;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	s = EWL_SEPARATOR(w);

	if (s->orientation == EWL_ORIENTATION_HORIZONTAL)
		v = ewl_theme_data_get(w, "/appearance/separator/horizontal/base/visible");
	else
		v = ewl_theme_data_get(w, "/appearance/separator/vertical/base/visible");

	if (w->ebits_object)
	  {
		ebits_hide(w->ebits_object);
		ebits_unset_clip(w->ebits_object);
		ebits_free(w->ebits_object);
	  }

	if (v && !strncasecmp(v, "yes", 3))
	  {
		char * i;

		if (s->orientation == EWL_ORIENTATION_HORIZONTAL)
			i = ewl_theme_image_get(w,"/appearance/separator/horizontal/base");
		else
			i = ewl_theme_image_get(w,"/appearance/separator/vertical/base");

		if (i)
		  {
			w->ebits_object = ebits_load(i);

			FREE(i);

			if (w->ebits_object)
			  {
				ebits_add_to_evas(w->ebits_object, w->evas);
				ebits_set_layer(w->ebits_object, LAYER(w));
				ebits_set_clip(w->ebits_object, w->fx_clip_box);
				ebits_show(w->ebits_object);
			  }

		  }
	  }

	DLEAVE_FUNCTION;
}
