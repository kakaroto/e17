
#include <Ewl.h>


static void ewl_entry_init(Ewl_Widget * widget);
static void ewl_entry_realize(Ewl_Widget * widget, void *event_data,
			      void *user_data);
static void ewl_entry_show(Ewl_Widget * widget, void *event_data,
			   void *user_data);
static void ewl_entry_hide(Ewl_Widget * widget, void *event_data,
			   void *user_data);
static void ewl_entry_destroy(Ewl_Widget * widget, void *event_data,
			      void *user_data);
static void ewl_entry_configure(Ewl_Widget * widget, void *event_data,
				void *user_data);
static void ewl_entry_key_down(Ewl_Widget * widget, void *event_data,
			       void *user_data);
static void ewl_entry_key_up(Ewl_Widget * widget, void *event_data,
			     void *user_data);
static void ewl_entry_mouse_down(Ewl_Widget * widget, void *event_data,
				 void *user_data);
static void ewl_entry_mouse_up(Ewl_Widget * widget, void *event_data,
			       void *user_data);
static void ewl_entry_select(Ewl_Widget * widget, void *event_data,
			     void *user_data);
static void ewl_entry_unselect(Ewl_Widget * widget, void *event_data,
			       void *user_data);

static void ewl_entry_delete_to_left(Ewl_Widget * widget);
static void ewl_entry_delete_to_right(Ewl_Widget * widget);

static void ewl_entry_move_cursor_to_start(Ewl_Widget * widget);
static void ewl_entry_move_cursor_to_end(Ewl_Widget * widget);
static void ewl_entry_move_cursor_to_left(Ewl_Widget * widget);
static void ewl_entry_move_cursor_to_right(Ewl_Widget * widget);
static void ewl_entry_insert_text(Ewl_Widget * widget, char *text);


Ewl_Widget *
ewl_entry_new()
{
	Ewl_Entry *entry = NULL;

	DENTER_FUNCTION;

	entry = NEW(Ewl_Entry, 1);

	ewl_entry_init(EWL_WIDGET(entry));

	DRETURN_PTR(EWL_WIDGET(entry));
}

static void
ewl_entry_init(Ewl_Widget * widget)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	memset(EWL_ENTRY(widget), 0, sizeof(Ewl_Entry));

	/*
	 * Initialize the inherited container fields
	 */
	ewl_container_init(EWL_CONTAINER(widget), EWL_WIDGET_ENTRY, 300, 35,
			   640, 130);

	/*
	 * Attach necessary callbacks
	 */
	ewl_callback_append(widget, EWL_CALLBACK_REALIZE,
			    ewl_entry_realize, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_SHOW, ewl_entry_show, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_HIDE, ewl_entry_hide, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_DESTROY,
			    ewl_entry_destroy, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_CONFIGURE,
			    ewl_entry_configure, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_KEY_DOWN,
			    ewl_entry_key_down, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_KEY_UP, ewl_entry_key_up,
			    NULL);
	ewl_callback_append(widget, EWL_CALLBACK_MOUSE_DOWN,
			    ewl_entry_mouse_down, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_MOUSE_UP,
			    ewl_entry_mouse_up, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_SELECT, ewl_entry_select,
			    NULL);
	ewl_callback_append(widget, EWL_CALLBACK_UNSELECT,
			    ewl_entry_unselect, NULL);

	EWL_ENTRY(widget)->text = ewl_text_new();

	EWL_ENTRY(widget)->font = strdup("borzoib");
	EWL_ENTRY(widget)->font_size = 8;

	DLEAVE_FUNCTION;
}

static void
ewl_entry_realize(Ewl_Widget * w, void *event_data, void *user_data)
{
	char *image = NULL;

	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("w", w);

	{
		Evas_Object *clip_box;

		clip_box = evas_add_rectangle(w->evas);
		evas_set_color(w->evas, clip_box, 255, 255, 255, 255);
		evas_set_layer(w->evas, clip_box, LAYER(w) - 1);
		if (w->parent && EWL_CONTAINER(w->parent)->clip_box)
			evas_set_clip(w->evas, clip_box,
				      EWL_CONTAINER(w->parent)->clip_box);
		w->fx_clip_box = clip_box;
		EWL_CONTAINER(w)->clip_box = clip_box;

	}

	image = ewl_theme_image_get(w, "/appearance/entry/default/base");

	w->ebits_object = ebits_load(image);
	IF_FREE(image);
	ebits_add_to_evas(w->ebits_object, w->evas);
	ebits_set_layer(w->ebits_object, w->object.layer);

	image = ewl_theme_image_get(w, "/appearance/entry/cursor/base");

	EWL_ENTRY(w)->cursor = ebits_load(image);
	IF_FREE(image);
	ebits_add_to_evas(EWL_ENTRY(w)->cursor, w->evas);
	ebits_set_layer(EWL_ENTRY(w)->cursor, w->object.layer + 3);

	EWL_WIDGET(EWL_ENTRY(w)->text)->parent = w;
	EWL_WIDGET(EWL_ENTRY(w)->text)->evas = w->evas;
	EWL_OBJECT(EWL_ENTRY(w)->text)->layer = EWL_OBJECT(w)->layer + 1;
	ewl_text_set_font_size(EWL_ENTRY(w)->text, 10);
	ewl_widget_realize(EWL_WIDGET(EWL_ENTRY(w)->text));

	ebits_show(w->ebits_object);

	ebits_set_clip(EWL_ENTRY(w)->cursor, w->fx_clip_box);

	ewl_widget_show(EWL_ENTRY(w)->text);

	evas_show(w->evas, EWL_CONTAINER(w)->clip_box);

	DLEAVE_FUNCTION;
}

static void
ewl_entry_show(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	evas_show(widget->evas, widget->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
ewl_entry_hide(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	evas_hide(widget->evas, widget->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
ewl_entry_destroy(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	ebits_hide(widget->ebits_object);
	ebits_unset_clip(widget->ebits_object);
	ebits_free(widget->ebits_object);

	ebits_hide(EWL_ENTRY(widget)->cursor);
	ebits_unset_clip(EWL_ENTRY(widget)->cursor);
	ebits_free(EWL_ENTRY(widget)->cursor);

	ewl_widget_destroy(EWL_ENTRY(widget)->text);

	evas_hide(widget->evas, widget->fx_clip_box);
	evas_unset_clip(widget->evas, widget->fx_clip_box);
	evas_del_object(widget->evas, widget->fx_clip_box);

	evas_hide(widget->evas, EWL_CONTAINER(widget)->clip_box);
	evas_unset_clip(widget->evas, EWL_CONTAINER(widget)->clip_box);
	evas_del_object(widget->evas, EWL_CONTAINER(widget)->clip_box);

	ewl_callback_clear(widget);

	ewl_theme_deinit_widget(widget);

	FREE(widget);

	DLEAVE_FUNCTION;
}

static void
ewl_entry_configure(Ewl_Widget * widget, void *event_data, void *user_data)
{
	int l, r, t, b;

	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	CURRENT_X(widget) = REQUEST_X(widget);
	CURRENT_Y(widget) = REQUEST_Y(widget);
	CURRENT_W(widget) = REQUEST_W(widget);
	CURRENT_H(widget) = REQUEST_H(widget);

	if (EWL_OBJECT(widget)->realized)
	  {
		  ebits_move(widget->ebits_object, CURRENT_X(widget),
			     CURRENT_Y(widget));
		  ebits_resize(widget->ebits_object, CURRENT_W(widget),
			       CURRENT_H(widget));

		  ewl_fx_clip_box_resize(widget);

		  ebits_get_insets(widget->ebits_object, &l, &r, &t, &b);

		  evas_move(widget->evas, EWL_CONTAINER(widget)->clip_box,
			    CURRENT_X(widget) + l, CURRENT_Y(widget) + t);
		  evas_resize(widget->evas, EWL_CONTAINER(widget)->clip_box,
			      CURRENT_W(widget) - (l + r),
			      CURRENT_H(widget) - (t + b));

		  l += 2;

		  REQUEST_X(EWL_ENTRY(widget)->text) = CURRENT_X(widget) + l;
		  REQUEST_Y(EWL_ENTRY(widget)->text) = CURRENT_Y(widget) + t;

		  ewl_widget_configure(EWL_ENTRY(widget)->text);

		  ewl_entry_set_cursor_pos(widget,
					   EWL_ENTRY(widget)->cursor_pos);
	  }

	DLEAVE_FUNCTION;
}

static void
ewl_entry_key_down(Ewl_Widget * widget, void *event_data, void *user_data)
{
	Ev_Key_Down *ev;

	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	ev = (Ev_Key_Down *) event_data;

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

	DLEAVE_FUNCTION;
}

static void
ewl_entry_key_up(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	DLEAVE_FUNCTION;
}

static void
ewl_entry_mouse_down(Ewl_Widget * widget, void *event_data, void *user_data)
{
	Ev_Mouse_Down *ev;
	double x, y, w, h;
	double px, py, pw, ph;
	int i;

	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	ev = (Ev_Mouse_Down *) event_data;

	if (ev->button == 1)
	  {
		  ewl_text_get_letter_geometry_at(EWL_ENTRY(widget)->text,
						  ev->x, ev->y, &x, &y, &w,
						  &h);
		  for (i = 0;
		       i < strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text);
		       i++)
		    {
			    ewl_text_get_letter_geometry(EWL_ENTRY(widget)->
							 text, i, &px, &py,
							 &pw, &ph);
			    if (px == x && py == y && pw == w && ph == h)
			      {
				      ewl_entry_set_cursor_pos(widget, i);
				      DRETURN;
			      }
		    }
		  ewl_entry_set_cursor_pos(widget,
					   strlen(EWL_TEXT
						  (EWL_ENTRY(widget)->text)->
						  text));
	  }
	else if (ev->button == 2)
	  {
	  }
	else if (ev->button == 3)
	  {
	  }

	DLEAVE_FUNCTION;
}

static void
ewl_entry_mouse_up(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	DLEAVE_FUNCTION;
}

static void
ewl_entry_select(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	ebits_show(EWL_ENTRY(widget)->cursor);

	DLEAVE_FUNCTION;
}

static void
ewl_entry_unselect(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	ebits_hide(EWL_ENTRY(widget)->cursor);

	DLEAVE_FUNCTION;
}

static void
ewl_entry_move_cursor_to_start(Ewl_Widget * widget)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	if (!EWL_ENTRY(widget)->cursor_pos)
		DRETURN;

	ewl_entry_set_cursor_pos(widget, 0);

	DLEAVE_FUNCTION;
}

static void
ewl_entry_move_cursor_to_end(Ewl_Widget * widget)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	if (strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text) <=
	    EWL_ENTRY(widget)->cursor_pos)
		DRETURN;

	ewl_entry_set_cursor_pos(widget,
				 strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->
					text));

	DLEAVE_FUNCTION;
}

static void
ewl_entry_move_cursor_to_left(Ewl_Widget * widget)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	if (!EWL_ENTRY(widget)->cursor_pos)
		DRETURN;

	ewl_entry_set_cursor_pos(widget, EWL_ENTRY(widget)->cursor_pos - 1);

	DLEAVE_FUNCTION;
}

static void
ewl_entry_move_cursor_to_right(Ewl_Widget * widget)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	if (strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text) <=
	    EWL_ENTRY(widget)->cursor_pos)
		DRETURN;

	ewl_entry_set_cursor_pos(widget, EWL_ENTRY(widget)->cursor_pos + 1);

	DLEAVE_FUNCTION;
}

static void
ewl_entry_delete_to_left(Ewl_Widget * widget)
{
	char *str = NULL;

	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	if (!ewl_entry_get_cursor_pos(widget))
		DRETURN;

	str = ewl_entry_get_text(widget);
	strcpy(&(str[ewl_entry_get_cursor_pos(widget) - 1]),
	       &(ewl_entry_get_text(widget)
		 [ewl_entry_get_cursor_pos(widget)]));
	ewl_entry_set_text(widget, str);
	ewl_entry_set_cursor_pos(widget,
				 ewl_entry_get_cursor_pos(widget) - 1);
	IF_FREE(str);

	DLEAVE_FUNCTION;
}

static void
ewl_entry_delete_to_right(Ewl_Widget * widget)
{
	char *str = NULL;

	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	if (!strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text) ||
	    strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text) <=
	    ewl_entry_get_cursor_pos(widget))
		DRETURN;

	str = ewl_entry_get_text(widget);
	strcpy(&(str[ewl_entry_get_cursor_pos(widget)]),
	       &ewl_entry_get_text(widget)[ewl_entry_get_cursor_pos(widget)
					   + 1]);
	ewl_entry_set_text(widget, str);
	ewl_entry_set_cursor_pos(widget, ewl_entry_get_cursor_pos(widget));
	IF_FREE(str);

	DLEAVE_FUNCTION;
}

static void
ewl_entry_insert_text(Ewl_Widget * widget, char *text)
{
	char *str = NULL;

	CHECK_PARAM_POINTER("widget", widget);
	CHECK_PARAM_POINTER("text", text);

	str = malloc(strlen(ewl_entry_get_text(widget)) + strlen(text) + 1);
	str[0] = 0;
	strncat(str, ewl_entry_get_text(widget),
		EWL_ENTRY(widget)->cursor_pos);
	strcat(str, text);
	strcat(str,
	       &ewl_entry_get_text(widget)[EWL_ENTRY(widget)->cursor_pos]);
	ewl_entry_set_text(widget, str);
	ewl_entry_set_cursor_pos(widget,
				 EWL_ENTRY(widget)->cursor_pos +
				 strlen(text));

	DLEAVE_FUNCTION;
}

void
ewl_entry_set_cursor_pos(Ewl_Widget * widget, int pos)
{
	int l = 0, r = 0, t = 0, b = 0;
	double x = 0, y = 0, w = 0;

	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	ebits_get_insets(widget->ebits_object, &l, &r, &t, &b);

	if (strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text) > pos)
		ewl_text_get_letter_geometry(EWL_WIDGET
					     (EWL_ENTRY(widget)->text),
					     pos, &x, &y, &w, 0);
	else if (!strlen(EWL_TEXT(EWL_ENTRY(widget)->text)->text))
	  {
		  x = 0;
		  y = 0;
		  w = 3;
	  }
	else
	  {
		  ewl_text_get_letter_geometry(EWL_WIDGET
					       (EWL_ENTRY(widget)->text),
					       pos - 1, &x, &y, &w, 0);
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

	DLEAVE_FUNCTION;
}

int
ewl_entry_get_cursor_pos(Ewl_Widget * widget)
{
	DENTER_FUNCTION;

	DRETURN_INT(EWL_ENTRY(widget)->cursor_pos);

	DLEAVE_FUNCTION;
}

void
ewl_entry_set_text(Ewl_Widget * widget, const char *text)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER("widget", widget);

	ewl_text_set_text(EWL_ENTRY(widget)->text, text);

	DLEAVE_FUNCTION;
}

char *
ewl_entry_get_text(Ewl_Widget * widget)
{
	DENTER_FUNCTION;

	CHECK_PARAM_POINTER_RETURN("widget", widget, NULL);

	DRETURN_PTR(strdup(EWL_TEXT(EWL_ENTRY(widget)->text)->text));
}
