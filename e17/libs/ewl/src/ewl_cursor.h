#ifndef __EWL_CURSOR_H__
#define __EWL_CURSOR_H__

/**
 * @defgroup Ewl_Cursor Cursor: A Text Cursor
 * Indicates cursor position in editable text and provides a visual selection
 * highlighting.
 *
 * @{
 */

/**
 * @themekey /cursor/file
 * @themekey /cursor/group
 */

/**
 * The cursor indicates where in an Ewl_Entry typed text will be placed.
 */
typedef struct Ewl_Cursor Ewl_Cursor;

/**
 * @def EWL_CURSOR(cursor)
 * Typecasts a pointer to an Ewl_Cursor pointer.
 */
#define EWL_CURSOR(cursor) ((Ewl_Cursor *) cursor)

/**
 * @struct Ewl_Cursor
 * Inherits from Ewl_Widget and extends it to position itself based on indices
 * into an Ewl_Entry.
 */
struct Ewl_Cursor
{
	Ewl_Widget      widget; /**< Inherit from Ewl_Widget */

	struct {
		unsigned int start; /**< Starting index in entry */
		unsigned int end; /**< Ending index in entry */
		unsigned int base; /**< Last index where start == end */
	} position;
};

Ewl_Widget     *ewl_cursor_new(void);
void            ewl_cursor_init(Ewl_Cursor * c);
void            ewl_cursor_position_set(Ewl_Cursor * w, unsigned int start,
					unsigned int end);
void            ewl_cursor_base_set(Ewl_Cursor *c, unsigned int pos);
void            ewl_cursor_select_to(Ewl_Cursor *c, unsigned int pos);
unsigned int    ewl_cursor_base_position_get(Ewl_Cursor * w);
unsigned int    ewl_cursor_start_position_get(Ewl_Cursor * w);
unsigned int    ewl_cursor_end_position_get(Ewl_Cursor * w);

/**
 * @}
 */

#endif				/* __EWL_CURSOR_H__ */
