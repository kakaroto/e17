#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif

#ifdef HAVE_EVAS_ENGINE_FB_H
#include <Ecore_Fb.h>
#include <Evas_Engine_FB.h>
#endif

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
#include <Ecore_X.h>
#endif

#ifdef HAVE_EVAS_ENGINE_GL_X11_H
#include <Evas_Engine_GL_X11.h>
#endif

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
#include <Evas_Engine_Software_X11.h>
#endif

Ecore_List *ewl_window_list = NULL;

/**
 * @return Returns a new window on success, or NULL on failure.
 * @brief Allocate and initialize a new window
 */
Ewl_Widget *ewl_window_new()
{
	Ewl_Window     *w;

	DENTER_FUNCTION(DLEVEL_STABLE);

	w = NEW(Ewl_Window, 1);
	if (!w)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

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
Ewl_Window *ewl_window_window_find(void *window)
{
	Ewl_Window     *retwin;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("window", window, NULL);

	ecore_list_goto_first(ewl_window_list);

	while ((retwin = ecore_list_next(ewl_window_list)))
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
void ewl_window_title_set(Ewl_Window * win, char *title)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);

	if (!title)
		title = "";

	if (strcmp(win->title, title)) {
		IF_FREE(win->title);
		win->title = strdup(title);
	}

	if (!REALIZED(win))
		return;

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (strstr(win->render, "x11")) {
		ecore_x_icccm_title_set((Ecore_X_Window)win->window, title);
		ecore_x_netwm_name_set((Ecore_X_Window)win->window, title);
	}
#endif

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to retrieve the window
 * @return Returns a pointer to a new copy of the title, NULL on failure.
 * @brief Retrieve the title of the specified window
 *
 * The returned title should be freed.
 */
char *ewl_window_title_get(Ewl_Window * win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, NULL);

	DRETURN_PTR(strdup(win->title), DLEVEL_STABLE);
}

/**
 * @param win: the window to change the name
 * @param name: the name to set for the window
 * @return Returns no value.
 * @brief Set the name of the specified window
 *
 * Sets the name of window @a w to @a name and calls the necessary X lib
 * functions to update the window.
 */
void ewl_window_name_set(Ewl_Window * win, char *name)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);

	if (!name)
		name = "";

	if (strcmp(win->name, name)) {
		IF_FREE(win->name);
		win->name = strdup(name);
	}

	if (!REALIZED(win))
		return;

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (strstr(win->render, "x11"))
		ecore_x_icccm_name_class_set((Ecore_X_Window)win->window,
					     name, win->name);
#endif

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to retrieve the window
 * @return Returns a pointer to a new copy of the name, NULL on failure.
 * @brief Retrieve the name of the specified window
 *
 * The returned name should be freed.
 */
char *ewl_window_name_get(Ewl_Window * win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, NULL);

	DRETURN_PTR(strdup(win->name), DLEVEL_STABLE);
}

/**
 * @param win: the window to change the class
 * @param classname: the class to set for the window
 * @return Returns no value.
 * @brief Set the class of the specified window
 *
 * Sets the class of window @a w to @a class and calls the necessary X lib
 * functions to update the window.
 */
void ewl_window_class_set(Ewl_Window * win, char *classname)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);

	if (!classname)
		classname = "";

	if (strcmp(win->classname, classname)) {
		IF_FREE(win->classname);
		win->classname = strdup(classname);
	}

	if (!REALIZED(win))
		return;

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (strstr(win->render, "x11"))
		ecore_x_icccm_name_class_set((Ecore_X_Window)win->window,
					     classname, win->classname);
#endif

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to retrieve the window
 * @return Returns a pointer to a new copy of the class, NULL on failure.
 * @brief Retrieve the class of the specified window
 *
 * The returned class should be freed.
 */
char *ewl_window_class_get(Ewl_Window * win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, NULL);

	DRETURN_PTR(strdup(win->classname), DLEVEL_STABLE);
}

/**
 * @param win: the window to remove the border
 * @return Returns no value.
 * @brief Remove the border from the specified window
 *
 * Remove the border from the specified widget and call the
 * necessary X lib functions to update the appearance.
 */
void ewl_window_borderless_set(Ewl_Window * win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);

	win->flags |= EWL_WINDOW_BORDERLESS;

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (REALIZED(win) && strstr(win->render, "x11"))
		ecore_x_mwm_borderless_set((Ecore_X_Window)win->window,
					   TRUE);
#endif

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

	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);
#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (strstr(win->render, "x11"))
		ecore_x_window_move((Ecore_X_Window)win->window, x, y);
#endif

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
void ewl_window_position_get(Ewl_Window * win, int *x, int *y)
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
 * @param win: the window to raise.
 * @return Returns no value.
 * @brief Raise a window.
 *
 * Raise the window @a win if it is realized.
 */
void
ewl_window_raise(Ewl_Window * win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);

	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (strstr(win->render, "x11"))
		ecore_x_window_raise((Ecore_X_Window)win->window);
#endif

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to lower.
 * @return Returns no value.
 * @brief Lower a window.
 *
 * Lower the window @a win if it is realized.
 */
void
ewl_window_lower(Ewl_Window * win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);

	if (!REALIZED(win))
		DRETURN(DLEVEL_STABLE);

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (strstr(win->render, "x11"))
		ecore_x_window_lower((Ecore_X_Window)win->window);
#endif

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: window to set transient
 * @param forwin: the window to be transient for
 * @return Returns no value.
 * @brief Sets a window to be transient for another window.
 */
void ewl_window_transient_for(Ewl_Window *win, Ewl_Window *forwin)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);

	win->transient = forwin;

	if (forwin && REALIZED(win)) {
#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
		if (REALIZED(forwin))
			ecore_x_icccm_transient_for_set((Ecore_X_Window)win->window,
							(Ecore_X_Window)forwin->window);
		else
			ewl_callback_append(EWL_WIDGET(forwin),
					    EWL_CALLBACK_REALIZE,
					    ewl_window_realize_transient_cb,
					    win);
	}
	else if (REALIZED(win)) {
		ecore_x_icccm_transient_for_unset((Ecore_X_Window)win->window);
#endif
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
	ewl_widget_appearance_set(EWL_WIDGET(w), "window");
	ewl_widget_inherit(EWL_WIDGET(w), "window");
	ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_FILL);
	w->title = strdup("EWL");
	w->name = strdup("EWL");
	w->classname  = strdup("EWL");

	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_REALIZE,
			     ewl_window_realize_cb, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_REALIZE,
			     ewl_window_postrealize_cb, NULL);
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_UNREALIZE,
			     ewl_window_unrealize_cb, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_SHOW,
			    ewl_window_show_cb, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_HIDE,
			    ewl_window_hide_cb, NULL);
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_DESTROY,
			     ewl_window_destroy_cb, NULL);
	/*
	 * Override the default configure callbacks since the window
	 * has special needs for placement.
	 */
	ewl_callback_del(EWL_WIDGET(w), EWL_CALLBACK_CONFIGURE,
			ewl_overlay_configure_cb);
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_CONFIGURE,
			     ewl_window_configure_cb, NULL);

	LAYER(w) = -1000;
	ecore_list_append(ewl_window_list, w);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

void ewl_window_realize_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Object       *o;
	Evas             *evas;
	Ewl_Embed        *embed;
	Ewl_Window       *window;
	char             *render;
	Evas_Engine_Info *info = NULL;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	embed = EWL_EMBED(w);
	window = EWL_WINDOW(w);
	o = EWL_OBJECT(w);

	/*
	 * Determine the type of evas to create.
	 */
	render = ewl_config_render_method_get();
	if (!render)
		render = strdup("software_x11");

	evas = evas_new();
	evas_output_method_set(evas, evas_render_method_lookup(render));

	info = evas_engine_info_get(evas);
	if (!info) {
		fprintf(stderr, "Unable to use %s engine for rendering, "
				"falling back to software_x11\n", render);
		FREE(render);
		render = strdup("software_x11");
		evas_output_method_set(evas, evas_render_method_lookup(render));
		info = evas_engine_info_get(evas);
	}

	if (!info) {
		DERROR("No valid engine available\n");
		exit(-1);
	}

	/*
	 * Prepare the base rendering region for the evas, such as the X
	 * window for the X11 based engines, or the surfaces for directfb.
	 */
#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (strstr(render, "x11") &&
			(ewl_engine_mask_get() & (EWL_ENGINE_SOFTWARE_X11 |
						  EWL_ENGINE_GL_X11))) {
		int width, height;
		Ecore_X_Window xwin;
		xwin = ecore_x_window_new(0, window->x,
					  window->y,
					  ewl_object_current_w_get(o),
					  ewl_object_current_h_get(o));

		ecore_x_icccm_name_class_set(xwin, window->name,
					     window->classname);
		ecore_x_icccm_title_set(xwin, window->title);
		ecore_x_netwm_name_set(xwin, window->title);
		ecore_x_icccm_protocol_set(xwin,
					   ECORE_X_WM_PROTOCOL_DELETE_REQUEST,1);

		if (window->flags & EWL_WINDOW_BORDERLESS)
			ecore_x_mwm_borderless_set(xwin, 1);

		width = ewl_object_maximum_w_get(EWL_OBJECT(window));
		height = ewl_object_maximum_h_get(EWL_OBJECT(window));
		if (width == EWL_OBJECT_MAX_SIZE && width == height) {
			ecore_x_window_geometry_get(0, NULL, NULL, &width,
						    &height);
			ewl_object_maximum_size_set(EWL_OBJECT(window),
						    width, height);
		}
		window->window = (void *)xwin;
	}
#endif

	/*
	 * Now perform engine specific info setup. This informs the evas of
	 * the drawing engine specific info it needs.
	 */
#ifdef HAVE_EVAS_ENGINE_GL_X11_H
	if (!strcmp(render, "gl_x11") &&
			(ewl_engine_mask_get() & EWL_ENGINE_GL_X11)) {
		Evas_Engine_Info_GL_X11 *glinfo;

		glinfo = (Evas_Engine_Info_GL_X11 *)info;

		glinfo->info.display = ecore_x_display_get();
		glinfo->info.visual = glinfo->func.best_visual_get(
				glinfo->info.display,
				DefaultScreen(glinfo->info.display));
		glinfo->info.colormap = glinfo->func.best_colormap_get(
				glinfo->info.display,
				DefaultScreen(glinfo->info.display));
		glinfo->info.drawable = (Ecore_X_Window)window->window;
		glinfo->info.depth = glinfo->func.best_depth_get(
				glinfo->info.display,
				DefaultScreen(glinfo->info.display));
	}
	else
#endif
#ifdef HAVE_EVAS_ENGINE_FB_H
	if (!strcmp(render, "fb") &&
			(ewl_engine_mask_get() & EWL_ENGINE_FB)) {
		Evas_Engine_Info_FB *fbinfo;

		window->window = fbinfo = (Evas_Engine_Info_FB *)info;

		fbinfo->info.virtual_terminal = 0;
		fbinfo->info.device_number = 0;
		fbinfo->info.refresh = 0;
		fbinfo->info.rotation = 0;
		evas_engine_info_set(evas, (Evas_Engine_Info *)fbinfo);
		ewl_object_geometry_request(EWL_OBJECT(w), 0, 0, 240, 320);
	}
	else
#endif
#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (!strcmp(render, "software_x11") && 
			(ewl_engine_mask_get() & EWL_ENGINE_SOFTWARE_X11)) {
		Evas_Engine_Info_Software_X11 *sinfo;

		sinfo = (Evas_Engine_Info_Software_X11 *)info;

		sinfo->info.display = ecore_x_display_get();
		sinfo->info.visual = DefaultVisual(sinfo->info.display,
				DefaultScreen(sinfo->info.display));
		sinfo->info.colormap = DefaultColormap(sinfo->info.display,
				DefaultScreen(sinfo->info.display));
		sinfo->info.drawable = (Ecore_X_Window)window->window;
		sinfo->info.depth = DefaultDepth(sinfo->info.display,
				DefaultScreen(sinfo->info.display));
		sinfo->info.rotation = 0;
		sinfo->info.debug = 0;
	}
	else
#endif
	{
		ewl_evas_destroy(evas);
		DRETURN(DLEVEL_STABLE);
	}

	evas_engine_info_set(evas, info);

	evas_output_size_set(evas, ewl_object_current_w_get(o),
			ewl_object_current_h_get(o));
	evas_output_viewport_set(evas, ewl_object_current_x_get(o),
			ewl_object_current_y_get(o),
			ewl_object_current_w_get(o),
			ewl_object_current_h_get(o));

	ewl_embed_evas_set(embed, evas, window->window);
	window->render = strdup(render);
	FREE(render);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_window_postrealize_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);

	evas_object_pass_events_set(EWL_EMBED(w)->ev_clip, 1);
	if (EWL_WINDOW(w)->transient)
		ewl_window_transient_for(EWL_WINDOW(w),
					 EWL_WINDOW(w)->transient);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_window_realize_transient_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
				     void *user_data)
{
	Ewl_Window *win = EWL_WINDOW(user_data);
	DENTER_FUNCTION(DLEVEL_STABLE);

	/*
	 * Make sure the window is still transient for the realized window.
	 */
	if (EWL_WIDGET(win->transient) == w)
		ewl_window_transient_for(win, EWL_WINDOW(w));

	/*
	 * Both windows realized so no need to keep the callback.
	 */
	ewl_callback_del(EWL_WIDGET(win), EWL_CALLBACK_REALIZE,
			 ewl_window_realize_transient_cb);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_window_unrealize_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Object     *o;
	Ewl_Embed      *embed;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	embed = EWL_EMBED(w);
	o = EWL_OBJECT(w);

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (REALIZED(w) && strstr(EWL_WINDOW(w)->render, "x11")) {
		ecore_x_window_hide((Ecore_X_Window)embed->evas_window);
		ecore_x_window_hide((Ecore_X_Window)EWL_WINDOW(w)->window);

		ecore_x_window_del((Ecore_X_Window)embed->evas_window);
		ecore_x_window_del((Ecore_X_Window)EWL_WINDOW(w)->window);
	}
#endif
	IF_FREE(EWL_WINDOW(w)->render);

	ewl_evas_destroy(embed->evas);
	embed->evas = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_window_show_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	if (!EWL_WINDOW(w)->window)
		DRETURN(DLEVEL_STABLE);

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (strstr(EWL_WINDOW(w)->render, "x11")) {
		if (EWL_WINDOW(w)->flags & EWL_WINDOW_BORDERLESS)
			ecore_x_mwm_borderless_set((Ecore_X_Window)
						   EWL_WINDOW(w)->window, 1);

		ecore_x_window_show((Ecore_X_Window)EWL_WINDOW(w)->window);
		ecore_x_window_show((Ecore_X_Window)EWL_EMBED(w)->evas_window);
	}
#endif

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_window_hide_cb(Ewl_Widget * widget, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("widget", widget);

#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (strstr(EWL_WINDOW(widget)->render, "x11")) {
		ecore_x_window_hide((Ecore_X_Window)EWL_EMBED(widget)->evas_window);
		ecore_x_window_hide((Ecore_X_Window)EWL_WINDOW(widget)->window);
	}
#endif

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_window_destroy_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Window     *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	win = EWL_WINDOW(w);

	IF_FREE(win->title);

	if ((win = ecore_list_goto(ewl_window_list, win)))
		ecore_list_remove(ewl_window_list);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

void ewl_window_configure_cb(Ewl_Widget * w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	Ewl_Window     *win;
	Ewl_Object     *child;
	int             width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);

	win = EWL_WINDOW(w);

	if (!win->window)
		DRETURN(DLEVEL_STABLE);

	/*
	 * Find out how much space the widget accepted.
	 */
	width = ewl_object_current_w_get(EWL_OBJECT(w));
	height = ewl_object_current_h_get(EWL_OBJECT(w));

	/*
	 * Now give the windows the appropriate size and adjust the evas as
	 * well.
	 */
	if (win->flags & EWL_WINDOW_USER_CONFIGURE)
		win->flags &= ~EWL_WINDOW_USER_CONFIGURE;
	else {
#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
		if (strstr(win->render, "x11"))
			ecore_x_window_resize((Ecore_X_Window)win->window,
					      width, height);
#endif

	}

	if (EWL_EMBED(win)->evas_window != win->window) {
#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
		if (strstr(win->render, "x11"))
			ecore_x_window_resize((Ecore_X_Window)
					EWL_EMBED(win)->evas_window, width,
					height);
#endif
	}
	evas_output_size_set(EWL_EMBED(win)->evas, width, height);
	evas_output_viewport_set(EWL_EMBED(win)->evas,
				 ewl_object_current_x_get(EWL_OBJECT(w)),
				 ewl_object_current_y_get(EWL_OBJECT(w)),
				 width, height);

	/*
	 * Adjust the minimum and maximum window bounds to match the widget.
	 * Do this after the resize to prevent early mapping, and the object
	 * keeps the bounds respected.
	 */
#ifdef HAVE_EVAS_ENGINE_SOFTWARE_X11_H
	if (strstr(win->render, "x11")) {
		ecore_x_icccm_size_pos_hints_set((Ecore_X_Window)win->window,
						 0, ECORE_X_GRAVITY_NW,
						 ewl_object_minimum_w_get(EWL_OBJECT(w)),
						 ewl_object_minimum_h_get(EWL_OBJECT(w)),
						 ewl_object_maximum_w_get(EWL_OBJECT(w)),
						 ewl_object_maximum_h_get(EWL_OBJECT(w)),
						 0, 0, /* base */
						 0, 0, /* step */
						 0, 0); /* aspect */
	}
#endif

	/*
	 * Configure each of the child widgets.
	 */
	ecore_list_goto_first(EWL_CONTAINER(w)->children);
	while ((child = ecore_list_next(EWL_CONTAINER(w)->children))) {
		int             x, y;

		/*
		 * Try to give the child the full size of the window from it's
		 * base position. The object will constrict it based on the
		 * fill policy. Don't add the TOP and LEFT insets since
		 * they've already been accounted for.
		 */
		x = ewl_object_current_x_get(EWL_OBJECT(child));
		y = ewl_object_current_y_get(EWL_OBJECT(child));
		if (x < CURRENT_X(w))
			x = CURRENT_X(w);
		if (y < CURRENT_Y(w))
			y = CURRENT_Y(w);
		ewl_object_place(child, x, y, CURRENT_W(w) - x,
				 CURRENT_H(w) - y);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}
