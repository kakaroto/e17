/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

Ecore_List *ewl_window_list = NULL;
unsigned int EWL_CALLBACK_EXPOSE = 0;
unsigned int EWL_CALLBACK_DELETE_WINDOW = 0;

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
        DCHECK_PARAM_PTR_RET(w, FALSE);

        /*
         * Initialize the fields of the inherited container class
         */
        if (!ewl_embed_init(EWL_EMBED(w)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(EWL_WIDGET(w), EWL_WINDOW_TYPE);
        ewl_widget_inherit(EWL_WIDGET(w), EWL_WINDOW_TYPE);
        ewl_object_fill_policy_set(EWL_OBJECT(w), EWL_FLAG_FILL_FILL);

        ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_REALIZE,
                             ewl_window_cb_realize, NULL);
        ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_REALIZE,
                             ewl_window_cb_postrealize, NULL);
        ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_UNREALIZE,
                             ewl_window_cb_unrealize, NULL);
        ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_SHOW,
                            ewl_window_cb_show, NULL);
        ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_EXPOSE,
                            ewl_window_cb_expose, NULL);
        ewl_callback_append(EWL_WIDGET(w), EWL_CALLBACK_HIDE,
                            ewl_window_cb_hide, NULL);
        ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_DESTROY,
                             ewl_window_cb_destroy, NULL);
        /*
         * Override the default configure callbacks since the window
         * has special needs for placement.
         */
        ewl_callback_prepend(EWL_WIDGET(w), EWL_CALLBACK_CONFIGURE,
                             ewl_window_cb_configure, NULL);

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
        DCHECK_PARAM_PTR_RET(window, NULL);

        ecore_list_first_goto(ewl_window_list);
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if ((!title) || (!win->title) || (strcmp(win->title, title))) {
                IF_FREE(win->title);
                win->title = ((title && *title) ? strdup(title) : NULL);
        }

        ewl_engine_window_title_set(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to retrieve the window
 * @return Returns a pointer to a new copy of the title, NULL on failure.
 * @brief Retrieve the title of the specified window
 *
 * The returned title should not be freed, and should be copied immediately if
 * needed for extended use.
 */
const char *
ewl_window_title_get(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(win, NULL);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, NULL);

        DRETURN_PTR(win->title, DLEVEL_STABLE);
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if ((!name) || (!win->name) || (strcmp(win->name, name))) {
                IF_FREE(win->name);
                win->name = ((name && *name) ? strdup(name) : NULL);
        }

        ewl_engine_window_name_class_set(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to retrieve the window
 * @return Returns a pointer to a new copy of the name, NULL on failure.
 * @brief Retrieve the name of the specified window
 *
 * The returned name should not be freed, and should be copied immediately if
 * needed for extended use.
 */
const char *
ewl_window_name_get(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(win, NULL);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, NULL);

        DRETURN_PTR(win->name, DLEVEL_STABLE);
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if ((!classname) || (!win->classname)
                        || (strcmp(win->classname, classname)))
        {
                IF_FREE(win->classname);
                win->classname = ((classname && *classname) ?
                                strdup(classname) : NULL);
        }

        ewl_engine_window_name_class_set(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to retrieve the window
 * @return Returns a pointer to a new copy of the class, NULL on failure.
 * @brief Retrieve the class of the specified window
 *
 * The returned class should not be freed, and should be copied immediately if
 * needed for extended use.
 */
const char *
ewl_window_class_get(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(win, NULL);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, NULL);

        DRETURN_PTR(win->classname, DLEVEL_STABLE);
}

/**
 * @param win: the window to change the border flag on
 * @param border: the borderless flag to set, either TRUe or FALSE
 * @return Returns no value.
 * @brief Changes the border from the specified window
 *
 * Changes the border from the specified window
 */
void
ewl_window_borderless_set(Ewl_Window *win, unsigned int border)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        border = !!border;

        /* do nothing if already set */
        if (border == ewl_window_borderless_get(win))
                DRETURN(DLEVEL_STABLE);

        if (border) win->flags |= EWL_WINDOW_BORDERLESS;
        else win->flags &= ~EWL_WINDOW_BORDERLESS;

        ewl_engine_window_borderless_set(win);
        ewl_widget_unrealize(EWL_WIDGET(win));
        ewl_widget_realize(EWL_WIDGET(win));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: The window to get the border settings from
 * @return Returns TRUE if the window is borderless, FALSE otherwise
 * @brief Retrieves the borderless flag for the window
 */
unsigned int
ewl_window_borderless_get(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(win, FALSE);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, FALSE);

        DRETURN_INT(!!(win->flags & EWL_WINDOW_BORDERLESS), DLEVEL_STABLE);
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

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
        DCHECK_PARAM_PTR_RET(win, FALSE);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, FALSE);

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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        /* do nothing if already set */
        if (fullscreen == ewl_window_fullscreen_get(win))
                DRETURN(DLEVEL_STABLE);

        if (fullscreen)
                win->flags |= EWL_WINDOW_FULLSCREEN;
        else
                win->flags &= ~EWL_WINDOW_FULLSCREEN;

        ewl_engine_window_states_set(win);

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
        DCHECK_PARAM_PTR_RET(win, FALSE);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, FALSE);

        DRETURN_INT((!!(win->flags & EWL_WINDOW_FULLSCREEN)), DLEVEL_STABLE);
}

/**
 * @param win: The window to work with
 * @param skip: If the @p win should not be in the taskbar
 * @return Returns no value
 * @brief Sets the skip taskbar setting for the window
 */
void
ewl_window_skip_taskbar_set(Ewl_Window *win, unsigned int skip)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        /* do nothing if already set */
        if (skip == ewl_window_skip_taskbar_get(win))
                DRETURN(DLEVEL_STABLE);

        if (skip)
                win->flags |= EWL_WINDOW_SKIP_TASKBAR;
        else
                win->flags &= ~EWL_WINDOW_SKIP_TASKBAR;

        ewl_engine_window_states_set(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: The window to work with
 * @return Returns TRUE if the window is to be skipped for the taskbar,
 *                FALSE otherwise
 * @brief Retrieve the skip taskbar setting for the window
 */
unsigned int
ewl_window_skip_taskbar_get(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(win, FALSE);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, FALSE);

        DRETURN_INT((!!(win->flags & EWL_WINDOW_SKIP_TASKBAR)), DLEVEL_STABLE);
}

/**
 * @param win: The window to work with
 * @param skip: If the @p win should be visible in the pager
 * @return Returns no value
 * @brief Sets the skip pager setting for the window
 */
void
ewl_window_skip_pager_set(Ewl_Window *win, unsigned int skip)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        /* do nothing if already set */
        if (skip == ewl_window_skip_pager_get(win))
                DRETURN(DLEVEL_STABLE);

        if (skip)
                win->flags |= EWL_WINDOW_SKIP_PAGER;
        else
                win->flags &= ~EWL_WINDOW_SKIP_PAGER;

        ewl_engine_window_states_set(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: The window to work with
 * @return Returns TRUE if the window is to be skipped for the pager,
 *                FALSE otherwise
 * @brief Retrieve the skip pager setting for the window
 */
unsigned int
ewl_window_skip_pager_get(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(win, FALSE);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, FALSE);

        DRETURN_INT((!!(win->flags & EWL_WINDOW_SKIP_PAGER)), DLEVEL_STABLE);
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        ewl_engine_window_lower(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with.
 * @return Returns no value.
 * @brief Request the WM to pay attention to the window
 *
 * Demand attention for the window @a win if it is realized.
 * The window manager will then try to draw attention to the
 * window, e.g. a blinking taskbar entry. When the window
 * got the wanted attention the window manager will stop
 * this action itself.
 */
void
ewl_window_attention_demand(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        win->flags |= EWL_WINDOW_DEMANDS_ATTENTION;
        ewl_engine_window_states_set(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with.
 * @param urgent: if the window should have the urgent hint
 * @return Returns no value.
 * @brief Set the window to be urgent
 *
 * This is similar to a attention demand with the difference, that
 * it is more urgent and the window manager might even raise the window
 * and/or let title bar blink. Different from attention demand the
 * window manager will not reset it to normal state, so it is up
 * to the application to do this, after it got the needed attention.
 */
void
ewl_window_urgent_set(Ewl_Window *win, unsigned int urgent)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (urgent)
                win->flags |= EWL_WINDOW_URGENT;
        else
                win->flags &= ~EWL_WINDOW_URGENT;

        ewl_engine_window_hints_set(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: the window to work with.
 * @return the urgent state
 * @brief Get the window urgent state
 */
unsigned int
ewl_window_urgent_get(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(win, FALSE);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, FALSE);

        DRETURN_INT(!!(win->flags & EWL_WINDOW_URGENT), DLEVEL_STABLE);
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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);
        DCHECK_TYPE(forwin, EWL_WINDOW_TYPE);

        win->transient.ewl = forwin;
        win->flags &= ~EWL_WINDOW_TRANSIENT_FOREIGN;

        /* if there is no forwin remove the transient for state
         * and update the window, if it already exists */
        if (!forwin) {
                win->flags &= ~EWL_WINDOW_TRANSIENT;
                if (win->window)
                        ewl_engine_window_transient_for(win);

                DRETURN(DLEVEL_STABLE);
        }

        win->flags |= EWL_WINDOW_TRANSIENT;

        if (win->window) {
                if (forwin->window)
                        ewl_engine_window_transient_for(win);
                else
                        ewl_callback_append(EWL_WIDGET(forwin),
                                            EWL_CALLBACK_REALIZE,
                                            ewl_window_cb_realize_parent,
                                            win);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: window to set transient
 * @param forwin: the window to be transient for
 * @return Returns no value.
 * @brief Sets a window to be transient for another window.
 */
void
ewl_window_transient_for_foreign(Ewl_Window *win, Ewl_Embed_Window *forwin)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        win->transient.foreign = forwin;
        win->flags |= EWL_WINDOW_TRANSIENT_FOREIGN;
        win->flags &= ~EWL_WINDOW_TRANSIENT;

        if (win->window)
                ewl_engine_window_transient_for(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: window to set leader for
 * @param leader: the window that is the leader of the window group
 * @return Returns no value.
 * @brief Sets the window to be client window of the leader
 */
void
ewl_window_leader_set(Ewl_Window *win, Ewl_Window *leader)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);
        DCHECK_TYPE(leader, EWL_WINDOW_TYPE);

        win->leader.ewl = leader;
        win->flags &= ~EWL_WINDOW_LEADER_FOREIGN;

        /* if there is no leader remove the leader for state
         * and update the window, if it already exists */
        if (!leader) {
                win->flags &= ~EWL_WINDOW_LEADER;
                if (win->window) {
                        ewl_engine_window_leader_set(win);
                        ewl_engine_window_hints_set(win);
                }

                DRETURN(DLEVEL_STABLE);
        }

        win->flags |= EWL_WINDOW_LEADER;

        if (win->window) {
                if (leader->window) {
                        ewl_engine_window_leader_set(win);
                        ewl_engine_window_hints_set(win);
                }
                else
                        ewl_callback_append(EWL_WIDGET(leader),
                                            EWL_CALLBACK_REALIZE,
                                            ewl_window_cb_realize_parent,
                                            win);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: window to set leader for
 * @param leader: the window that is the leader of the window group
 * @return Returns no value.
 * @brief Sets the window to be client window of the leader
 */
void
ewl_window_leader_foreign_set(Ewl_Window *win, Ewl_Embed_Window *leader)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        win->leader.foreign = leader;
        win->flags &= ~EWL_WINDOW_LEADER;

        /* if there is no leader remove the leader for state
         * and update the window, if it already exists */
        if (!leader)
                win->flags &= ~EWL_WINDOW_LEADER_FOREIGN;
        else
                win->flags |= EWL_WINDOW_LEADER_FOREIGN;

        if (win->window) {
                ewl_engine_window_leader_set(win);
                ewl_engine_window_hints_set(win);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param win: window to get leader for
 * @return leader of the window or NULL
 * @brief Gets the leader of this window
 *
 * @note this function returns even NULL if the leader
 * is a foreign window
 */
Ewl_Window *
ewl_window_leader_get(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(win, NULL);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, NULL);

        if (win->flags & EWL_WINDOW_LEADER)
                DRETURN_PTR(win->leader.ewl, DLEVEL_STABLE);

        DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @param win: window to get leader for
 * @return Returns the leader of this window or NULL
 * @brief Gets the leader of this window
 *
 * @note this function returns even NULL if the leader
 * is a ewl window
 */
Ewl_Embed_Window *
ewl_window_leader_foreign_get(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(win, NULL);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, NULL);

        if (win->flags & EWL_WINDOW_LEADER_FOREIGN)
                DRETURN_PTR(win->leader.foreign, DLEVEL_STABLE);

        DRETURN_PTR(NULL, DLEVEL_STABLE);
}

/**
 * @param win: The window to work with
 * @return Returns a boolean indicating if the window is modal.
 * @brief Gets the boolean flag indicating if @a win is modal
 */
int
ewl_window_modal_get(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(win, FALSE);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, FALSE);

        DRETURN_INT(!!(win->flags & EWL_WINDOW_MODAL), DLEVEL_STABLE);
}

/**
 * @param win: The window to work with
 * @param modal: Boolean to indicate if this window is modal
 * @return Returns no value
 * @brief Sets the window to modal or non-modal based on @a modal
 */
void
ewl_window_modal_set(Ewl_Window *win, int modal)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (modal)
                win->flags |= EWL_WINDOW_MODAL;
        else
                win->flags &= ~EWL_WINDOW_MODAL;

        ewl_engine_window_states_set(win);

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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

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
        DCHECK_PARAM_PTR_RET(win, FALSE);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, FALSE);

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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

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
        DCHECK_PARAM_PTR_RET(win, FALSE);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, FALSE);

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
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

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
        DCHECK_PARAM_PTR_RET(win, FALSE);
        DCHECK_TYPE_RET(win, EWL_WINDOW_TYPE, FALSE);

        if (win->flags & EWL_WINDOW_OVERRIDE)
                override = TRUE;
        else
                override = FALSE;

        DRETURN_INT(override, DLEVEL_STABLE);
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
ewl_window_cb_realize(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Window *window;
        int width, height;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WINDOW_TYPE);

        window = EWL_WINDOW(w);

        ewl_engine_window_new(window);
        ewl_engine_window_name_class_set(window);
        ewl_engine_window_title_set(window);
        ewl_engine_window_borderless_set(window);
        ewl_engine_window_dialog_set(window);
        ewl_engine_window_states_set(window);
        ewl_engine_window_hints_set(window);
        ewl_engine_window_leader_set(window);

        width = ewl_object_maximum_w_get(EWL_OBJECT(window));
        height = ewl_object_maximum_h_get(EWL_OBJECT(window));
        if ((width == EWL_OBJECT_MAX_SIZE) && (width == height))
        {
                ewl_engine_window_geometry_get(window, TRUE, &width, &height);
                if (width > 1 && height > 1)
                        ewl_object_maximum_size_set(EWL_OBJECT(window), width,
                                        height);
        }
        ewl_engine_embed_dnd_aware_set(EWL_EMBED(window));

        ewl_engine_canvas_setup(window, ewl_config_int_get(ewl_config,
                                        EWL_CONFIG_DEBUG_EVAS_RENDER));

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
ewl_window_cb_postrealize(Ewl_Widget *w, void *ev_data __UNUSED__,
                                                void *user_data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WINDOW_TYPE);

        if (EWL_WINDOW(w)->flags & EWL_WINDOW_TRANSIENT)
                ewl_window_transient_for(EWL_WINDOW(w),
                                         EWL_WINDOW(w)->transient.ewl);
        else if (EWL_WINDOW(w)->flags & EWL_WINDOW_TRANSIENT_FOREIGN)
                ewl_window_transient_for_foreign(EWL_WINDOW(w),
                                         EWL_WINDOW(w)->transient.foreign);

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
ewl_window_cb_realize_parent(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data)
{
        Ewl_Window *win;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(user_data);
        DCHECK_TYPE(w, EWL_WINDOW_TYPE);
        DCHECK_TYPE(user_data, EWL_WINDOW_TYPE);

        win = EWL_WINDOW(user_data);
        /*
         * Is the window transient for the realized window.
         */
        if (EWL_WIDGET(win->transient.ewl) == w)
                ewl_window_transient_for(win, EWL_WINDOW(w));

        /*
         * Is the window a client of the realized leader window.
         */
        if (EWL_WIDGET(win->leader.ewl) == w)
                ewl_window_leader_set(win, EWL_WINDOW(w));

        /*
         * Both windows realized so no need to keep the callback.
         */
        ewl_callback_del(EWL_WIDGET(win), EWL_CALLBACK_REALIZE,
                         ewl_window_cb_realize_parent);

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
ewl_window_cb_unrealize(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Window *win;
        Ewl_Embed *embed;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WINDOW_TYPE);

        embed = EWL_EMBED(w);
        win = EWL_WINDOW(w);

        ewl_embed_cache_cleanup(embed);
        ewl_canvas_destroy(embed->canvas);
        embed->canvas = NULL;

        if (REALIZED(w))
        {
                ewl_engine_window_hide(win);
                ewl_engine_window_destroy(win);
        }

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
ewl_window_cb_show(Ewl_Widget *w, void *ev_data __UNUSED__,
                                void *user_data __UNUSED__)
{
        Ewl_Window *win;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WINDOW_TYPE);

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
ewl_window_cb_expose(Ewl_Widget *w, void *ev __UNUSED__,
                                void *user_data __UNUSED__)
{
        Ewl_Window *win;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WINDOW_TYPE);

        win = EWL_WINDOW(w);

        ewl_engine_keyboard_grab(win);
        ewl_engine_pointer_grab(win);

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
ewl_window_cb_hide(Ewl_Widget *widget, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Window *win;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(widget);
        DCHECK_TYPE(widget, EWL_WINDOW_TYPE);

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
ewl_window_cb_destroy(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Window *win;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WINDOW_TYPE);

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
ewl_window_cb_configure(Ewl_Widget *w, void *ev_data __UNUSED__,
                                        void *user_data __UNUSED__)
{
        Ewl_Window *win;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_WINDOW_TYPE);

        win = EWL_WINDOW(w);

        if (!win->window)
                DRETURN(DLEVEL_STABLE);

        /*
         * Now give the windows the appropriate size and adjust the evas as
         * well.
         */
        if (win->flags & EWL_WINDOW_USER_CONFIGURE)
                win->flags &= ~EWL_WINDOW_USER_CONFIGURE;
        else
                ewl_engine_window_resize(win);

        ewl_engine_canvas_output_set(EWL_EMBED(win),
                        ewl_object_current_x_get(EWL_OBJECT(win)),
                        ewl_object_current_y_get(EWL_OBJECT(win)),
                        ewl_object_current_w_get(EWL_OBJECT(win)),
                        ewl_object_current_h_get(EWL_OBJECT(win)));

        /*
         * Adjust the minimum and maximum window bounds to match the widget.
         * Do this after the resize to prevent early mapping, and the object
         * keeps the bounds respected.
         */
        ewl_engine_window_min_max_size_set(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}


