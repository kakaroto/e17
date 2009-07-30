/*
 * eon_private.h
 *
 *  Created on: 14-Dec-2008
 *      Author: Hisham Mardam-Bey <hisham.mardambey@gmail.com>
 */

#ifndef _EON_PRIVATE_H
#define _EON_PRIVATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
//#define EON_DEBUG

#include "Emage.h"

#define RETURN_IF(expr) if ((expr)) return
#define RETURN_NULL_IF(expr) if ((expr)) return NULL

#define OFFSET(type, mem) ((size_t) ((char *)&((type *) 0)->mem - (char *)((type *) 0)))

/* Type names */
#define EON_TYPE_DOCUMENT "Eon_Document"
#define EON_TYPE_CANVAS "Eon_Canvas"
#define EON_TYPE_ANIMATION "Eon_Animation"
#define EON_TYPE_ANIMATION_KEY "Eon_Animation_Key"
#define EON_TYPE_ANIMATION_BASIC "Eon_Animation_Basic"
#define EON_TYPE_ANIMATION_MATRIX "Eon_Animation_Matrix"
#define EON_TYPE_EXTERNAL "Eon_External"
#define EON_TYPE_SCRIPT "Eon_Script"

#define EON_TYPE_SHAPE "Eon_Shape"
#define EON_TYPE_RECT "Eon_Rect"
#define EON_TYPE_POLYGON "Eon_Polygon"
#define EON_TYPE_CIRCLE "Eon_Circle"

#define EON_TYPE_PAINT "Eon_Paint"
#define EON_TYPE_IMAGE "Eon_Image"
#define EON_TYPE_TRANSITION "Eon_Transition"
#define EON_TYPE_HSWITCH "Eon_Hswitch"
#define EON_TYPE_FADE "Eon_Fade"
#define EON_TYPE_CHECKER "Eon_Sqpattern"

#define EON_TYPE_FILTER "Eon_Filter"
#define EON_TYPE_FILTER_EFFECT "Eon_Filter_Effect"

#define EON_TYPE_ENGINE "Eon_Engine"

typedef Ekeko_Type * (*Eon_Type_Constructor)(void);

/* Parser */
void eon_parser_init(void);
void eon_parser_shutdown(void);
void eon_parser_register(const char *name, Eon_Parser *p);

/* Engine */
typedef void * Eon_Engine_Surface;
void eon_engine_init(void);
void eon_engine_shutdown(void);
void eon_engine_register(const char *name, Eon_Type_Constructor n);
Eon_Engine * eon_engine_get(const char *name);

/* Enesim engine */
Ekeko_Type * eon_engine_enesim_type_get(void);

/* SDL engine */
void engine_sdl_init(void);

/* Ekeko_Value */
void eon_value_init(void);
void eon_value_shutdown(void);

/* Document */
Eon_Engine * eon_document_engine_get(Eon_Document *d);
Etch * eon_document_etch_get(Eon_Document *d);
void eon_document_script_execute(Eon_Document *d, const char *fname, Ekeko_Object *o);
void eon_document_script_unload(Eon_Document *d, const char *file);
void eon_document_script_load(Eon_Document *d, const char *file);

/* Canvas */
Eon_Document * eon_canvas_document_get(Eon_Canvas *c);

/* Shape */
Eon_Canvas * eon_shape_canvas_get(Eon_Shape *s);
void * eon_shape_engine_data_get(Eon_Shape *s);
void eon_shape_engine_data_set(Eon_Shape *s, void *engine_data);
void eon_shape_change(Eon_Shape *s);
Eina_Bool eon_shape_appendable(void *instance, void *child);

/* Coord */
void eon_coord_change(const Ekeko_Object *o, Eon_Coord *dst, Eon_Coord *curr,
		Eon_Coord *prev, int coord, int length, Ekeko_Object *parent,
		const char *cevent, const char *levent, Event_Listener el);
void eon_coord_length_change(const Ekeko_Object *o, Eon_Coord *dst, Eon_Coord *curr,
		Eon_Coord *prev, int length, Ekeko_Object *parent,
		const char *levent, Event_Listener el);

/* Paint */
void * eon_paint_engine_data_get(Eon_Paint *p);
Eon_Canvas * eon_paint_canvas_get(Eon_Paint *p);
Eina_Bool eon_paint_appendable(Ekeko_Object *p, Ekeko_Object *child);

/* Transition */
Eina_Bool eon_transition_appendable(Ekeko_Object *t, Ekeko_Object *child);
Eina_Bool eon_transition_paint_get(Eon_Transition *t, Eon_Paint **p1, Eon_Paint **p2, float *step);

/* Image */
Eina_Bool eon_image_loaded(Eon_Image *i);
Eina_Bool eon_image_size_get(Eon_Image *i, int *w, int *h);
Enesim_Surface * eon_image_surface_get(Eon_Image *i);

/* Hswitch */
float eon_hswitch_step_get(Eon_Hswitch *hs);


/* Parser */
Eina_Bool eon_parser_coord_str_from(Eon_Coord *c, char *v);
Eina_Bool eon_parser_matrix_str_from(Enesim_Matrix *m, char *v);
Eina_Bool eon_parser_clock_str_from(Eon_Clock *c, char *v);

/* Cache */
void eon_cache_image_load(const char *file, Enesim_Surface **s,
		Enesim_Format f, Emage_Load_Callback cb, void *data,
		const char *options);


#endif
