
#ifndef __EWL_ENTRY_H__
#define __EWL_ENTRY_H__

/**
 * @defgroup Ewl_Entry A Single Line Text Entry Widget
 * @brief Defines the Ewl_Entry class to allow for single line editable text.
 *
 * @{
 */

/**
 * Inherits from the Ewl_Widget and provides single line editable text.
 */
typedef struct Ewl_Entry Ewl_Entry;

/**
 * @def EWL_ENTRY(entry)
 * Typecasts a pointer to an Ewl_Entry pointer.
 */
#define EWL_ENTRY(entry) ((Ewl_Entry *) entry)

/**
 * @struct Ewl_Entry
 * Inherits from the Ewl_Container and extends it to provide text and a cursor
 * for editing the contents of the text.
 */
struct Ewl_Entry
{
	Ewl_Container   container; /**< Inherit from Ewl_Container */

	Ewl_Widget     *text; /**< Provide Ewl_Text for text display */
	Ewl_Widget     *cursor; /**< Provide Ewl_Cursor for cursor display */

	int             offset; /**< Starting position of cursor in text */
	int             editable; /**< Flag to indicate if user can edit text */
};

Ewl_Widget     *ewl_entry_new(char *text);
void            ewl_entry_init(Ewl_Entry * e, char *text);
void            ewl_entry_set_text(Ewl_Entry * e, char *t);
char           *ewl_entry_get_text(Ewl_Entry * e);
void            ewl_entry_set_editable(Ewl_Entry *e, unsigned int edit);

/**
 * @}
 */

#endif				/* __EWL_ENTRY_H__ */
