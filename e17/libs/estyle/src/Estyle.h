#ifndef _ESTYLE_H
#define _ESTYLE_H

#include <Edb.h>
#include <Evas.h>


#ifdef __cplusplus
extern "C"
{
#endif



/*
 * Constructor/destructor
 */
Evas_Object *estyle_new(Evas *evas, char *text, char *style);

/*
 * Content and appearance manipulators
 */

char *estyle_get_text(Evas_Object * obj);
void estyle_set_text(Evas_Object * obj, char *text);

char *estyle_get_font(Evas_Object *obj);
int estyle_get_font_size(Evas_Object *obj);
void estyle_set_font(Evas_Object *obj, char *name, int size);

char *estyle_get_style(Evas_Object *obj);
void estyle_set_style(Evas_Object *, char *name);

void estyle_lookup_color_db(char *name, int *r, int *g, int *b, int *a);
void estyle_set_color_db(Evas_Object *obj, char *name);

/*
 * Geometry querying
 */
inline int estyle_length(Evas_Object *obj);
void estyle_text_at(Evas_Object *obj, int index, Evas_Coord *char_x,
		    Evas_Coord *char_y, Evas_Coord *char_w, Evas_Coord *char_h);
int estyle_text_at_position(Evas_Object *obj, Evas_Coord x, Evas_Coord y,
			    Evas_Coord *char_x, Evas_Coord *char_y,
			    Evas_Coord *char_w, Evas_Coord *char_h);

/*
 * Fixing the reported geometry to set values.
 */
inline int estyle_fixed(Evas_Object *obj);
void estyle_fix_geometry(Evas_Object *obj, Evas_Coord x, Evas_Coord y,
			 Evas_Coord w, Evas_Coord h);
void estyle_unfix_geometry(Evas_Object *obj);

/*
 * Joining and splitting estyles.
 */
Evas_Object *estyle_split(Evas_Object *obj, unsigned int index);
int estyle_merge(Evas_Object *obj1, Evas_Object *obj2);


/*
 * Callbacks.
 */
void estyle_callback_add(Evas_Object *obj, Evas_Callback_Type callback,
			 void (*func) (void *_data, Evas_Object * _es,
				       void *event_info),
			 void *data);
void estyle_callback_del(Evas_Object *obj, Evas_Callback_Type callback,
			 void (*func) (void *_data, Evas_Object * _es,
				       void *event_info));

void estyle_set_type(Evas_Object *obj, int type);
int  estyle_get_type(Evas_Object *obj);
#ifdef __cplusplus
}
#endif
#endif
