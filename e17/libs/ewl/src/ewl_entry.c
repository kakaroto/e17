
#include <Ewl.h>


static void __ewl_entry_init(Ewl_Widget * w);
static void __ewl_entry_realize(Ewl_Widget * w, void *ev_data,
				void *user_data);
static void __ewl_entry_show(Ewl_Widget * w, void *ev_data,
			     void *user_data);
static void __ewl_entry_hide(Ewl_Widget * w, void *ev_data,
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

	__ewl_entry_init(EWL_WIDGET(e));

	e->text = ewl_text_new();

	DRETURN_PTR(EWL_WIDGET(e));
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
	ewl_container_init(EWL_CONTAINER(widget), 300, 16,
			   EWL_FILL_POLICY_FILL, EWL_ALIGNMENT_CENTER);

	MAX_H(widget) = 16;
	MIN_H(widget) = 16;

	/*
	 * Attach necessary callbacks
	 */
	ewl_callback_append(widget, EWL_CALLBACK_REALIZE,
			    __ewl_entry_realize, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_SHOW, __ewl_entry_show, NULL);
	ewl_callback_append(widget, EWL_CALLBACK_HIDE, __ewl_entry_hide, NULL);
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
	ewl_callback_append(widget, EWL_CALLBACK_THEME_UPDATE,
			__ewl_entry_theme_update, NULL);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry * e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

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

	__ewl_entry_theme_update(w, NULL, NULL);

	e->text->parent = w;
	e->text->evas = w->evas;
	e->text->evas_window = w->evas_window;
	LAYER(e->text) = LAYER(w) + 1;
	ewl_text_set_alignment(e->text, EWL_ALIGNMENT_LEFT);
	ewl_widget_show(e->text);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_show(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry * e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	evas_show(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_hide(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	evas_hide(w->evas, w->fx_clip_box);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry * e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	if (w->ebits_object)
	  {
		ebits_hide(w->ebits_object);
		ebits_unset_clip(w->ebits_object);
		ebits_free(w->ebits_object);
	  }

	if (e->cursor)
	  {
		ebits_hide(e->cursor);
		ebits_unset_clip(e->cursor);
		ebits_free(e->cursor);
	  }

	ewl_widget_destroy(EWL_ENTRY(w)->text);

	if (w->fx_clip_box)
	  {
		evas_hide(w->evas, w->fx_clip_box);
		evas_unset_clip(w->evas, w->fx_clip_box);
		evas_del_object(w->evas, w->fx_clip_box);
	  }

	if (EWL_CONTAINER(w)->clip_box)
	  {
		evas_hide(w->evas, EWL_CONTAINER(w)->clip_box);
		evas_unset_clip(w->evas, EWL_CONTAINER(w)->clip_box);
		evas_del_object(w->evas, EWL_CONTAINER(w)->clip_box);
	  }

	ewl_callback_clear(w);

	ewl_theme_deinit_widget(w);

	FREE(w);

	DLEAVE_FUNCTION;
}

static void
__ewl_entry_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry *e;
	int l = 0, r = 0, t = 0, b = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_object_apply_requested(w);

	if (w->ebits_object)
	  {
		ebits_move(w->ebits_object, CURRENT_X(w), CURRENT_Y(w));
		ebits_resize(w->ebits_object, CURRENT_W(w), CURRENT_H(w));
		ebits_get_insets(w->ebits_object, &l, &r, &t, &b);
	  }

	if (w->fx_clip_box)
	  {
		evas_move(w->evas, w->fx_clip_box, CURRENT_X(w), CURRENT_Y(w));
		evas_resize(w->evas, w->fx_clip_box, CURRENT_W(w),CURRENT_H(w));
	  }

	REQUEST_X(e->text) = CURRENT_X(w) + l + 2;
	REQUEST_Y(e->text) = CURRENT_Y(w) + t;
	REQUEST_W(e->text) = CURRENT_W(w) - l - r;
	REQUEST_H(e->text) = CURRENT_H(w) - t - b;

	ewl_widget_configure(e->text);

	ewl_entry_set_cursor_pos(w, e->cursor_pos);

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
	int x, y, wi, h;
	int px, py, pw, ph;
	int i;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ev = (Ev_Mouse_Down *) ev_data;
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

	printf("sdfsfd\n");

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

	v = ewl_theme_data_get(w, "/appearance/entry/default/base/visible");

	if (v && !strncasecmp(v, "yes", 3)) {
		char *i;

		i = ewl_theme_image_get(w, "/appearance/entry/default/base");   

		if (i) {
                        w->ebits_object = ebits_load(i);
        
                        if (w->ebits_object) {
                                ebits_add_to_evas(w->ebits_object,
                                                  w->evas);
                                ebits_set_layer(w->ebits_object,
                                                EWL_OBJECT(w)->layer);
                
                                ebits_set_clip(w->ebits_object,
                                               w->fx_clip_box);
        
                                ebits_show(w->ebits_object);
                        }
                }
        }

	v = ewl_theme_data_get(w, "/appearance/entry/default/cursor/visible");

	if (v && !strncasecmp(v, "yes", 3))
	  {
		char *i;

		i = ewl_theme_image_get(w, "/appearance/entry/default/cursor");

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

	if (e->text)
	  {
		void * t;

		t = ewl_theme_data_get(w,"/appearance/entry/default/text/font");
		ewl_text_set_font(e->text, t);

		t = ewl_theme_data_get(w, "/appearance/entry/default/text/style");
		ewl_text_set_style(e->text, t);

		ewl_text_set_font_size(e->text, 8);
	  }

        /* Finally comfigure the widget to update the changes */
        ewl_widget_configure(w);

        DLEAVE_FUNCTION;
}
