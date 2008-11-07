#ifndef __ENNA_SLIDESHOW_H__
#define __ENNA_SLIDESHOW_H__

#include "enna.h"

Evas_Object *enna_slideshow_add(Evas * evas);
void enna_slideshow_image_append(Evas_Object *obj, const char *filename);
int enna_slideshow_next(void *data);
int enna_slideshow_prev(void *data);
void enna_slideshow_play(void *data);
#endif
