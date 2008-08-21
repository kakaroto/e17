/* Interface */

#include "enna.h"
#include <player.h>

typedef struct _Enna_Module_libplayer
{
   Evas *evas;
   player_t *player;
   Enna_Module *em;
   void (*event_cb)(void *data, enna_mediaplayer_event_t event);
   void *event_cb_data;
   char *uri;
} Enna_Module_libplayer;

static Enna_Module_libplayer *mod;

/*****************************************************************************/
/*                         Private Module API                                */
/*****************************************************************************/

static void _class_init(int dummy)
{
   printf("libplayer class init\n");
}

static void _class_shutdown(int dummy)
{
   if (mod->uri)
     free(mod->uri);
   player_playback_stop (mod->player);
   player_uninit (mod->player);
}

static int _class_file_set(const char *uri)
{
   mrl_t *mrl;
   mrl_resource_local_args_t *args;

   args = calloc (1, sizeof (mrl_resource_local_args_t));
   args->location = strdup (uri);
   if(mod->uri)
     free(mod->uri);
   mod->uri = strdup(uri);
   mrl = mrl_new (mod->player, MRL_RESOURCE_FILE, args);
   if (!mrl)
    return 1;

   player_mrl_set (mod->player, mrl);
   return 0;
}

static int _class_play(void)
{
   player_pb_state_t state = player_playback_get_state (mod->player);

   if (state == PLAYER_PB_STATE_PAUSE)
     player_playback_pause (mod->player); /* unpause */
   else if (state == PLAYER_PB_STATE_IDLE)
     player_playback_start (mod->player);
   return 0;
}

static int _class_seek(double percent)
{
   player_playback_seek (mod->player, (int) (100 * percent), PLAYER_PB_SEEK_PERCENT);
   return 0;
}

static int _class_stop(void)
{
   player_playback_stop (mod->player);
   return 0;
}

static int _class_pause(void)
{
   printf("libplayer pause\n");
   if (player_playback_get_state (mod->player) == PLAYER_PB_STATE_PLAY)
     player_playback_pause (mod->player);
   return 0;
}

static double _class_position_get()
{
   double time_pos;

   /*
    * NOTE: it needs a fix in libplayer because get_time_pos changes the state
    *       of MPlayer and the pause is lost
    */
   if (player_playback_get_state (mod->player) == PLAYER_PB_STATE_PLAY)
     time_pos = (double) player_get_time_pos (mod->player) / 1000.0;
   return time_pos < 0.0 ? 0.0 : time_pos;
}

static double _class_length_get()
{
   return (double) mrl_get_property (mod->player, NULL, MRL_PROPERTY_LENGTH) / 1000.0;
}

static Enna_Metadata *_class_metadata_get(void)
{
   Enna_Metadata *meta;
   char *track_nb;

   meta = calloc (1, sizeof (Enna_Metadata));

   meta->uri = mod->uri+7;
   meta->title = mrl_get_metadata (mod->player, NULL, MRL_METADATA_TITLE);
   meta->artist = mrl_get_metadata (mod->player, NULL, MRL_METADATA_ARTIST);
   meta->album = mrl_get_metadata (mod->player, NULL, MRL_METADATA_ALBUM);
   meta->year = mrl_get_metadata (mod->player, NULL, MRL_METADATA_YEAR);
   meta->genre = mrl_get_metadata (mod->player, NULL, MRL_METADATA_GENRE);
   meta->comment = mrl_get_metadata (mod->player, NULL, MRL_METADATA_COMMENT);
   meta->discid = NULL;
   track_nb = mrl_get_metadata (mod->player, NULL, MRL_METADATA_TRACK);
   if (track_nb)
     {
        meta->track = atoi(track_nb);
        free(track_nb);
     }
   else
     meta->track = 0;

   return meta;
}

static void _class_event_cb_set(void (*event_cb)(void *data, enna_mediaplayer_event_t event), void *data)
{
   /* FIXME: function to call when end of stream is send by libplayer */

   mod->event_cb_data = data;
   mod->event_cb = event_cb;
}

static int
_event_cb (player_event_t e, void *data)
{
   if (e == PLAYER_EVENT_PLAYBACK_FINISHED)
     {
	printf ("PLAYBACK FINISHED\n");
	if (mod->event_cb)
	  {
	     mod->event_cb(mod->event_cb_data, ENNA_MP_EVENT_EOF);
	  }
     }
   return 0;
}

static Enna_Class_MediaplayerBackend class = {
  "libplayer",
  1,
  {
    _class_init,
    _class_shutdown,
    _class_file_set,
    _class_play,
    _class_seek,
    _class_stop,
    _class_pause,
    _class_position_get,
    _class_length_get,
    _class_metadata_get,
    _class_event_cb_set,
    NULL
  }
};

/*****************************************************************************/
/*                          Public Module API                                */
/*****************************************************************************/

EAPI Enna_Module_Api module_api = {
  ENNA_MODULE_VERSION,
  "libplayer"
};

EAPI void
module_init(Enna_Module *em)
{
   if (!em)
     return;

   mod = calloc(1, sizeof(Enna_Module_libplayer));
   mod->em = em;
   mod->evas = em->evas;
   mod->player =
     player_init (PLAYER_TYPE_MPLAYER, PLAYER_AO_AUTO, PLAYER_VO_AUTO,
                  PLAYER_MSG_WARNING, _event_cb);
   enna_mediaplayer_backend_register(&class);
   mod->uri = NULL;
}

EAPI void
module_shutdown(Enna_Module *em)
{
   _class_shutdown(0);
   free(mod);
}
