
#include <Ewl.h>

void ewl_entry_init(Ewl_Entry * e);
void __ewl_entry_realize(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_configure(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_focus_in(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_focus_out(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_key_down(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_key_up(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_mouse_up(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_mouse_move(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_select(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_deselect(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_theme_update(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_move_cursor_to_left(Ewl_Widget * w);
void __ewl_entry_move_cursor_to_right(Ewl_Widget * w);
void __ewl_entry_move_cursor_to_home(Ewl_Widget * w);
void __ewl_entry_move_cursor_to_end(Ewl_Widget * w);
void __ewl_entry_insert_text(Ewl_Widget * w, char *s);
void __ewl_entry_delete_to_left(Ewl_Widget * w);
void __ewl_entry_delete_to_right(Ewl_Widget * w);

void
__ewl_entry_update_selected_region(Ewl_Widget * w, void *user_data,
				   void *ev_data);

Ewl_Widget *
ewl_entry_new(void)
{
	Ewl_Entry *e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	e = NEW(Ewl_Entry, 1);
	if (!e)
		return NULL;

	memset(e, 0, sizeof(Ewl_Entry));

	e->text = ewl_text_new();
	e->cursor = ewl_cursor_new();
	e->selection = ewl_selection_new();

	ewl_entry_init(e);

	ewl_container_append_child(EWL_CONTAINER(e), e->text);
	ewl_container_append_child(EWL_CONTAINER(e), e->selection);
	ewl_container_append_child(EWL_CONTAINER(e), e->cursor);

	DRETURN_PTR(EWL_WIDGET(e), DLEVEL_STABLE);
}

void
ewl_entry_set_text(Ewl_Widget * w, char *t)
{
	Ewl_Entry *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_text_set_text(e->text, t);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

char *
ewl_entry_get_text(Ewl_Widget * w)
{
	Ewl_Entry *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	e = EWL_ENTRY(w);

	DRETURN_PTR(ewl_text_get_text(e->text), DLEVEL_STABLE);
}

void
ewl_entry_init(Ewl_Entry * e)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	w = EWL_WIDGET(e);

	ewl_container_init(EWL_CONTAINER(w), "/appearance/entry/default");
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FILL_POLICY_FILL);
	ewl_object_set_minimum_size(EWL_OBJECT(w), 20, 20);
	ewl_object_set_maximum_size(EWL_OBJECT(w), 1 << 30, 20);

	w->recursive = FALSE;

	/*
	 * Attach necessary callback mechanisms 
	 */
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_entry_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    __ewl_entry_configure, NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN,
			    __ewl_entry_focus_in, NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT,
			    __ewl_entry_focus_out, NULL);
	ewl_callback_append(w, EWL_CALLBACK_KEY_DOWN,
			    __ewl_entry_key_down, NULL);
	ewl_callback_append(w, EWL_CALLBACK_KEY_UP, __ewl_entry_key_up, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_entry_mouse_down, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP, __ewl_entry_mouse_up,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
			    __ewl_entry_mouse_move, NULL);
	ewl_callback_append(w, EWL_CALLBACK_SELECT, __ewl_entry_select, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESELECT, __ewl_entry_deselect,
			    NULL);
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_entry_theme_update, NULL);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_widget_show(e->text);
	ewl_widget_realize(e->cursor);
	ewl_widget_hide(e->cursor);

	ewl_widget_realize(e->selection);
	ewl_widget_hide(e->selection);

	if (w->ebits_object)
	  {
		  int ww, hh;

		  ebits_get_max_size(w->ebits_object, &ww, &hh);

		  ewl_object_set_maximum_size(EWL_OBJECT(w), ww, hh);
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry *e;
	int ll = 0, rr = 0, tt = 0, bb = 0;
	int xx, yy, ww, hh;
	double xx2, yy2, ww2, hh2;
	int c_pos, l;
	int ss, ee;
	double sx, sy, sw, sh, ex, ey, ew, eh;
	char *str;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	if (w->ebits_object)
		ebits_get_insets(w->ebits_object, &ll, &rr, &tt, &bb);

	/******************************************************************/

	xx = CURRENT_X(w);
	xx += ll;

	yy = CURRENT_Y(w);
	yy += tt;

	ewl_object_request_position(EWL_OBJECT(e->text), xx, yy);
	ewl_widget_configure(e->text);

	/******************************************************************/

	c_pos = ewl_cursor_get_position(e->cursor);

	ewl_text_get_letter_geometry(e->text, --c_pos, &xx2, &yy2, &ww2,
				     &hh2);

	str = EWL_TEXT(e->text)->text;

	if (str && (l = strlen(str)) && c_pos >= l)
	  {
		  xx += CURRENT_W(e->text);
		  ww = 5;
		  hh = CURRENT_H(w) - tt - bb;
	  }
	else
	  {
		  xx += xx2;
		  yy += yy2;
		  ww = ww2;
		  hh = CURRENT_H(w) - tt - bb;
	  }

	ewl_object_request_geometry(EWL_OBJECT(e->cursor), xx, yy, ww, hh);
	ewl_widget_configure(e->cursor);

	/******************************************************************/

	ewl_selection_get_covered(e->selection, &ss, &ee);

	if (ss >= 0)
	  {
		  xx = CURRENT_X(e->text);
		  yy = CURRENT_Y(e->text);
		  ww = 0;

		  ewl_text_get_letter_geometry(e->text, ss, &sx, &sy, &sw,
					       &sh);
		  ewl_text_get_letter_geometry(e->text, ss + ee, &ex, &ey,
					       &ew, &eh);

		  xx += sx;
		  ww += sw;

		  if (ee > 0)
		    {
			    ww -= sw;
			    ww += ex - sx;
			    ww += ew;
		    }
		  else if (ee < 0)
		    {
			    xx -= sx;
			    xx += ex;
			    ww += (int) (sx - ex);

		    }

		  ewl_object_request_geometry(EWL_OBJECT(e->selection), xx,
					      yy, ww, hh);
		  ewl_widget_configure(e->selection);
	  }

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_focus_in(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_focus_out(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_key_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ev_Key_Down *ev;

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
__ewl_entry_key_up(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ev_Mouse_Down *ev;
	Ewl_Entry *e;
	int index = 0, len = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ev = ev_data;
	e = EWL_ENTRY(w);

	if (ev->x < CURRENT_X(e->text))
		index = 0;
	else if (ev->x > CURRENT_X(e->text) + CURRENT_W(e->text))
	  {
		  char *str;

		  str = ewl_entry_get_text(w);

		  if (str)
			  len = index = strlen(str);
	  }
	else
		ewl_text_get_index_at(e->text, (double) (ev->x),
				      (double) (CURRENT_Y(e->text) +
						(CURRENT_H(e->text) / 2)),
				      &index);

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
__ewl_entry_mouse_up(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_mouse_move(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ev_Mouse_Move *ev;
	Ewl_Entry *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ev = ev_data;
	e = EWL_ENTRY(w);

	if (w->state & EWL_STATE_PRESSED)
	  {
		  int ss, ee;
		  int index;

		  ewl_widget_show(e->selection);

		  ewl_selection_get_covered(e->selection, &ss, &ee);
		  if (ev->x > CURRENT_X(e->text) &&
		      ev->x < CURRENT_X(e->text) + CURRENT_W(e->text))
		    {
			    ewl_text_get_index_at(e->text, (double) (ev->x),
						  (double) (CURRENT_Y(e->text)
							    +
							    (CURRENT_H
							     (e->text) / 2)),
						  &index);

			    ee = index - ss;

			    ewl_selection_set_covered(e->selection, ss, ee);

			    ewl_cursor_set_position(e->cursor, index + 1);

		    }
		  else if (ev->x < CURRENT_X(e->text))
		    {
		    }
		  else if (ev->x > CURRENT_X(e->text) + CURRENT_W(e->text))
		    {
			    char *str;

			    str = ewl_entry_get_text(w);

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
	Ewl_Entry *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_widget_show(e->cursor);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_deselect(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_widget_hide(e->cursor);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_theme_update(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry *e;
	char *font, *style;
	int size;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	font = ewl_theme_data_get_str(w,
				      "/appearance/entry/default/text/font");
	ewl_theme_data_get_int(w, "/appearance/entry/default/text/font_size",
			       &size);
	style = ewl_theme_data_get_str(w,
				       "/appearance/entry/default/text/style");

	if (font)
		ewl_text_set_font(e->text, font);
	if (size)
		ewl_text_set_font_size(e->text, size);
	if (style)
		ewl_text_set_style(e->text, style);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
__ewl_entry_move_cursor_to_left(Ewl_Widget * w)
{
	Ewl_Entry *e;
	int pos;

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
	Ewl_Entry *e;
	char *str;
	int len = 0;
	int pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	pos = ewl_cursor_get_position(e->cursor);
	str = ewl_entry_get_text(w);

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
	Ewl_Entry *e;

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
	Ewl_Entry *e;
	char *s;
	int l = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	s = ewl_entry_get_text(w);

	if (s)
	  {
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
	Ewl_Entry *e;
	char *s2, *s3;
	int l = 0, l2 = 0, p = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	s2 = ewl_entry_get_text(w);
	l = strlen(s);
	l2 = strlen(s2);

	p = ewl_cursor_get_position(e->cursor);

	s3 = NEW(char, l + 1 + l2);

	s3[0] = 0;
	strncat(s3, s2, p - 1);
	strcat(s3, s);
	strcat(s3, &(s2[p - 1]));

	ewl_entry_set_text(w, s3);

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
	Ewl_Entry *e;
	char *s, *s2;
	int p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	p = ewl_cursor_get_position(e->cursor);

	if (p == 1)
		DRETURN(DLEVEL_STABLE);

	s2 = ewl_entry_get_text(w);
	if (!strlen(s2))
		DRETURN(DLEVEL_STABLE);

	s = ewl_entry_get_text(w);

	strcpy(&(s[p - 2]), &(s2[p - 1]));
	ewl_entry_set_text(w, s);
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
	Ewl_Entry *e;
	char *s, *s2;
	int p;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	p = ewl_cursor_get_position(e->cursor);

	s2 = ewl_entry_get_text(w);

	if (!strlen(s2) || p == strlen(s2) + 1)
		DRETURN(DLEVEL_STABLE);

	s = ewl_entry_get_text(w);

	strcpy(&(s[p - 1]), &(s2[p]));
	ewl_entry_set_text(w, s);

	FREE(s);

	ewl_cursor_set_position(e->cursor, p);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
