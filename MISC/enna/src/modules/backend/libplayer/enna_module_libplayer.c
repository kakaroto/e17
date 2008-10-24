/* Interface */

#include "enna.h"
#include "codecs.h"
#include <player.h>

#define ENNA_MODULE_NAME "libplayer"

#define URI_TYPE_FTP  "ftp://"
#define URI_TYPE_HTTP "http://"
#define URI_TYPE_MMS  "mms://"
#define URI_TYPE_RTP  "rtp://"
#define URI_TYPE_RTSP "rtsp://"
#define URI_TYPE_SMB  "smb://"
#define URI_TYPE_TCP  "tcp://"
#define URI_TYPE_UDP  "udp://"
#define URI_TYPE_UNSV "unsv://"
typedef struct _Enna_Module_libplayer
{
    Evas *evas;
    player_t *player;
    Enna_Module *em;
    void (*event_cb)(void *data, enna_mediaplayer_event_t event);
    void *event_cb_data;
    char *uri;
    char *label;
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
    player_playback_seek(mod->player, (int) (100 * percent),
            PLAYER_PB_SEEK_PERCENT);
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

    /*
     * NOTE: it needs a fix in libplayer because get_time_pos changes the state
     *       of MPlayer and the pause is lost
     */
    if (player_playback_get_state(mod->player) == PLAYER_PB_STATE_PLAY)
        time_pos = (double) player_get_time_pos(mod->player) / 1000.0;
    return time_pos < 0.0 ? 0.0 : time_pos;
}

static double _class_length_get()
{
    return (double) mrl_get_property(mod->player, NULL, MRL_PROPERTY_LENGTH)
            / 1000.0;
}

static void _class_snapshot(const char *uri, const char *file)
{
    int sec;

    if (!uri || !file)
        return;

    /* take snapshot at 15% of stream */
    sec = (int) (_class_length_get() * 15 / 100);

    mrl_video_snapshot(mod->player, NULL, sec, MRL_SNAPSHOT_PNG, file);
}

static Enna_Metadata *_class_metadata_get(void)
{
    Enna_Metadata *meta;
    char *track_nb;
    int frameduration = 0;
    char *codec_id;
    
    meta = enna_metadata_new();

    meta->uri = strdup(mod->uri+7);
    meta->size = mrl_get_size(mod->player, NULL);
    meta->length = mrl_get_property(mod->player, NULL, MRL_PROPERTY_LENGTH);

    meta->title = mod->label ? strdup(mod->label) : mrl_get_metadata(
            mod->player, NULL, MRL_METADATA_TITLE);
    meta->music->artist = mrl_get_metadata(mod->player, NULL,
            MRL_METADATA_ARTIST);
    meta->music->album
            = mrl_get_metadata(mod->player, NULL, MRL_METADATA_ALBUM);
    meta->music->year = mrl_get_metadata(mod->player, NULL, MRL_METADATA_YEAR);
    meta->music->genre
            = mrl_get_metadata(mod->player, NULL, MRL_METADATA_GENRE);
    meta->music->comment = mrl_get_metadata(mod->player, NULL,
            MRL_METADATA_COMMENT);
    meta->music->discid = NULL;

    track_nb = mrl_get_metadata(mod->player, NULL, MRL_METADATA_TRACK);
    if (track_nb)
    {
        meta->music->track = atoi(track_nb);
        free(track_nb);
    }
    else
        meta->music->track = 0;

    codec_id = mrl_get_audio_codec(mod->player, NULL);
    meta->music->codec = get_codec_name (codec_id);
    free (codec_id);
    
    meta->music->bitrate = mrl_get_property(mod->player, NULL,
            MRL_PROPERTY_AUDIO_BITRATE);
    meta->music->channels = mrl_get_property(mod->player, NULL,
            MRL_PROPERTY_AUDIO_CHANNELS);
    meta->music->samplerate = mrl_get_property(mod->player, NULL,
            MRL_PROPERTY_AUDIO_SAMPLERATE);

    codec_id = mrl_get_video_codec(mod->player, NULL);
    meta->video->codec = get_codec_name (codec_id);
    free (codec_id);
    
    meta->video->width = mrl_get_property(mod->player, NULL,
            MRL_PROPERTY_VIDEO_WIDTH);
    meta->video->height = mrl_get_property(mod->player, NULL,
            MRL_PROPERTY_VIDEO_HEIGHT);
    meta->video->channels = mrl_get_property(mod->player, NULL,
            MRL_PROPERTY_VIDEO_CHANNELS);
    meta->video->streams = mrl_get_property(mod->player, NULL,
            MRL_PROPERTY_VIDEO_STREAMS);
    frameduration = mrl_get_property(mod->player, NULL,
            MRL_PROPERTY_VIDEO_FRAMEDURATION);
    if (frameduration)
        meta->video->framerate = PLAYER_VIDEO_FRAMEDURATION_RATIO_DIV
                / frameduration;
    meta->video->bitrate = mrl_get_property(mod->player, NULL,
            MRL_PROPERTY_VIDEO_BITRATE);

    return meta;
}

static void _class_video_resize(int x, int y, int w, int h)
{
    int flags = PLAYER_X_WINDOW_X | PLAYER_X_WINDOW_Y |
                PLAYER_X_WINDOW_W | PLAYER_X_WINDOW_H;

    /* if w or h is 0, libplayer guess the best size automatically */
    player_x_window_set_properties(mod->player, x, y, w, h, flags);
}

static void _class_event_cb_set(void (*event_cb)(void *data, enna_mediaplayer_event_t event), void *data)
{
    /* FIXME: function to call when end of stream is send by libplayer */

    mod->event_cb_data = data;
    mod->event_cb = event_cb;
}

static int _event_cb(player_event_t e, void *data)
{
    if (e == PLAYER_EVENT_PLAYBACK_FINISHED)
    {
        enna_log(ENNA_MSG_EVENT, ENNA_MODULE_NAME, "PLAYBACK FINISHED");
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
  { _class_init,
    _class_shutdown,
    _class_file_set,
    _class_play,
    _class_seek,
    _class_stop,
    _class_pause,
    _class_position_get,
    _class_length_get,
    _class_snapshot,
    _class_metadata_get,
    _class_video_resize,
    _class_event_cb_set,
    NULL
  }
};

/*****************************************************************************/
/*                          Public Module API                                */
/*****************************************************************************/

EAPI Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    "libplayer"
};

EAPI void module_init(Enna_Module *em)
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
                enna_config_value_store(&value, "type", ENNA_CONFIG_STRING,
                        pair);
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
                enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, " * video out: %s",
                        value);

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
                enna_log(ENNA_MSG_INFO, ENNA_MODULE_NAME, " * audio out: %s",
                        value);

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
    mod->player = player_init(type, ao, vo, verbosity, enna->ee_winid,
            _event_cb);

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

EAPI void module_shutdown(Enna_Module *em)
{
    _class_shutdown(0);
    free(mod);
}
