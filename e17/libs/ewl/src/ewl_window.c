
#include <Ewl.h>

Ewd_List       *ewl_window_list = NULL;

void            __ewl_window_realize(Ewl_Widget * w, void *ev_data,
				     void *user_data);
void            __ewl_window_unrealize(Ewl_Widget * w, void *ev_data,
				     void *user_data);
void            __ewl_window_show(Ewl_Widget * w, void *ev_data,
				  void *user_data);
void            __ewl_window_hide(Ewl_Widget * w, void *ev_data,
				  void *user_data);
void            __ewl_window_destroy(Ewl_Widget * w, void *ev_data,
				     void *user_data);
void            __ewl_window_configure(Ewl_Widget * w, void *ev_data,
				       void *user_data);
void            __ewl_window_child_add(Ewl_Container * win, Ewl_Widget * child);
void            __ewl_window_child_resize(Ewl_Container *c, Ewl_Widget *w,
					  int size, Ewl_Orientation o);

/**
 * ewl_window_new - allocate and initialize a new window
 *
 * Returns a new window on success, or NULL on failure.
 */
Ewl_Widget     *ewl_window_new()
{
	Ewl_Window     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Window, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	ZERO(w, Ewl_Window, 1);
	if (!ewl_window_init(w)) {
		ewl_widget_destroy(EWL_WIDGET(w));
		w = NULL;
	}

	DRETURN_PTR(EWL_WIDGET(w), DLEVEL_STABLE);
}

/**
 * ewl_window_find_window - find an ewl window by it's X window
 * @window: the X window to search for on the list of ewl window's
 *
 * Returns the found ewl window on success, NULL on failure to find the
 * window.
 */
Ewl_Window     *ewl_window_find_window(Window window)
{
	Ewl_Window     *retwin;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("window", window, NULL);

	ewd_list_goto_first(ewl_window_list);

	while ((retwin = ewd_list_next(ewl_window_list)))
		if (retwin->window == window)
			DRETURN_PTR(retwin, DLEVEL_STABLE);

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * ewl_window_font_path_add - add a font path to all windows after realized
 * @path: the font path to add to the windows
 *
 * Returns no value. Adds the search path @path to the evases created in the
 * windows. Using ewl_theme_font_path_add is preferred.
 */
void ewl_window_font_path_add(char *path)
{
	Ewl_Window *win;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR("path", path);

	ewd_list_goto_first(ewl_window_list);
	while ((win = ewd_list_next(ewl_window_list)))
		if (REALIZED(win))
			evas_font_path_append(win->evas, path);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_window_find_window_by_evas_window - find an ewl window by its evas window
 * @window: the evas window to search for on the list of windows
 *
 * Returns the found window on success, NULL on failure to find the window.
 */
Ewl_Window     *ewl_window_find_window_by_evas_window(Window window)
{
	Ewl_Window     *retwin;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("window", window, NULL);

	ewd_list_goto_first(ewl_window_list);

	while ((retwin = ewd_list_next(ewl_window_list)) != NULL) {
		if (retwin->evas_window == window)
			return retwin;
	}

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * ewl_window_find_window_by_widget - find an ewl window by a widget inside
 * @w: the widget to search for its window
 *
 * Returns the found window on success, NULL on failure to find the window.
 */
Ewl_Window     *ewl_window_find_window_by_widget(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, NULL);

	while (w->parent)
		w = w->parent;

	DRETURN_PTR(EWL_WINDOW(w), DLEVEL_STABLE);
}

/**
 * ewl_window_set_title - set the title of the specified window
 * @win: the window to change the title
 * @title: the title to set for the window
 *
 * Returns no value. Sets the title of window @w to @title and calls the
 * necessary X lib functions to update the window.
 */
void ewl_window_set_title(Ewl_Window * win, char *title)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);

	if (strcmp(win->title, title)) {
		IF_FREE(win->title);
		win->title = strdup(title);
	}

	if (!REALIZED(win))
		return;

	ecore_x_window_prop_title_set(win->window, title);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_window_get_title - retrieve the title of the specified window
 * @win: the window to retrieve the window
 *
 * Returns a pointer to a newly allocated copy of the title, NULL on failure.
 * The returned title should be freed.
 */
char           *ewl_window_get_title(Ewl_Window * win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, NULL);

	DRETURN_PTR(strdup(win->title), DLEVEL_STABLE);
}

/**
 * ewl_window_set_borderless - remove the border from the specified window
 * @win: the window to remove the border
 * @value: the boolean to indicate borderless settings
 *
 * Returns no value. Remove the border from the specified widget and call the
 * necessary X lib functions to update the appearance.
 */
void ewl_window_set_borderless(Ewl_Window * win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);

	win->flags |= EWL_WINDOW_BORDERLESS;

	if (REALIZED(win))
		ecore_x_window_prop_borderless_set(win->window, TRUE);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_window_move - move the specified window to the given position
 * @win: the window to move
 * @x: the x coordinate of the new position
 * @y: the y coordinate of the new position
 *
 * Returns no value. Moves the window into the specified position in the
 * window manager environment.
 */
void ewl_window_move(Ewl_Window * win, int x, int y)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);

	if (REALIZED(win))
		ecore_x_window_move(win->window, x, y);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_window_get_position - retrieve the position of the window
 * @x: a pointer to the integer that should receive the x coordinate
 * @y: a pointer to the integer that should receive the y coordinate
 *
 * Returns no value. Stores the window position into the parameters @x and @y.
 */
void ewl_window_get_position(Ewl_Window * win, int *x, int *y)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);

	if (REALIZED(win)) {
		*x = win->x;
		*y = win->y;
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_window_get_child_at - find the child at given coordinates
 * @win: the window to search for a child
 * @x: the x coordinate to look for an intersecting child
 * @y: the y coordinate to look for an intersecting child
 *
 * Returns the child found at the given coordinates @x, @y in the window @win.
 */
Ewl_Widget     *ewl_window_get_child_at(Ewl_Window * win, int x, int y)
{
	Ewl_Widget     *widget = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);

	DCHECK_PARAM_PTR_RET("win", win, NULL);

	/*
	 * Do a recursive search in the window for a child at the given
	 * coordinates.
	 */
	widget = ewl_container_get_child_at_recursive(EWL_CONTAINER(win), x, y);

	DRETURN_PTR(widget, DLEVEL_STABLE);
}

/**
 * ewl_window_init - initialize a window to default values and callbacks
 * @w: the window to be initialized to default values and callbacks
 *
 * Returns TRUE or FALSE depending on if initialization succeeds. Sets the
 * values and callbacks of a window @w to their defaults.
 */
int ewl_window_init(Ewl_Window * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);

	/*
	 * Initialize the fields of the inherited container class
	 */
	ewl_container_init(EWL_CONTAINER(w), "window",
			   __ewl_window_child_add, __ewl_window_child_resize,
			   NULL);

	w->title = strdup("EWL!");

	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_REALIZE,
			     __ewl_window_realize, NULL);
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_UNREALIZE,
			     __ewl_window_unrealize, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_SHOW,
			    __ewl_window_show, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_HIDE,
			    __ewl_window_hide, NULL);
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_DESTROY,
			     __ewl_window_destroy, NULL);
	/*
	 * Override the default configure callbacks since the window
	 * has special needs for placement.
	 */
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_CONFIGURE,
			     __ewl_window_configure, NULL);

	LAYER(w) = -1000;

	ewd_list_append(ewl_window_list, w);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void __ewl_window_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Object     *o;
	Ewl_Window     *window;
	char           *font_path;
	Ewd_List       *paths;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	window = EWL_WINDOW(w);
	o = EWL_OBJECT(w);

	/*
	 * FIXME: This needs to be explored a bit, the size should come down
	 * from a configure event, but neeed to double check.
	 */
	ewl_object_request_size(EWL_OBJECT(w), ewl_object_get_current_w(o),
			ewl_object_get_current_h(o));

	window->window = ecore_x_window_new(0, window->x, window->y,
			ewl_object_get_current_w(o),
			ewl_object_get_current_h(o));

	ecore_x_window_prop_name_class_set(window->window, "EWL", "EWL!");
	ecore_x_window_prop_title_set(window->window, window->title);
	ecore_x_window_prop_delete_request_set(window->window, 1);

	window->evas = evas_new();
	evas_output_method_set(window->evas,
			evas_render_method_lookup("software_x11"));
	evas_output_size_set(window->evas, ewl_object_get_current_w(o),
			ewl_object_get_current_h(o));
	evas_output_viewport_set(window->evas, ewl_object_get_current_x(o),
			ewl_object_get_current_y(o),
			ewl_object_get_current_w(o),
			ewl_object_get_current_h(o));

	{
		Evas_Engine_Info_Software_X11 *info;

		info = (Evas_Engine_Info_Software_X11 *)
			evas_engine_info_get(window->evas);

		info->info.display = ecore_x_display_get();
		info->info.visual = DefaultVisual(info->info.display,
				DefaultScreen(info->info.display));
		info->info.colormap = DefaultColormap(info->info.display,
				DefaultScreen(info->info.display));
		info->info.drawable = window->window;
		info->info.depth = DefaultDepth(info->info.display,
				DefaultScreen(info->info.display));
		info->info.rotation = 0;
		info->info.debug = 0;
		evas_engine_info_set(window->evas, (Evas_Engine_Info *)info);
	}

	paths = ewl_theme_font_path_get();
	ewd_list_goto_first(paths);
	while ((font_path = ewd_list_next(paths)))
		evas_font_path_append(window->evas, font_path);

	window->evas_window = window->window;

	if (window->flags & EWL_WINDOW_BORDERLESS)
		ecore_x_window_prop_borderless_set(window->window, 1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_window_unrealize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Object     *o;
	Ewl_Window     *window;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	window = EWL_WINDOW(w);
	o = EWL_OBJECT(w);

	evas_free(window->evas);
	window->evas = NULL;

	ecore_x_window_del(window->window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_window_show(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!EWL_WINDOW(w)->window)
		DRETURN(DLEVEL_STABLE);

	if (EWL_WINDOW(w)->flags & EWL_WINDOW_BORDERLESS)
		ecore_x_window_prop_borderless_set(EWL_WINDOW(w)->window, 1);

	ecore_x_window_show(EWL_WINDOW(w)->window);
	ecore_x_window_show(EWL_WINDOW(w)->evas_window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_window_hide(Ewl_Widget * widget, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("widget", widget);

	ecore_x_window_hide(EWL_WINDOW(widget)->evas_window);
	ecore_x_window_hide(EWL_WINDOW(widget)->window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_window_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Window     *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	win = EWL_WINDOW(w);

	IF_FREE(win->title);

	ecore_x_window_hide(win->evas_window);
	ecore_x_window_hide(win->window);

	ecore_x_window_del(win->evas_window);
	ecore_x_window_del(win->window);

	IF_FREE(win->title);

	if (ewd_list_goto(ewl_window_list, w))
		ewd_list_remove(ewl_window_list);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_window_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Window     *win;
	Ewl_Object     *child;
	unsigned int    width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	win = EWL_WINDOW(w);

	if (!win->window)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Adjust the maximum window bounds to match the widget
	 */
	ecore_x_window_prop_min_size_set(win->window,
			ewl_object_get_minimum_w(EWL_OBJECT(w)),
		       	ewl_object_get_minimum_h(EWL_OBJECT(w)));
	ecore_x_window_prop_max_size_set(win->window,
			ewl_object_get_maximum_w(EWL_OBJECT(w)),
			ewl_object_get_maximum_h(EWL_OBJECT(w)));

	/*
	 * Find out how much space the widget accepted.
	 */
	width = ewl_object_get_current_w(EWL_OBJECT(w));
	height = ewl_object_get_current_h(EWL_OBJECT(w));

	/*
	 * Now give the windows the appropriate size and adjust the evas as
	 * well.
	 */
	ecore_x_window_resize(win->window, width, height);
	ecore_x_window_resize(win->evas_window, width, height);
	evas_output_size_set(win->evas, width, height);
	evas_output_viewport_set(win->evas,
			ewl_object_get_current_x(EWL_OBJECT(w)),
			ewl_object_get_current_y(EWL_OBJECT(w)),
			width, height);

	/*
	 * Configure each of the child widgets.
	 */
	ewd_list_goto_first(EWL_CONTAINER(w)->children);
	while ((child = ewd_list_next(EWL_CONTAINER(w)->children))) {
		/*
		 * Try to give the child the full size of the window from it's
		 * base position. The object will constrict it based on the
		 * fill policy. Don't add the TOP and LEFT insets since
		 * they've already been accounted for.
		 */
		ewl_object_request_size(child,
					CURRENT_W(w) -
					ewl_object_get_current_x(child),
					CURRENT_H(w) -
					ewl_object_get_current_y(child));
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_window_child_add(Ewl_Container * win, Ewl_Widget * child)
{
	int size;

	LAYER(child) += 100;

	size = ewl_object_get_current_x(EWL_OBJECT(child)) +
		ewl_object_get_current_w(EWL_OBJECT(child)) - CURRENT_X(win);
	if (size > PREFERRED_W(win))
		ewl_object_set_preferred_w(EWL_OBJECT(win), size);

	size = ewl_object_get_current_y(EWL_OBJECT(child)) +
		ewl_object_get_current_h(EWL_OBJECT(child)) - CURRENT_Y(win);
	if (size > PREFERRED_H(win))
		ewl_object_set_preferred_h(EWL_OBJECT(win), size);
}

void __ewl_window_child_resize(Ewl_Container *c, Ewl_Widget *w,
		int size, Ewl_Orientation o)
{
	int            maxw = 0, maxh = 0;
	Ewl_Window    *win;
	Ewl_Object    *child;

	DENTER_FUNCTION(DLEVEL_STABLE);

	child = EWL_OBJECT(w);
	win = EWL_WINDOW(c);

	ewd_list_goto_first(EWL_CONTAINER(win)->children);
	while ((child = ewd_list_next(EWL_CONTAINER(win)->children))) {
		int             cs;

		/*
		 * Adjust children for insets
		 */
		if (ewl_object_get_current_x(child) < CURRENT_X(win))
			ewl_object_request_x(child, CURRENT_X(win));
		if (ewl_object_get_current_y(child) < CURRENT_Y(win))
			ewl_object_request_y(child, CURRENT_Y(win));

		cs = ewl_object_get_current_x(child) +
			ewl_object_get_minimum_w(child);

		/*
		 * Check the width and x position vs. window width.
		 */
		if (maxw < cs)
			maxw = cs;

		cs = ewl_object_get_current_y(child) +
			ewl_object_get_minimum_h(child);

		/*
		 * Check the height and y position vs. window height.
		 */
		if (maxh < cs)
			maxh = cs;

	}

	ewl_object_set_preferred_size(EWL_OBJECT(win), maxw, maxh);

	ewl_object_request_size(EWL_OBJECT(c),
				ewl_object_get_current_w(EWL_OBJECT(c)),
				ewl_object_get_current_h(EWL_OBJECT(c)));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
