#ifndef __ENNA_COVERS_H__
#define __ENNA_COVERS_H__

typedef struct _Enna_Class_CoverPlugin Enna_Class_CoverPlugin;

struct _Enna_Class_CoverPlugin
{
    const char *name;
    char *(*music_cover_get)(const char *artist, const char *album);
    char *(*movie_cover_get)(const char *movie);
};

int enna_cover_plugin_register(Enna_Class_CoverPlugin *class);

char *enna_cover_album_get(const char *artist, const char *album,
        const char *filename);
char *enna_cover_video_get(const char *filename);
#endif
