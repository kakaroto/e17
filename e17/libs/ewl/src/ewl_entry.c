
#include <Ewl.h>

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
void            __ewl_entry_move_cursor_to_left(Ewl_Widget * w);
void            __ewl_entry_move_cursor_to_right(Ewl_Widget * w);
void            __ewl_entry_move_cursor_to_home(Ewl_Widget * w);
void            __ewl_entry_move_cursor_to_end(Ewl_Widget * w);
void            __ewl_entry_insert_text(Ewl_Widget * w, char *s);
void            __ewl_entry_delete_to_left(Ewl_Widget * w);
void            __ewl_entry_delete_to_right(Ewl_Widget * w);

void            __ewl_entry_update_selected_region(Ewl_Widget * w,
						   void *user_data,
						   void *ev_data);
void            __ewl_entry_child_add(Ewl_Container * c, Ewl_Widget * w);
void            __ewl_entry_child_resize(Ewl_Container * entry,
					 Ewl_Widget * text, int size,
					 Ewl_Orientation o);

/**
 * ewl_entry_new - allocate and initialize a new entry widget
 * @text: the initial text to display in the widget
 *
 * Returns a newly allocated and initialized entry widget on success, NULL on
 * failure.
 */
Ewl_Widget     *ewl_entry_new(char *text)
{
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	e = NEW(Ewl_Entry, 1);
	if (!e)
		return NULL;

	ZERO(e, Ewl_Entry, 1);

	ewl_entry_init(e, text);

	DRETURN_PTR(EWL_WIDGET(e), DLEVEL_STABLE);
}

/**
 * ewl_entry_init - initialize an entry widget to default values
 * @e: the entry widget to initialize
 * @text: the initial text to display in the widget
 *
 * Returns no value. Initializes the entry widget @e to it's default values
 * and callbacks.
 */
void ewl_entry_init(Ewl_Entry * e, char *text)
{
	Ewl_Widget     *w;
	int             pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	w = EWL_WIDGET(e);

	ewl_container_init(EWL_CONTAINER(w), "entry", __ewl_entry_child_add,
			__ewl_entry_child_resize, NULL);
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_HSHRINK |
			EWL_FILL_POLICY_HFILL);

	w->recursive = FALSE;

	e->text = ewl_text_new(text);
	ewl_container_append_child(EWL_CONTAINER(e), e->text);
	ewl_widget_show(e->text);

	e->cursor = ewl_cursor_new();
	ewl_container_append_child(EWL_CONTAINER(e), e->cursor);
	ewl_widget_show(e->cursor);

	pos = ewl_text_get_length(EWL_TEXT(e->text)) + 1;
	ewl_cursor_set_position(EWL_CURSOR(e->cursor), pos, pos);

	/*
	 * Attach necessary callback mechanisms 
	 */
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_entry_configure, NULL);

	ewl_callback_append(w, EWL_CALLBACK_SELECT, __ewl_entry_select, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESELECT, __ewl_entry_deselect,
			    NULL);

	ewl_entry_set_editable(e, TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_entry_set_text - set the text for an entry widget
 * @e: the entry widget to change the text
 * @t: the text to set for the entry widget
 *
 * Returns no value. Change the text of the entry widget @e to the string @t.
 */
void ewl_entry_set_text(Ewl_Entry * e, char *t)
{
	int pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	ewl_text_set_text(EWL_TEXT(e->text), t);

	pos = ewl_text_get_length(EWL_TEXT(e->text)) + 1;
	ewl_cursor_set_position(EWL_CURSOR(e->cursor), pos, pos);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_entry_get_text - get the text from an entry widget
 * @e: the entry widget to retrieve the text
 *
 * Returns the text contained in the entry widget @e on success, NULL on
 * failure.
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
 * ewl_entry_set_editable - change the ability to edit the text in an entry
 * @e: then entry to change
 * @edit: a boolean value indicating the ability to edit the entry
 *
 * Returns no value.
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
				__ewl_entry_key_down, NULL);
		ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
				__ewl_entry_mouse_down, NULL);
		ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
				__ewl_entry_mouse_move, NULL);
	}
	else {
		ewl_callback_del(w, EWL_CALLBACK_KEY_DOWN,
				__ewl_entry_key_down);
		ewl_callback_del(w, EWL_CALLBACK_MOUSE_DOWN,
				__ewl_entry_mouse_down);
		ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE,
				__ewl_entry_mouse_move);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Layout the text and cursor within the entry widget.
 */
void __ewl_entry_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry      *e;
	int             xx, yy, ww, hh;
	int             c_spos, c_epos, l;
	int             sx = 0, sy = 0, ex = 0, ey = 0, ew = 0, eh = 0;
	char           *str;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	/*
	 * The contents are clipped starting at these positions
	 */
	xx = CURRENT_X(w);
	yy = CURRENT_Y(w);
	hh = CURRENT_H(w);

	/*
	 * First position the text.
	 * FIXME: This needs to be scrollable
	 */
	ewl_object_request_geometry(EWL_OBJECT(e->text), xx, yy,
			ewl_object_get_preferred_w(EWL_OBJECT(e->text)),
			ewl_object_get_preferred_h(EWL_OBJECT(e->text)));

	str = ewl_text_get_text(EWL_TEXT(e->text));
	c_spos = ewl_cursor_get_start_position(EWL_CURSOR(e->cursor));
	c_epos = ewl_cursor_get_end_position(EWL_CURSOR(e->cursor));

	l = ewl_text_get_length(EWL_TEXT(e->text));
	if (str && l && c_spos > l) {
		xx += ewl_object_get_current_w(EWL_OBJECT(e->text));
		ww = 5;
	} else {

		/*
		 * Now position the cursor based on the current position in the
		 * text.
		 */
		ewl_text_get_letter_geometry(EWL_TEXT(e->text), --c_spos, &sx,
					     &sy, NULL, NULL);

		ewl_text_get_letter_geometry(EWL_TEXT(e->text), --c_epos, &ex,
					     &ey, &ew, &eh);

		xx = sx;
		ww = (ex + ew) - sx;
	}

	ewl_object_request_geometry(EWL_OBJECT(e->cursor), xx, yy, ww, hh);

	FREE(str);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Handle key events to modify the text of the entry widget.
 */
void __ewl_entry_key_down(Ewl_Widget * w, void *ev_data, void *user_data)
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

/*
 * Place the cursor appropriately on a mouse down event.
 */
void __ewl_entry_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ecore_Event_Mouse_Down *ev;
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
	} else
		index = ewl_text_get_index_at(EWL_TEXT(e->text), ev->x,
					      CURRENT_Y(e->text) +
					      (CURRENT_H(e->text) / 2));

	index++;
	ewl_cursor_set_position(EWL_CURSOR(e->cursor), index, index);

	if (index > len)
		e->base_click = len;
	else
		e->base_click = index;

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Hilight text when the mouse moves when the button is pressed
 */
void __ewl_entry_mouse_move(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             ss, ee;
	int             index = 0;
	Ecore_Event_Mouse_Move *ev;
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ev = ev_data;
	e = EWL_ENTRY(w);

	/*
	 * Check for the button pressed state, otherwise, do nothing.
	 */
	if (!(w->state & EWL_STATE_PRESSED))
		DRETURN(DLEVEL_STABLE);

	ss = ewl_cursor_get_start_position(EWL_CURSOR(e->cursor));
	ee = ewl_cursor_get_end_position(EWL_CURSOR(e->cursor));

	/*
	 * FIXME: the correct behavior here is to scroll to the left
	 * or right when selecting text.
	 */
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
	if (index < e->base_click) {
		ss = index;
		ee = e->base_click;
	}
	else if (index > e->base_click) {
		ss = e->base_click;
		ee = index;
	}
	else
		ss = ee = index;

	ewl_cursor_set_position(EWL_CURSOR(e->cursor), ss, ee);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_entry_select(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_widget_show(e->cursor);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_entry_deselect(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_widget_hide(e->cursor);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_entry_move_cursor_to_left(Ewl_Widget * w)
{
	Ewl_Entry      *e;
	int             pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	pos = ewl_cursor_get_start_position(EWL_CURSOR(e->cursor));

	if (pos > 1)
		--pos;

	ewl_cursor_set_position(EWL_CURSOR(e->cursor), pos, pos);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_entry_move_cursor_to_right(Ewl_Widget * w)
{
	Ewl_Entry      *e;
	char           *str;
	int             len = 0;
	int             pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	pos = ewl_cursor_get_end_position(EWL_CURSOR(e->cursor));
	str = ewl_entry_get_text(EWL_ENTRY(w));

	len = strlen(str);

	FREE(str);

	if (pos <= len)
		++pos;

	ewl_cursor_set_position(EWL_CURSOR(e->cursor), pos, pos);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Position the cursor at the beginning of the widget. This is internal, so the
 * parameter is not checked.
 */
void __ewl_entry_move_cursor_to_home(Ewl_Widget * w)
{
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	e = EWL_ENTRY(w);

	ewl_cursor_set_position(EWL_CURSOR(e->cursor), 1, 1);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Position the cursor at the end of the widget. This is internal, so the
 * parameter is not checked.
 */
void __ewl_entry_move_cursor_to_end(Ewl_Widget * w)
{
	Ewl_Entry      *e;
	char           *s;
	int             l = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	e = EWL_ENTRY(w);
	s = ewl_entry_get_text(EWL_ENTRY(w));

	if (s) {
		l = strlen(s);
		FREE(s);
	}

	ewl_cursor_set_position(EWL_CURSOR(e->cursor), l + 1, l + 1);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_entry_insert_text(Ewl_Widget * w, char *s)
{
	Ewl_Entry      *e;
	char           *s2, *s3;
	int             l = 0, l2 = 0, sp = 0, ep = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	s2 = ewl_entry_get_text(EWL_ENTRY(w));
	l = strlen(s);
	l2 = strlen(s2);

	sp = ewl_cursor_get_start_position(EWL_CURSOR(e->cursor));

	s3 = NEW(char, l + 1 + l2);

	s3[0] = 0;
	strncat(s3, s2, sp - 1);
	strcat(s3, s);

	ep = ewl_cursor_get_end_position(EWL_CURSOR(e->cursor));
	if (sp != ep)
		ep++;
	strcat(s3, &(s2[ep - 1]));

	ewl_entry_set_text(EWL_ENTRY(w), s3);

	FREE(s2);
	FREE(s3);

	sp++;
	ewl_cursor_set_position(EWL_CURSOR(e->cursor), sp, sp);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_entry_delete_to_left(Ewl_Widget * w)
{
	Ewl_Entry      *e;
	char           *s;
	unsigned int    sp, ep;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	sp = ewl_cursor_get_start_position(EWL_CURSOR(e->cursor));
	ep = ewl_cursor_get_end_position(EWL_CURSOR(e->cursor));

	s = ewl_entry_get_text(EWL_ENTRY(w));
	if (!strlen(s) || (sp == ep && sp < 2)) {
		FREE(s);
		DRETURN(DLEVEL_STABLE);
	}

	if (sp != ep) {
		sp++;
		ep++;
	}

	strcpy(&(s[sp - 2]), &(s[ep - 1]));
	ewl_entry_set_text(EWL_ENTRY(w), s);
	sp--;

	FREE(s);

	ewl_cursor_set_position(EWL_CURSOR(e->cursor), sp, sp);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_entry_delete_to_right(Ewl_Widget * w)
{
	Ewl_Entry      *e;
	char           *s;
	int             sp, ep;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	sp = ewl_cursor_get_start_position(EWL_CURSOR(e->cursor));
	ep = ewl_cursor_get_end_position(EWL_CURSOR(e->cursor));

	s = ewl_entry_get_text(EWL_ENTRY(w));

	if (!strlen(s) || ep == strlen(s) + 1)
		DRETURN(DLEVEL_STABLE);

	strcpy(&(s[sp - 1]), &(s[ep]));
	ewl_entry_set_text(EWL_ENTRY(w), s);

	FREE(s);

	ewl_cursor_set_position(EWL_CURSOR(e->cursor), sp, sp);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_child_add(Ewl_Container * c, Ewl_Widget * w)
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
__ewl_entry_child_resize(Ewl_Container * entry, Ewl_Widget * w, int size,
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
