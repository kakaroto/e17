
#include <Ewl.h>
#include <Etox.h>

void ewl_text_init(Ewl_Text * t);
static void __ewl_text_realize(Ewl_Widget * w, void *ev_data,
			       void *user_data);
static void __ewl_text_destroy(Ewl_Widget * w, void *ev_data,
			       void *user_data);
static void __ewl_text_configure(Ewl_Widget * w, void *ev_data,
				 void *user_data);

#define START_W 2048
#define START_H 2048

Ewl_Widget *
ewl_text_new()
{
	Ewl_Text *t;

	DENTER_FUNCTION;

	t = NEW(Ewl_Text, 1);

	memset(t, 0, sizeof(Ewl_Text));
	ewl_text_init(t);

	DRETURN_PTR(EWL_WIDGET(t));
}

void
ewl_text_init(Ewl_Text * t)
{
	Ewl_Widget *w;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("t", t);

	w = EWL_WIDGET(t);

	ewl_widget_init(w, "/appearance/text/default");

	t->font = strdup("borzoib");
	t->font_size = 10;
	t->text = strdup("");
	t->color = etox_color_new();

	/*
	 * Set up appropriate callbacks for specific events
	 */
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_text_realize, NULL);
	ewl_callback_prepend(w, EWL_CALLBACK_DESTROY, __ewl_text_destroy,
			     NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE, __ewl_text_configure,
			    NULL);
	ewl_callback_del(w, EWL_CALLBACK_THEME_UPDATE,
			 __ewl_widget_theme_update);

	DLEAVE_FUNCTION;
}

static void
__ewl_text_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Etox_Align_Type h_align, v_align;
	Ewl_Text *t;
	char *name;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	t->style = etox_style_new("Default");

	/*
	 * Determine the proper vertical alignment
	 */
	if (EWL_OBJECT(t)->align & EWL_ALIGNMENT_BOTTOM)
		v_align = ETOX_ALIGN_TYPE_BOTTOM;
	else if (EWL_OBJECT(t)->align & EWL_ALIGNMENT_TOP)
		v_align = ETOX_ALIGN_TYPE_TOP;
	else
		v_align = ETOX_ALIGN_TYPE_CENTER;

	/*
	 * Determine the proper horizontal alignment
	 */
	if (EWL_OBJECT(t)->align & EWL_ALIGNMENT_RIGHT)
		h_align = ETOX_ALIGN_TYPE_RIGHT;
	else if (EWL_OBJECT(t)->align & EWL_ALIGNMENT_LEFT)
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
__ewl_text_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Text *t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	etox_hide(t->tox);
	etox_unset_clip(t->tox);
	etox_free(t->tox);

	IF_FREE(t->text);

	DLEAVE_FUNCTION;
}

static void
__ewl_text_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Text *t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	if (t->tox)
	  {
		  double xx, yy, ww, hh;

		  etox_move(t->tox, CURRENT_X(t), CURRENT_Y(t));
		  etox_resize(t->tox, START_W, START_H);
		  etox_get_actual_geometry(t->tox, &xx, &yy, &ww, &hh);
		  etox_resize(t->tox, ww, hh);
		  ewl_object_set_custom_size(EWL_OBJECT(w), ww, hh);
	  }

	DLEAVE_FUNCTION;
}

void
ewl_text_set_text(Ewl_Widget * w, char *text)
{
	Ewl_Text *t;
	char *ot;

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
		  ewl_object_set_custom_size(EWL_OBJECT(t), ww, hh);
		  ewl_widget_configure(EWL_WIDGET(t));
	  }

	IF_FREE(ot);

	DLEAVE_FUNCTION;
}

char *
ewl_text_get_text(Ewl_Widget * w)
{
	Ewl_Text *t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	t = EWL_TEXT(w);

	DRETURN_PTR(t->text ? strdup(t->text) : NULL);
}

void
ewl_text_set_font(Ewl_Widget * w, char *f)
{
	Ewl_Text *t;

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
	Ewl_Text *t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	t = EWL_TEXT(w);

	DRETURN_PTR(t->font ? strdup(t->font) : NULL);
}

void
ewl_text_set_font_size(Ewl_Widget * w, int s)
{
	Ewl_Text *t;

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
	Ewl_Text *t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, 0);

	t = EWL_TEXT(w);

	DRETURN_INT(t->font_size);
}

void
ewl_text_set_color(Ewl_Widget * w, int r, int g, int b, int a)
{
	Ewl_Text *t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	etox_color_set_member(t->color, "EWL", r, g, b, a);

	DLEAVE_FUNCTION;
}

void
ewl_text_get_color(Ewl_Widget * w, int *r, int *g, int *b, int *a)
{
	Ewl_Text *t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	etox_color_get_member(t->color, "EWL", r, g, b, a);

	DLEAVE_FUNCTION;
}

void
ewl_text_set_style(Ewl_Widget * w, char *s)
{
	Ewl_Text *t;

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
	Ewl_Text *t;

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
	Ewl_Text *t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	etox_get_at(t->tox, i, xx, yy, ww, hh);

	DLEAVE_FUNCTION;
}

void
ewl_text_get_letter_geometry_at(Ewl_Widget * w, double x, double y,
				double *tx, double *ty, double *tw,
				double *th)
{
	Ewl_Text *t;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	etox_get_at_position(t->tox, x, y, tx, ty, tw, th);

	DLEAVE_FUNCTION;
}

void
ewl_text_set_alignment(Ewl_Widget * w, Ewl_Alignment a)
{
	Ewl_Text *t;
	Etox_Align_Type h_align, v_align;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);
	EWL_OBJECT(w)->align = a;

	if (!t->tox)
		DRETURN;

	if (EWL_OBJECT(t)->align & EWL_ALIGNMENT_BOTTOM)
		v_align = ETOX_ALIGN_TYPE_BOTTOM;
	else if (EWL_OBJECT(t)->align & EWL_ALIGNMENT_TOP)
		v_align = ETOX_ALIGN_TYPE_TOP;
	else
		v_align = ETOX_ALIGN_TYPE_CENTER;

	/*
	 * Determine the proper horizontal alignment
	 */
	if (EWL_OBJECT(t)->align & EWL_ALIGNMENT_RIGHT)
		h_align = ETOX_ALIGN_TYPE_RIGHT;
	else if (EWL_OBJECT(t)->align & EWL_ALIGNMENT_LEFT)
		h_align = ETOX_ALIGN_TYPE_LEFT;
	else
		h_align = ETOX_ALIGN_TYPE_CENTER;

	etox_set_align(t->tox, h_align, v_align);

	DLEAVE_FUNCTION;
}

int
ewl_text_get_index_at(Ewl_Widget * w, double x, double y, int *index)
{
	Ewl_Text *t;
	int ret;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, -1);

	t = EWL_TEXT(w);

	ret = etox_get_index_at(t->tox, x, y, index);

	DRETURN_INT(ret);
}
