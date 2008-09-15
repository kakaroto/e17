#ifndef __ENNA_SLIDESHOW_H__
#define __ENNA_SLIDESHOW_H__

#include "enna.h"

EAPI Evas_Object *enna_slideshow_add(Evas * evas);
EAPI void enna_slideshow_image_append(Evas_Object *obj, const char *filename);
EAPI int enna_slideshow_next(void *data);
EAPI int enna_slideshow_prev(void *data);
EAPI void enna_slideshow_play(void *data);
#endif
