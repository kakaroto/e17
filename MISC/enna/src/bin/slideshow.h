#ifndef __ENNA_SLIDESHOW_H__
#define __ENNA_SLIDESHOW_H__

#include "enna.h"

EAPI Evas_Object   *enna_slideshow_add(Evas * evas);
EAPI void           enna_slideshow_image_append(Evas_Object *obj, const char *filename);
EAPI int            enna_slideshow_next(Evas_Object * obj);
EAPI int            enna_slideshow_prev(Evas_Object * obj);
EAPI void           enna_slideshow_play(Evas_Object * obj);
#endif
