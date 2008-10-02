#ifndef __ENNA_MEDIAPLAYER_H__
#define __ENNA_MEDIAPLAYER_H__

#include "enna.h"

typedef enum
{
    ENNA_BACKEND_EMOTION,
    ENNA_BACKEND_LIBPLAYER,
} enna_mediaplayer_backend_t;

typedef enum
{
    ENNA_MP_EVENT_EOF,
} enna_mediaplayer_event_t;

typedef struct _Enna_Class_MediaplayerBackend Enna_Class_MediaplayerBackend;

struct _Enna_Class_MediaplayerBackend
{
    const char *name;
    int pri;
    struct
    {
        void (*class_init)(int dummy);
        void (*class_shutdown)(int dummy);
        int (*class_file_set)(const char *uri, const char *label);
        int (*class_play)(void);
        int (*class_seek)(double percent);
        int (*class_stop)(void);
        int (*class_pause)(void);
        double (*class_position_get)(void);
        double (*class_length_get)(void);
        void (*class_snapshot)(const char *uri, const char *file);
        Enna_Metadata *(*class_metadata_get)(void);
        void (*class_video_resize)(int x, int y, int w, int h);
        void (*class_event_cb_set)(void (*event_cb)(void*data, enna_mediaplayer_event_t event), void *data);
        Evas_Object *(*class_video_obj_get)(void);
    } func;
};

/* Mediaplayer event */
EAPI int ENNA_EVENT_MEDIAPLAYER_EOS;
EAPI int ENNA_EVENT_MEDIAPLAYER_METADATA_UPDATE;

/* Mediaplayer API functions */
EAPI int enna_mediaplayer_init(void);
EAPI void enna_mediaplayer_shutdown(void);
EAPI void enna_mediaplayer_uri_append(const char *uri, const char *label);
EAPI int enna_mediaplayer_select_nth(int n);
EAPI int enna_mediaplayer_selected_get(void);
EAPI void enna_mediaplayer_snapshot(const char *uri, const char *file);
EAPI Enna_Metadata *enna_mediaplayer_metadata_get(void);
EAPI int enna_mediaplayer_play(void);
EAPI int enna_mediaplayer_stop(void);
EAPI int enna_mediaplayer_pause(void);
EAPI int enna_mediaplayer_next(void);
EAPI int enna_mediaplayer_prev(void);
EAPI double enna_mediaplayer_position_get(void);
EAPI double enna_mediaplayer_length_get(void);
EAPI int enna_mediaplayer_seek(double percent);
EAPI void enna_mediaplayer_video_resize(int x, int y, int w, int h);
EAPI int enna_mediaplayer_playlist_load(const char *filename);
EAPI int enna_mediaplayer_playlist_save(const char *filename);
EAPI void enna_mediaplayer_playlist_clear(void);
EAPI int enna_mediaplayer_playlist_count(void);
EAPI Evas_Object *enna_mediaplayer_video_obj_get(void);
EAPI int
        enna_mediaplayer_backend_register(Enna_Class_MediaplayerBackend *class);
#endif
