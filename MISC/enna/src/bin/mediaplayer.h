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
int ENNA_EVENT_MEDIAPLAYER_EOS;
int ENNA_EVENT_MEDIAPLAYER_METADATA_UPDATE;

/* Mediaplayer API functions */
int enna_mediaplayer_init(void);
void enna_mediaplayer_shutdown(void);
void enna_mediaplayer_uri_append(const char *uri, const char *label);
int enna_mediaplayer_select_nth(int n);
int enna_mediaplayer_selected_get(void);
void enna_mediaplayer_snapshot(const char *uri, const char *file);
Enna_Metadata *enna_mediaplayer_metadata_get(void);
int enna_mediaplayer_play(void);
int enna_mediaplayer_stop(void);
int enna_mediaplayer_pause(void);
int enna_mediaplayer_next(void);
int enna_mediaplayer_prev(void);
double enna_mediaplayer_position_get(void);
double enna_mediaplayer_length_get(void);
int enna_mediaplayer_seek(double percent);
void enna_mediaplayer_video_resize(int x, int y, int w, int h);
int enna_mediaplayer_playlist_load(const char *filename);
int enna_mediaplayer_playlist_save(const char *filename);
void enna_mediaplayer_playlist_clear(void);
int enna_mediaplayer_playlist_count(void);
Evas_Object *enna_mediaplayer_video_obj_get(void);
int
        enna_mediaplayer_backend_register(Enna_Class_MediaplayerBackend *class);
#endif
