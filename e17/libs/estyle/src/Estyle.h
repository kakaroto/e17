#ifndef _ESTYLE_H
#define _ESTYLE_H

#include <Ewd.h>
#include <Edb.h>
#include <Evas.h>


/*
 * The estyle holds all information necessary for display and layout of the text
 */
typedef struct _estyle Estyle;



#ifdef __cplusplus
extern "C" {
#endif


/*
 * Constructor/destructor
 */
	Estyle *estyle_new(Evas evas, char *text, char *style);
	void estyle_free(Estyle * es);

/*
 * Visibility modifiers
 */
	void estyle_show(Estyle * es);
	void estyle_hide(Estyle * es);

/*
 * Content and appearance manipulators
 */
	void estyle_move(Estyle * es, int x, int y);

	Evas_Object estyle_get_clip(Estyle * es);
	void estyle_set_clip(Estyle * es, Evas_Object clip);

	char *estyle_get_text(Estyle * es);
	void estyle_set_text(Estyle * es, char *text);

	char *estyle_get_font(Estyle * es);
	int estyle_get_font_size(Estyle * es);
	void estyle_set_font(Estyle * es, char *name, int size);

	int estyle_get_layer(Estyle * es);
	void estyle_set_layer(Estyle * es, int layer);

	char *estyle_get_style(Estyle * es);
	void estyle_set_style(Estyle * es, char *name);

	void estyle_get_color(Estyle * es, int *r, int *g, int *b, int *a);
	void estyle_set_color(Estyle * es, int r, int g, int b, int a);

	void estyle_lookup_color_db(char *name, int *r, int *g, int *b,
				    int *a);
	void estyle_set_color_db(Estyle * es, char *name);

/*
 * Geometry querying
 */
	inline int estyle_length(Estyle * es);
	void estyle_geometry(Estyle * es, int *x, int *y, int *w, int *h);
	void estyle_text_at(Estyle * es, int index, int *char_x,
			    int *char_y, int *char_w, int *char_h);
	int estyle_text_at_position(Estyle * es, int x, int y, int *char_x,
				    int *char_y, int *char_w, int *char_h);

/*
 * Fixing the reported geometry to set values.
 */
	inline int estyle_fixed(Estyle * es);
	void estyle_fix_geometry(Estyle * es, int x, int y, int w, int h);
	void estyle_unfix_geometry(Estyle * es);

/*
 * Joining and splitting estyles.
 */
	Estyle *estyle_split(Estyle * es, unsigned int index);
	int estyle_merge(Estyle * es1, Estyle * es2);

/*
 * Callbacks.
 */
	void estyle_callback_add(Estyle * es, Evas_Callback_Type callback,
				 void (*func) (void *_data, Estyle * _es,
					       int _b, int _x, int _y),
				 void *data);
/*
void estyle_callback_add(Evas evas, Estyle *es, Evas_Callback_Type callback,
     void (*func) (void *_data,  Estyle *es, int _b, int _x, int _y))
*/

#ifdef __cplusplus
}
#endif
#endif
