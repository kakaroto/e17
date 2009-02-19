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
#define API_ENTRY						\
    Smart_Data *sd;						\
    sd = evas_object_smart_data_get(obj);			\
    if ((!obj) || (!sd) || (evas_object_type_get(obj)		\
	    && strcmp(evas_object_type_get(obj), SMART_NAME)))

typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
    Evas_Coord x, y, w, h;
    Evas_Object *o_smart;
    Evas_Object *o_edje;
    Evas_Object *o_carousel;
    Evas_Object *o_home_button;
    Evas_Object *o_back_button;
    Evas_Object *o_btn_box;
    Eina_List *items;
    int selected;
    unsigned char visible : 1;

};

/* local subsystem functions */
static void _home_button_clicked_cb(void *data, Evas_Object *obj, void *event_info);
static void _smart_activate_cb (void *data, Evas_Object *obj, void *event_info);
static void _smart_reconfigure(Smart_Data * sd);
static void _smart_init(void);
static void _smart_add(Evas_Object * obj);
static void _smart_del(Evas_Object * obj);
static void _smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y);
static void _smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h);
static void _smart_show(Evas_Object * obj);
static void _smart_hide(Evas_Object * obj);
static void _smart_color_set(Evas_Object * obj, int r, int g, int b, int a);
static void _smart_clip_set(Evas_Object * obj, Evas_Object * clip);
static void _smart_clip_unset(Evas_Object * obj);

/* local subsystem globals */
static Evas_Smart *_e_smart = NULL;

/* externally accessible functions */
Evas_Object *
enna_mainmenu_add(Evas * evas)
{
    _smart_init();
    return evas_object_smart_add(evas, _e_smart);
}

void enna_mainmenu_append(Evas_Object *obj, Evas_Object *icon, const char *label, Enna_Class_Activity *act)
{

    API_ENTRY return;
    enna_carousel_object_append(sd->o_carousel, icon, label);
}

void enna_mainmenu_load_from_activities(Evas_Object *obj)
{
    Eina_List *activities, *l;
    API_ENTRY return;

    activities = enna_activities_get();

    for (l = activities; l; l = l->next)
    {
        Enna_Class_Activity *act;
        Evas_Object *icon = NULL;
	const char *icon_filename = NULL;
        act = l->data;

        if (act->icon)
        {
	    icon = edje_object_add(evas_object_evas_get(sd->o_edje));
	    icon_filename = act->icon;

        }
        else if (act->icon_file)
        {
            icon = enna_image_add(evas_object_evas_get(sd->o_edje));
	    icon_filename = act->icon_file;
        }

	edje_object_file_set(icon, enna_config_theme_get(), icon_filename);

	evas_object_data_set(icon, "icon", icon_filename);
	evas_object_data_set(icon, "activity", act);
	evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
        enna_mainmenu_append(obj, icon, act->name, act);
    }

}

void enna_mainmenu_activate_nth(Evas_Object *obj, int nth)
{

    int n = 0;

    API_ENTRY return;

    n = enna_carousel_selected_get(sd->o_carousel);
    enna_carousel_activate_nth(sd->o_carousel, n);

}

int enna_mainmenu_selected_get(Evas_Object *obj)
{
    API_ENTRY return -1;

    return enna_carousel_selected_get(sd->o_carousel);

}

void enna_mainmenu_select_nth(Evas_Object *obj, int nth)
{

    API_ENTRY return;

    enna_carousel_select_nth(sd->o_carousel, nth);
}

Enna_Class_Activity *enna_mainmenu_selected_activity_get(Evas_Object *obj)
{

    Enna_Class_Activity *act = NULL;
    Evas_Object *o;
    API_ENTRY return NULL;

    o = enna_carousel_selected_object_get(sd->o_carousel);

    act = evas_object_data_get(o, "activity");

    return act;
}



void enna_mainmenu_event_feed(Evas_Object *obj, void *event_info)
{
    API_ENTRY return;

    enna_carousel_event_feed(sd->o_carousel, event_info);
}

void enna_mainmenu_show(Evas_Object *obj)
{
    Evas_Object *icon;

    API_ENTRY return;
    if (sd->visible)
        return;

    sd->visible = 1;
    edje_object_signal_emit(sd->o_edje, "mainmenu,show", "enna");

    /* Unswallow and delete previous icons */
    icon = edje_object_part_swallow_get(sd->o_edje, "titlebar.swallow.icon");
    edje_object_part_unswallow(sd->o_edje, icon);
    ENNA_OBJECT_DEL(icon);

    edje_object_part_text_set(sd->o_edje, "titlebar.text.label", "Enna");

}

void enna_mainmenu_hide(Evas_Object *obj)
{
    API_ENTRY return;
    if (!sd->visible)
        return;

    sd->visible = 0;
    edje_object_signal_emit(sd->o_edje, "mainmenu,hide", "enna");
}

unsigned char enna_mainmenu_visible(Evas_Object *obj)
{
    API_ENTRY return 0;
    return sd->visible;
}

/* local subsystem globals */

static void _home_button_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
    evas_event_feed_key_down(enna->evas, "Super_L", "Super_L", "Super_L", NULL, ecore_time_get(), data);
}

static void _back_button_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{

    evas_event_feed_key_down(enna->evas, "BackSpace", "BackSpace", "BackSpace", NULL, ecore_time_get(), data);
}

static void
_smart_activate_cb (void *data, Evas_Object *obj, void *event_info)
{
    const char *label = event_info;
    const char *icon_filename = NULL;
    Smart_Data *sd = data;
    Evas_Object *icon, *o;

    if (!data || !event_info)
        return;

    enna_content_select(label);
    enna_mainmenu_hide(sd->o_smart);

    /* Unswallow and delete previous icons */
    icon = edje_object_part_swallow_get(sd->o_edje, "titlebar.swallow.icon");
    edje_object_part_unswallow(sd->o_edje, icon);
    ENNA_OBJECT_DEL(icon);

    o = enna_carousel_selected_object_get(sd->o_carousel);
    icon_filename = evas_object_data_get(o, "icon");

    edje_object_part_text_set(sd->o_edje, "titlebar.text.label", label);
    icon = edje_object_add(evas_object_evas_get(sd->o_edje));
    edje_object_file_set(icon, enna_config_theme_get(), icon_filename);
    edje_object_part_swallow(sd->o_edje, "titlebar.swallow.icon", icon);

}

static void _smart_reconfigure(Smart_Data * sd)
{
    Evas_Coord x, y, w, h;

    x = sd->x;
    y = sd->y;
    w = sd->w;
    h = sd->h;

    evas_object_move(sd->o_edje, x, y);
    evas_object_resize(sd->o_edje, w, h);

}

static void _smart_init(void)
{
    if (_e_smart)
        return;
    static const Evas_Smart_Class sc =
	{
	    SMART_NAME,
	    EVAS_SMART_CLASS_VERSION,
	    _smart_add,
	    _smart_del,
	    _smart_move,
	    _smart_resize,
	    _smart_show,
	    _smart_hide,
	    _smart_color_set,
	    _smart_clip_set,
	    _smart_clip_unset,
	    NULL,
	    NULL
	};
    _e_smart = evas_smart_class_new(&sc);
}

static void _smart_add(Evas_Object * obj)
{
    Smart_Data *sd;
    Evas_Object *o;
    Evas *e;
    unsigned int orientation = 1;
    char *tmp = NULL;
    sd = calloc(1, sizeof(Smart_Data));
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

    o = enna_carousel_add(e);
//    enna_carousel_orientation_set(o, orientation);
    sd->o_carousel = o;
    evas_object_smart_callback_add(sd->o_carousel, "activate", _smart_activate_cb, sd);

    edje_object_part_swallow(sd->o_edje, "enna.swallow.box", sd->o_carousel);

    sd->o_btn_box = enna_box_add(e);
    enna_box_homogenous_set(sd->o_btn_box, 0);
    enna_box_orientation_set(sd->o_btn_box, 1);
    evas_object_size_hint_align_set(sd->o_btn_box, 0, 0.5);
    evas_object_size_hint_weight_set(sd->o_btn_box, 1.0, 1.0);
    edje_object_part_swallow(sd->o_edje, "titlebar.swallow.button", sd->o_btn_box);

    o = enna_button_add(e);
    enna_button_icon_set(o, "icon/home_mini");
    evas_object_smart_callback_add(o, "clicked", _home_button_clicked_cb, sd);
    evas_object_size_hint_align_set(o, 0.5, 0.5);
    evas_object_size_hint_min_set(o, 64, 64);
    enna_box_pack_end(sd->o_btn_box, o);
    evas_object_show(o);
    sd->o_home_button = o;

    o = enna_button_add(e);
    enna_button_icon_set(o, "icon/arrow_left");
    evas_object_size_hint_align_set(o, 0.5, 0.5);
    evas_object_size_hint_min_set(o, 64, 64);
    enna_box_pack_end(sd->o_btn_box, o);
    evas_object_show(o);
    evas_object_smart_callback_add(o, "clicked", _back_button_clicked_cb, sd);
    sd->o_back_button = o;

    sd->o_smart = obj;
    evas_object_smart_member_add(sd->o_edje, obj);
    evas_object_smart_data_set(obj, sd);
}

static void _smart_del(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;

    eina_list_free(sd->items);
    evas_object_del(sd->o_edje);
    evas_object_del(sd->o_carousel);
    free(sd);
}

static void _smart_move(Evas_Object * obj, Evas_Coord x, Evas_Coord y)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    if ((sd->x == x) && (sd->y == y))
        return;
    sd->x = x;
    sd->y = y;
    _smart_reconfigure(sd);
}

static void _smart_resize(Evas_Object * obj, Evas_Coord w, Evas_Coord h)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    if ((sd->w == w) && (sd->h == h))
        return;
    sd->w = w;
    sd->h = h;
    _smart_reconfigure(sd);
}

static void _smart_show(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;

    evas_object_show(sd->o_edje);
}

static void _smart_hide(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_hide(sd->o_edje);
}

static void _smart_color_set(Evas_Object * obj, int r, int g, int b, int a)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_color_set(sd->o_edje, r, g, b, a);
}

static void _smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_set(sd->o_edje, clip);
}

static void _smart_clip_unset(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_unset(sd->o_edje);
}

