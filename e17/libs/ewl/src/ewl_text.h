
#ifndef __EWL_TEXT_H
#define __EWL_TEXT_H

/**
 * @defgroup Ewl_Text A Single Line Text Display
 * @brief Defines the Ewl_Text class to provide text display in a single line.
 *
 * @{
 */

/**
 * Inherits from the Ewl_Widget and provides single line text display.
 */
typedef struct Ewl_Text Ewl_Text;

/**
 * @def EWL_TEXT(text)
 * Typecast a pointer to an Ewl_Text pointer.
 */
#define EWL_TEXT(text) ((Ewl_Text *) text)

/**
 * @struct Ewl_Text
 * Inherit from the Ewl_Widget and extend to provide single line text display
 * capabilities.
 */
struct Ewl_Text
{
	Ewl_Widget      widget; /**< Inherit from Ewl_Widget */
	Evas_Object    *estyle; /**< Pointer to estyle displaying text */

	char           *text; /**< The text contents of the estyle */
	char           *font; /**< Name of the font used by the estyle */
	char           *style; /**< Name of the stylization on the estyle */
	int             font_size; /**< The size of the font in the estyle */
	int             align; /**< Alignment of the estyle within the widget */
	int             length; /**< Length of the text in the estyle */
	int             r, g, b, a; /**< Text color of the estyle */
	int             overrides; /**< Bitmask of programmer set properties */
};

typedef enum
{
	EWL_TEXT_OVERRIDE_COLOR = 0x1,
	EWL_TEXT_OVERRIDE_FONT = 0x2,
	EWL_TEXT_OVERRIDE_SIZE = 0x4,
	EWL_TEXT_OVERRIDE_STYLE = 0x8,
} EWL_TEXT_OVERRIDES;

Ewl_Widget     *ewl_text_new(char *text);

void            ewl_text_set_text(Ewl_Text * t, char *text);
char           *ewl_text_get_text(Ewl_Text * t);
void            ewl_text_set_font(Ewl_Text * t, char *font);
char           *ewl_text_get_font(Ewl_Text * t);
void            ewl_text_set_font_size(Ewl_Text * t, int size);
int             ewl_text_get_font_size(Ewl_Text * t);
void            ewl_text_set_color(Ewl_Text * t, int r, int g, int b, int a);
void            ewl_text_get_color(Ewl_Text * t, int *r, int *g, int *b,
				   int *a);
void            ewl_text_get_text_geometry(Ewl_Text * t, int *xx, int *yy,
					   int *ww, int *hh);
inline int      ewl_text_get_length(Ewl_Text *t);
void            ewl_text_set_style(Ewl_Text * t, char *s);
void            ewl_text_get_letter_geometry(Ewl_Text * t, int index, int *xx,
					     int *yy, int *ww, int *hh);
int             ewl_text_get_letter_geometry_at(Ewl_Text * t, int x, int y,
						int *tx, int *ty, int *tw,
						int *th);
void            ewl_text_set_alignment(Ewl_Text * t, Ewl_Alignment a);
int             ewl_text_get_index_at(Ewl_Text * t, int x, int y);

/**
 * @}
 */

#endif
