
#include <Ewl.h>
#include <Etox.h>

static void __ewl_text_init(Ewl_Widget * widget);
static void __ewl_text_realize(Ewl_Widget * w, void *ev_data,
			       void *user_data);
static void __ewl_text_show(Ewl_Widget * w, void *ev_data,
			    void *user_data);
static void ewl_text_hide(Ewl_Widget * w, void *ev_data,
			  void *user_data);
static void ewl_text_destroy(Ewl_Widget * w, void *ev_data,
			     void *user_data);
static void ewl_text_configure(Ewl_Widget * w, void *ev_data,
			       void *user_data);
#define START_W 2048
#define START_H 2048

Ewl_Widget *
ewl_text_new()
{
	Ewl_Text *text = NULL;

	DENTER_FUNCTION;

	text = NEW(Ewl_Text, 1);

	__ewl_text_init(EWL_WIDGET(text));

	DRETURN_PTR(EWL_WIDGET(text));
}

static void
__ewl_text_init(Ewl_Widget * widget)
{
	Ewl_Text *text = EWL_TEXT(widget);

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("widget", widget);

	/*
	 * Clear and initialize the text structure
	 */
	memset(text, 0, sizeof(Ewl_Text));
	ewl_widget_init(widget, START_W, START_H, EWL_FILL_POLICY_NORMAL,
			EWL_ALIGNMENT_LEFT);

	text->font = strdup("borzoib");
	text->font_size = 15;
	text->text = strdup("");
	text->color = etox_color_new();

	/*
	 * Set up appropriate callbacks for specific events
	 */
	ewl_callback_append(widget, EWL_CALLBACK_REALIZE,
			    __ewl_text_realize, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_SHOW, __ewl_text_show, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_HIDE, ewl_text_hide, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_DESTROY, ewl_text_destroy,
			NULL);
	ewl_callback_append(widget, EWL_CALLBACK_CONFIGURE, ewl_text_configure,
			NULL);

	DLEAVE_FUNCTION;
}

static void
__ewl_text_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Etox_Align_Type h_align, v_align;
	Ewl_Text * t;
	char *name;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	w->fx_clip_box = evas_add_rectangle(w->evas);
	evas_set_color(w->evas, w->fx_clip_box, 255, 255, 255, 255);
	evas_set_layer(w->evas, w->fx_clip_box, LAYER(w) - 1);
	if (w->parent && EWL_CONTAINER(w->parent)->clip_box)
		evas_set_clip(w->evas, w->fx_clip_box,
				EWL_CONTAINER(w->parent)->clip_box);

	t->style = etox_style_new("Default");

	/*
	 * Determine the proper vertical alignment
	 */
	if (t->align & EWL_ALIGNMENT_BOTTOM)
		v_align = ETOX_ALIGN_TYPE_BOTTOM;
	else if (t->align & EWL_ALIGNMENT_TOP)
		v_align = ETOX_ALIGN_TYPE_TOP;
	else
		v_align = ETOX_ALIGN_TYPE_CENTER;

	/*
	 * Determine the proper horizontal alignment
	 */
	if (t->align & EWL_ALIGNMENT_RIGHT)
		h_align = ETOX_ALIGN_TYPE_RIGHT;
	else if (t->align & EWL_ALIGNMENT_LEFT)
		h_align = ETOX_ALIGN_TYPE_LEFT;
	else
		h_align = ETOX_ALIGN_TYPE_CENTER;

	/*
	 * Create the evas objects needed to draw the text
	 */
	
	name = NEW(char, 11);
	snprintf(name, 11, "%p", w);

	t->tox = etox_new(w->evas, name);
	FREE(name);

	/*
	 * Set up necessary etox properties
	 */
	etox_set_align(t->tox, h_align, v_align);
	etox_set_style(t->tox, t->style);
	etox_set_layer(t->tox, LAYER(w));
	etox_resize(t->tox, START_W, START_H);
	etox_move(t->tox, CURRENT_X(w), CURRENT_Y(w));
	etox_set_padding(t->tox, t->padding);
	etox_set_font(t->tox, t->font, t->font_size);
	etox_set_text(t->tox, ET_TEXT(t->text), ET_END);
	{
		double xx, yy, ww, hh;

		etox_get_actual_geometry(t->tox, &xx, &yy, &ww, &hh);

		etox_resize(t->tox, ww, hh);
	}
	etox_set_alpha(t->tox, 255);
	etox_set_clip(t->tox, w->fx_clip_box);
	etox_show(t->tox);

	DLEAVE_FUNCTION;
}

static void
__ewl_text_show(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	evas_show(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
ewl_text_hide(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	evas_hide(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
ewl_text_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Text * t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	if (w->fx_clip_box)
	  {
		evas_hide(w->evas, w->fx_clip_box);
		evas_unset_clip(w->evas, w->fx_clip_box);
		evas_del_object(w->evas, w->fx_clip_box);
	  }

	if (t->tox)
	  {
		etox_hide(t->tox);
		etox_unset_clip(t->tox);
		etox_free(t->tox);
	  }

	IF_FREE(t->text);

	ewl_theme_deinit_widget(w);

	ewl_callback_clear(w);

	FREE(w);

	DLEAVE_FUNCTION;
}

static void
ewl_text_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Text * t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	ewl_object_apply_requested(EWL_OBJECT(w));

	if (w->fx_clip_box) {
		evas_move(w->evas, w->fx_clip_box,
				CURRENT_X(w), CURRENT_Y(w));
		evas_resize(w->evas, w->fx_clip_box,
				CURRENT_W(w), CURRENT_H(w));
	}

	if (t->tox)
	  {
		double xx, yy, ww, hh;

		etox_move(t->tox, CURRENT_X(t), CURRENT_Y(t));
		etox_resize(t->tox, START_W, START_H);
		etox_get_actual_geometry(t->tox, &xx, &yy, &ww, &hh);
		etox_resize(t->tox, ww, hh);
		ewl_object_set_custom_size(t, ww, hh);
	  }

	DLEAVE_FUNCTION;
}

void
ewl_text_set_text(Ewl_Widget * w, const char *text)
{
	Ewl_Text * t;
	char * ot;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	ot = t->text;

	if (text == NULL)
		t->text = strdup("");
	else
		t->text = strdup(text);

	if (t->tox)
	  {
		double xx, yy, ww, hh;

		etox_resize(t->tox, START_W, START_H);
		etox_set_text(t->tox, ET_TEXT(t->text), ET_END);
		etox_get_actual_geometry(t->tox, &xx, &yy, &ww, &hh);
		etox_resize(t->tox, ww, hh);
		ewl_widget_configure(w);
	  }

	IF_FREE(ot);

	DLEAVE_FUNCTION;
}

char *
ewl_text_get_text(Ewl_Widget * w)
{
	Ewl_Text * t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	t = EWL_TEXT(w);

	DRETURN_PTR(t->text ? strdup(t->text) : NULL);
}

void
ewl_text_set_font(Ewl_Widget * w, char *f)
{
	Ewl_Text * t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("f", f);

	t = EWL_TEXT(w);

	IF_FREE(t->font);

	t->font = strdup(f);

	etox_set_font(t->tox, t->font, t->font_size);

	DLEAVE_FUNCTION;
}

char *
ewl_text_get_font(Ewl_Widget * w)
{
	Ewl_Text * t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	t = EWL_TEXT(w);

	DRETURN_PTR(t->font ? strdup(t->font) : NULL);
}

void
ewl_text_set_font_size(Ewl_Widget * w, int s)
{
	Ewl_Text * t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	t->font_size = s;

	etox_set_font(t->tox, t->font, t->font_size);

	DLEAVE_FUNCTION;
}

int
ewl_text_get_font_size(Ewl_Widget * w)
{
	Ewl_Text * t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, 0);

	t = EWL_TEXT(w);

	DRETURN_INT(t->font_size);
}

void
ewl_text_set_color(Ewl_Widget * w, int r, int g, int b, int a)
{
	Ewl_Text * t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	etox_color_set_member(t->color, "EWL", r, g, b, a);

	DLEAVE_FUNCTION;
}

void
ewl_text_get_color(Ewl_Widget * w, int *r, int *g, int *b, int *a)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

void
ewl_text_set_style(Ewl_Widget * w, char * s)
{
	Ewl_Text * t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	if (!s || (s && !strlen(s)))
		t->style = etox_style_new("Default");
	else
		t->style = etox_style_new(s);

	DLEAVE_FUNCTION;
}

void
ewl_text_get_text_geometry(Ewl_Widget * w, double *xx, double *yy,
			double *ww, double *hh)
{
	Ewl_Text * t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	etox_get_geometry(t->tox, xx, yy, ww, hh);

	DLEAVE_FUNCTION;
}

void
ewl_text_get_letter_geometry(Ewl_Widget * w, int i,
			     double *xx, double *yy, double *ww, double *hh)
{
	Ewl_Text * t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	etox_get_at(t->tox, i, xx, yy, ww, hh);

	DLEAVE_FUNCTION;
}

void
ewl_text_get_letter_geometry_at(Ewl_Widget * w, double x, double y,
				double *tx, double *ty, double *tw, double *th)
{
	Ewl_Text * t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	etox_get_at_position(t->tox, x, y, tx, ty, tw, th);

	DLEAVE_FUNCTION;
}

void
ewl_text_set_alignment(Ewl_Widget * w, Ewl_Alignment a)
{
	Etox_Align_Type h_align, v_align;
	Ewl_Text * t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	t->align = a;

	if (!t->tox)
		DRETURN;

        if (t->align & EWL_ALIGNMENT_BOTTOM)
                v_align = ETOX_ALIGN_TYPE_BOTTOM;
        else if (t->align & EWL_ALIGNMENT_TOP)
                v_align = ETOX_ALIGN_TYPE_TOP;
        else
                v_align = ETOX_ALIGN_TYPE_CENTER;

        /*
         * Determine the proper horizontal alignment
         */
        if (t->align & EWL_ALIGNMENT_RIGHT)
                h_align = ETOX_ALIGN_TYPE_RIGHT;
        else if (t->align & EWL_ALIGNMENT_LEFT)
                h_align = ETOX_ALIGN_TYPE_LEFT;
        else
                h_align = ETOX_ALIGN_TYPE_CENTER;

	etox_set_align(t->tox, h_align, v_align);

	DLEAVE_FUNCTION;
}
