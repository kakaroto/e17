
#ifndef __EWL_WINDOW_H__
#define __EWL_WINDOW_H__

/*
 * The window structure is mostly a container for holding widgets and a
 * wrapper around the xlib window.
 */
typedef struct _ewl_window Ewl_Window;

struct _ewl_window
{
	Ewl_Box widget;
	Window window;
	Evas_Object bg_rect;
	Ebits_Object *ebits_object;
	char *title;
	int borderless;

	int float_layer;
	Ewd_List *floaters;
};

#define EWL_WINDOW(widget) ((Ewl_Window *) widget)

Ewl_Widget *ewl_window_new();
void ewl_window_init(Ewl_Window * w);
Ewl_Window *ewl_window_find_window(Window window);
Ewl_Window *ewl_window_find_window_by_evas_window(Window window);
void ewl_window_resize(Ewl_Widget * widget, int w, int h);
void ewl_window_set_min_size(Ewl_Widget * widget, int w, int h);
void ewl_window_set_max_size(Ewl_Widget * widget, int w, int h);
void ewl_window_set_title(Ewl_Widget * widget, char *title);
char *ewl_window_get_title(Ewl_Widget * widget);
void ewl_window_get_geometry(Ewl_Window * win, int *x, int *y, int *w,
		int *h);
void ewl_window_set_geometry(Ewl_Widget * widget, int x, int y, int w, int h);
void ewl_window_set_borderless(Ewl_Widget * w);
void ewl_window_move(Ewl_Widget * w, int x, int y);
Ewl_Widget * ewl_window_get_child_at(Ewl_Window *win, int x, int y);
void ewl_window_floater_add(Ewl_Window *w, Ewl_Floater *f);
void ewl_window_floater_remove(Ewl_Window *w, Ewl_Floater *f);

#endif /* __EWL_WINDOW_H__ */
