#include <Edje.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <ao/ao.h>
#include <sys/ioctl.h>
#include "eplayer.h"
#include "vorbis.h"

ao_device *device = NULL;
ao_sample_format format = {0};
OggVorbis_File current_track = {0};

/* Main Play Loop */
int play_loop(void *udata) {
	ePlayer *player = udata;
	long buff_len;
	unsigned char pcmout[16384];
	int big_endian = 0;
	
#ifdef WORDS_BIGENDIAN
	big_endian = 1;
#endif
	
	buff_len = ov_read(&current_track, pcmout, sizeof(pcmout), big_endian,
	                   2, 1, NULL);

    if (buff_len) {
		ao_play (device, pcmout, buff_len);
		update_time(player);
		return 1;
	}
		
	/* This sucks ass, but look for another file here....... THIS IS A BAD THING! */
	player->playlist->cur_item = player->playlist->cur_item->next;
	open_track(player);

	return 1;
}

int update_time(ePlayer *player) {
	static int old_time = -1;
	int cur_time;
	char time[9];

	if (player->time_display == TIME_DISPLAY_LEFT)
		cur_time = ov_time_tell(&current_track);
	else
		cur_time = ov_time_total(&current_track, -1) -
		           ov_time_tell(&current_track);

	if (cur_time == old_time) /* value didn't change, so update */
		return 1;

	old_time = cur_time;

	if (player->time_display == TIME_DISPLAY_LEFT)
		snprintf(time, sizeof(time), "%d:%02d",
		         (cur_time / 60), (cur_time % 60));
	else
		snprintf(time, sizeof(time), "-%d:%02d",
		         (cur_time / 60), (cur_time % 60));

	edje_object_part_text_set(player->gui.edje, "time_text", time);
	evas_render(player->gui.evas);

	return 1;
}

static int setup_ao(PlayListItem *current) {
	ao_info *driver_info;
	int default_driver;
	
	ao_initialize();
	default_driver = ao_default_driver_id();

#ifdef DEBUG
	printf("AO DEBUG: Driver is %d\n", default_driver);
#endif

	driver_info = ao_driver_info(default_driver);	

	format.bits = 16;
	format.channels = current->channels;
	format.rate = current->rate;
	format.byte_format = AO_FMT_NATIVE;

#ifdef DEBUG
	printf("AO DEBUG: %d Channels at %d Hz, in %d bit words\n",
	       format.channels, format.rate, format.bits);
	printf("AO DEBUG: Audio Device: %s\n", driver_info->name);
#endif
	
	if (!(device = ao_open_live(default_driver, &format, NULL))) {
		fprintf(stderr, "Error opening device.\n");
		return 0;
	}

	return 1;
}

void open_track(ePlayer *player) {
	PlayListItem *pli = player->playlist->cur_item->data;
	FILE *fp;

	ov_clear(&current_track);

	if (!(fp = fopen (pli->file, "rb"))
	    || ov_open(fp, &current_track, NULL, 0)) {
		fprintf (stderr, "ERROR: Can't open file '%s'\n", pli->file);
		return;
	}

	edje_object_part_text_set(player->gui.edje, "song_name", pli->title);
	edje_object_part_text_set(player->gui.edje, "artist_name", pli->artist);
	edje_object_part_text_set(player->gui.edje, "album_name", pli->album);

	setup_ao (pli);
}

void seek_forward(void *udata, Evas_Object *obj,
                  const char *emission, const char *src) {
	/* We don't care if you seek past the file, the play look will catch EOF and play next file */
#ifdef DEBUG
	printf("DEBUG: Seeking forward\n");
#endif

	ov_time_seek(&current_track, ov_time_tell(&current_track) + 5);
}

void seek_backward(void *udata, Evas_Object *obj,
                   const char *emission, const char *src) {
	double cur_time = ov_time_tell(&current_track);
	
	/* Lets not seek backward if there isn't a point */
	if (cur_time < 6) {
		printf("You arent even 5 secs into the stream!\n");
		return;
	} 

#ifdef DEBUG
	printf("DEBUG: Seeking backward - Current Pos: %lf\n", cur_time);
#endif
	
	ov_time_seek(&current_track, cur_time - 5);
}

