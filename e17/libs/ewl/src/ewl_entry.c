#include <Ewl.h>

/*
 * the cursor stuff 
 */
typedef struct Ewl_Entry_Cursor Ewl_Entry_Cursor;

#define EWL_ENTRY_CURSOR(cursor) ((Ewl_Entry_Cursor *) cursor)

struct Ewl_Entry_Cursor
{
	Ewl_Widget	widget;
	int		position;
};

static Ewl_Widget *ewl_entry_cursor_new(void);
static void ewl_entry_cursor_init(Ewl_Entry_Cursor *c); 

static void ewl_entry_cursor_position_set(Ewl_Entry_Cursor *c, int pos);
static int ewl_entry_cursor_position_get(Ewl_Entry_Cursor *c);

/*
 * the selection stuff
 */
typedef struct Ewl_Entry_Selection Ewl_Entry_Selection;
 
#define EWL_ENTRY_SELECTION(selection) ((Ewl_Entry_Selection *) selection)

struct Ewl_Entry_Selection
{
	Ewl_Widget	widget;
	unsigned int 	start;
	unsigned int	end;
};

static Ewl_Widget *ewl_entry_selection_new(void);
static void ewl_entry_selection_init(Ewl_Entry_Selection *s); 

static void ewl_entry_selection_start_position_set(Ewl_Entry_Selection *s,
						unsigned int start);
static int ewl_entry_selection_start_position_get(Ewl_Entry_Selection *s);
static void ewl_entry_selection_end_position_set(Ewl_Entry_Selection *s,
						unsigned int end);
static int ewl_entry_selection_end_position_get(Ewl_Entry_Selection *s);
static void ewl_entry_selection_select_to(Ewl_Entry_Selection *s,
						unsigned int pos);


static int ewl_entry_timer();

/**
 * @param text: the initial text to display in the widget
 * @return Returns a new entry widget on success, NULL on failure.
 * @brief Allocate and initialize a new multiline input entry widget
 */
Ewl_Widget *ewl_entry_multiline_new(char *text)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = ewl_entry_new(text);
	if (!w)
		return NULL;

	ewl_entry_multiline_set( EWL_ENTRY(w), TRUE );

	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param text: the initial text to display in the widget
 * @return Returns a new entry widget on success, NULL on failure.
 * @brief Allocate and initialize a new entry widget
 */
Ewl_Widget *ewl_entry_new(char *text)
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

	e->in_select_mode = FALSE;
	e->multiline = FALSE;

	if (!ewl_container_init(EWL_CONTAINER(w), "entry"))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_container_show_notify_set(EWL_CONTAINER(w), ewl_entry_child_show_cb);
	ewl_container_resize_notify_set(EWL_CONTAINER(w),
				    ewl_entry_child_resize_cb);

	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_HSHRINK |
			EWL_FLAG_FILL_HFILL);
	ewl_container_callback_intercept(EWL_CONTAINER(w), EWL_CALLBACK_SELECT);
	ewl_container_callback_intercept(EWL_CONTAINER(w),
					 EWL_CALLBACK_DESELECT);

	e->text = ewl_text_new(text);
	ewl_container_child_append(EWL_CONTAINER(e), e->text);
	ewl_callback_append(e->text, EWL_CALLBACK_CONFIGURE,
			    ewl_entry_configure_text_cb, e);
	ewl_widget_show(e->text);

	e->cursor = ewl_entry_cursor_new();
	ewl_container_child_append(EWL_CONTAINER(e), e->cursor);
	ewl_widget_internal_set(e->cursor, TRUE);
	ewl_object_fill_policy_set(EWL_OBJECT(e->cursor), EWL_FLAG_FILL_SHRINK);
	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), 0);
	ewl_widget_show(e->cursor);

	e->selection = NULL;

	/*
	 * Attach necessary callback mechanisms 
	 */
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
			    ewl_entry_configure_cb, NULL);

	ewl_callback_append(w, EWL_CALLBACK_SELECT, ewl_entry_select_cb, NULL);
	ewl_callback_append(w, EWL_CALLBACK_DESELECT, ewl_entry_deselect_cb,
			    NULL);

	ewl_entry_editable_set(e, TRUE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param e: the entry widget to set multiline
 * @param m: the value to set multiline to
 * @return Returns no value.
 * @brief Set multiline for an entry widget
 *
 * Set the multiline flag for $a e to @a m
 */
void ewl_entry_multiline_set(Ewl_Entry * e, int m)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	e->multiline = m;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the entry widget to retrieve the multiline flag
 * @return Returns the multiline flag on success, -1 on failure.
 * @brief Get the text from an entry widget
 */
int ewl_entry_multiline_get(Ewl_Entry * e)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, -1);

	DRETURN_INT(e->multiline, DLEVEL_STABLE);
}

/**
 * @param e: the entry widget to change the text
 * @param t: the text to set for the entry widget
 * @return Returns no value.
 * @brief Set the text for an entry widget
 *
 * Change the text of the entry widget @a e to the string @a t.
 */
void ewl_entry_text_set(Ewl_Entry * e, char *t)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	ewl_text_text_set(EWL_TEXT(e->text), t);

	if (e->cursor)
		ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), 0);

	if (e->selection) {
		ewl_widget_destroy(e->selection);
		e->selection = NULL;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: the entry widget to retrieve the text
 * @return Returns the entry text on success, NULL on failure.
 * @brief Get the text from an entry widget
 */
char *ewl_entry_text_get(Ewl_Entry * e)
{
	Ewl_Widget     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, NULL);

	w = EWL_WIDGET(e);

	DRETURN_PTR(ewl_text_text_get(EWL_TEXT(e->text)), DLEVEL_STABLE);
}

/**
 * @param e: then entry to change
 * @param edit: a boolean value indicating the ability to edit the entry
 * @return Returns no value.
 * @brief Change the ability to edit the text in an entry
 */
void
ewl_entry_editable_set(Ewl_Entry *e, unsigned int edit)
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
		ewl_callback_del(w, EWL_CALLBACK_MOUSE_UP,
				ewl_entry_mouse_up_cb);
		ewl_callback_append(w, EWL_CALLBACK_DOUBLE_CLICKED,
				ewl_entry_mouse_double_click_cb, NULL);
		ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
				ewl_entry_mouse_move_cb, NULL);
	}
	else {
		ewl_callback_del(w, EWL_CALLBACK_KEY_DOWN,
				ewl_entry_key_down_cb);
		ewl_callback_del(w, EWL_CALLBACK_MOUSE_DOWN,
				ewl_entry_mouse_down_cb);
		ewl_callback_del(w, EWL_CALLBACK_MOUSE_UP,
				ewl_entry_mouse_up_cb);
		ewl_callback_del(w, EWL_CALLBACK_DOUBLE_CLICKED,
				ewl_entry_mouse_double_click_cb);
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
	ewl_object_geometry_request(EWL_OBJECT(e->text), xx - e->offset, yy,
			ww, hh);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_configure_text_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry    *e;
	int           xx, yy, ww, hh;
	int	      c_pos = 0, l;
	int           cx = 0, cy = 0;
	unsigned int  cw = 0, ch = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	e = EWL_ENTRY(user_data);

	l = ewl_text_length_get(EWL_TEXT(w));

	/*
	 * The contents are clipped starting at these positions
	 */
	xx = CURRENT_X(e);
	yy = CURRENT_Y(e);
	ww = CURRENT_W(e);
	hh = CURRENT_H(e);

	c_pos = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));

	if (c_pos >= l) {
		cx = ewl_object_current_x_get(EWL_OBJECT(w)) +
				ewl_object_current_w_get(EWL_OBJECT(w));
	} else {
		ewl_text_index_geometry_map(EWL_TEXT(w), c_pos, &cx, &cy, &cw, &ch);
	}

	/*
	 * D'oh, get the hell out of here, the entry is way too small to do
	 * anything useful.
	 */
	if (cw > ww)
		DRETURN(DLEVEL_STABLE);

	if (e->offset < 0)
		e->offset = 0;

/* FIXME
	ch = ewl_text_font_size_get(EWL_TEXT(w)); */
	ch = ewl_theme_data_int_get(w, "font_size");

#if 0
printf("cx %d, cy %d, cw %d, ch %d\n", cx, cy, cw, ch);
#endif

	ewl_object_geometry_request(EWL_OBJECT(e->cursor), cx, cy, 
									CURRENT_W(e->cursor), ch);
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

	e->in_select_mode = (ev->modifiers & EWL_KEY_MODIFIER_SHIFT);

	if (!strcmp(ev->keyname, "Left"))
		ewl_entry_cursor_left_move(e);
	else if (!strcmp(ev->keyname, "Right"))
		ewl_entry_cursor_right_move(e);
	else if (!strcmp(ev->keyname, "Down"))
		ewl_entry_cursor_down_move(e);
	else if (!strcmp(ev->keyname, "Up"))
		ewl_entry_cursor_up_move(e);
	else if (!strcmp(ev->keyname, "Home"))
		ewl_entry_cursor_home_move(e);
	else if (!strcmp(ev->keyname, "End"))
		ewl_entry_cursor_end_move(e);
	else if (!strcmp(ev->keyname, "BackSpace"))
		ewl_entry_left_delete(e);
	else if (!strcmp(ev->keyname, "Delete"))
		ewl_entry_right_delete(e);
	else if (((!strcmp(ev->keyname, "w")) && 
		  (ev->modifiers & EWL_KEY_MODIFIER_CTRL)) ||
		 ((!strcmp(ev->keyname, "W")) && 
		  (ev->modifiers & EWL_KEY_MODIFIER_CTRL)))
		ewl_entry_word_begin_delete(e);
	else if (!strcmp(ev->keyname, "Return") || !strcmp(ev->keyname,
				"KP_Return") || !strcmp(ev->keyname, "Enter")
				|| !strcmp(ev->keyname, "KP_Enter")) {
		if (!e->multiline) {
			evd = ewl_text_text_get(EWL_TEXT(e->text));
			ewl_callback_call_with_event_data(w, EWL_CALLBACK_VALUE_CHANGED,
					evd);
			FREE(evd);
		} else {
			ewl_entry_text_insert(e, "\n");
			ewl_entry_cursor_home_move(e);
		}
	}
	else if (ev->keyname && strlen(ev->keyname) == 1) {
		ewl_entry_text_insert(e, ev->keyname);
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

	e->in_select_mode = TRUE;

	len = ewl_text_length_get(EWL_TEXT(e->text));
	if (ev->x < CURRENT_X(e->text))
		index = 0;

	else if (ev->x > CURRENT_X(e->text) + CURRENT_W(e->text)) 
		index = len;
	
	else
		index = ewl_text_coord_index_map(EWL_TEXT(e->text), ev->x, ev->y);

	if (!e->selection)
		e->selection = ewl_entry_selection_new();
	
	ewl_entry_selection_start_position_set(EWL_ENTRY_SELECTION(e->selection), index);
	ewl_entry_selection_end_position_set(EWL_ENTRY_SELECTION(e->selection), index);

	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), index);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Stop the scrolling timer.
 */
void ewl_entry_mouse_up_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Event_Mouse_Down *ev = ev_data;
	Ewl_Entry *e = EWL_ENTRY(w);

	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	if (e->timer) {
		ecore_timer_del(e->timer);
		e->timer = NULL;
		e->start_time = 0.0;
	}
	*/

	e->in_select_mode = (ev->modifiers & EWL_KEY_MODIFIER_SHIFT);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Hilight text when the mouse moves when the button is pressed
 */
void ewl_entry_mouse_move_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	int             index = 0, spos = 0;
	Ewl_Event_Mouse_Move *ev;
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	ev = ev_data;
	e = EWL_ENTRY(w);

	/*
	 * Check for the button pressed state, otherwise, do nothing.
	 */
	if (!(ewl_object_state_has(EWL_OBJECT(e), EWL_FLAG_STATE_PRESSED)) || 
	    !(e->in_select_mode))
		DRETURN(DLEVEL_STABLE);

	if (ev->x < CURRENT_X(e->text))
		index = 0;
	else if (ev->x > CURRENT_X(e->text) + CURRENT_W(e->text)) {
		index = ewl_text_length_get(EWL_TEXT(e->text));
	}
	else {
		index = ewl_text_coord_index_map(EWL_TEXT(e->text), ev->x,
						 ev->y);
	}

	/*
	 * Should begin scrolling in either direction?
	 */
	if (ev->x > CURRENT_X(e) || ev->x < CURRENT_X(e)) {
		/*
		e->start_time = ecore_time_get();
		e->timer = ecore_timer_add(0.02, ewl_entry_timer, e);
		*/
	}

	ewl_entry_selection_select_to(EWL_ENTRY_SELECTION(e->selection), index);

	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_mouse_double_click_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Event_Mouse_Down *ev;
	Ewl_Entry            *e;
	int                   len = 0;
  
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
  
	ev = ev_data;
	e = EWL_ENTRY(w);
  
	len = ewl_text_length_get(EWL_TEXT(e->text));

	if (ev->clicks == 2) {
		char   *s;
		int 	start, end;

		start = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));

		if (e->selection) {
			ewl_widget_destroy(e->selection);
			e->selection = ewl_entry_selection_new();
		}

		s = ewl_entry_text_get(e);
    
		while ((s[start] != ' ') && (s[start] != '\t')
				&& (s[start] != '\n') && (--start > 0))
			;

		if (start < 0) start++;
		end = start;

		while ((s[end + 1] != ' ') && (s[end + 1] != '\t')
				&& (s[end + 1] != '\n') && (++end < len))
			;

		if (end == len) end --; 
		ewl_entry_selection_start_position_set(
				EWL_ENTRY_SELECTION(e->selection), start);
		ewl_entry_selection_end_position_set(
				EWL_ENTRY_SELECTION(e->selection), end);
		ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), end + 1);

	} else {
		ewl_entry_selection_start_position_set(EWL_ENTRY_SELECTION(e->selection), 0);
		ewl_entry_selection_select_to(EWL_ENTRY_SELECTION(e->selection), len - 1);
		ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), len);
	}

	e->in_select_mode = (ev->modifiers & EWL_KEY_MODIFIER_SHIFT);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_select_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_widget_show(e->selection);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_deselect_cb(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Entry      *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	e = EWL_ENTRY(w);

	ewl_widget_hide(e->selection);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_cursor_left_move(Ewl_Entry * e)
{
	int             pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	pos = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));

	if (--pos < 0) pos = 0;

	if (e->in_select_mode)
		ewl_entry_selection_start_position_set(
			EWL_ENTRY_SELECTION(e->selection), pos);

	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), pos);

	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_cursor_right_move(Ewl_Entry * e)
{
	char           *str;
	int             len = 0;
	int             pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	pos = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));
	str = ewl_entry_text_get(e);

	len = strlen(str);
	FREE(str);

	if (++pos > len) pos = len;
	if (e->in_select_mode)
		ewl_entry_selection_select_to(EWL_ENTRY_SELECTION(e->selection), pos);

	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), pos);

	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Position the cursor at the current position in the next line.
 */
void ewl_entry_cursor_down_move(Ewl_Entry * e)
{
	char *s;
	int nlen, nline, nend;
	int lpos, start, len = 0;
	int bp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	if (e->multiline) {
		len = ewl_text_length_get(EWL_TEXT(e->text));
		nline = start = bp =
			ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));

		s = ewl_entry_text_get(e);

		while( --start > 1 && s[start] != '\n' )
			;

		if( s[start] == '\n' )
			start++;

		lpos = bp - start - 1;

		while( nline < len && s[nline++] != '\n' )
			;

		nend = nline;

		while( nend < len && s[nend++] != '\n' )
			;

		nlen = nend - nline;

		if( nlen >= lpos )
			lpos += nline + 1;
		else
			lpos = nend - 1;

		if (e->in_select_mode)
			ewl_entry_selection_select_to(EWL_ENTRY_SELECTION(e->selection), lpos);

		ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), lpos);

		ewl_widget_configure(EWL_WIDGET(e));
	}
}

/*
 * Position the cursor at the current position in the previous line.
 */
void ewl_entry_cursor_up_move(Ewl_Entry * e)
{
	char *s;
	int plen, pline, pend;
	int lpos, start, len = 0;
	int bp;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	if (e->multiline) {
		len = ewl_text_length_get(EWL_TEXT(e->text));
		start = bp = 
			ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));

		s = ewl_entry_text_get(e);

		while( --start > 1 && s[start] != '\n' )
			;

		if( s[start] == '\n' )
			start++;

		lpos = bp - start - 1;
		pline = start - 1;

		while( --pline > 1 && s[pline] != '\n' )
			;

		pend = pline;

		while( ++pend < len && s[pend] != '\n' )
			;

		plen = pend - pline;

		if( plen >= lpos )
			lpos += pline + 1;
		else
			lpos = pend - 1;

		if (e->in_select_mode)
			ewl_entry_selection_select_to(EWL_ENTRY_SELECTION(e->selection), lpos);
		
		ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), lpos);

		ewl_widget_configure(EWL_WIDGET(e));
	}
}

/*
 * Position the cursor at the beginning of the widget. This is internal, so the
 * parameter is not checked.
 */
void ewl_entry_cursor_home_move(Ewl_Entry * e)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	if (e->in_select_mode)
		ewl_entry_selection_start_position_set(EWL_ENTRY_SELECTION(e->selection), 0);

	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), 0);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * Position the cursor at the end of the widget. This is internal, so the
 * parameter is not checked.
 */
void ewl_entry_cursor_end_move(Ewl_Entry * e)
{
	char           *s;
	int             l = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = ewl_entry_text_get(e);

	if (s) {
		l = strlen(s);
		FREE(s);
	}

	if (e->in_select_mode)
		ewl_entry_selection_select_to(EWL_ENTRY_SELECTION(e->selection), l);

	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), l);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_text_insert(Ewl_Entry * e, char *s)
{
	char           *s2, *s3;
	int             l = 0, l2 = 0, sp = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_PARAM_PTR("s", s);

	s2 = ewl_entry_text_get(e);
	l = strlen(s);

	if (s2)
		l2 = strlen(s2);
	else
		l2 = 0;

	if (e->selection)
		sp = ewl_entry_selection_start_position_get(EWL_ENTRY_SELECTION(e->selection));
	else
		sp = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));

	s3 = NEW(char, l + 1 + l2);
	if (!s3) {
		IF_FREE(s2);
		DRETURN(DLEVEL_STABLE);
	}

	if (s2) strncat(s3, s2, sp);
	strcat(s3, s);

	if (e->selection)
		sp = ewl_entry_selection_end_position_get(EWL_ENTRY_SELECTION(e->selection));

	if (s2) strcat(s3, &(s2[sp]));

	ewl_entry_text_set(e, s3);
	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), ++sp);

	IF_FREE(s2);
	FREE(s3);

	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_left_delete(Ewl_Entry * e)
{
	char           *s;
	unsigned int    sp, ep, len;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	if (e->selection) {
		sp = ewl_entry_selection_start_position_get(EWL_ENTRY_SELECTION(e->selection));
		ep = ewl_entry_selection_end_position_get(EWL_ENTRY_SELECTION(e->selection));
	} else 
		sp = ep = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));

	s = ewl_entry_text_get(e);
	if (!s || !strlen(s) || (!sp && sp == ep))
		DRETURN(DLEVEL_STABLE);

	if (!sp) sp ++;

	strcpy(&(s[sp - 1]), &(s[ep]));
	ewl_entry_text_set(e, s);
	len = strlen(s);
	FREE(s);

	if (--sp > len) sp = len;
	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), sp);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_entry_right_delete(Ewl_Entry * e)
{
	char           *s;
	int             sp, ep, len;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	if (e->selection) {
		sp = ewl_entry_selection_start_position_get(EWL_ENTRY_SELECTION(e->selection));
		ep = ewl_entry_selection_end_position_get(EWL_ENTRY_SELECTION(e->selection));
	} else 
		sp = ep = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));
	
	s = ewl_entry_text_get(e);
	if (!s) DRETURN(DLEVEL_STABLE);
	len = strlen(s);

	if (!len || (sp == len))
		DRETURN(DLEVEL_STABLE);

	strcpy(&(s[sp]), &(s[ep + 1]));
	ewl_entry_text_set(e, s);

	len = strlen(s);
	if (sp > len) sp = len;

	FREE(s);

	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), sp);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_word_begin_delete(Ewl_Entry * e)
{
	char           *s;
	int             bp, index;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);

	s = ewl_entry_text_get(e);
	if (!s)
		DRETURN(DLEVEL_STABLE);

	bp = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));
	index = bp-2;
	
	while ((index-->0) && (s[index] == ' ')){}
	if (index < 0)
		index = 0;
	while ((index-->0) && (s[index] != ' ')){}
	index++;
	strcpy(&(s[index]), &(s[bp]));
	ewl_entry_text_set(e, s);

	FREE(s);

	if (index <= 0) 
	  index = 1;

	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), index);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_child_show_cb(Ewl_Container * c, Ewl_Widget * w)
{
	Ewl_Entry *e;

	DENTER_FUNCTION(DLEVEL_STABLE);

	e = EWL_ENTRY(c);

	if (e->text == w) {
		ewl_object_preferred_inner_size_set(EWL_OBJECT(c),
			   ewl_object_preferred_w_get(EWL_OBJECT(w)),
			   ewl_object_preferred_h_get(EWL_OBJECT(w)));
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
		ewl_object_preferred_inner_w_set(EWL_OBJECT(entry),
			   ewl_object_preferred_w_get(text));
	else
		ewl_object_preferred_inner_h_set(EWL_OBJECT(entry),
			   ewl_object_preferred_h_get(text));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int ewl_entry_timer(void *data)
{
	Ewl_Entry      *e;
	double          dt;
	double          value;
	int             velocity, direction, tmp;

	e = EWL_ENTRY(data);

	dt = ecore_time_get() - e->start_time;
	
	direction = CURRENT_X(e->cursor) - CURRENT_X(e);
	tmp = (CURRENT_X(e) + CURRENT_W(e)) - (CURRENT_X(e->cursor) +
			CURRENT_W(e->cursor));
	if (direction < tmp)
		direction = -1;
	else
		direction = 1;

	/*
	 * Check the theme for a velocity setting and bring it within normal
	 * useable bounds.
	 */
	velocity = ewl_theme_data_int_get(EWL_WIDGET(e), "velocity");
	if (velocity < 1)
		velocity = 1;
	else if (velocity > 10)
		velocity = 10;

	/*
	 * Move the value of the seeker based on the direction of it's motion
	 * and the velocity setting.
	 */
	value = (double)(direction) * 10.0 * (1 - exp(-dt)) *
		 ((double)(velocity) / 100.0);

	e->offset = value * ewl_object_current_w_get(EWL_OBJECT(e->text));

	return 1;
}

/*
 * Cursor functions
 */
static Ewl_Widget *
ewl_entry_cursor_new(void)
{
	Ewl_Entry_Cursor *c;

	DENTER_FUNCTION(DLEVEL_STABLE);

	c = NEW(Ewl_Entry_Cursor, 1);
	if (!c)
		return NULL;

	ewl_entry_cursor_init(c);

	DRETURN_PTR(EWL_WIDGET(c), DLEVEL_STABLE);
}

static void
ewl_entry_cursor_init(Ewl_Entry_Cursor *c)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	w = EWL_WIDGET(c);
	ewl_widget_init(w, "cursor");
	c->position = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_entry_cursor_position_set(Ewl_Entry_Cursor *c, int pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);

	c->position = pos;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_entry_cursor_position_get(Ewl_Entry_Cursor *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, 0);

	DRETURN_INT(c->position, DLEVEL_STABLE);
}

/*
 * Selection functions
 */
static Ewl_Widget *
ewl_entry_selection_new(void)
{
	Ewl_Entry_Selection *s;

	DENTER_FUNCTION(DLEVEL_STABLE);

	s = NEW(Ewl_Entry_Selection, 1);
	if (!s)
		return NULL;

	ewl_entry_selection_init(s);

	DRETURN_PTR(EWL_WIDGET(s), DLEVEL_STABLE);
}

static void
ewl_entry_selection_init(Ewl_Entry_Selection *s)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	w = EWL_WIDGET(s);
	ewl_widget_init(w, "selection");
	s->start = 0;
	s->end = 0;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_entry_selection_start_position_set(Ewl_Entry_Selection *s, 
						unsigned int start)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	s->start = start;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_entry_selection_start_position_get(Ewl_Entry_Selection *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0);

	DRETURN_INT(s->start, DLEVEL_STABLE);
}

static void
ewl_entry_selection_end_position_set(Ewl_Entry_Selection *s,
						unsigned int end)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	s->end = end;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_entry_selection_end_position_get(Ewl_Entry_Selection *s)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("s", s, 0);

	DRETURN_INT(s->end, DLEVEL_STABLE);
}

static void
ewl_entry_selection_select_to(Ewl_Entry_Selection *s,
					unsigned int pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("s", s);

	if (pos < s->start)
		s->start = pos;
	else 
		s->end = pos;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}



