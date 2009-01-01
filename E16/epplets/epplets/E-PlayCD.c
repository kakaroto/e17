#include <epplet.h>
#include <cdaudio.h>
#include <sys/time.h>
#include <sys/resource.h>

#define PACKAGE "E-PlayCD"
#define VERSION "0.5"

#define NUM_BUTTONS 10
#define MAX_LBL_LEN 15
#define SEPARATOR "-"

Epplet_gadget       cddb_label = NULL;	/* Label displayed on the epplet */
Epplet_gadget       tracks_popup = NULL;	/* Popup containing the CD tracks */
Epplet_gadget       volume_slider = NULL;
char                cddb_string[255];	/* The string containing our cddb info */
struct disc_data    DiscData;
struct disc_info    DiscInfo;
int                 cd_desc = 0;
int                 current_track = 1;
struct disc_volume  cd_volume;
unsigned long       current_data_id;

/* Config options */
char               *cdrom_device;
char               *scroll_or_bounce;
char               *separator;
char                show_album;
char                show_artist;
char                show_name;

/* Prototypes */
static void         cb_close(void *data);
static void         scroll_cddb_label(void *data);
void                cddb_update(void *data);
void                update_cddb_string(void);
void                update_cddb_tracks(void);
void                fake_cddb_string(void);
void                fake_cddb_tracks(void);
static void         bounce_cddb_label(void *data);
static void         update_track_info(void *data);
void                rew_cd(void *data);
void                ff_cd(void *data);
void                play_cd(void *data);
void                stop_cd(void *data);
void                eject_cd(void *data);
void                prev_track(void *data);
void                next_track(void *data);
void                do_help(void *data);
void                popup_tracks(void *data);
void                set_volume(void *data);
void                get_volume(void);
int                 eplaycd_cd_stat(int cd_desc, struct disc_info *disc);
static void         open_cdrom(char *cdrom_device);
static void         close_cdrom(void);

/* bounce the cddb_label */
static void
bounce_cddb_label(void *data)
{
   static int          start = 0;
   static int          direction = 1;
   char               *temp = (char *)data;
   char                saved_char = temp[start + MAX_LBL_LEN];

   return;			/* this is broke */

   if (strlen(temp) < MAX_LBL_LEN)
     {
	Epplet_timer(bounce_cddb_label, &cddb_string, 0.3, "Bouncer");
	return;
     }
   else
     {
	if (((start + MAX_LBL_LEN) > (int)strlen(temp)) || start < 0)
	   direction = -direction;

	temp[start + MAX_LBL_LEN] = '\0';
	Epplet_change_label(cddb_label, &temp[start]);

	temp[start + MAX_LBL_LEN] = saved_char;
	start += direction;
     }

   Epplet_timer(bounce_cddb_label, NULL, 0.3, "Bouncer");
   return;
   data = NULL;
}

static void
scroll_cddb_label(void *data)
{
   char               *temp = (char *)data;
   int                 length = strlen(temp);
   char                saved_char = temp[MAX_LBL_LEN];

   if (strlen(temp) < MAX_LBL_LEN)
     {
	Epplet_timer(scroll_cddb_label, &cddb_string, 0.3, "Scroller");
	return;
     }
   else
     {
	temp[MAX_LBL_LEN] = '\0';
	Epplet_change_label(cddb_label, temp);
	temp[MAX_LBL_LEN] = saved_char;
	saved_char = temp[0];
	memcpy(data, temp + 1, (size_t) length);
	temp[length - 1] = saved_char;
     }

   Epplet_timer(scroll_cddb_label, &cddb_string, 0.3, "Scroller");
   return;
   data = NULL;
}

static void
update_track_info(void *data)
{
   if (!cd_desc)
     {
	Epplet_timer(update_track_info, NULL, 2, "TrackInfo");
	return;
     }

   if (eplaycd_cd_stat(cd_desc, &DiscInfo) < 0)
     {
	printf("Error stat'ing cdrom\n");
	return;
     }

   if (DiscInfo.disc_mode == CDAUDIO_PLAYING)
     {
	if (DiscInfo.disc_current_track != current_track)
	  {
	     current_track = DiscInfo.disc_current_track;
	     cddb_update(NULL);
	  }
     }

   Epplet_timer(update_track_info, NULL, 2, "TrackInfo");
   return;
   data = NULL;
}

/* Close */
static void
cb_close(void *data)
{
   Epplet_unremember();
   cd_finish(cd_desc);
   exit(0);
   return;
   data = NULL;
}

/* Rewind */
void
rew_cd(void *data)
{
   struct disc_timeval time;

   if (!cd_desc)
      return;

   eplaycd_cd_stat(cd_desc, &DiscInfo);

   time.minutes = 0;
   time.seconds = -2;
   time.frames = 0;
   cd_advance(cd_desc, time);
   return;
   data = NULL;
}

/* Fast Forward */
void
ff_cd(void *data)
{
   struct disc_timeval time;

   if (!cd_desc)
      return;

   eplaycd_cd_stat(cd_desc, &DiscInfo);

   time.minutes = 0;
   time.seconds = 2;
   time.frames = 0;

   cd_advance(cd_desc, time);
   return;
   data = NULL;
}

/* Play */
void
play_cd(void *data)
{
   int                *the_track = (int *)data;

   if (!cd_desc)
      open_cdrom(cdrom_device);

   eplaycd_cd_stat(cd_desc, &DiscInfo);

   if (DiscInfo.disc_present)
     {
	if (the_track)
	  {
	     current_track = *the_track;
	     cd_play(cd_desc, current_track);
	     update_cddb_string();
	  }
	else
	  {
	     switch (DiscInfo.disc_mode)
	       {
	       case CDAUDIO_PLAYING:
		  cd_pause(cd_desc);
		  break;
	       case CDAUDIO_PAUSED:
		  cd_resume(cd_desc);
		  break;
	       default:
		  cd_play(cd_desc, current_track);
		  break;
	       }
	  }
     }
   else
      close_cdrom();
}

/* Stop */
void
stop_cd(void *data)
{
   if (!cd_desc)
      return;

   if (eplaycd_cd_stat(cd_desc, &DiscInfo) < 0)
      return;

   cd_stop(cd_desc);
   return;
   data = NULL;
}

/* Eject */
void
eject_cd(void *data)
{
   static int          open = 0;

   if (!cd_desc)
      open_cdrom(cdrom_device);

   if (eplaycd_cd_stat(cd_desc, &DiscInfo) < 0)
     {
	printf("Error ejecting CD...\n");
	return;
     }

   if (open)
     {
	open = 0;
	cddb_update(NULL);
     }
   else
     {
	cd_eject(cd_desc);
	close_cdrom();
	open = 1;
     }
   return;
   data = NULL;
}

/* Previous Track */
void
prev_track(void *data)
{
   /* eplaycd_cd_stat(cd_desc, &DiscInfo); */

   if (!cd_desc)
      return;

   if (current_track <= DiscInfo.disc_first_track)
      current_track = DiscInfo.disc_total_tracks;
   else
      current_track--;

   if (DiscInfo.disc_mode == CDAUDIO_PLAYING ||
       DiscInfo.disc_mode == CDAUDIO_PAUSED)
      cd_play(cd_desc, current_track);

   update_cddb_string();
   return;
   data = NULL;
}

/* Next Track */
void
next_track(void *data)
{
   /* eplaycd_cd_stat(cd_desc, &DiscInfo); */

   if (!cd_desc)
      return;

   if (current_track >= DiscInfo.disc_total_tracks)
      current_track = DiscInfo.disc_first_track;
   else
      current_track++;

   if ((DiscInfo.disc_mode == CDAUDIO_PLAYING) ||
       DiscInfo.disc_mode == CDAUDIO_PAUSED)
      cd_play(cd_desc, current_track);

   update_cddb_string();
   return;
   data = NULL;
}

/* Stat CD */
int
eplaycd_cd_stat(int cd_desc, struct disc_info *disc)
{

   if (!cd_desc)
      return -1;

   cd_stat(cd_desc, disc);

   if (!disc->disc_present)
     {
	cd_close(cd_desc);
	cd_stat(cd_desc, disc);
	if (!disc->disc_present)
	   return -1;
     }

   return 0;
}

/* Update CDDB Info */
void
cddb_update(void *data)
{
   if (!cd_desc)
      open_cdrom(cdrom_device);

   if ((eplaycd_cd_stat(cd_desc, &DiscInfo) >= 0) && DiscInfo.disc_present)
     {
	current_track = DiscInfo.disc_current_track;

	if (cddb_read_data(cd_desc, &DiscData) < 0)
	  {
	     current_data_id = DiscData.data_id;
	     fake_cddb_string();
	     fake_cddb_tracks();

	     current_track = DiscInfo.disc_current_track;
	  }
	else
	  {
	     if (DiscData.data_id != current_data_id)
	       {
		  current_data_id = DiscData.data_id;
		  update_cddb_string();
		  update_cddb_tracks();
	       }
	     else
	       {
		  update_cddb_string();
		  current_track = DiscInfo.disc_current_track;
	       }
	  }
     }

   switch (DiscInfo.disc_mode)
     {
     case CDAUDIO_PLAYING:
     case CDAUDIO_PAUSED:
	break;
     default:
	close_cdrom();
	break;
     }
   return;
   data = NULL;
}

/* Update String for CDDB label */
void
update_cddb_string(void)
{
   /*
    * strcpy(cddb_string, DiscData.data_artist);
    * strcat(cddb_string, SEPARATOR);
    * strcat(cddb_string, DiscData.data_title);
    * strcat(cddb_string, SEPARATOR);
    */

   if (DiscData.data_id <= 0)
     {
	fake_cddb_string();
	return;
     }

   cddb_string[0] = '\0';

   if (show_artist)
     {
	strcat(cddb_string, separator);
	strcat(cddb_string, DiscData.data_artist);
     }

   if (show_album)
     {
	strcat(cddb_string, separator);
	strcat(cddb_string, DiscData.data_title);
     }

   if (show_name)
     {
	strcat(cddb_string, separator);
	strcat(cddb_string, DiscData.data_track[current_track - 1].track_name);
     }

   Epplet_change_label(cddb_label, cddb_string);
}

void
fake_cddb_string(void)
{
   sprintf(cddb_string, "Track %d   ", current_track);

   Epplet_change_label(cddb_label, cddb_string);
}

void
update_cddb_tracks(void)
{
   int                 i;
   static int         *tracks = NULL;

   free(tracks);

   tracks = (int *)calloc((size_t) DiscInfo.disc_total_tracks, sizeof(int));

   /* Causes segfault...
    * if(tracks_popup != NULL)
    * Epplet_gadget_destroy(tracks_popup);
    */

   tracks_popup = Epplet_create_popup();

   Epplet_add_popup_entry(tracks_popup, "-NeverMind-", NULL, NULL, NULL);

   for (i = 0; i < DiscInfo.disc_total_tracks; i++)
     {
	tracks[i] = i + 1;

	Epplet_add_popup_entry(tracks_popup,
			       DiscData.data_track[i].track_name,
			       NULL, play_cd, &tracks[i]);
     }
}

void
fake_cddb_tracks(void)
{
   int                 i;
   static int         *tracks = NULL;
   char                temp_title[128];

   free(tracks);

   tracks = (int *)calloc((size_t) DiscInfo.disc_total_tracks, sizeof(int));

   tracks_popup = Epplet_create_popup();

   Epplet_add_popup_entry(tracks_popup, "-NeverMind-", NULL, NULL, NULL);

   for (i = 0; i < DiscInfo.disc_total_tracks; i++)
     {
	tracks[i] = i + 1;
	snprintf(temp_title, 128, "Track %d", i + 1);

	Epplet_add_popup_entry(tracks_popup,
			       temp_title, NULL, play_cd, &tracks[i]);
     }
}

/* Pop-Up About Box */
void
do_help(void *data)
{
   Epplet_show_about("E-PlayCD");
   return;
   data = NULL;
}

void
popup_tracks(void *data)
{
   Epplet_pop_popup(tracks_popup, 0);
   return;
   data = NULL;
}

/* Set the Volume */
void
set_volume(void *data)
{
   struct disc_volume *the_volume = (struct disc_volume *)data;

   the_volume->vol_front.right = the_volume->vol_front.left;
   cd_set_volume(cd_desc, *the_volume);
   return;
   data = NULL;
}

/* Get the Volume */
void
get_volume(void)
{
   cd_get_volume(cd_desc, &cd_volume);
}

static void
open_cdrom(char *cdrom_device)
{
   if ((cd_desc = cd_init_device(cdrom_device)) < 0)
     {
	printf("Could not open %s\n", cdrom_device);
	exit(0);
     }
}

static void
close_cdrom(void)
{
   cd_finish(cd_desc);
   cd_desc = 0;
}

static void
load_config(void)
{
   cdrom_device = Epplet_query_config_def("cdrom_device", "/dev/cdrom");
   scroll_or_bounce = Epplet_query_config_def("scroll_or_bounce", "scroll");
   separator = Epplet_query_config_def("separator", "-");

   show_artist = atoi(Epplet_query_config_def("show_artist", "1"));
   show_album = atoi(Epplet_query_config_def("show_album", "yes"));
   show_name = atoi(Epplet_query_config_def("show_name", "yes"));
}

/* MAIN */
int
main(int argc, char **argv)
{
   int                 i, prio;
   char               *button_stds[] = {
      "CLOSE", "PLAY", "STOP", "PREVIOUS", "NEXT", "ARROW_UP",
      "HELP", "REWIND", "FAST_FORWARD", "REPEAT", "CONFIGURE",
      "EJECT"
   };
   typedef void        (*callback_func) (void *data);
   callback_func       button_funcs[] = {
      cb_close, play_cd, stop_cd, prev_track, next_track,
      popup_tracks, do_help, rew_cd, ff_cd, NULL, cddb_update,
      eject_cd
   };
   atexit(Epplet_cleanup);
   prio = getpriority(PRIO_PROCESS, getpid());
   setpriority(PRIO_PROCESS, getpid(), prio + 10);
   Epplet_Init(PACKAGE, VERSION,
	       "Enlightenment CD Player Epplet", 5, 3, argc, argv, 0);
   Epplet_load_config();
   load_config();
   if (!strcmp(scroll_or_bounce, "bounce"))
      Epplet_timer(bounce_cddb_label, &cddb_string, 0.3, "Bouncer");
   else
      Epplet_timer(scroll_cddb_label, &cddb_string, 0.3, "Scroller");
   Epplet_timer(update_track_info, NULL, 1, "TrackInfo");
   for (i = 0; i < 6; i++)
     {
	Epplet_gadget_show(Epplet_create_button(NULL, NULL,
						2 + (i * 13),
						2, 0, 0,
						button_stds
						[i], 0, NULL,
						button_funcs[i], NULL));
     }

   for (i = 0; i < 6; i++)
     {
	Epplet_gadget_show(Epplet_create_button(NULL, NULL,
						2 + (i * 13),
						15, 0, 0,
						button_stds[6
							    +
							    i],
						0, NULL,
						button_funcs[6 + i], NULL));
     }

   tracks_popup = Epplet_create_popup();
   Epplet_add_popup_entry(tracks_popup, "-NeverMind-", NULL, NULL, NULL);

   get_volume();
   Epplet_gadget_show(volume_slider =
		      Epplet_create_hslider(2, 27, 76, 0,
					    255, 4, 8,
					    &cd_volume.vol_front.left,
					    set_volume, &cd_volume));
   /* Epplet_gadget_data_changed (volume_slider); */
   Epplet_gadget_show(cddb_label =
		      Epplet_create_label(2, 35, "Welcome To E-PlayCD", 2));
   strcpy(cddb_string, " Welcome To E-PlayCD ");
   Epplet_show();
   cddb_update(NULL);
   Epplet_Loop();
   return 0;
}
