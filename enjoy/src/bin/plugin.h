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
 * Enjoy was started.
 *
 * Plugins that called enjoy_plugin_register() will be called from here,
 * as well as when they are enabled/disabled.
 */
EAPI extern int ENJOY_EVENT_STARTED;
/**
 * Enjoy is about to quit
 *
 * Plugins that called enjoy_plugin_register() will be called from here,
 * as well as when they are enabled/disabled.
 *
 * Plugins should finalize its stuff. If mainloop is required any
 * further, call enjoy_quit_freeze(), do its stuff and then
 * enjoy_quit_thaw().
 *
 * When this is called, don't trust any of GUI is live. Just finish
 * your stuff.
 */
EAPI extern int ENJOY_EVENT_QUIT;
/**
 * Capabilities changed. Use enjoy_player_caps_get() for value.
 */
EAPI extern int ENJOY_EVENT_PLAYER_CAPS_CHANGE;
/**
 * Status changed. Use enjoy_player_status_get() for value.
 */
EAPI extern int ENJOY_EVENT_PLAYER_STATUS_CHANGE;
/**
 * Current track (song) changed. Use enjoy_song_current_get() for new song.
 */
EAPI extern int ENJOY_EVENT_PLAYER_TRACK_CHANGE;
/**
 * Current list view (playlist) changed. Use enjoy_playlist_count() and
 * enjoy_playlist_current_position_get() for new values.
 */
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

EAPI void       enjoy_quit(void);
EAPI void       enjoy_quit_freeze(void);
EAPI void       enjoy_quit_thaw(void);

EAPI char      *enjoy_cache_dir_get(void);
EAPI Eina_Bool  enjoy_repeat_get(void);
EAPI int32_t    enjoy_position_get(void);
EAPI int32_t    enjoy_volume_get(void);
EAPI int32_t    enjoy_playlist_count(void);
EAPI int32_t    enjoy_playlist_current_position_get(void);
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
EAPI void       enjoy_repeat_set(Eina_Bool repeat);
EAPI void       enjoy_volume_set(int32_t volume);

EAPI Enjoy_Player_Caps enjoy_player_caps_get(void);
EAPI Enjoy_Player_Status enjoy_player_status_get(void);

/**
 * When you're loaded use ENJOY_ABI_CHECK() to see if you're okay or not.
 */
#define ENJOY_ABI_VERSION (1U)
EAPI uint32_t   enjoy_abi_version(void) EINA_CONST;
#define ENJOY_ABI_CHECK() (ENJOY_ABI_VERSION == enjoy_abi_version())

typedef struct _Enjoy_Plugin     Enjoy_Plugin;
typedef struct _Enjoy_Plugin_Api Enjoy_Plugin_Api;

struct _Enjoy_Plugin_Api {
#define ENJOY_PLUGIN_API_VERSION (1U)
   unsigned int version;
   Eina_Bool (*enable)(Enjoy_Plugin *plugin);
   Eina_Bool (*disable)(Enjoy_Plugin *plugin);
};

typedef enum {
  ENJOY_PLUGIN_PRIORITY_HIGH = -1000,
  ENJOY_PLUGIN_PRIORITY_NORMAL = 0,
  ENJOY_PLUGIN_PRIORITY_LOW = 1000
} Enjoy_Plugin_Priority;

/**
 * Register the given plugin name with api at priority.
 *
 * Priority will define plugin order, but it will also consider the
 * name, so relative positioning based on name is possible (use
 * namespaces!)
 */
EAPI Enjoy_Plugin *enjoy_plugin_register(const char *name, const Enjoy_Plugin_Api *api, int priority);
EAPI void enjoy_plugin_unregister(Enjoy_Plugin *plugin);

#endif /* __PLUGIN_H__ */
