
#include <Ewl.h>


static void __ewl_entry_init(Ewl_Widget * widget);
static void __ewl_entry_realize(Ewl_Widget * widget, void *event_data,
				void *user_data);
static void __ewl_entry_show(Ewl_Widget * widget, void *event_data,
			     void *user_data);
static void __ewl_entry_hide(Ewl_Widget * widget, void *event_data,
			     void *user_data);
static void __ewl_entry_destroy(Ewl_Widget * widget, void *event_data,
				void *user_data);
static void __ewl_entry_configure(Ewl_Widget * widget, void *event_data,
				  void *user_data);
static void __ewl_entry_key_down(Ewl_Widget * widget, void *event_data,
				 void *user_data);
static void __ewl_entry_key_up(Ewl_Widget * widget, void *event_data,
			       void *user_data);
static void __ewl_entry_mouse_down(Ewl_Widget * widget, void *event_data,
				   void *user_data);
static void __ewl_entry_mouse_up(Ewl_Widget * widget, void *event_data,
				 void *user_data);
static void __ewl_entry_select(Ewl_Widget * widget, void *event_data,
			       void *user_data);
static void __ewl_entry_unselect(Ewl_Widget * widget, void *event_data,
				 void *user_data);

static void __ewl_entry_delete_to_left(Ewl_Widget * widget);
static void __ewl_entry_delete_to_right(Ewl_Widget * widget);

static void __ewl_entry_move_cursor_to_start(Ewl_Widget * widget);
static void __ewl_entry_move_cursor_to_end(Ewl_Widget * widget);
static void __ewl_entry_move_cursor_to_left(Ewl_Widget * widget);
static void __ewl_entry_move_cursor_to_right(Ewl_Widget * widget);
static void __ewl_entry_insert_text(Ewl_Widget * widget, char *text);


Ewl_Widget *
ewl_entry_new()
{
	Ewl_Entry *entry = NULL;

	DENTER_FUNCTION;

	entry = NEW(Ewl_Entry, 1);

	__ewl_entry_init(EWL_WIDGET(entry));

	DRETURN_PTR(EWL_WIDGET(entry));
}

static void
__ewl_entry_init(Ewl_Widget * widget)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("widget", widget);

	memset(EWL_ENTRY(widget), 0, sizeof(Ewl_Entry));

	/*
	 * Initialize the inherited container fields
	 */
	ewl_container_init(EWL_CONTAINER(widget), 300, 35,
			   EWL_FILL_POLICY_NORMAL, EWL_ALIGNMENT_CENTER);

	/*
	 * Attach necessary callbacks
	 */
	ewl_callback_append(widget, EWL_CALLBACK_REALIZE,
			    __ewl_entry_realize, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_SHOW, __ewl_entry_show,
			    NULL);
	ewl_callback_append(widget, EWL_CALLBACK_HIDE, __ewl_entry_hide,
			    NULL);
	ewl_callback_append(widget, EWL_CALLBACK_DESTROY,
			    __ewl_entry_destroy, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_CONFIGURE,
			    __ewl_entry_configure, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_KEY_DOWN,
			    __ewl_entry_key_down, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_KEY_UP,
			    __ewl_entry_key_up, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_entry_mouse_down, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_MOUSE_UP,
			    __ewl_entry_mouse_up, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_SELECT,
			    __ewl_entry_select, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_UNSELECT,
			    __ewl_entry_unselect, NULL);

	EWL_ENTRY(widget)->text = ewl_text_new();

	EWL_ENTRY(widget)->font = strdup("borzoib");
	EWL_ENTRY(widget)->font_size = 8;

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_realize(Ewl_Widget * w, void *event_data, void *user_data)
{
	char *image = NULL;

	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

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
__ewl_entry_show(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("widget", widget);

	evas_show(widget->evas, widget->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_hide(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("widget", widget);

	evas_hide(widget->evas, widget->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_destroy(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("widget", widget);

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
__ewl_entry_configure(Ewl_Widget * w, void *event_data, void *user_data)
{
	Ewl_Entry *e;
	int l, r, t, b;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	CURRENT_X(w) = REQUEST_X(w);
	CURRENT_Y(w) = REQUEST_Y(w);
	CURRENT_W(w) = REQUEST_W(w);
	CURRENT_H(w) = REQUEST_H(w);

	if (EWL_OBJECT(w)->realized) {
		ebits_move(w->ebits_object, CURRENT_X(w), CURRENT_Y(w));
		ebits_resize(w->ebits_object, CURRENT_W(w), CURRENT_H(w));

		ewl_fx_clip_box_resize(w);

		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

		evas_move(w->evas, EWL_CONTAINER(w)->clip_box,
			  CURRENT_X(w) + l, CURRENT_Y(w) + t);
		evas_resize(w->evas, EWL_CONTAINER(w)->clip_box,
			    CURRENT_W(w) - (l + r),
			    CURRENT_H(w) - (t + b));

		l += 2;

		REQUEST_X(e->text) = CURRENT_X(w) + l;
		REQUEST_Y(e->text) = CURRENT_Y(w) + t;

		ewl_widget_configure(e->text);

		ewl_entry_set_cursor_pos(w, e->cursor_pos);
	}

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_key_down(Ewl_Widget * widget, void *event_data,
		     void *user_data)
{
	Ev_Key_Down *ev;

	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("widget", widget);

	ev = (Ev_Key_Down *) event_data;

	if (!strcmp(ev->key, "Left")) {
		__ewl_entry_move_cursor_to_left(widget);
	} else if (!strcmp(ev->key, "Right")) {
		__ewl_entry_move_cursor_to_right(widget);
	} else if (!strcmp(ev->key, "Home")) {
		__ewl_entry_move_cursor_to_start(widget);
	} else if (!strcmp(ev->key, "End")) {
		__ewl_entry_move_cursor_to_end(widget);
	} else if (!strcmp(ev->key, "BackSpace")) {
		__ewl_entry_delete_to_left(widget);
	} else if (!strcmp(ev->key, "Delete")) {
		__ewl_entry_delete_to_right(widget);
	} else if (!strcmp(ev->key, "Return")) {
	} else if (!strcmp(ev->key, "KP_Enter")) {
	} else if (ev->compose && strlen(ev->compose) == 1) {
		__ewl_entry_insert_text(widget, ev->compose);
	}

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_key_up(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("widget", widget);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_mouse_down(Ewl_Widget * w, void *event_data, void *user_data)
{
	Ev_Mouse_Down *ev;
	Ewl_Entry *e;
	int x, y, wi, h;
	int px, py, pw, ph;
	int i;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ev = (Ev_Mouse_Down *) event_data;
	e = EWL_ENTRY(w);

	if (ev->button == 1) {
		ewl_text_get_letter_geometry_at(e->text,
						ev->x, ev->y, &x, &y, &wi,
						&h);
		for (i = 0; i < strlen(EWL_TEXT(e->text)->text); i++) {
			ewl_text_get_letter_geometry(e->text, i, &px, &py,
						     &pw, &ph);
			if (px == x && py == y && pw == wi && ph == h) {
				ewl_entry_set_cursor_pos(w, i);
				DRETURN;
			}
		}
		ewl_entry_set_cursor_pos(w,
					 strlen(EWL_TEXT(e->text)->text));
	} else if (ev->button == 2) {
	} else if (ev->button == 3) {
	}

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_mouse_up(Ewl_Widget * widget, void *event_data,
		     void *user_data)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("widget", widget);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_select(Ewl_Widget * widget, void *event_data, void *user_data)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("widget", widget);

	ebits_show(EWL_ENTRY(widget)->cursor);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_unselect(Ewl_Widget * widget, void *event_data,
		     void *user_data)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("widget", widget);

	ebits_hide(EWL_ENTRY(widget)->cursor);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_move_cursor_to_start(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (!EWL_ENTRY(w)->cursor_pos)
		DRETURN;

	ewl_entry_set_cursor_pos(w, 0);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_move_cursor_to_end(Ewl_Widget * w)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

	if (strlen(EWL_TEXT(EWL_ENTRY(w)->text)->text) <=
	    EWL_ENTRY(w)->cursor_pos)
		DRETURN;

	ewl_entry_set_cursor_pos(w,
				 strlen(EWL_TEXT(EWL_ENTRY(w)->text)->
					text));

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_move_cursor_to_left(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (!EWL_ENTRY(w)->cursor_pos)
		DRETURN;

	ewl_entry_set_cursor_pos(w, EWL_ENTRY(w)->cursor_pos - 1);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_move_cursor_to_right(Ewl_Widget * w)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (strlen(EWL_TEXT(EWL_ENTRY(w)->text)->text) <=
	    EWL_ENTRY(w)->cursor_pos)
		DRETURN;

	ewl_entry_set_cursor_pos(w, EWL_ENTRY(w)->cursor_pos + 1);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_delete_to_left(Ewl_Widget * w)
{
	char *str = NULL;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (!ewl_entry_get_cursor_pos(w))
		DRETURN;

	str = ewl_entry_get_text(w);
	strcpy(&(str[ewl_entry_get_cursor_pos(w) - 1]),
	       &(ewl_entry_get_text(w)
		 [ewl_entry_get_cursor_pos(w)]));
	ewl_entry_set_text(w, str);
	ewl_entry_set_cursor_pos(w, ewl_entry_get_cursor_pos(w) - 1);
	IF_FREE(str);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_delete_to_right(Ewl_Widget * w)
{
	char *str = NULL;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	if (!strlen(EWL_TEXT(EWL_ENTRY(w)->text)->text) ||
	    strlen(EWL_TEXT(EWL_ENTRY(w)->text)->text) <=
	    ewl_entry_get_cursor_pos(w))
		DRETURN;

	str = ewl_entry_get_text(w);
	strcpy(&(str[ewl_entry_get_cursor_pos(w)]), &ewl_entry_get_text(w)
	       [ewl_entry_get_cursor_pos(w) + 1]);
	ewl_entry_set_text(w, str);
	ewl_entry_set_cursor_pos(w, ewl_entry_get_cursor_pos(w));
	IF_FREE(str);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_insert_text(Ewl_Widget * w, char *text)
{
	char *str = NULL;

	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("text", text);

	str = malloc(strlen(ewl_entry_get_text(w)) + strlen(text) + 1);
	str[0] = 0;
	strncat(str, ewl_entry_get_text(w), EWL_ENTRY(w)->cursor_pos);
	strcat(str, text);
	strcat(str, &ewl_entry_get_text(w)[EWL_ENTRY(w)->cursor_pos]);
	ewl_entry_set_text(w, str);
	ewl_entry_set_cursor_pos(w,
				 EWL_ENTRY(w)->cursor_pos + strlen(text));

	DLEAVE_FUNCTION;
}

void
ewl_entry_set_cursor_pos(Ewl_Widget * w, int pos)
{
	Ewl_Entry *e;
	int l = 0, r = 0, t = 0, b = 0;
	int x = 0, y = 0, wi = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

	if (strlen(EWL_TEXT(e->text)->text) > pos)
		ewl_text_get_letter_geometry(EWL_WIDGET(e->text),
					     pos, &x, &y, &wi, 0);
	else if (!strlen(EWL_TEXT(e->text)->text)) {
		x = 0;
		y = 0;
		wi = 3;
	} else {
		ewl_text_get_letter_geometry(EWL_WIDGET(e->text),
					     pos - 1, &x, &y, &wi, 0);
		x += wi + 1;
		wi = 3;
	}

	EWL_OBJECT(e->cursor)->request.w = wi + 4;
	EWL_OBJECT(e->cursor)->request.h =
	    EWL_OBJECT(w)->current.h - t - b;

	ebits_move(e->cursor,
		   EWL_OBJECT(w)->current.x + x + l,
		   EWL_OBJECT(w)->current.y + y + t);
	ebits_resize(e->cursor, wi + 4, EWL_OBJECT(w)->current.h - t - b);

	e->cursor_pos = pos;

	DLEAVE_FUNCTION;
}

int
ewl_entry_get_cursor_pos(Ewl_Widget * w)
{
	Ewl_Entry *e;

	DENTER_FUNCTION;

	e = EWL_ENTRY(w);

	DRETURN_INT(e->cursor_pos);
}

void
ewl_entry_set_text(Ewl_Widget * w, const char *t)
{
	Ewl_Entry *e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_text_set_text(e->text, t);

	DLEAVE_FUNCTION;
}

char *
ewl_entry_get_text(Ewl_Widget * w)
{
	Ewl_Entry *e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	e = EWL_ENTRY(w);

	DRETURN_PTR(strdup(EWL_TEXT(e->text)->text));
}
