/*
 * mediaplayer.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * mediaplayer.c is free software copyrighted by Nicolas Aguirre.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Nicolas Aguirre'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * mediaplayer.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Nicolas Aguirre OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "enna.h"

typedef struct list_item_s
{
    const char *uri;
    const char *label;
} list_item_t;


typedef struct _Enna_Mediaplayer Enna_Mediaplayer;

struct _Enna_Mediaplayer
{
    PLAY_STATE play_state;
    Enna_Class_MediaplayerBackend *class;
};

static Enna_Mediaplayer *_mediaplayer;

static void
_event_cb(void *data, enna_mediaplayer_event_t event);

/* externally accessible functions */
int
enna_mediaplayer_init(void)
{
    Enna_Module *em;
    char *backend_name = NULL;

    if (!strcmp(enna_config->backend, "emotion"))
    {
#ifdef BUILD_BACKEND_EMOTION
        enna_log (ENNA_MSG_INFO, NULL, "Using Emotion Backend");
        backend_name = "emotion";
#else
        enna_log(ENNA_MSG_ERROR, NULL, "Backend selected not built !");
        return -1;
#endif
    }
    else if (!strcmp(enna_config->backend, "libplayer"))
    {
#ifdef BUILD_BACKEND_LIBPLAYER
        enna_log (ENNA_MSG_INFO, NULL, "Using libplayer Backend");
        backend_name = "libplayer";
#else
        enna_log(ENNA_MSG_ERROR, NULL, "Backend selected not built !");
        return -1;
#endif
    }
    else
    {
        enna_log(ENNA_MSG_ERROR, NULL,
                 "Unknown backend (%s)!", enna_config->backend);
        return -1;
    }

    _mediaplayer = calloc(1, sizeof(Enna_Mediaplayer));
    em = enna_module_open(backend_name, ENNA_MODULE_BACKEND, enna->evas);
    enna_module_enable(em);
    _mediaplayer->play_state = STOPPED;

    /* Create Ecore Event ID */
    ENNA_EVENT_MEDIAPLAYER_EOS = ecore_event_type_new();
    ENNA_EVENT_MEDIAPLAYER_METADATA_UPDATE = ecore_event_type_new();
    ENNA_EVENT_MEDIAPLAYER_START = ecore_event_type_new();
    ENNA_EVENT_MEDIAPLAYER_STOP = ecore_event_type_new();
    ENNA_EVENT_MEDIAPLAYER_PAUSE = ecore_event_type_new();
    ENNA_EVENT_MEDIAPLAYER_UNPAUSE = ecore_event_type_new();
    ENNA_EVENT_MEDIAPLAYER_PREV = ecore_event_type_new();
    ENNA_EVENT_MEDIAPLAYER_NEXT = ecore_event_type_new();
    ENNA_EVENT_MEDIAPLAYER_SEEK = ecore_event_type_new();
    return 0;
}
void
enna_mediaplayer_shutdown()
{
    free(_mediaplayer);
}

void
enna_mediaplayer_uri_append(Enna_Playlist *enna_playlist,const char *uri, const char *label)
{
    list_item_t *item = calloc(1, sizeof(list_item_t));
    item->uri = uri ? strdup(uri) : NULL;
    item->label = label ? strdup(label) : NULL;
    enna_playlist->playlist = eina_list_append(enna_playlist->playlist, item);
}

int
enna_mediaplayer_play(Enna_Playlist *enna_playlist)
{

    switch (_mediaplayer->play_state)
    {
        case STOPPED:
        {
            list_item_t *item;
            item = eina_list_nth(enna_playlist->playlist, enna_playlist->selected);
            if (_mediaplayer->class->func.class_stop)
                _mediaplayer->class->func.class_stop();
            if (item && item->uri && _mediaplayer->class->func.class_file_set)
                _mediaplayer->class->func.class_file_set(item->uri, item->label);
            if (_mediaplayer->class->func.class_play)
                _mediaplayer->class->func.class_play();
            _mediaplayer->play_state = PLAYING;
            ecore_event_add(ENNA_EVENT_MEDIAPLAYER_START, NULL, NULL, NULL);
        }
            break;
        case PLAYING:
            enna_mediaplayer_pause();
            break;
        case PAUSE:

            if (_mediaplayer->class->func.class_play)
                _mediaplayer->class->func.class_play();
            _mediaplayer->play_state = PLAYING;
            ecore_event_add(ENNA_EVENT_MEDIAPLAYER_UNPAUSE, NULL, NULL, NULL);
            break;
        default:
            break;

    }

    return 0;
}

int
enna_mediaplayer_select_nth(Enna_Playlist *enna_playlist,int n)
{
    list_item_t *item;
    if (n < 0 || n > eina_list_count(enna_playlist->playlist) - 1)
        return -1;
    item = eina_list_nth(enna_playlist->playlist, n);
    enna_log(ENNA_MSG_INFO, NULL, "select %d", n);
    if (item && item->uri && _mediaplayer->class->func.class_file_set)
        _mediaplayer->class->func.class_file_set(item->uri, item->label);
    enna_playlist->selected = n;
    return 0;
}

int
enna_mediaplayer_selected_get(Enna_Playlist *enna_playlist)
{
    return enna_playlist->selected;
}

int
enna_mediaplayer_stop(void)
{

    printf("Stop\n");
    if (_mediaplayer->class)
    {
        if (_mediaplayer->class->func.class_stop)
            _mediaplayer->class->func.class_stop();
        _mediaplayer->play_state = STOPPED;
        ecore_event_add(ENNA_EVENT_MEDIAPLAYER_STOP, NULL, NULL, NULL);
    }
    return 0;
}

int
enna_mediaplayer_pause(void)
{
    if (_mediaplayer->class)
    {
        if (_mediaplayer->class->func.class_pause)
            _mediaplayer->class->func.class_pause();
        _mediaplayer->play_state = PAUSE;
        ecore_event_add(ENNA_EVENT_MEDIAPLAYER_PAUSE, NULL, NULL, NULL);
    }
    return 0;
}

int
enna_mediaplayer_next(Enna_Playlist *enna_playlist)
{
    list_item_t *item;

    enna_playlist->selected++;
    if (enna_playlist->selected > eina_list_count(enna_playlist->playlist) - 1)
    {
        enna_playlist->selected--;
        return -1;
    }
    item = eina_list_nth(enna_playlist->playlist, enna_playlist->selected);
    enna_log(ENNA_MSG_INFO, NULL, "select %d", enna_playlist->selected);
    if (item)
    {
        enna_mediaplayer_stop();
        if (item->uri && _mediaplayer->class->func.class_file_set)
            _mediaplayer->class->func.class_file_set(item->uri, item->label);
        enna_mediaplayer_play(enna_playlist);
        ecore_event_add(ENNA_EVENT_MEDIAPLAYER_NEXT, NULL, NULL, NULL);
    }

    return 0;
}

int
enna_mediaplayer_prev(Enna_Playlist *enna_playlist)
{
    list_item_t *item;

    enna_playlist->selected--;
    if (enna_playlist->selected < 0)
    {
            enna_playlist->selected = 0;
        return -1;
    }
    item = eina_list_nth(enna_playlist->playlist, enna_playlist->selected);
    enna_log(ENNA_MSG_INFO, NULL, "select %d", enna_playlist->selected);
    if (item)
    {
        enna_mediaplayer_stop();
        if (item->uri && _mediaplayer->class->func.class_file_set)
            _mediaplayer->class->func.class_file_set(item->uri, item->label);
        enna_mediaplayer_play(enna_playlist);
        ecore_event_add(ENNA_EVENT_MEDIAPLAYER_PREV, NULL, NULL, NULL);
    }
    return 0;
}

double
enna_mediaplayer_position_get(void)
{
    if (_mediaplayer->play_state == PAUSE || _mediaplayer->play_state
            == PLAYING)
    {
        if (_mediaplayer->class->func.class_position_get)
            return _mediaplayer->class->func.class_position_get();
    }
    return 0.0;
}

double
enna_mediaplayer_length_get(void)
{
    if (_mediaplayer->play_state == PAUSE || _mediaplayer->play_state
            == PLAYING)
    {
        if (_mediaplayer->class->func.class_length_get)
            return _mediaplayer->class->func.class_length_get();
    }
    return 0.0;
}

int
enna_mediaplayer_seek(double percent)
{
    enna_log(ENNA_MSG_INFO, NULL, "Seeking to: %d%%", (int) (100 * percent));
    if (_mediaplayer->play_state == PAUSE || _mediaplayer->play_state
            == PLAYING)
    {
            Enna_Event_Mediaplayer_Seek_Data *ev;
            ev = calloc(1, sizeof(Enna_Event_Mediaplayer_Seek_Data));
            if (!ev)
                return 0;
            ev->seek_value=percent;
            ecore_event_add(ENNA_EVENT_MEDIAPLAYER_SEEK, ev, NULL, NULL);
            if (_mediaplayer->class->func.class_seek)
                return _mediaplayer->class->func.class_seek(percent);
    }
    return 0;
}

void
enna_mediaplayer_video_resize(int x, int y, int w, int h)
{
    if (_mediaplayer->class && _mediaplayer->class->func.class_video_resize)
        _mediaplayer->class->func.class_video_resize(x, y, w, h);
}

int
enna_mediaplayer_playlist_load(const char *filename)
{
    return 0;
}

int
enna_mediaplayer_playlist_save(const char *filename)
{
    return 0;
}

void
enna_mediaplayer_playlist_clear(Enna_Playlist *enna_playlist)
{
    eina_list_free(enna_playlist->playlist);
    enna_playlist->playlist = NULL;
    enna_playlist->selected = 0;
}

Enna_Metadata *
enna_mediaplayer_metadata_get(Enna_Playlist *enna_playlist)
{
    list_item_t *item;
    item = eina_list_nth(enna_playlist->playlist, enna_playlist->selected);

    if (!item)
        return NULL;

    if (item->uri)
        return enna_metadata_new ((char *) item->uri);

    return NULL;
}

int
enna_mediaplayer_playlist_count(Enna_Playlist *enna_playlist)
{
    return eina_list_count(enna_playlist->playlist);
}

int
enna_mediaplayer_backend_register(Enna_Class_MediaplayerBackend *class)
{
    if (!class)
        return -1;
    _mediaplayer->class = class;
    if (class->func.class_init)
        class->func.class_init(0);

    if (class->func.class_event_cb_set)
        class->func.class_event_cb_set(_event_cb, NULL);

    return 0;
}

Evas_Object *
enna_mediaplayer_video_obj_get(void)
{
    if (_mediaplayer->class->func.class_video_obj_get)
    return _mediaplayer->class->func.class_video_obj_get();
    else
    return NULL;
}

static void
_event_cb(void *data, enna_mediaplayer_event_t event)
{
    switch (event)
    {
        case ENNA_MP_EVENT_EOF:
            enna_log(ENNA_MSG_INFO, NULL, "End of stream");
            ecore_event_add(ENNA_EVENT_MEDIAPLAYER_EOS, NULL, NULL, NULL);
            break;
        default:
            break;
    }
}

PLAY_STATE
enna_mediaplayer_state_get(void)
{
    return(_mediaplayer->play_state);
}

Enna_Playlist *
enna_mediaplayer_playlist_create(void)
{
    Enna_Playlist  *enna_playlist = calloc(1, sizeof(Enna_Playlist));
    enna_playlist->selected=0;
    enna_playlist->playlist=NULL;
    return enna_playlist;
}

void
enna_mediaplayer_playlist_free(Enna_Playlist *enna_playlist)
{
    eina_list_free(enna_playlist->playlist);
    free(enna_playlist);
}

void
enna_mediaplayer_playlist_stop_clear(Enna_Playlist *enna_playlist)
{
    enna_mediaplayer_playlist_clear(enna_playlist);
    if (_mediaplayer->class)
    {
        if (_mediaplayer->class->func.class_stop)
            _mediaplayer->class->func.class_stop();
        _mediaplayer->play_state = STOPPED;
        ecore_event_add(ENNA_EVENT_MEDIAPLAYER_STOP, NULL, NULL, NULL);
    }
}
