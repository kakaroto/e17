#ifndef _ENNA_EMUSIC_H
#define _ENNA_EMUSIC_H

#include "enna.h"

EAPI Evas_Object   *enna_emusic_add(Evas * evas);
EAPI void           enna_emusic_process_event(Evas_Object * obj,
					      enna_event event);
EAPI int            enna_emusic_load(Evas_Object * obj, char *filename);
EAPI int            enna_emusic_cdda_load(Evas_Object * obj,
					  unsigned int selected,
					  unsigned int nb_tracks, char *device);
EAPI int            enna_emusic_play(Evas_Object * obj);
EAPI int            enna_emusic_pause(Evas_Object * obj);
EAPI int            enna_emusic_stop(Evas_Object * obj);
EAPI int            enna_emusic_prev(Evas_Object * obj);
EAPI int            enna_emusic_next(Evas_Object * obj);
EAPI int            enna_emusic_fastforward(Evas_Object * obj);
EAPI int            enna_emusic_rewind(Evas_Object * obj);
#endif
