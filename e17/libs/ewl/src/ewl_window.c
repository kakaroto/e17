#include <Ewl.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif

#ifdef HAVE_EVAS_ENGINE_GL_X11_H
#include <Evas_Engine_GL_X11.h>
#endif

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
#include <Evas_Engine_Software_X11.h>
#endif

Ewd_List *ewl_window_list = NULL;

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

/**
 * @return Returns a new window on success, or NULL on failure.
 * @brief Allocate and initialize a new window
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
 * @param window: the X window to search for on the list of ewl window's
 * @return Returns the found ewl window on success, NULL on failure.
 * @brief Find an ewl window by it's X window
 */
Ewl_Window     *ewl_window_find_window(Ecore_X_Window window)
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
 * @param win: the window to change the title
 * @param title: the title to set for the window
 * @return Returns no value.
 * @brief Set the title of the specified window
 *
 * Sets the title of window @a w to @a title and calls the necessary X lib
 * functions to update the window.
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
 * @param win: the window to retrieve the window
 * @return Returns a pointer to a new copy of the title, NULL on failure.
 * @brief Retrieve the title of the specified window
 *
 * The returned title should be freed.
 */
char           *ewl_window_get_title(Ewl_Window * win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, NULL);

	DRETURN_PTR(strdup(win->title), DLEVEL_STABLE);
}

/**
 * @param win: the window to remove the border
 * @return Returns no value.
 * @brief Remove the border from the specified window
 *
 * Remove the border from the specified widget and call the
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
 * @param win: the window to move
 * @param x: the x coordinate of the new position
 * @param y: the y coordinate of the new position
 * @return Returns no value.
 * @brief Move the specified window to the given position
 *
 * Moves the window into the specified position in the
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
 * @param win: the window to query for position
 * @param x: a pointer to the integer that should receive the x coordinate
 * @param y: a pointer to the integer that should receive the y coordinate
 * @return Returns no value.
 * @brief Retrieve the position of the window
 *
 * Stores the window position into the parameters @a x and @a y.
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
 * @param w: the window to be initialized to default values and callbacks
 * @return Returns TRUE or FALSE depending on if initialization succeeds.
 * @brief Initialize a window to default values and callbacks
 *
 * Sets the values and callbacks of a window @a w to their defaults.
 */
int ewl_window_init(Ewl_Window * w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);

	/*
	 * Initialize the fields of the inherited container class
	 */
	ewl_embed_init(EWL_EMBED(w));
	ewl_widget_set_appearance(EWL_WIDGET(w), "window");
	ewl_object_set_fill_policy(EWL_OBJECT(w), EWL_FLAG_FILL_FILL);

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
	Ewl_Object       *o;
	Ewl_Embed        *embed;
	Ewl_Window       *window;
	char             *font_path;
	Ewd_List         *paths;
	char             *render;
	Evas_Engine_Info *info = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	embed = EWL_EMBED(w);
	window = EWL_WINDOW(w);
	o = EWL_OBJECT(w);

	/*
	 * FIXME: This needs to be explored a bit, the size should come down
	 * from a configure event, but need to double check.
	 */
	ewl_object_request_size(EWL_OBJECT(w), ewl_object_get_current_w(o),
			ewl_object_get_current_h(o));

	window->window = ecore_x_window_new(0, window->x, window->y,
			ewl_object_get_current_w(o),
			ewl_object_get_current_h(o));

	ecore_x_window_prop_name_class_set(window->window, "EWL", "EWL!");
	ecore_x_window_prop_title_set(window->window, window->title);
	ecore_x_window_prop_protocol_set(window->window,
			ECORE_X_WM_PROTOCOL_DELETE_REQUEST,1);

	/*
	 * Determine the type of evas to create.
	 */
	render = ewl_config_get_render_method();
	if (!render)
		render = strdup("software_x11");

	embed->evas = evas_new();
	evas_output_method_set(embed->evas,
			evas_render_method_lookup(render));

	info = evas_engine_info_get(embed->evas);
	if (!info) {
		fprintf(stderr, "Unable to use %s engine for rendering, "
				"falling back to software_x11\n", render);
		FREE(render);
		render = strdup("software_x11");
		evas_output_method_set(embed->evas,
				evas_render_method_lookup(render));
		info = evas_engine_info_get(embed->evas);
	}

	evas_output_size_set(embed->evas, ewl_object_get_current_w(o),
			ewl_object_get_current_h(o));
	evas_output_viewport_set(embed->evas, ewl_object_get_current_x(o),
			ewl_object_get_current_y(o),
			ewl_object_get_current_w(o),
			ewl_object_get_current_h(o));

#ifdef HAVE_EVAS_ENGINE_GL_X11_H
	if (!strcmp(render, "gl_x11")) {
		Evas_Engine_Info_GL_X11 *glinfo;

		glinfo = (Evas_Engine_Info_GL_X11 *)info;

		glinfo->info.display = ecore_x_display_get();
		glinfo->info.visual = DefaultVisual(glinfo->info.display,
				DefaultScreen(glinfo->info.display));
		glinfo->info.colormap = DefaultColormap(glinfo->info.display,
				DefaultScreen(glinfo->info.display));
		glinfo->info.drawable = window->window;
		glinfo->info.depth = DefaultDepth(glinfo->info.display,
				DefaultScreen(glinfo->info.display));
	}
	else
#endif
#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	{
		Evas_Engine_Info_Software_X11 *sinfo;

		sinfo = (Evas_Engine_Info_Software_X11 *)info;

		sinfo->info.display = ecore_x_display_get();
		sinfo->info.visual = DefaultVisual(sinfo->info.display,
				DefaultScreen(sinfo->info.display));
		sinfo->info.colormap = DefaultColormap(sinfo->info.display,
				DefaultScreen(sinfo->info.display));
		sinfo->info.drawable = window->window;
		sinfo->info.depth = DefaultDepth(sinfo->info.display,
				DefaultScreen(sinfo->info.display));
		sinfo->info.rotation = 0;
		sinfo->info.debug = 0;
	}
#endif

	evas_engine_info_set(embed->evas, info);

	paths = ewl_theme_font_path_get();
	ewd_list_goto_first(paths);
	while ((font_path = ewd_list_next(paths)))
		evas_font_path_append(embed->evas, font_path);

	embed->evas_window = window->window;

	if (window->flags & EWL_WINDOW_BORDERLESS)
		ecore_x_window_prop_borderless_set(window->window, 1);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_window_unrealize(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Object     *o;
	Ewl_Embed      *embed;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	embed = EWL_EMBED(w);
	o = EWL_OBJECT(w);

	ewl_evas_destroy(embed->evas);
	embed->evas = NULL;

	ecore_x_window_del(EWL_WINDOW(embed)->window);

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
	ecore_x_window_show(EWL_EMBED(w)->evas_window);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_window_hide(Ewl_Widget * widget, void *ev_data, void *user_data)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("widget", widget);

	ecore_x_window_hide(EWL_EMBED(widget)->evas_window);
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

	ecore_x_window_hide(EWL_EMBED(win)->evas_window);
	ecore_x_window_hide(win->window);

	ecore_x_window_del(EWL_EMBED(win)->evas_window);
	ecore_x_window_del(win->window);

	IF_FREE(win->title);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void __ewl_window_configure(Ewl_Widget * w, void *ev_data, void *user_data)
{
	Ewl_Window     *win;
	int             width, height;

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
	ecore_x_window_resize(EWL_EMBED(win)->evas_window, width, height);
	evas_output_size_set(EWL_EMBED(win)->evas, width, height);
	evas_output_viewport_set(EWL_EMBED(win)->evas,
			ewl_object_get_current_x(EWL_OBJECT(w)),
			ewl_object_get_current_y(EWL_OBJECT(w)),
			width, height);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
