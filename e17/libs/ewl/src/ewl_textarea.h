
#ifndef __EWL_TEXTAREA_H__
#define __EWL_TEXTAREA_H__

/**
 * @file ewl_textarea.h
 * Defines a class for multi-line text layout and formatting.
 */

/**
 * Provides for layout of text across multiple lines, as well as formatting
 * portions of the text in different ways, and wrapping around obstacles.
 */
typedef struct Ewl_TextArea Ewl_TextArea;

/**
 * @def EWL_TEXTAREA(textarea)
 * Typecasts a pointer to an Ewl_TextArea pointer.
 */
#define EWL_TEXTAREA(textarea) ((Ewl_TextArea *) textarea)

/**
 * @struct Ewl_TextArea
 * Inherits from the Ewl_Widget class and extends it to provide for multi-line
 * text layout, obstacle wrapping, and a variety of formatting.
 */
struct Ewl_TextArea
{
	Ewl_Widget      widget; /**< Inherit from Ewl_Widget */
	char           *text; /**< The initial text in the textarea */
	Evas_Object    *etox; /**< The Etox does the actual layout work */
	Etox_Context   *etox_context; /**< Contains various format settings */
};

Ewl_Widget     *ewl_textarea_new(char *text);
void            ewl_textarea_init(Ewl_TextArea * ta, char *text);

void            ewl_textarea_set_text(Ewl_TextArea * ta, char *text);
char           *ewl_textarea_get_text(Ewl_TextArea * ta);

Evas_Object    *ewl_textarea_get_etox(Ewl_TextArea * ta);
void            ewl_textarea_set_context(Ewl_TextArea * ta,
					 Etox_Context * context);
Etox_Context   *ewl_textarea_get_context(Ewl_TextArea * ta);

#endif				/* __EWL_TEXTAREA_H__ */
