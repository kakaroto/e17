#ifndef __EWL_EMBED_H__
#define __EWL_EMBED_H__

/**
 * @defgroup Ewl_Embed Embed: A Container for Displaying on an Evas
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
 * The embed structure is mostly a container for holding widgets and a
 * wrapper evas smart object.
 */
typedef struct Ewl_Embed Ewl_Embed;

/**
 * @def EWL_EMBED(widget)
 * @brief Typecast a pointer to an Ewl_Embed pointer.
 */
#define EWL_EMBED(widget) ((Ewl_Embed *) widget)

/**
 * @struct Ewl_Embed
 * @brief The class inheriting from Ewl_Container that acts as a top level
 * widget for interacting with the evas.
 */
struct Ewl_Embed
{
	Ewl_Overlay     overlay; /**< Inherits from the Ewl_Overlay class */

	Evas           *evas; /**< Evas where drawing takes place. */
	Ecore_X_Window  evas_window; /**< The window holding the evas. */

	Evas_Object    *smart; /**< Object to manipulate Ewl_Embed from evas */
	Ewd_List       *tab_order; /**< Order of widgets to send focus on tab */
};

Ewl_Widget     *ewl_embed_new(void);
int             ewl_embed_init(Ewl_Embed * win);
Evas_Object    *ewl_embed_set_evas(Ewl_Embed *emb, Evas *evas,
				   Ecore_X_Window evas_window);
void            ewl_embed_font_path_add(char *path);
Ewl_Embed      *ewl_embed_find_by_evas_window(Ecore_X_Window win);
Ewl_Embed      *ewl_embed_find_by_widget(Ewl_Widget * w);
void            ewl_embed_next_tab_order(Ewl_Embed *e);
void            ewl_embed_remove_tab_order(Ewl_Embed *e, Ewl_Widget *w);
void            ewl_embed_push_tab_order(Ewl_Embed *e, Ewl_Widget *w);
void            ewl_embed_coord_to_screen(Ewl_Embed *e, int xx, int yy,
					  int *x, int *y);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_embed_unrealize_cb(Ewl_Widget *w, void *ev_data, void *user_data);
void ewl_embed_destroy_cb(Ewl_Widget *w, void *ev_data, void *user_data);

/**
 * @}
 */

#endif				/* __EWL_EMBED_H__ */
