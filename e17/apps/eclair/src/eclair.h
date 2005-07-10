#ifndef _ECLAIR_H_
#define _ECLAIR_H_

#include <Evas.h>
#include "eclair_video.h"
#include "eclair_playlist.h"
#include "eclair_subtitles.h"
#include "eclair_dialogs.h"
#include "eclair_meta_tag.h"
#include "eclair_cover.h"
#include "eclair_config.h"
#include "eclair_database.h"
#include "eclair_types.h"

struct _Eclair
{
   //Video related vars
   Eclair_Video video;
   Eclair_Engine video_engine;

   //Gui related vars
   Eclair_Window *gui_window;
   Eclair_Window *playlist_window;
   Eclair_Window *equalizer_window;
   Eclair_Window *cover_window;
   Evas_Object *playlist_container;
   Eclair_Window *playlist_container_owner;
   Evas_Object *previous_cover;
   Evas_Object *cover;
   Eclair_Window *cover_owner;

   Evas_Bool use_progress_bar_drag_for_time;
   Evas_Bool dont_update_progressbar;
   char *gui_theme_file;
   int playlist_entry_height;
   Eclair_Engine gui_engine;
   Eclair_Drop_Object drop_object;
   Eclair_Window *drop_window;

   //Core vars
   Eclair_State state;
   Eclair_Playlist playlist;
   Eclair_Subtitles subtitles;
   Eclair_Dialogs_Manager dialogs_manager;
   Eclair_Meta_Tag_Manager meta_tag_manager;
   Eclair_Cover_Manager cover_manager;
   Eclair_Config config;
   Eclair_Database database;
   Evas_Bool start_playing;
   double seek_to_pos;

   //Arguments
   int *argc;
   char ***argv;
};

//core functions
Evas_Bool eclair_init(Eclair *eclair, int *argc, char ***argv);
void eclair_shutdown(Eclair *eclair);
void eclair_update(Eclair *eclair);
void eclair_update_current_file_info(Eclair *eclair, Evas_Bool force_cover_update);
void eclair_gui_cover_set(Eclair *eclair, const char *cover_path, Evas_Bool force_cover_update);
void eclair_send_signal_to_all_windows(Eclair *eclair, const char *signal);
void eclair_all_windows_text_set(Eclair *eclair, const char *field_name, const char *text);

//navigation functions
void eclair_play_file(Eclair *eclair, const char *path);
void eclair_play_current(Eclair *eclair);
void eclair_play_prev(Eclair *eclair);
void eclair_play_next(Eclair *eclair);
void eclair_pause(Eclair *eclair);
void eclair_play(Eclair *eclair);
void eclair_stop(Eclair *eclair);
double eclair_progress_rate_get(Eclair *eclair);
void eclair_progress_rate_set(Eclair *eclair, double progress_rate);
double eclair_position_get(Eclair *eclair);
void eclair_position_set(Eclair *eclair, double position);
void eclair_audio_level_set(Eclair *eclair, double audio_level);

#endif
