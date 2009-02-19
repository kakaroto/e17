/* Interface */

#include "enna.h"
#include <player.h>

#define ENNA_MODULE_NAME "libplayer"

#define URI_TYPE_FTP      "ftp://"
#define URI_TYPE_HTTP     "http://"
#define URI_TYPE_MMS      "mms://"
#define URI_TYPE_RTP      "rtp://"
#define URI_TYPE_RTSP     "rtsp://"
#define URI_TYPE_SMB      "smb://"
#define URI_TYPE_TCP      "tcp://"
#define URI_TYPE_UDP      "udp://"
#define URI_TYPE_UNSV     "unsv://"
#define URI_TYPE_DVD      "dvd://"
#define URI_TYPE_DVDNAV   "dvdnav://"

typedef struct _Enna_Module_libplayer
{
    Evas *evas;
    player_t *player;
    Enna_Module *em;
    void (*event_cb)(void *data, enna_mediaplayer_event_t event);
    void *event_cb_data;
    char *uri;
    char *label;
    Ecore_Event_Handler *key_down_event_handler;
    Ecore_Pipe *pipe;
} Enna_Module_libplayer;

static Enna_Module_libplayer *mod;

/*****************************************************************************/
/*                         Private Module API                                */
/*****************************************************************************/

static void _class_init(int dummy)
{
    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "class init");
}

static void _class_shutdown(int dummy)
{
    if (mod->uri)
        free(mod->uri);
    if (mod->label)
        free(mod->label);
    ecore_event_handler_del(mod->key_down_event_handler);
    ecore_pipe_del(mod->pipe);
    player_playback_stop(mod->player);
    player_uninit(mod->player);
}

static mrl_t * set_network_stream(const char *uri, mrl_resource_t type)
{
    mrl_t *mrl;
    mrl_resource_network_args_t *args;

    args = calloc(1, sizeof(mrl_resource_network_args_t));
    args->url = strdup(uri);
    mrl = mrl_new(mod->player, type, args);

    return mrl;

}

static mrl_t * set_dvd_stream(const char *uri, mrl_resource_t type)
{
    mrl_t *mrl;
    mrl_resource_videodisc_args_t *args;
    char *meta;
    uint32_t prop = 0;
    int tmp = 0;
    int title = 0;

    args = calloc(1, sizeof(mrl_resource_videodisc_args_t));
    mrl = mrl_new(mod->player, type, args);

    meta = mrl_get_metadata_dvd (mod->player, mrl, (uint8_t *) &prop);
    if (meta)
    {
	enna_log (ENNA_MSG_INFO, ENNA_MODULE_NAME, "Meta DVD VolumeID: %s", meta);
	free (meta);
    }

    if (prop)
    {
	int i;

	enna_log (ENNA_MSG_INFO, ENNA_MODULE_NAME, "Meta DVD Titles: %i", prop);

	for (i = 1; i <= prop; i++)
	{
	    uint32_t chapters, angles, length;

	    chapters = mrl_get_metadata_dvd_title (mod->player, mrl, i,
		MRL_METADATA_DVD_TITLE_CHAPTERS);
	    angles = mrl_get_metadata_dvd_title (mod->player, mrl, i,
		MRL_METADATA_DVD_TITLE_ANGLES);
	    length = mrl_get_metadata_dvd_title (mod->player, mrl, i,
		MRL_METADATA_DVD_TITLE_LENGTH);

	    enna_log (ENNA_MSG_INFO, ENNA_MODULE_NAME,"Meta DVD Title %i (%.2f sec), Chapters: %i, Angles: %i",
		i, length / 1000.0, chapters, angles);
	    if (length > tmp)
	    {
		tmp = length;
		title = i;
	    }
	}
    }
    args->title_start = title;

    return mrl;
}

static mrl_t * set_local_stream(const char *uri)
{
    mrl_t *mrl;
    mrl_resource_local_args_t *args;

    args = calloc(1, sizeof(mrl_resource_local_args_t));
    args->location = strdup(uri);
    mrl = mrl_new(mod->player, MRL_RESOURCE_FILE, args);

    return mrl;
}

static int _class_file_set(const char *uri, const char *label)
{
    mrl_t *mrl = NULL;

    /* try network streams */
    if (!strncmp(uri, URI_TYPE_FTP, strlen(URI_TYPE_FTP)))
        mrl = set_network_stream(uri, MRL_RESOURCE_FTP);
    else if (!strncmp(uri, URI_TYPE_HTTP, strlen(URI_TYPE_HTTP)))
        mrl = set_network_stream(uri, MRL_RESOURCE_HTTP);
    else if (!strncmp(uri, URI_TYPE_MMS, strlen(URI_TYPE_MMS)))
        mrl = set_network_stream(uri, MRL_RESOURCE_MMS);
    else if (!strncmp(uri, URI_TYPE_RTP, strlen(URI_TYPE_RTP)))
        mrl = set_network_stream(uri, MRL_RESOURCE_RTP);
    else if (!strncmp(uri, URI_TYPE_RTSP, strlen(URI_TYPE_RTSP)))
        mrl = set_network_stream(uri, MRL_RESOURCE_RTSP);
    else if (!strncmp(uri, URI_TYPE_SMB, strlen(URI_TYPE_SMB)))
        mrl = set_network_stream(uri, MRL_RESOURCE_SMB);
    else if (!strncmp(uri, URI_TYPE_TCP, strlen(URI_TYPE_TCP)))
        mrl = set_network_stream(uri, MRL_RESOURCE_TCP);
    else if (!strncmp(uri, URI_TYPE_UDP, strlen(URI_TYPE_UDP)))
        mrl = set_network_stream(uri, MRL_RESOURCE_UDP);
    else if (!strncmp(uri, URI_TYPE_UNSV, strlen(URI_TYPE_UNSV)))
        mrl = set_network_stream(uri, MRL_RESOURCE_UNSV);

    /* Try Dvd video */
    else if (!strncmp(uri, URI_TYPE_DVD, strlen(URI_TYPE_DVD)))
        mrl = set_dvd_stream(uri, MRL_RESOURCE_DVD);
    else if (!strncmp(uri, URI_TYPE_DVDNAV, strlen(URI_TYPE_DVDNAV)))
        mrl = set_dvd_stream(uri, MRL_RESOURCE_DVDNAV);

    /* default is local files */
    if (!mrl)
        mrl = set_local_stream(uri);

    if (!mrl)
        return 1;

    if (mod->uri)
        free(mod->uri);
    mod->uri = strdup(uri);

    if (mod->label)
        free(mod->label);
    mod->label = label ? strdup(label) : NULL;

    player_mrl_set(mod->player, mrl);
    return 0;
}

static int _class_play(void)
{
    player_pb_state_t state = player_playback_get_state(mod->player);

    if (state == PLAYER_PB_STATE_PAUSE)
        player_playback_pause(mod->player); /* unpause */
    else if (state == PLAYER_PB_STATE_IDLE)
        player_playback_start(mod->player);
    return 0;
}

static int _class_seek(double percent)
{
    player_playback_seek(mod->player,
                         (int) (100 * percent), PLAYER_PB_SEEK_PERCENT);
    return 0;
}

static int _class_stop(void)
{
    player_playback_stop(mod->player);
    return 0;
}

static int _class_pause(void)
{
    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "pause");
    if (player_playback_get_state(mod->player) == PLAYER_PB_STATE_PLAY)
        player_playback_pause(mod->player);
    return 0;
}

static double _class_position_get()
{
    double time_pos = 0.0;

    time_pos = (double) player_get_time_pos(mod->player) / 1000.0;
    return time_pos < 0.0 ? 0.0 : time_pos;
}

static double _class_length_get()
{
    return (double) mrl_get_property(mod->player,
                                     NULL, MRL_PROPERTY_LENGTH) / 1000.0;
}

static void _class_video_resize(int x, int y, int w, int h)
{
    int flags = PLAYER_X_WINDOW_X | PLAYER_X_WINDOW_Y |
                PLAYER_X_WINDOW_W | PLAYER_X_WINDOW_H;

    /* if w or h is 0, libplayer guess the best size automatically */
    player_x_window_set_properties(mod->player, x, y, w, h, flags);
}

static void _class_event_cb_set(void (*event_cb)(void *data,
                                                 enna_mediaplayer_event_t event),
                                void *data)
{
    /* FIXME: function to call when end of stream is send by libplayer */

    mod->event_cb_data = data;
    mod->event_cb = event_cb;
}

static void _pipe_read(void *data, void *buf, unsigned int nbyte)
{
    enna_mediaplayer_event_t *event = buf;

    if (!mod->event_cb || !buf)
        return;

    mod->event_cb(mod->event_cb_data, *event);
}

static int _event_cb(player_event_t e, void *data)
{
    enna_mediaplayer_event_t event;

    if (e == PLAYER_EVENT_PLAYBACK_FINISHED)
    {
        event = ENNA_MP_EVENT_EOF;
        ecore_pipe_write(mod->pipe, &event, sizeof(event));
    }
    return 0;
}

static int _x_event_key_down(void *data, int type, void *event)
{
    Ecore_X_Event_Key_Down *e;
    e = event;
    /*
       HACK !
       If e->win is the same than enna winid, don't manage this event
       ecore_evas_x will do this for us.
       But if e->win is different than enna winid event are sent to
       libplayer subwindow and we must broadcast this event to Evas
    */
    if (e->win != enna->ee_winid)
    {
        enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME,
                 "Ecore_X_Event_Key_Down %s", e->keyname);
        evas_event_feed_key_down(enna->evas, e->keyname, e->keysymbol,
                                 e->key_compose, NULL, e->time, NULL);
    }
   return 1;
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
        _class_video_resize,
        _class_event_cb_set,
        NULL
    }
};

/*****************************************************************************/
/*                          Public Module API                                */
/*****************************************************************************/

Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    ENNA_MODULE_BACKEND,
    "backend_libplayer"
};

void module_init(Enna_Module *em)
{
    Enna_Config_Data *cfgdata;
    char *value = NULL;

    player_type_t type = PLAYER_TYPE_MPLAYER;
    player_vo_t vo = PLAYER_VO_AUTO;
    player_ao_t ao = PLAYER_AO_AUTO;
    player_verbosity_level_t verbosity = PLAYER_MSG_WARNING;

    if (!em)
        return;

    /* Load Config file values */
    cfgdata = enna_config_module_pair_get("libplayer");

    enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, "parameters:");

    if (cfgdata)
    {
        Eina_List *l;

        for (l = cfgdata->pair; l; l = l->next)
        {
            Config_Pair *pair = l->data;

            if (!strcmp("type", pair->key))
            {
                enna_config_value_store(&value, "type",
                                        ENNA_CONFIG_STRING, pair);
                enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, " * type: %s", value);

                if (!strcmp("gstreamer", value))
                    type = PLAYER_TYPE_GSTREAMER;
                else if (!strcmp("mplayer", value))
                    type = PLAYER_TYPE_MPLAYER;
                else if (!strcmp("vlc", value))
                    type = PLAYER_TYPE_VLC;
                else if (!strcmp("xine", value))
                    type = PLAYER_TYPE_XINE;
                else
                    enna_log(ENNA_MSG_WARNING, ENNA_MODULE_NAME,
                             "   - unknown type, 'mplayer' used instead");
            }
            else if (!strcmp("video_out", pair->key))
            {
                enna_config_value_store(&value, "video_out",
                                        ENNA_CONFIG_STRING, pair);
                enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME,
                         " * video out: %s", value);

                if (!strcmp("auto", value))
                    vo = PLAYER_VO_AUTO;
                else if (!strcmp("x11", value))
                    vo = PLAYER_VO_X11;
                else if (!strcmp("xv", value))
                    vo = PLAYER_VO_XV;
                else if (!strcmp("gl", value))
                    vo = PLAYER_VO_GL;
                else if (!strcmp("fb", value))
                    vo = PLAYER_VO_FB;
                else
                    enna_log(ENNA_MSG_WARNING, ENNA_MODULE_NAME,
                             "   - unknown video_out, 'auto' used instead");
            }
            else if (!strcmp("audio_out", pair->key))
            {
                enna_config_value_store(&value, "audio_out",
                                        ENNA_CONFIG_STRING, pair);
                enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME,
                         " * audio out: %s", value);

                if (!strcmp("auto", value))
                    ao = PLAYER_AO_AUTO;
                else if (!strcmp("alsa", value))
                    ao = PLAYER_AO_ALSA;
                else if (!strcmp("oss", value))
                    ao = PLAYER_AO_OSS;
                else
                    enna_log(ENNA_MSG_WARNING, ENNA_MODULE_NAME,
                             "   - unknown audio_out, 'auto' used instead");
            }
            else if (!strcmp("verbosity", pair->key))
            {
                enna_config_value_store(&value, "verbosity",
                                        ENNA_CONFIG_STRING, pair);
                enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME,
                         " * verbosity level: %s", value);

                if (!strcmp("verbose", value))
                    verbosity = PLAYER_MSG_VERBOSE;
                else if (!strcmp("info", value))
                    verbosity = PLAYER_MSG_INFO;
                else if (!strcmp("warning", value))
                    verbosity = PLAYER_MSG_WARNING;
                else if (!strcmp("error", value))
                    verbosity = PLAYER_MSG_ERROR;
                else if (!strcmp("critical", value))
                    verbosity = PLAYER_MSG_CRITICAL;
                else if (!strcmp("none", value))
                    verbosity = PLAYER_MSG_NONE;
                else
                    enna_log(ENNA_MSG_WARNING, ENNA_MODULE_NAME,
                             "   - unknown verbosity, 'warning' used instead");
            }
        }
    }

    if (!value)
        enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME,
                 " * use all parameters by default");

    mod = calloc(1, sizeof(Enna_Module_libplayer));
    mod->em = em;
    mod->evas = em->evas;

    mod->key_down_event_handler =
        ecore_event_handler_add(ECORE_X_EVENT_KEY_DOWN, _x_event_key_down, NULL);
    mod->pipe = ecore_pipe_add(_pipe_read, NULL);

    mod->player =
        player_init(type, ao, vo, verbosity, enna->ee_winid, _event_cb);

    if (!mod->player)
    {
        enna_log(ENNA_MSG_ERROR, ENNA_MODULE_NAME,
                "libplayer module initialization");
        return;
    }

    enna_mediaplayer_backend_register(&class);
    mod->uri = NULL;
    mod->label = NULL;
}

void module_shutdown(Enna_Module *em)
{
    _class_shutdown(0);
    free(mod);
}
