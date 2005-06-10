#ifndef _ECLAIR_H_
#define _ECLAIR_H_

#include "eclair_private.h"

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
