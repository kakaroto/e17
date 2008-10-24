/*
 * enna_mainmenu.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * enna_mainmenu.c is free software copyrighted by Nicolas Aguirre.
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
 * enna_mainmenu.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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

#include "mainmenu.h"
#include "enna.h"

#define SMART_NAME "enna_mainmenu"
#define API_ENTRY \
  E_Smart_Data *sd; \
  sd = evas_object_smart_data_get(obj); \
  if ((!obj) || (!sd) || (evas_object_type_get(obj) \
    && strcmp(evas_object_type_get(obj), SMART_NAME)))

typedef struct _E_Smart_Data E_Smart_Data;
typedef struct _E_Smart_Item E_Smart_Item;

struct _E_Smart_Data
{
    Evas_Coord x, y, w, h;
    Evas_Object *o_smart;
    Evas_Object *o_edje;
    Evas_Object *o_box;
    Eina_List *items;
    int selected;
    unsigned char visible : 1;

};

struct _E_Smart_Item
{
    E_Smart_Data *sd;
    Evas_Object *o_base;
    Evas_Object *o_icon;
    void (*func)(void *data);
    void *data;
    unsigned char selected : 1;
    Enna_Class_Activity *act;
};

/* local subsystem functions */
static void _enna_mainmenu_activate_cb(void *data);
static void _enna_mainmenu_smart_reconfigure(E_Smart_Data * sd);
static void _enna_mainmenu_smart_init(void);
static void _e_smart_add(Evas_Object * obj);
static void _e_smart_del(Evas_Object * obj);
static void _e_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y);
static void _e_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h);
static void _e_smart_show(Evas_Object * obj);
static void _e_smart_hide(Evas_Object * obj);
static void _e_smart_color_set(Evas_Object * obj, int r, int g, int b, int a);
static void _e_smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void _e_smart_clip_unset(Evas_Object * obj);
static void _e_smart_event_mouse_up(void *data, Evas *evas, Evas_Object *obj,
        void *event_info);
static void _e_smart_event_mouse_down(void *data, Evas *evas, Evas_Object *obj,
        void *event_info);

/* local subsystem globals */
static Evas_Smart *_e_smart = NULL;

/* externally accessible functions */
EAPI Evas_Object *
enna_mainmenu_add(Evas * evas)
{
    _enna_mainmenu_smart_init();
    return evas_object_smart_add(evas, _e_smart);
}

EAPI void enna_mainmenu_append(Evas_Object *obj, Evas_Object *icon,
        const char *label, Enna_Class_Activity *act, void (*func) (void *data), void *data)
{
    E_Smart_Item *si;
    Evas_Coord mw = 0, mh = 0;
    const char *tmp;

    API_ENTRY
    return;

    if (!act)
        return;

    si = malloc(sizeof(E_Smart_Item));
    si->sd = sd;
    si->act = act;
    si->o_base = edje_object_add(evas_object_evas_get(sd->o_edje));
    edje_object_file_set(si->o_base, enna_config_theme_get(),
            "enna/mainmenu/item");
    if (label)
    {
        edje_object_part_text_set(si->o_base, "enna.text.label", label);
    }

    si->o_icon = icon;
    if (icon)
    {
        edje_object_part_swallow(si->o_base, "enna.swallow.icon", si->o_icon);
        evas_object_show(icon);
    }
    si->func = func;
    si->data = data;
    si->selected = 0;
    sd->items = eina_list_append(sd->items, si);
    edje_object_size_min_calc(si->o_base, &mw, &mh);
    edje_object_size_min_get(si->o_base, &mw, &mh);
    enna_box_pack_end(sd->o_box, si->o_base);

    tmp = edje_object_data_get(si->o_base, "height");
    if (tmp)
        mh = atoi(tmp);
    tmp = edje_object_data_get(si->o_base, "width");
    if (tmp)
        mw = atoi(tmp);

    enna_box_pack_options_set(si->o_base, 1, 1, /* fill */
    1, 1, /* expand */
    0.5, 0.5, /* align */
    mw, mh, /* min */
    99999, 9999 /* max */
    );

    evas_object_event_callback_add(si->o_base, EVAS_CALLBACK_MOUSE_UP,
            _e_smart_event_mouse_up, si);
    evas_object_event_callback_add(si->o_base, EVAS_CALLBACK_MOUSE_DOWN,
            _e_smart_event_mouse_down, si);
    evas_object_show(si->o_base);
}

EAPI void enna_mainmenu_load_from_activities(Evas_Object *obj)
{
    Eina_List *activities, *l;
    API_ENTRY
    return;

    activities = enna_activities_get();

    for (l = activities; l; l = l->next)
    {
        Enna_Class_Activity *act;
        Evas_Object *icon = NULL;
        act = l->data;

        if (act->icon)
        {
            icon = edje_object_add(evas_object_evas_get(sd->o_edje));
            edje_object_file_set(icon, enna_config_theme_get(), act->icon);
        }
        else if (act->icon_file)
        {
            icon = enna_image_add(evas_object_evas_get(sd->o_edje));
            enna_image_file_set(icon, act->icon_file);
        }
        enna_mainmenu_append(obj, icon, act->name, act,
                _enna_mainmenu_activate_cb, act);
    }

}

EAPI void enna_mainmenu_activate_nth(Evas_Object *obj, int nth)
{
    E_Smart_Item *si;
    API_ENTRY
    return;

    si = eina_list_nth(sd->items, nth);
    if (!si)
        return;
    if (si->func)
    {

        Evas_Object *icon;
        si->func(si->data);
        /* Unswallow and delete previous icons */
        icon = edje_object_part_swallow_get(sd->o_edje, "titlebar.swallow.icon");
        edje_object_part_unswallow(sd->o_edje, icon);
        ENNA_OBJECT_DEL(icon);

        edje_object_part_text_set(sd->o_edje, "titlebar.text.label", si->act->name);
        icon = edje_object_add(evas_object_evas_get(sd->o_edje));
        edje_object_file_set(icon, enna_config_theme_get(),si->act->icon);
        edje_object_part_swallow(sd->o_edje, "titlebar.swallow.icon", icon);
        enna_mainmenu_hide(obj);
    }
}

EAPI int enna_mainmenu_selected_get(Evas_Object *obj)
{
    API_ENTRY
    return -1;
    return sd->selected;
}

EAPI void enna_mainmenu_select_nth(Evas_Object *obj, int nth)
{
    E_Smart_Item *new, *prev;

    API_ENTRY
    return;

    prev = eina_list_nth(sd->items, sd->selected);

    if (!prev)
        return;

    new = eina_list_nth(sd->items, nth);
    if (!new)
        return;

    sd->selected = nth;
    edje_object_signal_emit(new->o_base, "enna,state,selected", "enna");
    if (new != prev)
        edje_object_signal_emit(prev->o_base, "enna,state,unselected", "enna");

}

EAPI Enna_Class_Activity *
enna_mainmenu_selected_activity_get(Evas_Object *obj)
{
    E_Smart_Item *si;
    API_ENTRY return NULL;

    si = eina_list_nth(sd->items, sd->selected);

    if (!si)
    return NULL;

    return si->act;

}

EAPI void enna_mainmenu_select_next(Evas_Object *obj)
{
    E_Smart_Item *new, *prev;
    int ns = 0;

    API_ENTRY
    return;

    ns = sd->selected;
    prev = eina_list_nth(sd->items, ns);
    if (!prev)
        return;
    ns++;
    new = eina_list_nth(sd->items, ns);
    if (!new)
    {
        ns = 0;
        new = eina_list_nth(sd->items, ns);
        if (!new)
            return;
    }
    sd->selected = ns;
    edje_object_signal_emit(new->o_base, "enna,state,selected", "enna");
    edje_object_signal_emit(prev->o_base, "enna,state,unselected", "enna");

}

EAPI void enna_mainmenu_select_prev(Evas_Object *obj)
{
    E_Smart_Item *new, *prev;
    int ns = 0;

    API_ENTRY
    return;

    ns = sd->selected;
    prev = eina_list_nth(sd->items, ns);
    if (!prev)
        return;
    ns--;
    new = eina_list_nth(sd->items, ns);
    if (!new)
    {
        ns = eina_list_count(sd->items)-1;
        new = eina_list_nth(sd->items, ns);
        if (!new)
            return;
    }
    sd->selected = ns;
    edje_object_signal_emit(new->o_base, "enna,state,selected", "enna");
    edje_object_signal_emit(prev->o_base, "enna,state,unselected", "enna");

}

EAPI void enna_mainmenu_show(Evas_Object *obj)
{
    Evas_Object *icon;
    API_ENTRY return;
    if (sd->visible)
        return;

    sd->visible = 1;

    /* Unswallow and delete previous icons */
    icon = edje_object_part_swallow_get(sd->o_edje, "titlebar.swallow.icon");
    edje_object_part_unswallow(sd->o_edje, icon);
    ENNA_OBJECT_DEL(icon);

    edje_object_part_text_set(sd->o_edje, "titlebar.text.label", "Enna");
    /* FIXME: add enna icon */
    /*icon = edje_object_add(evas_object_evas_get(sd->o_edje));
    edje_object_file_set(icon, enna_config_theme_get(), si->act->icon);
    edje_object_part_swallow(sd->o_edje, "titlebar.swallow.icon", icon);
    */
    edje_object_signal_emit(sd->o_edje, "mainmenu,show", "enna");

}

EAPI void enna_mainmenu_hide(Evas_Object *obj)
{
    E_Smart_Item *si;
    API_ENTRY
    return;
    if (!sd->visible)
        return;

    sd->visible = 0;

    si = eina_list_nth(sd->items, sd->selected);

    if (si && si->act)
    {
       Evas_Object *icon;
       si->func(si->data);
       /* Unswallow and delete previous icons */
       icon = edje_object_part_swallow_get(sd->o_edje, "titlebar.swallow.icon");
       edje_object_part_unswallow(sd->o_edje, icon);
       ENNA_OBJECT_DEL(icon);

       edje_object_part_text_set(sd->o_edje, "titlebar.text.label", si->act->name);
       icon = edje_object_add(evas_object_evas_get(sd->o_edje));
       edje_object_file_set(icon, enna_config_theme_get(),si->act->icon);
       edje_object_part_swallow(sd->o_edje, "titlebar.swallow.icon", icon);
       enna_mainmenu_hide(obj);
    }

    edje_object_signal_emit(sd->o_edje, "mainmenu,hide", "enna");
}

EAPI unsigned char enna_mainmenu_visible(Evas_Object *obj)
{
    API_ENTRY
    return 0;
    return sd->visible;
}

/* local subsystem globals */

static void _enna_mainmenu_activate_cb(void *data)
{
    Enna_Class_Activity *act;

    if (!data)
        return;
    act = data;
    enna_content_select(act->name);

}

static void _enna_mainmenu_smart_reconfigure(E_Smart_Data * sd)
{
    Evas_Coord x, y, w, h;

    x = sd->x;
    y = sd->y;
    w = sd->w;
    h = sd->h;

    evas_object_move(sd->o_edje, x, y);
    evas_object_resize(sd->o_edje, w, h);

}

static void _enna_mainmenu_smart_init(void)
{
    if (_e_smart)
        return;
    static const Evas_Smart_Class sc =
    { SMART_NAME, EVAS_SMART_CLASS_VERSION, _e_smart_add, _e_smart_del,
            _e_smart_move, _e_smart_resize, _e_smart_show, _e_smart_hide,
            _e_smart_color_set, _e_smart_clip_set, _e_smart_clip_unset, NULL,
            NULL };
    _e_smart = evas_smart_class_new(&sc);
}

static void _e_smart_add(Evas_Object * obj)
{
    E_Smart_Data *sd;
    Evas_Object *o;
    Evas *e;
    unsigned int orientation = 1;
    char *tmp = NULL;
    sd = calloc(1, sizeof(E_Smart_Data));
    if (!sd)
        return;

    e = evas_object_evas_get(obj);

    sd->o_edje = evas_object_image_add(e);
    sd->x = 0;
    sd->y = 0;
    sd->w = 0;
    sd->h = 0;

    sd->items = NULL;
    sd->visible = 0;
    sd->selected = 0;
    o = edje_object_add(e);
    edje_object_file_set(o, enna_config_theme_get(), "enna/mainmenu");
    sd->o_edje = o;
    evas_object_show(o);

    /* Get Orientation from edje/
     * 1 => Horizontal
     * 0 => Vertical
     */
    tmp = (char *) edje_object_data_get(sd->o_edje, "orientation");
    if (tmp)
        orientation = atoi(tmp);

    o = enna_box_add(e);
    enna_box_orientation_set(o, orientation);
    enna_box_homogenous_set(o, 1);
    //    enna_box_align_set(o, 0.5, 0.5);
    sd->o_box = o;

    edje_object_part_swallow(sd->o_edje, "enna.swallow.box", sd->o_box);
    
    o = enna_button_add(e);
    enna_button_icon_set(o, "icon/home_mini");
    edje_object_part_swallow(sd->o_edje, "titlebar.swallow.button", o);
    evas_object_show(o);
    
    sd->o_smart = obj;
    evas_object_smart_member_add(sd->o_edje, obj);
    evas_object_smart_data_set(obj, sd);
}

static void _e_smart_del(Evas_Object * obj)
{
    E_Smart_Data *sd;
    Eina_List *l;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;

    for (l = sd->items; l; l = l->next)
    {
        E_Smart_Item *si;
        si = l->data;
        evas_object_del(si->o_base);
        evas_object_del(si->o_icon);
    }
    eina_list_free(sd->items);
    evas_object_del(sd->o_edje);
    evas_object_del(sd->o_box);
    free(sd);
}

static void _e_smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    if ((sd->x == x) && (sd->y == y))
        return;
    sd->x = x;
    sd->y = y;
    _enna_mainmenu_smart_reconfigure(sd);
}

static void _e_smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    if ((sd->w == w) && (sd->h == h))
        return;
    sd->w = w;
    sd->h = h;
    _enna_mainmenu_smart_reconfigure(sd);
}

static void _e_smart_show(Evas_Object * obj)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;

    evas_object_show(sd->o_edje);
}

static void _e_smart_hide(Evas_Object * obj)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_hide(sd->o_edje);
}

static void _e_smart_color_set(Evas_Object * obj, int r, int g, int b, int a)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_color_set(sd->o_edje, r, g, b, a);
}

static void _e_smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_set(sd->o_edje, clip);
}

static void _e_smart_clip_unset(Evas_Object * obj)
{
    E_Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_unset(sd->o_edje);
}

static void _e_smart_event_mouse_up(void *data, Evas *evas, Evas_Object *obj,
        void *event_info)
{
    E_Smart_Data *sd;
    Evas_Event_Mouse_Up *ev;
    Enna_List_Item *si;
    Eina_List *l;
    int i;

    ev = event_info;
    si = data;
    sd = si->sd;

    if (!sd->items)
        return;

    for (l = sd->items, i = 0; l; l = l->next, i++)
    {
        if (l->data == si)
        {
            enna_mainmenu_activate_nth(sd->o_smart, i);
            break;
        }
    }
}

static void _e_smart_event_mouse_down(void *data, Evas *evas, Evas_Object *obj,
        void *event_info)
{
    E_Smart_Data *sd;
    Evas_Event_Mouse_Down *ev;
    Enna_List_Item *si;
    Enna_List_Item *prev;
    Eina_List *l = NULL;
    int i;

    ev = event_info;
    si = data;
    sd = si->sd;

    if (!sd->items)
        return;
    prev = eina_list_nth(sd->items, sd->selected);

    for (i = 0, l = sd->items; l; l = l->next, i++)
    {
        if (l->data == si)
        {
            edje_object_signal_emit(si->o_base, "enna,state,selected", "enna");
            edje_object_signal_emit(prev->o_base, "enna,state,unselected",
                    "enna");
            sd->selected = i;
            break;
        }
    }

}
