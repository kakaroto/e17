
#include <Ewl.h>


static void __ewl_entry_init(Ewl_Widget * w);
static void __ewl_entry_realize(Ewl_Widget * w, void *ev_data,
				void *user_data);
static void __ewl_entry_destroy(Ewl_Widget * w, void *ev_data,
				void *user_data);
static void __ewl_entry_configure(Ewl_Widget * w, void *ev_data,
				  void *user_data);
static void __ewl_entry_key_down(Ewl_Widget * w, void *ev_data,
				 void *user_data);
static void __ewl_entry_key_up(Ewl_Widget * w, void *ev_data,
			       void *user_data);
static void __ewl_entry_mouse_down(Ewl_Widget * w, void *ev_data,
				   void *user_data);
static void __ewl_entry_mouse_up(Ewl_Widget * w, void *ev_data,
				 void *user_data);
static void __ewl_entry_select(Ewl_Widget * w, void *ev_data,
			       void *user_data);
static void __ewl_entry_unselect(Ewl_Widget * w, void *ev_data,
				 void *user_data);
static void __ewl_entry_theme_update(Ewl_Widget * w, void *ev_data,
				void *user_data);

static void __ewl_entry_delete_to_left(Ewl_Widget * w);
static void __ewl_entry_delete_to_right(Ewl_Widget * w);

static void __ewl_entry_update_cursor(Ewl_Entry *e);
static void __ewl_entry_move_cursor_to_start(Ewl_Widget * w);
static void __ewl_entry_move_cursor_to_end(Ewl_Widget * w);
static void __ewl_entry_move_cursor_to_left(Ewl_Widget * w);
static void __ewl_entry_move_cursor_to_right(Ewl_Widget * w);
static void __ewl_entry_insert_text(Ewl_Widget * w, char *text);


Ewl_Widget *
ewl_entry_new()
{
	Ewl_Entry *e;

	DENTER_FUNCTION;

	e = NEW(Ewl_Entry, 1);
	if (!e)
		DRETURN_PTR(NULL);

	memset(EWL_ENTRY(e), 0, sizeof(Ewl_Entry));
	__ewl_entry_init(EWL_WIDGET(e));

	DRETURN_PTR(EWL_WIDGET(e));
}

static void
__ewl_entry_init(Ewl_Widget * widget)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("widget", widget);

	/*
	 * Initialize the inherited container fields
	 */
	ewl_widget_set_appearance(widget, "/appearance/entry/default");
	ewl_object_set_minimum_size(EWL_OBJECT(widget), 300, 16);
	ewl_object_set_maximum_size(EWL_OBJECT(widget), MAX_W(widget), 16);
	ewl_object_set_alignment(EWL_OBJECT(widget), EWL_ALIGNMENT_CENTER);

	/*
	 * Attach necessary callbacks
	 */
	ewl_callback_append(widget, EWL_CALLBACK_REALIZE,
			    __ewl_entry_realize, NULL);
	ewl_callback_prepend(widget, EWL_CALLBACK_DESTROY,
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
	ewl_callback_append(widget, EWL_CALLBACK_THEME_UPDATE,
			__ewl_entry_theme_update, NULL);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	__ewl_entry_theme_update(w, NULL, NULL);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry * e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	if (e->cursor)
	  {
		ebits_hide(e->cursor);
		ebits_unset_clip(e->cursor);
		ebits_free(e->cursor);
	  }

	FREE(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry *e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_entry_set_cursor_pos(w, e->cursor_pos);

	__ewl_entry_theme_update(w, NULL, NULL);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_key_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ev_Key_Down *ev;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ev = (Ev_Key_Down *) ev_data;

	if (!strcmp(ev->key, "Left")) {
		__ewl_entry_move_cursor_to_left(w);
	} else if (!strcmp(ev->key, "Right")) {
		__ewl_entry_move_cursor_to_right(w);
	} else if (!strcmp(ev->key, "Home")) {
		__ewl_entry_move_cursor_to_start(w);
	} else if (!strcmp(ev->key, "End")) {
		__ewl_entry_move_cursor_to_end(w);
	} else if (!strcmp(ev->key, "BackSpace")) {
		__ewl_entry_delete_to_left(w);
	} else if (!strcmp(ev->key, "Delete")) {
		__ewl_entry_delete_to_right(w);
	} else if (!strcmp(ev->key, "Return")) {
	} else if (!strcmp(ev->key, "KP_Enter")) {
	} else if (ev->compose && strlen(ev->compose) == 1) {
		__ewl_entry_insert_text(w, ev->compose);
	}

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_key_up(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;

	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ev_Mouse_Down *ev;
	Ewl_Entry *e;
	double x, y, wi, h;
	double px, py, pw, ph;
	int i;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ev = (Ev_Mouse_Down *) ev_data;
	e = EWL_ENTRY(w);

	if (ev->button == 1) {
		ewl_text_get_letter_geometry_at(EWL_TEXT(e), ev->x, ev->y,
				&x, &y, &wi, &h);
		for (i = 0; i < strlen(EWL_TEXT(e)->text); i++) {
			ewl_text_get_letter_geometry(EWL_TEXT(e), i, &px, &py,
						     &pw, &ph);
			if (px == x && py == y && pw == wi && ph == h) {
				ewl_entry_set_cursor_pos(w, i);
				DRETURN;
			}
		}
		ewl_entry_set_cursor_pos(w,
					 strlen(EWL_TEXT(e)->text));
	} else if (ev->button == 2) {
	} else if (ev->button == 3) {
	}

	printf("sdfhdfjksdfhjsdkdsf\n");

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_mouse_up(Ewl_Widget * w, void *ev_data,
		     void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_select(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry * e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ebits_show(e->cursor);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_unselect(Ewl_Widget * w, void *ev_data,
		     void *user_data)
{
	Ewl_Entry * e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ebits_hide(e->cursor);

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

	if (strlen(EWL_TEXT(w)->text) <= EWL_ENTRY(w)->cursor_pos)
		DRETURN;

	ewl_entry_set_cursor_pos(w, strlen(EWL_TEXT(w)-> text));

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

	if (strlen(EWL_TEXT(w)->text) <= EWL_ENTRY(w)->cursor_pos)
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

	if (!strlen(EWL_TEXT(w)->text) ||
			strlen(EWL_TEXT(w)->text) <=
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
	double x = 0, y = 0, wi = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ebits_get_insets(w->ebits_object, &l, &r, &t, &b);

	if (strlen(EWL_TEXT(e)->text) > pos)
		ewl_text_get_letter_geometry(EWL_TEXT(e), pos, &x, &y, &wi, 0);
	else if (!strlen(EWL_TEXT(e)->text)) {
		x = 0;
		y = 0;
		wi = 3;
	} else {
		ewl_text_get_letter_geometry(EWL_TEXT(e), pos - 1, &x, &y,
				&wi, 0);
		x += wi + 1;
		wi = 3;
	}

	if (e->cursor)
	  {
		ebits_move(e->cursor, CURRENT_X(w) + x + l, CURRENT_Y(w) + y + t);
		ebits_resize(e->cursor, wi + 4, CURRENT_H(w) - t - b);
	  }

	e->cursor_pos = pos;

	DLEAVE_FUNCTION;
}

int
ewl_entry_get_cursor_pos(Ewl_Widget * w)
{
	Ewl_Entry *e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, 0);

	e = EWL_ENTRY(w);

	DRETURN_INT(e->cursor_pos);
}

void
ewl_entry_set_text(Ewl_Widget * w, char *t)
{
	Ewl_Entry *e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_text_set_text(EWL_TEXT(e), t);

	DLEAVE_FUNCTION;
}

char *
ewl_entry_get_text(Ewl_Widget * w)
{
	Ewl_Entry *e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	e = EWL_ENTRY(w);

	DRETURN_PTR(strdup(EWL_TEXT(e)->text));
}

static void     
__ewl_entry_theme_update(Ewl_Widget * w, void *ev_data, void *user_data)
{                                          
	Ewl_Entry *e;
	char *v;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);
        
	/* Don't want to update anything if the widget isn't realized. */
	if (!REALIZED(w))
		DRETURN;

	e = EWL_ENTRY(w);

	v = ewl_theme_data_get(w, "cursor/visible");

	if (v && !strncasecmp(v, "yes", 3))
	  {
		char *i;

		i = ewl_theme_image_get(w, "cursor");

		if (i)
		  {
			if (e->cursor)
			  {
				ebits_hide(e->cursor);
				ebits_unset_clip(e->cursor);
				ebits_free(e->cursor);
			  }

			e->cursor = ebits_load(i);

			if (e->cursor)
			  {
				ebits_add_to_evas(e->cursor, w->evas);
				ebits_set_layer(e->cursor, LAYER(e->cursor) +2);
                                ebits_set_clip(e->cursor, w->fx_clip_box);

                                ebits_show(e->cursor);

			  }
		  }
	  }

	if (e)
	  {
		void * t;

		t = ewl_theme_data_get(w, "text/font");
		ewl_text_set_font(EWL_TEXT(e), t);
		t = ewl_theme_data_get(w, "text/style");
		ewl_text_set_style(EWL_TEXT(e), t);
		t = ewl_theme_data_get(w, "text/font_size");
		ewl_text_set_font_size(EWL_TEXT(e), (int)(t));
	  }

        /* Finally comfigure the widget to update the changes */
        ewl_widget_configure(w);

        DLEAVE_FUNCTION;
}

static void
__ewl_entry_update_cursor(Ewl_Entry *e)
{
}
