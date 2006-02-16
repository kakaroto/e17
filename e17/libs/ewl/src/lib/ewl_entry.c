#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

/**
 * @param text: The text to set into the entry
 * @return Returns a new Ewl_Widget on success or NULL on failure
 * @brief Create and return a new Ewl_Entry widget
 */
Ewl_Widget *
ewl_entry_new(void)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Entry, 1);
	if (!w)
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_entry_init(EWL_ENTRY(w)))
	{
		ewl_widget_destroy(w);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}
	DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param e: The Ewl_Entry to initialize
 * @param text: The text to put into the entry
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes an Ewl_Entry widget to default values
 */
int
ewl_entry_init(Ewl_Entry *e)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, FALSE);

	w = EWL_WIDGET(e);

	if (!ewl_text_init(EWL_TEXT(e)))
	{
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	ewl_widget_inherit(w, EWL_ENTRY_TYPE);
	ewl_widget_appearance_set(w, EWL_ENTRY_TYPE);
	ewl_widget_focusable_set(EWL_WIDGET(e), TRUE);

	ewl_object_fill_policy_set(EWL_OBJECT(e), EWL_FLAG_FILL_HSHRINK |
						  EWL_FLAG_FILL_HFILL);

	ewl_container_callback_intercept(EWL_CONTAINER(w), EWL_CALLBACK_FOCUS_IN);
	ewl_container_callback_intercept(EWL_CONTAINER(w), EWL_CALLBACK_FOCUS_OUT);

	/* setup the cursor */
	e->cursor = ewl_entry_cursor_new(e);
	ewl_container_child_append(EWL_CONTAINER(e), e->cursor);
	ewl_widget_internal_set(e->cursor, TRUE);
	ewl_object_fill_policy_set(EWL_OBJECT(e->cursor), EWL_FLAG_FILL_SHRINK);

	/* Set the pointer */
	ewl_attach_mouse_cursor_set(EWL_WIDGET(e), EWL_MOUSE_CURSOR_XTERM);

	/* this has to be called after the cursor is created as it will try
	 * to show the cursor */
	ewl_entry_editable_set(e, TRUE);
	ewl_text_selectable_set(EWL_TEXT(e), TRUE);

	/* setup callbacks */
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN,
				ewl_entry_cb_focus_in, NULL);
	ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT,
				ewl_entry_cb_focus_out, NULL);
	ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
				ewl_entry_cb_configure, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
				ewl_entry_cb_mouse_down, NULL);
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP,
				ewl_entry_cb_mouse_up, NULL);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param e: The Ewl_Entry to set the multiline status
 * @param multiline: The multiline status to set
 * @return Returns no value
 * @brief Set if the entry is multiline or not
 */
void
ewl_entry_multiline_set(Ewl_Entry *e, unsigned int multiline)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_ENTRY_TYPE);

	e->multiline = multiline;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: The Ewl_Entry to get the multiline status from
 * @return Returns the multline status of the widget
 * @brief Retrieve if the entry is multiline or not
 */
unsigned int
ewl_entry_multiline_get(Ewl_Entry *e)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, 0);
	DCHECK_TYPE_RET("e", e, EWL_ENTRY_TYPE, 0);

	DRETURN_INT(e->multiline, DLEVEL_STABLE);
}

/**
 * @param e: The Ewl_Entry to set the editable status of
 * @param editable: The value to set for the editable flag
 * @return Returns no value
 * @brief Set if the entry is editable or not
 */
void
ewl_entry_editable_set(Ewl_Entry *e, unsigned int editable)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_ENTRY_TYPE);

	if (e->editable == editable)
	{
		DRETURN(DLEVEL_STABLE);
	}

	e->editable = editable;

	if (e->editable)
	{
		ewl_callback_append(EWL_WIDGET(e), EWL_CALLBACK_KEY_DOWN,
						ewl_entry_cb_key_down, NULL);

		if (ewl_object_state_has(EWL_OBJECT(e), EWL_FLAG_STATE_FOCUSED))
			ewl_widget_show(e->cursor);

		ewl_widget_state_set(EWL_WIDGET(e), "editable");
	}
	else
	{
		ewl_callback_del(EWL_WIDGET(e), EWL_CALLBACK_KEY_DOWN,
						ewl_entry_cb_key_down);

		if (ewl_object_state_has(EWL_OBJECT(e), EWL_FLAG_STATE_FOCUSED))
			ewl_widget_hide(e->cursor);

		ewl_widget_state_set(EWL_WIDGET(e), "noteditable");
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: The Ewl_Entry to get the editable status from
 * @return Returns the editable status of the entry
 * @brief Retrieve if the entry is editable or not
 */
unsigned int
ewl_entry_editable_get(Ewl_Entry *e)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, 0);
	DCHECK_TYPE_RET("e", e, EWL_ENTRY_TYPE, 0);

	DRETURN_INT(e->editable, DLEVEL_STABLE);
}

/**
 * @param e: The entry to clear the selection of
 * @return Returns TRUE if a selection was cleared, FALSE otherwise.
 * @brief Clear the current selection in the entry
 */
unsigned int 
ewl_entry_selection_clear(Ewl_Entry *e)
{
	Ewl_Text_Trigger *sel;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("e", e, FALSE);
	DCHECK_TYPE_RET("e", e, EWL_ENTRY_TYPE, FALSE);

	sel = ewl_text_selection_get(EWL_TEXT(e));
	if (sel)
	{
		int len, pos;

		len = ewl_text_trigger_length_get(sel);
		pos = ewl_text_trigger_start_pos_get(sel);
		ewl_text_cursor_position_set(EWL_TEXT(e), pos);
		ewl_text_text_delete(EWL_TEXT(e), len);

		/* remove the selection */
		ewl_text_trigger_length_set(sel, 0);

		DRETURN_INT(TRUE, DLEVEL_STABLE);
	}

	DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/*
 * internal stuff
 */
void
ewl_entry_cb_configure(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Entry *e;
	unsigned int c_pos;
	int cx = 0, cy = 0, cw = 0, ch = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	e = EWL_ENTRY(w);
	if (!e->editable)
	{
		DRETURN(DLEVEL_STABLE);
	}

	c_pos = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));
	ewl_text_index_geometry_map(EWL_TEXT(e), c_pos, &cx, &cy, &cw, &ch);

	if (!cw) cw = CURRENT_W(e->cursor);
	if (!ch) ch = CURRENT_H(e->cursor);

	ewl_object_geometry_request(EWL_OBJECT(e->cursor), cx, cy, cw, ch);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void 
ewl_entry_cb_focus_in(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Entry *entry;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	entry = EWL_ENTRY(w);
	if (entry->editable && !VISIBLE(entry->cursor))
		ewl_widget_show(entry->cursor);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void 
ewl_entry_cb_focus_out(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Entry *entry;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	entry = EWL_ENTRY(w);
	if (entry->editable && VISIBLE(entry->cursor))
		ewl_widget_hide(entry->cursor);	

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_cb_key_down(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
	Ewl_Event_Key_Down *event;
	Ewl_Entry *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	event = ev;
	e = EWL_ENTRY(w);

	/* reset the cursor blink */
	ewl_widget_state_set(EWL_WIDGET(e->cursor), "noblink");

	if (!strcmp(event->keyname, "Left"))
		ewl_entry_cursor_move_left(e);

	else if (!strcmp(event->keyname, "Right"))
		ewl_entry_cursor_move_right(e);

	else if (!strcmp(event->keyname, "Up"))
		ewl_entry_cursor_move_up(e);

	else if (!strcmp(event->keyname, "Down"))
		ewl_entry_cursor_move_down(e);

	else if (!strcmp(event->keyname, "BackSpace"))
	{
		if (!ewl_entry_selection_clear(e))
			ewl_entry_delete_left(e);
	}
	else if (!strcmp(event->keyname, "Delete"))
	{
		if (!ewl_entry_selection_clear(e))
			ewl_entry_delete_right(e);
	}
	else if ((!strcmp(event->keyname, "Return")) 
			|| (!strcmp(event->keyname, "KP_Return"))
			|| (!strcmp(event->keyname, "Enter"))
			|| (!strcmp(event->keyname, "KP_Enter")))
	{
		if (!e->multiline)
			ewl_callback_call(w, EWL_CALLBACK_VALUE_CHANGED);
		else
		{
			ewl_entry_selection_clear(e);

			ewl_text_text_insert(EWL_TEXT(e), "\n", 
				ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor)));
		}
	}
	else if ((event->keyname) && (strlen(event->keyname) == 1))
	{
		char *tmp;

		ewl_entry_selection_clear(e);

		tmp = calloc(2, sizeof(char));
		snprintf(tmp, 2, "%s", event->keyname);
		ewl_text_text_insert(EWL_TEXT(e), tmp,
				ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor)));
		FREE(tmp);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_cb_mouse_down(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
	Ewl_Event_Mouse_Down *event;
	Ewl_Entry *e;
	unsigned int idx = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	event = ev;
	e = EWL_ENTRY(w);

	e->in_select_mode = TRUE;
	ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE, 
				ewl_entry_cb_mouse_move, NULL);

	idx = ewl_text_coord_index_map(EWL_TEXT(e), event->x, event->y);
	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), idx);
	ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_cb_mouse_up(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	Ewl_Entry *e;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	e = EWL_ENTRY(w);

	e->in_select_mode = FALSE;
	ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE, ewl_entry_cb_mouse_move);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_cb_mouse_move(Ewl_Widget *w, void *ev __UNUSED__, 
					void *data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	/* XXX do we leave the cursor at the start? or move to the end? */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_cursor_move_left(Ewl_Entry *e)
{
	unsigned int pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_ENTRY_TYPE);

	pos = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));

	/* we're already at the start */
	if (pos == 0)
		DRETURN(DLEVEL_STABLE);

	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), pos - 1);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_cursor_move_right(Ewl_Entry *e)
{
	unsigned int pos;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_ENTRY_TYPE);

	pos = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));

	/* if we are already at the end, stop */
	if (pos == ewl_text_length_get(EWL_TEXT(e)))
		DRETURN(DLEVEL_STABLE);

	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), pos + 1);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_cursor_move_up(Ewl_Entry *e)
{
	unsigned int current_pos = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_ENTRY_TYPE);

	current_pos = ewl_text_cursor_position_line_up_get(EWL_TEXT(e));
	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), current_pos);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_cursor_move_down(Ewl_Entry *e)
{
	unsigned int current_pos = 0;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_ENTRY_TYPE);

	current_pos = ewl_text_cursor_position_line_down_get(EWL_TEXT(e));
	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), current_pos);
	ewl_widget_configure(EWL_WIDGET(e));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_delete_left(Ewl_Entry *e)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_ENTRY_TYPE);

	ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor),
		ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor)) - 1);
	ewl_text_text_delete(EWL_TEXT(e), 1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_entry_delete_right(Ewl_Entry *e)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("e", e);
	DCHECK_TYPE("e", e, EWL_ENTRY_TYPE);

	ewl_text_text_delete(EWL_TEXT(e), 1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * cursor stuff
 */
Ewl_Widget *
ewl_entry_cursor_new(Ewl_Entry *parent)
{
	Ewl_Widget *w;

	DENTER_FUNCTION(DLEVEL_STABLE);
	
	w = NEW(Ewl_Entry_Cursor, 1);
	if (!w)
	{
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}

	if (!ewl_entry_cursor_init(EWL_ENTRY_CURSOR(w), parent))
	{
		ewl_widget_destroy(w);
		DRETURN_PTR(NULL, DLEVEL_STABLE);
	}
	DRETURN_PTR(w, DLEVEL_STABLE);
}

int
ewl_entry_cursor_init(Ewl_Entry_Cursor *c, Ewl_Entry *parent)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, FALSE);
	DCHECK_PARAM_PTR_RET("parent", parent, FALSE);
	DCHECK_TYPE_RET("parent", parent, EWL_ENTRY_TYPE, FALSE);

	if (!ewl_widget_init(EWL_WIDGET(c)))
	{
		DRETURN_INT(FALSE, DLEVEL_STABLE);
	}

	ewl_widget_appearance_set(EWL_WIDGET(c), EWL_CURSOR_TYPE);
	ewl_widget_inherit(EWL_WIDGET(c), EWL_CURSOR_TYPE);
	c->parent = parent;

	ewl_widget_focusable_set(EWL_WIDGET(c), FALSE);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void
ewl_entry_cursor_position_set(Ewl_Entry_Cursor *c, unsigned int pos)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("c", c);
	DCHECK_TYPE("c", c, EWL_CURSOR_TYPE);

	ewl_text_cursor_position_set(EWL_TEXT(c->parent), pos);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

unsigned int
ewl_entry_cursor_position_get(Ewl_Entry_Cursor *c)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("c", c, 0);
	DCHECK_TYPE_RET("c", c, EWL_CURSOR_TYPE, 0);

	DRETURN_INT(ewl_text_cursor_position_get(EWL_TEXT(c->parent)), 
							DLEVEL_STABLE);
}

