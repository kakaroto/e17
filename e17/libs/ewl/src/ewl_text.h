
#ifndef __EWL_TEXT_H__
#define __EWL_TEXT_H__

/**
 * @defgroup Ewl_Text Text: A Multi-Line Text Layout Display
 * @brief Defines a class for multi-line text layout and formatting.
 *
 * @{
 */

/**
 * @themekey /text/file
 * @themekey /text/group
 */

/**
 * Provides for layout of text across multiple lines, as well as formatting
 * portions of the text in different ways, and wrapping around obstacles.
 */
typedef struct Ewl_Text Ewl_Text;

/**
 * @def EWL_TEXT(text)
 * Typecasts a pointer to an Ewl_Text pointer.
 */
#define EWL_TEXT(text) ((Ewl_Text *) text)

/**
 * @struct Ewl_Text
 * Inherits from the Ewl_Widget class and extends it to provide for multi-line
 * text layout, obstacle wrapping, and a variety of formatting.
 */
struct Ewl_Text
{
    Ewl_Entry  entry;   /**< Inherit from Ewl_Entry */
};

Ewl_Widget  *ewl_text_new(char *text);
void         ewl_text_init(Ewl_Text * ta, char *text);

void         ewl_text_text_set(Ewl_Text * ta, char *text);
void         ewl_text_text_prepend(Ewl_Text * ta, char *text);
void         ewl_text_text_append(Ewl_Text * ta, char *text);
void         ewl_text_text_insert(Ewl_Text * ta, char *text, int index);
char        *ewl_text_text_get(Ewl_Text * ta);

int          ewl_text_length_get(Ewl_Text *ta);

void         ewl_text_font_set(Ewl_Text *ta, char *font, int size);
char        *ewl_text_font_get(Ewl_Text *ta);
int          ewl_text_font_size_get(Ewl_Text *ta);

void         ewl_text_style_set(Ewl_Text *ta, char *style);
char        *ewl_text_style_get(Ewl_Text *ta);

void         ewl_text_color_set(Ewl_Text *ta, int r, int g, int b, int a);
void         ewl_text_color_get(Ewl_Text *ta, int *r, int *g, int *b, int *a);

void         ewl_text_align_set(Ewl_Text *ta, unsigned int align);
unsigned int ewl_text_align_get(Ewl_Text *ta);

void         ewl_text_index_select(Ewl_Text *ta, int si, int ei);
void         ewl_text_coord_select(Ewl_Text *ta, int sx, int sy, int ex,
				   int ey);

void         ewl_text_index_geometry_map(Ewl_Text *ta, int index, int *xx,
					 int *yy, int *ww, int *hh);
int          ewl_text_coord_index_map(Ewl_Text *ta, int x, int y);
int          ewl_text_coord_geometry_map(Ewl_Text *ta, int x, int y, int *xx,
					 int *yy, int *ww, int *hh);

/**
 * @}
 */

#endif				/* __EWL_TEXT_H__ */
