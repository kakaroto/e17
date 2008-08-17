/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include <Ewl_Engine_Win32.h>
#include "ewl_private.h"
#include "ewl_macros.h"
#include "ewl_debug.h"

/*
 * In general all of the X event handlers should find their matching window
 * with ewl_window_window_find, and not ewl_embed_canvas_window_find. If the
 * embed function is used, then we get duplicate events for apps that setup
 * their own handlers and embed EWL. The exception to this is selection events
 * such as copy/paste and DND. These events need to be handled for embedded
 * EWL, but have no equivalent in the Evas callback namespace.
 */
static int ewl_ev_win32_window_expose(void *data, int type, void *_ev);
static int ewl_ev_win32_window_configure(void *data, int type, void *_ev);
static int ewl_ev_win32_window_delete(void *data, int type, void *_ev);
static int ewl_ev_win32_key_down(void *data, int type, void *_ev);
static int ewl_ev_win32_key_up(void *data, int type, void *_ev);
static int ewl_ev_win32_mouse_down(void *data, int type, void *_ev);
static int ewl_ev_win32_mouse_wheel(void *data, int type, void *_ev);
static int ewl_ev_win32_mouse_up(void *data, int type, void *_ev);
static int ewl_ev_win32_mouse_move(void *data, int type, void *_ev);
static int ewl_ev_win32_mouse_out(void *data, int type, void *_ev);
static int ewl_ev_win32_focus_in(void *data, int type, void *_ev);
static int ewl_ev_win32_focus_out(void *data, int type, void *_ev);

static Ecore_Event_Handler *ee_expose_handler = NULL;
static Ecore_Event_Handler *ee_configure_handler = NULL;
static Ecore_Event_Handler *ee_delete_request_handler = NULL;
static Ecore_Event_Handler *ee_key_down_handler = NULL;
static Ecore_Event_Handler *ee_key_up_handler = NULL;
static Ecore_Event_Handler *ee_mouse_down_handler = NULL;
static Ecore_Event_Handler *ee_mouse_up_handler = NULL;
static Ecore_Event_Handler *ee_mouse_move_handler = NULL;
static Ecore_Event_Handler *ee_mouse_wheel_handler = NULL;
static Ecore_Event_Handler *ee_mouse_out_handler = NULL;
static Ecore_Event_Handler *ee_focus_in_handler = NULL;
static Ecore_Event_Handler *ee_focus_out_handler = NULL;

static void ee_shutdown(Ewl_Engine *engine);
static int  ee_init(Ewl_Engine *engine, int *argc, char ** argv);
static void ee_window_new(Ewl_Window *win);
static void ee_window_destroy(Ewl_Window *win);
static void ee_window_move(Ewl_Window *win);
static void ee_window_resize(Ewl_Window *win);
static void ee_window_min_max_size_set(Ewl_Window *win);
static void ee_window_show(Ewl_Window *win);
static void ee_window_hide(Ewl_Window *win);
static void ee_window_title_set(Ewl_Window *win);
static void ee_window_borderless_set(Ewl_Window *win);
static void ee_window_dialog_set(Ewl_Window *win);
static void ee_window_states_set(Ewl_Window *win);
static void ee_window_state_request(Ewl_Window *win, int states);
static void ee_window_state_handle(Ewl_Window *win, int states,
                                Ewl_Window_Flags ewl_flag,
                                Ecore_Win32_Window_State ecore_flag);
static void ee_window_states_set_helper(Ewl_Window *win);
static void ee_window_hints_set(Ewl_Window *win);
static void ee_window_transient_for(Ewl_Window *win);
static void ee_window_leader_set(Ewl_Window *win);
static void ee_window_raise(Ewl_Window *win);
static void ee_window_lower(Ewl_Window *win);
static void ee_window_geometry_set(Ewl_Window *win, int *width, int *height);
static void ee_desktop_size_get(Ewl_Embed *embed, int *w, int *h);

static int  ee_pointer_data_new(Ewl_Embed *embed, int *data, int w, int h);
static void ee_pointer_free(Ewl_Embed *embed, int pointer);
static void ee_pointer_set(Ewl_Embed *embed, int pointer);
static int  ee_pointer_get(Ewl_Embed *embed);

static void *window_funcs[EWL_ENGINE_WINDOW_MAX] =
        {
                ee_window_new,
                ee_window_destroy,
                ee_window_move,
                ee_window_resize,
                ee_window_min_max_size_set,
                ee_window_show,
                ee_window_hide,
                ee_window_title_set,
                NULL,
                ee_window_borderless_set,
                ee_window_dialog_set,
                ee_window_states_set,
                ee_window_hints_set,
                ee_window_transient_for,
                ee_window_leader_set,
                ee_window_raise,
                ee_window_lower,
                NULL,
                NULL,
                NULL,
                NULL,
                NULL,
                ee_window_geometry_set,
                NULL,
                ee_desktop_size_get,
                NULL,
                NULL,
                NULL,
                NULL,
        };

static void *pointer_funcs[EWL_ENGINE_POINTER_MAX] =
        {
                ee_pointer_data_new,
                ee_pointer_free,
                ee_pointer_get,
                ee_pointer_set,
        };

Ecore_DList *
ewl_engine_dependancies(void)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        DRETURN_PTR(NULL, DLEVEL_STABLE);
}

Ewl_Engine *
ewl_engine_create(int *argc, char ** argv)
{
        Ewl_Engine_Win32 *engine;

        DENTER_FUNCTION(DLEVEL_STABLE);

        engine = NEW(Ewl_Engine_Win32, 1);
        if (!engine)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ee_init(EWL_ENGINE(engine), argc, argv))
        {
                FREE(engine);
                DRETURN_PTR(NULL, DLEVEL_STABLE);
        }

        DRETURN_PTR(EWL_ENGINE(engine), DLEVEL_STABLE);
}

static int
ee_init(Ewl_Engine *engine, int *argc, char ** argv)
{
        Ewl_Engine_Info *info;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(engine, FALSE);

        printf (" **** ee_init\n");
        /* If the event handlers are already setup don't do it again */
        if (ee_expose_handler)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        printf (" **** ee_init : ecore_win32_init\n");
        if (!ecore_win32_init())
        {
                fprintf(stderr, "Unable to initialize Ecore Win32.\n"
                                "Is your DISPLAY variable set correctly?\n\n");
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        printf (" **** ee_init : events : damage, configure, delete\n");
        ee_expose_handler = ecore_event_handler_add(
                                        ECORE_WIN32_EVENT_WINDOW_DAMAGE,
                                        ewl_ev_win32_window_expose, NULL);
        ee_configure_handler = ecore_event_handler_add(
                                        ECORE_WIN32_EVENT_WINDOW_CONFIGURE,
                                        ewl_ev_win32_window_configure, NULL);
        ee_delete_request_handler = ecore_event_handler_add(
                                        ECORE_WIN32_EVENT_WINDOW_DELETE_REQUEST,
                                        ewl_ev_win32_window_delete, NULL);

        /*
         * Register dispatching functions for keyboard events.
         */
        ee_key_down_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_KEY_DOWN,
                                                ewl_ev_win32_key_down, NULL);
        ee_key_up_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_KEY_UP,
                                                ewl_ev_win32_key_up, NULL);

        /*
         * Register dispatching functions for DND events
         */
        /*
        ee_dnd_position_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_XDND_POSITION,
                                                ewl_ev_dnd_position, NULL);
        ee_dnd_enter_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_XDND_ENTER,
                                                ewl_ev_dnd_enter, NULL);
        ee_dnd_leave_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_XDND_LEAVE,
                                                ewl_ev_dnd_leave, NULL);
        ee_dnd_drop_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_XDND_DROP,
                                                ewl_ev_dnd_drop, NULL);
        */

        /*
         * Selection callbacks to allow for data transfers.
         */
        /*
        ee_selection_notify_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_SELECTION_NOTIFY,
                                                ewl_ev_win32_data_received, NULL);
        */

        /*
         * Selection callbacks to allow for pasting.
         */
        /*
        ee_selection_request_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_SELECTION_REQUEST,
                                                ewl_ev_win32_data_request, NULL);
        */

        /*
         * Finally, register dispatching functions for mouse events.
         */
        ee_mouse_down_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_MOUSE_BUTTON_DOWN,
                                                ewl_ev_win32_mouse_down, NULL);
        ee_mouse_up_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_MOUSE_BUTTON_UP,
                                                ewl_ev_win32_mouse_up, NULL);
        ee_mouse_move_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_MOUSE_MOVE,
                                                ewl_ev_win32_mouse_move, NULL);
        ee_mouse_wheel_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_MOUSE_WHEEL,
                                                ewl_ev_win32_mouse_wheel, NULL);
        ee_mouse_out_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_MOUSE_OUT,
                                                ewl_ev_win32_mouse_out, NULL);
        ee_focus_in_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_WINDOW_FOCUS_IN,
                                                ewl_ev_win32_focus_in, NULL);
        ee_focus_out_handler = ecore_event_handler_add(
                                                ECORE_WIN32_EVENT_WINDOW_FOCUS_OUT,
                                                ewl_ev_win32_focus_out, NULL);

        if (!ee_expose_handler || !ee_configure_handler
                        || !ee_delete_request_handler
                        || !ee_key_down_handler || !ee_key_up_handler
                        || !ee_mouse_down_handler
                        || !ee_mouse_up_handler || !ee_mouse_move_handler
                        || !ee_mouse_wheel_handler || !ee_mouse_out_handler
                        || !ee_focus_in_handler || !ee_focus_out_handler)
        {
                ee_shutdown(EWL_ENGINE(engine));

                fprintf(stderr, "Unable to create Ecore Win32 event handlers.\n");
                DRETURN_INT(FALSE, DLEVEL_STABLE);
        }

        info = NEW(Ewl_Engine_Info, 1);
        info->shutdown = ee_shutdown;
        info->hooks.window = window_funcs;
        info->hooks.pointer = pointer_funcs;

        engine->functions = info;

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static void
ee_shutdown(Ewl_Engine *engine)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(engine);

        printf (" **** ee_shutdown\n");

        if (ee_expose_handler)
                ecore_event_handler_del(ee_expose_handler);
        ee_expose_handler = NULL;

        if (ee_configure_handler)
                ecore_event_handler_del(ee_configure_handler);
        ee_configure_handler = NULL;

        if (ee_delete_request_handler)
                ecore_event_handler_del(ee_delete_request_handler);
        ee_delete_request_handler = NULL;

        if (ee_key_down_handler)
                ecore_event_handler_del(ee_key_down_handler);
        ee_key_down_handler = NULL;

        if (ee_key_up_handler)
                ecore_event_handler_del(ee_key_up_handler);
        ee_key_up_handler = NULL;

        /*
        if (ee_dnd_position_handler)
                ecore_event_handler_del(ee_dnd_position_handler);
        ee_dnd_position_handler = NULL;

        if (ee_dnd_enter_handler)
                ecore_event_handler_del(ee_dnd_enter_handler);
        ee_dnd_enter_handler = NULL;

        if (ee_dnd_leave_handler)
                ecore_event_handler_del(ee_dnd_leave_handler);
        ee_dnd_leave_handler = NULL;

        if (ee_dnd_drop_handler)
                ecore_event_handler_del(ee_dnd_drop_handler);
        ee_dnd_drop_handler = NULL;

        if (ee_selection_notify_handler)
                ecore_event_handler_del(ee_selection_notify_handler);
        ee_selection_notify_handler = NULL;

        if (ee_selection_request_handler)
                ecore_event_handler_del(ee_selection_request_handler);
        ee_selection_request_handler = NULL;
        */

        if (ee_mouse_down_handler)
                ecore_event_handler_del(ee_mouse_down_handler);
        ee_mouse_down_handler = NULL;

        if (ee_mouse_up_handler)
                ecore_event_handler_del(ee_mouse_up_handler);
        ee_mouse_up_handler = NULL;

        if (ee_mouse_move_handler)
                ecore_event_handler_del(ee_mouse_move_handler);
        ee_mouse_move_handler = NULL;

        if (ee_mouse_wheel_handler)
                ecore_event_handler_del(ee_mouse_wheel_handler);
        ee_mouse_wheel_handler = NULL;

        if (ee_mouse_out_handler)
                ecore_event_handler_del(ee_mouse_out_handler);
        ee_mouse_out_handler = NULL;

        if (ee_focus_in_handler)
                ecore_event_handler_del(ee_focus_in_handler);
        ee_focus_in_handler = NULL;

        if (ee_focus_out_handler)
                ecore_event_handler_del(ee_focus_out_handler);
        ee_focus_out_handler = NULL;

        ecore_win32_shutdown();

        IF_FREE(engine->functions);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_new(Ewl_Window *win)
{
        Ecore_Win32_Window *window;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        printf (" **** ee_window_new : %d %d\n",
                ewl_object_current_w_get(EWL_OBJECT(win)),
                ewl_object_current_h_get(EWL_OBJECT(win)));

        if (win->flags & EWL_WINDOW_OVERRIDE)
                window = ecore_win32_window_override_new(0,
                                EWL_EMBED(win)->x, EWL_EMBED(win)->y,
                                ewl_object_current_w_get(EWL_OBJECT(win)),
                                ewl_object_current_h_get(EWL_OBJECT(win)));
        else
                window = ecore_win32_window_new(0,
                                EWL_EMBED(win)->x, EWL_EMBED(win)->y,
                                ewl_object_current_w_get(EWL_OBJECT(win)),
                                ewl_object_current_h_get(EWL_OBJECT(win)));
        win->window = (void *)window;

        printf ("flags : %d %d\n", win->flags & EWL_WINDOW_OVERRIDE, win->flags & EWL_WINDOW_BORDERLESS);
        if (win->flags & EWL_WINDOW_BORDERLESS)
                ee_window_borderless_set(win);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_destroy(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        ee_window_hide(win);
        ecore_win32_window_del((EWL_EMBED(win)->canvas_window));
        ecore_win32_window_del((win->window));

        EWL_EMBED(win)->canvas_window = NULL;
        win->window = NULL;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_move(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        ecore_win32_window_move(win->window,
                                EWL_EMBED(win)->x,
                                EWL_EMBED(win)->y);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_resize(Ewl_Window *win)
{
        int width, height;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        width = ewl_object_current_w_get(EWL_OBJECT(win));
        height = ewl_object_current_h_get(EWL_OBJECT(win));
        printf ("ee_window_resize : %d %d\n", width, height);

        ecore_win32_window_resize(win->window, width, height);

        if (EWL_EMBED(win)->canvas_window != win->window)
                ecore_win32_window_resize(EWL_EMBED(win)->canvas_window,
                                                width, height);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_min_max_size_set(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        printf ("ee_window_min_max_size_set %p\n", win->window);
        ecore_win32_window_size_min_set(win->window,
                                        ewl_object_minimum_w_get(EWL_OBJECT(win)),
                                        ewl_object_minimum_h_get(EWL_OBJECT(win)));
        ecore_win32_window_size_max_set(win->window,
                                        ewl_object_maximum_w_get(EWL_OBJECT(win)),
                                        ewl_object_maximum_h_get(EWL_OBJECT(win)));
        ecore_win32_window_size_base_set(win->window, 0, 0);
        ecore_win32_window_size_step_set(win->window, 0, 0);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_show(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        printf ("ee_window_show\n");
        ecore_win32_window_show(win->window);
        ecore_win32_window_show(EWL_EMBED(win)->canvas_window);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_hide(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        ecore_win32_window_hide(EWL_EMBED(win)->canvas_window);
        ecore_win32_window_hide(EWL_WINDOW(win)->window);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_title_set(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        ecore_win32_window_title_set(win->window, win->title);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_borderless_set(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        ecore_win32_window_borderless_set(win->window,
                                (!!(win->flags & EWL_WINDOW_BORDERLESS)));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_dialog_set(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        ecore_win32_window_type_set(win->window,
                        ((!!(win->flags & EWL_WINDOW_DIALOG)) ?
                                        ECORE_WIN32_WINDOW_TYPE_DIALOG :
                                        ECORE_WIN32_WINDOW_TYPE_NORMAL));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_states_set(Ewl_Window *win)
{
        int diff;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        diff = win->flags ^ win->old_flags;
        if (!diff) DRETURN(DLEVEL_STABLE);

        if (REALIZED(win))
                ee_window_state_request(win, diff);
        else
                ee_window_states_set_helper(win);

        win->old_flags = win->flags;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_state_handle(Ewl_Window *win, int states,
                                Ewl_Window_Flags ewl_flag,
                                Ecore_Win32_Window_State ecore_flag)
{
        int state = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        if (states & ewl_flag)
        {
                state = (!!(win->flags & ewl_flag));
                ecore_win32_window_state_request_send(win->window,
                        ecore_flag, state);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_state_request(Ewl_Window *win, int states)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        ee_window_state_handle(win, states, EWL_WINDOW_FULLSCREEN,
                                        ECORE_WIN32_WINDOW_STATE_FULLSCREEN);
        ee_window_state_handle(win, states, EWL_WINDOW_MODAL,
                                        ECORE_WIN32_WINDOW_STATE_MODAL);
        ee_window_state_handle(win, states, EWL_WINDOW_DEMANDS_ATTENTION,
                                        ECORE_WIN32_WINDOW_STATE_DEMANDS_ATTENTION);

        win->flags &= ~EWL_WINDOW_DEMANDS_ATTENTION;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_states_set_helper(Ewl_Window *win)
{
        Ecore_Win32_Window_State states[2];
        int count = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        /* Note: We're ignoring the attention demand state in here as it is
         * only relevant if the window has been realized */

        if (!win->window)
                DRETURN(DLEVEL_STABLE);

        if (!!(win->flags & EWL_WINDOW_FULLSCREEN))
                states[count++] = ECORE_WIN32_WINDOW_STATE_FULLSCREEN;

        if (!!(win->flags & EWL_WINDOW_MODAL))
                states[count++] = ECORE_WIN32_WINDOW_STATE_MODAL;

        ecore_win32_window_state_set(win->window, states, count);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_hints_set(Ewl_Window *win)
{
        Ewl_Embed_Window *win_group;
        int urgent = FALSE;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

/*         if (win->flags & EWL_WINDOW_LEADER) */
/*                 win_group = win->leader.ewl->window; */
/*         else if (win->flags & EWL_WINDOW_LEADER_FOREIGN) */
/*                 win_group = win->leader.foreign; */
/*         else */
/*                 win_group = NULL; */

/*         if (win->flags & EWL_WINDOW_URGENT) */
/*                 urgent = TRUE; */

/*         ecore_x_icccm_hints_set( win->window, */
/*                                 1, // accepts focus */
/*                                 0, // initial states */
/*                                 0, // icon pixmap */
/*                                 0, // icon mask */
/*                                 0, // icon window */
/*                                  win_group, // window group */
/*                                 urgent); // is urgent */

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_transient_for(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

/*         if (win->flags & EWL_WINDOW_TRANSIENT) */
/*                 ecore_x_icccm_transient_for_set(win->window, */
/*                                 win->transient.ewl->window); */
/*         else if (win->flags & EWL_WINDOW_TRANSIENT_FOREIGN) */
/*                 ecore_x_icccm_transient_for_set(win->window, */
/*                                         win->transient.foreign); */
/*         else */
/*                 ecore_x_icccm_transient_for_unset(win->window); */

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_leader_set(Ewl_Window *win)
{
/*         Ewl_Embed_Window *leader; */

/*         DENTER_FUNCTION(DLEVEL_STABLE); */
/*         DCHECK_PARAM_PTR(win); */
/*         DCHECK_TYPE(win, EWL_WINDOW_TYPE); */

/*         if (win->flags & EWL_WINDOW_LEADER) */
/*                 leader = win->leader.ewl->window; */
/*         else if (win->flags & EWL_WINDOW_LEADER_FOREIGN) */
/*                 leader = win->leader.foreign; */
/*         else */
/*                 /\* according to the icccm specs a client leader */
/*                  * sets itself to the leader *\/ */
/*                 leader = win->window; */

/*         ecore_x_icccm_client_leader_set(win->window, */
/*                                         leader); */

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_raise(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        ecore_win32_window_raise(win->window);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_lower(Ewl_Window *win)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(win);
        DCHECK_TYPE(win, EWL_WINDOW_TYPE);

        ecore_win32_window_lower(win->window);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_window_geometry_set(Ewl_Window *win, int *width, int *height)
{
        DENTER_FUNCTION(DLEVEL_STABLE);

        printf ("ee_window_geometry_set\n");
        ecore_win32_window_size_get((win ? win->window : NULL),
                                        width, height);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ee_desktop_size_get(Ewl_Embed *embed, int *w, int *h)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        printf ("ee_desktop_size_get\n");
        ecore_win32_window_size_get(NULL, w, h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ee_pointer_data_new(Ewl_Embed *embed, int *data, int w, int h)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, 0);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, 0);

        DRETURN_INT((int)ecore_win32_cursor_new(embed->canvas_window,
                                                data, w, h, 0, 0), DLEVEL_STABLE);
}

static void
ee_pointer_free(Ewl_Embed *embed, int pointer)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        ecore_win32_cursor_free((void *)pointer);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/*
 * FIXME: No function to poll the active pointer yet, and we probably want to
 * add size get/set hooks as well.
 */
static int
ee_pointer_get(Ewl_Embed *embed)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(embed, 0);
        DCHECK_TYPE_RET(embed, EWL_EMBED_TYPE, 0);

        DRETURN_INT(ecore_win32_cursor_size_get(), DLEVEL_STABLE);
}

static void
ee_pointer_set(Ewl_Embed *embed, int pointer)
{
        Ecore_Win32_Cursor      *cursor;
        Ecore_Win32_Cursor_Shape shape;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(embed);
        DCHECK_TYPE(embed, EWL_EMBED_TYPE);

        switch (pointer)
          {
          case EWL_MOUSE_CURSOR_CLOCK:
            shape = ECORE_WIN32_CURSOR_SHAPE_APP_STARTING;
            break;
          case EWL_MOUSE_CURSOR_LEFT_PTR:
            shape = ECORE_WIN32_CURSOR_SHAPE_ARROW;
            break;
          case EWL_MOUSE_CURSOR_CROSSHAIR:
          case EWL_MOUSE_CURSOR_TCROSS:
            shape = ECORE_WIN32_CURSOR_SHAPE_CROSS;
            break;
          case EWL_MOUSE_CURSOR_HAND1:
          case EWL_MOUSE_CURSOR_HAND2:
            shape = ECORE_WIN32_CURSOR_SHAPE_HAND;
            break;
          case EWL_MOUSE_CURSOR_QUESTION_ARROW:
            shape = ECORE_WIN32_CURSOR_SHAPE_HELP;
            break;
          case EWL_MOUSE_CURSOR_XTERM:
            shape = ECORE_WIN32_CURSOR_SHAPE_I_BEAM;
            break;
          case EWL_MOUSE_CURSOR_X:
            shape = ECORE_WIN32_CURSOR_SHAPE_NO;
            break;
          case EWL_MOUSE_CURSOR_FLEUR:
            shape = ECORE_WIN32_CURSOR_SHAPE_SIZE_ALL;
            break;
          case EWL_MOUSE_CURSOR_BOTTOM_LEFT_CORNER:
          case EWL_MOUSE_CURSOR_TOP_RIGHT_CORNER:
            shape = ECORE_WIN32_CURSOR_SHAPE_SIZE_NESW;
            break;
          case EWL_MOUSE_CURSOR_TOP_SIDE:
          case EWL_MOUSE_CURSOR_BOTTOM_SIDE:
          case EWL_MOUSE_CURSOR_SB_V_DOUBLE_ARROW:
            shape = ECORE_WIN32_CURSOR_SHAPE_SIZE_NS;
            break;
          case EWL_MOUSE_CURSOR_BOTTOM_RIGHT_CORNER:
          case EWL_MOUSE_CURSOR_TOP_LEFT_CORNER:
            shape = ECORE_WIN32_CURSOR_SHAPE_SIZE_NWSE;
            break;
          case EWL_MOUSE_CURSOR_RIGHT_SIDE:
          case EWL_MOUSE_CURSOR_LEFT_SIDE:
          case EWL_MOUSE_CURSOR_SB_H_DOUBLE_ARROW:
            shape = ECORE_WIN32_CURSOR_SHAPE_SIZE_WE;
            break;
          case EWL_MOUSE_CURSOR_CENTER_PTR:
            shape = ECORE_WIN32_CURSOR_SHAPE_UP_ARROW;
            break;
          case EWL_MOUSE_CURSOR_WATCH:
            shape = ECORE_WIN32_CURSOR_SHAPE_WAIT;
            break;
          default:
            shape = ECORE_WIN32_CURSOR_SHAPE_ARROW;
            break;
          }
        cursor = ecore_win32_cursor_shape_get(shape);
        ecore_win32_window_cursor_set(embed->canvas_window, cursor);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static int
ewl_ev_win32_window_expose(void *data __UNUSED__, int type __UNUSED__, void *e)
{
        /*
         * Widgets don't need to know about this usually, but we still need to
         * let them know in case a widget is using a non-evas based draw method
         */
        Ecore_Win32_Event_Window_Damage *ev;
        Ewl_Event_Window_Expose event;
        Ewl_Window *window;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ev = e;

        window = ewl_window_window_find((void *)ev->window);
        if (!window)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        printf (" **** ewl_ev_win32_window_expose\n");
        event.x = ev->x;
        event.y = ev->y;
        event.w = ev->width;
        event.h = ev->height;

        ewl_engine_canvas_damage_add(EWL_EMBED(window), ev->x, ev->y, ev->width, ev->height);
        ewl_callback_call_with_event_data(EWL_WIDGET(window), EWL_CALLBACK_EXPOSE, &event);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_win32_window_configure(void *data __UNUSED__, int type __UNUSED__, void *e)
{
        /*
         * When a configure event occurs, we must update the windows geometry
         * based on the coordinates and dimensions given in the Ecore_Event.
         */
        Ecore_Win32_Event_Window_Configure *ev;
        Ewl_Window *window;
        Ewl_Embed *embed;
        int config = 0;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ev = e;

        if (!ev->window)
          printf ("ewl_ev_win32_window_configure  : ERROR !!\n");

        embed = ewl_embed_canvas_window_find((void *)ev->window);
        if (!embed)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        /*
         * Save coords and queue a configure event if the window is moved.
         */
        if (ev->x != embed->x) {
                embed->x = ev->x;
                config = 1;
        }

        if (ev->y != embed->y) {
                embed->y = ev->y;
                config = 1;
        }

        if (!ev->window)
          printf ("ewl_ev_win32_window_configure  : ERROR !!\n");

        window = ewl_window_window_find((void *)ev->window);
        /*
         * we can finish when the embed is not a window
         */
        if (!window)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        if (config)
                ewl_widget_configure(EWL_WIDGET(window));

        printf (" **** ewl_ev_win32_window_configure\n");
        /*
         * Configure events really only need to occur on resize.
         */
        if ((ewl_object_current_w_get(EWL_OBJECT(window)) != ev->width)
                        || (ewl_object_current_h_get(EWL_OBJECT(window)) != ev->height)) {

                /*
                 * Right now this is commented out, as it was preventing windows
                 * from resizing when WE were the ones setting the size
                 * (mainly on initial show of the window). It would be nice to
                 * add this back in to limit the # of window resizes occuring.
                 * (As long as it doesn't break initial size.)
                 */
                window->flags |= EWL_WINDOW_USER_CONFIGURE;
                ewl_object_geometry_request(EWL_OBJECT(window), 0, 0, ev->width,
                                                                        ev->height);
        }

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_win32_window_delete(void *data __UNUSED__, int type __UNUSED__, void *e)
{
        /*
         * Retrieve the appropriate ewl_window using the x window id that is
         * held in the eevent, and call it's handlers for a window delete event.
         */
        Ecore_Win32_Event_Window_Destroy *ev;
        Ewl_Window *window;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ev = e;

        window = ewl_window_window_find((void *)ev->window);
        if (!window)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        ewl_callback_call(EWL_WIDGET(window), EWL_CALLBACK_DELETE_WINDOW);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_win32_key_down(void *data __UNUSED__, int type __UNUSED__, void *e)
{
        Ewl_Window *window;
        Ecore_Win32_Event_Key_Down *ev;
        unsigned int key_modifiers;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ev = e;
        window = ewl_window_window_find((void *)ev->window);

        if (!window)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        key_modifiers = ewl_ev_modifiers_get();

        if (strstr(ev->keyname, "Shift_"))
                key_modifiers |= EWL_KEY_MODIFIER_SHIFT;
        else if (strstr(ev->keyname, "Control_"))
                key_modifiers |= EWL_KEY_MODIFIER_CTRL;
        else if (strstr(ev->keyname, "Alt_"))
                key_modifiers |= EWL_KEY_MODIFIER_ALT;
        else if (strstr(ev->keyname, "Mod_"))
                key_modifiers |= EWL_KEY_MODIFIER_MOD;
        else if (strstr(ev->keyname, "Super_"))
                key_modifiers |= EWL_KEY_MODIFIER_WIN;
        else if (strstr(ev->keyname, "Hyper_"))
                key_modifiers |= EWL_KEY_MODIFIER_WIN;
        else if (!ev->keycompose || iscntrl(*ev->keycompose))
                ewl_embed_key_down_feed(EWL_EMBED(window), ev->keyname,
                                                        key_modifiers);
        else
                ewl_embed_key_down_feed(EWL_EMBED(window), ev->keycompose,
                                                        key_modifiers);

        ewl_ev_modifiers_set(key_modifiers);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_win32_key_up(void *data __UNUSED__, int type __UNUSED__, void *e)
{
        Ewl_Window *window;
        Ecore_Win32_Event_Key_Up *ev;
        unsigned int key_modifiers;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ev = e;

        window = ewl_window_window_find((void *)ev->window);
        if (!window)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        key_modifiers = ewl_ev_modifiers_get();

        if (strstr(ev->keyname, "Shift_"))
                key_modifiers &= ~EWL_KEY_MODIFIER_SHIFT;
        else if (strstr(ev->keyname, "Control_"))
                key_modifiers &= ~EWL_KEY_MODIFIER_CTRL;
        else if (strstr(ev->keyname, "Alt_"))
                key_modifiers &= ~EWL_KEY_MODIFIER_ALT;
        else if (strstr(ev->keyname, "Mod_"))
                key_modifiers &= ~EWL_KEY_MODIFIER_MOD;
        else if (strstr(ev->keyname, "Super_"))
                key_modifiers &= ~EWL_KEY_MODIFIER_WIN;
        else if (strstr(ev->keyname, "Hyper_"))
                key_modifiers &= ~EWL_KEY_MODIFIER_WIN;
        else if (!ev->keycompose || iscntrl(*ev->keycompose))
                ewl_embed_key_up_feed(EWL_EMBED(window), ev->keyname,
                                                        key_modifiers);
        else
                ewl_embed_key_up_feed(EWL_EMBED(window), ev->keycompose,
                                                        key_modifiers);

        ewl_ev_modifiers_set(key_modifiers);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_win32_mouse_down(void *data __UNUSED__, int type __UNUSED__, void *e)
{
        int clicks = 1;
        Ewl_Window *window;
        Ecore_Win32_Event_Mouse_Button_Down *ev;
        unsigned int key_modifiers;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ev = e;

        window = ewl_window_window_find((void *)ev->window);
        if (!window)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        if (ev->double_click)
                clicks = 2;
        if (ev->triple_click)
                clicks = 3;

        key_modifiers = ewl_ev_modifiers_get();
        ewl_embed_mouse_down_feed(EWL_EMBED(window), ev->button, clicks,
                                                ev->x, ev->y, key_modifiers);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_win32_mouse_up(void *data __UNUSED__, int type __UNUSED__, void *e)
{
        Ewl_Window *window;
        Ecore_Win32_Event_Mouse_Button_Up *ev;
        unsigned int key_modifiers;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ev = e;

        window = ewl_window_window_find((void *)ev->window);
        if (!window)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        key_modifiers = ewl_ev_modifiers_get();

        ewl_embed_mouse_up_feed(EWL_EMBED(window), ev->button, ev->x,
                                                ev->y, key_modifiers);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_win32_mouse_move(void *data __UNUSED__, int type __UNUSED__, void *e)
{
        Ewl_Window *window;
        Ecore_Win32_Event_Mouse_Move *ev;
        unsigned int key_modifiers;

        DENTER_FUNCTION(DLEVEL_STABLE);

        ev = e;

        window = ewl_window_window_find((void *)ev->window);
        if (!window)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        key_modifiers = ewl_ev_modifiers_get();
        ewl_embed_mouse_move_feed(EWL_EMBED(window), ev->x, ev->y,
                                                        key_modifiers);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_win32_mouse_out(void *data __UNUSED__, int type __UNUSED__, void *e)
{
        Ewl_Window *window;
        Ecore_Win32_Event_Mouse_Out *ev = e;
        unsigned int key_modifiers;

        DENTER_FUNCTION(DLEVEL_STABLE);

        window = ewl_window_window_find((void *)ev->window);
        if (!window)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        key_modifiers = ewl_ev_modifiers_get();
        ewl_embed_mouse_out_feed(EWL_EMBED(window), ev->x, ev->y,
                                                key_modifiers);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_win32_mouse_wheel(void *data __UNUSED__, int type __UNUSED__, void *e)
{
        Ewl_Window *window;
        Ecore_Win32_Event_Mouse_Wheel *ev = e;
        unsigned int key_modifiers;

        DENTER_FUNCTION(DLEVEL_STABLE);

        window = ewl_window_window_find((void *)ev->window);
        if (!window)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        key_modifiers = ewl_ev_modifiers_get();
        ewl_embed_mouse_wheel_feed(EWL_EMBED(window), ev->x, ev->y,
                                        ev->z, ev->direction, key_modifiers);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}


static int
ewl_ev_win32_focus_in(void *data __UNUSED__, int type __UNUSED__, void *e)
{
        Ewl_Window *window;
        Ecore_Win32_Event_Window_Focus_In *ev = e;

        DENTER_FUNCTION(DLEVEL_STABLE);

        window = ewl_window_window_find((void *)ev->window);
        if (!window)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        ewl_callback_call(EWL_WIDGET(window), EWL_CALLBACK_FOCUS_IN);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

static int
ewl_ev_win32_focus_out(void *data __UNUSED__, int type __UNUSED__, void *e)
{
        Ewl_Window *window;
        Ecore_Win32_Event_Window_Focus_Out *ev = e;

        DENTER_FUNCTION(DLEVEL_STABLE);

        window = ewl_window_window_find((void *)ev->window);
        if (!window)
                DRETURN_INT(TRUE, DLEVEL_STABLE);

        ewl_callback_call(EWL_WIDGET(window), EWL_CALLBACK_FOCUS_OUT);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}
