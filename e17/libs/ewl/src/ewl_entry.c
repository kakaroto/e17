
#include <Ewl.h>

void            ewl_entry_init(Ewl_Entry * e);
void            __ewl_entry_realize(Ewl_Widget * w, void *ev_data,
				    void *user_data);
void            __ewl_entry_configure(Ewl_Widget * w, void *ev_data,
				      void *user_data);
void            __ewl_entry_key_down(Ewl_Widget * w, void *ev_data,
				     void *user_data);
void            __ewl_entry_mouse_down(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void            __ewl_entry_mouse_move(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void            __ewl_entry_select(Ewl_Widget * w, void *ev_data,
				   void *user_data);
void            __ewl_entry_deselect(Ewl_Widget * w, void *ev_data,
				     void *user_data);
void            __ewl_entry_theme_update(Ewl_Widget * w, void *ev_data,
					 void *user_data);
void            __ewl_entry_move_cursor_to_left(Ewl_Widget * w);
void            __ewl_entry_move_cursor_to_right(Ewl_Widget * w);
void            __ewl_entry_move_cursor_to_home(Ewl_Widget * w);
void            __ewl_entry_move_cursor_to_end(Ewl_Widget * w);
void            __ewl_entry_insert_text(Ewl_Widget * w, char *s);
void            __ewl_entry_delete_to_left(Ewl_Widget * w);
void            __ewl_entry_delete_to_right(Ewl_Widget * w);

void
                __ewl_entry_update_selected_region(Ewl_Widget * w, void *user_data,
						   void *ev_data);

/**
 * ewl_entry_new - allocate and initialize a new entry widget
 *
 * Returns a newly allocated and initialized entry widget on success, NULL on
 * failure.
 */
Ewl_Widget     *
ewl_entry_new(void)
{
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	e = NEW(Ewl_Entry, 1);
	if (!e)
		return NULL;

	ZERO(e, Ewl_Entry, 1);

	e->text = ewl_text_new();
	e->cursor = ewl_cursor_new();
	e->selection = ewl_selection_new();

	ewl_entry_init(e);

	ewl_container_append_child(EWL_CONTAINER(e), e->text);
	ewl_container_append_child(EWL_CONTAINER(e), e->selection);
	ewl_container_append_child(EWL_CONTAINER(e), e->cursor);

	DRETURN_PTR(EWL_WIDGET(e), DLEVEL_STABLE);
}

/**
 * ewl_entry_set_text - set the text for an entry widget
 * @e: the entry widget to change the text
 * @t: the text to set for the entry widget
 *
 * Returns no value. Change the text of the entry widget @e to the string @t.
 */
void
ewl_entry_set_text(Ewl_Entry * e, char *t)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	w = EWL_WIDGET(e);

	ewl_text_set_text(EWL_TEXT(e->text), t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

char           *
ewl_entry_get_text(Ewl_Entry * e)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, NULL);

	w = EWL_WIDGET(e);

	DRETURN_PTR(ewl_text_get_text(EWL_TEXT(e->text)), DLEVEL_STABLE);
}

/**
 * ewl_entry_init - initialize an entry widget to default values
 * @e: the entry widget to initialize
 *
 * Returns no value. Initializes the entry widget @e to it's default values
 * and callbacks.
 */
void
ewl_entry_init(Ewl_Entry * e)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	w = EWL_WIDGET(e);

	ewl_container_init(EWL_CONTAINER(w), "/appearance/entry/default",
			   NULL, NULL);
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_FILL);
	ewl_object_set_minimum_size(EWL_OBJECT(w), 20, 20);
	ewl_object_set_maximum_size(EWL_OBJECT(w), 1 << 30, 20);

	w->recursive = FALSE;

	/*
	 * Attach necessary callback mechanisms 
	 */
	ewl_callback_append(w, EWL_CALLBACK_REALIZE, __ewl_entry_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_entry_configure, NULL);
	ewl_callback_append(w, EWL_CALLBACK_KEY_DOWN, __ewl_entry_key_down,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN, __ewl_entry_mouse_down,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE, __ewl_entry_mouse_move,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_SELECT, __ewl_entry_select, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESELECT, __ewl_entry_deselect,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_entry_theme_update, NULL);

	ewl_widget_set_appearance(e->text, "/appearance/entry/default/text");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/*
 * Draw the appearance information for the entry widget
 */
void
__ewl_entry_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_widget_show(e->text);
	ewl_widget_realize(e->cursor);
	ewl_widget_hide(e->cursor);

	ewl_widget_realize(e->selection);
	ewl_widget_hide(e->selection);

	/*
	 * if (w->ebits_object) {
	 * int ww, hh;
	 * 
	 * ebits_get_max_size(w->ebits_object, &ww, &hh);
	 * 
	 * ewl_object_set_maximum_size(EWL_OBJECT(w), ww, hh);
	 * }
	 */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry      *e;
	int             xx, yy, ww, hh;
	int             xx2, yy2, ww2, hh2;
	int             c_pos, l;
	int             ss, ee;
	int             sx, sy, sw, sh, ex, ey, ew, eh;
	char           *str;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	/******************************************************************/

	xx = CURRENT_X(w);
	xx += INSET_LEFT(w);

	yy = CURRENT_Y(w);
	yy += INSET_TOP(w);

	ewl_object_request_position(EWL_OBJECT(e->text), xx, yy);
	ewl_widget_configure(e->text);

	/******************************************************************/

	c_pos = ewl_cursor_get_position(e->cursor);

	ewl_text_get_letter_geometry(EWL_TEXT(e->text), --c_pos, &xx2,
				     &yy2, &ww2, &hh2);

	str = EWL_TEXT(e->text)->text;

	if (str && (l = strlen(str)) && c_pos >= l) {
		xx += CURRENT_W(e->text);
		ww = 5;
		hh = CURRENT_H(w) - INSET_TOP(w) - INSET_BOTTOM(w);
	} else {
		xx = xx2;
		yy = yy2;
		ww = ww2;
		hh = CURRENT_H(w) - INSET_TOP(w) - INSET_BOTTOM(w);
	}

	ewl_object_request_geometry(EWL_OBJECT(e->cursor), xx, yy, ww, hh);
	ewl_widget_configure(e->cursor);

	/******************************************************************/

	ewl_selection_get_covered(e->selection, &ss, &ee);

	if (ss >= 0) {
		xx = CURRENT_X(e->text);
		yy = CURRENT_Y(e->text);
		ww = 0;

		ewl_text_get_letter_geometry(EWL_TEXT(e->text), ss, &sx,
					     &sy, &sw, &sh);
		ewl_text_get_letter_geometry(EWL_TEXT(e->text), ss + ee,
					     &ex, &ey, &ew, &eh);

		xx += sx;
		ww += sw;

		if (ee > 0) {
			ww -= sw;
			ww += ex - sx;
			ww += ew;
		} else if (ee < 0) {
			xx -= sx;
			xx += ex;
			ww += (int) (sx - ex);

		}

		ewl_object_request_geometry(EWL_OBJECT(e->selection), xx, yy,
					    ww, hh);
		ewl_widget_configure(e->selection);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Handle key events to modify the text of the entry widget.
 */
void
__ewl_entry_key_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ecore_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ev = ev_data;

	if (!strcmp(ev->key, "Left"))
		__ewl_entry_move_cursor_to_left(w);
	else if (!strcmp(ev->key, "Right"))
		__ewl_entry_move_cursor_to_right(w);
	else if (!strcmp(ev->key, "Home"))
		__ewl_entry_move_cursor_to_home(w);
	else if (!strcmp(ev->key, "End"))
		__ewl_entry_move_cursor_to_end(w);
	else if (!strcmp(ev->key, "BackSpace"))
		__ewl_entry_delete_to_left(w);
	else if (!strcmp(ev->key, "Delete"))
		__ewl_entry_delete_to_right(w);
	else if (ev->compose)
		__ewl_entry_insert_text(w, ev->compose);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ecore_Event_Mouse_Down *ev;
	Ewl_Entry      *e;
	int             index = 0, len = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ev = ev_data;
	e = EWL_ENTRY(w);

	if (ev->x < CURRENT_X(e->text))
		index = 0;
	else if (ev->x > CURRENT_X(e->text) + CURRENT_W(e->text)) {
		char           *str;

		str = ewl_entry_get_text(EWL_ENTRY(w));

		if (str)
			len = index = strlen(str);
	} else
		index = ewl_text_get_index_at(EWL_TEXT(e->text), ev->x,
					      CURRENT_Y(e->text) +
					      (CURRENT_H(e->text) / 2));

	ewl_cursor_set_position(e->cursor, index + 1);

	ewl_widget_hide(e->selection);

	if (index == 0)
		ewl_selection_set_covered(e->selection, index, 0);
	else if (len > 0)
		ewl_selection_set_covered(e->selection, len - 1, 0);
	else
		ewl_selection_set_covered(e->selection, index, 0);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_mouse_move(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ecore_Event_Mouse_Move *ev;
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ev = ev_data;
	e = EWL_ENTRY(w);

	if (w->state & EWL_STATE_PRESSED) {
		int             ss, ee;
		int             index = 0;

		ewl_widget_show(e->selection);

		ewl_selection_get_covered(e->selection, &ss, &ee);
		if (ev->x > CURRENT_X(e->text) &&
		    ev->x < CURRENT_X(e->text) + CURRENT_W(e->text)) {
			index = ewl_text_get_index_at(EWL_TEXT(e->text),
						      (ev->x),
						      (CURRENT_Y(e->text) +
						       (CURRENT_H(e->text) /
							2)));

			ee = index - ss;

			ewl_selection_set_covered(e->selection, ss, ee);

			ewl_cursor_set_position(e->cursor, index + 1);

		} else if (ev->x < CURRENT_X(e->text)) {
		} else if (ev->x > CURRENT_X(e->text) + CURRENT_W(e->text)) {
			char           *str;

			str = ewl_entry_get_text(EWL_ENTRY(w));

			if (str)
				index = strlen(str);

			ewl_cursor_set_position(e->cursor, index + 1);
			ewl_selection_set_covered(e->selection, ss,
						  index - ss - 1);
		}

		ewl_widget_configure(w);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_select(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_widget_show(e->cursor);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_deselect(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_widget_hide(e->cursor);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_theme_update(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry      *e;
	char           *font, *style;
	int             size;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	font = ewl_theme_data_get_str(w, "/appearance/entry/default/text/font");
	size = ewl_theme_data_get_int(w,
				      "/appearance/entry/default/text/font_size");
	style = ewl_theme_data_get_str(w,
				       "/appearance/entry/default/text/style");

	if (font)
		ewl_text_set_font(EWL_TEXT(e->text), font);
	if (size)
		ewl_text_set_font_size(EWL_TEXT(e->text), size);
	if (style)
		ewl_text_set_style(EWL_TEXT(e->text), style);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_move_cursor_to_left(Ewl_Widget * w)
{
	Ewl_Entry      *e;
	int             pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	pos = ewl_cursor_get_position(e->cursor);

	if (pos > 1)
		--pos;

	ewl_cursor_set_position(e->cursor, pos);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_move_cursor_to_right(Ewl_Widget * w)
{
	Ewl_Entry      *e;
	char           *str;
	int             len = 0;
	int             pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	pos = ewl_cursor_get_position(e->cursor);
	str = ewl_entry_get_text(EWL_ENTRY(w));

	if (str)
		len = strlen(str);

	FREE(str);

	if (pos <= len)
		++pos;

	ewl_cursor_set_position(e->cursor, pos);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_move_cursor_to_home(Ewl_Widget * w)
{
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_cursor_set_position(e->cursor, 1);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_move_cursor_to_end(Ewl_Widget * w)
{
	Ewl_Entry      *e;
	char           *s;
	int             l = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	s = ewl_entry_get_text(EWL_ENTRY(w));

	if (s) {
		l = strlen(s);
		FREE(s);
	}

	ewl_cursor_set_position(e->cursor, ++l);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_insert_text(Ewl_Widget * w, char *s)
{
	Ewl_Entry      *e;
	char           *s2, *s3;
	int             l = 0, l2 = 0, p = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	s2 = ewl_entry_get_text(EWL_ENTRY(w));
	l = strlen(s);
	l2 = strlen(s2);

	p = ewl_cursor_get_position(e->cursor);

	s3 = NEW(char, l + 1 + l2);

	s3[0] = 0;
	strncat(s3, s2, p - 1);
	strcat(s3, s);
	strcat(s3, &(s2[p - 1]));

	ewl_entry_set_text(EWL_ENTRY(w), s3);

	FREE(s2);
	FREE(s3);

	p += l;
	ewl_cursor_set_position(e->cursor, p);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_delete_to_left(Ewl_Widget * w)
{
	Ewl_Entry      *e;
	char           *s, *s2;
	int             p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	p = ewl_cursor_get_position(e->cursor);

	if (p == 1)
		DRETURN(DLEVEL_STABLE);

	s2 = ewl_entry_get_text(EWL_ENTRY(w));
	if (!strlen(s2))
		DRETURN(DLEVEL_STABLE);

	s = ewl_entry_get_text(EWL_ENTRY(w));

	strcpy(&(s[p - 2]), &(s2[p - 1]));
	ewl_entry_set_text(EWL_ENTRY(w), s);
	p--;

	FREE(s);
	FREE(s2);

	ewl_cursor_set_position(e->cursor, p);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_delete_to_right(Ewl_Widget * w)
{
	Ewl_Entry      *e;
	char           *s, *s2;
	int             p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	p = ewl_cursor_get_position(e->cursor);

	s2 = ewl_entry_get_text(EWL_ENTRY(w));

	if (!strlen(s2) || p == strlen(s2) + 1)
		DRETURN(DLEVEL_STABLE);

	s = ewl_entry_get_text(EWL_ENTRY(w));

	strcpy(&(s[p - 1]), &(s2[p]));
	ewl_entry_set_text(EWL_ENTRY(w), s);

	FREE(s);

	ewl_cursor_set_position(e->cursor, p);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
