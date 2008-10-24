/*
 * Copyright (C) 2002, Yury Hudobin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "engy.h"

typedef struct _Candidate Cand;

struct _Candidate
{
    int                 flags;
    double              x;
    double              y;
    double              a, b;
    double              ar1, ar2, angle;
    MG                 *parent;
};

Eina_List          *candlist = NULL;
Eina_List          *a_mg = NULL;
Eina_List          *mclist = NULL;

int                 mg_mask = 0xff;

Evas_Object        *hint_bg = NULL, *hint_te = NULL, *marker1 = NULL, *marker2 =
    NULL;
double              mg_x, mg_y;
int                 mg_flags;

/* protos */
void                magnet_attach_point(Point *);
void                magnet_attach_line(Line *);
MC                 *mc_create();
MG                 *mg_create();
MC                 *magnet_find_parent(void *);
int                mg_on_off(void *);
void                mg_on(MG *);
void                mg_off(MG *);

Cand               *mg_check_xy(MG *, double, double);
Cand               *mg_check_arc(MG *, double, double);
Cand               *mg_check_ab(MG *, double, double);
Cand               *mg_check_x(MG *, double, double);
Cand               *mg_check_y(MG *, double, double);

void                mg_create_along(Cand *);
void                mg_create_intersection(Cand *, Cand *);
void                parse_candidates(void);

/* callbacks */
static void 
    _mg_mouse_down(void *data, Evas *_e, Evas_Object *_o, void *event_info);

static void
      _mg_mouse_up(void *data, Evas *_e, Evas_Object *_o, void *event_info);

static void
      _mg_mouse_in(void *data, Evas *_e, Evas_Object *_o, void *event_info);

static void 
    _mg_mouse_out(void *data, Evas *_e, Evas_Object *_o, void *event_info);


void
magnet_attach(void *parent)
{
    Object             *obj;

    obj = (Object *) parent;

    if (obj->flags & FLAG_DELETED)
        return;

    switch (obj->type)
      {
      case OBJ_NONE:
          return;
      case OBJ_POINT:
          magnet_attach_point((Point *) parent);
          break;
      case OBJ_LINE:
          magnet_attach_line((Line *) parent);
          break;

      }
}

void
magnet_detach(void *pnt)
{
    MC                 *mc;

    mc = magnet_find_parent(pnt);
    if (!mc)
        return;
    mclist = eina_list_remove(mclist, mc);
    a_mg = eina_list_remove(a_mg, mc->ep1);
    a_mg = eina_list_remove(a_mg, mc->ep2);
    a_mg = eina_list_remove(a_mg, mc->mp);
    a_mg = eina_list_remove(a_mg, mc->cp);

    if (mc->ep1)
        evas_object_del(mc->ep1->o);
    if (mc->ep2)
        evas_object_del(mc->ep2->o);
    if (mc->mp)
        evas_object_del(mc->mp->o);
    if (mc->cp)
        evas_object_del(mc->cp->o);

    IF_FREE(mc->ep1);
    IF_FREE(mc->ep2);
    IF_FREE(mc->mp);
    IF_FREE(mc->cp);
    FREE(mc);
}

MC                 *
magnet_find_parent(void *pnt)
{
    Eina_List          *l;
    MC                 *mc;

    for (l = mclist; l; l = l->next)
      {
          mc = (MC *) l->data;
          if (mc->parent == pnt)
              return mc;
      }
    mc = mc_create();
    mc->parent = pnt;
    mclist = eina_list_append(mclist, mc);
    return mc;
}

MC                 *
mc_create(void)
{
    MC                 *mc;

    mc = (MC *) malloc(sizeof(MC));
    ENGY_ASSERT(mc);
    memset(mc, 0, sizeof(MC));
    return mc;
}

MG                 *
mg_create(void)
{
    MG                 *mg;

    mg = (MG *) malloc(sizeof(MG));
    ENGY_ASSERT(mg);
    memset(mg, 0, sizeof(MG));
    return mg;
}

void
enqueue_to_redraw(MG * mg)
{
    msg_create_and_send(CMD_REDRAW, OBJ_MAGNET, mg);
}

void
magnet_attach_point(Point * pnt)
{
    MC                 *mc;

    mc = magnet_find_parent(pnt);
    mc->flags = MC_CP;

    if (!mc->cp)
        mc->cp = mg_create();
    mc->cp->flags = MG_XY | MG_X | MG_Y;
    mc->cp->parent = mc;
    mc->cp->x = pnt->x;
    mc->cp->y = pnt->y;
    enqueue_to_redraw(mc->cp);
}

void
magnet_attach_line(Line * pnt)
{
    MC                 *mc;
    double              tan;

    mc = magnet_find_parent(pnt);
    mc->flags = MC_EP1 | MC_EP2 | MC_MP;

    if (!mc->ep1)
        mc->ep1 = mg_create();
    mc->ep1->flags = MG_XY | MG_X | MG_Y;
    mc->ep1->parent = mc;
    mc->ep1->x = pnt->x1;
    mc->ep1->y = pnt->y1;
    tan = pnt->x2 - pnt->x1;
    if (tan != 0)
      {
          mc->ep1->flags |= MG_AB;
          mc->ep1->b = pnt->y2 - (pnt->y2 - pnt->y1) / tan * pnt->x2;
          mc->ep1->a = (pnt->y2 - pnt->y1) / tan;
      }
    enqueue_to_redraw(mc->ep1);

    if (!mc->ep2)
        mc->ep2 = mg_create();
    mc->ep2->flags = MG_XY | MG_X | MG_Y;
    mc->ep2->parent = mc;
    mc->ep2->x = pnt->x2;
    mc->ep2->y = pnt->y2;
    tan = pnt->x2 - pnt->x1;
    if (tan != 0)
      {
          mc->ep2->flags |= MG_AB;
          mc->ep2->b = pnt->y2 - (pnt->y2 - pnt->y1) / tan * pnt->x2;
          mc->ep2->a = (pnt->y2 - pnt->y1) / tan;
      }

    enqueue_to_redraw(mc->ep2);

    if (!mc->mp)
        mc->mp = mg_create();
    mc->mp->flags = MG_XY | MG_X | MG_Y;
    mc->mp->parent = mc;
    mc->mp->x = pnt->x1 + (pnt->x2 - pnt->x1) / 2;
    mc->mp->y = pnt->y1 + (pnt->y2 - pnt->y1) / 2;
    tan = pnt->x2 - pnt->x1;
    if (tan != 0)
      {
          mc->mp->flags |= MG_AB;
          mc->mp->b = pnt->y2 - (pnt->y2 - pnt->y1) / tan * pnt->x2;
          mc->mp->a = (pnt->y2 - pnt->y1) / tan;
      }

    enqueue_to_redraw(mc->mp);
}

void
magnet_redraw(MG * mg)
{
    Evas_Object        *oo;
    Evas               *e;
    Drawing            *d;
    Evas_Coord          ex, ey, ew, eh;
    double              x1, y1;

    e = shell->evas;
    d = drawing;

    evas_output_viewport_get(e, &ex, &ey, &ew, &eh);

    if (!mg->o)
      {
          Evas_Object        *oo;

          oo = evas_object_rectangle_add(e);
          evas_object_resize(oo, 5, 5);
          evas_object_color_set(oo, 0, 0, 0, 0);
          evas_object_layer_set(oo, 17);
          evas_object_event_callback_add(oo, EVAS_CALLBACK_MOUSE_DOWN,
			  _mg_mouse_down, NULL);
          evas_object_event_callback_add(oo, EVAS_CALLBACK_MOUSE_UP, 
			  _mg_mouse_up, NULL);
          evas_object_event_callback_add(oo, EVAS_CALLBACK_MOUSE_IN, 
			  _mg_mouse_in, NULL);
          evas_object_event_callback_add(oo, EVAS_CALLBACK_MOUSE_OUT, 
			  _mg_mouse_out, NULL);
          evas_object_show(oo);
          mg->o = oo;
      }

    x1 = (int)((mg->x + d->x) * d->scale);
    y1 = (int)((mg->y + d->y) * d->scale);
    evas_object_move(mg->o, x1 - 2, ey + eh - y1 - 2);
}

static void
_mg_mouse_down(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
    Drawing            *d;
    double              x, y;
    Evas_Coord          ex, ey, ew, eh;
    int                 shift_state, ctrl_state;
    int			_b, _x, _y;
    Evas_Event_Mouse_Down * em = (Evas_Event_Mouse_Down*) event_info;

    if (!shell)
        return;
    if (!shell->evas)
        return;

    // GLS
    _b = em->button;
    shift_state = evas_key_modifier_is_set(em->modifiers, "Shift");
    ctrl_state = evas_key_modifier_is_set(em->modifiers, "Control");

    evas_output_viewport_get(shell->evas, &ex, &ey, &ew, &eh);
    d = drawing;
    if (!d)
        return;
    _x = em->canvas.x;
    _y = em->canvas.y;
    if (!ctrl_state && (_b == 1))
      {
          x = x / d->scale - d->x;
          y = -(y - eh - ey) / d->scale - d->y;
          gra_pre_draw_mouse_click(x, y);
      }

    if (!shift_state && !ctrl_state && (_b == 3))
      {
          menu_box_show(DUP("context.menu"), (double)_x + 1, (double)_y + 1, 1);
      }
}
static void
_mg_mouse_up(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
}
static void
_mg_mouse_in(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
    Eina_List          *l;
    MC                 *mc;
    MG                 *mg;

    for (l = mclist; l; l = l->next)
      {
          mc = (MC *) l->data;
          if (mc->ep1 && (mc->ep1->o == _o))
              mg = mc->ep1;
          if (mc->ep2 && (mc->ep2->o == _o))
              mg = mc->ep2;
          if (mc->mp && (mc->mp->o == _o))
              mg = mc->mp;
          if (mc->cp && (mc->cp->o == _o))
              mg = mc->cp;
      }
    evas_object_data_set(_o, "over", "1");
    ecore_timer_add(0.4, mg_on_off, (void *)mg);
}
static void
_mg_mouse_out(void *data, Evas *_e, Evas_Object *_o, void *event_info)
{
    evas_object_data_del(_o, "over");
}

int
mg_on_off(void *data)
{
    MG                 *mg;

    mg = (MG *) data;
    if (!evas_object_data_get(mg->o, "over"))
        return;

    if (mg->on)
      {
          mg_off(mg);
      }
    else
      {
          mg_on(mg);
      }
    return 0;
}

void
mg_on(MG * mg)
{
    evas_object_color_set(mg->o, 40, 120, 160, 200);
    mg->on = 1;
    a_mg = eina_list_append(a_mg, mg);
}

void
mg_off(MG * mg)
{
    evas_object_color_set(mg->o, 0, 0, 0, 0);
    mg->on = 0;
    a_mg = eina_list_remove(a_mg, mg);
}

void
magnet_off_all(void)
{
    while (a_mg)
      {
          mg_off((MG *) a_mg->data);
      }
}

void
magnet_mouse_move(int _x, int _y)
{
    Eina_List          *l;
    MG                 *mg;
    Cand               *cand;
    double              x, y;

    x = s2w_x(_x);
    y = s2w_y(_y);

    for (l = candlist; l; l = l->next)
        FREE(l->data);
    candlist = eina_list_free(candlist);

    for (l = a_mg; l; l = l->next)
      {
          mg = (MG *) l->data;
          cand = mg_check_xy(mg, x, y);
          if (cand)
              candlist = eina_list_append(candlist, cand);
      }
    cand = NULL;                /* I can't deal with 2 arc */
    for (l = a_mg; l && !cand; l = l->next)
      {
          mg = (MG *) l->data;
          cand = mg_check_arc(mg, x, y);
          if (cand)
              candlist = eina_list_append(candlist, cand);
      }
    for (l = a_mg; l; l = l->next)
      {
          mg = (MG *) l->data;
          cand = mg_check_ab(mg, x, y);
          if (cand)
              candlist = eina_list_append(candlist, cand);
      }

    cand = NULL;
    for (l = a_mg; l && !cand; l = l->next)
      {
          mg = (MG *) l->data;
          cand = mg_check_x(mg, x, y);
          if (cand)
              candlist = eina_list_append(candlist, cand);
      }

    cand = NULL;
    for (l = a_mg; l && !cand; l = l->next)
      {
          mg = (MG *) l->data;
          cand = mg_check_y(mg, x, y);
          if (cand)
              candlist = eina_list_append(candlist, cand);
      }

    parse_candidates();
}

Cand               *
mg_check_xy(MG * mg, double x, double y)
{
    Cand               *cn;
    double              epsilon;
    double              dist;

    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;
    if (!(mg->flags & MG_XY))
        return NULL;

    dist = (mg->x - x) * (mg->x - x) + (mg->y - y) * (mg->y - y);
    if (dist > epsilon)
        return NULL;
    cn = (Cand *) malloc(sizeof(Cand));
    ENGY_ASSERT(cn);
    memset(cn, 0, sizeof(Cand));
    cn->flags = CAND_XY;
    cn->x = mg->x;
    cn->y = mg->y;
    cn->parent = mg;
    return cn;
}

Cand               *
mg_check_arc(MG * mg, double x, double y)
{
    Cand               *cn;
    double              epsilon;
    double              dist;
    double              tmp;

    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;
    if (!(mg->flags & MG_AR))
        return NULL;

    tmp = x;
    x -= mg->ax;
    y -= mg->ay;
    x = tmp * cos(-mg->angle) + y * sin(-mg->angle);
    y = y * cos(-mg->angle) - tmp * sin(-mg->angle);

    dist = 1 - (x / mg->ar1) * (x / mg->ar1) - (y / mg->ar2) * (y / mg->ar2);
    dist *= dist;
    if (dist > epsilon)
        return NULL;
    cn = (Cand *) malloc(sizeof(Cand));
    ENGY_ASSERT(cn);
    memset(cn, 0, sizeof(Cand));
    cn->flags = CAND_ARC;
    cn->x = mg->ax;
    cn->y = mg->ay;
    cn->ar1 = mg->ar1;
    cn->ar2 = mg->ar2;
    cn->angle = mg->angle;
    cn->parent = mg;
    return cn;
}

Cand               *
mg_check_ab(MG * mg, double x, double y)
{
    Cand               *cn;
    double              epsilon;
    double              dist;
    double              b1;

    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;
    if (!(mg->flags & MG_AB))
        return NULL;
    if(0.0 == mg->a)
	    return NULL;
    dist = mg->b - y + mg->a * x;
    dist *= dist;

    if (dist > epsilon)
        return NULL;

    cn = (Cand *) malloc(sizeof(Cand));
    ENGY_ASSERT(cn);
    memset(cn, 0, sizeof(Cand));
    b1 = y + x / mg->a;
    cn->flags = CAND_LINE;
    cn->a = mg->a;
    cn->b = mg->b;
    cn->x = mg->a * (b1 - mg->b) / (mg->a * mg->a + 1);
    cn->y = mg->a * cn->x + mg->b;
    cn->parent = mg;

    return cn;
}

Cand               *
mg_check_x(MG * mg, double x, double y)
{
    Cand               *cn;
    double              epsilon;
    double              dist;

    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;
    if (!(mg->flags & MG_X))
        return NULL;

    dist = mg->x - x;
    dist *= dist;
    if (dist > epsilon)
        return NULL;

    cn = (Cand *) malloc(sizeof(Cand));
    ENGY_ASSERT(cn);
    memset(cn, 0, sizeof(Cand));
    cn->flags = CAND_X;
    cn->x = mg->x;
    cn->y = y;
    cn->parent = mg;

    return cn;
}

Cand               *
mg_check_y(MG * mg, double x, double y)
{
    Cand               *cn;
    double              epsilon;
    double              dist;

    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;
    if (!(mg->flags & MG_X))
        return NULL;

    dist = mg->y - y;
    dist *= dist;
    if (dist > epsilon)
        return NULL;

    cn = (Cand *) malloc(sizeof(Cand));
    ENGY_ASSERT(cn);
    memset(cn, 0, sizeof(Cand));
    cn->flags = CAND_Y;
    cn->x = x;
    cn->y = mg->y;
    cn->parent = mg;

    return cn;
}

void
mg_check_objects(void)
{
    Evas               *e;

    e = shell->evas;

    if (!hint_bg)
      {
          hint_bg = evas_object_rectangle_add(e);
          evas_object_color_set(hint_bg, 
			  200*ALPHA2/255, 
			  ALPHA2, 
			  ALPHA2, 
			  ALPHA2);
          evas_object_layer_set(hint_bg, 20);
          evas_object_pass_events_set(hint_bg, 1);
      }
    if (!hint_te)
      {
          hint_te = evas_object_text_add(e);
	  evas_object_text_font_set(hint_te, "verdana", 9);
	  evas_object_text_text_set(hint_te, "");
          evas_object_color_set(hint_te, 0, 0, 0, 255);
          evas_object_layer_set(hint_te, 20);
          evas_object_pass_events_set(hint_te, 1);
      }
    if (!marker1)
      { // FIXME !!!!
          marker1 = evas_image_load("resources/pics/marker.png");
          evas_object_layer_set(marker1, 17);
          evas_object_pass_events_set(marker1, 1);
      }
    if (!marker2)
      {
          marker2 = evas_image_load("resources/pics/marker.png");
          evas_object_layer_set(marker2, 17);
          evas_object_pass_events_set(marker2, 1);
      }
}

void
parse_candidates(void)
{
    Evas               *e;
    Cand               *c1 = NULL, *c2 = NULL;
    Eina_List          *l, *l2;

    if (!shell)
        return;

    e = shell->evas;

    mg_check_objects();

    evas_object_hide(hint_bg);
    evas_object_hide(hint_te);
    evas_object_hide(marker1);
    evas_object_hide(marker2);

    mg_x = 0.0;
    mg_y = 0.0;
    mg_flags = 0;

    if (!candlist)
        return;

    c1 = (Cand *) candlist->data;

    if (!c1)
        return;

    if (c1->flags == CAND_XY)
      {
          /* along xy */
          mg_create_along(c1);
          candlist = eina_list_remove(candlist, c1);
          for (l = candlist; l; l = l->next)
              FREE(l->data);
          candlist = eina_list_free(candlist);
          return;
      }

    /* looking for couples */
    for (l = candlist->next; l && !c2; l = l->next)
      {
          Cand               *ctmp;

          ctmp = (Cand *) l->data;
          if (memcmp(c1, ctmp, sizeof(Cand)))
              c2 = ctmp;
      }
    if (candlist->next)
        c2 = (Cand *) candlist->next->data;

    candlist = eina_list_remove(candlist, c1);
    candlist = eina_list_remove(candlist, c2);

    for (l = candlist; l; l = l->next)
        FREE(l->data);
    candlist = eina_list_free(candlist);
    if (c2)
      {
          mg_create_intersection(c1, c2);
      }
    else
      {
          mg_create_along(c1);
      }
}

void
mg_create_along(Cand * c1)
{
    Evas               *e;
    char                buf[4096];
    char               *s;
    int                 pr;

    e = shell->evas;
    pr = shell->float_prec;

    switch (c1->flags)
      {
      case CAND_XY:
          mg_x = c1->x;
          mg_y = c1->y;
          mg_flags = 3;
          s = my_iconv(shell->dcd, _("XY: %.*f %.*f"));
          sprintf(buf, s, pr, mg_x, pr, mg_y);
          FREE(s);
	  evas_object_text_text_set(hint_te, buf);
          break;
      case CAND_LINE:
          mg_x = c1->x;
          mg_y = c1->y;
          mg_flags = 3;
          s = my_iconv(shell->dcd, _("Line: %.*f %.*f"));
          sprintf(buf, s, pr, mg_x, pr, mg_y);
          FREE(s);
	  evas_object_text_text_set(hint_te, buf);
          break;
      case CAND_ARC:
          mg_x = c1->x;
          mg_y = c1->y;
          mg_flags = 3;
          s = my_iconv(shell->dcd, _("Point on Arc: %.*f %.*f"));
          sprintf(buf, s, pr, mg_x, pr, mg_y);
          FREE(s);
	  evas_object_text_text_set(hint_te, buf);
          break;
      case CAND_X:
          mg_x = c1->x;
          mg_flags = 1;
          s = my_iconv(shell->dcd, _("X = %.*f"));
          sprintf(buf, s, pr, mg_x);
          FREE(s);
	  evas_object_text_text_set(hint_te, buf);
          break;
      case CAND_Y:
          mg_y = c1->y;
          mg_flags = 2;
          s = my_iconv(shell->dcd, _("Y = %.*f"));
          sprintf(buf, s, pr, mg_y);
          FREE(s);
	  evas_object_text_text_set(hint_te, buf);
          break;
      }

    evas_object_move(hint_te, (double)w2s_x(c1->x) + 10, (double)w2s_y(c1->y) + 10);
    evas_object_resize(hint_bg, evas_object_text_horiz_advance_get(hint_te) + 4, 16);
    evas_object_move(hint_bg, (double)w2s_x(c1->x) + 8, (double)w2s_y(c1->y) + 14);
    evas_object_move(marker1, (double)w2s_x(c1->parent->x),
              (double)w2s_y(c1->parent->y));
    evas_object_show(hint_te);
    evas_object_show(marker1);
    evas_object_show(hint_bg);
}

void
mg_create_intersection(Cand * c1, Cand * c2)
{
    Evas               *e;
    char                buf[4096];
    char               *s;
    int                 pr;

    e = shell->evas;
    pr = shell->float_prec;
    /*if((c1->flags == CAND_ARC) &&(c2->flags == CAND_ARC)){
     * return;
     * }*/
    if ((c1->flags == CAND_ARC) && (c2->flags == CAND_LINE))
      {
          return;
      }
    if ((c1->flags == CAND_ARC) && (c2->flags == CAND_X))
      {
          return;
      }
    if ((c1->flags == CAND_ARC) && (c2->flags == CAND_Y))
      {
          return;
      }
    if ((c1->flags == CAND_LINE) && (c2->flags == CAND_LINE))
      {
          if (c1->a == c2->a)
              return;
          mg_x = (c1->b - c2->b) / (c2->a - c1->a);
          mg_y = c1->a * mg_x + c1->b;
          mg_flags = 3;
          s = my_iconv(shell->dcd, _("Line - Line: %.*f %.*f"));
          sprintf(buf, s, pr, mg_x, pr, mg_y);
          FREE(s);
      }
    if ((c1->flags == CAND_LINE) && (c2->flags == CAND_X))
      {
          mg_x = c2->x;
          mg_y = c1->a * mg_x + c1->b;
          mg_flags = 3;
          s = my_iconv(shell->dcd, _("Line - X: %.*f %.*f"));
          sprintf(buf, s, pr, mg_x, pr, mg_y);
          FREE(s);
      }
    if ((c1->flags == CAND_LINE) && (c2->flags == CAND_Y))
      {
	  if(0.0 == c1->a)
		  return;
          mg_y = c2->y;	 
	  mg_x = (mg_y - c1->b) / c1->a;
          mg_flags = 3;
          s = my_iconv(shell->dcd, _("Line - Y: %.*f %.*f"));
          sprintf(buf, s, pr, mg_x, pr, mg_y);
          FREE(s);
      }
    if ((c1->flags == CAND_X) && (c2->flags == CAND_Y))
      {
          mg_x = c1->x;
          mg_y = c2->y;
          mg_flags = 3;
          s = my_iconv(shell->dcd, _("X - Y: %.*f %.*f"));
          sprintf(buf, s, pr, mg_x, pr, mg_y);
          FREE(s);
      }

    evas_object_text_text_set(hint_te, buf);    
    evas_object_move(hint_te, (double)w2s_x(mg_x) + 10, (double)w2s_y(mg_y) + 10);
    evas_object_resize(hint_bg, 
		    evas_object_text_horiz_advance_get(hint_te) + 4, 16);
    evas_object_move(hint_bg, (double)w2s_x(mg_x) + 8, (double)w2s_y(mg_y) + 14);
    evas_object_move(marker1, (double)w2s_x(c1->parent->x),
              (double)w2s_y(c1->parent->y));
    evas_object_move(marker2, (double)w2s_x(c2->parent->x),
              (double)w2s_y(c2->parent->y));
    evas_object_show(hint_te);
    evas_object_show(marker1);
    evas_object_show(marker2);
    evas_object_show(hint_bg);
}

void
magnet_get_xy(int *res, double *_x, double *_y)
{
    int                 mode;

    *res = mg_flags;

    mode = shell->is_shifted;
    if (!mode)
      {
          if (shell->snap)
              *_x = shell->snap * (int)(*_x / shell->snap);
          if (mg_flags & 1)
              *_x = mg_x;
          if (shell->snap)
              *_y = shell->snap * (int)(*_y / shell->snap);
          if (mg_flags & 2)
              *_y = mg_y;
      }
    else
      {
          double              dx, dy;

          dx = fabs(shell->context.fx - *_x);
          dy = fabs(shell->context.fy - *_y);

          if (dx > dy)
            {
                *_y = shell->context.fy;
                if (shell->snap)
                    *_x = shell->snap * (int)(*_x / shell->snap);
                if (mg_flags & 1)
                    *_x = mg_x;
            }
          else
            {
                *_x = shell->context.fx;
                if (shell->snap)
                    *_y = shell->snap * (int)(*_y / shell->snap);
                if (mg_flags & 2)
                    *_y = mg_y;
            }
      }
}
