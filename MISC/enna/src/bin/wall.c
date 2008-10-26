/*
 * wall.c
 * Copyright (C) Nicolas Aguirre 2006,2007,2008 <aguirre.nicolas@gmail.com>
 *
 * wall.c is free software copyrighted by Nicolas Aguirre.
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
 * wall.c IS PROVIDED BY Nicolas Aguirre ``AS IS'' AND ANY EXPRESS
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

#include "wall.h"

#define SMART_NAME "enna_wall"
#define API_ENTRY Smart_Data *sd; sd = evas_object_smart_data_get(obj); if ((!obj) || (!sd) || (evas_object_type_get(obj) && strcmp(evas_object_type_get(obj), SMART_NAME)))
#define INTERNAL_ENTRY Smart_Data *sd; sd = evas_object_smart_data_get(obj); if (!sd) return;

#define KEY_NONE 0
#define KEY_LEFT 1
#define KEY_RIGHT 2
#define KEY_UP 3
#define KEY_DOWN 4

typedef struct _Smart_Data Smart_Data;
typedef struct _Picture_Item Picture_Item;

struct _Picture_Item
{
    Evas_Object *o_edje;
    int row;
    Evas_Object *o_pict; // Enna image object
    Smart_Data *sd;
    unsigned char selected : 1;
};

struct _Smart_Data
{
    Evas_Coord x, y, w, h;
    Evas_Object *obj;
    Evas_Object *o_scroll;
    Evas_Object *o_table;
    Evas_List *pictures;
    Evas_Object *o_cont;
    Evas_Object *o_box[3];
    Evas_List *items[3];
    int nb;
    int row_sel;
    int col_sel;
    struct
    {
        Evas_Coord x, y;
        Evas_Coord sx, sy;
        Evas_Coord w;
        double anim_start;
        double dt;
        Ecore_Animator *momentum_animator;
        unsigned char now : 1;
        Picture_Item *o_selected;
    } down;

};

struct _Preload_Data
{
    Smart_Data *sd;
    Evas_Object *item;
};

/* local subsystem functions */
static Picture_Item *_smart_selected_item_get(Smart_Data *sd, int *row,
        int *col);
static void _smart_item_unselect(Smart_Data *sd, Picture_Item *pi);
static void _smart_item_select(Smart_Data *sd, Picture_Item *pi);
static void _smart_event_mouse_down(void *data, Evas *evas, Evas_Object *obj,
        void *event_info);
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
Evas_Object * enna_wall_add(Evas * evas)
{
    _smart_init();
    return evas_object_smart_add(evas, _smart);
}

void enna_wall_picture_append(Evas_Object *obj, const char *filename)
{

    Evas_Coord w, h, ow, oh;
    Evas_Object *o, *o_pict;
    int row, w0, w1, w2;
    Picture_Item *pi;

    API_ENTRY
    return;

    pi = calloc(1, sizeof(Picture_Item));

    sd->nb++;

    o = edje_object_add(evas_object_evas_get(sd->o_scroll));
    edje_object_file_set(o, enna_config_theme_get(), "enna/picture/item");
    edje_object_part_text_set(o, "enna.text.label",
	ecore_file_file_get(filename+7));
    evas_object_show(o);

    o_pict = enna_image_add(evas_object_evas_get(sd->o_scroll));
    enna_image_file_set(o_pict, filename+7);
    enna_image_size_get(o_pict, &w, &h);

    if (w > h)
    {
        oh = 200;
        ow = oh * (float)w/(float)h;
    }
    else
    {
        oh = 200;
        ow = oh * (float)w/(float)h;
    }
    enna_image_fill_inside_set(o_pict, 0);
    enna_image_load_size_set(o_pict, ow, oh);
    evas_object_resize(o_pict, ow, oh);

    enna_image_preload(o_pict, 0);

    pi->o_pict = o_pict;
    pi->o_edje = o;
    pi->sd = sd;

    enna_box_min_size_get(sd->o_box[0], &w0, NULL);
    enna_box_min_size_get(sd->o_box[1], &w1, NULL);
    enna_box_min_size_get(sd->o_box[2], &w2, NULL);

    if (w0 <= w1 && w0 <= w2)
        row = 0;
    else if (w1 <= w2)
        row = 1;
    else
        row = 2;

    pi->row = row;

    sd->items[row] = evas_list_append(sd->items[row], pi);

    evas_object_geometry_get(o_pict, NULL, NULL, &ow, &oh);

    evas_object_repeat_events_set(o_pict, 1);
    edje_extern_object_min_size_set(o, ow, oh);
    enna_image_size_get(o_pict, &w, &h);
    edje_extern_object_aspect_set(o, EDJE_ASPECT_CONTROL_BOTH, (float)w
            /(float)h, (float)w/(float)h);
    edje_object_part_swallow(o, "enna.swallow.icon", o_pict);
    evas_object_show(o_pict);

    evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
            _smart_event_mouse_down, pi);

    enna_box_freeze(sd->o_box[pi->row]);
    enna_box_pack_end(sd->o_box[pi->row], o);
    enna_box_pack_options_set(o, 0, 0, /* fill */
    0, 0, /* expand */
    0, 0, /* align */
    ow, oh, /* min */
    99999, 9999 /* max */
    );
    enna_box_thaw(sd->o_box[pi->row]);

}

///////////////////// LEFT /////////////////////
void enna_wall_left_select(Evas_Object *obj)
{
    Picture_Item *pi, *ppi;
    int row, col;
    Evas_Coord x, y;
    Evas_Coord w;

    API_ENTRY
    return;

    ppi = _smart_selected_item_get(sd, &row, &col);

    if (!ppi)
    {
        col = 0;
    }
    else
    {
        _smart_item_unselect(sd, ppi);
        col--;
    }

    pi = evas_list_nth(sd->items[sd->row_sel], col);
    if (pi)
    {
        _smart_item_select(sd, pi);
        evas_object_geometry_get(pi->o_edje, NULL, NULL, &w, NULL);
        enna_scrollframe_child_pos_get(sd->o_scroll, &x, &y);
        enna_scrollframe_child_pos_set(sd->o_scroll, x - w, y);
    }

}


///////////////////// RIGHT /////////////////////
void enna_wall_right_select(Evas_Object *obj)
{
#if 0
    Picture_Item *pi, *ppi;
    Evas_Coord x, y;
    Evas_Coord w;

    API_ENTRY return;

    if (sd->down.momentum_animator)
    {
        ecore_animator_del(sd->down.momentum_animator);
        sd->down.momentum_animator = NULL;
    }

    if (!sd->down.now)
    {
        int row = 0;
        int col = 0;

        ppi = _smart_selected_item_get(sd, &row, &col);
        if (col == evas_list_count(sd->items[row]) - 1 || !ppi)
        {
            return;
        }
        else
        {
            _smart_item_unselect(sd, ppi);
            col++;
            pi = evas_list_nth(sd->items[row], col);
        }

        pi->selected = 1;
        sd->col_sel = col;
        sd->row_sel = row;
        sd->down.o_selected = pi;
        evas_object_geometry_get(ppi->o_edje, NULL, NULL, &w, NULL);
        enna_scrollframe_child_pos_get(sd->o_scroll, &x, &y);
        sd->down.now = 1;
        sd->down.sx = x;
        sd->down.x = x + w;
        sd->down.dt = 1500.0;
        sd->down.anim_start = ecore_time_get();
        enna_scrollframe_child_pos_get(sd->o_scroll, &x, &y);

    }
    else
    {

        w = 0;
        ppi = evas_list_nth(sd->items[sd->row_sel], sd->col_sel);
        if (ppi)
        {
            ppi->selected = 0;
            sd->col_sel++;

        }
        pi = evas_list_nth(sd->items[sd->row_sel], sd->col_sel);
        if (pi)
        {
            pi->selected = 1;
            evas_object_geometry_get(ppi->o_edje, NULL, NULL, &w, NULL);
        }

        sd->down.o_selected = pi;
        sd->down.dt += 500.0;
        if (sd->down.dt> 3000)
        sd->down.dt = 3000;

        sd->down.x += w;
    }

    //if (!sd->down.momentum_animator)
    //    sd->down.momentum_animator = ecore_animator_add(_smart_momentum_animator, sd)
#endif

    Picture_Item *pi, *ppi;
    int row, col;
    Evas_Coord x, y;
    Evas_Coord w;

    API_ENTRY
    return;

    ppi = _smart_selected_item_get(sd, &row, &col);

    if (!ppi)
    {
        col = 0;
    }
    else
    {
        _smart_item_unselect(sd, ppi);
        col++;
    }

    pi = evas_list_nth(sd->items[sd->row_sel], col);
    if (pi)
    {
        _smart_item_select(sd, pi);
        evas_object_geometry_get(pi->o_edje, NULL, NULL, &w, NULL);
        enna_scrollframe_child_pos_get(sd->o_scroll, &x, &y);
        enna_scrollframe_child_pos_set(sd->o_scroll, x + w, y);
    }
}

///////////////////// UP /////////////////////
void enna_wall_up_select(Evas_Object *obj)
{
    Picture_Item *pi, *ppi;
    Evas_List *l;
    Evas_Coord sx, x;
    int i;
    int row, col;

    API_ENTRY
    return;

    ppi = NULL;
    pi = NULL;
    sx = 0;
    x = 0;

    if (sd->down.now && sd->down.momentum_animator)
    {
        ecore_animator_del(sd->down.momentum_animator);
        sd->down.momentum_animator = NULL;
    }

    ppi = _smart_selected_item_get(sd, &row, &col);
    if (ppi)
    {
        evas_object_geometry_get(ppi->o_edje, &sx, NULL, NULL, NULL);
        _smart_item_unselect(sd, ppi);
    }

    row--;
    if (row < 0)
        row = 2;
    sd->row_sel = row;

    for (l = sd->items[sd->row_sel], i = 0; l; l = l->next, ++i)
    {
        Picture_Item *pi;
        pi = l->data;
        evas_object_geometry_get(pi->o_edje, &x, NULL, NULL, NULL);
        if (x >= sx)
        {
            _smart_item_select(sd, pi);
            break;
        }
    }
}
///////////////////// DOWN /////////////////////
void enna_wall_down_select(Evas_Object *obj)
{
    Picture_Item *pi, *ppi;
    Evas_List *l;
    Evas_Coord sx, x;
    int i;
    int row, col;

    API_ENTRY
    return;

    ppi = NULL;
    pi = NULL;
    sx = 0;
    x = 0;

    if (sd->down.now && sd->down.momentum_animator)
    {
        ecore_animator_del(sd->down.momentum_animator);
        sd->down.momentum_animator = NULL;
    }

    ppi = _smart_selected_item_get(sd, &row, &col);
    if (ppi)
    {
        evas_object_geometry_get(ppi->o_edje, &sx, NULL, NULL, NULL);
        _smart_item_unselect(sd, ppi);
    }
    row ++;
    row %= 3;
    sd->row_sel = row;
    for (l = sd->items[sd->row_sel], i = 0; l; l = l->next, ++i)
    {
        Picture_Item *pi;
        pi = l->data;
        evas_object_geometry_get(pi->o_edje, &x, NULL, NULL, NULL);
        if (x >= sx)
        {
            _smart_item_select(sd, pi);
            break;
        }
    }

}
/* local subsystem globals */

static Picture_Item *_smart_selected_item_get(Smart_Data *sd, int *row,
        int *col)
{
    Evas_List *l;
    int i, j;

    for (i = 0; i < 3; ++i)
    {
        for (l = sd->items[i], j = 0; l; l = l->next, j++)
        {
            Picture_Item *pi = l->data;
            if (pi->selected)
            {
                if (row)
                    *row = i;
                if (col)
                    *col = j;
                return pi;
            }
        }
    }
    if (row)
        *row = -1;
    if (col)
        *col = -1;
    return NULL;
}

static void _smart_item_unselect(Smart_Data *sd, Picture_Item *pi)
{
    if (!pi || !sd)
        return;
    pi->selected = 0;
    edje_object_signal_emit(pi->o_edje, "unselect", "enna");

}

static void _smart_item_select(Smart_Data *sd, Picture_Item *pi)
{
    if (!pi || !sd)
        return;

    pi->selected = 1;
    evas_object_raise(pi->o_edje);
    evas_object_raise(pi->sd->o_box[pi->row]);
    switch (pi->row)
    {
    case 0:
        edje_object_signal_emit(pi->o_edje, "select0", "enna");
        break;
    case 1:
        edje_object_signal_emit(pi->o_edje, "select1", "enna");
        break;
    case 2:
        edje_object_signal_emit(pi->o_edje, "select2", "enna");
        break;
    default:
        break;
    }
    sd->row_sel = pi->row;
}

static void _smart_event_mouse_down(void *data, Evas *evas, Evas_Object *obj,
        void *event_info)
{
    Picture_Item *pi, *ppi;
    int col, row;

    pi = data;

    ppi = _smart_selected_item_get(pi->sd, &row, &col);
    if (ppi && ppi != pi)
    {
        ppi->selected = 0;
        edje_object_signal_emit(ppi->o_edje, "unselect", "enna");
    }
    else if (ppi == pi)
        return;

    evas_object_raise(pi->sd->o_box[pi->row]);
    evas_object_raise(pi->o_edje);
    pi->selected = 1;
    pi->sd->row_sel = pi->row;
    switch (pi->row)
    {
    case 0:
        edje_object_signal_emit(pi->o_edje, "select0", "enna");
        break;
    case 1:
        edje_object_signal_emit(pi->o_edje, "select1", "enna");
        break;
    case 2:
        edje_object_signal_emit(pi->o_edje, "select2", "enna");
        break;
    default:
        break;
    }

}

static void _smart_reconfigure(Smart_Data * sd)
{
    Evas_Coord x, y, w, h;
    Evas_Coord ow = 0;

    int i = 0;

    x = sd->x;
    y = sd->y;
    w = sd->w;
    h = sd->h;

    evas_object_move(sd->o_scroll, x, y);
    evas_object_resize(sd->o_scroll, w, h);

    //  evas_object_geometry_get(sd->o_box[0], &ox, &oy, NULL, NULL);


    for (i = 0; i < 3; i++)
    {
        enna_box_min_size_get(sd->o_box[i], &w, &h);
        evas_object_resize(sd->o_box[i], w, sd->h / 3);
        if (w > ow)
            ow = w;
        enna_box_pack_options_set(sd->o_box[i], 0, 0, 0, 0, 0, 0, w, sd->h / 3,
                99999, 9999);

    }

    enna_box_min_size_get(sd->o_cont, &w, &h);
    evas_object_resize(sd->o_cont, w, h);

}

static void _smart_init(void)
{
    if (_smart)
        return;
    static const Evas_Smart_Class sc =
    { SMART_NAME, EVAS_SMART_CLASS_VERSION, _smart_add, _smart_del,
            _smart_move, _smart_resize, _smart_show, _smart_hide,
                    _smart_color_set, _smart_clip_set, _smart_clip_unset, NULL,
                    NULL };
    _smart = evas_smart_class_new(&sc);
}

static void _smart_add(Evas_Object * obj)
{
    Smart_Data *sd;
    int i;
    sd = calloc(1, sizeof(Smart_Data));
    if (!sd)
        return;
    sd->obj = obj;
    sd->nb = -1;
    sd->o_scroll = enna_scrollframe_add(evas_object_evas_get(obj));
    evas_object_move(sd->o_scroll, 0, 0);
    evas_object_show(sd->o_scroll);
    enna_scrollframe_policy_set(sd->o_scroll, ENNA_SCROLLFRAME_POLICY_ON,
            ENNA_SCROLLFRAME_POLICY_ON);

    sd->o_cont = enna_box_add(evas_object_evas_get(obj));
    enna_box_homogenous_set(sd->o_cont, 0);
    enna_box_orientation_set(sd->o_cont, 0);
    evas_object_show(sd->o_cont);
    enna_scrollframe_child_set(sd->o_scroll, sd->o_cont);

    for (i = 0; i < 3; i++)
    {
        sd->o_box[i] = enna_box_add(evas_object_evas_get(obj));
        enna_box_homogenous_set(sd->o_box[i], 0);
        enna_box_orientation_set(sd->o_box[i], 1);
        enna_box_pack_end(sd->o_cont, sd->o_box[i]);
        evas_object_show(sd->o_box[i]);
    }

    evas_object_smart_member_add(sd->o_scroll, obj);
    evas_object_smart_data_set(obj, sd);
}

static void _smart_del(Evas_Object * obj)
{
    Smart_Data *sd;
    int i;
    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_del(sd->o_scroll);
    evas_object_del(sd->o_cont);
    for (i = 0; i < 3; i++)
    {
        evas_object_del(sd->o_box[i]);
        while (sd->items[i])
        {
            Picture_Item *pi = sd->items[i]->data;
            sd->items[i] = evas_list_remove_list(sd->items[i], sd->items[i]);
            evas_object_del(pi->o_edje);
            evas_object_del(pi->o_pict);
            free(pi);
        }
        free(sd->items[i]);
    }

    evas_object_del(sd->o_scroll);
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
    evas_object_show(sd->o_scroll);
}

static void _smart_hide(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_hide(sd->o_scroll);
}

static void _smart_color_set(Evas_Object * obj, int r, int g, int b, int a)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_color_set(sd->o_scroll, r, g, b, a);
}

static void _smart_clip_set(Evas_Object * obj, Evas_Object * clip)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_set(sd->o_scroll, clip);
}

static void _smart_clip_unset(Evas_Object * obj)
{
    Smart_Data *sd;

    sd = evas_object_smart_data_get(obj);
    if (!sd)
        return;
    evas_object_clip_unset(sd->o_scroll);
}

