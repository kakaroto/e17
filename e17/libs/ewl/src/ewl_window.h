
#ifndef __EWL_WINDOW_H__
#define __EWL_WINDOW_H__

/**
 * @defgroup Ewl_Window A Container for Displaying in a New Window
 * @brief Defines the Ewl_Window class which extends the Ewl_Embed class by
 * creating it's own window and evas.
 *
 * @{
 */

/**
 * The window structure is mostly a container for holding widgets and a
 * wrapper around the xlib window.
 */
typedef struct Ewl_Window Ewl_Window;

/**
 * @def EWL_WINDOW(win)
 * Typecasts a pointer to an Ewl_Window pointer.
 */
#define EWL_WINDOW(win) ((Ewl_Window *) win)

/**
 * @struct Ewl_Window
 * Extends the Ewl_Embed class to create it's own window and evas for drawing,
 * sizing and positioning.
 */
struct Ewl_Window
{
	Ewl_Embed       embed; /**< Inherits from the Ewl_Embed class */

	Window          window; /**< Provides a window for drawing */

	char           *title; /**< The current title on the provided window */

	 
	Ewl_Window_Flags flags; /**< Flags indicating window properties */

	int             x; /**< Screen relative horizontal position of window */
	int             y; /**< Screen relative vertical position of window */
};

Ewl_Widget     *ewl_window_new();
int             ewl_window_init(Ewl_Window * win);
Ewl_Window     *ewl_window_find_window(Window window);
void            ewl_window_set_title(Ewl_Window * win, char *title);
char           *ewl_window_get_title(Ewl_Window * win);
void            ewl_window_set_borderless(Ewl_Window * win);
void            ewl_window_move(Ewl_Window * win, int x, int y);
void            ewl_window_get_position(Ewl_Window * win, int *x, int *y);

/**
 * @}
 */

#endif				/* __EWL_WINDOW_H__ */
