#ifndef _ENNA_REFLECTION_H
#define _ENNA_REFLECTION_H

#include "enna.h"

EAPI Evas_Object   *enna_reflection_add(Evas * evas);
EAPI void           enna_reflection_file_set(Evas_Object * obj,
					     const char *file);
EAPI const char    *enna_reflection_file_get(Evas_Object * obj);
EAPI void           enna_reflection_smooth_scale_set(Evas_Object * obj,
						     int smooth);
EAPI int            enna_reflection_smooth_scale_get(Evas_Object * obj);
EAPI void           enna_reflection_alpha_set(Evas_Object * obj, int smooth);
EAPI int            enna_reflection_alpha_get(Evas_Object * obj);
EAPI void           enna_reflection_size_get(Evas_Object * obj, int *w, int *h);
EAPI int            enna_reflection_fill_inside_get(Evas_Object * obj);
EAPI void           enna_reflection_fill_inside_set(Evas_Object * obj,
						    int fill_inside);
EAPI void           enna_reflection_data_set(Evas_Object * obj, void *data,
					     int w, int h);
EAPI void          *enna_reflection_data_get(Evas_Object * obj, int *w, int *h);

#endif
