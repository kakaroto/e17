/*
 * BROWSER.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * BROWSER.c is free software copyrighted by Nicolas Aguirre.
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
 * BROWSER.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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

/*
 * Smart Callback event
 *
 * "root" this event is sent when root is browse
 * "selected" this event is sent when a file or a directory is selected
 * "browse_down" this event is sent when browse down is detected
 *
 */

#include "enna.h"


#define SMART_NAME "Enna_Browser"

typedef struct _Smart_Data Smart_Data;

struct _Smart_Data
{
    Evas_Coord x, y, w, h;
    Evas_Object *obj;
    Evas_Object *o_edje;
    Evas_Object *o_list;
    Eina_List *files;
    Enna_Class_Vfs *vfs;
    Enna_Vfs_File *file;
    Evas *evas;
    char *prev;
    unsigned char accept_ev : 1;
    unsigned char show_file : 1;
};



/* local subsystem functions */
static void _list_transition_right_end_cb(void *data, Evas_Object *o, const char *sig, const char *src);
static void _list_transition_left_end_cb(void *data, Evas_Object *o, const char *sig, const char *src);

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
static Evas_Smart *_smart = NULL;

/* externally accessible functions */
Evas_Object *
enna_browser_add(Evas * evas)
{
    _smart_init();
    return evas_object_smart_add(evas, _smart);
}

void enna_browser_show_file_set(Evas_Object *obj, unsigned char show)
{

    API_ENTRY return;

    sd->show_file = show;

}

/* local subsystem globals */
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
    if (_smart)
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
            NULL };
    _smart = evas_smart_class_new(&sc);
}

static void _smart_add(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = calloc(1, sizeof(Smart_Data));
    if (!sd)
        return;

    sd->evas = evas_object_evas_get(obj);
    sd->o_edje = edje_object_add(sd->evas);
    edje_object_file_set(sd->o_edje, enna_config_theme_get(), "enna/browser");

    sd->o_list = enna_list_add(sd->evas);

    edje_object_part_swallow(sd->o_edje, "enna.swallow.content", sd->o_list);
    edje_object_signal_emit(sd->o_list, "list,right,now", "enna");

    sd->x = 0;
    sd->y = 0;
    sd->w = 0;
    sd->h = 0;
    sd->accept_ev = 0;
    sd->show_file = 1;
    evas_object_smart_member_add(sd->o_edje, obj);
    sd->obj = obj;
    evas_object_smart_data_set(obj, sd);
}

static void _smart_del(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    edje_object_signal_callback_del(sd->o_edje, "list,transition,end", "edje", _list_transition_right_end_cb);
    edje_object_signal_callback_del(sd->o_edje, "list,transition,end", "edje", _list_transition_left_end_cb);
    ENNA_OBJECT_DEL(sd->o_list);
    evas_object_del(sd->o_edje);
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

static void _browse(void *data, void *data2)
{
    Smart_Data *sd = data;

    if (!sd || !sd->vfs)
        return;

    sd->file = data2;
    sd->accept_ev = 0;
    if (sd->vfs->func.class_browse_up)
    {
	Browser_Selected_File_Data *ev = calloc(1, sizeof(Browser_Selected_File_Data));
	ev->vfs = sd->vfs;
	ev->file = sd->file;

        if (sd->file && sd->file->is_directory)
        {
            /* File selected is a directory */
            sd->files = sd->vfs->func.class_browse_up(sd->file->uri, sd->vfs->cookie);
	    ev->files = sd->files;

	    evas_object_smart_callback_call (sd->obj, "selected", ev);
        }
        else if (sd->show_file)
        {
            /* File selected is a regular file */
	    Enna_Vfs_File *prev_vfs;
            char *prev_uri;
            prev_vfs = sd->vfs->func.class_vfs_get(sd->vfs->cookie);
            prev_uri = prev_vfs->uri ? strdup(prev_vfs->uri) : NULL;
            sd->files = sd->vfs->func.class_browse_up(prev_uri, sd->vfs->cookie);
	    ENNA_FREE(prev_uri);
	    ev->files = sd->files;
	    sd->accept_ev = 1;
	    evas_object_smart_callback_call (sd->obj, "selected", ev);
            return;
        }

        /* Clear list and add new items */
        edje_object_signal_callback_add(sd->o_edje, "list,transition,end", "edje",
                _list_transition_left_end_cb, sd);
        edje_object_signal_emit(sd->o_edje, "list,left", "enna");
    }
}

static void _browse_down(Smart_Data *sd)
{
    if (!sd) return;

    sd->accept_ev = 0;

    if (sd->vfs && sd->vfs->func.class_browse_down)
    {
	sd->files = sd->vfs->func.class_browse_down(sd->vfs->cookie);
	if (!sd->files)
	{
	    evas_object_smart_callback_call (sd->obj, "root", NULL);
	    return;
	}

        /* Clear list and add new items */
        edje_object_signal_callback_add(sd->o_edje, "list,transition,end", "edje",
	    _list_transition_right_end_cb, sd);
        edje_object_signal_emit(sd->o_edje, "list,right", "enna");
    }
}

static void
_list_transition_core(Smart_Data *sd, unsigned char direction)
{

    Eina_List *l;
    Eina_List *files = sd->files;

    if (!direction)
        edje_object_signal_callback_del(sd->o_edje, "list,transition,end", "edje",
	    _list_transition_left_end_cb);
    else
        edje_object_signal_callback_del(sd->o_edje, "list,transition,end", "edje",
	    _list_transition_right_end_cb);

    ENNA_OBJECT_DEL(sd->o_list);

    sd->o_list = enna_list_add(sd->evas);
    edje_object_part_swallow(sd->o_edje, "enna.swallow.content", sd->o_list);
    edje_object_calc_force(sd->o_edje);

    if (direction == 0)
        edje_object_signal_emit(sd->o_edje, "list,right,now", "enna");
    else
        edje_object_signal_emit(sd->o_edje, "list,left,now", "enna");

    if (eina_list_count(files))
    {
        int i = 0;

        /* Create list of files */
        for (l = files, i = 0; l; l = l->next, i++)
        {
            Enna_Vfs_File *f;
            Evas_Object *icon = NULL;
            Evas_Object *item = NULL;

            f = l->data;

	    if (!f->is_directory && !sd->show_file)
		continue;

            if (f->icon_file && f->icon_file[0] == '/')
            {
                icon = enna_image_add(sd->evas);
                enna_image_file_set(icon, f->icon_file);
            }
            else
            {
                icon = edje_object_add(sd->evas);
                edje_object_file_set(icon, enna_config_theme_get(), f->icon);
            }

            item = enna_listitem_add(sd->evas);
            enna_listitem_create_simple(item, icon, f->label);
	    //sd->file = f;
            enna_list_append(sd->o_list, item, _browse, NULL, sd, f);
        }
	if (direction)
	    evas_object_smart_callback_call (sd->obj, "browse_down", NULL);
	else
	    enna_list_selected_set(sd->o_list, 0);
    }
    else if (!direction)
    {
        /* No files returned : create no media item */
        Evas_Object *icon;
        Evas_Object *item;
        icon = edje_object_add(sd->evas);
        edje_object_file_set(icon, enna_config_theme_get(), "icon_nofile");
        item = enna_listitem_add(sd->evas);
        enna_listitem_create_simple(item, icon, "No Media found!");
        enna_list_append(sd->o_list, item, NULL, NULL, NULL, NULL);
	enna_list_selected_set(sd->o_list, 0);
    }
    else
    {
        /* Browse down and no file detected : Root */
        sd->vfs = NULL;

    }
    edje_object_signal_emit(sd->o_edje, "list,default", "enna");
    sd->accept_ev = 1;
}

static void
_list_transition_left_end_cb(void *data, Evas_Object *o, const char *sig, const char *src)
{
    _list_transition_core(data, 0);
}

static void
_list_transition_right_end_cb(void *data, Evas_Object *o, const char *sig, const char *src)
{
    _list_transition_core(data, 1);
}


void enna_browser_root_set(Evas_Object *obj, Enna_Class_Vfs *vfs)
{
    API_ENTRY return;

    if (!vfs) return;

    if (vfs->func.class_browse_up)
    {
	/* create Root menu */
	sd->files = vfs->func.class_browse_up(NULL, vfs->cookie);
        sd->vfs = vfs;
        edje_object_signal_callback_add(sd->o_edje, "list,transition,end", "edje", _list_transition_left_end_cb, sd);
        edje_object_signal_emit(sd->o_edje, "list,left", "enna");
    }
}

void enna_browser_event_feed(Evas_Object *obj, void *event_info)
{
    Evas_Event_Key_Down *ev = event_info;
    enna_key_t key = enna_get_key(ev);

    API_ENTRY return;

    edje_object_signal_callback_del(sd->o_edje, "list,transition,end", "edje",
	_list_transition_left_end_cb);
    edje_object_signal_callback_del(sd->o_edje, "list,transition,end", "edje",
	_list_transition_right_end_cb);

    enna_log(ENNA_MSG_EVENT, SMART_NAME, "Key pressed : %s\n", ev->key);
    switch (key)
    {
    case ENNA_KEY_LEFT:
    case ENNA_KEY_CANCEL:
	_browse_down(sd);
	break;
    case ENNA_KEY_RIGHT:
    case ENNA_KEY_OK:
    case ENNA_KEY_SPACE:
	_browse(sd, enna_list_selected_data2_get(sd->o_list));
	break;
    default:
	enna_list_event_key_down(sd->o_list, event_info);
    }


}

int
enna_browser_select_label(Evas_Object *obj, const char *label)
{

    API_ENTRY return -1;

    if (!sd || !sd->o_list) return -1;

    return enna_list_jump_label(sd->o_list, label);

}
