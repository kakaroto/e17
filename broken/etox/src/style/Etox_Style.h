#ifndef _ETOX_STYLE_H
#define _ETOX_STYLE_H

#include <Edb.h>
#include <Evas.h>

#define IF_FREE(ptr) if (ptr) free(ptr); ptr = NULL;
#define FREE(ptr) free(ptr); ptr = NULL;

#define CHECK_PARAM_POINTER_RETURN(sparam, param, ret) \
     if (!(param)) \
	 { \
	    fprintf(stderr, "Fix: func: %s, param: %s\n", __FUNCTION__, sparam); \
	    return ret; \
	 }

#define CHECK_PARAM_POINTER(sparam, param) \
     if (!(param)) \
	 { \
	    fprintf(stderr, "Fix: func: %s, param: %s\n", __FUNCTION__, sparam); \
	    return; \
	 }


#ifdef __cplusplus
extern "C"
{
#endif



/*
 * Constructor/destructor
 */
Evas_Object *etox_style_new(Evas *evas, char *text, char *style);
void etox_style_gc_collect();

/*
 * Content and appearance manipulators
 */

char *etox_style_get_text(Evas_Object * obj);
void etox_style_set_text(Evas_Object * obj, char *text);

char *etox_style_get_font(Evas_Object *obj);
int etox_style_get_font_size(Evas_Object *obj);
void etox_style_set_font(Evas_Object *obj, char *name, int size);

char *etox_style_get_style(Evas_Object *obj);
void etox_style_set_style(Evas_Object *, char *name);

void etox_style_lookup_color_db(char *name, int *r, int *g, int *b, int *a);
void etox_style_set_color_db(Evas_Object *obj, char *name);

/*
 * Geometry querying
 */
inline int etox_style_length(Evas_Object *obj);
void etox_style_text_at(Evas_Object *obj, int index, Evas_Coord *char_x,
		    Evas_Coord *char_y, Evas_Coord *char_w, Evas_Coord *char_h);
int etox_style_text_at_position(Evas_Object *obj, Evas_Coord x, Evas_Coord y,
			    Evas_Coord *char_x, Evas_Coord *char_y,
			    Evas_Coord *char_w, Evas_Coord *char_h);

/*
 * Fixing the reported geometry to set values.
 */
inline int etox_style_fixed(Evas_Object *obj);
void etox_style_fix_geometry(Evas_Object *obj, Evas_Coord x, Evas_Coord y,
			 Evas_Coord w, Evas_Coord h);
void etox_style_unfix_geometry(Evas_Object *obj);

/*
 * Joining and splitting etox_styles.
 */
Evas_Object *etox_style_split(Evas_Object *obj, unsigned int index);
int etox_style_merge(Evas_Object *obj1, Evas_Object *obj2);


/*
 * Callbacks.
 */
void etox_style_callback_add(Evas_Object *obj, Evas_Callback_Type callback,
			 void (*func) (void *_data, Evas_Object * _es,
				       void *event_info),
			 void *data);
void etox_style_callback_del(Evas_Object *obj, Evas_Callback_Type callback,
			 void (*func) (void *_data, Evas_Object * _es,
				       void *event_info));

void etox_style_set_type(Evas_Object *obj, int type);
int  etox_style_get_type(Evas_Object *obj);
#ifdef __cplusplus
}
#endif
#endif
