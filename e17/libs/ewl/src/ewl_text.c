
#include <Ewl.h>

struct _ewl_text_row
{
	Ewl_Widget widget;
	Evas_Object object;
	char *text;
	struct
	{
		int r, g, b, a;
	}
	color;
};

typedef struct _ewl_text_row Ewl_Text_Row;

#define EWL_TEXT_ROW(row) ((Ewl_Text_Row *) row)

static void ewl_text_init(Ewl_Widget * widget);
static void ewl_text_realize(Ewl_Widget * widget, void *event_data,
			     void *user_data);
static void ewl_text_show(Ewl_Widget * widget, void *event_data,
			  void *user_data);
static void ewl_text_hide(Ewl_Widget * widget, void *event_data,
			  void *user_data);
static void ewl_text_destroy(Ewl_Widget * widget, void *event_data,
			     void *user_data);
static void ewl_text_configure(Ewl_Widget * widget, void *event_data,
			       void *user_data);

static Ewl_Text_Row *ewl_text_row_new();
static void ewl_text_row_init(Ewl_Text_Row * row);
static void ewl_text_row_realize(Ewl_Widget * widget, void *event_data,
				 void *user_data);
static void ewl_text_row_destroy(Ewl_Widget * widget, void *event_data,
				 void *user_data);
static void ewl_text_row_configure(Ewl_Widget * widget, void *event_data,
				   void *user_data);


Ewl_Widget *
ewl_text_new()
{
	Ewl_Text *text = NULL;

	DENTER_FUNCTION;

	text = NEW(Ewl_Text, 1);

	ewl_text_init(EWL_WIDGET(text));

	text->font = strdup("borzoib");
	text->font_size = 15;
	text->text = strdup("");
	text->color.r = 0;
	text->color.g = 0;
	text->color.b = 0;
	text->color.a = 200;

	DRETURN_PTR(EWL_WIDGET(text));
}

static void
ewl_text_init(Ewl_Widget * widget)
{
	DENTER_FUNCTION;
	CHECK_PARAM_POINTER("widget", widget);


	memset(EWL_TEXT(widget), 0, sizeof(Ewl_Text));
	ewl_container_init(EWL_CONTAINER(widget), EWL_WIDGET_TEXT, 10, 10,
			   2048, 2048);

	ewl_callback_append(widget, EWL_CALLBACK_REALIZE, ewl_text_realize,
			    NULL);
	ewl_callback_append(widget, EWL_CALLBACK_SHOW, ewl_text_show, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_HIDE, ewl_text_hide, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_DESTROY, ewl_text_destroy,
			    NULL);
	ewl_callback_append(widget, EWL_CALLBACK_CONFIGURE,
			    ewl_text_configure, NULL);

	EWL_WIDGET(widget)->recursive = TRUE;

	DLEAVE_FUNCTION;
}

static void
ewl_text_realize(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;
	CHECK_PARAM_POINTER("widget", widget);

	DLEAVE_FUNCTION;
}

static void
ewl_text_show(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	ewl_fx_clip_box_create(widget);

	EWL_CONTAINER(widget)->clip_box = widget->fx_clip_box;

	evas_set_clip(widget->evas,
		      widget->fx_clip_box,
		      EWL_CONTAINER(widget->parent)->clip_box);

	if (EWL_TEXT(widget)->text)
		ewl_text_set_text(widget, strdup(EWL_TEXT(widget)->text));

	evas_set_color(widget->evas, widget->fx_clip_box, 255, 255, 255, 255);

	DLEAVE_FUNCTION;
}

static void
ewl_text_hide(Ewl_Widget * widget, void *event_data, void *user_data)
{
	Ewl_Text_Row *row;

	CHECK_PARAM_POINTER("widget", widget);

	if (!EWL_CONTAINER(widget)->children ||
	    ewd_list_is_empty(EWL_CONTAINER(widget)->children))
		return;

	ewd_list_goto_first(EWL_CONTAINER(widget)->children);

	while ((row = ewd_list_next(EWL_CONTAINER(widget)->children)) != NULL)
	  {
		  ewl_callback_call(EWL_WIDGET(row), EWL_CALLBACK_HIDE);
	  }
}

static void
ewl_text_destroy(Ewl_Widget * widget, void *event_data, void *user_data)
{
	Ewl_Text_Row *row = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_CONTAINER(widget)->children ||
	    !ewd_list_is_empty(EWL_CONTAINER(widget)->children))
	  {
		  while ((row =
			  ewd_list_remove_last(EWL_CONTAINER(widget)->
					       children)) != NULL)
			  ewl_widget_destroy(EWL_WIDGET(row));
	  }

	evas_hide(widget->evas, widget->fx_clip_box);
	evas_unset_clip(widget->evas, widget->fx_clip_box);
	evas_del_object(widget->evas, widget->fx_clip_box);

	IF_FREE(EWL_TEXT(widget)->text);
	IF_FREE(EWL_TEXT(widget)->font);
	FREE(widget);
}

static void
ewl_text_configure(Ewl_Widget * widget, void *event_data, void *user_data)
{
	Ewl_Text_Row *row;
	int y, h = 0, w = 0;

	CHECK_PARAM_POINTER("widget", widget);

	EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
	EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;

	if (!EWL_CONTAINER(widget)->children ||
	    ewd_list_is_empty(EWL_CONTAINER(widget)->children))
		return;

	ewd_list_goto_first(EWL_CONTAINER(widget)->children);

	y = EWL_OBJECT(widget)->current.y;

	ewl_fx_clip_box_resize(widget);

	while ((row = ewd_list_next(EWL_CONTAINER(widget)->children)) != NULL)
	  {
		  EWL_OBJECT(row)->request.x = EWL_OBJECT(widget)->current.x;
		  EWL_OBJECT(row)->request.y = y;
		  ewl_callback_call(EWL_WIDGET(row), EWL_CALLBACK_CONFIGURE);
		  y += EWL_OBJECT(row)->current.h + 2;
		  h += EWL_OBJECT(row)->current.h + 2;
		  if (w < EWL_OBJECT(row)->current.w)
			  w = EWL_OBJECT(row)->current.w;
	  }

	EWL_OBJECT(widget)->request.w = w;
	EWL_OBJECT(widget)->request.h = h;
	EWL_OBJECT(widget)->current.w = w;
	EWL_OBJECT(widget)->current.h = h;
	EWL_OBJECT(widget)->minimum.w = w;
	EWL_OBJECT(widget)->minimum.h = h;
	EWL_OBJECT(widget)->maximum.w = w;
	EWL_OBJECT(widget)->maximum.h = h;
}

static Ewl_Text_Row *
ewl_text_row_new()
{
	Ewl_Text_Row *row = NULL;

	row = NEW(Ewl_Text_Row, 1);

	if (!row)
		return NULL;

	ewl_text_row_init(row);

	row->text = strdup("");

	return row;
}

static void
ewl_text_row_init(Ewl_Text_Row * row)
{
	CHECK_PARAM_POINTER("row", row);

	memset(row, 0, sizeof(Ewl_Text_Row));
	ewl_widget_init(EWL_WIDGET(row), EWL_WIDGET_TEXT, 10, 10, 2048, 2048);

	ewl_callback_append(EWL_WIDGET(row), EWL_CALLBACK_REALIZE,
			    ewl_text_row_realize, NULL);
	ewl_callback_append(EWL_WIDGET(row), EWL_CALLBACK_DESTROY,
			    ewl_text_row_destroy, NULL);
	ewl_callback_append(EWL_WIDGET(row), EWL_CALLBACK_CONFIGURE,
			    ewl_text_row_configure, NULL);
}

static void
ewl_text_row_realize(Ewl_Widget * widget, void *event_data, void *user_data)
{
	Ewl_Text *text;

	CHECK_PARAM_POINTER("widget", widget);

	text = EWL_TEXT(widget->parent);

	EWL_TEXT_ROW(widget)->object = evas_add_text(widget->evas,
						     text->font,
						     text->font_size,
						     EWL_TEXT_ROW(widget)->
						     text);

	evas_move(widget->evas, EWL_TEXT_ROW(widget)->object,
		  EWL_OBJECT(widget)->request.x,
		  EWL_OBJECT(widget)->request.y);

	EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
	EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;

	evas_set_clip(widget->evas, EWL_TEXT_ROW(widget)->object,
		      EWL_CONTAINER(widget->parent)->clip_box);

	evas_set_layer(widget->evas, EWL_TEXT_ROW(widget)->object,
		       EWL_OBJECT(widget)->layer);
	evas_set_color(widget->evas, EWL_TEXT_ROW(widget)->object,
		       text->color.r, text->color.g, text->color.b,
		       text->color.a);

	evas_show(widget->evas, EWL_TEXT_ROW(widget)->object);
}


static void
ewl_text_row_destroy(Ewl_Widget * widget, void *event_data, void *user_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	evas_hide(widget->evas, EWL_TEXT_ROW(widget)->object);
	evas_unset_clip(widget->evas, EWL_TEXT_ROW(widget)->object);
	evas_del_object(widget->evas, EWL_TEXT_ROW(widget)->object);

	IF_FREE(EWL_TEXT_ROW(widget)->text);
	FREE(widget);
}

static void
ewl_text_row_configure(Ewl_Widget * widget, void *event_data, void *user_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	evas_move(widget->evas, EWL_TEXT_ROW(widget)->object,
		  EWL_OBJECT(widget)->request.x,
		  EWL_OBJECT(widget)->request.y);

	EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
	EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;
	EWL_OBJECT(widget)->current.w = evas_get_text_width(widget->evas,
							    EWL_TEXT_ROW
							    (widget)->object);
	EWL_OBJECT(widget)->current.h =
		evas_get_text_height(widget->evas,
				     EWL_TEXT_ROW(widget)->object);
}

void
ewl_text_set_text(Ewl_Widget * widget, const char *text)
{
	Ewl_Text_Row *row = NULL;
	char *string = NULL, *temp = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	if (EWL_CONTAINER(widget)->children &&
	    !ewd_list_is_empty(EWL_CONTAINER(widget)->children))
		while ((row =
			ewd_list_remove_last(EWL_CONTAINER(widget)->
					     children)) != NULL)
			ewl_widget_destroy(EWL_WIDGET(row));


	IF_FREE(EWL_TEXT(widget)->text);
	EWL_TEXT(widget)->text = strdup(text);

	if (!EWL_OBJECT(widget)->visible)
		return;

	/*
	 * This is bad bad!!!!!! REALLY BAD hack! FIXME FIXME 
	 */
	string = strdup(text);
	temp = strchr(string, '\n');

	while (temp && *temp)
	  {
		  char *line;
		  row = ewl_text_row_new();

		  *temp = '\0';
		  line = strdup(string);
		  string = temp + 1;
		  temp = strchr(string, '\n');

		  IF_FREE(row->text);
		  row->text = strdup(line);

		  EWL_WIDGET(row)->parent = widget;
		  EWL_WIDGET(row)->evas = widget->evas;
		  EWL_CONTAINER(row)->clip_box =
			  EWL_CONTAINER(widget)->clip_box;

		  if (EWL_OBJECT(widget)->realized)
			  ewl_widget_realize(EWL_WIDGET(row));
		  if (EWL_OBJECT(widget)->visible)
			  ewl_widget_show(EWL_WIDGET(row));

		  ewl_container_append_child(EWL_CONTAINER(widget),
					     EWL_WIDGET(row));
	  }

	row = ewl_text_row_new();

	row->text = strdup(string);
	EWL_WIDGET(row)->parent = widget;
	EWL_WIDGET(row)->evas = widget->evas;
	EWL_CONTAINER(row)->clip_box = EWL_CONTAINER(widget)->clip_box;

	if (EWL_OBJECT(widget)->realized)
		ewl_widget_realize(EWL_WIDGET(row));
	if (EWL_OBJECT(widget)->visible)
		ewl_widget_show(EWL_WIDGET(row));

	ewl_container_append_child(EWL_CONTAINER(widget), EWL_WIDGET(row));

	ewl_widget_configure(widget);

	if (widget->parent)
		ewl_widget_configure(widget->parent);
}

char *
ewl_text_get_text(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER_RETURN("widget", widget, NULL);

	return strdup(EWL_TEXT(widget)->text);
}

void
ewl_text_set_font(Ewl_Widget * widget, char *font)
{
	Ewl_Widget *child = NULL;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("font", font);

	if (!strcmp(font, EWL_TEXT(widget)->font))
		return;

	IF_FREE(EWL_TEXT(widget)->font);

	EWL_TEXT(widget)->font = strdup(font);

	if (!EWL_CONTAINER(widget)->children)
		return;

	ewd_list_goto_first(EWL_CONTAINER(widget)->children);

	while ((child =
		ewd_list_next(EWL_CONTAINER(widget)->children)) != NULL)
	  {
		  if (!EWL_TEXT_ROW(child)->object)
			  continue;

		  evas_set_font(child->evas, EWL_TEXT_ROW(child)->object,
				EWL_TEXT(widget)->font,
				EWL_TEXT(widget)->font_size);
	  }

	ewl_widget_configure(widget);
	if (widget->parent)
		ewl_widget_configure(widget->parent);
}

char *
ewl_text_get_font(Ewl_Widget * widget)
{
	return strdup(EWL_TEXT(widget)->font);
}

void
ewl_text_set_font_size(Ewl_Widget * widget, int size)
{
	Ewl_Widget *child = NULL;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("size", size);

	if (EWL_TEXT(widget)->font_size == size)
		return;

	EWL_TEXT(widget)->font_size = size;

	if (!EWL_CONTAINER(widget)->children)
		return;

	ewd_list_goto_first(EWL_CONTAINER(widget)->children);

	while ((child =
		ewd_list_next(EWL_CONTAINER(widget)->children)) != NULL)
	  {
		  if (!EWL_TEXT_ROW(child)->object)
			  continue;

		  evas_set_font(child->evas, EWL_TEXT_ROW(child)->object,
				EWL_TEXT(widget)->font,
				EWL_TEXT(widget)->font_size);
	  }

	if (widget->parent)
		ewl_widget_configure(widget->parent);
}

void
ewl_text_get_font_size(Ewl_Widget * widget, int *size)
{
	CHECK_PARAM_POINTER("widget", widget);

	*size = EWL_TEXT(widget)->font_size;
}

void
ewl_text_set_color(Ewl_Widget * widget, int r, int g, int b, int a)
{
	Ewl_Widget *child = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	EWL_TEXT(widget)->color.r = r;
	EWL_TEXT(widget)->color.g = g;
	EWL_TEXT(widget)->color.b = b;
	EWL_TEXT(widget)->color.a = a;

	if (!EWL_CONTAINER(widget)->children)
		DRETURN;

	ewd_list_goto_first(EWL_CONTAINER(widget)->children);

	while ((child =
		ewd_list_next(EWL_CONTAINER(widget)->children)) != NULL)
	  {
		  if (!EWL_TEXT_ROW(child)->object)
			  continue;

		  evas_set_color(child->evas, EWL_TEXT_ROW(child)->object, r,
				 g, b, a);
	  }
}

void
ewl_text_get_color(Ewl_Widget * widget, int *r, int *g, int *b, int *a)
{
	CHECK_PARAM_POINTER("widget", widget);

	*r = EWL_TEXT(widget)->color.r;
	*g = EWL_TEXT(widget)->color.g;
	*b = EWL_TEXT(widget)->color.b;
	*a = EWL_TEXT(widget)->color.a;
}

void
ewl_text_get_letter_geometry(Ewl_Widget * widget,
			     int index, double *x, double *y, double *w,
			     double *h)
{
	Ewl_Widget *child = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	if (!EWL_CONTAINER(widget)->children ||
	    ewd_list_is_empty(EWL_CONTAINER(widget)->children))
		return;

	ewd_list_goto_first(EWL_CONTAINER(widget)->children);

	child = EWL_CONTAINER(widget)->children->first->data;
	evas_text_at(widget->evas, EWL_TEXT_ROW(child)->object, index,
		     x, y, w, h);
}

void
ewl_text_get_letter_geometry_at(Ewl_Widget * widget,
				int x,
				int y, double *tx, double *ty, double *tw,
				double *th)
{
	Ewl_Widget *child = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	if (!EWL_CONTAINER(widget)->children ||
	    ewd_list_is_empty(EWL_CONTAINER(widget)->children))
		return;

	ewd_list_goto_first(EWL_CONTAINER(widget)->children);

	child = EWL_CONTAINER(widget)->children->first->data;
	evas_text_at_position(child->evas, EWL_TEXT_ROW(child)->object,
			      x, y, &*tx, &*ty, &*tw, &*th);
}
