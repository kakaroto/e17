#include <sys/ioctl.h>
#include "eplayer.h"

	int	eof = 0;
	int 	endian;
        int	time_left;

        /* AUDIO BUFFER */
        char            pcmout[16384];   

        /* XIPH AO */
        ao_device *     device;
        ao_sample_format format;
        int             default_driver;
        int             dev_type;

        /* OggVorbis */
        OggVorbis_File  vf;
        int             current_section;
        vorbis_info *   info;
        vorbis_comment * comment;


/* Main Play Loop */
int play_loop(void *udata) {
	player_session *st_session = udata;


        /* printf("DEBUG: In play loop function \n"); */

    long buff_len = ov_read(&vf, pcmout, sizeof(pcmout), endian, 2, 1, &current_section);

    if (buff_len)
		ao_play (device, pcmout, buff_len);
	else {
		printf("DEBUG: Hit EOF.  Idle timer should be removed now\n");
		/* This sucks ass, but look for another file here....... THIS IS A BAD THING! */
		st_session->play_list = evas_list_next(st_session->play_list);
        setup_ao();
        get_vorbis (st_session, (PlayListItem *) st_session->play_list->data);
        ao_open();
        st_session->play_idler = ecore_idler_add(play_loop, st_session);
		
		return 0; /* Stream is empty, EOF reached, leave func and remove timer */
	}

	update_time(st_session);

	return 1;
}

int update_time(player_session *st_session) {
	int  cur_time;
	char time[9];

	if (st_session->time_display == TIME_DISPLAY_LEFT)
		cur_time = ov_time_tell (&vf);
	else
		cur_time = ov_time_total (&vf, -1) - ov_time_tell (&vf);

	if (cur_time == time_left)
		return 1;

	time_left = cur_time;

	/* printf("DEBUG: Updating time\n"); */

	if (st_session->time_display == TIME_DISPLAY_LEFT)
		snprintf (time, sizeof(time), "%d:%02d", (time_left/60), (time_left%60));
	else
		snprintf (time, sizeof(time), "-%d:%02d", (time_left/60), (time_left%60));

	edje_object_part_text_set(st_session->edje, "time_text", time);
	evas_render(st_session->evas);

	return 1;
}

int setup_ao(){

	ao_initialize();
	default_driver = ao_default_driver_id();
	printf("AO DEBUG: Driver is %d\n", default_driver);

        return 1;

}

int ao_open(){

	ao_info * driver_info;

	driver_info = ao_driver_info(default_driver);	

	format.bits = 16;
        format.channels = info->channels;
        format.rate = info->rate;
        format.byte_format = AO_FMT_NATIVE;

	printf("AO DEBUG: %d Channels at %d Hz, in %d bit words\n", format.channels, format.rate, format.bits);
	printf("AO DEBUG: Audio Device: %s\n", driver_info->name);
	
	endian = ao_is_big_endian();

	printf("AO DEBUG: Endianness is %d\n", endian);

        device = ao_open_live(default_driver, &format, NULL);
                if (device == NULL) {
                  fprintf(stderr, "Error opening device.\n");
                  return 0;
                }

  return 1;
}

int get_vorbis(player_session *st_session, PlayListItem *pli) {
	FILE *fp;

	if (!(fp = fopen (pli->file, "rb")) || ov_open(fp, &vf, NULL, 0)) {
		fprintf (stderr, "ERROR: Can't open file '%s'\n", pli->file);
		return 0;
	}

    printf("DEBUG: Opened file %s\n", pli->file);

	info = ov_info(&vf, -1);
	comment = ov_comment(&vf, -1);

	printf("Going to parse comments\n");
	handle_comments(st_session);

	return 1;
}

/* Use VorbisFile for comment handling */
void handle_comments(player_session *st_session){

        int i;
        int  track_len;
        char time[9];

        track_len = (int) ov_time_total(&vf, -1);
        time_left = track_len;
        sprintf(time, "-%d:%02d", (track_len/60), (track_len%60));

        //evas_object_text_text_set(st_session->time_text, time);
	edje_object_part_text_set(st_session->edje, "time_text", time);



        printf("DEBUG: There are %d comments\n", comment->comments);

  for(i = 0; i < comment->comments; i++){

	/* Use strncasecmp instead - or strcasecmp */
        if(strstr(comment->user_comments[i],"title=")){
                char * title;

                title = strchr(comment->user_comments[i], '=');
                if (title)
                  title = strdup(title + 1);


                  //evas_object_text_text_set(st_session->title_text, title);
		edje_object_part_text_set(st_session->edje, "song_name", title);

        } else if (strstr(comment->user_comments[i],"artist=")){
                char * artist;

                artist = strchr(comment->user_comments[i], '=');
                if (artist)
                  artist = strdup(artist + 1);

                  //evas_object_text_text_set(st_session->artist_text, artist );
		edje_object_part_text_set(st_session->edje, "artist_name", artist);


        } else if (strstr(comment->user_comments[i],"album=")){
                char * album;

                album = strchr(comment->user_comments[i], '=');
                if (album)
                  album = strdup(album + 1);

                //evas_object_text_text_set(st_session->album_text, album);
		edje_object_part_text_set(st_session->edje, "album_name", album);
        }
  }


}

/* More Bite Size Functions for comments */

char *get_track_name (char *file) {
	/* Shit.......... I've gotta think about this...... */	
	return NULL;
}


void seek_forward(){
        double  cur_time;

	/* We don't care if you seek past the file, the play look will catch EOF and play next file */

	printf("DEBUG: Seeking forward\n");

        cur_time =  ov_time_tell(&vf);
        ov_time_seek(&vf, cur_time + 5);
}

void seek_backward(){
        double  cur_time;//, total_time;

        cur_time =  ov_time_tell(&vf);
	
	/* Lets not seek backward if there isn't a point */
	if(cur_time < 6) {
		printf("You arent even 5 secs into the stream!\n");
		return;
	} 

	//printf("DEBUG: Seeking backward - Current Pos: %d\n", (int)cur_time);
        ov_time_seek(&vf, cur_time - 5);
}


/************ STOLEN FROM MOC (Music on CLI) *********************/
/* Get PCM volume, return -1 on error */
int read_mixer (player_session *st_session) {
        int vol;
	int mixer_fd;

	printf("DEBUG: Reading mixer\n");

	mixer_fd = open ("/dev/mixer", O_RDWR);

        if (mixer_fd == -1) {
                open ("/dev/mixer0", O_RDWR);
        }
        if (mixer_fd == -1) {
                printf("MIXER: Can't open mixer device\n");
		return 0;
        }

        if (mixer_fd != -1) {
                if (ioctl(mixer_fd, MIXER_READ(SOUND_MIXER_PCM), &vol) == -1)
                        printf("MIXER: Can't read from mixer\n");
                else {
			int return_val;
                        /* Average between left and right */
                        return_val =  ((vol & 0xFF) + ((vol >> 8) & 0xFF)) / 2;
			printf("MIXER: Returning value: %d\n", return_val);
			close(mixer_fd);
	

	/* Update the display with the volume level */
	{

                char vol_str[3];

                sprintf(vol_str, "%d", (int)return_val);
                printf("DEBUG: Setting the mixer vol: %s\n", vol_str);

                edje_object_part_text_set(st_session->edje, "vol_display_text",  vol_str);	
	}
/*

		if(st_session->edje) {
			if(return_val > 99) 
				return_val == 99;
			if(return_val < 1) 
				edje_object_part_text_set(st_session->edje, "vol_display_text", "--");
			if(return_val < 10) 
				sprintf(return_val, "0%s", (int)return_val);

			edje_object_part_text_set(st_session->edje, "vol_display_text", return_val);
		}
*/
			return return_val;
                }
        }

        return -1;
}

/* Set PCM volume */
void set_mixer(int vol){

        int mixer_fd;

        printf("DEBUG: Setting mixer\n");

        mixer_fd = open ("/dev/mixer", O_RDWR);

        if (mixer_fd == -1) 
                mixer_fd = open ("/dev/mixer0", O_RDWR);

        if (mixer_fd == -1) {
                printf("MIXER: Can't open mixer device\n");
                return;
        }


        if (mixer_fd != -1) {
                if (vol > 100)
                        vol = 100;
                else if (vol < 0)
                        vol = 0;

                vol = vol | (vol << 8);
                if (ioctl(mixer_fd, MIXER_WRITE(SOUND_MIXER_PCM), &vol) == -1)
                        printf("DEBUG: Can't set mixer\n");
        }
	close(mixer_fd);
}
/********** END THEFT *********************************************/

