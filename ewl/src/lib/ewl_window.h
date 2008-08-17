/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_WINDOW_H
#define EWL_WINDOW_H

/**
 * @addtogroup Ewl_Window Ewl_Window: A Container for Displaying in a New Window
 * Defines the Ewl_Window class which extends the Ewl_Embed class by
 * creating it's own window and evas.
 *
 * @remarks Inherits from Ewl_Embed.
 * @if HAVE_IMAGES
 * @image html Ewl_Window_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /window/file
 * @themekey /window/group
 */

/**
 * @def EWL_WINDOW_TYPE
 * The type name for the Ewl_Window widget
 */
#define EWL_WINDOW_TYPE "window"

/**
 * @def EWL_WINDOW_IS(w)
 * Returns TRUE if the widget is an Ewl_Window, FALSE otherwise
 */
#define EWL_WINDOW_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_WINDOW_TYPE))

/**
 * The window structure is mostly a container for holding widgets and a
 * wrapper around the xlib window.
 */
typedef struct Ewl_Window Ewl_Window;

/**
 * @def EWL_WINDOW(win)
 * Typecasts a pointer to an Ewl_Window pointer.
 */
#define EWL_WINDOW(win) ((Ewl_Window *) win)

/**
 * @brief Inherits from Ewl_Embed class to create it's own window and canvas for drawing,
 * sizing and positioning.
 */
struct Ewl_Window
{
        Ewl_Embed embed; /**< Inherits from the Ewl_Embed class */

        void *window; /**< Provides a window for drawing */
        union {
                Ewl_Window *ewl;
                Ewl_Embed_Window *foreign;
        } transient; /**< Window to be transient for */
        union {
                Ewl_Window *ewl;
                Ewl_Embed_Window *foreign;
        } leader; /**< the leader of the window group */

        char *title; /**< The current title on the provided window */
        char *name; /**< Current name on the provided window */
        char *classname; /**< Current class on the provided window */

        Ewl_Window_Flags flags; /**< Flags indicating window properties */
        Ewl_Window_Flags old_flags; /**< Flags used on our last state set call */
};

extern unsigned int EWL_CALLBACK_EXPOSE; /**< Triggered when the window needs redrawing */
extern unsigned int EWL_CALLBACK_DELETE_WINDOW; /**< The window is being closed */

Ewl_Widget              *ewl_window_new(void);
int                      ewl_window_init(Ewl_Window *win);
Ewl_Window              *ewl_window_window_find(void *window);
void                     ewl_window_title_set(Ewl_Window *win,
                                        const char *title);
const char              *ewl_window_title_get(Ewl_Window *win);
void                     ewl_window_name_set(Ewl_Window *win, const char *name);
const char              *ewl_window_name_get(Ewl_Window *win);
void                     ewl_window_class_set(Ewl_Window *win,
                                        const char *classname);
const char              *ewl_window_class_get(Ewl_Window *win);
void                     ewl_window_borderless_set(Ewl_Window *win,
                                        unsigned int border);
unsigned int             ewl_window_borderless_get(Ewl_Window *win);
void                     ewl_window_dialog_set(Ewl_Window *win, int dialog);
int                      ewl_window_dialog_get(Ewl_Window *win);
void                     ewl_window_fullscreen_set(Ewl_Window *win,
                                        unsigned int fullscreen);
unsigned int             ewl_window_fullscreen_get(Ewl_Window *win);
void                     ewl_window_skip_taskbar_set(Ewl_Window *win,
                                        unsigned int skip);
unsigned int             ewl_window_skip_taskbar_get(Ewl_Window *win);
void                     ewl_window_skip_pager_set(Ewl_Window *win,
                                        unsigned int skip);
unsigned int             ewl_window_skip_pager_get(Ewl_Window *win);
void                     ewl_window_attention_demand(Ewl_Window *win);
void                     ewl_window_urgent_set(Ewl_Window *win,
                                        unsigned int urgent);
unsigned int             ewl_window_urgent_get(Ewl_Window *win);
void                     ewl_window_move(Ewl_Window *win, int x, int y);
void                     ewl_window_raise(Ewl_Window *win);
void                     ewl_window_lower(Ewl_Window *win);
void                     ewl_window_transient_for(Ewl_Window *win,
                                        Ewl_Window *forwin);
void                     ewl_window_transient_for_foreign(Ewl_Window *win,
                                        Ewl_Embed_Window *forwin);
void                     ewl_window_leader_set(Ewl_Window *win,
                                        Ewl_Window *leader);
void                     ewl_window_leader_foreign_set(Ewl_Window *win,
                                        Ewl_Embed_Window *leader);
Ewl_Window              *ewl_window_leader_get(Ewl_Window *win);
Ewl_Embed_Window        *ewl_window_leader_foreign_get(Ewl_Window *win);
int                      ewl_window_modal_get(Ewl_Window *win);
void                     ewl_window_modal_set(Ewl_Window *win, int modal);
void                     ewl_window_keyboard_grab_set(Ewl_Window *win, int grab);
int                      ewl_window_keyboard_grab_get(Ewl_Window *win);
void                     ewl_window_pointer_grab_set(Ewl_Window *win, int grab);
int                      ewl_window_pointer_grab_get(Ewl_Window *win);
void                     ewl_window_override_set(Ewl_Window *win, int override);
int                      ewl_window_override_get(Ewl_Window *win);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_window_cb_realize(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_window_cb_postrealize(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_window_cb_realize_parent(Ewl_Widget *w, void *ev_data,
                                                void *user_data);
void ewl_window_cb_unrealize(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_window_cb_show(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_window_cb_expose(Ewl_Widget *w, void *ev, void *user_data);
void ewl_window_cb_hide(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_window_cb_destroy(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_window_cb_configure(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif
