
#ifndef __EWL_WINDOW_H__
#define __EWL_WINDOW_H__

/*
 * We want to support a variety of window types, including normal windows,
 * dialog boxes, and MacOS X style drop down dialogs.
 */
typedef enum _ewl_window_type Ewl_Window_Type;

enum _ewl_window_type
{
	EWL_WINDOW_NORMAL,
	EWL_WINDOW_DIALOG,
	EWL_WINDOW_DROPDOWN
};

/*
 * The window structure is mostly a container for holding widgets and a
 * wrapper around the xlib window.
 */
typedef struct _ewl_window Ewl_Window;

struct _ewl_window
{
	Ewl_Container widget;
	Window window;
	char *title;
};

#define EWL_WINDOW(widget) ((Ewl_Window *) widget)

Ewl_Widget *ewl_window_new();
Ewl_Window *ewl_window_find_window(Window window);
Ewl_Window *ewl_window_find_window_by_evas_window(Window window);
void ewl_window_resize(Ewl_Widget * widget, int w, int h);
void ewl_window_set_min_size(Ewl_Widget * widget, int w, int h);
void ewl_window_set_max_size(Ewl_Widget * widget, int w, int h);
void ewl_window_set_title(Ewl_Widget * widget, char *title);
char *ewl_window_get_title(Ewl_Widget * widget);

#endif
