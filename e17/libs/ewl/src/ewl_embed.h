
#ifndef __EWL_EMBED_H__
#define __EWL_EMBED_H__

/*
 * The embed structure is mostly a container for holding widgets and a
 * wrapper evas smart object.
 */
typedef struct _ewl_embed Ewl_Embed;

struct _ewl_embed
{
	Ewl_Container   widget;

	/*
	 * The following fields allow for drawing the widgets
	 */
	Evas           *evas;
	Window         evas_window;

	/*
	 * This smart object can be used by a lower level evas access library
	 * to manipulate the size and position of the embedded container.
	 */
	Evas_Object *smart;
};

#define EWL_EMBED(widget) ((Ewl_Embed *) widget)

Ewl_Widget     *ewl_embed_new();
int             ewl_embed_init(Ewl_Embed * win);
Evas_Object    *ewl_embed_set_evas(Ewl_Embed *emb, Evas *evas);
void            ewl_embed_font_path_add(char *path);
Ewl_Embed      *ewl_embed_find_by_evas_window(Window win);
Ewl_Embed      *ewl_embed_find_by_widget(Ewl_Widget * w);

#endif				/* __EWL_EMBED_H__ */
