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

#define	WIDTH	500
#define	HEIGHT	500

typedef struct {
        Evas        *   evas;
	
        Evas_List   *   play_list;
        Evas_List   *   full_list;
        Ecore_Idler *   play_idler;

	Evas_Object *	edje;

	char	last_file[1000];
  } player_session;


/* Protos */
int play_loop(player_session *st_session);
void unpause_playback();
void pause_playback();
int update_time(OggVorbis_File * vf, player_session *st_session);
int setup_ao();
int ao_open();
int get_vorbis(char *filename[], player_session *st_session);
int handle_comments(player_session *st_session);
static int app_signal_exit(void *data, int type, void *event);
void seek_forward();
void seek_backward();
int setup_ecore();
void next_file();
void prev_file();
int read_mixer (player_session *st_session);

void raise_vol(player_session *data, Evas *e, Evas_Object *obj, void *event_info);
void lower_vol(player_session *data, Evas *e, Evas_Object *obj, void *event_info);
