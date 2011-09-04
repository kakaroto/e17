#ifndef __PLUGIN_H__
#define __PLUGIN_H__

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Eina.h>

typedef struct _Song Song;
typedef struct _Enjoy_Player_Status Enjoy_Player_Status;

EAPI extern int ENJOY_EVENT_PLAYER_CAPS_CHANGE;
EAPI extern int ENJOY_EVENT_PLAYER_STATUS_CHANGE;
EAPI extern int ENJOY_EVENT_PLAYER_TRACK_CHANGE;
EAPI extern int ENJOY_EVENT_TRACKLIST_TRACKLIST_CHANGE;

/*
 * Capabilities and player status values conform to the MPRIS 1.0 standard:
 * http://www.mpris.org/1.0/spec.html
 */
typedef enum {
  ENJOY_CAPABILITY_CAN_GO_NEXT = 1 << 0,
  ENJOY_CAPABILITY_CAN_GO_PREV = 1 << 1,
  ENJOY_CAPABILITY_CAN_PAUSE = 1 << 2,
  ENJOY_CAPABILITY_CAN_PLAY = 1 << 3,
  ENJOY_CAPABILITY_CAN_SEEK = 1 << 4,
  ENJOY_CAPABILITY_CAN_PROVIDE_METADATA = 1 << 5,
  ENJOY_CAPABILITY_CAN_HAS_TRACKLIST = 1 << 6
} Enjoy_Capabilities;

struct _Enjoy_Player_Status {
  Eina_Bool playback;
  Eina_Bool shuffle;
  Eina_Bool repeat;
  Eina_Bool endless;
};

EAPI char      *enjoy_cache_dir_get(void);
EAPI Eina_Bool  enjoy_repeat_get(void);
EAPI int32_t    enjoy_playlist_current_position_get(void);
EAPI int32_t    enjoy_position_get(void);
EAPI int32_t    enjoy_volume_get(void);
EAPI int        enjoy_caps_get(void);
EAPI Song      *enjoy_playlist_song_position_get(int32_t position);
EAPI Song      *enjoy_song_current_get(void);
EAPI Song      *enjoy_song_position_get(int32_t position);
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
EAPI Enjoy_Player_Status *enjoy_status_get(void);
EAPI void       enjoy_volume_set(int32_t volume);
EAPI void	no_free();

#endif /* __PLUGIN_H__ */
