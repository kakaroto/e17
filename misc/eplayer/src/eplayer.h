#include <Ecore_Evas.h>
#include <Ecore.h>
#include <Edje.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <ao/ao.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <linux/soundcard.h>

#include "playlist.h"

#define	WIDTH	500
#define	HEIGHT	500

typedef enum {
	TIME_DISPLAY_LEFT,
	TIME_DISPLAY_ELAPSED
} Time_Display;

typedef struct {
	Evas *evas;
	Evas_List *play_list;
	Evas_List *full_list;
	Ecore_Idler *play_idler;
	Evas_Object *edje;
	Evas_Object *playlist_obj; /* container */

	char last_file[1000];
	Time_Display time_display;
} player_session;

/* Protos */
int play_loop(void *udata);
void unpause_playback();
void pause_playback();

int update_time(player_session *st_session);

int setup_ao();
int ao_open();
int get_vorbis(player_session *st_session, PlayListItem *pli);
void handle_comments(player_session *st_session);
void seek_forward();
void seek_backward();
void setup_ecore(player_session *st_session);
void next_file();
void prev_file();
int read_mixer (player_session *st_session);

void switch_time_display (void *udata, Evas_Object *obj, const char *emission, const char *src);

void raise_vol(void *udata, Evas_Object *obj, const char *emission, const char *src);
void lower_vol(void *udata, Evas_Object *obj, const char *emission, const char *src);

void show_playlist(player_session *session);

