#include "ewl_private.h"
#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"

Ecore_List *ewl_window_list = NULL;

/**
 * @return Returns a new window on success, or NULL on failure.
 * @brief Allocate and initialize a new window
 */
Ewl_Widget *
ewl_window_new(void)
{
	Ewl_Window *w;

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
 * @param w: the window to be initialized to default values and callbacks
 * @return Returns TRUE or FALSE depending on if initialization succeeds.
 * @brief Initialize a window to default values and callbacks
 *
 * Sets the values and callbacks of a window @a w to their defaults.
 */
int
ewl_window_init(Ewl_Window *w)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("w", w, FALSE);

	/*
	 * Initialize the fields of the inherited container class
	 */
	if (!ewl_embed_init(EWL_EMBED(w)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_widget_appearance_set(EWL_WIDGET(w), EWL_WINDOW_TYPE);
	ewl_widget_inherit(EWL_WIDGET(w), EWL_WINDOW_TYPE);
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
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_EXPOSE,
			    ewl_window_expose_cb, NULL);
	ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_HIDE,
			    ewl_window_hide_cb, NULL);
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_DESTROY,
			     ewl_window_destroy_cb, NULL);
	/*
	 * Override the default configure callbacks since the window
	 * has special needs for placement.
	 */
	ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_CONFIGURE,
			     ewl_window_configure_cb, NULL);

	ecore_list_append(ewl_window_list, w);

	DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param window: the X window to search for on the list of ewl window's
 * @return Returns the found ewl window on success, NULL on failure.
 * @brief Find an ewl window by it's X window
 */
Ewl_Window *
ewl_window_window_find(void *window)
{
	Ewl_Window *retwin;

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
void
ewl_window_title_set(Ewl_Window *win, const char *title)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if ((!title) || (strcmp(win->title, title))) {
		IF_FREE(win->title);
		win->title = (title ? strdup(title) : strdup(""));
	}

	ewl_engine_window_title_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to retrieve the window
 * @return Returns a pointer to a new copy of the title, NULL on failure.
 * @brief Retrieve the title of the specified window
 *
 * The returned title should be freed.
 */
char *
ewl_window_title_get(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, NULL);
	DCHECK_TYPE_RET("win", win, EWL_WINDOW_TYPE, NULL);

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
void
ewl_window_name_set(Ewl_Window *win, const char *name)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if ((!name) || (strcmp(win->name, name))) {
		IF_FREE(win->name);
		win->name = (name ? strdup(name) : strdup(""));
	}

	ewl_engine_window_name_class_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to retrieve the window
 * @return Returns a pointer to a new copy of the name, NULL on failure.
 * @brief Retrieve the name of the specified window
 *
 * The returned name should be freed.
 */
char *
ewl_window_name_get(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, NULL);
	DCHECK_TYPE_RET("win", win, EWL_WINDOW_TYPE, NULL);

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
void
ewl_window_class_set(Ewl_Window *win, const char *classname)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if ((!classname) || (!win->classname) 
			|| (strcmp(win->classname, classname)))
	{
		IF_FREE(win->classname);
		win->classname = (classname ? strdup(classname) : strdup(""));
	}

	ewl_engine_window_name_class_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to retrieve the window
 * @return Returns a pointer to a new copy of the class, NULL on failure.
 * @brief Retrieve the class of the specified window
 *
 * The returned class should be freed.
 */
char *
ewl_window_class_get(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, NULL);
	DCHECK_TYPE_RET("win", win, EWL_WINDOW_TYPE, NULL);

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
void
ewl_window_borderless_set(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	win->flags |= EWL_WINDOW_BORDERLESS;

	ewl_engine_window_borderless_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window
 * @param dialog: TRUE or FALSE to indicate dialog state.
 * @return Returns no value.
 * @brief Changes the dialog state on the specified window.
 *
 * A dialog window has not a iconify and/or maximize button.
 */
void
ewl_window_dialog_set(Ewl_Window *win, int dialog)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	/* do nothing if already set */
	if (dialog == ewl_window_dialog_get(win))
		DRETURN(DLEVEL_STABLE);
	
	if (dialog)
		win->flags |= EWL_WINDOW_DIALOG;
	else
		win->flags &= ~EWL_WINDOW_DIALOG;

	ewl_engine_window_dialog_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: window to retrieve dialog state
 * @return Returns TRUE if window is an dialog window, FALSE otherwise.
 * @brief Retrieves the current dialog state on a window.
 */
int
ewl_window_dialog_get(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, FALSE);
	DCHECK_TYPE_RET("win", win, EWL_WINDOW_TYPE, FALSE);

	DRETURN_INT((!!(win->flags & EWL_WINDOW_DIALOG)), DLEVEL_STABLE);
}


/**
 * @param win: The window to work with
 * @param fullscreen: The fullscreen setting to use
 * @return Returns no value
 * @brief Sets the fullscreen setting for the window
 */
void 
ewl_window_fullscreen_set(Ewl_Window *win, unsigned int fullscreen)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	/* do nothing if already set */
	if (fullscreen == ewl_window_fullscreen_get(win))
		DRETURN(DLEVEL_STABLE);

	if (fullscreen)
		win->flags |= EWL_WINDOW_FULLSCREEN;
	else
		win->flags &= ~EWL_WINDOW_FULLSCREEN;

	ewl_engine_window_fullscreen_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: The window to work with
 * @return Returns TRUE if the window is fullscreen, FALSE otherwise
 * @brief Retrieve the fullscreen setting for the window
 */
unsigned int
ewl_window_fullscreen_get(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, FALSE);
	DCHECK_TYPE_RET("win", win, EWL_WINDOW_TYPE, FALSE);

	DRETURN_INT((!!(win->flags & EWL_WINDOW_FULLSCREEN)), DLEVEL_STABLE);
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
void
ewl_window_move(Ewl_Window *win, int x, int y)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	EWL_EMBED(win)->x = x;
	EWL_EMBED(win)->y = y;

	ewl_engine_window_move(win);

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
ewl_window_raise(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ewl_engine_window_raise(win);

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
ewl_window_lower(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ewl_engine_window_lower(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: window to set transient
 * @param forwin: the window to be transient for
 * @return Returns no value.
 * @brief Sets a window to be transient for another window.
 */
void
ewl_window_transient_for(Ewl_Window *win, Ewl_Window *forwin)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	win->transient = forwin;

	if (forwin && win->window) {
		if (forwin->window)
			ewl_engine_window_transient_for(win);
		else
			ewl_callback_append(EWL_WIDGET(forwin),
					    EWL_CALLBACK_REALIZE,
					    ewl_window_realize_transient_cb,
					    win);
	}
	else if (win->window) 
		ewl_engine_window_transient_for(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: The window to work with
 * @param forwin: The window to go modal for
 * @return Returns no value
 * @brief Sets the window to modal for @a forwin
 */
void
ewl_window_modal_for(Ewl_Window *win, Ewl_Window *forwin)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	/* make sure the modal window is transient */
	ewl_window_transient_for(win, forwin);

	if (forwin)
		win->flags |= EWL_WINDOW_MODAL;
	else
		win->flags &= ~EWL_WINDOW_MODAL;

	ewl_engine_window_modal_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to change keyboard grab settings.
 * @param grab: TRUE or FALSE to indicate grab state.
 * @return Returns no value.
 * @brief Changes the keyboard grab state on the specified window.
 */
void
ewl_window_keyboard_grab_set(Ewl_Window *win, int grab)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (grab)
		win->flags |= EWL_WINDOW_GRAB_KEYBOARD;
	else 
		win->flags &= ~EWL_WINDOW_GRAB_KEYBOARD;

	ewl_engine_keyboard_grab(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: window to retrieve keyboard grab state
 * @return Returns TRUE if window is grabbing keyboard, FALSE otherwise.
 * @brief Retrieves the current keyboard grab state on a window.
 */
int
ewl_window_keyboard_grab_get(Ewl_Window *win)
{
	int grab;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, FALSE);
	DCHECK_TYPE_RET("win", win, EWL_WINDOW_TYPE, FALSE);

	if (win->flags & EWL_WINDOW_GRAB_KEYBOARD)
		grab = TRUE;
	else
		grab = FALSE;

	DRETURN_INT(grab, DLEVEL_STABLE);
}

/**
 * @param win: the window to change pointer grab settings.
 * @param grab: TRUE or FALSE to indicate grab state.
 * @return Returns no value.
 * @brief Changes the pointer grab state on the specified window.
 */
void
ewl_window_pointer_grab_set(Ewl_Window *win, int grab)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (grab)
		win->flags |= EWL_WINDOW_GRAB_POINTER;
	else 
		win->flags &= ~EWL_WINDOW_GRAB_POINTER;

	ewl_engine_pointer_grab(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: window to retrieve pointer grab state
 * @return Returns TRUE if window is grabbing pointer, FALSE otherwise.
 * @brief Retrieves the current pointer grab state on a window.
 */
int
ewl_window_pointer_grab_get(Ewl_Window *win)
{
	int grab;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, FALSE);
	DCHECK_TYPE_RET("win", win, EWL_WINDOW_TYPE, FALSE);

	if (win->flags & EWL_WINDOW_GRAB_POINTER)
		grab = TRUE;
	else
		grab = FALSE;

	DRETURN_INT(grab, DLEVEL_STABLE);
}

/**
 * @param win: the window to change override settings
 * @param override: TRUE or FALSE to indicate override state.
 * @return Returns no value.
 * @brief Changes the override state on the specified window.
 */
void
ewl_window_override_set(Ewl_Window *win, int override)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	if (override)
		win->flags |= EWL_WINDOW_OVERRIDE;
	else
		win->flags &= ~EWL_WINDOW_OVERRIDE;

	/* FIXME: Should probably unrealize and re-realize here. */

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: window to retrieve override state
 * @return Returns TRUE if window is an override window, FALSE otherwise.
 * @brief Retrieves the current override state on a window.
 */
int
ewl_window_override_get(Ewl_Window *win)
{
	int override;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("win", win, FALSE);
	DCHECK_TYPE_RET("win", win, EWL_WINDOW_TYPE, FALSE);

	if (win->flags & EWL_WINDOW_OVERRIDE)
		override = TRUE;
	else
		override = FALSE;

	DRETURN_INT(override, DLEVEL_STABLE);
}

/**
 * @param win: the window to remove the border
 * @return Returns no value.
 * @brief Set a window as being DND aware
 *
 * Inform ecore_x that this window is capable of receiving DND events
 */
void
ewl_window_dnd_aware_set(Ewl_Window *win)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	win->flags |= EWL_FLAG_PROPERTY_DND_AWARE;
	ewl_engine_window_dnd_aware_set(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: The window to set the selection on
 * @param txt: The text to set into the selection
 * @return Returns no value.
 *
 ' @brief This will set the given @a txt as the selection text on the window
 * or clear the text if @a txt is NULL
 */
void
ewl_window_selection_text_set(Ewl_Window *win, const char *txt)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("win", win);
	DCHECK_TYPE("win", win, EWL_WINDOW_TYPE);

	ewl_engine_window_selection_text_set(win, txt);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The realize callback
 */
void
ewl_window_realize_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Window *window;
	int width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	window = EWL_WINDOW(w);

	ewl_engine_window_new(window);	
	ewl_engine_window_name_class_set(window);
	ewl_engine_window_title_set(window);
	ewl_engine_window_borderless_set(window);
	ewl_engine_window_dialog_set(window);
	ewl_engine_window_fullscreen_set(window);
	ewl_engine_window_modal_set(window);

	width = ewl_object_maximum_w_get(EWL_OBJECT(window));
	height = ewl_object_maximum_h_get(EWL_OBJECT(window));
	if ((width == EWL_OBJECT_MAX_SIZE) && (width == height))
	{
		ewl_engine_window_geometry_get(window, TRUE, &width, &height);
		ewl_object_maximum_size_set(EWL_OBJECT(window), width, height);
	}
	ewl_engine_window_dnd_aware_set(window);

	ewl_engine_canvas_setup(window, ewl_config.evas.render_debug);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The postrealize callback
 */
void
ewl_window_postrealize_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
						void *user_data __UNUSED__)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	if (EWL_WINDOW(w)->transient)
		ewl_window_transient_for(EWL_WINDOW(w),
					 EWL_WINDOW(w)->transient);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The realize transient callback
 */
void
ewl_window_realize_transient_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
				     void *user_data)
{
	Ewl_Window *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("user_data", user_data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	win = EWL_WINDOW(user_data);
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

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The unrealize callback
 */
void
ewl_window_unrealize_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Window *win;
	Ewl_Embed *embed;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	embed = EWL_EMBED(w);
	win = EWL_WINDOW(w);

	if (REALIZED(w))
	{
		ewl_engine_window_hide(win);
		ewl_engine_window_destroy(win);
	}

	ewl_evas_destroy(embed->evas);
	embed->evas = NULL;

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The show callback
 */
void
ewl_window_show_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
				void *user_data __UNUSED__)
{
	Ewl_Window *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	win = EWL_WINDOW(w);
	if (!win->window)
		DRETURN(DLEVEL_STABLE);

	ewl_engine_window_borderless_set(win);
	/*
	 * Now give the windows the appropriate size
	 */
	if (win->flags & EWL_WINDOW_USER_CONFIGURE)
		win->flags &= ~EWL_WINDOW_USER_CONFIGURE;
	else    
		ewl_engine_window_resize(win);

	ewl_engine_window_show(win);

	if (win->flags & EWL_WINDOW_OVERRIDE)
		ewl_widget_configure(w);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The expose callback
 */
void
ewl_window_expose_cb(Ewl_Widget *w, void *ev __UNUSED__, 
				void *user_data __UNUSED__)
{
	Ewl_Window *win;
	int grabval;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	win = EWL_WINDOW(w);

	ewl_engine_keyboard_grab(win);
	grabval = ewl_engine_pointer_grab(win);

	if (grabval == GrabNotViewable)
		printf("GrabNotViewable\n");
	else if (grabval == AlreadyGrabbed)
		printf("AlreadyGrabbed\n");
	else if (grabval == GrabFrozen)
		printf("GrabFrozen\n");
	else if (grabval == GrabInvalidTime)
		printf("GrabInvalidTime\n");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param widget: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The hide callback
 */
void
ewl_window_hide_cb(Ewl_Widget *widget, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Window *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("widget", widget);
	DCHECK_TYPE("widget", widget, EWL_WIDGET_TYPE);

	win = EWL_WINDOW(widget);

	ewl_engine_window_hide(win);
	if (win->flags & EWL_WINDOW_GRAB_KEYBOARD)
		ewl_engine_keyboard_ungrab(win);

	if (win->flags & EWL_WINDOW_GRAB_POINTER)
		ewl_engine_pointer_ungrab(win);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_window_destroy_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Window *win;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

	win = EWL_WINDOW(w);

	IF_FREE(win->title);
	IF_FREE(win->name);
	IF_FREE(win->classname);

	if ((win = ecore_list_goto(ewl_window_list, win)))
		ecore_list_remove(ewl_window_list);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev_data: UNUSED
 * @param user_data: UNUSED
 * @return Returns no value
 * @brief The configure callback
 */
void
ewl_window_configure_cb(Ewl_Widget *w, void *ev_data __UNUSED__,
					void *user_data __UNUSED__)
{
	Ewl_Window *win;
	int width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);

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
	else
		ewl_engine_window_resize(win);

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
	ewl_engine_window_min_max_size_set(win);
	
	DLEAVE_FUNCTION(DLEVEL_STABLE);
}


