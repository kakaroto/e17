#ifndef EWL_CURSOR_H
#define EWL_CURSOR_H

/**
 * @addtogroup Ewl_Cursor Ewl_Cursor: Container to create custom mouse cursors
 * @brief Defines the Ewl_Cursor class used for generating ARGB cursor data.
 *
 * @{
 */

/**
 * @themekey /cursor/file
 * @themekey /cursor/group
 */

/**
 * @def EWL_CURSOR_TYPE
 * The type name for the Ewl_Cursor widget
 */
#define EWL_CURSOR_TYPE "cursor"

/**
 * Ewl_Cursor provides a container that has a labeled cursor.
 */
typedef struct Ewl_Cursor Ewl_Cursor;

/**
 * @def EWL_CURSOR(cursor)
 * Typecast a pointer to an Ewl_Separator pointer.
 */
#define EWL_CURSOR(cursor) ((Ewl_Cursor *) cursor)

/**
 * @brief Inherits from Ewl_Window to allow packing widgets as cursor objects.
 */
struct Ewl_Cursor
{
	Ewl_Window window;       /**< Inherit from Ewl_Window */
	int refcount;     /**< Number of references to cursor */
	int handle;       /**< Engine id for generated cursor */
};

Ewl_Widget     *ewl_cursor_new(void);
int             ewl_cursor_init(Ewl_Cursor *c);

/**
 * @}
 */

#endif

