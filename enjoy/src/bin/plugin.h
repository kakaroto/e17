#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>

typedef struct _Song Song;
typedef struct _Enjoy_Player_Status Enjoy_Player_Status;
typedef struct _Enjoy_Player_Caps Enjoy_Player_Caps;

/**
 * Capabilities changed. Use enjoy_player_caps_get() for value.
 */
EAPI extern int ENJOY_EVENT_PLAYER_CAPS_CHANGE;
/**
 * Status changed. Use enjoy_player_status_get() for value.
 */
EAPI extern int ENJOY_EVENT_PLAYER_STATUS_CHANGE;
EAPI extern int ENJOY_EVENT_PLAYER_TRACK_CHANGE;
EAPI extern int ENJOY_EVENT_TRACKLIST_TRACKLIST_CHANGE;

struct _Enjoy_Player_Caps {
   Eina_Bool can_go_next:1;
   Eina_Bool can_go_prev:1;
   Eina_Bool can_pause:1;
   Eina_Bool can_play:1;
   Eina_Bool can_seek:1;
   Eina_Bool can_provide_metadata:1;
   Eina_Bool has_tracklist:1;
};

typedef enum {
  ENJOY_PLAYBACK_PLAYING = 0,
  ENJOY_PLAYBACK_PAUSED,
  ENJOY_PLAYBACK_STOPPED
} Enjoy_Playback_Status;

struct _Enjoy_Player_Status {
   Enjoy_Playback_Status playback;
   Eina_Bool shuffle:1;
   Eina_Bool repeat:1;
   Eina_Bool endless:1;
};

EAPI char      *enjoy_cache_dir_get(void);
EAPI Eina_Bool  enjoy_repeat_get(void);
EAPI int32_t    enjoy_playlist_current_position_get(void);
EAPI int32_t    enjoy_position_get(void);
EAPI int32_t    enjoy_volume_get(void);
EAPI int32_t    enjoy_playlist_count(void);
EAPI const Song*enjoy_playlist_song_position_get(int32_t position);
EAPI const Song*enjoy_song_current_get(void);
EAPI const Song*enjoy_song_position_get(int32_t position);
EAPI void       enjoy_control_loop_set(Eina_Bool param);
EAPI void       enjoy_control_next(void);
EAPI void       enjoy_control_pause(void);
EAPI void       enjoy_control_play(void);
EAPI void       enjoy_control_previous(void);
EAPI void       enjoy_control_seek(uint64_t position);
EAPI void       enjoy_control_shuffle_set(Eina_Bool param);
EAPI void       enjoy_control_stop(void);
EAPI void       enjoy_position_set(int32_t position);
EAPI void       enjoy_quit(void);
EAPI void       enjoy_repeat_set(Eina_Bool repeat);
EAPI void       enjoy_volume_set(int32_t volume);

EAPI Enjoy_Player_Caps enjoy_player_caps_get(void);
EAPI Enjoy_Player_Status enjoy_player_status_get(void);

#endif /* __PLUGIN_H__ */
