#ifndef _EX_CDDEV_H
#define _EX_CDDEV_H

void   ex_cddev_update(Extrackt *ex);    

/* Used with disc_info */
#define CDAUDIO_PLAYING				0
#define CDAUDIO_PAUSED				1
#define CDAUDIO_COMPLETED			2
#define CDAUDIO_NOSTATUS			3

#define MAX_TRACKS				100
#define MAX_SLOTS				100 /* For CD changers */

#define CURRENT_CDDBREVISION			2


/* Used for keeping track of times */
typedef struct _disc_time {
   int mins;
   int secs;
} Ex_Disc_Time;

/* Track specific information */
typedef struct _track_info {
   Ex_Disc_Time length;
   Ex_Disc_Time start_pos;
   int num_frames;
   int start_frame;
   int end_frame;
   unsigned char flags;
} Ex_Track_Info;

/* Disc information such as current track, amount played, etc */
typedef struct _disc_info {
   int cd_desc;                             /* CD device file desc. */
   int have_info;                           /* Do we have disc info yet? */
   int disc_present;	                    /* Is disc present? */
   int disc_mode;		            /* Current disc mode */
   Ex_Disc_Time track_time;		    /* Current track time */
   Ex_Disc_Time disc_time;		    /* Current disc time */
   Ex_Disc_Time length;		            /* Total disc length */
   int curr_frame;			    /* Current frame */
   int curr_track;		            /* Current track */
   int num_tracks;		            /* Number of tracks on disc */
   Ex_Track_Info track[MAX_TRACKS];	    /* Track specific information */
} Ex_Disc_Info;

/* Channle volume structure */
typedef struct _channel_volume { 
   int left;
   int right;
} Ex_Channel_Volume;

/* Volume structure */
typedef struct _disc_volume {
   Ex_Channel_Volume vol_front;
   Ex_Channel_Volume vol_back;
} Ex_Disc_Volume;


int ex_cddev_init(char *devname, Extrackt *ex);
int ex_cddev_close(Extrackt *ex);
int ex_cddev_stat(Extrackt *ex,int read_toc);
int ex_cddev_is_data_track(Extrackt *ex, int track);
int ex_cddev_play_frames(Extrackt *ex, int startframe, int endframe);
int ex_cddev_play_track_pos(Extrackt *ex, int starttrack, int endtrack, int startpos);
int ex_cddev_play_track(Extrackt *ex, int starttrack, int endtrack);
int ex_cddev_advance(Extrackt *ex, Ex_Disc_Time *time);
int ex_cddev_stop(Extrackt *ex);
int ex_cddev_pause(Extrackt *ex);
int ex_cddev_resume(Extrackt *ex);
int ex_cddev_tray_is_open(Extrackt *ex);
int ex_cddev_tray_eject(Extrackt *ex);
int ex_cddev_tray_close(Extrackt *ex);
int ex_cddev_volume_get(Extrackt *ex, Ex_Disc_Volume *vol);
int ex_cddev_volume_set(Extrackt *ex, Ex_Disc_Volume *vol);
int ex_cddev_disc_select(Extrackt *ex, int disc_num);  
int ex_cddev_changer_slots_get(Extrackt *ex);
int ex_cddev_track_size(Ex_Track_Info *track);

#endif
