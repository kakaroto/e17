
#ifndef __EWL_WINDOW_H__
#define __EWL_WINDOW_H__

typedef struct _ewl_window Ewl_Window;

struct _ewl_window {
	Ewl_Widget			  widget;
	Ebits_Object		  ebits_object;
	Window				  window;
	char				* title;
};

#define EWL_WINDOW(widget) ((Ewl_Window *) widget)

Ewl_Widget * ewl_window_new();
Ewl_Window * ewl_window_find_window(Window window);
Ewl_Window * ewl_window_find_window_by_evas_window(Window window);
void ewl_window_resize(Ewl_Widget * widget, int w, int h);
void ewl_window_set_min_size(Ewl_Widget * widget, int w, int h);
void ewl_window_set_max_size(Ewl_Widget * widget, int w, int h);
void ewl_window_set_title(Ewl_Widget * widget, char * title);
char * ewl_window_get_title(Ewl_Widget * widget);

#endif
