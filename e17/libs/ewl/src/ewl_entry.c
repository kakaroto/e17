
#include <Ewl.h>

void ewl_entry_init(Ewl_Entry * e);
void __ewl_entry_realize(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_focus_in(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_focus_out(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_key_down(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_key_up(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_mouse_up(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_theme_update(Ewl_Widget * w, void *ev_data, void *user_data);
void __ewl_entry_update_cursor_pos(Ewl_Widget * w, void *ev_data,
				   void *user_data);
void __ewl_entry_set_cursor_pos_by_coords(Ewl_Widget * w, int xx, int yy);
void __ewl_entry_move_cursor_to_left(Ewl_Widget * w);
void __ewl_entry_move_cursor_to_right(Ewl_Widget * w);
void __ewl_entry_move_cursor_to_home(Ewl_Widget * w);
void __ewl_entry_move_cursor_to_end(Ewl_Widget * w);
void __ewl_entry_insert_text(Ewl_Widget * w, char *s);
void __ewl_entry_delete_to_left(Ewl_Widget * w);
void __ewl_entry_delete_to_right(Ewl_Widget * w);

Ewl_Widget *
ewl_entry_new(void)
{
	Ewl_Entry *e;

	DENTER_FUNCTION;

	e = NEW(Ewl_Entry, 1);
	if (!e)
		return NULL;

	memset(e, 0, sizeof(Ewl_Entry));

	e->text = ewl_text_new();
	e->cursor = ewl_cursor_new();

	ewl_entry_init(e);

	ewl_container_append_child(EWL_CONTAINER(e), e->text);
	ewl_container_append_child(EWL_CONTAINER(e), e->cursor);

	DRETURN_PTR(EWL_WIDGET(e));
}

void
ewl_entry_set_text(Ewl_Widget * w, char *t)
{
	Ewl_Entry *e;
	int p;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	p = ewl_cursor_get_position(e->cursor);

	ewl_text_set_text(e->text, t);
	ewl_cursor_set_position(e->cursor, p);

	DLEAVE_FUNCTION;
}

char *
ewl_entry_get_text(Ewl_Widget * w)
{
	Ewl_Entry *e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	e = EWL_ENTRY(w);

	DRETURN_PTR(ewl_text_get_text(e->text));
}

void
ewl_entry_init(Ewl_Entry * e)
{
	Ewl_Widget *w;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("e", e);

	w = EWL_WIDGET(e);

	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_HORIZONTAL);
	ewl_widget_set_appearance(w, "/appearance/entry/default");
	ewl_object_set_minimum_size(EWL_OBJECT(w), 17, 17);
	ewl_object_set_maximum_size(EWL_OBJECT(w), 1 << 30, 17);

	w->recursive = FALSE;

	/*
	 * Attach necessary callback mechanisms 
	 */
	ewl_callback_append(w, EWL_CALLBACK_REALIZE,
			    __ewl_entry_realize, NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN,
			    __ewl_entry_focus_in, NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT,
			    __ewl_entry_focus_out, NULL);
	ewl_callback_append(w, EWL_CALLBACK_KEY_DOWN,
			    __ewl_entry_key_down, NULL);
	ewl_callback_append(w, EWL_CALLBACK_KEY_UP, __ewl_entry_key_up, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
			    __ewl_entry_mouse_down, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP,
			    __ewl_entry_mouse_up, NULL);
	ewl_callback_append(w, EWL_CALLBACK_THEME_UPDATE,
			    __ewl_entry_theme_update, NULL);

	ewl_callback_append(e->cursor, EWL_CALLBACK_SHOW,
			    __ewl_entry_update_cursor_pos, NULL);
	ewl_callback_append(e->cursor, EWL_CALLBACK_VALUE_CHANGED,
			    __ewl_entry_update_cursor_pos, NULL);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry *e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_widget_show(e->text);
	ewl_widget_show(e->cursor);

	DLEAVE_FUNCTION;
}


void
__ewl_entry_focus_in(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_focus_out(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_key_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ev_Key_Down *ev;

	DENTER_FUNCTION;
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
	else if (!strcmp(ev->key, "Return"))
	  {
	  }
	else if (!strcmp(ev->key, "KP_Enter"))
	  {
	  }
	else if (ev->compose)
		__ewl_entry_insert_text(w, ev->compose);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_key_up(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_mouse_down(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ev_Mouse_Down *ev;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	ev = ev_data;

	__ewl_entry_set_cursor_pos_by_coords(w, ev->x, ev->y);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_mouse_up(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_theme_update(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry *e;
	char *font, *style;
	int size;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	font = ewl_theme_data_get(w, "/appearance/entry/default/text/font");
	size = (int) ewl_theme_data_get(w,
					"/appearance/entry/default/text/font_size");
	style = ewl_theme_data_get(w, "/appearance/entry/default/text/style");

	if (font)
		ewl_text_set_font(e->text, font);
	if (size)
		ewl_text_set_font_size(e->text, size);
	if (style)
		ewl_text_set_style(e->text, style);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_update_cursor_pos(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry *e;
	char *t;
	double xx, yy, ww, hh;
	int l = 0;
	int p = 0;
	int xp = 0;
	int tt = 0, bb = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w->parent);
	t = ewl_entry_get_text(w->parent);
	p = ewl_cursor_get_position(w);

	if (t)
		l = strlen(t);

	if (t && l < p)
		p = l + 1;

	ewl_text_get_letter_geometry(e->text, p - 1, &xx, &yy, &ww, &hh);

	xp -= CURRENT_W(e->text);
	xp += xx;

	if (l + 1 == p)
	  {
		  xp = 0;
		  ww = 5;
		  hh = 13;
	  }

	if (EWL_WIDGET(e)->ebits_object)
		ebits_get_insets(EWL_WIDGET(e)->ebits_object, 0, 0, &tt, &bb);

	ewl_widget_set_data(e->cursor, "/x_padding", (void *) xp);
	REQUEST_Y(e->cursor) = CURRENT_Y(e->text) + (int) (yy);
	REQUEST_W(e->cursor) = (int) (ww);
	REQUEST_H(e->cursor) = CURRENT_H(e) - tt - bb;

	ewl_widget_configure(e->cursor);
	ewl_widget_configure(EWL_WIDGET(e));

	FREE(t);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_set_cursor_pos_by_coords(Ewl_Widget * w, int xx, int yy)
{
	Ewl_Entry *e;
	char *s;
	int l = 0, p = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	s = ewl_entry_get_text(w);

	if (s)
		l = strlen(s);

	if (xx > CURRENT_X(e->text) &&
	    xx < CURRENT_X(e->text) + CURRENT_W(e->text))
	  {
		  double axx, ayy, aww, ahh;

		  ewl_text_get_letter_geometry_at(e->text, xx, yy,
						  &axx, &ayy, &aww, &ahh);
		  while (p != l + 1)
		    {
			    double cxx, cyy, cww, chh;

			    ewl_text_get_letter_geometry(e->text, p++,
							 &cxx, &cyy, &cww,
							 &chh);

			    if (axx == cxx &&
				ayy == cyy && aww == cww && ahh == chh)
			      {
				      ewl_cursor_set_position(e->cursor, p);
				      break;
			      }
		    }
	  }
	else if (xx < CURRENT_X(e->text))
		__ewl_entry_move_cursor_to_home(w);
	else if (xx > CURRENT_X(e->text) + CURRENT_W(e->text))
		__ewl_entry_move_cursor_to_end(w);

	FREE(s);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_move_cursor_to_left(Ewl_Widget * w)
{
	Ewl_Entry *e;
	int pos;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	pos = ewl_cursor_get_position(e->cursor);

	if (pos > 1)
		--pos;

	ewl_cursor_set_position(e->cursor, pos);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_move_cursor_to_right(Ewl_Widget * w)
{
	Ewl_Entry *e;
	char *str;
	int len = 0;
	int pos;

	DENTER_FUNCTION;
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

	DLEAVE_FUNCTION;
}

void
__ewl_entry_move_cursor_to_home(Ewl_Widget * w)
{
	Ewl_Entry *e;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_cursor_set_position(e->cursor, 1);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_move_cursor_to_end(Ewl_Widget * w)
{
	Ewl_Entry *e;
	char *s;
	int l = 0;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	s = ewl_entry_get_text(w);

	if (s)
	  {
		  l = strlen(s);
		  FREE(s);
	  }

	ewl_cursor_set_position(e->cursor, ++l);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_insert_text(Ewl_Widget * w, char *s)
{
	Ewl_Entry *e;
	char *s2, *s3;
	int l = 0, l2 = 0, p = 0;

	DENTER_FUNCTION;
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

	DLEAVE_FUNCTION;
}

void
__ewl_entry_delete_to_left(Ewl_Widget * w)
{
	Ewl_Entry *e;
	char *s, *s2;
	int p;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	p = ewl_cursor_get_position(e->cursor);

	if (p == 1)
		DRETURN;

	s2 = ewl_entry_get_text(w);
	if (!strlen(s2))
		DRETURN;

	s = ewl_entry_get_text(w);

	strcpy(&(s[p - 2]), &(s2[p - 1]));
	ewl_entry_set_text(w, s);
	p--;

	FREE(s);
	FREE(s2);

	ewl_cursor_set_position(e->cursor, p);

	DLEAVE_FUNCTION;
}

void
__ewl_entry_delete_to_right(Ewl_Widget * w)
{
	Ewl_Entry *e;
	char *s, *s2;
	int p;

	DENTER_FUNCTION;
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);
	p = ewl_cursor_get_position(e->cursor);

	s2 = ewl_entry_get_text(w);

	if (!strlen(s2) || p == strlen(s2) + 1)
		DRETURN;

	s = ewl_entry_get_text(w);

	strcpy(&(s[p - 1]), &(s2[p]));
	ewl_entry_set_text(w, s);

	FREE(s);

	ewl_cursor_set_position(e->cursor, p);

	DLEAVE_FUNCTION;
}
