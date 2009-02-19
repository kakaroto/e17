/* Interface */

#define _GNU_SOURCE
#include "enna.h"
#include "codecs.h"
#include <player.h>

#define ENNA_MODULE_NAME        "metadata_libplayer"
#define ENNA_GRABBER_NAME       "libplayer"
#define ENNA_GRABBER_PRIORITY   2

#define SNAPSHOTS_PATH          "snapshots"

#define URI_TYPE_FTP  "ftp://"
#define URI_TYPE_HTTP "http://"
#define URI_TYPE_MMS  "mms://"
#define URI_TYPE_RTP  "rtp://"
#define URI_TYPE_RTSP "rtsp://"
#define URI_TYPE_SMB  "smb://"
#define URI_TYPE_TCP  "tcp://"
#define URI_TYPE_UDP  "udp://"
#define URI_TYPE_UNSV "unsv://"

typedef struct _Metadata_Module_libplayer
{
    Evas *evas;
    Enna_Module *em;
    player_t *player;
    Eina_List *bl_keywords;

} Metadata_Module_libplayer;

static Metadata_Module_libplayer *mod;

/*****************************************************************************/
/*                         Private Module API                                */
/*****************************************************************************/

static mrl_t *
set_network_stream (const char *uri, mrl_resource_t type)
{
    mrl_t *mrl;
    mrl_resource_network_args_t *args;

    args = calloc (1, sizeof (mrl_resource_network_args_t));
    args->url = strdup (uri);
    mrl = mrl_new (mod->player, type, args);

    return mrl;
}

static mrl_t *
set_local_stream (const char *uri)
{
    mrl_t *mrl;
    mrl_resource_local_args_t *args;

    args = calloc (1, sizeof (mrl_resource_local_args_t));
    args->location = strdup (uri);
    mrl = mrl_new (mod->player, MRL_RESOURCE_FILE, args);

    return mrl;
}

static int
set_mrl (const char *uri)
{
    mrl_t *mrl = NULL;

    /* try network streams */
    if (!strncmp (uri, URI_TYPE_FTP, strlen(URI_TYPE_FTP)))
        mrl = set_network_stream (uri, MRL_RESOURCE_FTP);
    else if (!strncmp (uri, URI_TYPE_HTTP, strlen(URI_TYPE_HTTP)))
        mrl = set_network_stream (uri, MRL_RESOURCE_HTTP);
    else if (!strncmp (uri, URI_TYPE_MMS, strlen(URI_TYPE_MMS)))
        mrl = set_network_stream (uri, MRL_RESOURCE_MMS);
    else if (!strncmp (uri, URI_TYPE_RTP, strlen(URI_TYPE_RTP)))
        mrl = set_network_stream (uri, MRL_RESOURCE_RTP);
    else if (!strncmp (uri, URI_TYPE_RTSP, strlen(URI_TYPE_RTSP)))
        mrl = set_network_stream (uri, MRL_RESOURCE_RTSP);
    else if (!strncmp (uri, URI_TYPE_SMB, strlen(URI_TYPE_SMB)))
        mrl = set_network_stream (uri, MRL_RESOURCE_SMB);
    else if (!strncmp (uri, URI_TYPE_TCP, strlen(URI_TYPE_TCP)))
        mrl = set_network_stream (uri, MRL_RESOURCE_TCP);
    else if (!strncmp (uri, URI_TYPE_UDP, strlen(URI_TYPE_UDP)))
        mrl = set_network_stream (uri, MRL_RESOURCE_UDP);
    else if (!strncmp (uri, URI_TYPE_UNSV, strlen(URI_TYPE_UNSV)))
        mrl = set_network_stream (uri, MRL_RESOURCE_UNSV);

    /* default is local files */
    if (!mrl)
        mrl = set_local_stream (uri);

    if (!mrl)
        return 1;

    player_mrl_set (mod->player, mrl);
    return 0;
}

static char *
clean_whitespace (char *str)
{
    char *clean, *clean_it;
    char *it;

    clean = malloc (strlen (str) + 1);
    if (!clean)
        return str;
    clean_it = clean;

    for (it = str; *it; it++)
    {
        if (!isspace (*it))
        {
            *clean_it = *it;
            clean_it++;
        }
        else if (!isspace (*(it + 1)) && *(it + 1) != '\0')
        {
            *clean_it = ' ';
            clean_it++;
        }
    }

    /* remove spaces after */
    while (clean_it > str && isspace (*(clean_it - 1)))
      clean_it--;
    *clean_it = '\0';

    /* remove spaces before */
    clean_it = clean;
    while (isspace (*clean_it))
      clean_it++;

    strcpy (str, clean_it);
    free (clean);
    return str;
}

static char *
proceed_blacklist (char *name)
{
    Eina_List *l;

    for (l = mod->bl_keywords; l; l = l->next)
    {
        const char *it = l->data;
        char *p = strcasestr (name, it);
        size_t size;
        if (!p)
            continue;

        size = strlen (it);
        if (!isgraph (*(p + size)) && (p == name || !isgraph (*(p - 1))))
            memset (p, ' ', size);
    }

    return name;
}

static char *
get_movie_name (const char *filename)
{
    char *it, *movie;
    char *path = strdup (filename);
    char *file = (char *) ecore_file_file_get (path);

    it = strrchr(file, '.');
    if (it) /* remove suffix? */
        *it = '\0';

    /* decrapify the movie's name */
    for (it = file; *it; it++)
        if (!isspace (*it)
            && (*it < '0' || *it > '9')
            && (*it < 'A' || *it > 'Z')
            && (*it < 'a' || *it > 'z')
            && (unsigned) *it <= 0x7F) /* limit to ASCII 7-bit */
            *it = ' ';

    proceed_blacklist (file);
    clean_whitespace (file);

    movie = strdup (file);
    free (path);

    return movie;
}

static void
libplayer_grab (Enna_Metadata *meta, int caps)
{
    char *track_nb;
    int frameduration = 0;
    char *codec_id;
    mrl_type_t type;

    if (!meta || !meta->uri)
        return;

    enna_log (ENNA_MSG_EVENT, ENNA_MODULE_NAME,
              "Grabbing info from %s", meta->uri);

    set_mrl (meta->uri);

    type = mrl_get_type (mod->player, NULL);
    if (type == MRL_TYPE_AUDIO)
        meta->type = ENNA_METADATA_AUDIO;
    else if (type == MRL_TYPE_VIDEO)
        meta->type = ENNA_METADATA_VIDEO;

    if (!meta->size)
        meta->size = mrl_get_size (mod->player, NULL);

    if (!meta->length)
        meta->length =
            mrl_get_property (mod->player, NULL, MRL_PROPERTY_LENGTH);

    if (!meta->title)
        meta->title = mrl_get_metadata (mod->player, NULL, MRL_METADATA_TITLE);

    if (caps & ENNA_GRABBER_CAP_AUDIO)
    {
        if (!meta->music->artist)
        {
            meta->music->artist = mrl_get_metadata (mod->player, NULL,
                                                    MRL_METADATA_ARTIST);
            enna_metadata_add_keywords (meta, meta->music->artist);
        }

        if (!meta->music->album)
        {
            meta->music->album = mrl_get_metadata (mod->player, NULL,
                                                   MRL_METADATA_ALBUM);
            enna_metadata_add_keywords (meta, meta->music->album);
        }

        if (!meta->music->year)
            meta->music->year = mrl_get_metadata (mod->player, NULL,
                                                  MRL_METADATA_YEAR);
        if (!meta->music->genre)
            meta->music->genre = mrl_get_metadata (mod->player, NULL,
                                                   MRL_METADATA_GENRE);
        if (!meta->music->comment)
            meta->music->comment = mrl_get_metadata (mod->player, NULL,
                                                     MRL_METADATA_COMMENT);

        if (!meta->music->track)
        {
            track_nb =
                mrl_get_metadata (mod->player, NULL, MRL_METADATA_TRACK);
            if (track_nb)
            {
                meta->music->track = atoi (track_nb);
                free (track_nb);
            }
        }

        if (!meta->music->codec)
        {
            codec_id = mrl_get_audio_codec (mod->player, NULL);
            meta->music->codec = get_codec_name (codec_id);
            free (codec_id);
        }

        if (!meta->music->bitrate)
            meta->music->bitrate =
                mrl_get_property (mod->player, NULL,
                                  MRL_PROPERTY_AUDIO_BITRATE);

        if (!meta->music->channels)
            meta->music->channels =
                mrl_get_property (mod->player, NULL,
                                  MRL_PROPERTY_AUDIO_CHANNELS);

        if (!meta->music->samplerate)
            meta->music->samplerate =
                mrl_get_property (mod->player, NULL,
                                  MRL_PROPERTY_AUDIO_SAMPLERATE);
    }

    if (caps & ENNA_GRABBER_CAP_VIDEO)
    {
        char *name = get_movie_name (meta->uri);
        enna_log (ENNA_MSG_INFO, ENNA_MODULE_NAME, "movie's name: \"%s\"", name);
        enna_metadata_add_keywords (meta, name);

        if (!meta->title)
            meta->title = name;
        else
            free (name);

        if (!meta->video->codec)
        {
            codec_id = mrl_get_video_codec (mod->player, NULL);
            meta->video->codec = get_codec_name (codec_id);
            free (codec_id);
        }

        if (!meta->video->width)
            meta->video->width =
                mrl_get_property (mod->player, NULL,
                                  MRL_PROPERTY_VIDEO_WIDTH);

        if (!meta->video->height)
            meta->video->height =
                mrl_get_property (mod->player, NULL,
                                  MRL_PROPERTY_VIDEO_HEIGHT);

        if (!meta->video->channels)
            meta->video->channels =
                mrl_get_property (mod->player, NULL,
                                  MRL_PROPERTY_VIDEO_CHANNELS);

        if (!meta->video->streams)
            meta->video->streams =
                mrl_get_property (mod->player, NULL,
                                  MRL_PROPERTY_VIDEO_STREAMS);

        if (!meta->video->framerate)
        {
            frameduration =
                mrl_get_property (mod->player, NULL,
                                  MRL_PROPERTY_VIDEO_FRAMEDURATION);
            if (frameduration)
                meta->video->framerate =
                    PLAYER_VIDEO_FRAMEDURATION_RATIO_DIV / frameduration;
        }

        if (!meta->video->bitrate)
            meta->video->bitrate =
                mrl_get_property (mod->player, NULL,
                                  MRL_PROPERTY_VIDEO_BITRATE);
    }

    if (caps & ENNA_GRABBER_CAP_COVER)
    {
        /* snapshot */
        if (enna->use_snapshots && !meta->snapshot)
        {
            char dst[1024];

            memset (dst, '\0', sizeof (dst));
            snprintf (dst, sizeof (dst), "%s/.enna/%s/%s.png",
                      enna_util_user_home_get(), SNAPSHOTS_PATH, meta->md5);

            if (!ecore_file_exists (dst))
            {
                double length;
                int sec;

                /* take snapshot at 15% of stream */
                length = mrl_get_property (mod->player, NULL,
                                           MRL_PROPERTY_LENGTH) / 1000.0;
                sec = (int) (length * 15 / 100);
                mrl_video_snapshot (mod->player, NULL, sec,
                                    MRL_SNAPSHOT_PNG, dst);
            }
            meta->snapshot = strdup (dst);
        }
    }
}

static Enna_Metadata_Grabber grabber = {
    ENNA_GRABBER_NAME,
    ENNA_GRABBER_PRIORITY,
    0,
    ENNA_GRABBER_CAP_AUDIO | ENNA_GRABBER_CAP_VIDEO | ENNA_GRABBER_CAP_COVER,
    libplayer_grab,
};

/*****************************************************************************/
/*                          Public Module API                                */
/*****************************************************************************/

Enna_Module_Api module_api =
{
    ENNA_MODULE_VERSION,
    ENNA_MODULE_METADATA,
    ENNA_MODULE_NAME
};

void module_init(Enna_Module *em)
{
    Enna_Config_Data *cfgdata;
    player_type_t type = PLAYER_TYPE_MPLAYER;
    player_verbosity_level_t verbosity = PLAYER_MSG_WARNING;
    char *value = NULL;
    Eina_List *bl_keywords = NULL;

    if (!em)
        return;

    cfgdata = enna_config_module_pair_get("libplayer");
    if (cfgdata)
    {
        Eina_List *l;

        for (l = cfgdata->pair; l; l = l->next)
        {
            Config_Pair *pair = l->data;

            if (!strcmp ("type", pair->key))
            {
                enna_config_value_store (&value, "type",
                                         ENNA_CONFIG_STRING, pair);
                enna_log (ENNA_MSG_INFO, ENNA_MODULE_NAME,
                          " * type: %s", value);

                if (!strcmp ("gstreamer", value))
                    type = PLAYER_TYPE_GSTREAMER;
                else if (!strcmp ("mplayer", value))
                    type = PLAYER_TYPE_MPLAYER;
                else if (!strcmp ("vlc", value))
                    type = PLAYER_TYPE_VLC;
                else if (!strcmp ("xine", value))
                    type = PLAYER_TYPE_XINE;
                else
                    enna_log (ENNA_MSG_WARNING, ENNA_MODULE_NAME,
                              "   - unknown type, 'mplayer' used instead");
            }
            else if (!strcmp ("verbosity", pair->key))
            {
                enna_config_value_store (&value, "verbosity",
                                         ENNA_CONFIG_STRING, pair);
                enna_log (ENNA_MSG_INFO, ENNA_MODULE_NAME,
                          " * verbosity level: %s", value);

                if (!strcmp ("verbose", value))
                    verbosity = PLAYER_MSG_VERBOSE;
                else if (!strcmp ("info", value))
                    verbosity = PLAYER_MSG_INFO;
                else if (!strcmp ("warning", value))
                    verbosity = PLAYER_MSG_WARNING;
                else if (!strcmp ("error", value))
                    verbosity = PLAYER_MSG_ERROR;
                else if (!strcmp ("critical", value))
                    verbosity = PLAYER_MSG_CRITICAL;
                else if (!strcmp ("none", value))
                    verbosity = PLAYER_MSG_NONE;
                else
                    enna_log(ENNA_MSG_WARNING, ENNA_MODULE_NAME,
                             "   - unknown verbosity, 'warning' used instead");
            }
            enna_config_value_store (&bl_keywords, "blacklist_keywords",
                                     ENNA_CONFIG_STRING_LIST, pair);
        }
    }

    mod = calloc (1, sizeof (Metadata_Module_libplayer));

    mod->em = em;
    mod->evas = em->evas;
    mod->bl_keywords = bl_keywords;

    mod->player = player_init (type, PLAYER_AO_NULL, PLAYER_VO_NULL,
                               verbosity, 0, NULL);
    if (!mod->player)
    {
        enna_log (ENNA_MSG_ERROR, ENNA_MODULE_NAME,
                  "libplayer metadata module initialization");
        return;
    }

    enna_metadata_add_grabber (&grabber);
}

void module_shutdown(Enna_Module *em)
{
    //enna_metadata_remove_grabber (ENNA_GRABBER_NAME);
    player_playback_stop (mod->player);
    player_uninit (mod->player);
    if (mod->bl_keywords)
        eina_list_free (mod->bl_keywords);
    free(mod);
}
