
#ifndef __EWL_WINDOW_H__
#define __EWL_WINDOW_H__

/*
 * The window structure is mostly a container for holding widgets and a
 * wrapper around the xlib window.
 */
typedef struct _ewl_window Ewl_Window;

struct _ewl_window
{
	Ewl_Embed       embed;

	Window          window;

	char           *title;

	/*
	 * Flag to indicate if the window has a border.
	 */
	Ewl_Window_Flags flags;

	int x, y;
};

#define EWL_WINDOW(widget) ((Ewl_Window *) widget)

Ewl_Widget     *ewl_window_new();
int             ewl_window_init(Ewl_Window * win);
Ewl_Window     *ewl_window_find_window(Window window);
void            ewl_window_set_title(Ewl_Window * win, char *title);
char           *ewl_window_get_title(Ewl_Window * win);
void            ewl_window_set_borderless(Ewl_Window * win);
void            ewl_window_move(Ewl_Window * win, int x, int y);
void            ewl_window_get_position(Ewl_Window * win, int *x, int *y);

#endif				/* __EWL_WINDOW_H__ */
