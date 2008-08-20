/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_TEXT_H
#define EWL_TEXT_H

/**
 * @addtogroup Ewl_Text Ewl_Text: A text layout widget
 * @brief Defines a class for text layout and formatting
 *
 * @remarks Inherits from Ewl_Container.
 * @if HAVE_IMAGES
 * @image html Ewl_Text_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /text/file
 * @themekey /text/group
 * @themekey /text/font
 * @themekey /text/font_size
 * @themekey /text/color/r
 * @themekey /text/color/g
 * @themekey /text/color/b
 * @themekey /text/color/a
 */

/**
 * @def EWL_TEXT_TYPE
 * The type name for the Ewl_Text widget
 */
#define EWL_TEXT_TYPE "text"

/**
 * @def EWL_TEXT_IS(w)
 * Returns TRUE if the widget is an Ewl_Text, FALSE otherwise
 */
#define EWL_TEXT_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TEXT_TYPE))

/**
 * @def EWL_TEXT_SELECTION_TYPE
 * The type name for the Ewl_Text_Selection widget
 */
#define EWL_TEXT_SELECTION_TYPE "selection"

/**
 * @def EWL_TEXT_SELECTION_IS(w)
 * Returns TRUE if the widget is an Ewl_Text_Selection, FALSE otherwise
 */
#define EWL_TEXT_SELECTION_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_TEXT_SELECTION_TYPE))

/**
 * @def EWL_TEXT(text)
 * Typecasts a pointer to an Ewl_Text pointer.
 */
#define EWL_TEXT(x) ((Ewl_Text *)x)

/**
 * Provides for layout of text as well as formatting portions of the text in
 * different ways.
 */
typedef struct Ewl_Text Ewl_Text;

/**
 * @brief Inherits from the Ewl_Container class and extends it to provide text layout
 * and formatting
 */
struct Ewl_Text
{
        Ewl_Container container;  /**< Inherit from Ewl_Container */
        void *textblock;          /**< The Evas_Object_Textblock */

        char *text;               /**< The text itself */
        char *obscure;            /**< The utf8 character to be displayed
                                       instead of the real text */
        struct
        {
                unsigned int chars;     /**< Number of characters in the text */
                unsigned int bytes;     /**< Number of bytes in the text */
                unsigned int max_chars; /**< Maximal number of characters in 
                                                the text */
        } length;                 /**< Different text lengths */

        unsigned int total_size;  /**< The total size we've alloc'd for text */
        unsigned int cursor_position;   /**< The cursor position */

        struct
        {
                void *nodes;      /**< The formatting nodes */
                void *tx;         /**< The current formatting context */
        } formatting;             /**< Holds the formatting information */

        Ecore_List *triggers;     /**< The list of triggers */
        Ecore_List *areas_cache;  /**< unused areas */
        Ewl_Widget *selection;    /**< The current selection */

        struct
        {
                int x;            /**< X offset for layout */
                int y;            /**< Y offset for layout */
        } offset;                 /**< Layout offset values */

        unsigned char delete_count;     /**< Number of deletes */
        unsigned char in_select;        /**< Are we in select mode? */

        unsigned char selectable;       /**< Is the text selectable? */
        unsigned char dirty;            /**< Does the text need layout? */
};

Ewl_Widget      *ewl_text_new(void);
int              ewl_text_init(Ewl_Text *t);

unsigned int     ewl_text_length_get(Ewl_Text *t);
void             ewl_text_length_maximum_set(Ewl_Text *t,
                                        unsigned int char_num);
unsigned int     ewl_text_length_maximum_get(Ewl_Text *t);

void             ewl_text_offsets_set(Ewl_Text *t, int x, int y);
void             ewl_text_offsets_get(Ewl_Text *t, int *x, int *y);

void             ewl_text_index_geometry_map(Ewl_Text *t,
                                        unsigned int char_idx,
                                        int *x, int *y,
                                        int *w, int *h);
unsigned int     ewl_text_coord_index_map(Ewl_Text *t, int x, int y);

char            *ewl_text_text_get(Ewl_Text *t);
void             ewl_text_text_set(Ewl_Text *t, const char *text);
void             ewl_text_clear(Ewl_Text *t);

void             ewl_text_text_prepend(Ewl_Text *t, const char *text);
void             ewl_text_text_append(Ewl_Text *t, const char *text);
void             ewl_text_text_insert(Ewl_Text *t, const char *text,
                                        unsigned int char_idx);
void             ewl_text_text_delete(Ewl_Text *t, unsigned int length);

void             ewl_text_obscure_set(Ewl_Text *t, const char *utf8_character);

void             ewl_text_selectable_set(Ewl_Text *t, unsigned int selectable);
unsigned int     ewl_text_selectable_get(Ewl_Text *t);

char            *ewl_text_selection_text_get(Ewl_Text *t);
unsigned int     ewl_text_has_selection(Ewl_Text *t);
Ewl_Widget      *ewl_text_selection_get(Ewl_Text *t);
void             ewl_text_select(Ewl_Text *t, unsigned int char_idx,
                                        unsigned int char_len);
void             ewl_text_all_select(Ewl_Text *t);

void             ewl_text_cursor_position_set(Ewl_Text *t,
                                        unsigned int char_pos);
unsigned int     ewl_text_cursor_position_get(Ewl_Text *t);

unsigned int     ewl_text_cursor_position_line_up_get(Ewl_Text *t);
unsigned int     ewl_text_cursor_position_line_down_get(Ewl_Text *t);

void             ewl_text_font_set(Ewl_Text *t, const char *font);
void             ewl_text_font_apply(Ewl_Text *t, const char *font,
                                        unsigned int char_len);
char            *ewl_text_font_get(Ewl_Text *t, unsigned int char_idx);

void             ewl_text_font_source_set(Ewl_Text *t, const char *source,
                                        const char *font);
void             ewl_text_font_source_apply(Ewl_Text *t, const char *source,
                                        const char *font,
                                        unsigned int char_len);
char            *ewl_text_font_source_get(Ewl_Text *t, unsigned int char_idx);

void             ewl_text_font_size_set(Ewl_Text *t, unsigned int size);
void             ewl_text_font_size_apply(Ewl_Text *t, unsigned int size,
                                        unsigned int char_len);
unsigned int     ewl_text_font_size_get(Ewl_Text *t, unsigned int char_idx);

void             ewl_text_color_set(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a);
void             ewl_text_color_apply(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a,
                                        unsigned int char_len);
void             ewl_text_color_get(Ewl_Text *t,
                                        unsigned int *r, unsigned int *g,
                                        unsigned int *b, unsigned int *a,
                                        unsigned int char_idx);

void             ewl_text_align_set(Ewl_Text *t, unsigned int align);
void             ewl_text_align_apply(Ewl_Text *t, unsigned int align,
                                        unsigned int char_len);
unsigned int     ewl_text_align_get(Ewl_Text *t, unsigned int char_idx);

void             ewl_text_styles_set(Ewl_Text *t, unsigned int styles);
void             ewl_text_styles_apply(Ewl_Text *t, unsigned int styles,
                                        unsigned int char_len);
unsigned int     ewl_text_styles_get(Ewl_Text *t, unsigned int char_idx);

void             ewl_text_style_add(Ewl_Text *t, Ewl_Text_Style sytle,
                                        unsigned int char_len);
void             ewl_text_style_del(Ewl_Text *t, Ewl_Text_Style style,
                                        unsigned int char_len);
void             ewl_text_style_invert(Ewl_Text *t, Ewl_Text_Style style,
                                        unsigned int char_len);
unsigned int     ewl_text_style_has(Ewl_Text *t, Ewl_Text_Style style,
                                        unsigned int char_idx);

void             ewl_text_wrap_set(Ewl_Text *t, Ewl_Text_Wrap wrap);
void             ewl_text_wrap_apply(Ewl_Text *t, Ewl_Text_Wrap wrap,
                                        unsigned int char_len);
Ewl_Text_Wrap    ewl_text_wrap_get(Ewl_Text *t, unsigned int char_idx);

void             ewl_text_bg_color_set(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a);
void             ewl_text_bg_color_apply(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a,
                                        unsigned int char_len);
void             ewl_text_bg_color_get(Ewl_Text *t,
                                        unsigned int *r, unsigned int *g,
                                        unsigned int *b, unsigned int *a,
                                        unsigned int char_idx);

void             ewl_text_glow_color_set(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a);
void             ewl_text_glow_color_apply(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a,
                                        unsigned int char_len);
void             ewl_text_glow_color_get(Ewl_Text *t,
                                        unsigned int *r, unsigned int *g,
                                        unsigned int *b, unsigned int *a,
                                        unsigned int char_idx);

void             ewl_text_outline_color_set(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a);
void             ewl_text_outline_color_apply(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a,
                                        unsigned int char_len);
void             ewl_text_outline_color_get(Ewl_Text *t,
                                        unsigned int *r, unsigned int *g,
                                        unsigned int *b, unsigned int *a,
                                        unsigned int char_idx);

void             ewl_text_shadow_color_set(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a);
void             ewl_text_shadow_color_apply(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a,
                                        unsigned int char_len);
void             ewl_text_shadow_color_get(Ewl_Text *t,
                                        unsigned int *r, unsigned int *g,
                                        unsigned int *b, unsigned int *a,
                                        unsigned int char_idx);

void             ewl_text_strikethrough_color_set(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a);
void             ewl_text_strikethrough_color_apply(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a,
                                        unsigned int char_len);
void             ewl_text_strikethrough_color_get(Ewl_Text *t,
                                        unsigned int *r, unsigned int *g,
                                        unsigned int *b, unsigned int *a,
                                        unsigned int char_idx);

void             ewl_text_underline_color_set(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a);
void             ewl_text_underline_color_apply(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a,
                                        unsigned int char_len);
void             ewl_text_underline_color_get(Ewl_Text *t,
                                        unsigned int *r, unsigned int *g,
                                        unsigned int *b, unsigned int *a,
                                        unsigned int char_idx);

void             ewl_text_double_underline_color_set(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a);
void             ewl_text_double_underline_color_apply(Ewl_Text *t,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a,
                                        unsigned int char_len);
void             ewl_text_double_underline_color_get(Ewl_Text *t,
                                        unsigned int *r, unsigned int *g,
                                        unsigned int *b, unsigned int *a,
                                        unsigned int char_idx);

char            *ewl_text_text_next_char(const char *text, unsigned int *idx);

/*
 * Internal stuff
 */

void ewl_text_cb_configure(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_reveal(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_obscure(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_show(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_hide(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_destroy(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_mouse_down(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_mouse_up(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_mouse_move(Ewl_Widget *w, void *ev, void *data);
void ewl_text_cb_key_down(Ewl_Widget *w, void *ev, void *data);

void ewl_text_cb_child_add(Ewl_Container *c, Ewl_Widget *w);
void ewl_text_cb_child_remove(Ewl_Container *c, Ewl_Widget *w, int idx);

void ewl_text_trigger_cb_destroy(Ewl_Widget *w, void *ev, void *data);

/**
 * @}
 */

#endif

