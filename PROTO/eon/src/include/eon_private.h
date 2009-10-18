/* EON - Canvas and Toolkit library
 * Copyright (C) 2008-2009 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
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
#define EON_TYPE_DOCUMENT "eon"
#define EON_TYPE_CANVAS "canvas"
#define EON_TYPE_ANIMATION "animation_base"
#define EON_TYPE_ANIMATION_KEY "key"
#define EON_TYPE_ANIMATION_BASIC "anim"
#define EON_TYPE_ANIMATION_MATRIX "animMatrix"
#define EON_TYPE_EXTERNAL "external"
#define EON_TYPE_SCRIPT "script"
#define EON_TYPE_STYLE "style"
#define EON_TYPE_STYLE_APPLIER "style_applier"
#define EON_TYPE_TRIGGER "trigger"
#define EON_TYPE_SETTER "setter"

#define EON_TYPE_SHAPE "shape"
#define EON_TYPE_SHAPE_SQUARE "shape_square"
#define EON_TYPE_TEXT "text"
#define EON_TYPE_RECT "rect"
#define EON_TYPE_POLYGON "polygon"
#define EON_TYPE_CIRCLE "circle"

#define EON_TYPE_PAINT "paint"
#define EON_TYPE_PAINT_SQUARE "paint_square"
#define EON_TYPE_COMPOUND "compound"
#define EON_TYPE_COMPOUND_LAYER "layer"
#define EON_TYPE_IMAGE "image"
#define EON_TYPE_TRANSITION "transition"
#define EON_TYPE_HSWITCH "hswitch"
#define EON_TYPE_FADE "fade"
#define EON_TYPE_CHECKER "checker"
#define EON_TYPE_STRIPES "stripes"

#define EON_TYPE_FILTER "filter"
#define EON_TYPE_FILTER_EFFECT "Eon_Filter_Effect"

typedef Ekeko_Type * (*Eon_Type_Constructor)(void);

/* Parser */
void eon_parser_init(void);
void eon_parser_shutdown(void);
void eon_parser_register(const char *name, Eon_Parser *p);

/* Engine */
typedef void * Eon_Engine_Surface;
void eon_engine_init(void);
void eon_engine_shutdown(void);
void eon_engine_register(const char *name, Eon_Engine *);
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
void * eon_canvas_engine_data_get(Eon_Canvas *c);

/* Shape */
Eina_Bool eon_shape_appendable(void *instance, void *child);

/* Coord */
void eon_coord_length_calculate(Eon_Coord *sl, int plength, int *l);
void eon_coord_calculate(Eon_Coord *sc, int pc, int plength, int *c);
void eon_coord_change(const Ekeko_Object *o, Eon_Coord *dst, Eon_Coord *curr,
		Eon_Coord *prev, int coord, int length, Ekeko_Object *parent,
		const char *cevent, const char *levent, Event_Listener el);
void eon_coord_length_change(const Ekeko_Object *o, Eon_Coord *dst, Eon_Coord *curr,
		Eon_Coord *prev, int length, Ekeko_Object *parent,
		const char *levent, Event_Listener el);

/* Paint */
void eon_paint_change(Eon_Paint *p);
void * eon_paint_engine_data_get(Eon_Paint *p);
Eon_Canvas * eon_paint_canvas_get(Eon_Paint *p);
Eon_Canvas * eon_paint_canvas_topmost_get(Eon_Paint *p);
Eina_Bool eon_paint_appendable(Ekeko_Object *p, Ekeko_Object *child);
void eon_paint_geometry_set(Eon_Paint *p, Eina_Rectangle *rect);
void eon_paint_geometry_get(Eon_Paint *p, Eina_Rectangle *rect);
void eon_paint_style_coords_get(Eon_Paint *p, Eon_Paint *rel, int *x, int *y,
		int *w, int *h);
void eon_paint_style_inverse_matrix_get(Eon_Paint *p, Eon_Paint *rel,
		Enesim_Matrix *m);
void eon_paint_inverse_matrix_get(Eon_Paint *p, Enesim_Matrix *m);
Eon_Document * eon_paint_document_get(Eon_Paint *p);


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

/* Style */

/* Circle */
void eon_circle_style_coords_get(Eon_Circle *c, Eon_Paint *p,
		int *cx, int *cy, float *rad);

/* Paint Square */
void eon_paint_square_style_coords_get(Eon_Paint_Square *s, Eon_Paint *p, int *x, int *y,
		int *w, int *h);
/* Shape Square */
void eon_shape_square_style_coords_get(Eon_Shape_Square *s, Eon_Paint *p, int *x, int *y,
		int *w, int *h);

/* Core */
void eon_type_register(Ekeko_Type *t, char *name);
void eon_type_unregister(Ekeko_Type *t);
Ekeko_Type * eon_type_get(char *name);

#endif
