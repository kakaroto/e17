#ifndef _ENNA_EMOTION_H
#define _ENNA_EMOTION_H

#include "enna.h"
#include "enna_volume_manager.h"

EAPI Evas_Object   *enna_emotion_add(Evas * evas);
EAPI void           enna_emotion_process_event(Evas_Object * obj,
					       enna_event event);
EAPI void           enna_emotion_focus_set(Evas_Object * obj,
					   unsigned int focus);
EAPI int            enna_emotion_load(Evas_Object * obj, char *filename);
EAPI int            enna_emotion_load_dvd(Evas_Object * obj, char *filename,
					  Volume * v);
EAPI void           enna_emotion_fullscreen_toggle(Evas_Object * obj);
EAPI int            enna_emotion_play(Evas_Object * obj);
EAPI int            enna_emotion_pause(Evas_Object * obj);
EAPI int            enna_emotion_stop(Evas_Object * obj);
EAPI int            enna_emotion_prev(Evas_Object * obj);
EAPI int            enna_emotion_next(Evas_Object * obj);
EAPI int            enna_emotion_fastforward(Evas_Object * obj);
EAPI int            enna_emotion_rewind(Evas_Object * obj);

#endif
