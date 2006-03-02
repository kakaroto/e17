#ifndef EWL_WINDOW_H
#define EWL_WINDOW_H

/**
 * @defgroup Ewl_Window Ewl_Window: A Container for Displaying in a New Window
 * Defines the Ewl_Window class which extends the Ewl_Embed class by
 * creating it's own window and evas.
 *
 * @{
 */

/**
 * @themekey /window/file
 * @themekey /window/group
 */

#define EWL_WINDOW_TYPE "window"

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
 * Extends the Ewl_Embed class to create it's own window and evas for drawing,
 * sizing and positioning.
 */
struct Ewl_Window
{
	Ewl_Embed       embed; /**< Inherits from the Ewl_Embed class */

	void           *window; /**< Provides a window for drawing */
	Ewl_Window     *transient; /**< Window to be transient for */

	char           *title; /**< The current title on the provided window */
	char           *name; /**< Current name on the provided window */
	char           *classname; /**< Current class on the provided window */

	 
	Ewl_Window_Flags flags; /**< Flags indicating window properties */

	int             x; /**< Screen relative horizontal position of window */
	int             y; /**< Screen relative vertical position of window */
	char           *render; /**< The render engine in use */

	Ewl_Dnd_Types dnd_types;	/**< The dnd type */
	Ewl_Widget* dnd_last_position;	/**< The last dnd position */
};

Ewl_Widget     *ewl_window_new(void);
int             ewl_window_init(Ewl_Window *win);
Ewl_Window     *ewl_window_window_find(void *window);
void            ewl_window_title_set(Ewl_Window *win, const char *title);
char           *ewl_window_title_get(Ewl_Window *win);
void            ewl_window_name_set(Ewl_Window *win, const char *name);
char           *ewl_window_name_get(Ewl_Window *win);
void            ewl_window_class_set(Ewl_Window *win, const char *classname);
char           *ewl_window_class_get(Ewl_Window *win);
void            ewl_window_borderless_set(Ewl_Window *win);
void            ewl_window_move(Ewl_Window *win, int x, int y);
void            ewl_window_position_get(Ewl_Window *win, int *x, int *y);
void            ewl_window_raise(Ewl_Window *win);
void            ewl_window_lower(Ewl_Window *win);
void            ewl_window_transient_for(Ewl_Window *win, Ewl_Window * forwin);
void            ewl_window_keyboard_grab_set(Ewl_Window *win, int grab);
int             ewl_window_keyboard_grab_get(Ewl_Window *win);
void            ewl_window_pointer_grab_set(Ewl_Window *win, int grab);
int             ewl_window_pointer_grab_get(Ewl_Window *win);
void            ewl_window_override_set(Ewl_Window *win, int override);
int             ewl_window_override_get(Ewl_Window *win);
void		ewl_window_dnd_aware_set(Ewl_Window *win);
void 		ewl_window_selection_text_set(Ewl_Window *win, const char *txt);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_window_realize_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_window_postrealize_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_window_realize_transient_cb(Ewl_Widget *w, void *ev_data,
					        void *user_data);
void ewl_window_unrealize_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_window_show_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_window_expose_cb(Ewl_Widget *w, void *ev, void *user_data);
void ewl_window_hide_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_window_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_window_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif
