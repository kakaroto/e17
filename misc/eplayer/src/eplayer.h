#ifndef __EPLAYER_H
#define __EPLAYER_H

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_Fb.h>
#include <pthread.h>
#include "playlist.h"
#include "plugin.h"

typedef enum {
	TIME_DISPLAY_ELAPSED,
	TIME_DISPLAY_LEFT
} TimeDisplay;

typedef struct {
	char evas_engine[255];
	char output_plugin[255];
	char theme[255];
	TimeDisplay time_display;
	int repeat;
} Config;

typedef struct {
	Evas *evas;
	Ecore_Evas *ee;
	Evas_Object *edje;
	Evas_Object *playlist; /* playlist container */
	int playlist_font_size;
} Gui;

typedef struct {
	int seeker_seeking; /* 1 if seeking, 0 if not */
	int seeking;
	int seek_dir;
} Flags;

typedef struct {
	const char **args;

	PlayList *playlist;
	Ecore_Timer *time_timer;

	pthread_t playback_thread;
	
	pthread_mutex_t playback_stop_mutex;
	int playback_stop;
	
	pthread_mutex_t playback_next_mutex;
	int playback_next;

	OutputPlugin *output;
	Evas_List *input_plugins; /* lists all available input plugins */

	Config cfg;
	Gui gui;
	Flags flags;
} ePlayer;

void eplayer_playback_stop(ePlayer *player);
bool eplayer_playback_start(ePlayer *player, bool rewind_track);

#endif

