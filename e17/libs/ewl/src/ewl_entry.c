
#include <Ewl.h>


static void ewl_entry_init(Ewl_Widget * widget, void * func_data);
static void ewl_entry_realize(Ewl_Widget * widget, void * func_data);
static void ewl_entry_show(Ewl_Widget * widget, void * func_data);
static void ewl_entry_hide(Ewl_Widget * widget, void * func_data);
static void ewl_entry_destroy(Ewl_Widget * widget, void * func_data);
static void ewl_entry_configure(Ewl_Widget * widget, void * func_data);
static void ewl_entry_key_down(Ewl_Widget * widget, void * func_data);
static void ewl_entry_key_up(Ewl_Widget * widget, void * func_data);
static void ewl_entry_mouse_down(Ewl_Widget * widget, void * func_data);
static void ewl_entry_mouse_up(Ewl_Widget * widget, void * func_data);
static void ewl_entry_focus_in(Ewl_Widget * widget, void * func_data);
static void ewl_entry_focus_out(Ewl_Widget * widget, void * func_data);

static void ewl_entry_delete_to_left(Ewl_Widget * widget);
static void ewl_entry_delete_to_right(Ewl_Widget * widget);

static void ewl_entry_move_cursor_to_start(Ewl_Widget * widget);
static void ewl_entry_move_cursor_to_end(Ewl_Widget * widget);
static void ewl_entry_move_cursor_to_left(Ewl_Widget * widget);
static void ewl_entry_move_cursor_to_right(Ewl_Widget * widget);
static void ewl_entry_insert_text(Ewl_Widget * widget, char * text);


Ewl_Widget *
ewl_entry_new()
{
	Ewl_Entry * entry = NULL;

	entry = malloc(sizeof(Ewl_Entry));

	if (!entry)
		return NULL;

	ewl_entry_init(EWL_WIDGET(entry), NULL);

	return EWL_WIDGET(entry);
}

static void
ewl_entry_init(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	memset(EWL_ENTRY(widget), 0, sizeof(Ewl_Entry));

	widget->container.recursive = FALSE;

	EWL_ENTRY(widget)->cursor = ewl_widget_new();
	EWL_ENTRY(widget)->selection = ewl_widget_new();
	EWL_ENTRY(widget)->text = ewl_text_new();

	ewl_callback_append(widget, Ewl_Callback_Realize,
                            ewl_entry_realize, NULL);
    ewl_callback_append(widget, Ewl_Callback_Show,
                            ewl_entry_show, NULL);
    ewl_callback_append(widget, Ewl_Callback_Hide,
                            ewl_entry_hide, NULL);
    ewl_callback_append(widget, Ewl_Callback_Destroy,
                            ewl_entry_destroy, NULL);
    ewl_callback_append(widget, Ewl_Callback_Configure,
                            ewl_entry_configure, NULL);
    ewl_callback_append(widget, Ewl_Callback_Key_Down,
                            ewl_entry_key_down, NULL);
    ewl_callback_append(widget, Ewl_Callback_Key_Up,
                            ewl_entry_key_up, NULL);
    ewl_callback_append(widget, Ewl_Callback_Mouse_Down,
                            ewl_entry_mouse_down, NULL);
    ewl_callback_append(widget, Ewl_Callback_Mouse_Up,
                            ewl_entry_mouse_up, NULL);
    ewl_callback_append(widget, Ewl_Callback_Focus_In,
                            ewl_entry_focus_in, NULL);
    ewl_callback_append(widget, Ewl_Callback_Focus_Out,
                            ewl_entry_focus_out, NULL);

	EWL_ENTRY(widget)->font = strdup("borzoib");
	EWL_ENTRY(widget)->font_size = 8;

    EWL_OBJECT(widget)->current.w = 130;
    EWL_OBJECT(widget)->current.h = 35;
    EWL_OBJECT(widget)->minimum.w = 256;
    EWL_OBJECT(widget)->minimum.h = 130;
    EWL_OBJECT(widget)->maximum.w = 640;
    EWL_OBJECT(widget)->maximum.h = 20;
    EWL_OBJECT(widget)->request.w = 130;
    EWL_OBJECT(widget)->request.h = 35;

	EWL_OBJECT(EWL_ENTRY(widget)->cursor)->current.w = 10;
	EWL_OBJECT(EWL_ENTRY(widget)->cursor)->current.h = 35;
}

static void
ewl_entry_realize(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewl_widget_set_ebit(widget, ewl_theme_ebit_get("entry", "default", "base"));
	EWL_ENTRY(widget)->cursor->evas = widget->evas;
	EWL_ENTRY(widget)->cursor->parent = widget;
	EWL_OBJECT(EWL_ENTRY(widget)->cursor)->layer = EWL_OBJECT(widget)->layer +3;
	ewl_widget_set_ebit(EWL_ENTRY(widget)->cursor,
						ewl_theme_ebit_get("entry", "cursor", "base"));
	ewl_widget_realize(EWL_ENTRY(widget)->cursor);
	EWL_ENTRY(widget)->selection->evas = widget->evas;
	EWL_ENTRY(widget)->selection->parent = widget;
	EWL_OBJECT(EWL_ENTRY(widget)->selection)->layer =
							EWL_OBJECT(widget)->layer +2;
	ewl_widget_set_ebit(EWL_ENTRY(widget)->selection,
						ewl_theme_ebit_get("entry", "selection", "base"));
	EWL_WIDGET(EWL_ENTRY(widget)->text)->parent = widget;
	EWL_WIDGET(EWL_ENTRY(widget)->text)->evas = widget->evas;
	EWL_OBJECT(EWL_ENTRY(widget)->text)->layer = EWL_OBJECT(widget)->layer +1;
	ewl_text_set_font(EWL_WIDGET(EWL_ENTRY(widget)->text),
				EWL_ENTRY(widget)->font);
	ewl_text_set_font_size(EWL_WIDGET(EWL_ENTRY(widget)->text),
				EWL_ENTRY(widget)->font_size);
	ewl_widget_realize(EWL_WIDGET(EWL_ENTRY(widget)->text));
}

static void
ewl_entry_show(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_show(widget->ebits_object);
	ewl_widget_show(EWL_WIDGET(EWL_ENTRY(widget)->text));
	ebits_show(EWL_ENTRY(widget)->cursor->ebits_object);
}

static void
ewl_entry_hide(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_hide(widget->ebits_object);
}

static void
ewl_entry_destroy(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_hide(widget->ebits_object);
	ebits_free(widget->ebits_object);
}

static void
ewl_entry_configure(Ewl_Widget * widget, void * func_data)
{
	int l, r, t, b;
	CHECK_PARAM_POINTER("widget", widget);

	EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
	EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;
	EWL_OBJECT(widget)->current.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->current.h = EWL_OBJECT(widget)->maximum.h;

	ebits_move(widget->ebits_object, EWL_OBJECT(widget)->current.x,
									 EWL_OBJECT(widget)->current.y);
	ebits_resize(widget->ebits_object, EWL_OBJECT(widget)->current.w,
									   EWL_OBJECT(widget)->current.h);

	ewl_widget_clip_box_resize(widget);

	ebits_get_insets(widget->ebits_object, &l, &r, &t, &b);

	l += 2;

	EWL_OBJECT(EWL_ENTRY(widget)->text)->request.x =
					EWL_OBJECT(widget)->current.x +l;
	EWL_OBJECT(EWL_ENTRY(widget)->text)->request.y =
					EWL_OBJECT(widget)->current.y +t;

	EWL_OBJECT(EWL_ENTRY(widget)->text)->request.y +=
						(EWL_OBJECT(EWL_ENTRY(widget)->text)->maximum.h / 2) -
						((EWL_OBJECT(widget)->current.h / 2) - t -b);

	ewl_widget_configure(EWL_WIDGET(EWL_ENTRY(widget)->text));

	EWL_OBJECT(EWL_ENTRY(widget)->cursor)->request.x =
			EWL_OBJECT(widget)->current.x +l;
	EWL_OBJECT(EWL_ENTRY(widget)->cursor)->request.y =
			EWL_OBJECT(widget)->current.y +t;

	ebits_move(EWL_ENTRY(widget)->cursor->ebits_object,
					EWL_OBJECT(EWL_ENTRY(widget)->cursor)->request.x,
					EWL_OBJECT(EWL_ENTRY(widget)->cursor)->request.y);
	ebits_resize(EWL_ENTRY(widget)->cursor->ebits_object,
					EWL_OBJECT(EWL_ENTRY(widget)->cursor)->current.w,
					EWL_OBJECT(EWL_ENTRY(widget)->cursor)->current.h);

	ewl_entry_set_cursor_pos(widget, EWL_ENTRY(widget)->cursor_pos);
}

static void
ewl_entry_key_down(Ewl_Widget * widget, void * func_data)
{
	Ev_Key_Down * ev;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	ev = func_data;

	if (!strcmp(ev->key, "Left")) {
		ewl_entry_move_cursor_to_left(widget);
	} else if (!strcmp(ev->key, "Right")) {
		ewl_entry_move_cursor_to_right(widget);
	} else if (!strcmp(ev->key, "Home")) {
		ewl_entry_move_cursor_to_start(widget);
	} else if (!strcmp(ev->key, "End")) {
		ewl_entry_move_cursor_to_end(widget);
	} else if (!strcmp(ev->key, "BackSpace")) {
		ewl_entry_delete_to_left(widget);
	} else if (!strcmp(ev->key, "Delete")) {
		ewl_entry_delete_to_right(widget);
	} else if (ev->compose && strlen(ev->compose) == 1) {
		ewl_entry_insert_text(widget, ev->compose);
	}
}

static void
ewl_entry_key_up(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
}

static void
ewl_entry_mouse_down(Ewl_Widget * widget, void * func_data)
{
	Ev_Mouse_Down * ev;
	int x, y, w, h;
	int px, py, pw, ph;
	int i;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("func_data", func_data);

	ev = func_data;

	if (ev->button == 1) {
		ewl_text_get_letter_geometry_at(EWL_ENTRY(widget)->text,
								ev->x, ev->y, &x, &y, &w, &h);
		for (i=0;i<strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text);i++) {
			ewl_text_get_letter_geometry(EWL_ENTRY(widget)->text, i,
										&px, &py, &pw, &ph);
			if (px == x && py == y && pw == w && ph == h) {
				ewl_entry_set_cursor_pos(widget, i);
				return;
			}
		}
		ewl_entry_set_cursor_pos(widget,
					strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text));
	} else if (ev->button == 2) {
	} else if (ev->button == 3) {
	}
}

static void
ewl_entry_mouse_up(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
}

static void
ewl_entry_focus_in(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
}

static void
ewl_entry_focus_out(Ewl_Widget * widget, void * func_data)
{
	CHECK_PARAM_POINTER("widget", widget);
}

static void
ewl_entry_move_cursor_to_start(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (!EWL_ENTRY(widget)->cursor_pos)
		return;

	ewl_entry_set_cursor_pos(widget, 0);
}

static void
ewl_entry_move_cursor_to_end(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text) <=
			EWL_ENTRY(widget)->cursor_pos)
		return;

	ewl_entry_set_cursor_pos(widget,
			strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text));
}

static void
ewl_entry_move_cursor_to_left(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (!EWL_ENTRY(widget)->cursor_pos)
		return;

	ewl_entry_set_cursor_pos(widget, EWL_ENTRY(widget)->cursor_pos -1);
}

static void
ewl_entry_move_cursor_to_right(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	if (strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text) <=
				EWL_ENTRY(widget)->cursor_pos)
		return;

	ewl_entry_set_cursor_pos(widget, EWL_ENTRY(widget)->cursor_pos +1);
}

static void
ewl_entry_delete_to_left(Ewl_Widget * widget)
{
	char * str = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	if (!ewl_entry_get_cursor_pos(widget))
		return;

	str = ewl_entry_get_text(widget);
	strcpy(&(str[ewl_entry_get_cursor_pos(widget) -1]),
		   &(ewl_entry_get_text(widget)[ewl_entry_get_cursor_pos(widget)]));
	ewl_entry_set_text(widget, str);
	ewl_entry_set_cursor_pos(widget, ewl_entry_get_cursor_pos(widget) -1);
	IF_FREE(str);
}

static void
ewl_entry_delete_to_right(Ewl_Widget * widget)
{
	char * str = NULL;

	CHECK_PARAM_POINTER("widget", widget);
	
	if (!strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text) ||
		strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text) <=
		ewl_entry_get_cursor_pos(widget))
		return;

	str = ewl_entry_get_text(widget);
	strcpy(&(str[ewl_entry_get_cursor_pos(widget)]),
		   &ewl_entry_get_text(widget)[ewl_entry_get_cursor_pos(widget) +1]);
	ewl_entry_set_text(widget, str);
	ewl_entry_set_cursor_pos(widget, ewl_entry_get_cursor_pos(widget));
	IF_FREE(str);
}

static void
ewl_entry_insert_text(Ewl_Widget * widget, char * text)
{
	char * str = NULL;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("text", text);

	str = malloc(strlen(ewl_entry_get_text(widget)) + strlen(text) +1);
	str [0] = 0;
	strncat(str, ewl_entry_get_text(widget),
			EWL_ENTRY(widget)->cursor_pos);
	strcat(str, text);
	strcat(str, &ewl_entry_get_text(widget)[EWL_ENTRY(widget)->cursor_pos]);
	ewl_entry_set_text(widget, str);
	ewl_entry_set_cursor_pos(widget,
		EWL_ENTRY(widget)->cursor_pos + strlen(text));
}

void
ewl_entry_set_cursor_pos(Ewl_Widget * widget, int pos)
{
	int l = 0, r = 0, t = 0, b = 0;
	int x = 0, y = 0, w = 0;

	CHECK_PARAM_POINTER("widget", widget);

	ebits_get_insets(widget->ebits_object, &l, &r, &t, &b);

	if (strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text) > pos)
		ewl_text_get_letter_geometry(EWL_WIDGET(EWL_ENTRY(widget)->text), pos,
								&x, &y, &w, 0);
	else if (!strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text)) {
		x = 0;
		y = 0;
		w = 3;
	} else {
		ewl_text_get_letter_geometry(EWL_WIDGET(EWL_ENTRY(widget)->text), pos-1,
								&x, &y, &w, 0);
		x += w + 1;
		w = 3;
	}

	EWL_OBJECT(EWL_ENTRY(widget)->cursor)->request.x =
							EWL_OBJECT(widget)->current.x + x + l;
	EWL_OBJECT(EWL_ENTRY(widget)->cursor)->request.y =
							EWL_OBJECT(widget)->current.y + y +t;
	EWL_OBJECT(EWL_ENTRY(widget)->cursor)->request.w = w + 4;
	EWL_OBJECT(EWL_ENTRY(widget)->cursor)->request.h =
							EWL_OBJECT(widget)->current.h - t - b;

	ebits_move(EWL_ENTRY(widget)->cursor->ebits_object,
				EWL_OBJECT(EWL_ENTRY(widget)->cursor)->request.x,
				EWL_OBJECT(EWL_ENTRY(widget)->cursor)->request.y);
	ebits_resize(EWL_ENTRY(widget)->cursor->ebits_object,
				EWL_OBJECT(EWL_ENTRY(widget)->cursor)->request.w,
				EWL_OBJECT(EWL_ENTRY(widget)->cursor)->request.h);

	EWL_ENTRY(widget)->cursor_pos = pos;
}

int
ewl_entry_get_cursor_pos(Ewl_Widget * widget)
{
	return EWL_ENTRY(widget)->cursor_pos;
}

void
ewl_entry_set_text(Ewl_Widget * widget, char * text)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewl_text_set_text(EWL_WIDGET(EWL_ENTRY(widget)->text), text);
}

char *
ewl_entry_get_text(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER_RETURN("widget", widget, NULL);

	return strdup(EWL_TEXT(EWL_ENTRY(widget)->text)->text);
}
