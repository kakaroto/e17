#ifndef _ENNA_SLIDESHOW_H
#define _ENNA_SLIDESHOW_H

#include "enna_event.h"

EAPI Evas_Object   *enna_slideshow_add(Evas * evas);

EAPI void           enna_slideshow_play(Evas_Object * obj);
EAPI void           enna_slideshow_set_filename(Evas_Object * obj,
						char *filename);
EAPI int            enna_slideshow_prev(Evas_Object * obj);
EAPI int            enna_slideshow_next(Evas_Object * obj);
EAPI void           enna_slideshow_stop(Evas_Object * obj);
EAPI void           enna_slideshow_decrease_time(Evas_Object * obj);
EAPI void           enna_slideshow_increase_time(Evas_Object * obj);
EAPI void           enna_slideshow_process_event(Evas_Object * obj,
						 enna_event event);
EAPI int            enna_slideshow_exit_cb_set(Evas_Object * obj,
					       void (exit_cb) (void *data1,
							       void *data2),
					       void *data1, void *data2);
#endif
