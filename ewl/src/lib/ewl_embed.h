/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_EMBED_H
#define EWL_EMBED_H

/**
 * @addtogroup Ewl_Embed Ewl_Embed: A Container for Displaying on an canvas
 * Defines the Ewl_Embed class to provide EWL with the ability to work with a
 * canvas.
 *
 * @remarks Inherits from Ewl_Cell.
 * @if HAVE_IMAGES
 * @image html Ewl_Embed_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @themekey /embed/file
 * @themekey /embed/group
 */

/**
 * @def EWL_EMBED_TYPE
 * The type name for the Ewl_Embed widget
 */
#define EWL_EMBED_TYPE "embed"

/**
 * @def EWL_EMBED_IS(w)
 * Returns TRUE if the widget is an Ewl_Embed, FALSE otherwise
 */
#define EWL_EMBED_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_EMBED_TYPE))

/**
 * The embed structure is mostly a container for holding widgets and a
 * wrapper canvas smart object.
 */
typedef struct Ewl_Embed Ewl_Embed;

/**
* Typedef and abstract out embedded canvas windows
*/
typedef void *Ewl_Embed_Window;

/**
 * @def EWL_EMBED(widget)
 * @brief Typecast a pointer to an Ewl_Embed pointer.
 */
#define EWL_EMBED(widget) ((Ewl_Embed *) widget)

/**
* @def EWL_EMBED_WINDOW(window)
* @brief Typecast a pointer to an canvas window
*/
#define EWL_EMBED_WINDOW(window) ((Ewl_Embed_Window *)window)

/**
 * @brief Inherits from Ewl_Cell that acts as a top level widget
 * for interacting with the canvas.
 */
struct Ewl_Embed
{
        Ewl_Cell cell;                 /**< Inherits from the Ewl_Cell class */

        const char *engine_name; /**< Name of current engine used */
        void *engine;                 /**< The display engine */

        void *canvas;                 /**< Canvas where drawing takes place. */
        void *canvas_window;        /**< The window holding the canvas. */
        int cursor;                /**< Id for current cursor in use */

        void *smart;         /**< Manipulate Ewl_Embed from canvas */
        void *ev_clip;         /**< Clip box to receive canvas events */
        Ecore_DList *tab_order; /**< Order of widgets to send focus */

        Ecore_Hash  *obj_cache; /**< Hash of object queues for reuse */

        struct
        {
                Ewl_Widget *clicked; /**< Last clicked widget */
                Ewl_Widget *focused; /**< Last focused widget */
                Ewl_Widget *mouse_in; /**< Last widget to receive a mouse_in */
                Ewl_Widget *drop_widget; /**< The current DND drop target */
                Ewl_Widget *drag_widget; /**< The current DND drag source */
        } last;        /**< Collection of widgets to last receive events */

        int x; /**< Screen relative horizontal position of window */
        int y; /**< Screen relative vertical position of window */

        int dnd_count;        /**< DND aware widget count */

        Ewl_Dnd_Types dnd_types;        /**< The dnd type */
        Ewl_Widget *dnd_last_position;        /**< The last dnd position */

        unsigned char focus:1;         /**< Indicates if it receives focus */
        unsigned char render:1;        /**< Indicates if the embed needs
                                            to be rendered by ewl, or if
                                            it is done elsewhere, by
                                            ecore_evas for example */
};

Ewl_Widget      *ewl_embed_new(void);
int              ewl_embed_init(Ewl_Embed *emb);
int              ewl_embed_engine_name_set(Ewl_Embed *embed, const char *engine);
const char      *ewl_embed_engine_name_get(Ewl_Embed *embed);
void             ewl_embed_shutdown(void);
void            *ewl_embed_canvas_set(Ewl_Embed *emb, void *canvas,
                                        Ewl_Embed_Window *canvas_window);

void             ewl_embed_focus_set(Ewl_Embed *embed, int f);
int              ewl_embed_focus_get(Ewl_Embed *embed);

void             ewl_embed_active_set(Ewl_Embed *embed, unsigned int act);
Ewl_Embed       *ewl_embed_active_embed_get(void);

void             ewl_embed_last_mouse_position_get(int *x, int *y);

void             ewl_embed_key_down_feed(Ewl_Embed *embed, const char *keyname,
                                        unsigned int modifiers);
void             ewl_embed_key_up_feed(Ewl_Embed *embed, const char *keyname,
                                        unsigned int modifiers);
void             ewl_embed_mouse_down_feed(Ewl_Embed *embed, int b, int clicks,
                                        int x, int y, unsigned int modifiers);
void             ewl_embed_mouse_up_feed(Ewl_Embed *embed, int b, int x,
                                        int y, unsigned int modifiers);
void             ewl_embed_mouse_move_feed(Ewl_Embed *embed, int x, int y,
                                        unsigned int modifiers);

const char      *ewl_embed_dnd_position_feed(Ewl_Embed *embed, int x, int y,
                                        int *, int *, int *, int *);
const char      *ewl_embed_dnd_drop_feed(Ewl_Embed* embed, int x, int y,
                                        int internal);
void             ewl_embed_dnd_data_received_feed(Ewl_Embed* embed, char *type,
                                        void *data, unsigned int len,
                                        unsigned int format);
void             ewl_embed_dnd_data_request_feed(Ewl_Embed* embed, void *handle,                                        char *type);

void             ewl_embed_mouse_out_feed(Ewl_Embed *embed, int x, int y,
                                        unsigned int modifiers);
void             ewl_embed_mouse_wheel_feed(Ewl_Embed *embed, int x, int y,
                                        int z, int dir, unsigned int mods);
void             ewl_embed_selection_text_clear_feed(void);

void             ewl_embed_font_path_add(char *path);
Ewl_Embed       *ewl_embed_canvas_window_find(Ewl_Embed_Window *win);
Ewl_Embed       *ewl_embed_widget_find(Ewl_Widget *w);
void             ewl_embed_object_cache(Ewl_Embed *e, void *obj);
void            *ewl_embed_object_request(Ewl_Embed *e, char *type);

void             ewl_embed_tab_order_append(Ewl_Embed *e, Ewl_Widget *w);
void             ewl_embed_tab_order_prepend(Ewl_Embed *e, Ewl_Widget *w);
void             ewl_embed_tab_order_insert(Ewl_Embed *e, Ewl_Widget *w,
                                        unsigned int idx);
void             ewl_embed_tab_order_insert_before(Ewl_Embed *e, Ewl_Widget *w,
                                        Ewl_Widget *before);
void             ewl_embed_tab_order_insert_after(Ewl_Embed *e, Ewl_Widget *w,
                                        Ewl_Widget *after);
void             ewl_embed_tab_order_remove(Ewl_Embed *e, Ewl_Widget *w);
void             ewl_embed_tab_order_next(Ewl_Embed *e);
void             ewl_embed_tab_order_previous(Ewl_Embed *e);

void             ewl_embed_focused_widget_set(Ewl_Embed *e, Ewl_Widget *w);
Ewl_Widget      *ewl_embed_focused_widget_get(Ewl_Embed *e);
void             ewl_embed_info_widgets_cleanup(Ewl_Embed *e, Ewl_Widget *w);

void             ewl_embed_coord_to_screen(Ewl_Embed *e, int xx, int yy,
                                        int *x, int *y);

void             ewl_embed_mouse_cursor_set(Ewl_Widget *w);
void             ewl_embed_position_get(Ewl_Embed *e, int *x, int *y);
void             ewl_embed_window_position_get(Ewl_Embed *e, int *x, int *y);
void             ewl_embed_desktop_size_get(Ewl_Embed *e, int *w, int *y);
void             ewl_embed_freeze(Ewl_Embed *e);
void             ewl_embed_thaw(Ewl_Embed *e);

void             ewl_embed_dnd_aware_set(Ewl_Embed *embed);
void             ewl_embed_dnd_aware_remove(Ewl_Embed *embed);

void             ewl_embed_selection_text_set(Ewl_Embed *emb, Ewl_Widget *w,
                                                const char *txt);

void             ewl_embed_render_set(Ewl_Embed *emb, unsigned int render);
unsigned int     ewl_embed_render_get(Ewl_Embed *emb);

void             ewl_embed_cache_cleanup(Ewl_Embed *emb);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_embed_cb_realize(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_embed_cb_unrealize(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_embed_cb_configure(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_embed_cb_focus_out(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_embed_cb_destroy(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif
