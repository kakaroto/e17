#ifndef __EPLAYER_H
#define __EPLAYER_H

#include <Ecore.h>
#include <config.h>
#include "playlist.h"

#define	WIDTH	500
#define	HEIGHT	500

typedef enum {
	TIME_DISPLAY_LEFT,
	TIME_DISPLAY_ELAPSED
} TimeDisplay;

typedef struct {
	PlayList *playlist;
	Ecore_Idler *play_idler;
	
	struct {
		Evas *evas;
		Evas_Object *edje;
		Evas_Object *playlist; /* playlist container */
	} gui;

	TimeDisplay time_display;
} ePlayer;

#endif

