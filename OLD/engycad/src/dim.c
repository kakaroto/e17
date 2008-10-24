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

/* protos */
void                _dim_h_create(double, double, double, double, double,
                                  double);
void                _dim_v_create(double, double, double, double, double,
                                  double);
void                _dim_a_create(double, double, double, double, double,
                                  double);

Evas_Object         *ol1 = NULL, *ol2 = NULL, *ol3 = NULL, *op1 = NULL;

void
dim_h_create(void)
{
    float               x1, y1, x2, y2, x3, y3;
    char               *s;
    int                 res;

    serv_set_hint(DUP(_("enter first point: ")));

    serv_set_state(ST_DIM1);

    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x1, &y1);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_DIM2);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    pre_line_x1y1(x1, y1);
    shell->context.fx = x1;
    shell->context.fy = y1;

    serv_set_hint(DUP(_("enter second point: ")));
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

    serv_set_state(ST_DIMH);

    pre_set_x1y1x2y2(x1, y1, x2, y2);

    serv_set_hint(DUP(_("define view of the dimension: ")));

    do
      {
          s = serv_get_string();
          res = get_values(s, x2, y2, &x3, &y3);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;

    lock_data();
    _dim_h_create(x1, y1, x2, y2, x3, y3);
    unlock_data();
}

void
_dim_create_line(double x1, double y1, double x2, double y2,
                 char *style, double scale, double thickness)
{
    Line               *li;
    Layer              *layer;
    double              a, b;

    layer = drawing->current_layer;
    if (!layer)
        return;

    li = _line_create_copy(NULL);

    li->x1 = x1;
    li->y1 = y1;
    li->x2 = x2;
    li->y2 = y2;
    li->color = shell->context.color;
    li->line_style = style;
    li->line_scale = scale;
    li->thickness = thickness;

    layer->objects = eina_list_append(layer->objects, li);

    append_undo_new_object(li, CMD_SYNC, OBJ_LINE, li);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, li);

    li->owner = (Object *) drawing->current_layer;
    line_ssync(li);
}

void
_dim_h_create(double x1, double y1, double x2, double y2, double x3, double y3)
{

    Text               *te;
    Layer              *layer;
    int                 sign1, sign2;
    double              a, b, pr, th, sc;
    char                buf[4096];
    char                radix_char[10];
    char               *s;

    layer = drawing->current_layer;
    if (!layer)
        return;

    dimstyle_set(shell->context.dim_style);

    sign1 = (y1 > y3) ? -1 : 1;
    sign2 = (y2 > y3) ? -1 : 1;

    a = dimstyle_get_ext_line_offset() * sign1;
    b = dimstyle_get_ext_line_extend() * sign1;

    s = dimstyle_get_ext_line_style();
    sc = dimstyle_get_ext_line_scale();
    th = dimstyle_get_ext_line_thickness();
    pr = dimstyle_get_dim_line_prespacing();

    _dim_create_line(x1, y1 + a, x1, y3 + b, s, sc, th);

    a = dimstyle_get_ext_line_offset() * sign2;
    b = dimstyle_get_ext_line_extend() * sign2;

    _dim_create_line(x2, y2 + a, x2, y3 + b, s, sc, th);

    s = dimstyle_get_dim_line_style();
    sc = dimstyle_get_dim_line_scale();
    th = dimstyle_get_dim_line_thickness();
    a = dimstyle_get_arrow_size();

    _dim_create_line(x1 + pr, y3, x2 - pr, y3, s, sc, th);
    s = dimstyle_get_arrow_style();

    _dim_create_line(x1, y3, x1 + a, y3, s, 1, th);

    _dim_create_line(x2, y3, x2 - a, y3, s, 1, th);

    te = _text_create_copy(NULL);
    te->color = shell->context.color;
    te->textstyle = dimstyle_get_text_style();
    te->g = 0;
    te->h = dimstyle_get_text_height();
    te->gravity = GRAVITY_M | GRAVITY_C;

    a = dimstyle_get_text_offset();
    te->x = x3;
    te->y = y3 + a;

    sprintf(buf, "%.*f", shell->float_prec, fabs(x2 - x1));
    while (buf[strlen(buf) - 1] == '0')
        buf[strlen(buf) - 1] = 0;
    sprintf(radix_char, "%.*f", 1, 1.0);
    if (buf[strlen(buf) - 1] == radix_char[1])
        buf[strlen(buf) - 1] = 0;

    te->text = DUP(buf);

    layer->objects = eina_list_append(layer->objects, te);
    append_undo_new_object(te, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);

    te->owner = (Object *) drawing->current_layer;
    _text_ssync(te);
}

void
dim_v_create(void)
{
    float               x1, y1, x2, y2, x3, y3;
    char               *s;
    int                 res;

    serv_set_hint(DUP(_("enter first point: ")));

    serv_set_state(ST_DIM1);

    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x1, &y1);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_DIM2);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    pre_line_x1y1(x1, y1);
    shell->context.fx = x1;
    shell->context.fy = y1;

    serv_set_hint(DUP(_("enter second point: ")));
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

    serv_set_state(ST_DIMV);

    pre_set_x1y1x2y2(x1, y1, x2, y2);

    serv_set_hint(DUP(_("define view of the dimension: ")));

    do
      {
          s = serv_get_string();
          res = get_values(s, x2, y2, &x3, &y3);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;

    lock_data();
    _dim_v_create(x1, y1, x2, y2, x3, y3);
    unlock_data();
}

void
dim_a_create(void)
{
    float               x1, y1, x2, y2, x3, y3;
    char               *s;
    int                 res;

    serv_set_hint(DUP(_("enter first point: ")));

    serv_set_state(ST_DIM1);

    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x1, &y1);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_DIM2);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    pre_line_x1y1(x1, y1);
    shell->context.fx = x1;
    shell->context.fy = y1;
    serv_set_hint(DUP(_("enter second point: ")));
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

    serv_set_state(ST_DIMA);

    pre_set_x1y1x2y2(x1, y1, x2, y2);

    serv_set_hint(DUP(_("define view of the dimension: ")));

    do
      {
          s = serv_get_string();
          res = get_values(s, x2, y2, &x3, &y3);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;

    lock_data();
    _dim_a_create(x1, y1, x2, y2, x3, y3);
    unlock_data();

}

void
ghost_dim_create(void)
{
    Evas               *e;

    e = shell->evas;

    if (!ol1)
      {
          ol1 = evas_object_line_add(e);
          evas_object_color_set( ol1, ALPHA2, ALPHA2/5, ALPHA2/5, ALPHA2);
          evas_object_layer_set(ol1, 17);
          evas_object_pass_events_set(ol1, 1);
      }
    if (!ol2)
      {
          ol2 = evas_object_line_add(e);
          evas_object_color_set( ol2, ALPHA2, ALPHA2/5, ALPHA2/5, ALPHA2);
          evas_object_layer_set(ol2, 17);
          evas_object_pass_events_set(ol2, 1);
      }
    if (!ol3)
      {
          ol3 = evas_object_line_add(e);
          evas_object_color_set(ol3, ALPHA2, ALPHA2/5, ALPHA2/5, ALPHA2);
          evas_object_layer_set(ol3, 17);
          evas_object_pass_events_set(ol3, 1);
      }
}

void
ghost_dim_h_redraw(Eina_List *data, double x, double y)
{
    Evas               *e;
    XY                 *xy;
    double              x1, x2, x3, y1, y2, y3;
    int                 sign1 = 1, sign2 = 1;

    if (!data)
        return;
    if (!data->next)
        return;
    e = shell->evas;
    xy = (XY *) data->data;
    x1 = xy->x;
    y1 = xy->y;
    xy = (XY *) data->next->data;
    x2 = xy->x;
    y2 = xy->y;
    if (y1 > y)
        sign1 = -1;
    if (y2 > y)
        sign2 = -1;

    evas_object_line_xy_set(ol1, w2s_x(x1 + 0.1), w2s_y(y1),
                     w2s_x(x1), w2s_y(y + sign1 * 3));
    evas_object_line_xy_set(ol2, w2s_x(x2 + 0.1), w2s_y(y2),
                     w2s_x(x2), w2s_y(y + sign2 * 3));
    evas_object_line_xy_set(ol3, w2s_x(x1 + 0.1), w2s_y(y),
                     w2s_x(x2 + 0.1), w2s_y(y));

    evas_object_show(ol1);
    evas_object_show(ol2);
    evas_object_show(ol3);
}
void
ghost_dim_destroy(void)
{
    Evas               *e;

    e = shell->evas;
    evas_object_hide(ol1);
    evas_object_hide(ol2);
    evas_object_hide(ol3);
}

void
ghost_dim_v_redraw(Eina_List *data, double x, double y)
{
    Evas               *e;
    XY                 *xy;
    double              x1, x2, x3, y1, y2, y3;
    int                 sign1 = 1, sign2 = 1;

    if (!data)
        return;
    if (!data->next)
        return;
    e = shell->evas;
    xy = (XY *) data->data;
    x1 = xy->x;
    y1 = xy->y;
    xy = (XY *) data->next->data;
    x2 = xy->x;
    y2 = xy->y;
    if (x1 > x)
        sign1 = -1;
    if (x2 > x)
        sign2 = -1;

    evas_object_line_xy_set(ol1, w2s_x(x1 + 0.1), w2s_y(y1),
                     w2s_x(x + sign1 * 3), w2s_y(y1));
    evas_object_line_xy_set(ol2, w2s_x(x2 + 0.1), w2s_y(y2),
                     w2s_x(x + sign1 * 3), w2s_y(y2));
    evas_object_line_xy_set(ol3, w2s_x(x), w2s_y(y1), w2s_x(x + 0.1), w2s_y(y2));

    evas_object_show(ol1);
    evas_object_show(ol2);
    evas_object_show(ol3);
}

void
ghost_dim_a_redraw(Eina_List *data, double x, double y)
{
    Evas               *e;
    XY                 *xy;
    double              x1, x2, x3, y1, y2, y3;
    double              a, b, g, a2, b2, r, x0, y0;
    int                 sign = 1;

    if (!data)
        return;
    if (!data->next)
        return;
    e = shell->evas;
    xy = (XY *) data->data;
    x1 = xy->x;
    y1 = xy->y;
    xy = (XY *) data->next->data;
    x2 = xy->x;
    y2 = xy->y;

    if (x1 == x2)
      {
          ghost_dim_v_redraw(data, x, y);
          return;
      }
    if (y1 == y2)
      {
          ghost_dim_h_redraw(data, x, y);
          return;
      }

    a = (y2 - y1) / (x2 - x1);
    b = y2 - a * x2;

    g = atan(a);

    a2 = tan(g + M_PI / 2);
    b2 = y - a2 * x;
    x0 = -(b2 - b) / (a2 - a);
    y0 = a * x0 + b;

    r = hypot(x - x0, y - y0);

    if (a * x + b < y)
        sign = -1;

    evas_object_line_xy_set(ol1, w2s_x(x1),
                     w2s_y(y1),
                     w2s_x(cos(g - sign * M_PI / 2) * (r + 3) + x1),
                     w2s_y(sin(g - sign * M_PI / 2) * (r + 3) + y1));
    evas_object_line_xy_set(ol2, w2s_x(x2),
                     w2s_y(y2),
                     w2s_x(cos(g - sign * M_PI / 2) * (r + 3) + x2),
                     w2s_y(sin(g - sign * M_PI / 2) * (r + 3) + y2));
    evas_object_line_xy_set(ol3, w2s_x(cos(g - sign * M_PI / 2) * r + x1),
                     w2s_y(sin(g - sign * M_PI / 2) * r + y1),
                     w2s_x(cos(g - sign * M_PI / 2) * r + x2),
                     w2s_y(sin(g - sign * M_PI / 2) * r + y2));

    evas_object_show(ol1);
    evas_object_show(ol2);
    evas_object_show(ol3);
}

void
_dim_v_create(double x1, double y1, double x2, double y2, double x3, double y3)
{
    Text               *te;
    Layer              *layer;
    int                 sign1, sign2;
    double              a, b, pr, sc, th;
    char                buf[4096];
    char                radix_char[10];
    char               *s;

    layer = drawing->current_layer;
    if (!layer)
        return;

    dimstyle_set(shell->context.dim_style);

    sign1 = (x1 > x3) ? -1 : 1;
    sign2 = (x2 > x3) ? -1 : 1;

    a = dimstyle_get_ext_line_offset() * sign1;
    b = dimstyle_get_ext_line_extend() * sign1;
    th = dimstyle_get_ext_line_thickness();
    sc = dimstyle_get_ext_line_scale();
    s = dimstyle_get_ext_line_style();

    _dim_create_line(x1 + a, y1, x3 + b, y1, s, sc, th);

    a = dimstyle_get_ext_line_offset() * sign2;
    b = dimstyle_get_ext_line_extend() * sign2;

    _dim_create_line(x2 + a, y2, x3 + b, y2, s, sc, th);

    pr = dimstyle_get_dim_line_prespacing();
    s = dimstyle_get_dim_line_style();
    sc = dimstyle_get_dim_line_scale();
    th = dimstyle_get_dim_line_thickness();

    a = dimstyle_get_arrow_size();

    _dim_create_line(x3, y1 + pr, x3, y2 + pr, s, sc, th);
    s = dimstyle_get_arrow_style();

    _dim_create_line(x3, y1, x3, y1 + a, s, 1, th);

    _dim_create_line(x3, y2, x3, y2 - a, s, 1, th);

    te = _text_create_copy(NULL);
    te->color = shell->context.color;
    te->textstyle = dimstyle_get_text_style();
    te->g = 90;
    te->h = dimstyle_get_text_height();
    te->gravity = GRAVITY_M | GRAVITY_C;

    a = dimstyle_get_text_offset();
    te->x = x3 - a;
    te->y = y3;

    sprintf(buf, "%.*f", shell->float_prec, fabs(y2 - y1));
    while (buf[strlen(buf) - 1] == '0')
        buf[strlen(buf) - 1] = 0;
    sprintf(radix_char, "%.*f", 1, 1.0);
    if (buf[strlen(buf) - 1] == radix_char[1])
        buf[strlen(buf) - 1] = 0;

    te->text = DUP(buf);

    layer->objects = eina_list_append(layer->objects, te);
    append_undo_new_object(te, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);

    te->owner = (Object *) drawing->current_layer;
    _text_ssync(te);
}

void
_dim_a_create(double x1, double y1, double x2, double y2, double x3, double y3)
{

    Text               *te;
    Layer              *layer;
    int                 sign = 1;
    double              a, b;
    double              ext, off, sc, th, pr;
    double              x0, y0, rad, gamma, a2, b2;
    char                buf[4096];
    char                radix_char[10];
    char               *s;

    if (x1 == x2)
      {
          _dim_v_create(x1, y1, x2, y2, x3, y3);
          return;
      }
    if (y1 == y2)
      {
          _dim_h_create(x1, y1, x2, y2, x3, y3);
          return;
      }

    layer = drawing->current_layer;
    if (!layer)
        return;

    dimstyle_set(shell->context.dim_style);

    {
        a = (y2 - y1) / (x2 - x1);
        b = y1 - a * x1;

        gamma = atan(a);
        a2 = tan(gamma + M_PI / 2);
        b2 = y3 - a2 * x3;

        x0 = -(b2 - b) / (a2 - a);
        y0 = a2 * x0 + b2;

        rad = hypot(x0 - x3, y0 - y3);
        if (x3 * a + b > y3)
            sign = -1;

    }
    off = dimstyle_get_ext_line_offset();
    ext = dimstyle_get_ext_line_extend();

    sc = dimstyle_get_ext_line_scale();
    th = dimstyle_get_ext_line_thickness();
    s = dimstyle_get_ext_line_style();
    pr = dimstyle_get_dim_line_prespacing();

    _dim_create_line(x1 + (off) *
                     cos(gamma + sign * M_PI / 2),
                     y1 + (off)
                     * sin(gamma + sign * M_PI / 2),
                     x1 + (rad + ext) *
                     cos(gamma + sign * M_PI / 2),
                     y1 + (rad + ext) *
                     sin(gamma + sign * M_PI / 2), s, sc, th);

    _dim_create_line(x2 + off *
                     cos(gamma + sign * M_PI / 2),
                     y2 + off *
                     sin(gamma + sign * M_PI / 2),
                     x2 + (rad + ext) *
                     cos(gamma + sign * M_PI / 2),
                     y2 + (rad + ext) *
                     sin(gamma + sign * M_PI / 2), s, sc, th);

    _dim_create_line(x1 + rad * cos(gamma + sign * M_PI / 2) +
                     cos(gamma) * pr,
                     y1 + rad * sin(gamma + sign * M_PI / 2) +
                     sin(gamma) * pr,
                     x2 + rad * cos(gamma + sign * M_PI / 2) -
                     cos(gamma) * pr,
                     y2 + rad * sin(gamma + sign * M_PI / 2) -
                     sin(gamma) * pr, s, sc, th);

    ext = dimstyle_get_arrow_size();
    s = dimstyle_get_arrow_style();
    th = dimstyle_get_dim_line_thickness();

    _dim_create_line(x1 + rad * cos(gamma + sign * M_PI / 2),
                     y1 + rad * sin(gamma + sign * M_PI / 2),
                     x1 + rad * cos(gamma + sign * M_PI / 2) + cos(gamma) *
                     dimstyle_get_arrow_size(),
                     y1 + rad * sin(gamma + sign * M_PI / 2) +
                     sin(gamma) * ext, s, 1, th);

    _dim_create_line(x2 + rad * cos(gamma + sign * M_PI / 2),
                     y2 + rad * sin(gamma + sign * M_PI / 2),
                     x2 + rad * cos(gamma + sign * M_PI / 2) + cos(gamma +
                                                                   M_PI) *
                     dimstyle_get_arrow_size(),
                     y2 + rad * sin(gamma + sign * M_PI / 2) + sin(gamma +
                                                                   M_PI) *
                     ext, s, 1, th);

    te = _text_create_copy(NULL);
    te->color = shell->context.color;
    te->textstyle = dimstyle_get_text_style();
    te->g = gamma / M_PI * 180;
    te->h = dimstyle_get_text_height();
    te->gravity = GRAVITY_M | GRAVITY_C;

    a = dimstyle_get_text_offset();
    te->x = x3 + a * cos(gamma + sign * M_PI / 2);
    te->y = y3 + a * sin(gamma + sign * M_PI / 2);

    sprintf(buf, "%.*f", shell->float_prec, hypot(x2 - x1, y2 - y1));
    while (buf[strlen(buf) - 1] == '0')
        buf[strlen(buf) - 1] = 0;
    sprintf(radix_char, "%.*f", 1, 1.0);
    if (buf[strlen(buf) - 1] == radix_char[1])
        buf[strlen(buf) - 1] = 0;

    te->text = DUP(buf);

    layer->objects = eina_list_append(layer->objects, te);
    append_undo_new_object(te, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);

    te->owner = (Object *) drawing->current_layer;
    _text_ssync(te);
}
