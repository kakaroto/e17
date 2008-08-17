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
#include <math.h>

Evas_List          *hatch_items = NULL;
Evas_Object        *oh1;

/* protos */
Evas_List          *hatch_get_poly(void);
Evas_List          *find_points(Evas_List*, double, double, double, double);
Evas_List          *sort_list(Evas_List*);
void                _hatch_create_line(double x1, double y1, double x2,
                                       double y2, char *style, double scale,
                                       double thickness, double offset);

void
hatch_create(void)
{
    Evas_List          *list, *l;
    XY                 *a, *b;
    int                 i, num, res, res1;
    int                 flag = 0;
    Evas               *e;
    char                buf[4096];
    char               *s, *st;
    Evas_List          *poly;
    double              dx, dy, g, g2;

    e = shell->evas;

    hs_set(shell->context.hatch_style);

    poly = hatch_get_poly();

    serv_set_hint(DUP(_("enter hatch angle: ")));
    s = serv_get_string();

    g = atof(s);
    FREE(s);

    num = hs_get_num();
    for (i = 0; i < num; i++)
      {
          int                 flag = 0, cnt = 0;
          double              y, yr, ystep, scale;

          g2 = g + hs_get_angle(i);
          scale = hs_get_scale(i);
          ystep = hs_get_ystep(i);
          dy = hs_get_yoffset(i);
          dx = hs_get_xoffset(i);
          st = hs_get_line_style(i);

          trans_rotate(poly, g2 / 180 * M_PI);

          for (y = drawing->h * (-2); y < drawing->h * 3; y += ystep)
            {
                cnt++;
                yr = y + dy;
                list = find_points(poly, -210.0, yr, 420.0, yr);

                if (!list)
                    continue;
                if (!list->next)
                    continue;

                trans_rotate(list, -g2 / 180 * M_PI);

                flag = 0;
                for (l = list; l->next; l = l->next)
                  {
                      flag = !flag;

                      a = (XY *) l->data;
                      b = (XY *) l->next->data;
                      if (flag)
                        {
                            _hatch_create_line(a->x, a->y,
                                               b->x, b->y,
                                               DUP(st),
                                               scale,
                                               shell->
                                               context.thickness, dx * cnt);
                        }
                  }
            }
          trans_rotate(poly, -g2 / 180 * M_PI);
          FREE(st);
      }
}

XY                 *
_get_point(double x1, double y1, double x2, double y2,
           double x3, double y3, double x4, double y4)
{

    double              ua, ub;
    double              x, y;
    double              den;
    XY                 *res;

    den = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
    if (den == 0)
        return NULL;

    ua = (x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3);
    ua /= den;
    ub = (x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3);
    ub /= den;

    if ((ua >= 1) || (ua < 0) || (ub >= 1) || (ub < 0))
        return NULL;

    res = malloc(sizeof(XY));
    res->x = x1 + ua * (x2 - x1);
    res->y = y1 + ub * (y2 - y1);
    return res;
}

Evas_List*
find_points(Evas_List *poly, double x1, double y1, double x2, double y2)
{
    XY                 *a, *b;
    Evas_List          *sect = NULL;
    Evas_List          *l;
    double              x3, x4, y3, y4;
    XY                 *ret;

    if (!poly)
        return NULL;

    a = (XY *) evas_list_last(poly)->data;
    x3 = a->x;
    y3 = a->y;
    b = (XY *) poly->data;
    x4 = b->x;
    y4 = b->y;
    ret = _get_point(x1, y1, x2, y2, x3, y3, x4, y4);
    if (ret)
        sect = evas_list_append(sect, ret);

    for (l = poly; l->next; l = l->next)
      {
          a = (XY *) l->data;
          x3 = a->x;
          y3 = a->y;
          b = (XY *) l->next->data;
          x4 = b->x;
          y4 = b->y;
          ret = _get_point(x1, y1, x2, y2, x3, y3, x4, y4);
          if (ret)
              sect = evas_list_append(sect, ret);
      }
    if (!sect)
        return NULL;
    if (!sect->next)
        return NULL;

    for (l = sect; l; l = l->next)
        ENGY_ASSERT(l->data);
    return sort_list(sect);
}

void
hatch_item_redraw(Line * line)
{
    int                 flag, sign = 1;
    Evas_List          *list = NULL, *l, *lo;
    Evas               *e;
    Evas_Object        *o;
    Drawing            *d;
    float               x, y, dx, dy, len, len1;
    double              x1, y1, x2, y2, g;
    XY                 *xy;

    d = drawing;

    dx = line->x2 - line->x1;
    len1 = (dx > 0) ? hypot(line->x1, line->y1) : -hypot(line->x1, line->y1);
    if (dx == 0)
        dx = 1e-40;             /* ok for 1200 dpi? */
    dy = line->y2 - line->y1;
    sign = fabs(dx) / dx;
    g = atan(-dy / dx) + M_PI * (sign - 1) / 2;

    x1 = line->x1 * cos(-g) + line->y1 * sin(-g);
    y1 = line->y1 * cos(-g) - line->x1 * sin(-g);

    x2 = line->x2 * cos(-g) + line->y2 * sin(-g);
    y2 = line->y2 * cos(-g) - line->x2 * sin(-g);

    len = fabs(x2 - x1);

    flag = linestyle_get_odd();
    list = linestyle_get_dx_points(x1 + line->hatch_offset,
                                   x2 + line->hatch_offset, line->line_scale);
    ENGY_ASSERT(list);
    if (!list)
        return;
    if (!list->next)
        return;

    trans_move(list, -x1 - line->hatch_offset, 0);
    trans_rotate(list, g);
    trans_move(list, line->x1, line->y1);

    e = shell->evas;

    lo = line->list;

    for (l = list->next; l; l = l->next)
      {
          XY                 *a, *b;

          a = (XY *) l->prev->data;
          b = (XY *) l->data;

          flag = !flag;

          if (flag)
            {
                if (!lo || !lo->data)
                  {
                      o = _line_item(e, line->thickness *
                                     shell->context.show_thickness * d->scale);
                      evas_object_layer_set( o, 10);
                      evas_object_pass_events_set(o, 1);
                      line->list = evas_list_append(line->list, o);
                      lo = evas_list_last(line->list);
                  }
                o = lo->data;
                lo = lo->next;
                x1 = w2s_x(a->x);
                y1 = w2s_y(a->y);
                x2 = w2s_x(b->x);
                y2 = w2s_y(b->y);
                if (line->flags & FLAG_VISIBLE)
                  {
                      evas_object_color_set(o,
			line->color.red*line->color.alpha/255,
			line->color.green*line->color.alpha/255,
			line->color.blue*line->color.alpha/255, 
			line->color.alpha);
                  }
                else
                  {
                      evas_object_color_set(o, 0, 0, 0, 0);
                  }

                if (line->flags & FLAG_SELECTED)
                    evas_object_color_set(o, 
				    ALPHA5, 
				    ALPHA5/5, 
				    ALPHA5/5, 
				    ALPHA5);
                if (line->flags & FLAG_DELETED)
                    evas_object_color_set(o, 0, 0, 0, 0);
                _line_item_xy(e, o, x1, y1, x2, y2,
                              line->thickness *
                              shell->context.show_thickness * d->scale);
                evas_object_show(o);
            }
      }

    for (l = list; l; l = l->next)
        FREE(l->data);
    list = evas_list_free(list);
}

Evas_List*
hatch_get_poly(void)
{
    Evas_List          *list = NULL;
    XY                 *xy;
    int                 res;
    float               x1, y1, x2, y2;
    char               *s;

    serv_set_hint(DUP(_("enter first point: ")));

    serv_set_state(ST_LINE1);

    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x1, &y1);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    xy = (XY *) malloc(sizeof(XY));
    xy->x = x1;
    xy->y = y1;
    list = evas_list_append(list, xy);

    serv_set_state(ST_LINE2);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return NULL;
      }

    do
      {
          pre_line_x1y1(x1, y1);
          shell->context.fx = x1;
          shell->context.fy = y1;

          serv_set_hint(DUP(_("enter next point: ")));
          do
            {
                s = serv_get_string();
                res = get_values(s, x1, y1, &x2, &y2);
                if (res == 1)
                    serv_set_hint(DUP(_("error, please reenter: ")));
            }
          while (res == 1);
          if (res == 0)
            {
                serv_set_state(ST_NORMAL);
                return list;
            }

          xy = (XY *) malloc(sizeof(XY));
          xy->x = x2;
          xy->y = y2;
          list = evas_list_append(list, xy);

          x1 = x2;
          y1 = y2;

      }
    while (1);
}

Evas_List*
sort_list(Evas_List *src)
{
    Evas_List          *l, *l1;
    XY                 *a, *b;
    double              tmp;
    int                 need_y = 1;

    if (!src)
        return NULL;
    for (l = src; l; l = l->next)
        ENGY_ASSERT(l->data);

    for (l = src; l; l = l->next)
      {
          for (l1 = src; l1->next; l1 = l1->next)
            {
                a = (XY *) l1->data;
                b = (XY *) l1->next->data;
                ENGY_ASSERT(a);
                ENGY_ASSERT(b);
                if (a->x != b->x)
                    need_y = 0;
                if (a->x > b->x)
                  {
                      tmp = a->x;
                      a->x = b->x;
                      b->x = tmp;
                      tmp = a->y;
                      a->y = b->y;
                      b->y = tmp;
                  }
            }
      }

    if (!need_y)
        return src;

    for (l = src; l; l = l->next)
      {
          for (l1 = src; l1->next; l1 = l1->next)
            {
                a = (XY *) l1->data;
                b = (XY *) l1->next->data;
                if (a->y > b->y)
                  {
                      tmp = a->x;
                      a->x = b->x;
                      b->x = tmp;
                      tmp = a->y;
                      a->y = b->y;
                      b->y = tmp;
                  }
            }
      }
    return src;
}

void
_hatch_create_line(double x1, double y1, double x2, double y2,
                   char *style, double scale, double thickness, double offset)
{

    Line               *li;
    Layer              *layer;
    int                 sign1, sign2;
    double              a, b;

    layer = drawing->current_layer;
    if (!layer)
        return;
    if (!style)
        return;

    li = _line_create_copy(NULL);
    li->flags |= FLAG_HATCH_ITEM;
    li->x1 = x1;
    li->y1 = y1;
    li->x2 = x2;
    li->y2 = y2;
    li->color = shell->context.color;
    li->line_style = style;
    li->line_scale = scale;
    li->thickness = thickness;
    li->hatch_offset = offset;

    layer->objects = evas_list_append(layer->objects, li);

    append_undo_new_object(li, CMD_SYNC, OBJ_LINE, li);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, li);

    li->owner = (Object *) drawing->current_layer;
    line_ssync(li);

}
