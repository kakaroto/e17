
#include <Ewl.h>

struct _ewl_text_row {
	Ewl_Widget			widget;
	Evas_Object			object;
	char			  * text;
	struct {
		int				r, g, b, a;
	} color;
};

typedef struct _ewl_text_row Ewl_Text_Row;

#define EWL_TEXT_ROW(row) ((Ewl_Text_Row *) row)

static void		ewl_text_init(Ewl_Widget * widget);
static void 	ewl_text_realize(Ewl_Widget * widget, void * func_data);
static void 	ewl_text_show(Ewl_Widget * widget, void * func_data);
static void 	ewl_text_hide(Ewl_Widget * widget, void * func_data);
static void 	ewl_text_destroy(Ewl_Widget * widget, void * func_data);
static void		ewl_text_configure(Ewl_Widget * widget, void * func_data);

static Ewl_Text_Row * ewl_text_row_new();
static void		ewl_text_row_init(Ewl_Text_Row * row);
static void		ewl_text_row_realize(Ewl_Widget * widget, void * func_data);
static void		ewl_text_row_show(Ewl_Widget * widget, void * func_data);
static void		ewl_text_row_hide(Ewl_Widget * widget, void * func_data);
static void		ewl_text_row_destroy(Ewl_Widget * widget, void * func_data);
static void		ewl_text_row_configure(Ewl_Widget * widget, void * func_data);
static void		ewl_text_row_free(void * func_data);
static void		ewl_text_row_set_text(Ewl_Widget * widget, const char * text);


Ewl_Widget *
ewl_text_new()
{
	Ewl_Text * text = NULL;

	text = NEW(Ewl_Text, 1);

	ewl_text_init(EWL_WIDGET(text));

	text->font = strdup("borzoib.ttf");
	text->font_size = 15;
	text->text = strdup("");
	text->color.r = 0;
	text->color.g = 0;
	text->color.b = 0;
	text->color.a = 200;

	return EWL_WIDGET(text);
}

static void
ewl_text_init(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	memset(EWL_TEXT(widget), 0, sizeof(Ewl_Text));

	ewl_callback_append(widget, Ewl_Callback_Realize,
											ewl_text_realize, NULL);
	ewl_callback_append(widget, Ewl_Callback_Show,
											ewl_text_show, NULL);
	ewl_callback_append(widget, Ewl_Callback_Hide,
											ewl_text_hide, NULL);
	ewl_callback_append(widget, Ewl_Callback_Destroy,
											ewl_text_destroy, NULL);
	ewl_callback_append(widget, Ewl_Callback_Configure,
											ewl_text_configure, NULL);

	widget->container.recursive = FALSE;
	widget->container.free_cb = ewl_text_row_free;

    EWL_OBJECT(widget)->current.w = 10;
    EWL_OBJECT(widget)->current.h = 10;
    EWL_OBJECT(widget)->maximum.w = 2024;
    EWL_OBJECT(widget)->maximum.h = 2024;
    EWL_OBJECT(widget)->minimum.w = 10;
    EWL_OBJECT(widget)->minimum.h = 10;
    EWL_OBJECT(widget)->request.w = 10;
    EWL_OBJECT(widget)->request.h = 10;
}

static void
ewl_text_realize(Ewl_Widget * widget, void * func_data)
{
	Ewl_Text_Row * row;

	CHECK_PARAM_POINTER("widget", widget);

	ewl_container_new(widget);

	ewd_list_goto_first(widget->container.children);

	while ((row = ewd_list_next(widget->container.children)) != NULL) {
		ewl_callback_call(EWL_WIDGET(row), Ewl_Callback_Realize);
	}
}

static void
ewl_text_show(Ewl_Widget * widget, void * func_data)
{
	Ewl_Text_Row * row;

	ewl_container_set_clip(widget);

	if (!widget->container.children ||
			ewd_list_is_empty(widget->container.children))
		return;

	ewd_list_goto_first(widget->container.children);

	while ((row = ewd_list_next(widget->container.children)) != NULL) {
		ewl_callback_call(EWL_WIDGET(row), Ewl_Callback_Show);
	}
}

static void
ewl_text_hide(Ewl_Widget * widget, void * func_data)
{
	Ewl_Text_Row * row;

	CHECK_PARAM_POINTER("widget", widget);

    if (!widget->container.children ||
            ewd_list_is_empty(widget->container.children))
        return;

    ewd_list_goto_first(widget->container.children);

    while ((row = ewd_list_next(widget->container.children)) != NULL) {
        ewl_callback_call(EWL_WIDGET(row), Ewl_Callback_Hide);
    }

	return;
	func_data = NULL;
}

static void
ewl_text_destroy(Ewl_Widget * widget, void * func_data)
{
	Ewl_Text_Row * row = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	if (widget->container.children ||
			!ewd_list_is_empty(widget->container.children)) {

		while ((row = ewd_list_next(widget->container.children)) != NULL) {
			ewl_widget_destroy(EWL_WIDGET(row));
		}
	}

	IF_FREE(EWL_TEXT(widget)->text);
	IF_FREE(EWL_TEXT(widget)->font);
	FREE(EWL_TEXT(widget));
}

static void
ewl_text_configure(Ewl_Widget * widget, void * func_data)
{
    Ewl_Text_Row * row;
	int y, h = 0, w = 0;

	CHECK_PARAM_POINTER("widget", widget);

	EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
	EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;

    if (!widget->container.children ||
            ewd_list_is_empty(widget->container.children))
        return;

    ewd_list_goto_first(widget->container.children);

	y = EWL_OBJECT(widget)->current.y;

    while ((row = ewd_list_next(widget->container.children)) != NULL) {
		EWL_OBJECT(row)->request.x = EWL_OBJECT(widget)->current.x;
		EWL_OBJECT(row)->request.y = y;
        ewl_callback_call(EWL_WIDGET(row), Ewl_Callback_Configure);
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

	ewl_container_clip_box_resize(widget);

    return;
    func_data = NULL;
}

static Ewl_Text_Row *
ewl_text_row_new()
{
	Ewl_Text_Row * row = NULL;

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

    ewl_callback_append(EWL_WIDGET(row), Ewl_Callback_Realize,
                                            ewl_text_row_realize, NULL);
    ewl_callback_append(EWL_WIDGET(row), Ewl_Callback_Show,
                                            ewl_text_row_show, NULL);
    ewl_callback_append(EWL_WIDGET(row), Ewl_Callback_Hide,
                                            ewl_text_row_hide, NULL);
    ewl_callback_append(EWL_WIDGET(row), Ewl_Callback_Destroy,
                                            ewl_text_row_destroy, NULL);
    ewl_callback_append(EWL_WIDGET(row), Ewl_Callback_Configure,
                                            ewl_text_row_configure, NULL);

    EWL_OBJECT(row)->current.w = 10;
    EWL_OBJECT(row)->current.h = 10;
    EWL_OBJECT(row)->maximum.w = 2024;
    EWL_OBJECT(row)->maximum.h = 2024;
    EWL_OBJECT(row)->minimum.w = 10;
    EWL_OBJECT(row)->minimum.h = 10;
    EWL_OBJECT(row)->request.w = 10;
    EWL_OBJECT(row)->request.h = 10;
}

static void
ewl_text_row_realize(Ewl_Widget * widget, void * func_data)
{
	Ewl_Text * text;

	CHECK_PARAM_POINTER("widget", widget);

	text = EWL_TEXT(widget->parent);

	EWL_TEXT_ROW(widget)->object = evas_add_text(widget->evas,
			text->font, text->font_size, EWL_TEXT_ROW(widget)->text);

	evas_move(widget->evas, EWL_TEXT_ROW(widget)->object,
		EWL_OBJECT(widget)->request.x, EWL_OBJECT(widget)->request.y);

	EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
	EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;

	evas_set_layer(widget->evas, EWL_TEXT_ROW(widget)->object,
								EWL_OBJECT(widget)->layer);
	evas_set_color(widget->evas, EWL_TEXT_ROW(widget)->object,
					text->color.r, text->color.g, text->color.b, text->color.a);

	return;
	func_data = NULL;
}

static void
ewl_text_row_show(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	evas_show(widget->evas, EWL_TEXT_ROW(widget)->object);

	evas_set_clip(widget->evas, EWL_TEXT_ROW(widget)->object,
					widget->parent->container.clip_box);

	return;
}

static void
ewl_text_row_hide(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	evas_hide(widget->evas, EWL_TEXT_ROW(widget)->object);
}

static void
ewl_text_row_destroy(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	evas_del_object(widget->evas, EWL_TEXT_ROW(widget)->object);
	
	IF_FREE(EWL_TEXT_ROW(widget)->text);
	FREE(widget);
}

static void
ewl_text_row_configure(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	evas_move(widget->evas, EWL_TEXT_ROW(widget)->object,
			EWL_OBJECT(widget)->request.x, EWL_OBJECT(widget)->request.y);

	EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
	EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;
	EWL_OBJECT(widget)->current.w = evas_get_text_width(widget->evas,
											EWL_TEXT_ROW(widget)->object);
	EWL_OBJECT(widget)->current.h = evas_get_text_height(widget->evas,
											EWL_TEXT_ROW(widget)->object);
}

static void
ewl_text_row_free(void * func_data)
{
	Ewl_Text_Row * row = NULL;

	CHECK_PARAM_POINTER("func_data", func_data);

	row = func_data;

	IF_FREE(row->text);
	evas_del_object(EWL_WIDGET(row)->evas, row->object);
}

static void
ewl_text_row_set_text(Ewl_Widget * widget, const char * text)
{
	CHECK_PARAM_POINTER("widget", widget);
}

void
ewl_text_set_text(Ewl_Widget * widget, const char * text)
{
	Ewl_Text_Row * row;
	char * string = NULL, * temp = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	if (widget->container.children)
		ewd_list_clear(widget->container.children);

	if (!strlen(EWL_TEXT(widget)->text))
		ewl_container_show_clip(widget);

	IF_FREE(EWL_TEXT(widget)->text);
	EWL_TEXT(widget)->text = strdup(text);

	if (!strlen(text)) {
		ewl_container_hide_clip(widget);
		return;
	}

	/* This is bad bad!!!!!! REALLY BAD hack! FIXME FIXME */
	string = strdup(text);
	temp = strchr(string, '\n');

	while (temp && *temp) {
		char * line;
		row = ewl_text_row_new();

		*temp = '\0';
		line = strdup(string);
		string = temp + 1;
		temp = strchr(string, '\n');

		row->text = strdup(line);

		EWL_WIDGET(row)->parent = widget;
		EWL_WIDGET(row)->evas = widget->evas;
		EWL_WIDGET(row)->container.clip_box = widget->container.clip_box;

		if (EWL_OBJECT(widget)->realized)
			ewl_widget_realize(EWL_WIDGET(row));
		if (EWL_OBJECT(widget)->visible)
			ewl_widget_show(EWL_WIDGET(row));

		ewl_container_append_child(widget, EWL_WIDGET(row));
	}

	row = ewl_text_row_new();

	row->text = strdup(string);
	EWL_WIDGET(row)->parent = widget;
	EWL_WIDGET(row)->evas = widget->evas;
	EWL_WIDGET(row)->container.clip_box = widget->container.clip_box;

	if (EWL_OBJECT(widget)->realized)
		ewl_widget_realize(EWL_WIDGET(row));
	if (EWL_OBJECT(widget)->visible)
		ewl_widget_show(EWL_WIDGET(row));

	ewl_container_append_child(widget, EWL_WIDGET(row));

	ewl_widget_configure(widget->parent);
}

char *
ewl_text_get_text(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER_RETURN("widget", widget, NULL);

	return strdup(EWL_TEXT(widget)->text);
}

void
ewl_text_set_font(Ewl_Widget * widget, char * font)
{
	Ewl_Widget * child = NULL;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("font", font);

	if (!strcmp(font, EWL_TEXT(widget)->font))
		return;

	IF_FREE(EWL_TEXT(widget)->font);

	EWL_TEXT(widget)->font = strdup(font);

	if (!widget->container.children)
		return;

	ewd_list_goto_first(widget->container.children);

	while ((child = ewd_list_next(widget->container.children)) != NULL) {
		if (!EWL_TEXT_ROW(child)->object)
			continue;

		evas_set_font(child->evas, EWL_TEXT_ROW(child)->object,
					EWL_TEXT(widget)->font, EWL_TEXT(widget)->font_size);
	}

	ewl_widget_configure(widget);
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
    Ewl_Widget * child = NULL;

    CHECK_PARAM_POINTER("widget", widget);
    CHECK_PARAM_POINTER("size", size);

	if (EWL_TEXT(widget)->font_size == size)
		return;

    EWL_TEXT(widget)->font_size = size;

	if (!widget->container.children)
		return;

    ewd_list_goto_first(widget->container.children);

    while ((child = ewd_list_next(widget->container.children)) != NULL) {
        if (!EWL_TEXT_ROW(child)->object)
            continue;

        evas_set_font(child->evas, EWL_TEXT_ROW(child)->object,
                    EWL_TEXT(widget)->font, EWL_TEXT(widget)->font_size);
    }

	ewl_widget_configure(widget->parent);
}

void
ewl_text_get_font_size(Ewl_Widget * widget, int * size)
{
	CHECK_PARAM_POINTER("widget", widget);

	*size = EWL_TEXT(widget)->font_size;
}

void
ewl_text_set_color(Ewl_Widget * widget, int r, int g, int b, int a)
{
	Ewl_Widget * child = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	EWL_TEXT(widget)->color.r = r;
	EWL_TEXT(widget)->color.g = g;
	EWL_TEXT(widget)->color.b = b;
	EWL_TEXT(widget)->color.a = a;

	if (!widget->container.children)
		return;

	ewd_list_goto_first(widget->container.children);

	while ((child = ewd_list_next(widget->container.children)) != NULL) {
		if (!EWL_TEXT_ROW(child)->object)
			continue;

		evas_set_color(child->evas, EWL_TEXT_ROW(child)->object, r, g, b, a);
	}
}

void
ewl_text_get_color(Ewl_Widget * widget, int * r, int * g, int * b, int * a)
{
	CHECK_PARAM_POINTER("widget", widget);

	*r = EWL_TEXT(widget)->color.r;
	*g = EWL_TEXT(widget)->color.g;
	*b = EWL_TEXT(widget)->color.b;
	*a = EWL_TEXT(widget)->color.a;
}
void
ewl_text_get_letter_geometry(Ewl_Widget * widget,
							int index, int * x, int * y, int * w, int * h)
{
	Ewl_Widget * child = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	if (!widget->container.children ||
		ewd_list_is_empty(widget->container.children))
		return;

	ewd_list_goto_first(widget->container.children);

	child = widget->container.children->first->data;
	evas_text_at(widget->evas, EWL_TEXT_ROW(child)->object, index,
						 &*x, &*y, &*w, &*h);
}

void
ewl_text_get_letter_geometry_at(Ewl_Widget * widget,
						   int x, int y, int * tx, int *ty, int * tw, int *th)
{
	Ewl_Widget * child = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	if (!widget->container.children ||
		ewd_list_is_empty(widget->container.children))
		return;

	ewd_list_goto_first(widget->container.children);

	child = widget->container.children->first->data;
	evas_text_at_position(child->evas, EWL_TEXT_ROW(child)->object,
						x, y, &*tx, &*ty, &*tw, &*th);
}
