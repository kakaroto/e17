/*
 * enna_util.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_util.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_util.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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
#include "utils.h"

char * enna_util_user_home_get()
{
    static char *home = NULL;

    if (home)
        return home;

    home = strdup(getenv("HOME"));
    if (!home)
        return strdup(getenv("CWD"));
    return home;
}

int enna_util_has_suffix(char *str, Eina_List * patterns)
{
    Eina_List *l;
    int result = 0;

    int i;
    char *tmp;

    if (!patterns || !str || !str[0])
        return 0;

    for (l = patterns; l; l = eina_list_next(l))
    {
        tmp = calloc(1, strlen(str) + 1);
        for (i = 0; i < strlen(str); i++)
            tmp[i] = tolower(str[i]);
        result |= ecore_str_has_suffix(tmp, (char *)l->data);
        ENNA_FREE(tmp);
    }
    return result;
}

unsigned char enna_util_uri_has_extension(const char *uri, int type)
{

    Eina_List *l;
    Eina_List *filters = NULL;

    if (type == ENNA_CAPS_MUSIC)
        filters = enna_config->music_filters;
    else if (type == ENNA_CAPS_VIDEO)
        filters = enna_config->video_filters;
    else if (type == ENNA_CAPS_PHOTO)
        filters = enna_config->photo_filters;

    if (!filters)
        return 0;

    for (l = filters; l; l = l->next)
    {
        const char *ext = l->data;
        if (ecore_str_has_extension(uri, ext))
            return 1;
    }

    return 0;

}

unsigned int enna_util_calculate_font_size(Evas_Coord w, Evas_Coord h)
{
    float size = 12;

    size = sqrt(w * w + h * h) / (float)50.0;
    size = MMIN(size, 8);
    size = MMAX(size, 30);

    return (unsigned int)size;

}

void enna_util_switch_objects(Evas_Object * container, Evas_Object * obj1, Evas_Object * obj2)
{
    Evas_Object *s;

    if (!obj1 && !obj2)
        return;

    if ((obj1 && obj2))
    {
        s = edje_object_part_swallow_get(container, "enna.switcher.swallow2");
        edje_object_part_swallow(container, "enna.switcher.swallow1", obj2);
        edje_object_part_swallow(container, "enna.switcher.swallow2", obj1);
        if (s)
            evas_object_hide(s);

        edje_object_signal_emit(container, "enna,swallow2,default,now", "enna");
        edje_object_signal_emit(container, "enna,swallow1,state1,now", "enna");
        edje_object_signal_emit(container, "enna,swallow1,default", "enna");
        edje_object_signal_emit(container, "enna,swallow2,state2", "enna");
    }
    else if (!obj2)
    {
        edje_object_part_swallow(container, "enna.switcher.swallow2", obj1);
        edje_object_signal_emit(container, "enna,swallow2,default,now", "enna");
        edje_object_signal_emit(container, "enna,swallow2,state2", "enna");
    }
    else if (!obj1)
    {
        edje_object_part_swallow(container, "enna.switcher.swallow1", obj2);
        edje_object_signal_emit(container, "enna,swallow1,state1,now", "enna");
        edje_object_signal_emit(container, "enna,swallow1,default", "enna");
    }
}
