
#include <Ewl.h>

Ewd_List *ewl_window_list = NULL;

static void __ewl_window_init(Ewl_Window * w);
static void __ewl_window_realize(Ewl_Widget * w, void *ev_data,
				 void *user_data);
static void __ewl_window_show(Ewl_Widget * w, void *ev_data, void *user_data);
static void __ewl_window_hide(Ewl_Widget * w, void *ev_data, void *user_data);
static void __ewl_window_destroy(Ewl_Widget * w, void *ev_data,
				 void *user_data);
static void __ewl_window_configure(Ewl_Widget * w, void *ev_data,
				   void *user_data);

/**
 * ewl_window_new - allocate and initialize a new window
 *
 * Returns a new window on success, or NULL on failure.
 */
Ewl_Widget *
ewl_window_new()
{
	Ewl_Window *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Window, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	memset(w, 0, sizeof(Ewl_Window));
	__ewl_window_init(w);

	DRETURN_PTR(EWL_WIDGET(w), DLEVEL_STABLE);
}

static void
__ewl_window_init(Ewl_Window * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	/*
	 * Initialize the fields of the inherited container class
	 */
	ewl_box_init(EWL_BOX(w), EWL_ORIENTATION_VERTICAL);
	ewl_widget_set_appearance(EWL_WIDGET(w),
				  "/appearance/window/default");
	ewl_object_set_current_size(EWL_OBJECT(w), 256, 256);
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

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_window_realize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Window *window;
	char *font_path;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	window = EWL_WINDOW(w);

	window->window = ecore_window_new(0, CURRENT_X(w), CURRENT_Y(w),
					  CURRENT_W(w), CURRENT_H(w));
	ecore_window_set_events(window->window, XEV_CONFIGURE);
	ecore_window_set_name_class(window->window, "EWL", "EWL!");
	ecore_window_set_min_size(window->window, MINIMUM_W(w), MINIMUM_H(w));
	ecore_window_set_max_size(window->window, MAXIMUM_W(w), MAXIMUM_H(w));
	ecore_window_set_title(window->window, window->title);

	ecore_window_set_delete_inform(window->window);

	font_path = ewl_theme_font_path();

	w->evas = evas_new_all(ecore_display_get(),
			       window->window, 0, 0,
			       CURRENT_W(w),
			       CURRENT_H(w),
			       ewl_config_get_render_method(),
			       216, 1024 * 1024 * 2,
			       1024 * 1024 * 5, font_path);
	FREE(font_path);

	w->evas_window = evas_get_window(w->evas);
	ecore_window_set_events(w->evas_window, XEV_KEY | XEV_BUTTON |
				XEV_IN_OUT | XEV_EXPOSE | XEV_VISIBILITY |
				XEV_MOUSE_MOVE);

	window->bg_rect = evas_add_rectangle(w->evas);
	evas_set_color(w->evas, window->bg_rect, 0, 0, 0, 255);
	evas_set_layer(w->evas, window->bg_rect, LAYER(w) - 1000);
	evas_show(w->evas, window->bg_rect);

	if (window->borderless)
		ecore_window_hint_set_borderless(window->window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_window_show(Ewl_Widget * w, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!EWL_WINDOW(w)->window)
		DRETURN(DLEVEL_STABLE);

	if (EWL_WINDOW(w)->borderless)
		ecore_window_hint_set_borderless(EWL_WINDOW(w)->window);

	ecore_window_show(EWL_WINDOW(w)->window);
	ecore_window_show(w->evas_window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_window_hide(Ewl_Widget * widget, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("widget", widget);

	ecore_window_hide(widget->evas_window);
	ecore_window_hide(EWL_WINDOW(widget)->window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_window_destroy(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Window *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	win = EWL_WINDOW(w);

	IF_FREE(win->title);

	ecore_window_hide(w->evas_window);
	ecore_window_hide(win->window);

	ecore_window_destroy(w->evas_window);
	ecore_window_destroy(win->window);

	IF_FREE(win->title);

	if (ewd_list_goto(ewl_window_list, w))
		ewd_list_remove(ewl_window_list);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
__ewl_window_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Window *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	win = EWL_WINDOW(w);
	ewl_object_apply_requested(EWL_OBJECT(w));

	if (win->bg_rect)
	  {
		  evas_move(w->evas, win->bg_rect, 0, 0);
		  evas_resize(w->evas, win->bg_rect, CURRENT_W(w),
			      CURRENT_H(w));
	  }

	ecore_window_resize(w->evas_window, CURRENT_W(w), CURRENT_H(w));
	evas_set_output_size(w->evas, CURRENT_W(w), CURRENT_H(w));
	evas_set_output_viewport(w->evas, 0, 0, CURRENT_W(w), CURRENT_H(w));

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_window_find_window - find an ewl window by it's X window
 * @window: the X window to search for on the list of ewl window's
 *
 * Returns the found ewl window on success, NULL on failure to find the
 * window.
 */
Ewl_Window *
ewl_window_find_window(Window window)
{
	Ewl_Window *retwin;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("window", window, NULL);

	ewd_list_goto_first(ewl_window_list);

	while ((retwin = ewd_list_next(ewl_window_list)) != NULL)
		if (retwin->window == window)
			DRETURN_PTR(retwin, DLEVEL_STABLE);

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * ewl_window_find_window_by_evas_window - find an ewl window by its evas window
 * @window: the evas window to search for on the list of windows
 *
 * Returns the found window on success, NULL on failure to find the window.
 */
Ewl_Window *
ewl_window_find_window_by_evas_window(Window window)
{
	Ewl_Window *retwin;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("window", window, NULL);

	ewd_list_goto_first(ewl_window_list);

	while ((retwin = ewd_list_next(ewl_window_list)) != NULL)
	  {
		  if (EWL_WIDGET(retwin)->evas_window == window)
			  return retwin;
	  }

	DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * ewl_window_resize - resize the specified window to the specified size
 * @widget: the window to resize
 * @w: the new width for the window
 * @h: the new height for the window
 *
 * Returns no value. Resize the specified window the the specified size,
 * configure is called to have the display updated.
 */
void
ewl_window_resize(Ewl_Widget * widget, int w, int h)
{
	Ewl_Window *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("widget", widget);

	win = EWL_WINDOW(widget);

	ewl_object_set_current_size(EWL_OBJECT(widget), w, h);

	if (!win->window)
		DRETURN(DLEVEL_STABLE);

	ecore_window_resize(win->window, w, h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_window_set_min_size - set the minimum size a window can attain
 * @widget: the window to change the minimum size
 * @w: the minimum width the window can attain
 * @h: the minimum height the window can attain
 *
 * Returns no value. Sets the minimum size the window can attain.
 */
void
ewl_window_set_min_size(Ewl_Widget * widget, int w, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("widget", widget);

	EWL_OBJECT(widget)->minimum.w = w;
	EWL_OBJECT(widget)->minimum.h = h;

	if (!REALIZED(widget))
		return;

	ecore_window_set_min_size(EWL_WINDOW(widget)->window, w, h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


/**
 * ewl_window_set_max_size - set the maximum size a window can attain
 * @widget: the window to change the maximum size
 * @w: the maximum width the window can attain
 * @h: the maximum height the window can attain
 *
 * Returns no value. Sets the maximum size the window can attain.
 */
void
ewl_window_set_max_size(Ewl_Widget * widget, int w, int h)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("widget", widget);

	EWL_OBJECT(widget)->maximum.w = w;
	EWL_OBJECT(widget)->maximum.h = h;

	if (!REALIZED(widget))
		return;

	ecore_window_set_max_size(EWL_WINDOW(widget)->window, w, h);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_window_set_title - set the title of the specified window
 * @w: the window to change the title
 * @title: the title to set for the window
 *
 * Returns no value. Sets the title of window @w to @title and calls the
 * necessary X lib functions to update the window.
 */
void
ewl_window_set_title(Ewl_Widget * w, char *title)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (strcmp(EWL_WINDOW(w)->title, title))
	  {
		  IF_FREE(EWL_WINDOW(w)->title);
		  EWL_WINDOW(w)->title = strdup(title);
	  }

	if (!REALIZED(w))
		return;

	ecore_window_set_title(EWL_WINDOW(w)->window, title);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_window_get_title - retrieve the title of the specified window
 * @widget: the window to retrieve the window
 *
 * Returns a pointer to a newly allocated copy of the title, NULL on failure.
 * The returned title should be freed.
 */
char *
ewl_window_get_title(Ewl_Widget * widget)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("widget", widget, NULL);

	DRETURN_PTR(strdup(EWL_WINDOW(widget)->title), DLEVEL_STABLE);
}

/**
 * ewl_window_set_borderless - remove the border from the specified window
 * @w: the window to remove the border
 *
 * Returns no value. Remove the border from the specified widget and call the
 * necessary X lib functions to update the appearance.
 */
void
ewl_window_set_borderless(Ewl_Widget * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	EWL_WINDOW(w)->borderless = 1;

	if (REALIZED(w))
		ecore_window_hint_set_borderless(EWL_WINDOW(w)->window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * ewl_window_move - move the specified window to the given position
 * @w: the window to move
 * @x: the x coordinate of the new position
 * @y: the y coordinate of the new position
 *
 * Returns no value. Moves the window into the specified position in the
 * window manager environment.
 */
void
ewl_window_move(Ewl_Widget * w, int x, int y)
{
	Ewl_Window *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	win = EWL_WINDOW(w);

	REQUEST_X(w) = x;
	REQUEST_Y(w) = y;

	CURRENT_X(w) = x;
	CURRENT_Y(w) = y;

	if (REALIZED(w))
		ecore_window_move(win->window, x, y);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
