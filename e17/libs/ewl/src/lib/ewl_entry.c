/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_entry.h"
#include "ewl_text_trigger.h"
#include <Ecore_Txt.h>
#include "ewl_macros.h"
#include "ewl_private.h"
#if HAVE_LANGINFO_H
# include <langinfo.h>
#endif /* HAVE_LANGINFO_H */
#include "ewl_debug.h"

#ifdef HAVE_EVIL
# include <Evil.h>
#endif /* HAVE_EVIL */


static Ewl_Widget *ewl_entry_view_cb_widget_fetch(void *data, unsigned int row,
                                                        unsigned int col);
static Ewl_Widget *ewl_entry_view_cb_header_fetch(void *data, unsigned int col);


/**
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
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_entry_init(EWL_ENTRY(w)))
        {
                ewl_widget_destroy(w);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @return Returns a new password widget on success, NULL on failure.
 * @brief Allocate and initialize a new password widget
 *
 * A password widget is an entry with a set obscure character. The default is a
 * star (*)
 */
Ewl_Widget *
ewl_password_new(void)
{
        Ewl_Widget *e;

        DENTER_FUNCTION(DLEVEL_STABLE);

        e = ewl_entry_new();
        ewl_widget_appearance_set(e, "password/"EWL_ENTRY_TYPE);
        ewl_text_obscure_set(EWL_TEXT(e), "*");

        DRETURN_PTR(e, DLEVEL_STABLE);
}

/**
 * @param e: The Ewl_Entry to initialize
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes an Ewl_Entry widget to default values
 */
int
ewl_entry_init(Ewl_Entry *e)
{
        const char *text_types[] = { "UTF8_STRING", "text/plain", NULL };
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(e, FALSE);

        w = EWL_WIDGET(e);

        if (!ewl_text_init(EWL_TEXT(e)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_inherit(w, EWL_ENTRY_TYPE);
        ewl_widget_appearance_set(w, EWL_ENTRY_TYPE);
        ewl_widget_focusable_set(EWL_WIDGET(e), TRUE);

        ewl_object_fill_policy_set(EWL_OBJECT(e), EWL_FLAG_FILL_HSHRINK |
                                                  EWL_FLAG_FILL_HFILL);

        ewl_container_callback_intercept(EWL_CONTAINER(w), EWL_CALLBACK_FOCUS_IN);
        ewl_container_callback_intercept(EWL_CONTAINER(w), EWL_CALLBACK_FOCUS_OUT);
        ewl_container_callback_intercept(EWL_CONTAINER(w), EWL_CALLBACK_DND_POSITION);
        ewl_container_callback_intercept(EWL_CONTAINER(w), EWL_CALLBACK_DND_DATA_RECEIVED);

        /* setup the cursor */
        e->cursor = ewl_entry_cursor_new(e);
        ewl_container_child_append(EWL_CONTAINER(e), e->cursor);
        ewl_widget_internal_set(e->cursor, TRUE);
        ewl_object_fill_policy_set(EWL_OBJECT(e->cursor), EWL_FLAG_FILL_VFILL);

        /* Set the pointer */
        ewl_attach_mouse_cursor_set(EWL_WIDGET(e), EWL_MOUSE_CURSOR_XTERM);

        /* this has to be called after the cursor is created as it will try
         * to show the cursor */
        ewl_entry_multiline_set(e, FALSE);
        ewl_entry_editable_set(e, TRUE);
        ewl_text_selectable_set(EWL_TEXT(e), TRUE);
        ewl_dnd_accepted_types_set(EWL_WIDGET(e), text_types);

        /* setup callbacks */
        ewl_callback_append(w, EWL_CALLBACK_FOCUS_IN,
                                ewl_entry_cb_focus_in, NULL);
        ewl_callback_append(w, EWL_CALLBACK_FOCUS_OUT,
                                ewl_entry_cb_focus_out, NULL);
        ewl_callback_prepend(w, EWL_CALLBACK_CONFIGURE,
                                ewl_entry_cb_configure, NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_DOWN,
                                ewl_entry_cb_mouse_down, NULL);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_UP,
                                ewl_entry_cb_mouse_up, NULL);
        ewl_callback_append(w, EWL_CALLBACK_WIDGET_DISABLE,
                                ewl_entry_cb_disable, NULL);
        ewl_callback_append(w, EWL_CALLBACK_WIDGET_ENABLE,
                                ewl_entry_cb_enable, NULL);
        ewl_callback_append(w, EWL_CALLBACK_DND_POSITION,
                                ewl_entry_cb_dnd_position, NULL);
        ewl_callback_append(w, EWL_CALLBACK_DND_DATA_RECEIVED,
                                ewl_entry_cb_dnd_data, NULL);

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
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_ENTRY_TYPE);

        e->multiline = !!multiline;

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
        DCHECK_PARAM_PTR_RET(e, 0);
        DCHECK_TYPE_RET(e, EWL_ENTRY_TYPE, 0);

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
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_ENTRY_TYPE);

        if (e->editable == !!editable)
                DRETURN(DLEVEL_STABLE);

        e->editable = !!editable;

        if (e->editable)
        {
                ewl_callback_append(EWL_WIDGET(e), EWL_CALLBACK_KEY_DOWN,
                                                ewl_entry_cb_key_down, NULL);

                if (ewl_widget_state_has(EWL_WIDGET(e), EWL_FLAG_STATE_FOCUSED))
                        ewl_widget_show(e->cursor);

                ewl_widget_state_set(EWL_WIDGET(e), "enabled",
                                        EWL_STATE_PERSISTENT);
        }
        else
        {
                ewl_callback_del(EWL_WIDGET(e), EWL_CALLBACK_KEY_DOWN,
                                                ewl_entry_cb_key_down);

                if (ewl_widget_state_has(EWL_WIDGET(e), EWL_FLAG_STATE_FOCUSED))
                        ewl_widget_hide(e->cursor);

                ewl_widget_state_set(EWL_WIDGET(e), "disabled",
                                        EWL_STATE_PERSISTENT);
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
        DCHECK_PARAM_PTR_RET(e, 0);
        DCHECK_TYPE_RET(e, EWL_ENTRY_TYPE, 0);

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
        DCHECK_PARAM_PTR_RET(e, FALSE);
        DCHECK_TYPE_RET(e, EWL_ENTRY_TYPE, FALSE);

        sel = EWL_TEXT_TRIGGER(ewl_text_selection_get(EWL_TEXT(e)));
        if (sel)
        {
                unsigned int len, pos;

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

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_entry_cb_configure(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Entry *e;
        unsigned int c_pos;
        int cx = 0, cy = 0, cw = 0, ch = 0;
        int ox = 0, oy = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_ENTRY_TYPE);

        e = EWL_ENTRY(w);
        if (!e->editable)
                DRETURN(DLEVEL_STABLE);

        c_pos = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));
        ewl_text_index_geometry_map(EWL_TEXT(e), c_pos, &cx, &cy, &cw, &ch);

        if (!cw) cw = CURRENT_W(e->cursor);
        if (!ch) ch = CURRENT_H(e->cursor);

        ox = (cx + cw) - (CURRENT_X(e) + CURRENT_W(e));
        oy = (cy + ch) - (CURRENT_Y(e) + CURRENT_H(e));

        if (ox < 0)
                ox = 0;
        if (oy < 0)
                oy = 0;

        ewl_text_offsets_set(EWL_TEXT(e), -ox, -oy);

        ewl_object_geometry_request(EWL_OBJECT(e->cursor), cx, cy, cw, ch);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The focus in callback
 */
void
ewl_entry_cb_focus_in(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Entry *entry;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_ENTRY_TYPE);

        entry = EWL_ENTRY(w);
        if (entry->editable && !VISIBLE(entry->cursor))
                ewl_widget_show(entry->cursor);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The focus out callback
 */
void
ewl_entry_cb_focus_out(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Entry *entry;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_ENTRY_TYPE);

        entry = EWL_ENTRY(w);
        if (entry->editable && VISIBLE(entry->cursor))
                ewl_widget_hide(entry->cursor);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: The Ewl_Event_Key_Down data
 * @param data: UNUSED
 * @return Returns no value
 * @brief The key down callback
 */
void
ewl_entry_cb_key_down(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Event_Key *event;
        Ewl_Entry *e;
        Ewl_Text_Trigger *sel;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_ENTRY_TYPE);

        event = ev;
        e = EWL_ENTRY(w);
        sel = EWL_TEXT_TRIGGER(EWL_TEXT(w)->selection);

        /* reset the cursor blink */
        ewl_widget_state_set(EWL_WIDGET(e->cursor), "noblink",
                                EWL_STATE_PERSISTENT);

        if ((!event->keyname) || (!event->keyname[0]))
                DRETURN(DLEVEL_STABLE);

        /* If we are holding shift and key pressed isn't a character,
         * then this is a selection event
         */
        if ((event->modifiers & EWL_KEY_MODIFIER_SHIFT) &&
                        ((event->keyname[1] != '\0') &&
                        (event->keyname[0] >= 0)))
                DRETURN(DLEVEL_STABLE);

        if (!strcmp(event->keyname, "Left"))
        {
                if (sel) ewl_text_trigger_length_set(sel, 0);
                ewl_entry_cursor_move_left(e);
        }

        else if (!strcmp(event->keyname, "Right"))
        {
                if (sel) ewl_text_trigger_length_set(sel, 0);
                ewl_entry_cursor_move_right(e);
        }

        else if (!strcmp(event->keyname, "Up"))
        {
                if (sel) ewl_text_trigger_length_set(sel, 0);
                ewl_entry_cursor_move_up(e);
        }

        else if (!strcmp(event->keyname, "Down"))
        {
                if (sel) ewl_text_trigger_length_set(sel, 0);
                ewl_entry_cursor_move_down(e);
        }

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
                        || (!strcmp(event->keyname, "KP_Enter"))
                        || (!strcmp(event->keyname, "\n")))
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
        else if (!(event->modifiers & EWL_KEY_MODIFIER_CTRL))
        {
                ewl_entry_selection_clear(e);

                if ((strlen(event->keyname) == 1) || (*event->keyname < 0))
                        ewl_text_text_insert(EWL_TEXT(e), event->keyname,
                                ewl_entry_cursor_position_get(
                                        EWL_ENTRY_CURSOR(e->cursor)));
        }

        if (sel)
                ewl_text_trigger_base_set(sel,
                                ewl_entry_cursor_position_get(
                                        EWL_ENTRY_CURSOR(e->cursor)));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: The Ewl_Event_Mouse_Down data
 * @param data: UNUSED
 * @return Returns no value
 * @brief The mouse down callback
 */
void
ewl_entry_cb_mouse_down(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Event_Mouse *event;
        Ewl_Entry *e;
        unsigned int idx = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_ENTRY_TYPE);

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

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The mouse up callback
 */
void
ewl_entry_cb_mouse_up(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Entry *e;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_ENTRY_TYPE);

        e = EWL_ENTRY(w);

        e->in_select_mode = FALSE;
        ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE, ewl_entry_cb_mouse_move);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The disable callback
 */
void
ewl_entry_cb_disable(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_ENTRY_TYPE);

        ewl_entry_editable_set(EWL_ENTRY(w), FALSE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The enable callback
 */
void
ewl_entry_cb_enable(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_ENTRY_TYPE);

        ewl_entry_editable_set(EWL_ENTRY(w), TRUE);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The mouse move callback
 */
void
ewl_entry_cb_mouse_move(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Event_Mouse *event;
        unsigned int idx = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_ENTRY_TYPE);

        event = ev;
        idx = ewl_text_coord_index_map(EWL_TEXT(w), event->x, event->y);
        ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(EWL_ENTRY(w)->cursor),
                        idx);
        ewl_widget_configure(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: DND position event
 * @param data: UNUSED
 * @return Returns no value
 * @brief The dnd mouse move callback
 */
void
ewl_entry_cb_dnd_position(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Event_Dnd_Position *event;
        Ewl_Text *txt;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(ev);
        DCHECK_TYPE(w, EWL_ENTRY_TYPE);

        event = ev;
        txt = EWL_TEXT(w);

        if (EWL_ENTRY(w)->editable && !DISABLED(w)) {
                ewl_widget_focus_send(w);
                ewl_text_cursor_position_set(txt,
                                ewl_text_coord_index_map(txt, event->x,
                                        event->y));
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: DND data event
 * @param data: UNUSED
 * @return Returns no value
 * @brief The dnd mouse move callback
 */
void
ewl_entry_cb_dnd_data(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Event_Dnd_Data_Received *event;
        Ewl_Text *txt;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(ev);
        DCHECK_TYPE(w, EWL_ENTRY_TYPE);

        event = ev;
        txt = EWL_TEXT(w);

        if (EWL_ENTRY(w)->editable && !DISABLED(w)) {
                if (!strcmp(event->type, "text/plain")
                                && strcmp(nl_langinfo(CODESET), "UTF-8")) {
                        char *text;

                        text = ecore_txt_convert(nl_langinfo(CODESET), "UTF-8",
                                        event->data);
                        if (text) {
                                ewl_text_text_insert(txt, text,
                                        ewl_text_cursor_position_get(txt));
                        }
                        else {
                                ewl_text_text_insert(txt, event->data,
                                        ewl_text_cursor_position_get(txt));
                        }

                        IF_FREE(text);
                }
                else
                        ewl_text_text_insert(txt, event->data,
                                        ewl_text_cursor_position_get(txt));
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: The Ewl_Entry to work with
 * @return Returns no value
 * @brief Moves the cursor to the left of it's current position
 */
void
ewl_entry_cursor_move_left(Ewl_Entry *e)
{
        unsigned int pos;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_ENTRY_TYPE);

        pos = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));

        /* we're already at the start */
        if (pos == 0)
                DRETURN(DLEVEL_STABLE);

        ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), pos - 1);
        ewl_widget_configure(EWL_WIDGET(e));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: The Ewl_Entry to work with
 * @return Returns no value.
 * @brief Moves the cursor to the right of its current position
 */
void
ewl_entry_cursor_move_right(Ewl_Entry *e)
{
        unsigned int pos;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_ENTRY_TYPE);

        pos = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));

        /* if we are already at the end, stop */
        if (pos == ewl_text_length_get(EWL_TEXT(e)))
                DRETURN(DLEVEL_STABLE);

        ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), pos + 1);
        ewl_widget_configure(EWL_WIDGET(e));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: The Ewl_Entry to work with
 * @return Returns no value.
 * @brief Moves the cursor up one line from its current position
 */
void
ewl_entry_cursor_move_up(Ewl_Entry *e)
{
        unsigned int current_pos = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_ENTRY_TYPE);

        current_pos = ewl_text_cursor_position_line_up_get(EWL_TEXT(e));
        ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), current_pos);
        ewl_widget_configure(EWL_WIDGET(e));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: The Ewl_Entry to work with
 * @return Returns no value.
 * @brief Moves the cursor down one line from its current position
 */
void
ewl_entry_cursor_move_down(Ewl_Entry *e)
{
        unsigned int current_pos = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_ENTRY_TYPE);

        current_pos = ewl_text_cursor_position_line_down_get(EWL_TEXT(e));
        ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), current_pos);
        ewl_widget_configure(EWL_WIDGET(e));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: The Ewl_Entry to work with
 * @return Returns no value.
 * @brief Deletes the character to the left of the cursor
 */
void
ewl_entry_delete_left(Ewl_Entry *e)
{
        unsigned int pos;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_ENTRY_TYPE);

        if (!EWL_TEXT(e)->text)
                DRETURN(DLEVEL_STABLE);

        pos = ewl_entry_cursor_position_get(EWL_ENTRY_CURSOR(e->cursor));

        /* we cannot delete anything if we are at the beginning of the text */
        if (pos == 0)
                DRETURN(DLEVEL_STABLE);

        ewl_entry_cursor_position_set(EWL_ENTRY_CURSOR(e->cursor), pos - 1);
        ewl_text_text_delete(EWL_TEXT(e), 1);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param e: The Ewl_Entry to work with
 * @return Returns no value.
 * @brief Deletes the character to the right of the cursor
 */
void
ewl_entry_delete_right(Ewl_Entry *e)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(e);
        DCHECK_TYPE(e, EWL_ENTRY_TYPE);

        if (!EWL_TEXT(e)->text)
                DRETURN(DLEVEL_STABLE);

        ewl_text_text_delete(EWL_TEXT(e), 1);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * cursor stuff
 */

/**
 * @param parent: The parent Ewl_Entry widget
 * @return Returns a new Ewl_Entry_Cursor on success or NULL on failure
 * @brief Creates and initializes a new Ewl_Entry_Cursor widget
 */
Ewl_Widget *
ewl_entry_cursor_new(Ewl_Entry *parent)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);

        w = NEW(Ewl_Entry_Cursor, 1);
        if (!w)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_entry_cursor_init(EWL_ENTRY_CURSOR(w), parent))
        {
                ewl_widget_destroy(w);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param c: The Ewl_Entry_Cursor to initialize
 * @param parent: The parent Ewl_Text widget
 * @return Returns TRUE on success or FALSE on failure
 * @brief Initializes an Ewl_Entry_Cursor to default values
 */
int
ewl_entry_cursor_init(Ewl_Entry_Cursor *c, Ewl_Entry *parent)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, FALSE);
        DCHECK_PARAM_PTR_RET(parent, FALSE);
        DCHECK_TYPE_RET(parent, EWL_ENTRY_TYPE, FALSE);

        if (!ewl_widget_init(EWL_WIDGET(c)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(EWL_WIDGET(c), EWL_ENTRY_CURSOR_TYPE);
        ewl_widget_inherit(EWL_WIDGET(c), EWL_ENTRY_CURSOR_TYPE);
        c->parent = parent;

        ewl_widget_focusable_set(EWL_WIDGET(c), FALSE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param c: The cursor to work with
 * @param pos: The position to set the cursor too
 * @return Returns no value.
 * @brief Set the current cursor position
 */
void
ewl_entry_cursor_position_set(Ewl_Entry_Cursor *c, unsigned int pos)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_TYPE(c, EWL_ENTRY_CURSOR_TYPE);

        ewl_text_cursor_position_set(EWL_TEXT(c->parent), pos);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param c: The cursor to get the position from
 * @return Returns the current position of the cursor
 * @brief Retrieves the current position of the Ewl_Entry_Cursor
 */
unsigned int
ewl_entry_cursor_position_get(Ewl_Entry_Cursor *c)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, 0);
        DCHECK_TYPE_RET(c, EWL_ENTRY_CURSOR_TYPE, 0);

        DRETURN_INT(ewl_text_cursor_position_get(EWL_TEXT(c->parent)),
                                                        DLEVEL_STABLE);
}

/**
 * @return Returns a view that can be used to display Ewl_Entry widgets
 * @brief Creates and returns a view to be used by Ewl_Entry widgets
 */
Ewl_View *
ewl_entry_view_get(void)
{
        Ewl_View *view;

        DENTER_FUNCTION(DLEVEL_STABLE);

        view = ewl_view_new();
        ewl_view_widget_fetch_set(view, ewl_entry_view_cb_widget_fetch);
        ewl_view_header_fetch_set(view, ewl_entry_view_cb_header_fetch);

        DRETURN_PTR(view, DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_entry_view_cb_widget_fetch(void *data, unsigned int row __UNUSED__,
                                unsigned int col __UNUSED__)
{
        Ewl_Widget *entry;

        DENTER_FUNCTION(DLEVEL_STABLE);

        entry = ewl_entry_new();
        ewl_text_text_set(EWL_TEXT(entry), data);

        DRETURN_PTR(entry, DLEVEL_STABLE);
}

static Ewl_Widget *
ewl_entry_view_cb_header_fetch(void *data, unsigned int col __UNUSED__)
{
        Ewl_Widget *entry;

        DENTER_FUNCTION(DLEVEL_STABLE);

        entry = ewl_entry_new();
        ewl_text_text_set(EWL_TEXT(entry), data);

        DRETURN_PTR(entry, DLEVEL_STABLE);
}
