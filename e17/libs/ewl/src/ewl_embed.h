
#ifndef __EWL_EMBED_H__
#define __EWL_EMBED_H__

/**
 * @defgroup Ewl_Embed A Container for Displaying on an Evas
 * Defines the Ewl_Embed class to provide EWL with the ability to work with an
 * evas.
 *
 * @{
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
	Ewl_Container   container; /**< Inherits from the Ewl_Container class */

	Evas           *evas; /**< Evas where drawing takes place. */
	Window         evas_window; /**< The window holding the evas. */

	Evas_Object *smart; /**< Object to manipulate Ewl_Embed from evas */
};

Ewl_Widget     *ewl_embed_new();
int             ewl_embed_init(Ewl_Embed * win);
Evas_Object    *ewl_embed_set_evas(Ewl_Embed *emb, Evas *evas);
void            ewl_embed_font_path_add(char *path);
Ewl_Embed      *ewl_embed_find_by_evas_window(Window win);
Ewl_Embed      *ewl_embed_find_by_widget(Ewl_Widget * w);

/**
 * @}
 */

#endif				/* __EWL_EMBED_H__ */
