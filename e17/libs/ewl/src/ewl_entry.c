
#include <Ewl.h>

/**
 * @param text: the initial text to display in the widget
 * @return Returns a new entry widget on success, NULL on failure.
 * @brief Allocate and initialize a new entry widget
 */
Ewl_Widget     *ewl_entry_new(char *text)
{
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	e = NEW(Ewl_Entry, 1);
	if (!e)
		return NULL;

	ewl_entry_init(e, text);

	DRETURN_PTR(EWL_WIDGET(e), DLEVEL_STABLE);
}

/**
 * @param e: the entry widget to initialize
 * @param text: the initial text to display in the widget
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize an entry widget to default values
 *
 * Initializes the entry widget @a e to it's default values and callbacks.
 */
int ewl_entry_init(Ewl_Entry * e, char *text)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, FALSE);

	w = EWL_WIDGET(e);

	if (!ewl_container_init(EWL_CONTAINER(w), "entry",
				ewl_entry_child_add_cb,
				ewl_entry_child_resize_cb, NULL))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FLAG_FILL_HSHRINK |
			EWL_FLAG_FILL_HFILL);
	ewl_container_intercept_callback(EWL_CONTAINER(w), EWL_CALLBACK_SELECT);
	ewl_container_intercept_callback(EWL_CONTAINER(w),
					 EWL_CALLBACK_DESELECT);

	e->text = ewl_text_new(text);
	ewl_container_append_child(EWL_CONTAINER(e), e->text);
	ewl_callback_append(e->text, EWL_CALLBACK_CONFIGURE,
			    ewl_entry_configure_text_cb, e);
	ewl_widget_show(e->text);

	e->cursor = ewl_cursor_new();
	ewl_container_append_child(EWL_CONTAINER(e), e->cursor);

	ewl_cursor_set_base(EWL_CURSOR(e->cursor), 1);

	/*
	 * Attach necessary callback mechanisms 
	 */
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_entry_configure_cb, NULL);

	ewl_callback_append(w, EWL_CALLBACK_SELECT, ewl_entry_select_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESELECT, ewl_entry_deselect_cb,
			    NULL);

	ewl_entry_set_editable(e, TRUE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param e: the entry widget to change the text
 * @param t: the text to set for the entry widget
 * @return Returns no value.
 * @brief Set the text for an entry widget
 *
 * Change the text of the entry widget @a e to the string @a t.
 */
void ewl_entry_set_text(Ewl_Entry * e, char *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	ewl_text_set_text(EWL_TEXT(e->text), t);
	ewl_cursor_set_base(EWL_CURSOR(e->cursor), 1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the entry widget to retrieve the text
 * @return Returns the entry text on success, NULL on failure.
 * @brief Get the text from an entry widget
 */
char           *ewl_entry_get_text(Ewl_Entry * e)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, NULL);

	w = EWL_WIDGET(e);

	DRETURN_PTR(ewl_text_get_text(EWL_TEXT(e->text)), DLEVEL_STABLE);
}

/**
 * @param e: then entry to change
 * @param edit: a boolean value indicating the ability to edit the entry
 * @return Returns no value.
 * @brief Change the ability to edit the text in an entry
 */
void
ewl_entry_set_editable(Ewl_Entry *e, unsigned int edit)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	if (e->editable == edit)
		DRETURN(DLEVEL_STABLE);

	w = EWL_WIDGET(e);

	e->editable = edit;

	if (edit) {
		ewl_callback_append(w, EWL_CALLBACK_KEY_DOWN,
				ewl_entry_key_down_cb, NULL);
		ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
				ewl_entry_mouse_down_cb, NULL);
		ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
				ewl_entry_mouse_move_cb, NULL);
	}
	else {
		ewl_callback_del(w, EWL_CALLBACK_KEY_DOWN,
				ewl_entry_key_down_cb);
		ewl_callback_del(w, EWL_CALLBACK_MOUSE_DOWN,
				ewl_entry_mouse_down_cb);
		ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE,
				ewl_entry_mouse_move_cb);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Layout the text and cursor within the entry widget.
 */
void ewl_entry_configure_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry      *e;
	int             xx, yy, ww, hh;

	DENTER_FUNCTION(DLEVEL_STABLE);

	e = EWL_ENTRY(w);

	/*
	 * The contents are clipped starting at these positions
	 */
	xx = CURRENT_X(w);
	yy = CURRENT_Y(w);
	ww = CURRENT_W(w);
	hh = CURRENT_H(w);

	/*
	 * First position the text to a known base position.
	 */
	ewl_object_request_geometry(EWL_OBJECT(e->text), xx - e->offset, yy,
			ww, hh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_configure_text_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry      *e;
	int             xx, yy, ww, hh;
	int             c_spos, c_epos, base, l;
	int             sx = 0, sy = 0, ex = 0, ey = 0, dx = 0;
	unsigned int    ew = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	e = EWL_ENTRY(user_data);

	l = ewl_text_get_length(EWL_TEXT(w));
	if (!l)
		DRETURN(DLEVEL_STABLE);

	/*
	 * The contents are clipped starting at these positions
	 */
	xx = CURRENT_X(e);
	yy = CURRENT_Y(e);
	ww = CURRENT_W(e);
	hh = CURRENT_H(e);

	c_spos = ewl_cursor_get_start_position(EWL_CURSOR(e->cursor));
	c_epos = ewl_cursor_get_end_position(EWL_CURSOR(e->cursor));
	base = ewl_cursor_get_base_position(EWL_CURSOR(e->cursor));

	if (c_spos > l) {
		ex = sx = ewl_object_get_current_x(EWL_OBJECT(w)) +
			ewl_object_get_current_w(EWL_OBJECT(w));
		ew = 5;
	} else {

		/*
		 * Now position the cursor based on the current position in the
		 * text.
		 */
		ewl_text_get_letter_geometry(EWL_TEXT(w), --c_spos, &sx,
					     &sy, NULL, NULL);

		ewl_text_get_letter_geometry(EWL_TEXT(w), --c_epos, &ex,
					     &ey, &ew, NULL);
		base--;
	}

	/*
	 * D'oh, get the hell out of here, the entry is way too small to do
	 * anything useful.
	 */
	if (ew > ww)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Scroll the text to fit the cursor position.
	 */
	if ((c_spos == base) && ((int)(ex + ew) > (int)(xx + ww))) {
		dx -= (int)((ex + ew) - (xx + ww));
	}
	else if ((c_epos == base) && (sx < xx)) {
		dx = xx - sx;
	}

	if (e->offset < 0)
		e->offset = 0;

	if (dx)
		ewl_object_request_geometry(EWL_OBJECT(w),
				ewl_object_get_current_x(EWL_OBJECT(w)) + dx,
				CURRENT_Y(e), CURRENT_W(e), hh);

	ew = (ex + ew) - sx;
	ewl_object_request_geometry(EWL_OBJECT(e->cursor), sx + dx, yy,
			ew, hh);

	e->offset -= dx;
}

/*
 * Handle key events to modify the text of the entry widget.
 */
void ewl_entry_key_down_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry *e;
	char *evd = NULL;
	Ewl_Event_Key_Down *ev;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	ev = ev_data;

	if (!strcmp(ev->keyname, "Left"))
		ewl_entry_move_cursor_to_left(e);
	else if (!strcmp(ev->keyname, "Right"))
		ewl_entry_move_cursor_to_right(e);
	else if (!strcmp(ev->keyname, "Home"))
		ewl_entry_move_cursor_to_home(e);
	else if (!strcmp(ev->keyname, "End"))
		ewl_entry_move_cursor_to_end(e);
	else if (!strcmp(ev->keyname, "BackSpace"))
		ewl_entry_delete_to_left(e);
	else if (!strcmp(ev->keyname, "Delete"))
		ewl_entry_delete_to_right(e);
	else if (!strcmp(ev->keyname, "Return") || !strcmp(ev->keyname,
				"KP_Return") || !strcmp(ev->keyname, "Enter")
				|| !strcmp(ev->keyname, "KP_Enter")) {
		evd = ewl_text_get_text(EWL_TEXT(e->text));
		ewl_callback_call_with_event_data(w, EWL_CALLBACK_VALUE_CHANGED,
				EWL_TEXT(w)->text);
		FREE(evd);
	}
	else if (ev->keyname) {
		ewl_entry_insert_text(e, ev->keyname);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Place the cursor appropriately on a mouse down event.
 */
void ewl_entry_mouse_down_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Event_Mouse_Down *ev;
	Ewl_Entry      *e;
	int             index = 0, len = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ev = ev_data;
	e = EWL_ENTRY(w);

	len = ewl_text_get_length(EWL_TEXT(e->text));
	if (ev->x < CURRENT_X(e->text))
		index = 0;
	else if (ev->x > CURRENT_X(e->text) + CURRENT_W(e->text)) {
		index = len;
	}
	else {
		index = ewl_text_get_index_at(EWL_TEXT(e->text), ev->x,
					      CURRENT_Y(e->text) +
					      (CURRENT_H(e->text) / 2));
		index++;
	}

	if (index > len + 1)
		index = len + 1;
	ewl_cursor_set_base(EWL_CURSOR(e->cursor), index);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Hilight text when the mouse moves when the button is pressed
 */
void ewl_entry_mouse_move_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             index = 0;
	Ewl_Event_Mouse_Move *ev;
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ev = ev_data;
	e = EWL_ENTRY(w);

	/*
	 * Check for the button pressed state, otherwise, do nothing.
	 */
	if (!(ewl_object_has_state(EWL_OBJECT(e), EWL_FLAG_STATE_PRESSED)))
		DRETURN(DLEVEL_STABLE);

	if (ev->x < CURRENT_X(e->text))
		index = 0;
	else if (ev->x > CURRENT_X(e->text) + CURRENT_W(e->text))
		index = ewl_text_get_length(EWL_TEXT(e->text));
	else {
		index = ewl_text_get_index_at(EWL_TEXT(e->text), ev->x,
				      (CURRENT_Y(e->text) +
				       (CURRENT_H(e->text) / 2)));
	}

	index++;

	ewl_cursor_select_to(EWL_CURSOR(e->cursor), index);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_select_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_widget_show(e->cursor);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_deselect_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_widget_hide(e->cursor);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_move_cursor_to_left(Ewl_Entry * e)
{
	int             pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	pos = ewl_cursor_get_start_position(EWL_CURSOR(e->cursor));

	if (pos > 1)
		--pos;

	ewl_cursor_set_base(EWL_CURSOR(e->cursor), pos);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_move_cursor_to_right(Ewl_Entry * e)
{
	char           *str;
	int             len = 0;
	int             pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	pos = ewl_cursor_get_end_position(EWL_CURSOR(e->cursor));
	str = ewl_entry_get_text(e);

	len = strlen(str);

	FREE(str);

	if (pos <= len)
		++pos;

	ewl_cursor_set_base(EWL_CURSOR(e->cursor), pos);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Position the cursor at the beginning of the widget. This is internal, so the
 * parameter is not checked.
 */
void ewl_entry_move_cursor_to_home(Ewl_Entry * e)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	ewl_cursor_set_base(EWL_CURSOR(e->cursor), 1);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Position the cursor at the end of the widget. This is internal, so the
 * parameter is not checked.
 */
void ewl_entry_move_cursor_to_end(Ewl_Entry * e)
{
	char           *s;
	int             l = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = ewl_entry_get_text(e);

	if (s) {
		l = strlen(s);
		FREE(s);
	}

	ewl_cursor_set_base(EWL_CURSOR(e->cursor), l + 1);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_insert_text(Ewl_Entry * e, char *s)
{
	char           *s2, *s3;
	int             l = 0, l2 = 0, sp = 0, ep = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	s2 = ewl_entry_get_text(e);
	l = strlen(s);
	l2 = strlen(s2);

	sp = ewl_cursor_get_start_position(EWL_CURSOR(e->cursor));

	s3 = NEW(char, l + 1 + l2);
	if (!s3) {
		FREE(s2);
		DRETURN(DLEVEL_STABLE);
	}

	s3[0] = 0;
	strncat(s3, s2, sp - 1);
	strcat(s3, s);

	ep = ewl_cursor_get_end_position(EWL_CURSOR(e->cursor));
	if (!ep || (sp != ep))
		ep++;
	strcat(s3, &(s2[ep - 1]));

	ewl_entry_set_text(e, s3);
	ewl_cursor_set_base(EWL_CURSOR(e->cursor), ep);

	FREE(s2);
	FREE(s3);

	sp++;
	ewl_cursor_set_base(EWL_CURSOR(e->cursor), sp);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_delete_to_left(Ewl_Entry * e)
{
	char           *s;
	unsigned int    sp, ep;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	sp = ewl_cursor_get_start_position(EWL_CURSOR(e->cursor));
	ep = ewl_cursor_get_end_position(EWL_CURSOR(e->cursor));

	s = ewl_entry_get_text(e);
	if (!strlen(s) || (sp == ep && sp < 2)) {
		FREE(s);
		DRETURN(DLEVEL_STABLE);
	}

	if (sp != ep) {
		sp++;
		ep++;
	}

	strcpy(&(s[sp - 2]), &(s[ep - 1]));
	ewl_entry_set_text(e, s);
	sp--;

	FREE(s);

	ewl_cursor_set_base(EWL_CURSOR(e->cursor), sp);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_delete_to_right(Ewl_Entry * e)
{
	char           *s;
	int             sp, ep;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	sp = ewl_cursor_get_start_position(EWL_CURSOR(e->cursor));
	ep = ewl_cursor_get_end_position(EWL_CURSOR(e->cursor));

	s = ewl_entry_get_text(e);

	if (!strlen(s) || ep == strlen(s) + 1)
		DRETURN(DLEVEL_STABLE);

	strcpy(&(s[sp - 1]), &(s[ep]));
	ewl_entry_set_text(e, s);

	FREE(s);

	ewl_cursor_set_base(EWL_CURSOR(e->cursor), sp);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_child_add_cb(Ewl_Container * c, Ewl_Widget * w)
{
	Ewl_Entry *e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	e = EWL_ENTRY(c);

	if (e->text == w) {
		ewl_object_set_preferred_size(EWL_OBJECT(c),
			   ewl_object_get_preferred_w(EWL_OBJECT(w)),
			   ewl_object_get_preferred_h(EWL_OBJECT(w)));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_child_resize_cb(Ewl_Container * entry, Ewl_Widget * w, int size,
			 Ewl_Orientation o)
{
	Ewl_Object *text;

	DENTER_FUNCTION(DLEVEL_STABLE);

	text = EWL_OBJECT(EWL_ENTRY(entry)->text);

	if (w != EWL_WIDGET(text))
		DRETURN(DLEVEL_STABLE);

	if (o == EWL_ORIENTATION_HORIZONTAL)
		ewl_object_set_preferred_w(EWL_OBJECT(entry),
			   ewl_object_get_preferred_w(text));
	else
		ewl_object_set_preferred_h(EWL_OBJECT(entry),
			   ewl_object_get_preferred_h(text));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
