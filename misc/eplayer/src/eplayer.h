#ifndef __EPLAYER_H
#define __EPLAYER_H

#include <Ecore.h>
#include <vorbis/vorbisfile.h>
#include <ao/ao.h>
#include "playlist.h"
#include "mixer.h"

typedef enum {
	TIME_DISPLAY_ELAPSED,
	TIME_DISPLAY_LEFT
} TimeDisplay;

typedef struct {
	PlayList *playlist;
	Mixer *mixer;
	Ecore_Idler *play_idler;
	Ecore_Timer *time_timer;
	
	struct {
		Evas *evas;
		Evas_Object *edje;
		Evas_Object *playlist; /* playlist container */
	} gui;

	TimeDisplay time_display;

	OggVorbis_File current_track;
	ao_device *ao_dev;
} ePlayer;

void eplayer_playback_stop(ePlayer *player, int rewind_track);
void eplayer_playback_start(ePlayer *player, int rewind_track);

#endif

