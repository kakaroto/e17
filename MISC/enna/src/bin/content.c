/*
 * enna_content.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_content.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_content.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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

/* derived from e_icon */

#include "content.h"

Evas_Object *_content = NULL;

typedef struct _Enna_Content_Element Enna_Content_Element;

struct _Enna_Content_Element
{
    const char *name;
    Evas_Object *content;
    unsigned char selected : 1;
};

static Evas_List *_enna_contents = NULL;

/* local subsystem functions */

/* externally accessible functions */
EAPI Evas_Object *
enna_content_add(Evas *evas)
{
    Evas_Object *o;

    o = edje_object_add(evas);
    edje_object_file_set(o, enna_config_theme_get(), "content");
    _content = o;
    return o;
}

EAPI int enna_content_append(const char *name, Evas_Object *content)
{
    Evas_List *l;
    Enna_Content_Element *elem;

    if (!name || !content)
        return -1;
    for (l = _enna_contents; l; l = l ->next)
    {
        Enna_Content_Element *e;
        e = l->data;
        if (!e)
            continue;
        if (!strcmp(e->name, name))
            return -1;
    }
    elem = calloc(1, sizeof(Enna_Content_Element));
    elem->name = evas_stringshare_add(name);
    elem->content = content;
    elem->selected = 0;
    _enna_contents = evas_list_append(_enna_contents, elem);
    return 0;
}

EAPI int enna_content_select(const char *name)
{

    Evas_List *l;
    Enna_Content_Element *new = NULL;
    Enna_Content_Element *prev = NULL;

    if (!name)
        return -1;
    for (l = _enna_contents; l; l = l->next)
    {
        Enna_Content_Element *e;
        e = l->data;

        if (!e)
            continue;

        if (!strcmp(name, e->name))
        {
            if (!e->selected)
            {
                new = e;
                e->selected = 1;
            }
        }
        else if (e->selected)
        {
            prev = e;
            e->selected = 0;
        }
    }

    if (prev)
    {
        edje_object_part_unswallow(_content, prev->content);
        enna_activity_hide(prev->name);
    }
    if (new)
    {
        edje_object_part_swallow(_content, "enna.swallow.content", new->content);
        enna_activity_show(new->name);
    }

    return 0;
}

