
#include <Ewl.h>
#include <Etox.h>

void ewl_text_init(Ewl_Text * t);
static void __ewl_text_realize(Ewl_Widget * w, void *ev_data,
			       void *user_data);
static void __ewl_text_destroy(Ewl_Widget * w, void *ev_data,
			       void *user_data);
static void __ewl_text_configure(Ewl_Widget * w, void *ev_data,
				 void *user_data);
static void __ewl_text_theme_update(Ewl_Widget * w, void *ev_data,
				    void *user_data);
static void __ewl_text_reparent(Ewl_Widget * w, void *ev_data,
				void *user_data);


#define START_W 2048
#define START_H 2048

/**
 * ewl_text_new - allocate a new text widget
 *
 * Returns a pointer to a newly allocated text widget on success, NULL on
 * failure.
 */
Ewl_Widget *
ewl_text_new()
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);

	t = NEW(Ewl_Text, 1);

	ZERO(t, Ewl_Text, 1);
	ewl_text_init(t);

	DRETURN_PTR(EWL_WIDGET(t), DLEVEL_STABLE);
}

/**
 * ewl_text_init - initialize a text widget to default values and callbacks
 * @t: the text widget to initialize to default values and callbacks
 *
 * Returns no value. Sets the fields and callbacks of the text widget @t to
 * their defaults.
 */
void
ewl_text_init(Ewl_Text * t)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
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
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_text_configure, NULL);
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_text_theme_update, NULL);
	ewl_callback_append(w, EWL_CALLBACK_REPARENT,
			    __ewl_text_reparent, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_text_set_text - set the text of a text widget
 * @t: the text widget to set the text
 * @text: the new text for the text widget @t
 *
 * Returns no value. Sets the text of the text widget @t to @text.
 */
void
ewl_text_set_text(Ewl_Text * t, char *text)
{
	Ewl_Widget *w;
	char *ot;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	w = EWL_WIDGET(t);

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

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_text_get_text - retrieve the text of a text widget
 * @t: the text widget to retrieve the text
 *
 * Returns a pointer to a copy of the text in @t on success, NULL on failure.
 */
char *
ewl_text_get_text(Ewl_Text * t)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);

	w = EWL_WIDGET(t);

	DRETURN_PTR(t->text ? strdup(t->text) : NULL, DLEVEL_STABLE);
}

/**
 * ewl_text_set_font - set the font of a text widget
 * @t: the text widget to set the font
 * @f: the name of the font to use for the text widget
 *
 * Returns no value. Sets the name of the font for text widget @t to @f and
 * updates the display to use that font.
 */
void
ewl_text_set_font(Ewl_Text * t, char *f)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);
	DCHECK_PARAM_PTR("f", f);

	w = EWL_WIDGET(t);

	IF_FREE(t->font);

	t->font = strdup(f);

	etox_set_font(t->tox, t->font, t->font_size);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_text_get_font - retrieve the font used by a text widget
 * @t: the text widget to get the font
 *
 * Returns a pointer to a copy of the font name used by @t on success, NULL on
 * failure.
 */
char *
ewl_text_get_font(Ewl_Text * t)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, NULL);

	w = EWL_WIDGET(t);

	DRETURN_PTR(t->font ? strdup(t->font) : NULL, DLEVEL_STABLE);
}

/**
 * ewl_text_set_font_size - set the font size of a text widget
 * @t: the text widget to set the font size
 * @s: the font size to use for the text widget
 *
 * Returns no value. Sets the font size for the text widget @t to @s.
 */
void
ewl_text_set_font_size(Ewl_Text * t, int s)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	w = EWL_WIDGET(t);

	t->font_size = s;

	etox_set_font(t->tox, t->font, t->font_size);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_text_get_font_size - retrieve the font size of a text widget
 * @t: the text widget to retrieve the font size
 *
 * Returns the font size of the text widget on success, 0 on failure.
 */
int
ewl_text_get_font_size(Ewl_Text * t)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);

	w = EWL_WIDGET(t);

	DRETURN_INT(t->font_size, DLEVEL_STABLE);
}

/**
 * ewl_text_set_color - set the color of the text for a text widget
 * @t: the text widget to set the color
 * @r: the red value for the color
 * @g: the green value for the color
 * @b: the blue value for the color
 * @a: the alpha value for the color
 *
 * Returns no value. Sets the color of the text in the text widget @t to the
 * new color values specified.
 */
void
ewl_text_set_color(Ewl_Text * t, int r, int g, int b, int a)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	w = EWL_WIDGET(t);

	etox_color_set_member(t->color, "EWL", r, g, b, a);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_text_get_color - get the color of the text in a text widget
 * @t: the text widget to get the color
 * @r: a pointer to the integer to store the red value
 * @g: a pointer to the integer to store the green value
 * @b: a pointer to the integer to store the blue value
 * @a: a pointer to the integer to store the alpha value
 *
 * Returns no value. Stores the color values into any non-NULL color pointers.
 */
void
ewl_text_get_color(Ewl_Text * t, int *r, int *g, int *b, int *a)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	w = EWL_WIDGET(t);

	etox_color_get_member(t->color, "EWL", r, g, b, a);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_text_set_style - set the text style for a text widget
 * @t: the text widget to set the text style
 * @s: the name of the style to be set for the text
 *
 * Returns no value. Changes the text style of the text widget @t to the style
 * identified by the name @s.
 */
void
ewl_text_set_style(Ewl_Text * t, char *s)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	w = EWL_WIDGET(t);

	if (!s || (s && !strlen(s)))
		t->style = etox_style_new("Default");
	else
		t->style = etox_style_new(s);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_text_get_text_geometry - retrieve the geometry of a text widget
 * @t: the text widget to retrieve geometry
 * @xx: a pointer to an integer to store the x coordinate
 * @yy: a pointer to an integer to store the y coordinate
 * @ww: a pointer to an integer to store the width
 * @hh: a pointer to an integer to store the height
 *
 * Returns no value. Stores the position and size of the text in the text
 * widget @t into the integers pointed to by @xx, @yy, @ww, and @hh
 * respectively.
 */
void
ewl_text_get_text_geometry(Ewl_Text * t, int *xx, int *yy, int *ww, int *hh)
{
	Ewl_Widget *w;
	double nxx, nyy, nww, nhh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	w = EWL_WIDGET(t);

	etox_get_geometry(t->tox, &nxx, &nyy, &nww, &nhh);

	if (xx)
		*xx = (int) (nxx);
	if (yy)
		*yy = (int) (nyy);
	if (ww)
		*ww = (int) (nww);
	if (hh)
		*hh = (int) (nhh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_text_get_letter_geometry - retrieve the geomtry of a specific letter
 * @t: the widget that holds the text to retrieve a letters geometry
 * @i: the index of the letter in the text to retrieve geometry
 * @xx: a pointer to an integer to store the x coordinate of the letter
 * @yy: a pointer to an integer to store the y coordinate of the letter
 * @ww: a pointer to an integer to store the width of the letter
 * @hh: a pointer to an integer to store the height of the letter
 *
 * Returns no value. Stores the geometry of the letter at index @i of the text
 * widget @t into @xx, @yy, @ww, and @hh respectively.
 */
void
ewl_text_get_letter_geometry(Ewl_Text * t, int i, int *xx, int *yy, int *ww,
		int *hh)
{
	Ewl_Widget *w;
	double nxx, nyy, nww, nhh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	w = EWL_WIDGET(t);

	etox_get_char_geometry_at(t->tox, i, &nxx, &nyy, &nww, &nhh);

	if (xx)
		*xx = (int) (nxx);
	if (yy)
		*yy = (int) (nyy);
	if (ww)
		*ww = (int) (nww);
	if (hh)
		*hh = (int) (nhh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_text_get_letter_geometry_at - get the letter geometry at coordinates
 * @t: the text widget to get the letter geometry by coordinates
 * @x: the x coordinate to check for letter geometry
 * @y: the y coordinate to check for letter geometry
 * @tx: the x coordinate of the letter that intersects @x, @y
 * @ty: the y coordinate of the letter that intersects @x, @y
 * @tw: the width of the letter that intersects @x, @y
 * @th: the height of the letter that intersects @x, @y
 *
 * Returns no value. Stores the geometry of a letter at specified coordinates
 * @x, @y of text widget @t into @tx, @ty, @tw, and @th.
 */
void
ewl_text_get_letter_geometry_at(Ewl_Text * t, int x, int y,
				int *tx, int *ty, int *tw, int *th)
{
	Ewl_Widget *w;
	double xx, yy, ww, hh;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	w = EWL_WIDGET(t);

	etox_get_char_geometry_at_position(t->tox, (double) x, (double) y,
					   &xx, &yy, &ww, &hh);

	if (tx)
		*tx = (int) (xx);
	if (ty)
		*ty = (int) (yy);
	if (tw)
		*tw = (int) (ww);
	if (th)
		*th = (int) (hh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_text_set_alignment - set the alignment of the text in a text widget
 * @t: the text widget to change text alignment
 * @a: the new alignment for the text in @t
 *
 * Returns no value. Changes the alignment of the text in @t to @a.
 */
void
ewl_text_set_alignment(Ewl_Text * t, Ewl_Alignment a)
{
	Ewl_Widget *w;
	Etox_Align_Type h_align, v_align;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("t", t);

	w = EWL_WIDGET(t);
	EWL_OBJECT(w)->alignment = a;

	if (!t->tox)
		DRETURN(DLEVEL_STABLE);

	if (EWL_OBJECT(t)->alignment & EWL_ALIGNMENT_BOTTOM)
		v_align = ETOX_ALIGN_TYPE_BOTTOM;
	else if (EWL_OBJECT(t)->alignment & EWL_ALIGNMENT_TOP)
		v_align = ETOX_ALIGN_TYPE_TOP;
	else
		v_align = ETOX_ALIGN_TYPE_CENTER;

	/*
	 * Determine the proper horizontal alignment
	 */
	if (EWL_OBJECT(t)->alignment & EWL_ALIGNMENT_RIGHT)
		h_align = ETOX_ALIGN_TYPE_RIGHT;
	else if (EWL_OBJECT(t)->alignment & EWL_ALIGNMENT_LEFT)
		h_align = ETOX_ALIGN_TYPE_LEFT;
	else
		h_align = ETOX_ALIGN_TYPE_CENTER;

	etox_set_align(t->tox, h_align, v_align);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_text_get_index_at - get the index of the letter at coordinates
 * @t: the text widget to find the letter index by coordinates
 * @x: the x coordinate to check for the letter index
 * @y: the y coordinate to check for the letter index
 *
 * Returns the index of the letter at the coordinates @x, @y in the text
 * widget @t.
 */
int
ewl_text_get_index_at(Ewl_Text * t, int x, int y)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("t", t, 0);

	w = EWL_WIDGET(t);

	DRETURN_INT(DLEVEL_STABLE, etox_get_char_geometry_at_position(t->tox,
							    (double) x,
							    (double) y, NULL,
							    NULL, NULL, NULL));
}

static void
__ewl_text_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Etox_Align_Type h_align, v_align;
	Ewl_Text *t;
	char *name;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	t->style = etox_style_new("Default");

	/*
	 * Determine the proper vertical alignment
	 */

/*	if (EWL_OBJECT(t)->align & EWL_ALIGNMENT_BOTTOM)
		v_align = ETOX_ALIGN_TYPE_BOTTOM;
	else if (EWL_OBJECT(t)->align & EWL_ALIGNMENT_TOP)*/
	v_align = ETOX_ALIGN_TYPE_TOP;

/*	else
		v_align = ETOX_ALIGN_TYPE_CENTER;*/

	/*
	 * Determine the proper horizontal alignment
	 */

/*	if (EWL_OBJECT(t)->align & EWL_ALIGNMENT_RIGHT)
		h_align = ETOX_ALIGN_TYPE_RIGHT;
	else if (EWL_OBJECT(t)->align & EWL_ALIGNMENT_LEFT)*/
	h_align = ETOX_ALIGN_TYPE_LEFT;

/*	else
		h_align = ETOX_ALIGN_TYPE_CENTER;*/

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

	__ewl_text_theme_update(w, NULL, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_text_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	if (t->tox)
	  {
		  etox_hide(t->tox);
		  etox_unset_clip(t->tox);
		  etox_free(t->tox);
	  }

	IF_FREE(t->text);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_text_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	if (t->tox)
	  {
		  double ww, hh;

		  etox_move(t->tox, CURRENT_X(t), CURRENT_Y(t));
		  etox_resize(t->tox, START_W, START_H);
		  etox_get_actual_geometry(t->tox, NULL, NULL, &ww, &hh);
//                etox_resize(t->tox, ww, hh);
		  ewl_object_set_custom_size(EWL_OBJECT(w), ww, hh);
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_text_theme_update(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Text *t;
	char key[PATH_LEN];
	char *font = NULL /*, *style = NULL */ ;
	int font_size = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	if (!t->tox)
		DRETURN(DLEVEL_STABLE);

	snprintf(key, PATH_LEN, "%s/font", w->appearance);
	font = ewl_theme_data_get_str(w, key);

	snprintf(key, PATH_LEN, "%s/font_size", w->appearance);
	font_size = ewl_theme_data_get_int(w, key);

	if (font)
	  {
		  IF_FREE(t->font);

		  t->font = font;
		  t->font_size = font_size;

		  etox_set_font(t->tox, font, font_size);
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_text_reparent(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Text *t;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	t = EWL_TEXT(w);

	if (!t->tox)
		DRETURN(DLEVEL_STABLE);

	etox_unset_clip(t->tox);
	etox_set_clip(t->tox, w->fx_clip_box);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
