
#include <Ewl.h>

Ewd_List       *ewl_window_list = NULL;

void            __ewl_window_realize(Ewl_Widget * w, void *ev_data,
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

	while ((retwin = ewd_list_next(ewl_window_list)) != NULL)
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
 * ewl_window_resize - resize the specified window to the specified size
 * @win: the window to resize
 * @w: the new width for the window
 * @h: the new height for the window
 *
 * Returns no value. Resize the specified window the the specified size,
 * configure is called to have the display updated.
 */
void ewl_window_resize(Ewl_Window * win, int w, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);

	ewl_object_request_size(EWL_OBJECT(win), w, h);

	if (!win->window)
		DRETURN(DLEVEL_STABLE);

	ecore_window_resize(win->window,
			ewl_object_get_current_w(EWL_OBJECT(win)),
			ewl_object_get_current_h(EWL_OBJECT(win)));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_window_set_min_size - set the minimum size a window can attain
 * @win: the window to change the minimum size
 * @w: the minimum width the window can attain
 * @h: the minimum height the window can attain
 *
 * Returns no value. Sets the minimum size the window can attain.
 */
void ewl_window_set_min_size(Ewl_Window * win, int w, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);

	EWL_OBJECT(win)->minimum.w = w;
	EWL_OBJECT(win)->minimum.h = h;

	if (!REALIZED(win))
		return;

	ecore_window_set_min_size(win->window, w, h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_window_set_max_size - set the maximum size a window can attain
 * @win: the window to change the maximum size
 * @w: the maximum width the window can attain
 * @h: the maximum height the window can attain
 *
 * Returns no value. Sets the maximum size the window can attain.
 */
void ewl_window_set_max_size(Ewl_Window * win, int w, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);

	EWL_OBJECT(win)->maximum.w = w;
	EWL_OBJECT(win)->maximum.h = h;

	if (!REALIZED(win))
		return;

	ecore_window_set_max_size(win->window, w, h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_window_get_geometry - retrieve the size and position of a window
 * @win: the window to retrieve the size
 * @x: the pointer to the integer to store the x position
 * @y: the pointer to the integer to store the y position
 * @w: the pointer to the integer to store the width
 * @h: the pointer to the integer to store the height
 *
 * Returns no value. Stores the current position and size of the window into
 * @x, @y, @w, and @h.
 */
void ewl_window_get_geometry(Ewl_Window * win, int *x, int *y, int *w, int *h)
{
	DCHECK_PARAM_PTR("win", win);

	ecore_window_get_geometry(win->window, x, y, w, h);
	ecore_window_get_root_relative_location(win->window, x, y);
}

/**
 * ewl_window_set_geometry - set the current size and position of a window
 * @win: the window to change geometry
 * @x: the new x position of the window
 * @y: the new y position of the window
 * @w: the new width of the window
 * @h: the new height of the window
 *
 * Returns no value. Changes the current size and position of the window.
 */
void ewl_window_set_geometry(Ewl_Window * win, int x, int y, int w, int h)
{
	DCHECK_PARAM_PTR("win", win);

	ewl_window_resize(win, w, h);
	ewl_window_move(win, x, y);
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

	ecore_window_set_title(win->window, title);

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
		ecore_window_hint_set_borderless(win->window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_window_set_auto_size - set the window to resize to fit contents
 * @win: the window to change auto resize flag
 * @value: the TRUE or FALSE value for the auto resize flag
 *
 * Returns no value.
 */
void ewl_window_set_auto_size(Ewl_Window * win, int value)
{
	DCHECK_PARAM_PTR("win", win);

	if (value)
		win->flags |= EWL_WINDOW_AUTO_SIZE;
	else
		win->flags &= (~EWL_WINDOW_AUTO_SIZE);
	ewl_widget_configure(EWL_WIDGET(win));
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

	/*
	CURRENT_X(win) = x;
	CURRENT_Y(win) = y;
	*/

	if (REALIZED(win))
		ecore_window_move(win->window, x, y);

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
	ewl_container_init(EWL_CONTAINER(w), "/window",
			   __ewl_window_child_add, __ewl_window_child_resize,
			   NULL);
	ewl_object_request_size(EWL_OBJECT(w), 256, 256);

	w->title = strdup("EWL!");

	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_REALIZE,
			     __ewl_window_realize, NULL);
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

	window->window = ecore_window_new(0, ewl_object_get_current_x(o),
			ewl_object_get_current_y(o),
			ewl_object_get_current_w(o),
			ewl_object_get_current_h(o));
	ecore_window_set_events(window->window, XEV_CONFIGURE);
	ecore_window_set_name_class(window->window, "EWL", "EWL!");
	ecore_window_set_min_size(window->window,
			ewl_object_get_minimum_w(o),
		       	ewl_object_get_minimum_h(o));
	ecore_window_set_max_size(window->window, ewl_object_get_maximum_w(o),
			ewl_object_get_maximum_h(o));
	ecore_window_set_title(window->window, window->title);

	ecore_window_set_delete_inform(window->window);

	window->evas = evas_new();
	evas_output_method_set(window->evas,
			evas_render_method_lookup("software_x11"));
	evas_output_size_set(window->evas, ewl_object_get_current_w(o),
			ewl_object_get_current_h(o));
	evas_output_viewport_set(window->evas, 0, 0,
			ewl_object_get_current_w(o),
			ewl_object_get_current_h(o));

	{
		Evas_Engine_Info_Software_X11 *info;

		info = (Evas_Engine_Info_Software_X11 *)
			evas_engine_info_get(window->evas);

		info->info.display = ecore_display_get();
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
	ecore_window_set_events(window->evas_window, XEV_KEY | XEV_BUTTON |
				XEV_IN_OUT | XEV_EXPOSE | XEV_VISIBILITY |
				XEV_MOUSE_MOVE | XEV_FOCUS);

	/*
	window->bg_rect = evas_add_rectangle(window->evas);
	evas_set_color(window->evas, window->bg_rect, 0, 0, 0, 255);
	evas_set_layer(window->evas, window->bg_rect, LAYER(w) - 1000);
	evas_show(window->evas, window->bg_rect);
	*/

	if (window->flags & EWL_WINDOW_BORDERLESS)
		ecore_window_hint_set_borderless(window->window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_window_show(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!EWL_WINDOW(w)->window)
		DRETURN(DLEVEL_STABLE);

	if (EWL_WINDOW(w)->flags & EWL_WINDOW_BORDERLESS)
		ecore_window_hint_set_borderless(EWL_WINDOW(w)->window);

	ecore_window_show(EWL_WINDOW(w)->window);
	ecore_window_show(EWL_WINDOW(w)->evas_window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_window_hide(Ewl_Widget * widget, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("widget", widget);

	ecore_window_hide(EWL_WINDOW(widget)->evas_window);
	ecore_window_hide(EWL_WINDOW(widget)->window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_window_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Window     *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	win = EWL_WINDOW(w);

	IF_FREE(win->title);

	ecore_window_hide(win->evas_window);
	ecore_window_hide(win->window);

	ecore_window_destroy(win->evas_window);
	ecore_window_destroy(win->window);

	IF_FREE(win->title);

	if (ewd_list_goto(ewl_window_list, w))
		ewd_list_remove(ewl_window_list);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_window_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Window     *win;
	Ewl_Object     *child;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	win = EWL_WINDOW(w);

	if (!win->window)
		DRETURN(DLEVEL_STABLE);

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
					CURRENT_W(w) - (CURRENT_X(child)),
					CURRENT_H(w) - (CURRENT_Y(child)));
	}

	ecore_window_resize(win->window,
			ewl_object_get_current_w(EWL_OBJECT(w)),
			ewl_object_get_current_h(EWL_OBJECT(w)));
	ecore_window_resize(win->evas_window,
			ewl_object_get_current_w(EWL_OBJECT(w)),
			ewl_object_get_current_h(EWL_OBJECT(w)));
	evas_output_size_set(win->evas,
			ewl_object_get_current_w(EWL_OBJECT(w)),
			ewl_object_get_current_h(EWL_OBJECT(w)));
	evas_output_viewport_set(win->evas,
			ewl_object_get_current_x(EWL_OBJECT(w)),
			ewl_object_get_current_y(EWL_OBJECT(w)),
			ewl_object_get_current_w(EWL_OBJECT(w)),
			ewl_object_get_current_h(EWL_OBJECT(w)));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_window_child_add(Ewl_Container * win, Ewl_Widget * child)
{
	LAYER(child) += 100;
}

void __ewl_window_child_resize(Ewl_Container *c, Ewl_Widget *w,
		int size, Ewl_Orientation o)
{
	int            maxw = 0, maxh = 0;
	Ewl_Window    *win;
	Ewl_Object    *child;

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
			ewl_object_get_preferred_w(child);

		/*
		 * Check the width and x position vs. window width.
		 */
		if (maxw < cs)
			maxw = cs;

		cs = ewl_object_get_current_y(child) +
			ewl_object_get_preferred_h(child);

		/*
		 * Check the height and y position vs. window height.
		 */
		if (maxh < cs)
			maxh = cs;

	}

	ewl_object_set_preferred_size(EWL_OBJECT(win), maxw, maxh);

	if (win->flags & EWL_WINDOW_AUTO_SIZE) {
		ewl_object_request_size(EWL_OBJECT(c),
				ewl_object_get_preferred_w(EWL_OBJECT(c)),
				ewl_object_get_preferred_h(EWL_OBJECT(c)));
	}
}
