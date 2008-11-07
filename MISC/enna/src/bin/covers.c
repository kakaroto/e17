/*
 * enna_covers.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_covers.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_covers.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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

static Enna_Class_CoverPlugin *cover_class = NULL;

int enna_cover_plugin_register(Enna_Class_CoverPlugin *class)
{
    if (!class)
        return -1;

    cover_class = class;

    return 0;
}

static char * cover_get_from_picture_file(const char *filename)
{
    const char *known_filenames[] =
    { "cover", "front" };

    const char *known_extensions[] =
    { "jpg", "jpeg", "png" };

    char *dir, *ret = NULL;
    char cover[1024];
    int i, j;

    enna_log(ENNA_MSG_EVENT, NULL, "Trying to get cover from picture files");

    if (!filename)
        return NULL;

    dir = ecore_file_dir_get(filename);
    if (!ecore_file_can_read(dir))
        goto out;

    for (i = 0; i < ARRAY_NB_ELEMENTS(known_extensions); i++)
    {
        memset(cover, '\0', sizeof (cover));
        snprintf(cover, sizeof (cover), "%s/%s.%s", dir, filename,
                known_extensions[i]);

        if (ecore_file_exists(cover))
        {
            ret = strdup(cover);
            goto out;
        }

        for (j = 0; j < ARRAY_NB_ELEMENTS(known_filenames); j++)
        {
            memset(cover, '\0', sizeof (cover));
            snprintf(cover, sizeof (cover), "%s/%s.%s", dir,
                    known_filenames[j], known_extensions[i]);

            if (!ecore_file_exists(cover))
                continue;

            ret = strdup(cover);
            goto out;
        }
    }

out:
    free(dir);
    return ret;
}

static char * cover_get_from_saved_file(char *keywords)
{
    char *md5;
    char cover[1024];

    enna_log(ENNA_MSG_EVENT, NULL,
            "Trying to get cover from previously saved cover file");

    if (!keywords)
        return NULL;

    md5 = md5sum(keywords);
    memset(cover, '\0', sizeof (cover));
    snprintf(cover, sizeof (cover), "%s/.enna/covers/%s.png",
            enna_util_user_home_get(), md5);
    free(md5);

    if (!ecore_file_exists(cover))
        return NULL;

    return strdup(cover);
}

static char * cover_get_movie_keywords(const char *filename)
{
    char *it, *movie;
    char *path = strdup(filename);
    char *file = (char *) ecore_file_file_get(path);

    it = strrchr(file, '.');
    if (it) /* remove suffix? */
        *it = '\0';

    movie = strdup(file);
    free(path);
    return movie;
}

static char * cover_get_from_amazon(const char *artist, const char *album,
        const char *filename)
{
    Enna_Module *em;
    char *cover = NULL;
    char tmp[1024];

    em = enna_module_open("amazon", enna->evas);
    enna_module_enable(em);

    /* try to create cover directory storage first */
    memset(tmp, '\0', sizeof (tmp));
    snprintf(tmp, sizeof (tmp), "%s/.enna/covers", enna_util_user_home_get());
    mkdir(tmp, 0755);

    if (artist || album) /* i.e. "music" */
    {
        if (cover_class && cover_class->music_cover_get)
            cover = cover_class->music_cover_get(artist, album);
    }
    else if (filename) /* i.e. movie */
    {
        if (cover_class && cover_class->movie_cover_get)
        {
            char *movie = cover_get_movie_keywords(filename);
            cover = cover_class->movie_cover_get(movie);
            free(movie);
        }
    }

    enna_module_disable(em);
    cover_class = NULL;

    return cover;
}

static char * enna_cover_get(const char *artist, const char *album,
        const char *filename)
{
    char *cover = NULL;
    char tmp[1024];

    memset(tmp, '\0', sizeof (tmp));

    /* check for previously downloaded cover file */
    if (artist && album)
    {
        snprintf(tmp, sizeof (tmp), "%s %s", artist, album);
    }
    else if (filename)
    {
        char *movie = cover_get_movie_keywords(filename);
        snprintf(tmp, sizeof (tmp), "%s", movie);
        free(movie);
    }

    cover = cover_get_from_saved_file(tmp);
    if (cover)
        goto cover_found;

    /* check for known cover artwork filenames */
    if (filename)
    {
        cover = cover_get_from_picture_file(filename);
        if (cover)
            goto cover_found;
    }

    /* check on Amazon.com */
    cover = cover_get_from_amazon(artist, album, filename);

cover_found:
    if (cover)
        enna_log(ENNA_MSG_INFO, NULL, "Using cover from: %s", cover);

    return cover;
}

char * enna_cover_album_get(const char *artist, const char *album,
        const char *filename)
{
    if (!enna->use_covers)
        return NULL;

    return enna_cover_get(artist, album, filename);
}

char * enna_cover_video_get(const char *filename)
{
    if (!enna->use_covers)
        return NULL;

    return enna_cover_get(NULL, NULL, filename);
}
