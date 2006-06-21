#ifndef EWL_EMBED_H
#define EWL_EMBED_H

/**
 * @addtogroup Ewl_Embed Ewl_Embed: A Container for Displaying on an Evas
 * Defines the Ewl_Embed class to provide EWL with the ability to work with an
 * evas.
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
 * The embed structure is mostly a container for holding widgets and a
 * wrapper evas smart object.
 */
typedef struct Ewl_Embed Ewl_Embed;

/**
* Typedef and abstract out embedded evas windows
*/
typedef void *Ewl_Embed_Evas_Window;

/**
 * @def EWL_EMBED(widget)
 * @brief Typecast a pointer to an Ewl_Embed pointer.
 */
#define EWL_EMBED(widget) ((Ewl_Embed *) widget)

/**
* @def EWL_EMBED_EVAS_WINDOW(window)
* @brief Typecast a pointer to an Evas_Window
*/
#define EWL_EMBED_EVAS_WINDOW(window) ((Ewl_Embed_Evas_Window *)window)

/**
 * @brief The class inheriting from Ewl_Container that acts as a top level
 * widget for interacting with the evas.
 */
struct Ewl_Embed
{
	Ewl_Overlay     overlay;     /**< Inherits from the Ewl_Overlay class */

	Evas           *evas;        /**< Evas where drawing takes place. */
	void           *evas_window; /**< The window holding the evas. */

	Evas_Object    *smart;       /**< Manipulate Ewl_Embed from evas */
	Evas_Object    *ev_clip;     /**< Clip box to receive evas events */
	Ecore_DList    *tab_order;   /**< Order of widgets to send focus */

	Ecore_Hash     *obj_cache;  /**< Hash of object queues for reuse */

	int             focus;       /**< Indicates if it receives focus */

	struct
	{
		Ewl_Widget *clicked; /**< Last clicked widget */
		Ewl_Widget *focused; /**< Last focused widget */
		Ewl_Widget *mouse_in; /**< Last widget to receive a mouse_in */
	} last;			     /**< Collection of widgets to last receive events */

        int             x; /**< Screen relative horizontal position of window */
        int             y; /**< Screen relative vertical position of window */

	Ewl_Widget *dnd_widget;	     /**< The current DND widget */
};

Ewl_Widget     *ewl_embed_new(void);
int             ewl_embed_init(Ewl_Embed *emb);
void            ewl_embed_shutdown(void);
Evas_Object    *ewl_embed_evas_set(Ewl_Embed *emb, Evas *evas,
				   Ewl_Embed_Evas_Window *evas_window);

void            ewl_embed_focus_set(Ewl_Embed *embed, int f);
int             ewl_embed_focus_get(Ewl_Embed *embed);

void		ewl_embed_active_set(Ewl_Embed *embed, unsigned int act);
Ewl_Embed      *ewl_embed_active_embed_get(void);

void            ewl_embed_key_down_feed(Ewl_Embed *embed, const char *keyname,
					unsigned int modifiers);
void            ewl_embed_key_up_feed(Ewl_Embed *embed, const char *keyname,
				      unsigned int modifiers);
void            ewl_embed_mouse_down_feed(Ewl_Embed *embed, int b, int clicks,
					  int x, int y, unsigned int modifiers);
void            ewl_embed_mouse_up_feed(Ewl_Embed *embed, int b, int x,
					int y, unsigned int modifiers);
void            ewl_embed_mouse_move_feed(Ewl_Embed *embed, int x, int y,
					  unsigned int modifiers);

void		ewl_embed_dnd_position_feed(Ewl_Embed *embed, int x, int y,int*,int*,int*,int*);
void		ewl_embed_dnd_drop_feed(Ewl_Embed* , int x, int y, int internal);

void            ewl_embed_mouse_out_feed(Ewl_Embed *embed, int x, int y,
					 unsigned int modifiers);
void            ewl_embed_mouse_wheel_feed(Ewl_Embed *embed, int x, int y,
					   int z, int dir, unsigned int mods);

void            ewl_embed_font_path_add(char *path);
Ewl_Embed      *ewl_embed_evas_window_find(Ewl_Embed_Evas_Window *win);
Ewl_Embed      *ewl_embed_widget_find(Ewl_Widget *w);
void            ewl_embed_object_cache(Ewl_Embed *e, Evas_Object *obj);
Evas_Object    *ewl_embed_object_request(Ewl_Embed *e, char *type);

void            ewl_embed_tab_order_append(Ewl_Embed *e, Ewl_Widget *w);
void		ewl_embed_tab_order_prepend(Ewl_Embed *e, Ewl_Widget *w);
void		ewl_embed_tab_order_insert(Ewl_Embed *e, Ewl_Widget *w, 
					   unsigned int idx);
void		ewl_embed_tab_order_insert_before(Ewl_Embed *e, Ewl_Widget *w,
						 Ewl_Widget *before);
void		ewl_embed_tab_order_insert_after(Ewl_Embed *e, Ewl_Widget *w,
						 Ewl_Widget *after);
void            ewl_embed_tab_order_remove(Ewl_Embed *e, Ewl_Widget *w);
void            ewl_embed_tab_order_next(Ewl_Embed *e);
void		ewl_embed_tab_order_previous(Ewl_Embed *e);

void		ewl_embed_focused_widget_set(Ewl_Embed *e, Ewl_Widget *w);
Ewl_Widget     *ewl_embed_focused_widget_get(Ewl_Embed *e);
void		ewl_embed_info_widgets_cleanup(Ewl_Embed *e, Ewl_Widget *w);

void            ewl_embed_coord_to_screen(Ewl_Embed *e, int xx, int yy,
					  int *x, int *y);

void		ewl_embed_mouse_cursor_set(Ewl_Widget *w);
void            ewl_embed_position_get(Ewl_Embed *e, int *x, int *y);
void            ewl_embed_window_position_get(Ewl_Embed *e, int *x, int *y);
void            ewl_embed_freeze(Ewl_Embed *e);
void            ewl_embed_thaw(Ewl_Embed *e);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_embed_realize_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_embed_unrealize_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_embed_configure_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_embed_focus_out_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_embed_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif
