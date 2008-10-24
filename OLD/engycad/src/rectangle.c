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

Evas_Object        *l1 = NULL, *l2 = NULL, *l3 = NULL, *l4 = NULL;

/* protos */
void                _rect_create(double, double, double, double);

void
pre_rect_x1y1(double x, double y)
{
    XY                 *xy;
    Eina_List          *list = NULL;

    xy = (XY *) malloc(sizeof(XY));
    ENGY_ASSERT(xy);

    xy->x = x;
    xy->y = y;

    list = eina_list_append(list, xy);
    msg_create_and_send(CMD_PRE_DATA, 0, list);
}

void
rect_create(void)
{
    char               *s;
    float               x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    int                 res;

    serv_set_hint(DUP(_("enter top left point: ")));

    serv_set_state(ST_RECT1);

    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x1, &y1);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_RECT2);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    pre_rect_x1y1(x1, y1);

    serv_set_hint(DUP(_("enter bottom right point: ")));
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
          return;
      }

    lock_data();
    _rect_create(x1, y1, x2, y2);
    shell->context.fx = x2;
    shell->context.fy = y2;
    unlock_data();

}

void
_rect_create(double x1, double y1, double x2, double y2)
{
    char                buf[4096];

    sprintf(buf, "line|%f %f|%f %f|%f %f|%f %f|%f %f|done",
            x1, y1, x1, y2, x2, y2, x2, y1, x1, y1);
    serv_put_string(DUP(buf));
}

void
ghost_rect_create(void)
{
    Evas               *e;

    e = shell->evas;
    if (!l1)
      {
          l1 = evas_object_line_add(e);
          l2 = evas_object_line_add(e);
          l3 = evas_object_line_add(e);
          l4 = evas_object_line_add(e);
          evas_object_color_set(l1, ALPHA2, ALPHA2/5, ALPHA2/5, ALPHA2);
          evas_object_color_set(l2, ALPHA2, ALPHA2/5, ALPHA2/5, ALPHA2);
          evas_object_color_set(l3, ALPHA2, ALPHA2/5, ALPHA2/5, ALPHA2);
          evas_object_color_set(l4, ALPHA2, ALPHA2/5, ALPHA2/5, ALPHA2);
          evas_object_layer_set(l1, 17);
          evas_object_layer_set(l2, 17);
          evas_object_layer_set(l3, 17);
          evas_object_layer_set(l4, 17);
          evas_object_pass_events_set(l1, 1);
          evas_object_pass_events_set(l2, 1);
          evas_object_pass_events_set(l3, 1);
          evas_object_pass_events_set(l4, 1);
      }
    evas_object_show(l1);
    evas_object_show(l2);
    evas_object_show(l3);
    evas_object_show(l4);
}

void
ghost_rect_redraw(Eina_List *data, double x, double y)
{
    XY                 *xy;
    Evas               *e;
    Drawing            *d;
    double              x1, y1, x2, y2;

    if (!data)
        return;
    e = shell->evas;
    d = drawing;
    if (!d)
        return;

    xy = (XY *) eina_list_last(data)->data;
    x1 = w2s_x(xy->x);
    y1 = w2s_y(xy->y);
    x2 = w2s_x(x);
    y2 = w2s_y(y);

    evas_object_line_xy_set(l1, x1, y1, x1, y2);
    evas_object_line_xy_set(l2, x1, y2, x2, y2);
    evas_object_line_xy_set(l3, x2, y2, x2, y1);
    evas_object_line_xy_set(l4, x2, y1, x1, y1);
}

void
ghost_rect_destroy(void)
{
    evas_object_hide(l1);
    evas_object_hide(l2);
    evas_object_hide(l3);
    evas_object_hide(l4);
}
