#ifndef __EUPHORIA_H
#define __EUPHORIA_H

/*
 * $Id$
 * vim:noexpandtab:sw=4:sts=4:ts=4
 */

#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Ecore_Fb.h>
#include <xmms/xmmsclient.h>
#include <xmms/xmmsclient-result.h>
#include "playlist.h"

typedef enum {
	TIME_DISPLAY_ELAPSED,
	TIME_DISPLAY_LEFT
} TimeDisplay;

typedef struct {
	char evas_engine[255];
	char theme[255];
	TimeDisplay time_display;
	bool repeat;
} Config;

typedef struct {
	Evas *evas;
	Ecore_Evas *ee;
	Evas_Object *edje;
	Evas_Object *playlist; /* playlist container */
} Gui;

typedef struct {
	bool seeker_seeking; /* true if seeking, false if not */
	bool seeking;
	bool is_vertical;
	bool invert_dir;
	int seek_dir;
} SeekerFlags;

typedef struct _Euphoria {
	PlayList *playlist;
	xmmsc_connection_t *xmms;

	unsigned int track_current_pos;

	Config cfg;
	Gui gui;
	SeekerFlags seekerflags;
} Euphoria;

#endif

