
#ifndef __EWL_WINDOW_H__
#define __EWL_WINDOW_H__

/*
 * The window structure is mostly a container for holding widgets and a
 * wrapper around the xlib window.
 */
typedef struct _ewl_window Ewl_Window;

struct _ewl_window
{
	Ewl_Container   widget;

	Window          window;

	/*
	 * The following fields allow for drawing the widgets
	 */
	Evas           *evas;
	Window          evas_window;

	Ebits_Object   *ebits_object;

	char           *title;

	/*
	 * Flag to indicate if the window has a border.
	 */
	Ewl_Window_Flags flags;
};

#define EWL_WINDOW(widget) ((Ewl_Window *) widget)

Ewl_Widget     *ewl_window_new();
int             ewl_window_init(Ewl_Window * win);
void            ewl_window_font_path_add(char *path);
Ewl_Window     *ewl_window_find_window(Window window);
Ewl_Window     *ewl_window_find_window_by_evas_window(Window window);
Ewl_Window     *ewl_window_find_window_by_widget(Ewl_Widget * w);
void            ewl_window_resize(Ewl_Window * win, int w, int h);
void            ewl_window_set_min_size(Ewl_Window * win, int w, int h);
void            ewl_window_set_max_size(Ewl_Window * win, int w, int h);
void            ewl_window_set_title(Ewl_Window * win, char *title);
char           *ewl_window_get_title(Ewl_Window * win);
void            ewl_window_get_geometry(Ewl_Window * win, int *x, int *y,
					int *w, int *h);
void            ewl_window_set_geometry(Ewl_Window * win, int x, int y,
					int w, int h);
void            ewl_window_set_borderless(Ewl_Window * win);
void            ewl_window_set_auto_size(Ewl_Window * win, int value);
void            ewl_window_move(Ewl_Window * win, int x, int y);
Ewl_Widget     *ewl_window_get_child_at(Ewl_Window * win, int x, int y);

#endif				/* __EWL_WINDOW_H__ */
