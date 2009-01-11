/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_text.h"
#include "ewl_text_context.h"
#include "ewl_text_fmt.h"
#include "ewl_text_trigger.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

#include <Evas.h>

static Ewl_Text_Context *ewl_text_default_context = NULL;

/* how much do we extend the text by when we need more space? */
#define EWL_TEXT_EXTEND_VAL  4096

static const char ewl_text_trailing_bytes[32] =
{
        1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 
        1,1,1,1, 1,1,1,1, 2,2,2,2, 3,3,4,6
};

/* returns length of the next utf-8 sequence */
#define EWL_TEXT_CHAR_BYTE_LEN(s) \
        (ewl_text_trailing_bytes[((unsigned char)((s)[0])) >> 3])

static void ewl_text_current_fmt_set(Ewl_Text *t, unsigned int context_mask,
                                                Ewl_Text_Context *change);

static void ewl_text_text_insert_private(Ewl_Text *t, const char *txt,
                                unsigned int char_idx, unsigned int *char_len,
                                unsigned int *byte_len);
static int ewl_text_char_utf8_is(const char *c);

static void ewl_text_size(Ewl_Text *t);
static void ewl_text_display(Ewl_Text *t);
static void ewl_text_minmax_size_update(Ewl_Text *t);
static void ewl_text_cb_format(Ewl_Text_Fmt_Node *node, Ewl_Text *t,
                                                unsigned int byte_idx);
static void ewl_text_plaintext_parse(Evas_Object *tb, char *txt);

static Evas_Textblock_Cursor *ewl_text_textblock_cursor_position(Ewl_Text *t,
                                                        unsigned int char_idx);
static unsigned int ewl_text_textblock_cursor_to_index(
                                                Evas_Textblock_Cursor *cursor);

static void ewl_text_triggers_remove(Ewl_Text *t);
static void ewl_text_triggers_shift(Ewl_Text *t, unsigned int char_pos,
                                unsigned int char_len, unsigned int del);
static void ewl_text_trigger_position(Ewl_Text *t, Ewl_Text_Trigger *trig);

static void ewl_text_trigger_add(Ewl_Text *t, Ewl_Text_Trigger *trigger);

static Ewl_Widget *ewl_text_selection_new(Ewl_Text *t);
static void ewl_text_selection_select_to(Ewl_Text_Trigger *s,
                                                unsigned int char_idx);

static void ewl_text_theme_color_get(Ewl_Text *t, Ewl_Color_Set *color, char *name);
static Ewl_Text_Context *ewl_text_context_default_create(Ewl_Text *t);

static void ewl_text_triggers_place(Ewl_Text *t);
static void ewl_text_triggers_unrealize(Ewl_Text *t);
static void ewl_text_triggers_show(Ewl_Text *t);
static void ewl_text_triggers_hide(Ewl_Text *t);
static unsigned int  ewl_text_drawn_byte_to_char(Ewl_Text *t, unsigned int byte_idx);
static unsigned int  ewl_text_char_to_drawn_byte(Ewl_Text *t, unsigned int char_idx);

/**
 * @return Returns a new Ewl_Text widget on success, NULL on failure.
 * @brief Creates a new Ewl_Text widget
 */
Ewl_Widget *
ewl_text_new(void)
{
        Ewl_Widget *w;
        DENTER_FUNCTION(DLEVEL_STABLE);

        w = NEW(Ewl_Text, 1);
        if (!w)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_text_init(EWL_TEXT(w)))
        {
                ewl_widget_destroy(w);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text widget
 * @return Returns TRUE on successfully init or FALSE on failure
 * @brief Initializes an Ewl_Text widget to default values
 */
int
ewl_text_init(Ewl_Text *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, FALSE);

        if (!ewl_container_init(EWL_CONTAINER(t)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(EWL_WIDGET(t), EWL_TEXT_TYPE);
        ewl_widget_inherit(EWL_WIDGET(t), EWL_TEXT_TYPE);

        ewl_object_fill_policy_set(EWL_OBJECT(t), EWL_FLAG_FILL_HFILL
                                                | EWL_FLAG_FILL_VFILL);

        t->formatting.nodes = ewl_text_fmt_new(t);
        if (!t->formatting.nodes)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        t->formatting.tx = ewl_text_context_default_create(t);
        ewl_text_context_acquire(t->formatting.tx);

        ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_CONFIGURE,
                                        ewl_text_cb_configure, NULL);
        ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_REVEAL,
                                        ewl_text_cb_reveal, NULL);
        ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_REALIZE,
                                        ewl_text_cb_realize, NULL);
        ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_OBSCURE,
                                        ewl_text_cb_obscure, NULL);
        ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_SHOW,
                                        ewl_text_cb_show, NULL);
        ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_HIDE,
                                        ewl_text_cb_hide, NULL);
        ewl_callback_prepend(EWL_WIDGET(t), EWL_CALLBACK_DESTROY,
                                        ewl_text_cb_destroy, NULL);

        ewl_container_add_notify_set(EWL_CONTAINER(t),
                                        ewl_text_cb_child_add);
        ewl_container_remove_notify_set(EWL_CONTAINER(t),
                                        ewl_text_cb_child_remove);

        t->dirty = TRUE;

        /* text consumes tabs by default */
//        ewl_widget_ignore_focus_change_set(EWL_WIDGET(t), TRUE);
        ewl_widget_focusable_set(EWL_WIDGET(t), FALSE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the length from
 * @return Returns the character length of the text in the widget @a t
 * @brief Retrieve the character length of the text
 */
unsigned int
ewl_text_length_get(Ewl_Text *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        DRETURN_INT(t->length.chars, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the maximum number of characters
 * @param char_num: The maximum number of characters
 * @return Returns no value
 * @brief Set the maximum number of characters
 *
 * This function set the maximum number of characters that can be insert into
 * the text. The number of characters is unlimited if the value is equal to 0.
 * If there is already text inside of the text widget, every thing after the
 * limit will be deleted.
 */
void
ewl_text_length_maximum_set(Ewl_Text *t, unsigned int char_num)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (t->length.max_chars == char_num)
                DRETURN(DLEVEL_STABLE);

        t->length.max_chars = char_num;
        if ((char_num > 0) && (char_num < t->length.max_chars))
        {
                unsigned int tmp_pos;

                tmp_pos = ewl_text_cursor_position_get(t);
                ewl_text_cursor_position_set(t, char_num);
                ewl_text_text_delete(t, ewl_text_length_get(t) - char_num);
                ewl_text_cursor_position_set(t, MIN(char_num, tmp_pos));
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the maximum number of characters
 * @return Returns the maximum length of characters, if the number is 
 *         unlimited it returns 0
 * @brief Retrieve if maximum number of characters
 */
unsigned int
ewl_text_length_maximum_get(Ewl_Text *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        DRETURN_INT(t->length.max_chars, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the minimum size string
 * @param string: The string to set define the minimum size
 * @return Returns no value
 * @brief Set a minimum size based on a given string
 *
 * The text widget will set its minimum size to be wide enough to show the
 * given string. It will use the default context, for font and size.
 */
void
ewl_text_minimum_size_string_set(Ewl_Text *t, const char *string)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        IF_RELEASE(t->min.size_string);

        if (!string)
                DRETURN(DLEVEL_STABLE);

        t->min.size_string = ecore_string_instance(string);
        if (REALIZED(t))
                ewl_text_minmax_size_update(t);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the minimum size string from
 * @return Returns the string which defines the minimum size
 * @brief Retrieve the minimum size string
 */
const char *
ewl_text_minimum_size_string_get(Ewl_Text *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, NULL);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, NULL);

        DRETURN_PTR(t->min.size_string, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the maximum size string
 * @param string: The string to set define the maximum size
 * @return Returns no value
 * @brief Set a maximum size based on a given string
 *
 * The text widget will set its maximum size to be not wider then to show the
 * given string. It will use the default context, for font and size.
 */
void
ewl_text_maximum_size_string_set(Ewl_Text *t, const char *string)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        IF_RELEASE(t->max.size_string);

        if (!string)
                DRETURN(DLEVEL_STABLE);

        t->max.size_string = ecore_string_instance(string);
        if (REALIZED(t))
                ewl_text_minmax_size_update(t);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the maximum size string from
 * @return Returns the string which defines the maximum size
 * @brief Retrieve the maximum size string
 */
const char *
ewl_text_maximum_size_string_get(Ewl_Text *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, NULL);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, NULL);

        DRETURN_PTR(t->max.size_string, DLEVEL_STABLE);
}

/**
 * @internal
 */
static void
ewl_text_minmax_size_update(Ewl_Text *t)
{
        Ewl_Text_Context *tx;
        Ewl_Embed *emb;
        Evas *evas;
        Evas_Imaging_Font *fn;
        int width;
        char font[EWL_TEXT_FONT_LENGHT_MAX];
        const char *source;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);
        
        emb = ewl_embed_widget_find(EWL_WIDGET(t));
        if (!emb)
        {
                DWARNING("Did not find a parent embed widget");
                DRETURN(DLEVEL_STABLE);
        }

        evas = emb->canvas;
        if (!evas)
        {
                DWARNING("Did not find a canvas");
                DRETURN(DLEVEL_STABLE);
        }

        tx = ewl_text_context_default_create(t);
        if (tx->font_source)
        {
                source = tx->font_source;
                ecore_strlcpy(font, tx->font, sizeof(font));
        }
        else
        {
                source = ewl_theme_path_get();
                snprintf(font, sizeof(font), "fonts/%s", tx->font);
        }

        fn = evas_imaging_font_load(source, font, tx->size);
        if (t->min.size_string)
        {
                evas_imaging_font_string_size_query(fn, t->min.size_string, 
                                                                &width, NULL);
                ewl_object_minimum_w_set(EWL_OBJECT(t), width);
        }
        if (t->max.size_string)
        {
                evas_imaging_font_string_size_query(fn, t->max.size_string, 
                                                                &width, NULL);
                ewl_object_maximum_w_set(EWL_OBJECT(t), width);
        }
        evas_imaging_font_free(fn);
        ewl_text_context_release(tx);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the layout offsets from
 * @param x: A pointer to an integer to store the x offset of the text display
 * @param y: A pointer to an integer to store the y offset of the text display
 * @return Returns no value.
 * @brief Retrieve the current layout offsets of the text
 */
void
ewl_text_offsets_get(Ewl_Text *t, int *x, int *y)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (x) *x = t->offset.x;
        if (y) *y = t->offset.y;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the layout offsets
 * @param x: The x amount to offset of the text display
 * @param y: The y amount to offset of the text display
 * @return Returns no value.
 * @brief Set the current layout offsets of the text
 */
void
ewl_text_offsets_set(Ewl_Text *t, int x, int y)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        t->offset.x = x;
        t->offset.y = y;

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the geometry from
 * @param char_idx: The character index to get the geometry for
 * @param x: Where to put the x value
 * @param y: Where to put the y value
 * @param w: Where to put the w value
 * @param h: Where to put the h value
 * @brief Map the given character index into a position in the text widget
 */
void
ewl_text_index_geometry_map(Ewl_Text *t, unsigned int char_idx,
                                                int *x, int *y,
                                                int *w, int *h)
{
        Evas_Coord tx = 0, ty = 0, tw = 0, th = 0;
        Evas_Textblock_Cursor *cursor;
        int shifting = 0;
        unsigned int byte_idx = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        /* can't do this if we don't have an evas object */
        if ((!REALIZED(t)) || (!t->textblock) || (!t->text))
        {
                if (x) *x = 0;
                if (y) *y = 0;
                if (w) *w = 1;
                if (h) *h = ewl_theme_data_int_get(EWL_WIDGET(t), "font_size");

                DRETURN(DLEVEL_STABLE);
        }

        /* force a display of the text */
        if (t->dirty) ewl_text_display(t);

        if (char_idx >= t->length.chars)
        {
                char_idx --;
                shifting = 1;
        }

        byte_idx = ewl_text_char_to_drawn_byte(t, char_idx);

        cursor = ewl_text_textblock_cursor_position(t, byte_idx);
        evas_textblock_cursor_char_geometry_get(cursor, &tx, &ty, &tw, &th);
        evas_textblock_cursor_free(cursor);

        if (x) *x = (int)(tx + CURRENT_X(t));
        if (y) *y = (int)(ty + CURRENT_Y(t));
        if (w) *w = (int)tw;
        if (h) *h = (int)th;

        /* if we didn't count the last item, move us over to the other side
         * of it */
        if (x && shifting) *x += tw;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to mapp the coords into
 * @param x: The x coord to map
 * @param y: The y coord to map
 * @return Returns the character index of the given coordinates
 * @brief Map the given coordinate into an index into the text widget
 */
unsigned int
ewl_text_coord_index_map(Ewl_Text *t, int x, int y)
{
        Evas_Textblock_Cursor *cursor;
        unsigned int byte_idx = 0, char_idx = 0, ctmp = 0;
        Evas_Coord tx, ty, cx = 0, cy, cw, ch;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        if ((!REALIZED(t)) || (!t->textblock) || (!t->text))
                DRETURN_INT(0, DLEVEL_STABLE);

        /* force a display of the text */
        if (t->dirty) ewl_text_display(t);

        tx = (Evas_Coord)(x - CURRENT_X(t));
        ty = (Evas_Coord)(y - CURRENT_Y(t));

        cursor = evas_object_textblock_cursor_new(t->textblock);

        /* see if we have the mouse over a char */
        if (!evas_textblock_cursor_char_coord_set(cursor, tx, ty))
        {
                int line;

                /* if not, see if the mouse is by a line */
                line = evas_textblock_cursor_line_coord_set(cursor, ty);
                if (line >= 0)
                {
                        /* if so, get the line geometry and determine start
                         * or end of line */
                        evas_textblock_cursor_line_geometry_get(cursor,
                                                        &cx, &cy, &cw, &ch);
                        if (x < (cx + (cw / 2)))
                                evas_textblock_cursor_line_first(cursor);
                        else
                        {
                                const char *txt;
                                evas_textblock_cursor_line_last(cursor);

                                /* we want to be past the last char so we
                                 * need to increment this by 1 to begin */
                                txt = evas_textblock_cursor_node_format_get(cursor);

                                /* Increment if we're on the last line */
                                if (!txt || (strcmp(txt, "\n")))
                                        char_idx++;
                        }
                }
                else
                {
                        evas_textblock_cursor_line_set(cursor, 0);
                        evas_textblock_cursor_line_first(cursor);
                }
        }
        else
        {
                evas_textblock_cursor_char_geometry_get(cursor,
                                                &cx, &cy, &cw, &ch);
                if (tx > (cx + ((cw + 1) >> 1)))
                         char_idx++;
        }

        byte_idx = ewl_text_textblock_cursor_to_index(cursor);
        ctmp = ewl_text_drawn_byte_to_char(t, byte_idx);
        evas_textblock_cursor_free(cursor);

        char_idx += ctmp;

        DRETURN_INT(char_idx, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text from
 * @return Returns the text in the widget @a t or NULL if no text is set
 * @brief Retrieve the text from the text widget
 */
char *
ewl_text_text_get(Ewl_Text *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, NULL);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, NULL);

        DRETURN_PTR(((t->text) ? strdup(t->text) : NULL), DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to clear
 * @return Returns no value
 * @brief Clear the text widget
 */
void
ewl_text_clear(Ewl_Text *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (t->length.chars > 0)
        {
                ewl_text_cursor_position_set(t, 0);
                ewl_text_text_delete(t, t->length.chars);
        }
        t->dirty = TRUE;

        if (t->formatting.tx)
                ewl_text_context_release(t->formatting.tx);

        t->formatting.tx = ewl_text_context_default_create(t);
        ewl_text_fmt_clear(t->formatting.nodes);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text into
 * @param text: The text to set into the widget
 * @return Returns no value
 * @brief Set the text in the text widget
 */
void
ewl_text_text_set(Ewl_Text *t, const char *text)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        ewl_text_clear(t);
        ewl_text_text_append(t, text);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text into
 * @param text: The text to set into the widget
 * @return Returns no value
 * @brief Prepend the given text into the text widget
 */
void
ewl_text_text_prepend(Ewl_Text *t, const char *text)
{
        unsigned int char_len = 0, byte_len = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);

        /* don't do anything if there is no text */
        if ((!text) || (!text[0]))
                DRETURN(DLEVEL_STABLE);

        /* don't insert text if we already reached the maximum */
        if (t->length.max_chars && t->length.chars >= t->length.max_chars)
                DRETURN(DLEVEL_STABLE);

        ewl_text_text_insert_private(t, text, 0, &char_len, &byte_len);
        ewl_text_fmt_node_prepend(t->formatting.nodes,
                                        t->formatting.tx,
                                        char_len, byte_len);

        if (t->formatting.tx)
        {
                /* we release this here as the cursor_position_set may not
                 * actually remove it if the cursor dosen't move */
                ewl_text_context_release(t->formatting.tx);
                t->formatting.tx = NULL;
        }

        ewl_text_cursor_position_set(t, char_len);
        t->dirty = TRUE;

        if (text) ewl_text_triggers_shift(t, 0, char_len, FALSE);
        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text into
 * @param text: The text to set into the widget
 * @return Returns no value
 * @brief Append the text into the text widget
 */
void
ewl_text_text_append(Ewl_Text *t, const char *text)
{
        unsigned int char_len = 0, byte_len = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        /* don't do anything if there is no text */
        if ((!text) || (!text[0]))
                DRETURN(DLEVEL_STABLE);

        /* don't insert text if we already reached the maximum */
        if (t->length.max_chars && t->length.chars >= t->length.max_chars)
                DRETURN(DLEVEL_STABLE);

        ewl_text_text_insert_private(t, text, t->length.chars, &char_len, &byte_len);
        ewl_text_fmt_node_append(t->formatting.nodes,
                                        t->formatting.tx,
                                        char_len, byte_len);

        if (t->formatting.tx)
        {
                /* we free this here as the cursor_position_set may not
                 * actually remove it if the cursor dosen't move */
                ewl_text_context_release(t->formatting.tx);
                t->formatting.tx = NULL;
        }

        ewl_text_cursor_position_set(t, t->length.chars);
        t->dirty = TRUE;
        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text into
 * @param text: The text to set into the widget
 * @param char_idx: The index to insert the text at
 * @return Returns no value
 * @brief Insert the given text into the text widget
 */
void
ewl_text_text_insert(Ewl_Text *t, const char *text, unsigned int char_idx)
{
        Ewl_Text_Context *tx;
        unsigned int char_len = 0, byte_len = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        /* don't do anything if there is no text */
        if ((!text) || (!text[0]))
                DRETURN(DLEVEL_STABLE);

        /* don't insert text if we already reached the maximum */
        if (t->length.max_chars && t->length.chars >= t->length.max_chars)
                DRETURN(DLEVEL_STABLE);

        /* Limit the index to be within safe boundaries */
        if (char_idx > t->length.chars + 1)
                char_idx = t->length.chars + 1;

        /* make sure we set the position _before_ inserting the text else
         * it'll fuck up the cursor_position_set call when inserting into
         * and empty node list */
        tx = t->formatting.tx;
        t->formatting.tx = NULL;
        ewl_text_cursor_position_set(t, char_idx);

        ewl_text_text_insert_private(t, text, char_idx, &char_len, &byte_len);
        ewl_text_fmt_node_insert(t->formatting.nodes, char_idx, tx,
                                                char_len, byte_len);

        if (tx) ewl_text_context_release(tx);
        ewl_text_cursor_position_set(t, char_idx + char_len);
        t->dirty = TRUE;

        if (text) ewl_text_triggers_shift(t, char_idx, char_len, FALSE);
        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_text_insert_private(Ewl_Text *t, const char *txt,
                         unsigned int char_idx, unsigned int *char_len,
                        unsigned int *byte_len)
{
        unsigned int new_byte_len, clen = 0, blen = 0, bidx = 0;
        unsigned int max_chars;
        char *tmp, *ptr;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        /* nothign to do if no text */
        if (!txt) DRETURN(DLEVEL_STABLE);

        /* count the number of chars in the text */
        tmp = (char *)txt;
        max_chars = (t->length.max_chars) ? t->length.max_chars : UINT_MAX;
        while ((*tmp) && ((clen + t->length.chars) < max_chars))
        {
                if (ewl_text_char_utf8_is(tmp))
                        tmp = ewl_text_text_next_char(tmp, NULL);
                else
                        tmp++;

                clen++;
        }
        blen = tmp - txt;

        new_byte_len = t->length.bytes + blen;
        if ((new_byte_len + 1) >= t->total_size)
        {
                int extend;

                /*
                 * Determine the size in blocks of EWL_TEXT_EXTEND_VAL
                 */
                extend = ((new_byte_len + 1) / EWL_TEXT_EXTEND_VAL);
                extend = (extend + 1) * EWL_TEXT_EXTEND_VAL;

                REALLOC(t->text, char, extend);
                t->total_size = extend;
        }

        ewl_text_fmt_char_to_byte(t->formatting.nodes, char_idx,
                                                0, &bidx, NULL);

        if (char_idx < t->length.chars)
                memmove(t->text + bidx + blen, t->text + bidx,
                                        t->length.bytes - bidx);

        /* copy the text over, replace invalid UTF-8 chars */
        tmp = (char *)txt;
        ptr = t->text + bidx;
        while (*tmp && (tmp - txt) < (int)blen)
        {
                if (ewl_text_char_utf8_is(tmp))
                {
                        char *s;

                        s = tmp;
                        tmp = ewl_text_text_next_char(tmp, NULL);
                        for ( ; s != tmp; s++, ptr++)
                                *ptr = *s;
                }
                else
                {
                        *ptr = '?';
                        tmp++;
                }
        }

        /* update the text information */
        t->length.chars += clen;
        t->length.bytes += blen;
        t->text[t->length.bytes] = '\0';

        if (char_len) *char_len = clen;
        if (byte_len) *byte_len = blen;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to delete the text from
 * @param char_len: The length of text to delete
 * @return Returns no value
 * @brief This will delete the specified length of text from the current cursor
 * position
 */
void
ewl_text_text_delete(Ewl_Text *t, unsigned int char_len)
{
        unsigned int byte_idx = 0, byte_len = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if ((!t->text) || (char_len == 0) ||
                        (t->cursor_position >= t->length.chars))
                DRETURN(DLEVEL_STABLE);

        /* don't try to delete more then we have after the current cursor
         * position */
        if ((t->length.chars - t->cursor_position) < char_len)
                char_len = t->length.chars - t->cursor_position;

        ewl_text_fmt_char_to_byte(t->formatting.nodes,
                                t->cursor_position, char_len,
                                &byte_idx, &byte_len);

        t->length.chars -= char_len;
        if (t->length.chars > 0)
        {
                t->length.bytes -= byte_len;
                memmove(t->text + byte_idx,
                                t->text + byte_idx + byte_len,
                                t->length.bytes - byte_idx);

                t->text[t->length.bytes] = '\0';

                ewl_text_triggers_shift(t, t->cursor_position, char_len, TRUE);
        }
        else
        {
                IF_FREE(t->text);
                t->length.bytes = 0;
                t->length.chars = 0;
                t->total_size = 0;
                t->cursor_position = 0;
                ewl_text_triggers_remove(t);

                /* cleanup the selection */
                if (t->selection)
                        ewl_widget_destroy(EWL_WIDGET(t->selection));

                t->selection = NULL;
        }

        ewl_text_fmt_node_delete(t->formatting.nodes,
                                t->cursor_position, char_len);
        t->dirty = TRUE;

        if (ewl_text_fmt_node_count_get(t->formatting.nodes) == 0)
                t->formatting.tx = ewl_text_context_default_create(t);

        if (t->cursor_position > t->length.chars)
                ewl_text_cursor_position_set(t, t->length.chars);

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: the text to set the obscuring character
 * @param o: the character to obscure the password characters. This need to be
 *           a string, because UTF-8 characters can be longer then one byte.
 *           If it is not a valid UTF-8 character it will fallback to an
 *           asterix (*). NULL will turn the option off and let the text widget
 *           return to the normal text view.
 * @return Returns no value.
 * @brief Sets the character used to obscure the text for a password. Every
 * character will be replaced by this character on the output.
 */
void
ewl_text_obscure_set(Ewl_Text *t, const char *o)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        /* free the old character */
        IF_FREE(t->obscure);

        if (!o)
        {
                t->obscure = NULL;
        }
        else if (ewl_text_char_utf8_is(o))
        {
                size_t len;

                len = EWL_TEXT_CHAR_BYTE_LEN(o);
                t->obscure = NEW(char, len + 1);
                memcpy(t->obscure, o, len);
                t->obscure[len] = 0;
        }
        else
                t->obscure = strdup("*");

        t->dirty = TRUE;
        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: the text to retrieve the obscuring character
 * @return Returns the character value of the obscuring character. This is
 *         a null-terminated string containing an UTF-8 character or NULL
 *         if the text isn't set to obscure the text.
 * @brief Retrieves the character used to obscure the text for a password.
 */
const char *
ewl_text_obscure_get(Ewl_Text *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        DRETURN_PTR(t->obscure, DLEVEL_STABLE);
}

/**
 * @param t: The text to set the selectable value of
 * @param selectable: The selectable value to set
 * @return Returns no value
 * @brief Set if the text is selectable
 */
void
ewl_text_selectable_set(Ewl_Text *t, unsigned int selectable)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (t->selectable == !!selectable)
                DRETURN(DLEVEL_STABLE);

        t->selectable = !!selectable;

        if (t->selectable)
        {
                ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_MOUSE_DOWN,
                                                ewl_text_cb_mouse_down, NULL);
                ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_MOUSE_UP,
                                                ewl_text_cb_mouse_up, NULL);
                ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_KEY_DOWN,
                                                ewl_text_cb_key_down, NULL);
                ewl_callback_append(EWL_WIDGET(t), EWL_CALLBACK_SELECTION_CLEAR,
                                                ewl_text_cb_selection_clear, 
                                                NULL);
        }
        else
        {
                ewl_callback_del(EWL_WIDGET(t), EWL_CALLBACK_MOUSE_DOWN,
                                                ewl_text_cb_mouse_down);
                ewl_callback_del(EWL_WIDGET(t), EWL_CALLBACK_MOUSE_UP,
                                                ewl_text_cb_mouse_up);
                ewl_callback_del(EWL_WIDGET(t), EWL_CALLBACK_KEY_DOWN,
                                                ewl_text_cb_key_down);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The text to get the selectable value from
 * @return Returns the selectable value of the widget
 * @brief Get the selectable state of the text
 */
unsigned int
ewl_text_selectable_get(Ewl_Text *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        DRETURN_INT(t->selectable, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text widget to get the selection text from
 * @return Returns the selection text or NULL if none set
 * @brief Gets the current text of the selection
 */
char *
ewl_text_selection_text_get(Ewl_Text *t)
{
        char *ret = NULL;
        Ewl_Text_Trigger *sel;
        unsigned int byte_pos = 0;
        unsigned int byte_len = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, NULL);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, NULL);

        sel = EWL_TEXT_TRIGGER(t->selection);
        if ((!sel) || (sel->char_len == 0))
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        ewl_text_fmt_char_to_byte(t->formatting.nodes,
                                 sel->char_pos,
                                 sel->char_len,
                                &byte_pos, &byte_len);

        ret = malloc(sizeof(char) * (byte_len + 1));
        if (!ret) DRETURN_PTR(NULL, DLEVEL_STABLE);

        memcpy(ret, t->text + byte_pos, byte_len);
        ret[byte_len] = '\0';

        DRETURN_PTR(ret, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the selection from
 * @return Returns the selection object of this text or NULL if no current
 * selection
 * @brief Get the current text selection
 */
Ewl_Widget *
ewl_text_selection_get(Ewl_Text *t)
{
        Ewl_Text_Trigger *sel;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, NULL);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, NULL);

        sel = EWL_TEXT_TRIGGER(t->selection);
        if (sel && ewl_text_trigger_length_get(sel) > 0)
                DRETURN_PTR(EWL_WIDGET(sel), DLEVEL_STABLE);

        DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @param t: The text to check if there is a selection
 * @return Returns TRUE if there is selected text, FALSE otherwise
 * @brief Check if anything is selected in the text widget
 */
unsigned int
ewl_text_has_selection(Ewl_Text *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, FALSE);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, FALSE);

        if (ewl_text_selection_get(t))
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/**
 * @param t: The text to do the selection on
 * @param char_idx: The start position of the selection
 * @param char_len: The length of the selection
 * @return Returns no value
 * @brief Select the text
 */
void
ewl_text_select(Ewl_Text *t, unsigned int char_idx, unsigned int char_len)
{
        Ewl_Text_Trigger *s;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (t->length.chars == 0)
                DRETURN(DLEVEL_STABLE);

        if (char_idx > t->length.chars)
                char_idx = t->length.chars;

        if (char_idx + char_len > t->length.chars)
                char_len = t->length.chars - char_idx;

        /* if we haven't already have an selection create one */
        if (!t->selection)
                t->selection = ewl_text_selection_new(t);

        s = EWL_TEXT_TRIGGER(t->selection);
        ewl_text_trigger_start_pos_set(s, char_idx);
        ewl_text_trigger_length_set(s, char_len);

        ewl_text_trigger_position(t, s);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The text to do the selection on
 * @return Returns no value
 * @brief Select the whole text
 */
void
ewl_text_all_select(Ewl_Text *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        ewl_text_select(t, 0, t->length.chars);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text widget to set the position into
 * @param char_pos: The position to set
 * @return Returns no value.
 * @brief Set the cursor position in the text widget
 */
void
ewl_text_cursor_position_set(Ewl_Text *t, unsigned int char_pos)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        /* make sure we aren't more then the next char past the
         * end of the text */
        if (char_pos > t->length.chars) char_pos = t->length.chars;

        /* it's the same position, do nothing */
        if (char_pos == t->cursor_position)
                DRETURN(DLEVEL_STABLE);

        /* clean the current context if it exists */
        if (t->formatting.tx)
        {
                ewl_text_context_release(t->formatting.tx);
                t->formatting.tx = NULL;
        }
        t->cursor_position = char_pos;

        ewl_text_fmt_goto(t->formatting.nodes, char_pos);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the cursor position from
 * @return Returns the current cursor position in the widget
 * @brief Retrieve the cursor position from the text widget
 */
unsigned int
ewl_text_cursor_position_get(Ewl_Text *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        DRETURN_INT(t->cursor_position, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the cursor position one line up from
 * @return Returns the cursor position if we moved up one line
 * @brief Get the index if we were to move the cursor up one line
 */
unsigned int
ewl_text_cursor_position_line_up_get(Ewl_Text *t)
{
        Evas_Textblock_Cursor *cursor;
        unsigned int cur_char_idx = 0, byte_idx = 0;
        Evas_Coord cx, cw;
        Evas_Coord lx, ly, lw, lh;
        int line;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        cur_char_idx = ewl_text_cursor_position_get(t);
        byte_idx = ewl_text_char_to_drawn_byte(t, cur_char_idx);

        cursor = ewl_text_textblock_cursor_position(t, byte_idx);
        line = evas_textblock_cursor_char_geometry_get(cursor, &cx, NULL,
                                                                &cw, NULL);
        line --;

        if (evas_object_textblock_line_number_geometry_get(t->textblock,
                                                line, &lx, &ly, &lw, &lh))
        {
                if (!evas_textblock_cursor_char_coord_set(cursor,
                                                        cx + (cw / 2), ly))
                {
                        if (evas_textblock_cursor_line_set(cursor, line))
                        {
                                if ((cx + (cw / 2)) >= (lx + lw))
                                        evas_textblock_cursor_line_last(cursor);
                                else
                                        evas_textblock_cursor_line_first(cursor);
                        }
                }

        }

        byte_idx = ewl_text_textblock_cursor_to_index(cursor);
        cur_char_idx = ewl_text_drawn_byte_to_char(t, byte_idx);

        DRETURN_INT(cur_char_idx, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the cursor position one line down from
 * @return Returns the cursor position if we moved down one line
 * @brief Get the index if we were to move the cursor down one line
 */
unsigned int
ewl_text_cursor_position_line_down_get(Ewl_Text *t)
{
        Evas_Textblock_Cursor *cursor;
        unsigned int cur_char_idx = 0, byte_idx = 0;
        Evas_Coord cx, cw;
        Evas_Coord lx, ly, lw, lh;
        int line;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        cur_char_idx = ewl_text_cursor_position_get(t);
        ewl_text_fmt_char_to_byte(t->formatting.nodes, cur_char_idx,
                                                0, &byte_idx, NULL);

        cursor = ewl_text_textblock_cursor_position(t, byte_idx);
        line = evas_textblock_cursor_char_geometry_get(cursor, &cx, NULL,
                                                                &cw, NULL);
        line ++;

        if (evas_object_textblock_line_number_geometry_get(t->textblock,
                                                line, &lx, &ly, &lw, &lh))
        {
                if (!evas_textblock_cursor_char_coord_set(cursor,
                                                        cx + (cw / 2), ly))
                {
                        if (evas_textblock_cursor_line_set(cursor, line))
                        {
                                if ((cx + (cw / 2)) >= (lx + lw))
                                        evas_textblock_cursor_line_last(cursor);
                                else
                                        evas_textblock_cursor_line_first(cursor);
                        }
                }

        }

        byte_idx = ewl_text_textblock_cursor_to_index(cursor);
        cur_char_idx = 0;
        ewl_text_fmt_byte_to_char(t->formatting.nodes, byte_idx,
                                        0, &cur_char_idx, NULL);

        DRETURN_INT(cur_char_idx, DLEVEL_STABLE);
}


static void
ewl_text_cursor_back(Evas_Textblock_Cursor *c)
{
        if(!evas_textblock_cursor_char_prev(c))
        {
                if(evas_textblock_cursor_node_prev(c))
                {
                        while(evas_textblock_cursor_node_format_get(c))
                        {
                                if(evas_textblock_cursor_node_format_is_visible_get(c))
                                        break;
                                if(!evas_textblock_cursor_node_prev(c))
                                        break;
                        }
                }
        }
}

static void
ewl_text_cursor_next(Evas_Textblock_Cursor *c)
{
        if (!evas_textblock_cursor_char_next(c))
        {
                if (evas_textblock_cursor_node_next(c))
                {
                        while (evas_textblock_cursor_node_format_get(c))
                        {
                                if (evas_textblock_cursor_node_format_is_visible_get(c))
                                        break;
                                if (!evas_textblock_cursor_node_next(c))
                                break;
                        }
                }
        }
        else
        {
                int len, pos;

                len = evas_textblock_cursor_node_text_length_get(c);
                pos = evas_textblock_cursor_pos_get(c);
                if (pos == len)
                        evas_textblock_cursor_node_next(c);
        }
}

/**
 * @param ch: character to test
 * @return Returns 1 if the @ch is a word break character
 * @brief Test if a character is a word break character
 */
static int
ewl_text_word_break(char ch)
{
        return ch == ' ' || ch == '\t'
               || ch == '(' || ch == ')'
               || ch == '[' || ch == ']'
               || ch == ',' || ch == '.'
               || ch == ':' || ch == ';'
               || ch == '\n'
              ;
}


unsigned int
ewl_text_cursor_position_line_start_get(Ewl_Text *t)
{
        Evas_Textblock_Cursor *cursor;
        unsigned int char_idx, byte_idx;
        char const *format;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        char_idx = ewl_text_cursor_position_get(t);
        byte_idx = ewl_text_char_to_drawn_byte(t, char_idx);

        cursor = ewl_text_textblock_cursor_position(t, byte_idx);

        format = evas_textblock_cursor_node_format_get(cursor);
        if (format)
        {
                if (!strcmp(format, "-"))
                        ewl_text_cursor_back(cursor);
        }

        evas_textblock_cursor_line_first(cursor);

        byte_idx = ewl_text_textblock_cursor_to_index(cursor);
        char_idx = ewl_text_drawn_byte_to_char(t, byte_idx);

        evas_textblock_cursor_free(cursor);

        DRETURN_INT(char_idx, DLEVEL_STABLE);
}

unsigned int
ewl_text_cursor_position_line_end_get(Ewl_Text *t)
{
        Evas_Textblock_Cursor *cursor;
        unsigned int char_idx, byte_idx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        char_idx = ewl_text_cursor_position_get(t);
        byte_idx = ewl_text_char_to_drawn_byte(t, char_idx);

        cursor = ewl_text_textblock_cursor_position(t, byte_idx);

        evas_textblock_cursor_line_last(cursor);
        if (!evas_textblock_cursor_node_format_get(cursor))
            ewl_text_cursor_next(cursor);

        byte_idx = ewl_text_textblock_cursor_to_index(cursor);
        char_idx = ewl_text_drawn_byte_to_char(t, byte_idx);

        evas_textblock_cursor_free(cursor);

        DRETURN_INT(char_idx, DLEVEL_STABLE);
}


/**
 * @param t: The Ewl_Text to get the cursor position of the beginning of previous word
 * @return Returns the cursor position if we moved to the beginning of the previous word
 * @brief Get the index if we were to move the cursor to the beginning of the previous word
 */
unsigned int
ewl_text_cursor_position_word_previous_get(Ewl_Text *t)
{
        const char   *ptr;
        unsigned int char_idx, byte_idx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, t->cursor_position);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, t->cursor_position);

        char_idx = ewl_text_cursor_position_get(t);
        ewl_text_fmt_char_to_byte(t->formatting.nodes,
                                  char_idx, 0, &byte_idx, NULL);
        ptr = t->text + byte_idx - 1;

        while (ptr >= t->text && ewl_text_word_break(*ptr))
                --ptr;
        while (ptr >= t->text && !ewl_text_word_break(*ptr))
                --ptr;

        byte_idx = ptr - t->text + 1;
        ewl_text_fmt_byte_to_char(t->formatting.nodes, byte_idx, 0, &char_idx, NULL);

        DRETURN_INT(char_idx, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the cursor position of the beginning of the next word
 * @return Returns the cursor position if we moved to the beginning of the next word
 * @brief Get the index if we were to move the cursor to the beginning of the next word
 */
unsigned int
ewl_text_cursor_position_word_next_get(Ewl_Text *t)
{
        const char *ptr;
        unsigned int char_idx, byte_idx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        char_idx = ewl_text_cursor_position_get(t);
        ewl_text_fmt_char_to_byte(t->formatting.nodes,
                                  char_idx, 0, &byte_idx, NULL);

        ptr = t->text + byte_idx;

        while (*ptr && !ewl_text_word_break(*ptr))
                ++ptr;
        while (*ptr && ewl_text_word_break(*ptr))
                ++ptr;

        byte_idx = ptr - t->text;
        ewl_text_fmt_byte_to_char(t->formatting.nodes, byte_idx, 0, &char_idx, NULL);

        DRETURN_INT(char_idx, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Widget to set the font into
 * @param font: The font to set
 * @return Returns no value
 * @brief This will set the current font to be used when we insert more text
 */
void
ewl_text_font_set(Ewl_Text *t, const char *font)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        ewl_text_font_source_set(t, NULL, font);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the font too
 * @param font: The font to set
 * @param char_len: The distance to set the font over
 * @return Returns no value
 * @brief This will apply the specfied @a font from the current cursor position to
 * the length specified
 */
void
ewl_text_font_apply(Ewl_Text *t, const char *font, unsigned int char_len)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        ewl_text_font_source_apply(t, NULL, font, char_len);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the font from
 * @param char_idx: The index to get the font at
 * @return Returns no value
 * @brief This will retrive the font used at the specified index in the text
 */
char *
ewl_text_font_get(Ewl_Text *t, unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;
        char *font = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, NULL);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, NULL);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (fmt && fmt->tx)
        {
                if (fmt->tx->font)
                        font = strdup(fmt->tx->font);
        }
        else
        {
                Ewl_Text_Context *tx;

                tx = ewl_text_context_default_create(t);
                if (tx->font) font = strdup(tx->font);
                ewl_text_context_release(tx);
        }

        DRETURN_PTR(font, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Widget to set the font into
 * @param source: The font source to set
 * @param font: The font to set
 * @return Returns no value
 * @brief This will set the current font to be used when we insert more text
 */
void
ewl_text_font_source_set(Ewl_Text *t, const char *source, const char *font)
{
        Ewl_Text_Context *change;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        change = ewl_text_context_new();
        if (source) change->font_source = ecore_string_instance(source);

        /* null font will go back to the theme default */
        if (!font) font = ewl_theme_data_str_get(EWL_WIDGET(t), "font");

        /* Duplicate a local copy of the font */
        if (font) change->font = ecore_string_instance(font);

        ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_FONT, change);
        ewl_text_context_release(change);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the font too
 * @param source: The font souce
 * @param font: The font to set
 * @param char_len: The distance to set the font over
 * @return Returns no value
 * @brief This will apply the specfied @a font from the current cursor position to
 * the length specified
 */
void
ewl_text_font_source_apply(Ewl_Text *t, const char *source, const char *font,
                                                        unsigned int char_len)
{
        Ewl_Text_Context *tx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        /* if length is 0 we have nothing to do */
        if (char_len == 0)
                DRETURN(DLEVEL_STABLE);

        tx = ewl_text_context_new();

        if (source) tx->font_source = ecore_string_instance(source);

        /* null font will go back to the theme default */
        if (!font) font = ewl_theme_data_str_get(EWL_WIDGET(t), "font");

        /* Duplicate a local copy of the font */
        if (font) tx->font = ecore_string_instance(font);

        ewl_text_fmt_apply(t->formatting.nodes, EWL_TEXT_CONTEXT_MASK_FONT, tx,
                                        t->cursor_position, char_len);
        ewl_text_context_release(tx);
        t->dirty = TRUE;

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the font from
 * @param char_idx: The index to get the font at
 * @return Returns no value
 * @brief This will retrive the font source used at the specified index in the text
 */
char *
ewl_text_font_source_get(Ewl_Text *t, unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;
        char *source = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, NULL);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, NULL);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (fmt && fmt->tx)
        {
                if (fmt->tx->font_source)
                        source = strdup(fmt->tx->font_source);
        }
        else
        {
                Ewl_Text_Context *tx;

                tx = ewl_text_context_default_create(t);
                if (tx->font_source) source = strdup(tx->font_source);
                ewl_text_context_release(tx);
        }

        DRETURN_PTR(source, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the size too
 * @param size: The size to set the font too
 * @return Returns no value
 * @brief Set the font size to use when inserting new text
 */
void
ewl_text_font_size_set(Ewl_Text *t, unsigned int size)
{
        Ewl_Text_Context *change;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        change = ewl_text_context_new();
        change->size = size;

        ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_SIZE, change);
        ewl_text_context_release(change);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the size into
 * @param size: The size to set
 * @param char_len: Length of block to get the new size
 * @return Returns no value
 * @brief This will apply the font size to the text from the current cursor
 * position for the given length
 */
void
ewl_text_font_size_apply(Ewl_Text *t, unsigned int size, unsigned int char_len)
{
        Ewl_Text_Context *tx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (char_len == 0)
                DRETURN(DLEVEL_STABLE);

        tx = ewl_text_context_new();
        tx->size = size;
        ewl_text_fmt_apply(t->formatting.nodes, EWL_TEXT_CONTEXT_MASK_SIZE, tx,
                                                t->cursor_position, char_len);
        ewl_text_context_release(tx);
        t->dirty = TRUE;

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the size from
 * @param char_idx: The index you want to get the size for
 * @return Returns no value
 * @brief Retrieve the font size at the given index
 */
unsigned int
ewl_text_font_size_get(Ewl_Text *t, unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;
        int size = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (fmt && fmt->tx)
        {
                size = fmt->tx->size;
        }
        else
        {
                Ewl_Text_Context *tx;

                tx = ewl_text_context_default_create(t);
                size = tx->size;
                ewl_text_context_release(tx);
        }

        DRETURN_INT(size, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the colour on
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the text colour at the cursor
 */
void
ewl_text_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
                                unsigned int b, unsigned int a)
{
        Ewl_Text_Context *change;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        change = ewl_text_context_new();
        change->color.r = r;
        change->color.g = g;
        change->color.b = b;
        change->color.a = a;

        ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_COLOR, change);
        ewl_text_context_release(change);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the colour into
 * @param r: The red value to set
 * @param g: The green value to set
 * @param b: The blue value to set
 * @param a: The alpha value to set
 * @param char_len: The length of text to apply the colour over
 * @return Returns no value
 * @brief This will set the given colour from the current cursor position for the
 * specified length
 */
void
ewl_text_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
                                unsigned int b, unsigned int a,
                                unsigned int char_len)
{
        Ewl_Text_Context *tx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (char_len == 0)
                DRETURN(DLEVEL_STABLE);

        tx = ewl_text_context_new();
        tx->color.r = r;
        tx->color.g = g;
        tx->color.b = b;
        tx->color.a = a;

        ewl_text_fmt_apply(t->formatting.nodes, EWL_TEXT_CONTEXT_MASK_COLOR, tx,
                                        t->cursor_position, char_len);
        ewl_text_context_release(tx);
        t->dirty = TRUE;

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Retrives the text colour at the given index
 */
void
ewl_text_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
                                unsigned int *b, unsigned int *a,
                                unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (fmt && fmt->tx)
        {
                if (r) *r = fmt->tx->color.r;
                if (g) *g = fmt->tx->color.g;
                if (b) *b = fmt->tx->color.b;
                if (a) *a = fmt->tx->color.a;
        }
        else
        {
                Ewl_Text_Context *tx;

                tx = ewl_text_context_default_create(t);
                if (r) *r = tx->color.r;
                if (g) *g = tx->color.g;
                if (b) *b = tx->color.b;
                if (a) *a = tx->color.a;
                ewl_text_context_release(tx);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text ot set the alignment into
 * @param align: The alignment to set
 * @return Returns no value
 * @brief Set the current alignment value of the text
 */
void
ewl_text_align_set(Ewl_Text *t, unsigned int align)
{
        Ewl_Text_Context *change;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        change = ewl_text_context_new();
        change->align = align;

        ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_ALIGN, change);
        ewl_text_context_release(change);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to apply the alignment too
 * @param align: The alignment to apply
 * @param char_len: The length to apply the alignment for
 * @return Returns no value
 * @brief This will set the given alignment from the current cursor position for
 * the given length of text
 */
void
ewl_text_align_apply(Ewl_Text *t, unsigned int align, unsigned int char_len)
{
        Ewl_Text_Context *tx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (char_len == 0)
                DRETURN(DLEVEL_STABLE);

        tx = ewl_text_context_new();
        tx->align = align;
        ewl_text_fmt_apply(t->formatting.nodes, EWL_TEXT_CONTEXT_MASK_ALIGN, tx,
                                        t->cursor_position, char_len);
        ewl_text_context_release(tx);
        t->dirty = TRUE;

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the alignment from
 * @param char_idx: The index to get the alignment from
 * @return Returns the current text alignment value
 * @brief Retrieves the alignment value from the given index
 */
unsigned int
ewl_text_align_get(Ewl_Text *t, unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;
        int align = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (fmt && fmt->tx)
        {
                align = fmt->tx->align;
        }
        else
        {
                Ewl_Text_Context *tx;

                tx = ewl_text_context_default_create(t);
                align = tx->align;
                ewl_text_context_release(tx);
        }

        DRETURN_INT(align, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the style into
 * @param styles: The styles to set into the text
 * @return Returns no value
 * @brief Sets the given styles into the text at the cursor
 */
void
ewl_text_styles_set(Ewl_Text *t, unsigned int styles)
{
        Ewl_Text_Context *change;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        change = ewl_text_context_new();
        change->styles = styles;

        ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_STYLES, change);
        ewl_text_context_release(change);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to apply the style too
 * @param styles: The styles to set into the text
 * @param char_len: The length of text to apply the style too
 * @return Returns no value
 * @brief This will set the given style from the current cursor position for the
 * given length of text
 */
void
ewl_text_styles_apply(Ewl_Text *t, unsigned int styles, unsigned int char_len)
{
        Ewl_Text_Context *tx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (char_len == 0)
                DRETURN(DLEVEL_STABLE);

        tx = ewl_text_context_new();
        tx->styles = styles;
        ewl_text_fmt_apply(t->formatting.nodes, EWL_TEXT_CONTEXT_MASK_STYLES, tx,
                                        t->cursor_position, char_len);
        ewl_text_context_release(tx);
        t->dirty = TRUE;

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The text to add the style too
 * @param style: The style to add to the text
 * @param char_len: The lenght of text to add the style too
 * @return Returns no value
 * @brief This will add the given style to the text from the cursor up to length
 * characters
 */
void
ewl_text_style_add(Ewl_Text *t, Ewl_Text_Style style, unsigned int char_len)
{
        Ewl_Text_Fmt_Node *fmt;
        unsigned int styles;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        fmt = ewl_text_fmt_get(t->formatting.nodes, t->cursor_position);
        if (!fmt || !fmt->tx) DRETURN(DLEVEL_STABLE);

        styles = fmt->tx->styles;
        styles |= style;

        ewl_text_styles_apply(t, styles, char_len);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The text to delete the style from
 * @param style: The style to delete from the text
 * @param char_len: The lenght of text to delete the style from
 * @return Returns no value
 * @brief This will delete the given style from the text starting at the cursor up
 * to length characters
 */
void
ewl_text_style_del(Ewl_Text *t, Ewl_Text_Style style, unsigned int char_len)
{
        Ewl_Text_Fmt_Node *fmt;
        unsigned int styles;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        fmt = ewl_text_fmt_get(t->formatting.nodes, t->cursor_position);
        if (!fmt || !fmt->tx) DRETURN(DLEVEL_STABLE);

        styles = fmt->tx->styles;
        styles &= ~style;

        ewl_text_styles_apply(t, styles, char_len);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
 }

/**
 * @param t: The text to invert the style on
 * @param style: The style to invert in the text
 * @param char_len: The lenght of text to invert the style on
 * @return Returns no value
 * @brief This will invert the given style in the text starting at the cursor up
 * to length characters
 */
void
ewl_text_style_invert(Ewl_Text *t, Ewl_Text_Style style, unsigned int char_len)
{
        Ewl_Text_Fmt_Node *fmt;
        unsigned int styles;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        fmt = ewl_text_fmt_get(t->formatting.nodes, t->cursor_position);
        if (!fmt || !fmt->tx) DRETURN(DLEVEL_STABLE);

        styles = fmt->tx->styles;
        styles ^= style;

        ewl_text_styles_apply(t, styles, char_len);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The text to check for the style
 * @param style: The style to check for
 * @param char_idx: The index to check for the style
 * @return Returns no value
 * @brief Check if the given style is set at the given index in the text
 */
unsigned int
ewl_text_style_has(Ewl_Text *t, Ewl_Text_Style style, unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, FALSE);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, FALSE);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (!fmt || !fmt->tx)
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        DRETURN_INT((fmt->tx->styles & style), DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the style from
 * @param char_idx: The index to get the style from
 * @return Get the styles set at the given index in the text
 * @brief Retrives the styles in use at the given index
 */
unsigned int
ewl_text_styles_get(Ewl_Text *t, unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;
        int styles = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (fmt && fmt->tx)
        {
                styles = fmt->tx->styles;
        }
        else
        {
                Ewl_Text_Context *tx;

                tx = ewl_text_context_default_create(t);
                styles = tx->styles;
                ewl_text_context_release(tx);
        }

        DRETURN_INT(styles, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the wrap into
 * @param wrap: The wrap value to set
 * @return Returns no value
 * @brief Sets the wrap value of the text at the given index
 */
void
ewl_text_wrap_set(Ewl_Text *t, Ewl_Text_Wrap wrap)
{
        Ewl_Text_Context *change;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        change = ewl_text_context_new();
        change->wrap = wrap;

        if (wrap == EWL_TEXT_WRAP_NONE)
                ewl_object_fill_policy_set(EWL_OBJECT(t), EWL_FLAG_FILL_HFILL
                                                        | EWL_FLAG_FILL_VFILL);
        else
                ewl_object_fill_policy_set(EWL_OBJECT(t), EWL_FLAG_FILL_HSHRINK
                                                        | EWL_FLAG_FILL_HFILL
                                                        | EWL_FLAG_FILL_VFILL);

        ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_WRAP, change);
        ewl_text_context_release(change);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to apply the wrap value too
 * @param wrap: The wrap value to apply
 * @param char_len: The length of text to apply the wrap value over
 * @return Returns no value
 * @brief This will apply the given wrap value from the current cursor position for
 * the given length of text
 */
void
ewl_text_wrap_apply(Ewl_Text *t, Ewl_Text_Wrap wrap, unsigned int char_len)
{
        Ewl_Text_Context *tx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (char_len == 0)
                DRETURN(DLEVEL_STABLE);

        tx = ewl_text_context_new();
        tx->wrap = wrap;
        ewl_text_fmt_apply(t->formatting.nodes, EWL_TEXT_CONTEXT_MASK_WRAP, tx,
                                                t->cursor_position, char_len);
        ewl_text_context_release(tx);
        t->dirty = TRUE;

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the wrap value for
 * @param char_idx: The index to get the wrap value from
 * @return Returns the wrap value of the text at the given index
 * @brief Retrives the text wrap value at the given index
 */
Ewl_Text_Wrap
ewl_text_wrap_get(Ewl_Text *t, unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;
        int wrap = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (fmt && fmt->tx)
        {
                wrap = fmt->tx->wrap;
        }
        else
        {
                Ewl_Text_Context *tx;

                tx = ewl_text_context_default_create(t);
                wrap = tx->wrap;
                ewl_text_context_release(tx);
        }

        DRETURN_INT(wrap, DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text background colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the background colour at the cursor
 */
void
ewl_text_bg_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a)
{
        Ewl_Text_Context *change;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        change = ewl_text_context_new();
        change->style_colors.bg.r = r;
        change->style_colors.bg.g = g;
        change->style_colors.bg.b = b;
        change->style_colors.bg.a = a;

        ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_BG_COLOR, change);
        ewl_text_context_release(change);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text background colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param char_len: The length of text to apply the bg colour over
 * @return Returns no value
 * @brief This will set the bg colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_bg_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a,
                                        unsigned int char_len)
{
        Ewl_Text_Context *tx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (char_len == 0)
                DRETURN(DLEVEL_STABLE);

        tx = ewl_text_context_new();
        tx->style_colors.bg.r = r;
        tx->style_colors.bg.g = g;
        tx->style_colors.bg.b = b;
        tx->style_colors.bg.a = a;

        ewl_text_fmt_apply(t->formatting.nodes, EWL_TEXT_CONTEXT_MASK_BG_COLOR,
                                        tx, t->cursor_position, char_len);
        ewl_text_context_release(tx);
        t->dirty = TRUE;

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text background colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Get the text background colour at the given index
 */
void
ewl_text_bg_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
                                        unsigned int *b, unsigned int *a,
                                        unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (fmt && fmt->tx)
        {
                if (r) *r = fmt->tx->style_colors.bg.r;
                if (g) *g = fmt->tx->style_colors.bg.g;
                if (b) *b = fmt->tx->style_colors.bg.b;
                if (a) *a = fmt->tx->style_colors.bg.a;
        }
        else
        {
                Ewl_Text_Context *tx;

                tx = ewl_text_context_default_create(t);
                if (r) *r = tx->style_colors.bg.r;
                if (g) *g = tx->style_colors.bg.g;
                if (b) *b = tx->style_colors.bg.b;
                if (a) *a = tx->style_colors.bg.a;
                ewl_text_context_release(tx);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text glow colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the glow colour at the cursor
 */
void
ewl_text_glow_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a)
{
        Ewl_Text_Context *change;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        change = ewl_text_context_new();
        change->style_colors.glow.r = r;
        change->style_colors.glow.g = g;
        change->style_colors.glow.b = b;
        change->style_colors.glow.a = a;

        ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_GLOW_COLOR, change);
        ewl_text_context_release(change);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text glow colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param char_len: The length of text to apply the glow colour over
 * @return Returns no value
 * @brief This will set the glow colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_glow_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a,
                                        unsigned int char_len)
{
        Ewl_Text_Context *tx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (char_len == 0)
                DRETURN(DLEVEL_STABLE);

        tx = ewl_text_context_new();
        tx->style_colors.glow.r = r;
        tx->style_colors.glow.g = g;
        tx->style_colors.glow.b = b;
        tx->style_colors.glow.a = a;

        ewl_text_fmt_apply(t->formatting.nodes, EWL_TEXT_CONTEXT_MASK_GLOW_COLOR, tx,
                                                t->cursor_position, char_len);
        ewl_text_context_release(tx);
        t->dirty = TRUE;

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text glow colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Get the glow colour at the given index
 */
void
ewl_text_glow_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
                                        unsigned int *b, unsigned int *a,
                                        unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (fmt && fmt->tx)
        {
                if (r) *r = fmt->tx->style_colors.glow.r;
                if (g) *g = fmt->tx->style_colors.glow.g;
                if (b) *b = fmt->tx->style_colors.glow.b;
                if (a) *a = fmt->tx->style_colors.glow.a;
        }
        else
        {
                Ewl_Text_Context *tx;

                tx = ewl_text_context_default_create(t);
                if (r) *r = tx->style_colors.glow.r;
                if (g) *g = tx->style_colors.glow.g;
                if (b) *b = tx->style_colors.glow.b;
                if (a) *a = tx->style_colors.glow.a;
                ewl_text_context_release(tx);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text outline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the outline colour at the cursor
 */
void
ewl_text_outline_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
                                                unsigned int b, unsigned int a)
{
        Ewl_Text_Context *change;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        change = ewl_text_context_new();
        change->style_colors.outline.r = r;
        change->style_colors.outline.g = g;
        change->style_colors.outline.b = b;
        change->style_colors.outline.a = a;

        ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR, change);
        ewl_text_context_release(change);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text outline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param char_len: The length of text to apply the outline colour over
 * @return Returns no value
 * @brief This will set the outline colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_outline_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
                                                unsigned int b, unsigned int a,
                                                unsigned int char_len)
{
        Ewl_Text_Context *tx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (char_len == 0)
                DRETURN(DLEVEL_STABLE);

        tx = ewl_text_context_new();
        tx->style_colors.outline.r = r;
        tx->style_colors.outline.g = g;
        tx->style_colors.outline.b = b;
        tx->style_colors.outline.a = a;

        ewl_text_fmt_apply(t->formatting.nodes, EWL_TEXT_CONTEXT_MASK_OUTLINE_COLOR,
                                        tx, t->cursor_position, char_len);
        ewl_text_context_release(tx);
        t->dirty = TRUE;

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text outline colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Get the outline colour at the given index
 */
void
ewl_text_outline_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
                                                unsigned int *b, unsigned int *a,
                                                unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (fmt && fmt->tx)
        {
                if (r) *r = fmt->tx->style_colors.outline.r;
                if (g) *g = fmt->tx->style_colors.outline.g;
                if (b) *b = fmt->tx->style_colors.outline.b;
                if (a) *a = fmt->tx->style_colors.outline.a;
        }
        else
        {
                Ewl_Text_Context *tx;

                tx = ewl_text_context_default_create(t);
                if (r) *r = tx->style_colors.outline.r;
                if (g) *g = tx->style_colors.outline.g;
                if (b) *b = tx->style_colors.outline.b;
                if (a) *a = tx->style_colors.outline.a;
                ewl_text_context_release(tx);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text shadow colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the shadow colour at the cursor
 */
void
ewl_text_shadow_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
                                                unsigned int b, unsigned int a)
{
        Ewl_Text_Context *change;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        change = ewl_text_context_new();
        change->style_colors.shadow.r = r;
        change->style_colors.shadow.g = g;
        change->style_colors.shadow.b = b;
        change->style_colors.shadow.a = a;

        ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR, change);
        ewl_text_context_release(change);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text shadow colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param char_len: The length of text to apply the shadow colour over
 * @return Returns no value
 * @brief This will set the shadow colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_shadow_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
                                                unsigned int b, unsigned int a,
                                                unsigned int char_len)
{
        Ewl_Text_Context *tx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (char_len == 0)
                DRETURN(DLEVEL_STABLE);

        tx = ewl_text_context_new();
        tx->style_colors.shadow.r = r;
        tx->style_colors.shadow.g = g;
        tx->style_colors.shadow.b = b;
        tx->style_colors.shadow.a = a;

        ewl_text_fmt_apply(t->formatting.nodes, EWL_TEXT_CONTEXT_MASK_SHADOW_COLOR,
                                        tx, t->cursor_position, char_len);
        ewl_text_context_release(tx);
        t->dirty = TRUE;

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text shadow colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Retrieve the shadow colour at the given index
 */
void
ewl_text_shadow_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
                                                unsigned int *b, unsigned int *a,
                                                unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (fmt && fmt->tx)
        {
                if (r) *r = fmt->tx->style_colors.shadow.r;
                if (g) *g = fmt->tx->style_colors.shadow.g;
                if (b) *b = fmt->tx->style_colors.shadow.b;
                if (a) *a = fmt->tx->style_colors.shadow.a;
        }
        else
        {
                Ewl_Text_Context *tx;

                tx = ewl_text_context_default_create(t);
                if (r) *r = tx->style_colors.shadow.r;
                if (g) *g = tx->style_colors.shadow.g;
                if (b) *b = tx->style_colors.shadow.b;
                if (a) *a = tx->style_colors.shadow.a;
                ewl_text_context_release(tx);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text strikethrough colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the strikethrough colour at the cursor
 */
void
ewl_text_strikethrough_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
                                                unsigned int b, unsigned int a)
{
        Ewl_Text_Context *change;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        change = ewl_text_context_new();
        change->style_colors.strikethrough.r = r;
        change->style_colors.strikethrough.g = g;
        change->style_colors.strikethrough.b = b;
        change->style_colors.strikethrough.a = a;

        ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR, change);
        ewl_text_context_release(change);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text strikethrough colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param char_len: The length of text to apply the strikethrough colour over
 * @return Returns no value
 * @brief This will set the strikethrough colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_strikethrough_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
                                                unsigned int b, unsigned int a,
                                                unsigned int char_len)
{
        Ewl_Text_Context *tx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (char_len == 0)
                DRETURN(DLEVEL_STABLE);

        tx = ewl_text_context_new();
        tx->style_colors.strikethrough.r = r;
        tx->style_colors.strikethrough.g = g;
        tx->style_colors.strikethrough.b = b;
        tx->style_colors.strikethrough.a = a;

        ewl_text_fmt_apply(t->formatting.nodes, EWL_TEXT_CONTEXT_MASK_STRIKETHROUGH_COLOR,
                                        tx, t->cursor_position, char_len);
        ewl_text_context_release(tx);
        t->dirty = TRUE;

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text strikethrough colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Retrieve the strikethrough colour at the given index
 */
void
ewl_text_strikethrough_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
                                                unsigned int *b, unsigned int *a,
                                                unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (fmt && fmt->tx)
        {
                if (r) *r = fmt->tx->style_colors.strikethrough.r;
                if (g) *g = fmt->tx->style_colors.strikethrough.g;
                if (b) *b = fmt->tx->style_colors.strikethrough.b;
                if (a) *a = fmt->tx->style_colors.strikethrough.a;
        }
        else
        {
                Ewl_Text_Context *tx;

                tx = ewl_text_context_default_create(t);
                if (r) *r = tx->style_colors.strikethrough.r;
                if (g) *g = tx->style_colors.strikethrough.g;
                if (b) *b = tx->style_colors.strikethrough.b;
                if (a) *a = tx->style_colors.strikethrough.a;
                ewl_text_context_release(tx);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text underline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the underline colour at the cursor
 */
void
ewl_text_underline_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
                                                unsigned int b, unsigned int a)
{
        Ewl_Text_Context *change;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        change = ewl_text_context_new();
        change->style_colors.underline.r = r;
        change->style_colors.underline.g = g;
        change->style_colors.underline.b = b;
        change->style_colors.underline.a = a;

        ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR, change);
        ewl_text_context_release(change);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text underline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param char_len: The length of text to apply the underline colour over
 * @return Returns no value
 * @brief This will set the underline colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_underline_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
                                                unsigned int b, unsigned int a,
                                                unsigned int char_len)
{
        Ewl_Text_Context *tx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (char_len == 0)
                DRETURN(DLEVEL_STABLE);

        tx = ewl_text_context_new();
        tx->style_colors.underline.r = r;
        tx->style_colors.underline.g = g;
        tx->style_colors.underline.b = b;
        tx->style_colors.underline.a = a;

        ewl_text_fmt_apply(t->formatting.nodes, EWL_TEXT_CONTEXT_MASK_UNDERLINE_COLOR,
                                        tx, t->cursor_position, char_len);
        ewl_text_context_release(tx);
        t->dirty = TRUE;

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text underline colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Retrieve the underline colour at the given index
 */
void
ewl_text_underline_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
                                                unsigned int *b, unsigned int *a,
                                                unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (fmt && fmt->tx)
        {
                if (r) *r = fmt->tx->style_colors.outline.r;
                if (g) *g = fmt->tx->style_colors.outline.g;
                if (b) *b = fmt->tx->style_colors.outline.b;
                if (a) *a = fmt->tx->style_colors.outline.a;
        }
        else
        {
                Ewl_Text_Context *tx;

                tx = ewl_text_context_default_create(t);
                if (r) *r = tx->style_colors.outline.r;
                if (g) *g = tx->style_colors.outline.g;
                if (b) *b = tx->style_colors.outline.b;
                if (a) *a = tx->style_colors.outline.a;
                ewl_text_context_release(tx);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text double underline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @return Returns no value
 * @brief Set the double underline colour at the cursor
 */
void
ewl_text_double_underline_color_set(Ewl_Text *t, unsigned int r, unsigned int g,
                                                unsigned int b, unsigned int a)
{
        Ewl_Text_Context *change;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        change = ewl_text_context_new();
        change->style_colors.double_underline.r = r;
        change->style_colors.double_underline.g = g;
        change->style_colors.double_underline.b = b;
        change->style_colors.double_underline.a = a;

        ewl_text_current_fmt_set(t, EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR, change);
        ewl_text_context_release(change);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to set the text double underline colour of
 * @param r: The red value
 * @param g: The green value
 * @param b: The blue value
 * @param a: The alpha value
 * @param char_len: The length of text to apply the double underline colour over
 * @return Returns no value
 * @brief This will set the double_underline colour of the text from the current cursor position
 * to the given length.
 */
void
ewl_text_double_underline_color_apply(Ewl_Text *t, unsigned int r, unsigned int g,
                                                        unsigned int b, unsigned int a,
                                                        unsigned int char_len)
{
        Ewl_Text_Context *tx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (char_len == 0)
                DRETURN(DLEVEL_STABLE);

        tx = ewl_text_context_new();
        tx->style_colors.double_underline.r = r;
        tx->style_colors.double_underline.g = g;
        tx->style_colors.double_underline.b = b;
        tx->style_colors.double_underline.a = a;

        ewl_text_fmt_apply(t->formatting.nodes, EWL_TEXT_CONTEXT_MASK_DOUBLE_UNDERLINE_COLOR,
                                        tx, t->cursor_position, char_len);
        ewl_text_context_release(tx);
        t->dirty = TRUE;

        ewl_widget_configure(EWL_WIDGET(t));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param t: The Ewl_Text to get the text double underline colour from
 * @param r: Where to put the red value
 * @param g: Where to put the green value
 * @param b: Where to put the blue value
 * @param a: Where to put the alpha value
 * @param char_idx: The index to get the colour from
 * @return Returns no value
 * @brief Retrieve the double underline colour at the given index
 */
void
ewl_text_double_underline_color_get(Ewl_Text *t, unsigned int *r, unsigned int *g,
                                                unsigned int *b, unsigned int *a,
                                                unsigned int char_idx)
{
        Ewl_Text_Fmt_Node *fmt;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        fmt = ewl_text_fmt_get(t->formatting.nodes, char_idx);
        if (fmt && fmt->tx)
        {
                if (r) *r = fmt->tx->style_colors.double_underline.r;
                if (g) *g = fmt->tx->style_colors.double_underline.g;
                if (b) *b = fmt->tx->style_colors.double_underline.b;
                if (a) *a = fmt->tx->style_colors.double_underline.a;
        }
        else
        {
                Ewl_Text_Context *tx;

                tx = ewl_text_context_default_create(t);
                if (r) *r = tx->style_colors.double_underline.r;
                if (g) *g = tx->style_colors.double_underline.g;
                if (b) *b = tx->style_colors.double_underline.b;
                if (a) *a = tx->style_colors.double_underline.a;
                ewl_text_context_release(tx);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_theme_color_get(Ewl_Text *t, Ewl_Color_Set *color, char *name)
{
        char buf[128];
        int pos;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        pos = strlen(name) + 1;
        snprintf(buf, sizeof(buf), "%s/r", name);
        color->r = ewl_theme_data_int_get(EWL_WIDGET(t), buf);

        buf[pos] = 'g';
        color->g = ewl_theme_data_int_get(EWL_WIDGET(t), buf);

        buf[pos] = 'b';
        color->b = ewl_theme_data_int_get(EWL_WIDGET(t), buf);

        buf[pos] = 'a';
        color->a = ewl_theme_data_int_get(EWL_WIDGET(t), buf);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * @param t: The Ewl_Text to work with
 * @return Returns the default Ewl_Text_Context
 * @brief Retrieves the default Ewl_Text_Context.
 */
static Ewl_Text_Context *
ewl_text_context_default_create(Ewl_Text *t)
{
        const char *font;
        Ewl_Text_Context *tx = NULL, *tmp;
        int i;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, NULL);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, NULL);

        if (ewl_text_default_context)
        {
                ewl_text_context_acquire(ewl_text_default_context);
                DRETURN_PTR(ewl_text_default_context, DLEVEL_STABLE);
        }

        tmp = ewl_text_context_new();

        /* handle default values */
        font = ewl_theme_data_str_get(EWL_WIDGET(t), "font");
        if (font) tmp->font = ecore_string_instance(font);
        tmp->font_source = NULL;
        tmp->size = ewl_theme_data_int_get(EWL_WIDGET(t), "font_size");

        ewl_text_theme_color_get(t, &(tmp->color), "color");
        i = ewl_theme_data_int_get(EWL_WIDGET(t), "underline");
        if (i)
        {
                tmp->styles |= EWL_TEXT_STYLE_UNDERLINE;
                ewl_text_theme_color_get(t, &(tmp->style_colors.underline),
                                        "underline/color");
        }

        i = ewl_theme_data_int_get(EWL_WIDGET(t), "double_underline");
        if (i)
        {
                tmp->styles |= EWL_TEXT_STYLE_DOUBLE_UNDERLINE;
                ewl_text_theme_color_get(t, &(tmp->style_colors.double_underline),
                                        "double_underline/color");
        }

        i = ewl_theme_data_int_get(EWL_WIDGET(t), "strikethrough");
        if (i)
        {
                tmp->styles |= EWL_TEXT_STYLE_STRIKETHROUGH;
                ewl_text_theme_color_get(t, &(tmp->style_colors.strikethrough),
                                        "strikethrough/color");
        }

        i = ewl_theme_data_int_get(EWL_WIDGET(t), "shadow");
        if (i)
        {
                tmp->styles |= EWL_TEXT_STYLE_SHADOW;
                ewl_text_theme_color_get(t, &(tmp->style_colors.shadow),
                                        "shadow/color");
        }

        i = ewl_theme_data_int_get(EWL_WIDGET(t), "soft_shadow");
        if (i)
        {
                tmp->styles |= EWL_TEXT_STYLE_SOFT_SHADOW;
                ewl_text_theme_color_get(t, &(tmp->style_colors.shadow),
                                        "shadow/color");
        }

        i = ewl_theme_data_int_get(EWL_WIDGET(t), "far_shadow");
        if (i)
        {
                tmp->styles |= EWL_TEXT_STYLE_FAR_SHADOW;
                ewl_text_theme_color_get(t, &(tmp->style_colors.shadow),
                                        "shadow/color");
        }

        i = ewl_theme_data_int_get(EWL_WIDGET(t), "outline");
        if (i)
        {
                tmp->styles |= EWL_TEXT_STYLE_OUTLINE;
                ewl_text_theme_color_get(t, &(tmp->style_colors.outline),
                                        "outline/color");
        }

        i = ewl_theme_data_int_get(EWL_WIDGET(t), "glow");
        if (i)
        {
                tmp->styles |= EWL_TEXT_STYLE_GLOW;
                ewl_text_theme_color_get(t, &(tmp->style_colors.glow),
                                        "glow/color");
        }

        /* XXX grab the alignment and wrap data from the theme here */
        tmp->align = EWL_FLAG_ALIGN_LEFT;

        tx = ewl_text_context_find(tmp, EWL_TEXT_CONTEXT_MASK_NONE, NULL);
        ewl_text_context_release(tmp);

        /* setup the default context and acquire a ref on it so
         * it won't go away */
        ewl_text_default_context = tx;

        DRETURN_PTR(tx, DLEVEL_STABLE);
}

/*
 * This function checks if a given character is a utf character.
 * It only checks the first character in the string.
 */
static int
ewl_text_char_utf8_is(const char *c)
{
        unsigned const char *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(c, FALSE);

        t = (unsigned const char *)c;

        /* check for ascii chars first */
        if (t[0] < 0x80) DRETURN_INT(TRUE, DLEVEL_STABLE);

        /* the first byte will give use the length of the character, we 
         * already checked if it is an ASCII, wrong lengths like 1 and 5
         * are catched by the switch.
         * So we only have to check if the following bytes are valid UTF-8
         * trailing bytes. UTF-8 trailing bytes are the values between
         * 0x80 and 0xBF, or in other words everthing that looks like this
         * 0b10xxxxxx. Anding this with 0b11000000 (0xc0) must result in
         * 0b10000000 (0x80) else it is not a legal UTF-8 trailing byte
         * and thus not a valid utf8 character.
         */ 
        switch (EWL_TEXT_CHAR_BYTE_LEN(t))
        {
                case 4: 
                        if ((*(++t) & 0xc0) != 0x80) 
                                DRETURN_INT(FALSE, DLEVEL_STABLE);
                        /* fall through */
                case 3: 
                        if ((*(++t) & 0xc0) != 0x80) 
                                DRETURN_INT(FALSE, DLEVEL_STABLE);
                        /* fall through */
                case 2: 
                        if ((*(++t) & 0xc0) != 0x80) 
                                DRETURN_INT(FALSE, DLEVEL_STABLE);

                        DRETURN_INT(TRUE, DLEVEL_STABLE);
                default:
                /* this is actually:
                 * case 1:
                 *         We already checked if it is a 7-bit ASCII character,
                 *         so anything else with the length of 1 byte is not
                 *         a valid utf8 character
                 * case 5: case 6:
                 *         Although a character sequence of the length 5 or 6
                 *         is possible it is not a legal utf8 character */
                        break;
        }

        DRETURN_INT(FALSE, DLEVEL_STABLE);
}

/*
 * This function return the next character of a utf string.
 * The text pointer should point on the leading byte of the
 * current character, otherwise it will return the adress of
 * the next byte.
 */
char *
ewl_text_text_next_char(const char *text, unsigned int *idx)
{
        int len;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(text, NULL);

        len = EWL_TEXT_CHAR_BYTE_LEN(text);
        if (idx) *idx = len;

        DRETURN_PTR((char *)(text + len), DLEVEL_STABLE);
}

static void
ewl_text_size(Ewl_Text *t)
{
        int xx, yy, ww, hh;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        xx = CURRENT_X(t);
        yy = CURRENT_Y(t);
        hh = CURRENT_H(t);
        ww = CURRENT_W(t);

        evas_object_move(t->textblock, xx + t->offset.x, yy + t->offset.y);
        evas_object_resize(t->textblock, ww - t->offset.x, hh - t->offset.y);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_display(Ewl_Text *t)
{
        Evas_Coord w = 0, h = 0;
        Evas_Textblock_Cursor *cursor;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        ewl_text_size(t);

        evas_object_textblock_clear(t->textblock);

        cursor = (Evas_Textblock_Cursor *)
                evas_object_textblock_cursor_get(t->textblock);
        evas_textblock_cursor_text_append(cursor, "");

        if (t->length.chars > 0)
        {
                Ewl_Text_Context *cur_tx;
                unsigned int cur_pos;

                /* save these so we can restore the list state */
                cur_pos = ewl_text_cursor_position_get(t);
                cur_tx = t->formatting.tx;
                if (cur_tx) ewl_text_context_acquire(cur_tx);

                ewl_text_fmt_walk(t->formatting.nodes, ewl_text_cb_format);

                ewl_text_cursor_position_set(t, cur_pos);
                t->formatting.tx = cur_tx;
        }
        evas_object_textblock_size_formatted_get(t->textblock, &w, &h);

        /* Fallback, just in case we hit a corner case */
        if (!h) h = 1;
        if (!w) w = 1;

        ewl_object_preferred_inner_size_set(EWL_OBJECT(t), (int)w, (int)h);
        t->dirty = FALSE;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_cb_format(Ewl_Text_Fmt_Node *node, Ewl_Text *t, unsigned int byte_idx)
{
        Evas_Textblock_Cursor *cursor;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(node);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        ewl_text_context_format_string_create(node->tx);

        /* Don't free this as it's suppost to be const ... */
        cursor = (Evas_Textblock_Cursor *)
                        evas_object_textblock_cursor_get(t->textblock);

        /* only need to append if we aren't default */
        if (node->tx != ewl_text_default_context)
                evas_textblock_cursor_format_append(cursor, node->tx->format);

        if (!t->obscure)
        {
                char *ptr, tmp;
                ptr = t->text + byte_idx;
                tmp = *(ptr + node->byte_len);
                if (strlen(ptr) < node->byte_len)
                        DWARNING("Byte length of node %u overruns actual"
                                 " text %d", node->byte_len, (int)strlen(ptr));
                                
                *(ptr + node->byte_len) = '\0';

                ewl_text_plaintext_parse(t->textblock, ptr);
                *(ptr + node->byte_len) = tmp;
        }
        else
        {
                char *otxt, *ptr;
                size_t len;
                size_t i;

                len = strlen(t->obscure);
                otxt = alloca(len * node->char_len + 1);
                ptr = otxt;
                for (i = 0; i < node->char_len; i++)
                {
                        memcpy(ptr, t->obscure, len);
                        ptr += len;
                }
                *ptr = '\0';
                ewl_text_plaintext_parse(t->textblock, otxt);
        }

        evas_textblock_cursor_format_append(cursor, "-");

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_plaintext_parse(Evas_Object *tb, char *txt)
{
        Evas_Textblock_Cursor *cursor;
        char *tmp;
        unsigned int idx;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(tb);

        if (!txt) DRETURN(DLEVEL_STABLE);

        /* we don't free this cursor as it is actually const
         * Evas_Textblock_Cursor * and i'm casting it...  */
        cursor = (Evas_Textblock_Cursor *)evas_object_textblock_cursor_get(tb);
        for (tmp = txt; *tmp; tmp = ewl_text_text_next_char(tmp, &idx))
        {
                if (*tmp == '\n')
                {
                        *tmp = '\0';
                        if (*txt) evas_textblock_cursor_text_append(cursor, txt);
                        evas_textblock_cursor_format_append(cursor, "\n");
                        *tmp = '\n';

                        txt = ewl_text_text_next_char(tmp, &idx);
                }
                else if (*tmp == '\r' && *(tmp + 1) == '\n')
                {
                        *tmp = '\0';
                        if (*txt) evas_textblock_cursor_text_append(cursor, txt);
                        evas_textblock_cursor_format_append(cursor, "\n");
                        *tmp = '\r';
                        tmp = ewl_text_text_next_char(tmp, &idx);
                        txt = ewl_text_text_next_char(tmp, &idx);
                }
                else if (*tmp == '\t')
                {
                        *tmp = '\0';
                        if (*txt) evas_textblock_cursor_text_append(cursor, txt);
                        evas_textblock_cursor_format_append(cursor, "\t");
                        *tmp = '\t';
                        txt = ewl_text_text_next_char(tmp, &idx);
                }
        }
        if (*txt) evas_textblock_cursor_text_append(cursor, txt);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* This will give you a cursor into the textblock setup for your given
 * character index. You _MUST_ call evas_textblock_cursor_free(cursor)
 * on this object so it won't leak */
static Evas_Textblock_Cursor *
ewl_text_textblock_cursor_position(Ewl_Text *t, unsigned int byte_idx)
{
        Evas_Textblock_Cursor *cursor;
        unsigned int cur_byte_idx = 0;
        const char *txt;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, NULL);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, NULL);

        /* place the cursor at the first node in the textblock */
        cursor = evas_object_textblock_cursor_new(t->textblock);
        evas_textblock_cursor_node_first(cursor);
        while (TRUE)
        {
                txt = evas_textblock_cursor_node_format_get(cursor);

                /* if we have text this is a formatting node, need to see if
                 * this is a \n or \t as they are special */
                if (txt)
                {
                        /* do we have a \n or \t node? */
                        if ((!strcmp(txt, "\n")) || (!strcmp(txt, "\t")))
                        {
                                /* will this push us past the end? */
                                if ((cur_byte_idx + 1) > byte_idx)
                                {
                                        evas_textblock_cursor_pos_set(cursor,
                                                byte_idx - cur_byte_idx);
                                        break;
                                }
                                else cur_byte_idx++;
                        }
                }
                else
                {
                        int pos;

                        /* this is a text node, so check the length of the
                         * text against our current position and the idx we
                         * are looking for */
                        pos = evas_textblock_cursor_node_text_length_get(cursor);

                        /* if this would move us past our index, find the
                         * difference between our desired index and the
                         * current index and set that */
                        if ((cur_byte_idx + pos) > byte_idx)
                        {
                                evas_textblock_cursor_pos_set(cursor,
                                                byte_idx - cur_byte_idx);
                                break;
                        }
                        cur_byte_idx += pos;
                }

                /* if we fail to goto the next node, just assume we're at
                 * the end of the text and jump the cursor there */
                if (!evas_textblock_cursor_node_next(cursor))
                {
                        evas_textblock_cursor_node_last(cursor);
                        evas_textblock_cursor_char_last(cursor);
                        break;
                }

                /* This shouldn't happen, we've moved past our index. Just
                 * checking so the loop isn't (hopefully) infinite */
                if (cur_byte_idx > byte_idx)
                {
                        DWARNING("This shouldn't happen, breaking loop.");
                        break;
                }
        }

        DRETURN_PTR(cursor, DLEVEL_STABLE);
}

static unsigned int
ewl_text_textblock_cursor_to_index(Evas_Textblock_Cursor *cursor)
{
        unsigned int char_idx = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(cursor, 0);

        /* this gives the index inside the _node_ the cursor points to, we
         * then need to add the length of all the nodes before it plus any
         * formatting nodes that are \n or \t */
        char_idx = evas_textblock_cursor_pos_get(cursor);

        while (evas_textblock_cursor_node_prev(cursor))
        {
                const char *txt;

                txt = evas_textblock_cursor_node_format_get(cursor);
                if (!txt) char_idx += evas_textblock_cursor_node_text_length_get(cursor);
                else if (!strcmp(txt, "\n")) char_idx ++;
                else if (!strcmp(txt, "\t")) char_idx ++;

        }

        DRETURN_INT(char_idx, DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_text_cb_configure(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Text *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TYPE);

        /* don't do anything if we're obscured */
        if (!REVEALED(w)) DRETURN(DLEVEL_STABLE);

        t = EWL_TEXT(w);
        if (t->textblock)
        {
                Ewl_Widget *child;

                ewl_text_size(t);
                if (t->dirty) ewl_text_display(t);

                ewl_text_triggers_place(t);

                /*
                 * configure all children. We actually only need to configure
                 * the area parts, but it doesn't hurt if we call the configure
                 * call for the trigger, too
                 */
                ecore_dlist_first_goto(EWL_CONTAINER(t)->children);
                while ((child = ecore_dlist_next(EWL_CONTAINER(t)->children)))
                        ewl_widget_configure(child);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The reveal callback
 */
void
ewl_text_cb_reveal(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Text *t;
        Ewl_Embed *emb;
        Ewl_Text_Context *ctx;
        Evas_Textblock_Style *st;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TYPE);

        t = EWL_TEXT(w);
        if (t->textblock)
        {
                DWARNING("We have a textblock when we shoudn't.");
                DRETURN(DLEVEL_STABLE);
        }

        /* find the embed so we know the evas */
        emb = ewl_embed_widget_find(w);
        if (!emb) DRETURN(DLEVEL_STABLE);

        /* create the textblock */
        t->textblock = ewl_embed_object_request(emb, "textblock");
        if (!t->textblock)
                t->textblock = evas_object_textblock_add(emb->canvas);

        if (t->textblock)
        {
                char *fmt2;
                int len;

                ctx = ewl_text_context_default_create(t);
                ewl_text_context_format_string_create(ctx);

                len = strlen(ctx->format) + 12;  /* 12 == DEFAULT='' + \n + \0 */
                fmt2 = NEW(char, len);
                snprintf(fmt2, len, "DEFAULT='%s'\n", ctx->format);

                st = evas_textblock_style_new();
                evas_textblock_style_set(st, fmt2);
                evas_object_textblock_style_set(t->textblock, st);
                evas_textblock_style_free(st);

                ewl_text_context_release(ctx);
                FREE(fmt2);

                evas_object_pass_events_set(t->textblock, 1);
                evas_object_smart_member_add(t->textblock, w->smart_object);
                evas_object_raise(t->textblock);

                ewl_text_display(t);
                evas_object_show(t->textblock);
        }

        ewl_text_triggers_place(t);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The realize callback
 */
void
ewl_text_cb_realize(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Text *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TYPE);

        t = EWL_TEXT(w);

        if (t->min.size_string || t->max.size_string)
                ewl_text_minmax_size_update(t);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The obscure callback
 */
void
ewl_text_cb_obscure(Ewl_Widget *w, void *ev __UNUSED__,
                                        void *data __UNUSED__)
{
        Ewl_Text *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TYPE);

        t = EWL_TEXT(w);

        if (t->textblock)
        {
                Ewl_Embed *emb;

                emb = ewl_embed_widget_find(w);
                evas_object_textblock_clear(t->textblock);
                ewl_embed_object_cache(emb, t->textblock);
                t->textblock = NULL;
        }

        ewl_text_triggers_unrealize(t);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The show callback
 */
void
ewl_text_cb_show(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Text *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TYPE);

        t = EWL_TEXT(w);
        if (t->textblock)
        {
                evas_object_show(t->textblock);
                ewl_text_triggers_show(t);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The hide callback
 */
void
ewl_text_cb_hide(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Text *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TYPE);

        t = EWL_TEXT(w);
        if (t->textblock) evas_object_hide(t->textblock);
        ewl_text_triggers_hide(t);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_text_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Text *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TYPE);

        t = EWL_TEXT(w);

        IF_FREE(t->obscure);
        /* Note, we don't explictly destroy the triggers or the selection
         * because they will be cleared, because they are children of the
         * text widget itself */
        IF_FREE_LIST(t->triggers);
        t->selection = NULL;

        ewl_text_fmt_destroy(t->formatting.nodes);
        t->formatting.nodes = NULL;
        t->formatting.tx = NULL;

        IF_FREE(t->text);
        IF_RELEASE(t->min.size_string);
        IF_RELEASE(t->max.size_string);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: The Ewl_Event_Mouse_Down
 * @param data: UNUSED
 * @return Returns no value
 * @brief The mouse down callback
 */
void
ewl_text_cb_mouse_down(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Text *t;
        Ewl_Text_Trigger *sel;
        Ewl_Event_Mouse *event;
        unsigned int char_idx = 0;
        unsigned int modifiers;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TYPE);

        event = ev;
        t = EWL_TEXT(w);
        ewl_callback_append(w, EWL_CALLBACK_MOUSE_MOVE,
                                ewl_text_cb_mouse_move, NULL);

        if (!t->selection)
                t->selection = ewl_text_selection_new(t);
        
        sel = EWL_TEXT_TRIGGER(t->selection);

        char_idx = ewl_text_coord_index_map(EWL_TEXT(w), event->x, event->y);
        ewl_text_cursor_position_set(t, char_idx);
        modifiers = ewl_ev_modifiers_get();
        if (modifiers & EWL_KEY_MODIFIER_SHIFT)
        {
                ewl_text_selection_select_to(EWL_TEXT_TRIGGER(t->selection),
                                                                char_idx);
        }
        else
        {
                ewl_widget_hide(t->selection);
                ewl_text_trigger_areas_cleanup(sel);
                ewl_widget_show(t->selection);

                ewl_text_trigger_start_pos_set(sel, char_idx);
                ewl_text_trigger_base_set(sel, char_idx);
                ewl_text_trigger_length_set(sel, 0);
        }
        t->in_select = TRUE;
        ewl_text_trigger_position(t, sel);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: The Ewl_Event_Mouse_Up data
 * @param data: UNUSED
 * @return Returns no value
 * @brief The mouse up callback
 */
void
ewl_text_cb_mouse_up(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Text *t;
        Ewl_Event_Mouse *event;
        unsigned int modifiers;
        unsigned int char_idx = 0;


        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TYPE);

        event = ev;
        t = EWL_TEXT(w);

        if (!t->in_select) DRETURN(DLEVEL_STABLE);

        t->in_select = FALSE;
        ewl_callback_del(w, EWL_CALLBACK_MOUSE_MOVE, ewl_text_cb_mouse_move);
        char_idx = ewl_text_coord_index_map(t, event->x, event->y);
        ewl_text_cursor_position_set(t, char_idx);

        modifiers = ewl_ev_modifiers_get();
        if (modifiers & EWL_KEY_MODIFIER_SHIFT)
                ewl_text_selection_select_to(EWL_TEXT_TRIGGER(t->selection),
                                char_idx);
        ewl_text_trigger_position(t, EWL_TEXT_TRIGGER(t->selection));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: The Ewl_Event_Mouse_Move data
 * @param data: UNUSED
 * @return Returns no value
 * @brief The mouse move callback
 */
void
ewl_text_cb_mouse_move(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Text *t;
        Ewl_Event_Mouse *event;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TYPE);

        event = ev;
        t = EWL_TEXT(w);

        if (t->in_select)
        {
                unsigned int char_idx = 0;

                char_idx = ewl_text_coord_index_map(EWL_TEXT(w), event->x, event->y);
                ewl_text_selection_select_to(EWL_TEXT_TRIGGER(t->selection), char_idx);
                ewl_text_trigger_position(t, EWL_TEXT_TRIGGER(t->selection));
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_key_down(Ewl_Widget *w, void *ev, void *data __UNUSED__)
{
        Ewl_Text *t;
        Ewl_Event_Key *event;
        Ewl_Text_Trigger *sel;
        unsigned int pos = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TYPE);

        t = EWL_TEXT(w);
        event = ev;

        if ((!event->keyname) || (!event->keyname[0]))
                DRETURN(DLEVEL_STABLE);

        if ( (!(event->modifiers & EWL_KEY_MODIFIER_SHIFT))
             || (event->keyname[1] == '\0'))
        {
                DRETURN(DLEVEL_STABLE);
        }

        if (!t->selection)
        {
                t->selection = ewl_text_selection_new(t);

                /* This can happen after key/mouse actions, so default to
                 * wherever the cursor is at this point, not 0
                 */
                ewl_text_trigger_base_set(EWL_TEXT_TRIGGER(t->selection), 
                                ewl_text_cursor_position_get(t));

                /* Same problem as mouse_down... place it inside the
                 * text widget */
                ewl_object_position_request(EWL_OBJECT(t->selection),
                                CURRENT_X(w) + 1, CURRENT_Y(w) + 1);
        }

        sel = EWL_TEXT_TRIGGER(t->selection);

        if (!strcmp(event->keyname, "Left"))
        {
                if (event->modifiers & EWL_KEY_MODIFIER_CTRL)
                        pos = ewl_text_cursor_position_word_previous_get(t);
                else
                {
                        pos = t->cursor_position;
                        if (pos > 0) pos--;
                }
        }
        
        else if (!strcmp(event->keyname, "Right"))
        {
                if (event->modifiers & EWL_KEY_MODIFIER_CTRL)
                        pos = ewl_text_cursor_position_word_next_get(t);
                else
                {
                        pos = t->cursor_position;
                        if (pos < t->length.chars) pos++;
                }
        }

        else if (!strcmp(event->keyname, "Up"))
        {
                pos = ewl_text_cursor_position_line_up_get(t);
        }
        else if (!strcmp(event->keyname, "Down"))
        {
                pos = ewl_text_cursor_position_line_down_get(t);
        }
        else if (!strcmp(event->keyname, "Home"))
        {
                if (event->modifiers & EWL_KEY_MODIFIER_CTRL)
                        pos = 0;
                else
                        pos = ewl_text_cursor_position_line_start_get(t);
        }
        else if (!strcmp(event->keyname, "End"))
        {
                if (event->modifiers & EWL_KEY_MODIFIER_CTRL)
                        pos = t->length.chars;
                else
                        pos = ewl_text_cursor_position_line_end_get(t);
        }
        else
                DRETURN(DLEVEL_STABLE);

        /* Resize the selection */

        ewl_text_selection_select_to(sel, pos);
        ewl_text_trigger_position(t, sel);
        ewl_text_cursor_position_set(t, pos);
        ewl_widget_configure(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void
ewl_text_cb_selection_clear(Ewl_Widget *w, void *ev __UNUSED__, 
                                void *data __UNUSED__)
{
        Ewl_Text *t;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_TEXT_TYPE);

        t = EWL_TEXT(w);

        if (t->selection)
        {
                ewl_widget_destroy(t->selection);
                t->selection = NULL;
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/* create the selection */
static Ewl_Widget *
ewl_text_selection_new(Ewl_Text *t)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, NULL);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, NULL);

        w = ewl_text_trigger_new(EWL_TEXT_TRIGGER_TYPE_SELECTION);
        ewl_container_child_append(EWL_CONTAINER(t), w);
        ewl_widget_internal_set(w, TRUE);

        ewl_text_trigger_start_pos_set(EWL_TEXT_TRIGGER(w), 0);
        ewl_text_trigger_length_set(EWL_TEXT_TRIGGER(w), 0);

        ewl_widget_show(w);

        DRETURN_PTR(w, DLEVEL_STABLE);
}

static void
ewl_text_selection_select_to(Ewl_Text_Trigger *s, unsigned int char_idx)
{
        Ewl_Text *t;
        unsigned int base;
        char *txt;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_TEXT_TRIGGER_TYPE);

        t = EWL_TEXT(s->text_parent);
        base = ewl_text_trigger_base_get(s);

        if (char_idx < base)
        {
                ewl_text_trigger_start_pos_set(s, char_idx);
                ewl_text_trigger_length_set(s, base - char_idx);
        }
        else
        {
                ewl_text_trigger_start_pos_set(s, base);
                ewl_text_trigger_length_set(s, char_idx - base);
        }

        if (t->obscure)
        {
                char *ptr;
                unsigned int obs_len, len, i;

                len = ewl_text_trigger_length_get(s);
                obs_len = strlen(t->obscure);
                txt = ptr = malloc((obs_len * len) + 1);
                for(i = 0; i < len; i++)
                {
                        memcpy(ptr, t->obscure, obs_len);
                        ptr += obs_len;
                }
                *ptr = '\0';
        }
        else
                txt = ewl_text_selection_text_get(t);

        /* set the primary selection text */
        if (txt)
        {
                Ewl_Embed *emb;

                emb = ewl_embed_widget_find(EWL_WIDGET(t));
                ewl_embed_selection_text_set(emb, EWL_WIDGET(t), txt);
                FREE(txt);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @return Returns no value
 * @brief The child add callback
 */
void
ewl_text_cb_child_add(Ewl_Container *c, Ewl_Widget *w)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_TEXT_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /* if this is a trigger then add it as such */
        if (EWL_TEXT_TRIGGER_IS(w))
                ewl_text_trigger_add(EWL_TEXT(c), EWL_TEXT_TRIGGER(w));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param c: The container to work with
 * @param w: The widget to work with
 * @param idx: UNUSED
 * @return Returns no value
 * @brief The child remove callback
 */
void
ewl_text_cb_child_remove(Ewl_Container *c, Ewl_Widget *w, int idx __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(c);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(c, EWL_TEXT_TYPE);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);

        /* if it is a trigger, we need to treat it special */
        if (EWL_TEXT_TRIGGER_IS(w))
        {
                Ewl_Text_Trigger *trigger;

                trigger = EWL_TEXT_TRIGGER(w);
                ewl_text_trigger_areas_cleanup(trigger);
                trigger->text_parent = NULL;

                if (trigger->type == EWL_TEXT_TRIGGER_TYPE_TRIGGER)
                {
                        ecore_list_goto(EWL_TEXT(c)->triggers, trigger);
                        ecore_list_remove(EWL_TEXT(c)->triggers);
                }
                else
                {
                        /* for debug */
                        if (EWL_TEXT(c)->selection != w)
                                DWARNING("We are removing a selection, that"
                                         "isn't our own. WTF is happening?");
                                        
                        EWL_TEXT(c)->selection = NULL;
                }
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_current_fmt_set(Ewl_Text *t, unsigned int context_mask,
                                Ewl_Text_Context *change)
{
        Ewl_Text_Context *old = NULL, *new;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_PARAM_PTR(change);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        /* if we've already made some formatting changes before setting the
         * text make sure we use the current context. Else, use the current
         * nodes context. */
        if (t->formatting.tx)
        {
                old = t->formatting.tx;
        }
        else
        {
                Ewl_Text_Fmt_Node *fmt;

                fmt = ewl_text_fmt_get_current(t->formatting.nodes);
                if (fmt)
                {
                        old = fmt->tx;

                        /* grab on to this so releasing later doesn't screw
                         * things up */
                        ewl_text_context_acquire(old);
                }
                else old = ewl_text_context_default_create(t);
        }

        new = ewl_text_context_find(old, context_mask, change);
        if (old) ewl_text_context_release(old);

        t->formatting.tx = new;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_trigger_position(Ewl_Text *t, Ewl_Text_Trigger *trig)
{
        Evas_Textblock_Cursor *cur1, *cur2;
        Eina_List *rects;
        unsigned int byte_idx = 0, byte_len = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_PARAM_PTR(trig);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);
        DCHECK_TYPE(trig, EWL_TEXT_TRIGGER_TYPE);

        if (trig->char_len == 0)
                DRETURN(DLEVEL_STABLE);

        ewl_text_trigger_areas_cleanup(trig);
        ewl_text_fmt_char_to_byte(t->formatting.nodes,
                                        trig->char_pos, trig->char_len - 1,
                                        &byte_idx, &byte_len);

        cur1 = ewl_text_textblock_cursor_position(t, byte_idx);
        cur2 = ewl_text_textblock_cursor_position(t, byte_idx + byte_len);

        /* get all the rectangles and create areas with them */
        rects = evas_textblock_cursor_range_geometry_get(cur1, cur2);
        while (rects)
        {
                Evas_Textblock_Rectangle *tr;

                tr = rects->data;
                ewl_text_trigger_area_add(t, trig, tr->x + CURRENT_X(t),
                                                tr->y + CURRENT_Y(t),
                                                tr->w, tr->h);

                FREE(tr);
                rects = eina_list_remove_list(rects, rects);
        }
        evas_textblock_cursor_free(cur1);
        evas_textblock_cursor_free(cur2);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_triggers_remove(Ewl_Text *t)
{
        Ewl_Text_Trigger *trig;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (!t->triggers)
                DRETURN(DLEVEL_STABLE);

        while ((trig = ecore_list_first_remove(t->triggers)))
                ewl_widget_destroy(EWL_WIDGET(trig));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/* if we move the text (insertion, deleteion, etc) we need to shift the
 * position of the current cursors so they appear in the correct positions */
static void
ewl_text_triggers_shift(Ewl_Text *t, unsigned int char_pos,
                                        unsigned int char_len,
                                        unsigned int del)
{
        Ewl_Text_Trigger *cur;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (!t->triggers)
                DRETURN(DLEVEL_STABLE);

        ecore_list_first_goto(t->triggers);
        while ((cur = ecore_list_next(t->triggers)))
        {
                /* check if the change is after the trigger */
                if (char_pos >= (cur->char_pos + cur->char_len))
                        continue;

                /* change is completely before the trigger */
                if ((char_pos + char_len) < cur->char_pos)
                 {
                        if (del) cur->char_pos -= char_len;
                        else cur->char_pos += char_len;
                        continue;
                }

                if (del)
                {
                        /* delete the entire trigger? */
                        if ((char_pos <= cur->char_pos) &&
                                        ((char_pos + char_len) >=
                                                 (cur->char_pos + cur->char_len)))
                        {
                                int index;

                                index = ecore_list_index(t->triggers);
                                if (index == 0)
                                {
                                        DWARNING("Is this possible?");
                                }
                                else
                                {
                                        index --;

                                        /* remove the node before the
                                         * current one as _next will put us
                                         * on the next node */
                                        ecore_list_index_goto(t->triggers, index);
                                        ecore_list_remove(t->triggers);
                                        ecore_list_index_goto(t->triggers, index);
                                }
                                continue;
                        }

                        /* delete part of the start of the trigger */
                        if (char_pos <= cur->char_pos)
                        {
                                cur->char_len -= ((char_pos + char_len) - cur->char_pos);
                                continue;
                        }

                        /* delete from the center of the trigger */
                        if ((char_pos >= cur->char_pos) &&
                                        ((char_pos + char_len) <=
                                                 (cur->char_pos + cur->char_len)))
                        {
                                cur->char_len -= char_len;
                                continue;
                        }

                        /* must be deleted from the end of the trigger then */
                        cur->char_len = char_pos - cur->char_pos;
                }
                else
                {
                        /* we are inserting, just see if we are before */
                        if (char_pos < cur->char_pos)
                        {
                                cur->char_pos += char_len;
                                continue;
                        }
                        cur->char_len += char_len;
                }
          }

         DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param t: The ewl_text to work with
 * @return Returns no value
 * @brief Positions all of the triggers in the text
 */
static void
ewl_text_triggers_place(Ewl_Text *t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (t->triggers)
        {
                Ewl_Text_Trigger *cur;

                ecore_list_first_goto(t->triggers);
                while ((cur = ecore_list_next(t->triggers)))
                        ewl_text_trigger_position(t, cur);
        }

        if (t->selection)
                ewl_text_trigger_position(t, EWL_TEXT_TRIGGER(t->selection));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param t: The ewl_text to work with
 * @return Returns no value
 * @brief Sets all of the triggers in the text @a t as unrealized
 */
static void
ewl_text_triggers_unrealize(Ewl_Text *t)
{
        Ewl_Text_Trigger *cur;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (t->triggers)
        {
                ecore_list_first_goto(t->triggers);
                while ((cur = ecore_list_next(t->triggers)))
                        ewl_widget_unrealize(EWL_WIDGET(cur));
        }

        if (t->selection) ewl_widget_unrealize(t->selection);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param t: The ewl_text to work with
 * @return Returns no value
 * @brief Shows all triggers in text @a t
 */
static void
ewl_text_triggers_show(Ewl_Text *t)
{
        Ewl_Text_Trigger *cur;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        if (t->triggers)
        {
                ecore_list_first_goto(t->triggers);
                while ((cur = ecore_list_next(t->triggers)))
                        ewl_widget_show(EWL_WIDGET(cur));
        }

        if (t->selection) ewl_widget_show(t->selection);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param t: The text to work with
 * @return Returns no value
 * @brief Hides all of the triggers in the text @a t
 */
static void
ewl_text_triggers_hide(Ewl_Text *t)
{
        Ewl_Text_Trigger *cur;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);

        /* hide the triggers */
        if (t->triggers)
        {
                ecore_list_first_goto(t->triggers);
                while ((cur = ecore_list_next(t->triggers)))
                        ewl_widget_hide(EWL_WIDGET(cur));
        }

        /* hide the selection */
        if (t->selection) ewl_widget_hide(t->selection);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_text_trigger_add(Ewl_Text *t, Ewl_Text_Trigger *trigger)
{
        Ewl_Text_Trigger *cur = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(t);
        DCHECK_PARAM_PTR(trigger);
        DCHECK_TYPE(t, EWL_TEXT_TYPE);
        DCHECK_TYPE(trigger, EWL_TEXT_TRIGGER_TYPE);

        /* this code there is for both triggers and selections */
        trigger->text_parent = t;

        /* the rest will be for real triggers only */
        if (trigger->type == EWL_TEXT_TRIGGER_TYPE_SELECTION)
                DRETURN(DLEVEL_STABLE);

        /* create the trigger list if needed */
        if (!t->triggers)
                t->triggers = ecore_list_new();

        /* if we have no length, we start past the end of the text, or we
         * extend past the end of the text then return an error */
        if ((trigger->char_len == 0)
                        || ((trigger->char_pos + trigger->char_len) > t->length.chars))
                DRETURN(DLEVEL_STABLE);


        /* check now for overlapping */
        ecore_list_first_goto(t->triggers);
        while ((cur = ecore_list_next(t->triggers)))
        {
                if (trigger->char_pos < cur->char_pos)
                {
                        if ((trigger->char_pos + trigger->char_len) <= cur->char_pos)
                                continue;

                        DWARNING("Overlapping triggers are not allowed.");
                        DRETURN(DLEVEL_STABLE);
                }

                if ((trigger->char_pos >= (cur->char_pos + cur->char_len)))
                        continue;

                if ((trigger->char_pos >= cur->char_pos)
                                && (trigger->char_pos <= (cur->char_pos + cur->char_len)))
                {
                        DWARNING("Overlapping triggers are not allowed.");
                        DRETURN(DLEVEL_STABLE);
                }
        }

        if (cur)
        {
                /* we need to set our position to the one before the one we
                 * are on because the _next call in the while will have
                 * advanced us to the next node, but we want to insert
                  * at the one before that */
                ecore_list_index_goto(t->triggers, ecore_list_index(t->triggers) - 1);
                ecore_list_insert(t->triggers, trigger);
        }
        else
                ecore_list_append(t->triggers, trigger);

        DRETURN(DLEVEL_STABLE);
}

static unsigned int
ewl_text_drawn_byte_to_char(Ewl_Text *t, unsigned int byte_idx)
{
        unsigned int char_idx = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        if (!t->obscure)
                ewl_text_fmt_byte_to_char(t->formatting.nodes, byte_idx,
                                                0, &char_idx, NULL);
        else
        {
                size_t len = strlen(t->obscure);

                if (len != 0)
                        char_idx = byte_idx / len;
                else
                        char_idx = ewl_text_length_get(t);
        }

        DRETURN_INT(char_idx, DLEVEL_STABLE);
}

static unsigned int
ewl_text_char_to_drawn_byte(Ewl_Text *t, unsigned int char_idx)
{
        unsigned int byte_idx = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(t, 0);
        DCHECK_TYPE_RET(t, EWL_TEXT_TYPE, 0);

        if (!t->obscure)
                ewl_text_fmt_char_to_byte(t->formatting.nodes, char_idx,
                                                0, &byte_idx, NULL);
        else
                byte_idx = char_idx * strlen(t->obscure);

        DRETURN_INT(byte_idx, DLEVEL_STABLE);
}

