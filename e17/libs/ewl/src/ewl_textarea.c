
#include <Ewl.h>

void __ewl_textarea_realize(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_textarea_select(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_textarea_deselect(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_textarea_key_down(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_textarea_key_up(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_textarea_mouse_down(Ewl_Widget * w, void *ev_data,
			       void *user_data);
void __ewl_textarea_mouse_up(Ewl_Widget * w, void *ev_data, void *user_data);

void __ewl_textarea_text_configure(Ewl_Widget * w, void *ev_data,
				   void *user_data);


void __ewl_textarea_insert_text(Ewl_TextArea * ta, char *s);
void __ewl_textarea_move_cursor_to_left(Ewl_TextArea * ta);
void __ewl_textarea_move_cursor_to_right(Ewl_TextArea * ta);
void __ewl_textarea_move_cursor_to_home(Ewl_TextArea * ta);
void __ewl_textarea_move_cursor_to_end(Ewl_TextArea * ta);
void __ewl_textarea_delete_to_left(Ewl_TextArea * ta);
void __ewl_textarea_delete_to_right(Ewl_TextArea * ta);


/**
 * ewl_textarea_new - allocate a new text area widget
 *
 * Returns a pointer to a newly allocated text area widget on success, NULL on
 * failure.
 */
Ewl_Widget *
ewl_textarea_new(void)
{
	Ewl_TextArea *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);

	ta = NEW(Ewl_TextArea, 1);
	ZERO(ta, Ewl_TextArea, 1);

	ta->text = ewl_text_new();
	ta->cursor = ewl_cursor_new();

	ewl_textarea_init(ta);

	ewl_container_append_child(EWL_CONTAINER(ta), ta->text);

	DRETURN_PTR(EWL_WIDGET(ta), DLEVEL_STABLE);
}

/**
 * ewl_textarea_set_text - set the text of a text area widget
 * @ta: the text area widget to set the text
 * @text: the text to set in the text area widget @ta
 *
 * Returns no value. Sets the text of the text area widget @ta to a copy of
 * the contents of @text.
 */
void
ewl_textarea_set_text(Ewl_TextArea * ta, char *text)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);
	DCHECK_PARAM_PTR("text", text);

	ewl_text_set_text(EWL_TEXT(ta->text), text);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_textarea_get_text - retrieve the text of a text widget
 * @ta: the text widget to retrieve text contents
 *
 * Returns a pointer to a copy of the text in @ta on success, NULL on failure.
 */
char *
ewl_textarea_get_text(Ewl_TextArea * ta)
{
	char *text;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("ta", ta, NULL);

	text = ewl_text_get_text(EWL_TEXT(ta->text));

	DRETURN_PTR(text, DLEVEL_STABLE);
}

/**
 * ewl_textarea_init - initialize the fields and callbacks of a text area
 * @ta: the text area to be initialized
 *
 * Returns no value. Sets the internal fields and callbacks of a text area to
 * their defaults.
 */
void
ewl_textarea_init(Ewl_TextArea * ta)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	w = EWL_WIDGET(ta);

	ewl_scrollpane_init(EWL_SCROLLPANE(ta));
	ewl_widget_set_appearance(w, "/appearance/textarea/default");

	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_textarea_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_KEY_DOWN,
			    __ewl_textarea_key_down, ta);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_textarea_mouse_down, ta);

	ewl_callback_append(ta->text, EWL_CALLBACK_CONFIGURE,
			    __ewl_textarea_text_configure, ta);
	ewl_callback_append(ta->text, EWL_CALLBACK_SELECT,
			    __ewl_textarea_select, ta);
	ewl_callback_append(ta->text, EWL_CALLBACK_KEY_DOWN,
			    __ewl_textarea_key_down, ta);
	ewl_callback_append(ta->text, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_textarea_mouse_down, ta);

	ewl_scrollpane_set_hscrollbar_flag(EWL_SCROLLPANE(ta),
					   EWL_SCROLLBAR_FLAG_AUTO_VISIBLE);
	ewl_scrollpane_set_vscrollbar_flag(EWL_SCROLLPANE(ta),
					   EWL_SCROLLBAR_FLAG_AUTO_VISIBLE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_TextArea *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ta = EWL_TEXTAREA(w);

	ewl_widget_set_parent(ta->cursor, EWL_WIDGET(ta));

	ewl_widget_realize(ta->text);
	ewl_widget_realize(ta->cursor);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_select(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_TextArea *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	ta = user_data;

	ewl_widget_show(ta->cursor);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_deselect(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_TextArea *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	ta = user_data;

	ewl_widget_hide(ta->cursor);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_key_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ecore_Event_Key_Down *ev;
	Ewl_TextArea *ta;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev_data", ev_data);
	DCHECK_PARAM_PTR("user_data", user_data);

	ev = ev_data;
	ta = user_data;

	if (!strcmp(ev->key, "Left"))
		__ewl_textarea_move_cursor_to_left(ta);
	else if (!strcmp(ev->key, "Right"))
		__ewl_textarea_move_cursor_to_right(ta);
	else if (!strcmp(ev->key, "Home"))
		__ewl_textarea_move_cursor_to_home(ta);
	else if (!strcmp(ev->key, "End"))
		__ewl_textarea_move_cursor_to_end(ta);
	else if (!strcmp(ev->key, "Return"))
		__ewl_textarea_insert_text(ta, "\n");
	else if (!strcmp(ev->key, "BackSpace"))
		__ewl_textarea_delete_to_left(ta);
	else if (!strcmp(ev->key, "Delete"))
		__ewl_textarea_delete_to_right(ta);
	else if (ev->compose)
		__ewl_textarea_insert_text(ta, ev->compose);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_key_up(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ecore_Event_Mouse_Up *ev;
	Ewl_TextArea *ta;
	int i = 0, l = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev_data", ev_data);
	DCHECK_PARAM_PTR("user_data", user_data);

	ev = ev_data;
	ta = user_data;

	if (ev->x < CURRENT_X(ta->text))
		i = 0;
	else if (ev->x > CURRENT_X(ta->text) + CURRENT_W(ta->text) ||
		 ev->y > CURRENT_Y(ta->text) + CURRENT_H(ta->text))
	  {
		  char *str;

		  str = ewl_textarea_get_text(ta);

		  if (str)
			  l = i = strlen(str);
	  }
	else
		i = ewl_text_get_index_at(EWL_TEXT(ta->text), ev->x, ev->y);

	ewl_cursor_set_position(ta->cursor, i + 1);

	ewl_widget_configure(EWL_WIDGET(ta));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_mouse_up(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("ev_data", ev_data);
	DCHECK_PARAM_PTR("user_data", user_data);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_text_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_TextArea *ta;
	char *str;
	int c_pos, req_x, req_y, req_w, req_h, l;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);

	ta = user_data;

	c_pos = ewl_cursor_get_position(ta->cursor);

	ewl_text_get_letter_geometry(EWL_TEXT(ta->text), --c_pos, &req_x,
			&req_y, &req_w, &req_h);

	str = EWL_TEXT(ta->text)->text;

	if (str && (l = strlen(str)) && c_pos >= l)
	  {
		  ewl_text_get_letter_geometry(EWL_TEXT(ta->text), l - 1,
				  &req_x, &req_y, &req_w, &req_h);
		  req_x += req_w + CURRENT_X(ta->text);
		  req_y += CURRENT_Y(ta->text);
		  req_w = 5;
	  }
	else
	  {
		  req_x += CURRENT_X(ta->text);
		  req_y += CURRENT_Y(ta->text);
	  }

	ewl_object_request_geometry(EWL_OBJECT(ta->cursor), req_x, req_y,
				    req_w, req_h);

	ewl_widget_configure(ta->cursor);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_insert_text(Ewl_TextArea * ta, char *s)
{
	char *s2, *s3;
	int l = 0, l2 = 0, p = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	s2 = ewl_textarea_get_text(ta);
	l = strlen(s);
	l2 = strlen(s2);

	p = ewl_cursor_get_position(ta->cursor);

	s3 = NEW(char, l + 1 + l2);

	s3[0] = 0;
	strncat(s3, s2, p - 1);
	strcat(s3, s);
	strcat(s3, &(s2[p - 1]));

	ewl_textarea_set_text(ta, s3);

	FREE(s2);
	FREE(s3);

	p += l;
	ewl_cursor_set_position(ta->cursor, p);
	ewl_widget_configure(EWL_WIDGET(ta));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_move_cursor_to_left(Ewl_TextArea * ta)
{
	int pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	pos = ewl_cursor_get_position(ta->cursor);

	if (pos > 1)
		--pos;

	ewl_cursor_set_position(ta->cursor, pos);
	ewl_widget_configure(EWL_WIDGET(ta));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_move_cursor_to_right(Ewl_TextArea * ta)
{
	char *str;
	int l = 0, pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	pos = ewl_cursor_get_position(ta->cursor);
	str = ewl_textarea_get_text(ta);

	if (str)
		l = strlen(str);

	IF_FREE(str);

	if (pos <= l)
		++pos;

	ewl_cursor_set_position(ta->cursor, pos);
	ewl_widget_configure(EWL_WIDGET(ta));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_move_cursor_to_home(Ewl_TextArea * ta)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	ewl_cursor_set_position(ta->cursor, 1);
	ewl_widget_configure(EWL_WIDGET(ta));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_move_cursor_to_end(Ewl_TextArea * ta)
{
	char *s;
	int l = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	s = ewl_textarea_get_text(ta);

	if (s)
	  {
		  l = strlen(s);
		  FREE(s);
	  }

	ewl_cursor_set_position(ta->cursor, ++l);
	ewl_widget_configure(EWL_WIDGET(ta));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_delete_to_left(Ewl_TextArea * ta)
{
	char *s, *s2;
	int p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	p = ewl_cursor_get_position(ta->cursor);

	if (p == 1)
		DRETURN(DLEVEL_STABLE);

	s2 = ewl_textarea_get_text(ta);

	if (!strlen(s2))
		DRETURN(DLEVEL_STABLE);

	s = ewl_textarea_get_text(ta);

	strcpy(&(s[p - 2]), &(s2[p - 1]));
	ewl_textarea_set_text(ta, s);
	p--;

	FREE(s);
	FREE(s2);

	ewl_cursor_set_position(ta->cursor, p);
	ewl_widget_configure(EWL_WIDGET(ta));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_textarea_delete_to_right(Ewl_TextArea * ta)
{
	char *s, *s2;
	int p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("ta", ta);

	p = ewl_cursor_get_position(ta->cursor);

	s2 = ewl_textarea_get_text(ta);

	if (!strlen(s2) || p == strlen(s2) + 1)
		DRETURN(DLEVEL_STABLE);

	s = ewl_textarea_get_text(ta);

	strcpy(&(s[p - 1]), &(s2[p]));
	ewl_textarea_set_text(ta, s);

	FREE(s);

	ewl_cursor_set_position(ta->cursor, p);
	ewl_widget_configure(EWL_WIDGET(ta));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
