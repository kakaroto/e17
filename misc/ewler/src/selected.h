#ifndef SELECTED_H
#define SELECTED_H

typedef struct Ewler_Selected Ewler_Selected;

#define EWLER_SELECTED(s) ((Ewler_Selected *) s)

struct Ewler_Selected {
	Ewl_Box box;
	Ewl_Widget *selected;
	char *dragging;
	struct {
		int x, y; /* top left */
		int u, v; /* lower right */
	} corners;
	int index;
};


Ewl_Widget *ewler_selected_new(Ewl_Widget *w);
int ewler_selected_init(Ewler_Selected *s, Ewl_Widget *w);
Ewl_Widget *ewler_selected_get(Ewler_Selected *s);
void ewler_selected_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewler_selected_realize_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewler_selected_selector_realize_cb(Ewl_Widget *w, void *ev_data,
																				void *user_data);
void ewler_selected_deselect_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewler_selected_mouse_down_cb(Ewl_Widget *w, void *ev_data,
																	void *user_data);
void ewler_selected_mouse_move_cb(Ewl_Widget *w, void *ev_data,
																	void *user_data);
void ewler_selected_mouse_up_cb(Ewl_Widget *w, void *ev_data,
																void *user_data);

void ewler_selected_part_down(void *data, Evas_Object *o,
															const char *emission, const char *source);
void ewler_selected_part_up(void *data, Evas_Object *o,
														const char *emission, const char *source);
#endif
