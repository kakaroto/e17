
#include <Ewl.h>
#include <Etox.h>

static void __ewl_text_init(Ewl_Widget * widget);
static void __ewl_text_realize(Ewl_Widget * widget, void *event_data,
			       void *user_data);
static void __ewl_text_show(Ewl_Widget * widget, void *event_data,
			    void *user_data);
static void ewl_text_hide(Ewl_Widget * widget, void *event_data,
			  void *user_data);
static void ewl_text_destroy(Ewl_Widget * widget, void *event_data,
			     void *user_data);
static void ewl_text_configure(Ewl_Widget * widget, void *event_data,
			       void *user_data);

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
	CHECK_PARAM_POINTER("widget", widget);

	/*
	 * Clear and initialize the text structure
	 */
	memset(text, 0, sizeof(Ewl_Text));
	ewl_widget_init(widget, 10, 10, EWL_FILL_POLICY_NORMAL,
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
	ewl_callback_append(widget, EWL_CALLBACK_SHOW, __ewl_text_show,
			    NULL);
	ewl_callback_append(widget, EWL_CALLBACK_HIDE, ewl_text_hide,
			    NULL);
	ewl_callback_append(widget, EWL_CALLBACK_DESTROY, ewl_text_destroy,
			    NULL);
	ewl_callback_append(widget, EWL_CALLBACK_CONFIGURE,
			    ewl_text_configure, NULL);

	DLEAVE_FUNCTION;
}

static void
__ewl_text_realize(Ewl_Widget * widget, void *event_data, void *user_data)
{
	Etox_Align_Type h_align, v_align;
	char *name;

	DENTER_FUNCTION;
	CHECK_PARAM_POINTER("widget", widget);

	EWL_TEXT(widget)->style = etox_style_new("Default");

	/*
	 * Determine the proper vertical alignment
	 */
	if (EWL_TEXT(widget)->align & EWL_ALIGNMENT_BOTTOM)
		v_align = ETOX_ALIGN_TYPE_BOTTOM;
	else if (EWL_TEXT(widget)->align & EWL_ALIGNMENT_TOP)
		v_align = ETOX_ALIGN_TYPE_TOP;
	else
		v_align = ETOX_ALIGN_TYPE_CENTER;

	/*
	 * Determine the proper horizontal alignment
	 */
	if (EWL_TEXT(widget)->align & EWL_ALIGNMENT_RIGHT)
		h_align = ETOX_ALIGN_TYPE_RIGHT;
	else if (EWL_TEXT(widget)->align & EWL_ALIGNMENT_LEFT)
		h_align = ETOX_ALIGN_TYPE_LEFT;
	else
		h_align = ETOX_ALIGN_TYPE_CENTER;

	/*
	 * Create the evas objects needed to draw the text
	 */
	ewl_fx_clip_box_create(widget);

	name = NEW(char, 11);

	snprintf(name, 11, "%p", name);
	EWL_TEXT(widget)->tox = etox_new(widget->evas, name);

	FREE(name);

	/*
	 * Set up necessary etox properties
	 */
	etox_set_align(EWL_TEXT(widget)->tox, h_align, v_align);
	etox_set_style(EWL_TEXT(widget)->tox, EWL_TEXT(widget)->style);
	etox_set_layer(EWL_TEXT(widget)->tox, LAYER(widget));
	etox_move(EWL_TEXT(widget)->tox, CURRENT_X(widget),
		  CURRENT_Y(widget));
	etox_resize(EWL_TEXT(widget)->tox, CURRENT_W(widget),
		    CURRENT_H(widget));
	etox_set_padding(EWL_TEXT(widget)->tox, EWL_TEXT(widget)->padding);
	etox_set_font(EWL_TEXT(widget)->tox, EWL_TEXT(widget)->font,
		      EWL_TEXT(widget)->font_size);
	etox_set_text(EWL_TEXT(widget)->tox, EWL_TEXT(widget)->text);
	etox_show(EWL_TEXT(widget)->tox);
	etox_set_clip(EWL_TEXT(widget)->tox, widget->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_text_show(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	evas_set_clip(widget->evas,
		      widget->fx_clip_box,
		      EWL_CONTAINER(widget->parent)->clip_box);

	if (EWL_TEXT(widget)->text)
		ewl_text_set_text(widget, strdup(EWL_TEXT(widget)->text));

	evas_set_color(widget->evas, widget->fx_clip_box, 255, 255, 255,
		       255);

	DLEAVE_FUNCTION;
}

static void
ewl_text_hide(Ewl_Widget * widget, void *event_data, void *user_data)
{

	CHECK_PARAM_POINTER("widget", widget);
}

static void
ewl_text_destroy(Ewl_Widget * widget, void *event_data, void *user_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	evas_hide(widget->evas, widget->fx_clip_box);
	evas_unset_clip(widget->evas, widget->fx_clip_box);
	evas_del_object(widget->evas, widget->fx_clip_box);

	FREE(widget);
}

static void
ewl_text_configure(Ewl_Widget * widget, void *event_data, void *user_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_TEXT(widget)->tox) {
		double x, y, w, h;

		etox_get_geometry(EWL_TEXT(widget)->tox, &x, &y, &w, &h);
		if (MIN_W(widget) < (int) (w))
			MIN_W(widget) = (int) (w);
		if (MIN_H(widget) < (int) (h))
			MIN_H(widget) = (int) (h);
	}

	ewl_object_apply_requested(EWL_OBJECT(widget));
	ewl_fx_clip_box_resize(widget);
}

void
ewl_text_set_text(Ewl_Widget * widget, const char *text)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_TEXT(widget)->text)
		FREE(EWL_TEXT(widget)->text);

	if (text == NULL)
		EWL_TEXT(widget)->text = strdup("");
	else
		EWL_TEXT(widget)->text = strdup(text);

	if (EWL_TEXT(widget)->tox)
		etox_set_text(EWL_TEXT(widget)->tox,
			      EWL_TEXT(widget)->text);
}

char *
ewl_text_get_text(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER_RETURN("widget", widget, NULL);

	return (EWL_TEXT(widget)->
		text ? strdup(EWL_TEXT(widget)->text) : NULL);
}

void
ewl_text_set_font(Ewl_Widget * widget, char *font)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("font", font);

	EWL_TEXT(widget)->font = strdup(font);
	etox_set_font(EWL_TEXT(widget)->tox, font,
		      EWL_TEXT(widget)->font_size);
}

char *
ewl_text_get_font(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER_RETURN("widget", widget, NULL);

	return (EWL_TEXT(widget)->
		font ? strdup(EWL_TEXT(widget)->font) : NULL);
}

void
ewl_text_set_font_size(Ewl_Widget * widget, int size)
{
	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("size", size);

	EWL_TEXT(widget)->font_size = size;
	etox_set_font(EWL_TEXT(widget)->tox, EWL_TEXT(widget)->font, size);
}

int
ewl_text_get_font_size(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER_RETURN("widget", widget, 0);

	return EWL_TEXT(widget)->font_size;
}

void
ewl_text_set_color(Ewl_Widget * widget, int r, int g, int b, int a)
{
	CHECK_PARAM_POINTER("widget", widget);

	etox_color_set_member(EWL_TEXT(widget)->color, "EWL", r, g, b, a);
}

void
ewl_text_get_color(Ewl_Widget * widget, int *r, int *g, int *b, int *a)
{
	CHECK_PARAM_POINTER("widget", widget);
}

void
ewl_text_get_text_geometry(Ewl_Widget * widget, double *x, double *y,
			   double *w, double *h)
{
	CHECK_PARAM_POINTER("widget", widget);

	etox_get_geometry(EWL_TEXT(widget)->tox, x, y, w, h);
}

void
ewl_text_get_letter_geometry(Ewl_Widget * widget, int index,
			     int *x, int *y, int *w, int *h)
{
	CHECK_PARAM_POINTER("widget", widget);

	etox_get_at(EWL_TEXT(widget)->tox, index, x, y, w, h);
}

void
ewl_text_get_letter_geometry_at(Ewl_Widget * widget, int x, int y,
				int *tx, int *ty, int *tw, int *th)
{
	CHECK_PARAM_POINTER("widget", widget);

	etox_get_at_position(EWL_TEXT(widget)->tox, x, y, tx, ty, tw, th);
}
