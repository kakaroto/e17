
#include <Ewl.h>


static void ewl_entry_init(Ewl_Widget * widget);
static void ewl_entry_realize(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_entry_show(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_entry_hide(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_entry_destroy(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_entry_configure(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_entry_key_down(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_entry_key_up(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_entry_mouse_down(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_entry_mouse_up(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_entry_select(Ewl_Widget * widget, Ewl_Callback * cb);
static void ewl_entry_unselect(Ewl_Widget * widget, Ewl_Callback * cb);

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

	entry = NEW(Ewl_Entry, 1);

	ewl_entry_init(EWL_WIDGET(entry));

	return EWL_WIDGET(entry);
}

static void
ewl_entry_init(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER("widget", widget);

	memset(EWL_ENTRY(widget), 0, sizeof(Ewl_Entry));

	EWL_WIDGET(widget)->type = EWL_WIDGET_ENTRY;

	ewl_callback_append(widget, EWL_CALLBACK_REALIZE,
                            ewl_entry_realize, NULL);
    ewl_callback_append(widget, EWL_CALLBACK_SHOW,
                            ewl_entry_show, NULL);
    ewl_callback_append(widget, EWL_CALLBACK_HIDE,
                            ewl_entry_hide, NULL);
    ewl_callback_append(widget, EWL_CALLBACK_DESTROY,
                            ewl_entry_destroy, NULL);
    ewl_callback_append(widget, EWL_CALLBACK_CONFIGURE,
                            ewl_entry_configure, NULL);
    ewl_callback_append(widget, EWL_CALLBACK_KEY_DOWN,
                            ewl_entry_key_down, NULL);
    ewl_callback_append(widget, EWL_CALLBACK_KEY_UP,
                            ewl_entry_key_up, NULL);
    ewl_callback_append(widget, EWL_CALLBACK_MOUSE_DOWN,
                            ewl_entry_mouse_down, NULL);
    ewl_callback_append(widget, EWL_CALLBACK_MOUSE_UP,
                            ewl_entry_mouse_up, NULL);
    ewl_callback_append(widget, EWL_CALLBACK_SELECT,
                            ewl_entry_select, NULL);
    ewl_callback_append(widget, EWL_CALLBACK_UNSELECT,
                            ewl_entry_unselect, NULL);

	widget->container.recursive = FALSE;

	EWL_ENTRY(widget)->text = ewl_text_new();
			
	EWL_ENTRY(widget)->font = strdup("borzoib");
	EWL_ENTRY(widget)->font_size = 8;

    EWL_OBJECT(widget)->current.w = 300;
    EWL_OBJECT(widget)->current.h = 35;
    EWL_OBJECT(widget)->minimum.w = 256;
    EWL_OBJECT(widget)->minimum.h = 130;
    EWL_OBJECT(widget)->maximum.w = 640;
    EWL_OBJECT(widget)->maximum.h = 20;
    EWL_OBJECT(widget)->request.w = 300;
    EWL_OBJECT(widget)->request.h = 35;
}

static void
ewl_entry_realize(Ewl_Widget * widget, Ewl_Callback * cb)
{
	char * image = NULL;

	CHECK_PARAM_POINTER("widget", widget);

	image = ewl_theme_ebit_get("entry", "default", "base");

	EWL_ENTRY(widget)->ebits_object = ebits_load(image);
	IF_FREE(image);
	ebits_add_to_evas(EWL_ENTRY(widget)->ebits_object, widget->evas);
	ebits_set_layer(EWL_ENTRY(widget)->ebits_object, widget->object.layer);

	image = ewl_theme_ebit_get("entry", "cursor", "base");

	EWL_ENTRY(widget)->cursor = ebits_load(image);
	IF_FREE(image);
	ebits_add_to_evas(EWL_ENTRY(widget)->cursor, widget->evas);
	ebits_set_layer(EWL_ENTRY(widget)->cursor, widget->object.layer +3);

	EWL_WIDGET(EWL_ENTRY(widget)->text)->parent = widget;
	EWL_WIDGET(EWL_ENTRY(widget)->text)->evas = widget->evas;
	EWL_OBJECT(EWL_ENTRY(widget)->text)->layer = EWL_OBJECT(widget)->layer +1;
	ewl_text_set_font_size(EWL_ENTRY(widget)->text, 10);
	ewl_widget_realize(EWL_WIDGET(EWL_ENTRY(widget)->text));

    ewl_fx_clip_box_create(widget);
    ewl_container_clip_box_create(widget);
    ewl_container_show_clip(widget);

    ebits_set_clip(EWL_ENTRY(widget)->ebits_object, widget->fx_clip_box);
    evas_set_clip(widget->evas, widget->fx_clip_box,
                        widget->parent->container.clip_box);
    evas_set_clip(widget->evas, widget->container.clip_box,widget->fx_clip_box);
    evas_show(widget->evas, widget->fx_clip_box);
    ebits_show(EWL_ENTRY(widget)->ebits_object);

    ebits_set_clip(EWL_ENTRY(widget)->cursor, widget->fx_clip_box);

    ewl_widget_show(EWL_ENTRY(widget)->text);

    evas_show(widget->evas, widget->container.clip_box);

    evas_set_color(widget->evas, widget->fx_clip_box, 255, 255, 255, 255);
}

static void
ewl_entry_show(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	evas_show(widget->evas, widget->container.clip_box);

	evas_show(widget->evas, widget->fx_clip_box);
}

static void
ewl_entry_hide(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	evas_hide(widget->evas, widget->fx_clip_box);
}

static void
ewl_entry_destroy(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_hide(EWL_ENTRY(widget)->ebits_object);
	ebits_unset_clip(EWL_ENTRY(widget)->ebits_object);
	ebits_free(EWL_ENTRY(widget)->ebits_object);

	ebits_hide(EWL_ENTRY(widget)->cursor);
	ebits_unset_clip(EWL_ENTRY(widget)->cursor);
	ebits_free(EWL_ENTRY(widget)->cursor);

	ewl_widget_destroy(EWL_ENTRY(widget)->text);

	evas_hide(widget->evas, widget->fx_clip_box);
	evas_unset_clip(widget->evas, widget->fx_clip_box);
	evas_del_object(widget->evas, widget->fx_clip_box);

	evas_hide(widget->evas, widget->container.clip_box);
	evas_unset_clip(widget->evas, widget->container.clip_box);
	evas_del_object(widget->evas, widget->container.clip_box);

	FREE(widget);
}

static void
ewl_entry_configure(Ewl_Widget * widget, Ewl_Callback * cb)
{
	int l, r, t, b;

	CHECK_PARAM_POINTER("widget", widget);

	EWL_OBJECT(widget)->current.x = EWL_OBJECT(widget)->request.x;
	EWL_OBJECT(widget)->current.y = EWL_OBJECT(widget)->request.y;
	EWL_OBJECT(widget)->current.w = EWL_OBJECT(widget)->request.w;
	EWL_OBJECT(widget)->current.h = EWL_OBJECT(widget)->maximum.h;

	if (EWL_OBJECT(widget)->realized)
	  {
	ebits_move(EWL_ENTRY(widget)->ebits_object, EWL_OBJECT(widget)->current.x,
									 			EWL_OBJECT(widget)->current.y);
	ebits_resize(EWL_ENTRY(widget)->ebits_object, EWL_OBJECT(widget)->current.w,
												EWL_OBJECT(widget)->current.h);

	ewl_fx_clip_box_resize(widget);

	ebits_get_insets(EWL_ENTRY(widget)->ebits_object, &l, &r, &t, &b);

	evas_move(widget->evas, widget->container.clip_box,
							EWL_OBJECT(widget)->current.x + l,
							EWL_OBJECT(widget)->current.y + t);
	evas_resize(widget->evas, widget->container.clip_box,
							EWL_OBJECT(widget)->current.w - (l+r),
							EWL_OBJECT(widget)->current.h - (t+b));

	l += 2;

	EWL_OBJECT(EWL_ENTRY(widget)->text)->request.x =
					EWL_OBJECT(widget)->current.x +l;
	EWL_OBJECT(EWL_ENTRY(widget)->text)->request.y =
					EWL_OBJECT(widget)->current.y +t;

	ewl_widget_configure(EWL_ENTRY(widget)->text);

	ewl_entry_set_cursor_pos(widget, EWL_ENTRY(widget)->cursor_pos);
	  }
}

static void
ewl_entry_key_down(Ewl_Widget * widget, Ewl_Callback * cb)
{
	Ev_Key_Down * ev;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	ev = cb->func_data;

	if (!strcmp(ev->key, "Left"))
	  {
		ewl_entry_move_cursor_to_left(widget);
	  }
	else if (!strcmp(ev->key, "Right"))
	  {
		ewl_entry_move_cursor_to_right(widget);
	  }
	else if (!strcmp(ev->key, "Home"))
	  {
		ewl_entry_move_cursor_to_start(widget);
	  }
	else if (!strcmp(ev->key, "End"))
	  {
		ewl_entry_move_cursor_to_end(widget);
	  }
	else if (!strcmp(ev->key, "BackSpace"))
	  {
		ewl_entry_delete_to_left(widget);
	  }
	else if (!strcmp(ev->key, "Delete"))
	  {
		ewl_entry_delete_to_right(widget);
	  }
	else if (!strcmp(ev->key, "Return"))
	  {
	  }
	else if (!strcmp(ev->key, "KP_Enter"))
	  {
	  }
	else if (ev->compose && strlen(ev->compose) == 1)
	  {
		ewl_entry_insert_text(widget, ev->compose);
	  }
}

static void
ewl_entry_key_up(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
}

static void
ewl_entry_mouse_down(Ewl_Widget * widget, Ewl_Callback * cb)
{
	Ev_Mouse_Down * ev;
	int x, y, w, h;
	int px, py, pw, ph;
	int i;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("cb", cb);

	ev = cb->func_data;

	if (ev->button == 1)
	  {
		ewl_text_get_letter_geometry_at(EWL_ENTRY(widget)->text,
								ev->x, ev->y, &x, &y, &w, &h);
		for (i=0;i<strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text);i++)
		  {
			ewl_text_get_letter_geometry(EWL_ENTRY(widget)->text, i,
										&px, &py, &pw, &ph);
			if (px == x && py == y && pw == w && ph == h)
			  {
				ewl_entry_set_cursor_pos(widget, i);
				return;
			  }
		  }
		ewl_entry_set_cursor_pos(widget,
					strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text));
	  }
	else if (ev->button == 2)
	  {
	  }
	else if (ev->button == 3)
	  {
	  }
}

static void
ewl_entry_mouse_up(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);
}

static void
ewl_entry_select(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_show(EWL_ENTRY(widget)->cursor);
}

static void
ewl_entry_unselect(Ewl_Widget * widget, Ewl_Callback * cb)
{
	CHECK_PARAM_POINTER("widget", widget);

	ebits_hide(EWL_ENTRY(widget)->cursor);
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

	ebits_get_insets(EWL_ENTRY(widget)->ebits_object, &l, &r, &t, &b);

	if (strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text) > pos)
		ewl_text_get_letter_geometry(EWL_WIDGET(EWL_ENTRY(widget)->text), pos,
								&x, &y, &w, 0);
	else if (!strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text))
	  {
		x = 0;
		y = 0;
		w = 3;
	  }
	else
	  {
		ewl_text_get_letter_geometry(EWL_WIDGET(EWL_ENTRY(widget)->text), pos-1,
								&x, &y, &w, 0);
		x += w + 1;
		w = 3;
	  }

	EWL_OBJECT(EWL_ENTRY(widget)->cursor)->request.w = w + 4;
	EWL_OBJECT(EWL_ENTRY(widget)->cursor)->request.h =
							EWL_OBJECT(widget)->current.h - t - b;

	ebits_move(EWL_ENTRY(widget)->cursor,
				EWL_OBJECT(widget)->current.x + x + l,
				EWL_OBJECT(widget)->current.y + y + t);
	ebits_resize(EWL_ENTRY(widget)->cursor,
				w + 4, EWL_OBJECT(widget)->current.h - t - b);

	EWL_ENTRY(widget)->cursor_pos = pos;
}

int
ewl_entry_get_cursor_pos(Ewl_Widget * widget)
{
	return EWL_ENTRY(widget)->cursor_pos;
}

void
ewl_entry_set_text(Ewl_Widget * widget, const char * text)
{
	CHECK_PARAM_POINTER("widget", widget);

	ewl_text_set_text(EWL_ENTRY(widget)->text, text);
}

char *
ewl_entry_get_text(Ewl_Widget * widget)
{
	CHECK_PARAM_POINTER_RETURN("widget", widget, NULL);

	return strdup(EWL_TEXT(EWL_ENTRY(widget)->text)->text);
}
