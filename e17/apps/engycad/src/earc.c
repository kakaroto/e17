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
void                _earc_create(double, double, double, double, double, double,
                                 double);
void                _earc_check_evas_objects(EArc *);
void                _earc_refresh_evas_objects(EArc *);
void                _create_tiled_earc(EArc *);
void                _create_scaled_earc(EArc *);
void                _earc_ssync(EArc *);

void
earc_create(void)
{
    char               *s;
    float               x1 = 0, y1 = 0, x2 = 0, y2 = 0, rx = 0, ry = 0, x3 =
        0, y3 = 0, g = 0, a, b;
    int                 res;

    serv_set_hint(DUP(_("enter center point of the ellipse: ")));

    serv_set_state(ST_ELLIPSE1);

    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x1, &y1);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_ELLIPSE2);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    pre_ell_x1y1(x1, y1);
    serv_set_hint(DUP(_("enter first axis: ")));

    do
      {
          s = serv_get_string();
          res = get_values(s, x1, y1, &x2, &y2);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    rx = hypot(x2 - x1, y2 - y1);
    g = atan((y2 - y1) / (x2 - x1)) / M_PI * 180;
    pre_ell_rxg(x1, y1, rx, g);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    serv_set_state(ST_ELLIPSE3);

    serv_set_hint(DUP(_("enter point on circumference or axis: ")));
    s = serv_get_string();
    res = get_values(s, x1, y1, &x3, &y3);
    if (res == 1)
      {
          ry = x3;
      }
    else
      {
          ry = hypot(x3 - x1, y3 - y1);
      }

    serv_set_state(ST_LINE2);
    pre_set_x1y1(x1, y1);

    serv_set_hint(DUP(_("enter start angle: ")));
    s = serv_get_string();
    res = get_values(s, x1, y1, &x2, &y2);
    if (res == 1)
      {
          a = x2;
      }
    else
      {
          a = (x2 - x1) ? 180 * atan((y2 - y1) / (x2 - x1)) / M_PI : 90;
          if (x2 - x1 < 0)
              a += 180;
      }

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    serv_set_hint(DUP(_("enter end angle: ")));
    s = serv_get_string();
    res = get_values(s, x1, y1, &x2, &y2);
    if (res == 1)
      {
          b = x2;
      }
    else
      {
          b = (x2 - x1) ? 180 * atan((y2 - y1) / (x2 - x1)) / M_PI : 90;
          if (x2 - x1 < 0)
              b += 180;
      }

    a -= g;
    b -= g;

    if (a > b)
        b += 360;

    serv_set_state(ST_NORMAL);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    lock_data();
    _earc_create(x1, y1, rx, ry, g, a, b);
    unlock_data();
}

EArc               *
_earc_create_copy(EArc * src)
{
    EArc               *earc;

    earc = (EArc *) malloc(sizeof(EArc));
    ENGY_ASSERT(earc);
    memset(earc, 0, sizeof(EArc));

    earc->type = OBJ_EARC;
    earc->id = rand() + my_int++;
    earc->flags = FLAG_VISIBLE;

    earc->old.color.red = -1;
    earc->old.color.green = -1;
    earc->old.color.blue = -1;
    earc->old.color.alpha = -1;

    if (!src)
        return earc;

    earc->flags = src->flags;
    earc->color = src->color;
    earc->line_style = DUP(src->line_style);
    earc->thickness = src->thickness;
    earc->line_scale = src->line_scale;
    earc->x = src->x;
    earc->y = src->y;
    earc->rx = src->rx;
    earc->ry = src->ry;
    earc->g = src->g;
    earc->a = src->a;
    earc->b = src->b;

    return earc;
}

void
_earc_create(double x, double y,
             double rx, double ry, double g, double a, double b)
{
    EArc               *earc;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    earc = _earc_create_copy(NULL);
    earc->color = shell->context.color;
    earc->line_style = DUP(shell->context.line_style);
    earc->thickness = shell->context.thickness;
    earc->line_scale = shell->context.line_scale;
    earc->x = x;
    earc->y = y;
    earc->rx = rx;
    earc->ry = ry;
    earc->g = g;
    earc->a = a;
    earc->b = b;

    layer->objects = evas_list_append(layer->objects, earc);

    append_undo_new_object(earc, CMD_SYNC, OBJ_EARC, earc);
    msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);

    earc->owner = (Object *) drawing->current_layer;
    _earc_ssync(earc);

}

void
earc_redraw(EArc * earc)
{
    if (!earc)
        return;
    if (shell->psout);          /* TODO */
    magnet_attach(earc);
    _earc_check_evas_objects(earc);
}

void
earc_sync(EArc * earc)
{
    char                buf[4096];
    char               *f;

    f = drawing->fileout;

    if (earc->color.red != earc->old.color.red)
      {
          sprintf(buf, "/entity/%d/red", earc->id);
          E_DB_INT_SET(f, buf, earc->color.red);
      }

    if (earc->color.green != earc->old.color.green)
      {
          sprintf(buf, "/entity/%d/green", earc->id);
          E_DB_INT_SET(f, buf, earc->color.green);
      }

    if (earc->color.blue != earc->old.color.blue)
      {
          sprintf(buf, "/entity/%d/blue", earc->id);
          E_DB_INT_SET(f, buf, earc->color.blue);
      }
    if (earc->color.alpha != earc->old.color.alpha)
      {
          sprintf(buf, "/entity/%d/alpha", earc->id);
          E_DB_INT_SET(f, buf, earc->color.alpha);
      }

    if (earc->flags != earc->old.flags)
      {
          sprintf(buf, "/entity/%d/flags", earc->id);
          E_DB_INT_SET(f, buf, earc->flags);
      }
    if (!earc->old.line_style || strcmp(earc->old.line_style, earc->line_style))
      {
          sprintf(buf, "/entity/%d/linestyle", earc->id);
          E_DB_STR_SET(f, buf, earc->line_style);
      }
    if (earc->line_scale != earc->old.line_scale)
      {
          sprintf(buf, "/entity/%d/linescale", earc->id);
          E_DB_FLOAT_SET(f, buf, (float)earc->line_scale);
      }
    if (earc->thickness != earc->old.thickness)
      {
          sprintf(buf, "/entity/%d/thickness", earc->id);
          E_DB_FLOAT_SET(f, buf, (float)earc->thickness);
      }
    if (earc->x != earc->old.x)
      {
          sprintf(buf, "/entity/%d/x", earc->id);
          E_DB_FLOAT_SET(f, buf, (float)earc->x);
      }
    if (earc->y != earc->old.y)
      {
          sprintf(buf, "/entity/%d/y", earc->id);
          E_DB_FLOAT_SET(f, buf, (float)earc->y);
      }
    if (earc->rx != earc->old.rx)
      {
          sprintf(buf, "/entity/%d/rx", earc->id);
          E_DB_FLOAT_SET(f, buf, (float)earc->rx);
      }
    if (earc->ry != earc->old.ry)
      {
          sprintf(buf, "/entity/%d/ry", earc->id);
          E_DB_FLOAT_SET(f, buf, (float)earc->ry);
      }
    if (earc->g != earc->old.g)
      {
          sprintf(buf, "/entity/%d/g", earc->id);
          E_DB_FLOAT_SET(f, buf, (float)earc->g);
      }
    if (earc->a != earc->old.a)
      {
          sprintf(buf, "/entity/%d/a", earc->id);
          E_DB_FLOAT_SET(f, buf, (float)earc->a);
      }
    if (earc->b != earc->old.b)
      {
          sprintf(buf, "/entity/%d/b", earc->id);
          E_DB_FLOAT_SET(f, buf, (float)earc->b);
      }

    earc_redraw(earc);
}

void
earc_destroy(EArc * earc)
{
    Evas_List          *l;

    for (l = drawing->layers; l; l = l->next)
      {
          Layer              *layer;

          layer = (Layer *) l->data;
          layer->objects = evas_list_remove(layer->objects, earc);
      }

    earc_free(earc);
}

void
earc_free(EArc * earc)
{
    Evas_List          *l;

    if (!earc)
        return;

    magnet_detach(earc);

    if (earc->line_style)
        FREE(earc->line_style);
    if (earc->old.line_style)
        FREE(earc->old.line_style);

    for (l = earc->list; l; l = l->next)
        evas_object_del(l->data);
    earc->list = evas_list_free(earc->list);

    FREE(earc);
}

void
earc_move(EArc * earc, double dx, double dy)
{
    if (!earc)
        return;

    append_undo_double((void *)(&(earc->x)),
                       earc->x, earc->x + dx, CMD_SYNC, OBJ_EARC, earc);
    append_undo_double((void *)(&(earc->y)), earc->y, earc->y + dy, 0, 0, NULL);

    earc->x += dx;
    earc->y += dy;

    msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);
}

void
earc_scale(EArc * earc, double x0, double y0, double sc)
{
    double              a, b, c, d;

    if (!earc)
        return;
    a = earc->x;
    b = earc->y;
    c = earc->rx;
    d = earc->ry;

    earc->x -= x0;
    earc->y -= y0;
    earc->x *= sc;
    earc->y *= sc;
    earc->x += x0;
    earc->y += y0;
    earc->rx *= sc;
    earc->ry *= sc;

    append_undo_double((void *)(&(earc->x)),
                       a, earc->x, CMD_SYNC, OBJ_EARC, earc);
    append_undo_double((void *)(&(earc->y)), b, earc->y, 0, 0, NULL);
    append_undo_double((void *)(&(earc->rx)), c, earc->rx, 0, 0, NULL);
    append_undo_double((void *)(&(earc->ry)), d, earc->ry, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);
}

void
earc_scale_xy(EArc * earc, double x0, double y0, double scx, double scy)
{
    double              a, b, c, d;

    if (!earc)
        return;
    a = earc->x;
    b = earc->y;
    c = earc->rx;
    d = earc->ry;

    earc->x -= x0;
    earc->y -= y0;
    earc->x *= scx;
    earc->y *= scy;
    earc->x += x0;
    earc->y += y0;
    earc->rx *= scx;
    earc->ry *= scy;

    append_undo_double((void *)(&(earc->x)),
                       a, earc->x, CMD_SYNC, OBJ_EARC, earc);
    append_undo_double((void *)(&(earc->y)), b, earc->y, 0, 0, NULL);
    append_undo_double((void *)(&(earc->rx)), c, earc->rx, 0, 0, NULL);
    append_undo_double((void *)(&(earc->ry)), d, earc->ry, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);
}

void
earc_rotate(EArc * earc, double x0, double y0, double angle)
{
    double              a, b, c, d, e;
    double              tx, ty;

    if (!earc)
        return;
    a = earc->x;
    b = earc->y;
    c = earc->a;
    d = earc->b;
    e = earc->g;

    angle = -angle;
    earc->x -= x0;
    earc->y -= y0;
    tx = earc->x * cos(angle) + earc->y * sin(angle);
    ty = earc->y * cos(angle) - earc->x * sin(angle);
    earc->x = tx + x0;
    earc->y = ty + y0;
    earc->g -= angle / M_PI * 180;

    append_undo_double((void *)(&(earc->x)),
                       a, earc->x, CMD_SYNC, OBJ_EARC, earc);
    append_undo_double((void *)(&(earc->y)), b, earc->y, 0, 0, NULL);
    append_undo_double((void *)(&(earc->a)), c, earc->a, 0, 0, NULL);
    append_undo_double((void *)(&(earc->b)), d, earc->b, 0, 0, NULL);
    append_undo_double((void *)(&(earc->g)), e, earc->g, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);
}

void
earc_clone(EArc * src, double dx, double dy)
{
    EArc               *earc;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    earc = _earc_create_copy(src);
    earc->flags = FLAG_VISIBLE;

    earc->x += dx;
    earc->y += dy;

    layer->objects = evas_list_append(layer->objects, earc);

    append_undo_new_object(earc, CMD_SYNC, OBJ_EARC, earc);
    msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);

    earc->owner = (Object *) drawing->current_layer;
    _earc_ssync(earc);
}

void
earc_array(EArc * earc, int numx, int numy, double dx, double dy)
{
    int                 i, j;

    if (!earc)
        return;
    if (numx < 1)
        return;
    if (numy < 1)
        return;
    for (i = 0; i < numx; i++)
        for (j = 0; j < numy; j++)
            if (i || j)
                earc_clone(earc, i * dx, j * dy);
}

void
earc_array_polar(EArc * earc, double x0, double y0, int num, double da)
{
    int                 i;

    if (!earc)
        return;
    if (num < 1)
        return;
    for (i = 1; i < num; i++)
      {
          earc_clone(earc, 0, 0);
          earc_rotate(evas_list_last(drawing->current_layer->objects)->data,
                      x0, y0, i * da);
      }
}

void
earc_mirror_ab(EArc * src, double a, double b)
{
    EArc               *earc;
    Drawing            *d;
    Layer              *layer;
    double              resx, resy, tx, ty, angle;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    earc = _earc_create_copy(src);
    earc->flags = FLAG_VISIBLE;

    tx = earc->x;
    ty = earc->y;
    ty -= b;
    angle = atan(a);
    resx = tx * cos(2 * angle) + ty * sin(2 * angle);
    resy = -ty * cos(2 * angle) + tx * sin(2 * angle);
    earc->x = resx;
    earc->y = resy + b;

    earc->b = 180 - src->a;
    earc->a = 180 - src->b;
    earc->g = 2 * angle / M_PI * 180 - earc->g;
    if ((earc->g > 90) && (earc->g < 270))
        earc->g -= 180;
    if ((earc->g < -90) && (earc->g > -270))
        earc->g += 180;

    layer->objects = evas_list_append(layer->objects, earc);

    append_undo_new_object(earc, CMD_SYNC, OBJ_EARC, earc);
    msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);

    earc->owner = (Object *) drawing->current_layer;
    _earc_ssync(earc);
}

void
earc_mirror_y(EArc * src, double y0)
{
    EArc               *earc;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    earc = _earc_create_copy(src);
    earc->flags = FLAG_VISIBLE;

    earc->x = src->x;
    earc->y = y0 * 2 - src->y;
    earc->a = 360 - src->b;
    earc->b = 360 - src->a;
    earc->g = -src->g;

    layer->objects = evas_list_append(layer->objects, earc);

    append_undo_new_object(earc, CMD_SYNC, OBJ_EARC, earc);
    msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);

    earc->owner = (Object *) drawing->current_layer;
    _earc_ssync(earc);
}

void
earc_mirror_x(EArc * src, double x0)
{
    EArc               *earc;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    earc = _earc_create_copy(src);
    earc->flags = FLAG_VISIBLE;

    earc->y = src->y;
    earc->x = x0 * 2 - src->x;
    earc->a = 180 - src->b;
    earc->b = 180 - src->a;
    earc->g = -src->g;

    layer->objects = evas_list_append(layer->objects, earc);

    append_undo_new_object(earc, CMD_SYNC, OBJ_EARC, earc);
    msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);

    earc->owner = (Object *) drawing->current_layer;
    _earc_ssync(earc);
}

void
earc_trim_ab(EArc * earc, double a, double b, int side)
{
    if (!earc)
        return;
    if ((earc->y > earc->x * a + b) ^ (side))
      {
          earc_delete(earc);
      }
}

void
earc_trim_y(EArc * earc, double y0, int side)
{
    if (!earc)
        return;
    if ((earc->y > y0) ^ (!side))
      {
          earc_delete(earc);
      }
}

void
earc_trim_x(EArc * earc, double x0, int side)
{
    if (!earc)
        return;
    if ((earc->x > x0) ^ (!side))
      {
          earc_delete(earc);
      }
}

void
earc_delete(EArc * earc)
{
    int                 dummy;

    dummy = earc->flags;
    earc->flags |= FLAG_DELETED;
    append_undo_long((void *)&(earc->flags),
                    dummy, earc->flags, CMD_SYNC, OBJ_EARC, earc);
    msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);
}

void
earc_cut(EArc * earc)
{
}

void
earc_copy(EArc * earc, int sock, double dx, double dy)
{
    CP_Header           hd;

    if (!earc)
        return;
    if (!earc->line_style)
        return;

    hd.type = OBJ_EARC;
    hd.length = sizeof(EArc);

    earc->x -= dx;
    earc->y -= dy;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, earc, hd.length);

    earc->x += dx;
    earc->y += dy;

    hd.type = OBJ_NONE;
    hd.length = strlen(earc->line_style) + 1;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, earc->line_style, hd.length);
}

void
earc_paste(CP_Header hd, int sock, double dx, double dy)
{
    EArc               *earc, *src;
    void               *data;
    int                 res;

    if (hd.type != OBJ_EARC)
        return;
    if (hd.length != sizeof(EArc))
        return;

    earc = _earc_create_copy(NULL);
    src = (EArc *) malloc(hd.length);
    res = read(sock, src, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "earc_paste: lack of bytes for EArc\n");
      }

    earc->flags = src->flags;
    earc->color = src->color;
    earc->x = src->x + dx;
    earc->y = src->y + dy;
    earc->rx = src->rx;
    earc->ry = src->ry;
    earc->a = src->a;
    earc->b = src->b;
    earc->thickness = src->thickness;
    earc->line_scale = src->line_scale;

    read(sock, &hd, sizeof(hd));
    data = malloc(hd.length);
    res = read(sock, data, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "earc_paste: lack of bytes for line_style\n");
      }
    earc->line_style = data;
    if(earc->line_style && (strlen(earc->line_style)>4000))
	                earc->line_style[4000]=0;
    FREE(src);

    drawing->current_layer->objects =
        evas_list_append(drawing->current_layer->objects, earc);

    append_undo_new_object(earc, CMD_SYNC, OBJ_EARC, earc);
    msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);

    earc->owner = (Object *) drawing->current_layer;
    earc_sync(earc);
    _earc_ssync(earc);
}

char              **
earc_get_info(EArc * earc)
{
    char              **p;
    char               *s;
    char                buf[4096];
    int                 pr;

    pr = shell->float_prec;

    p = (char **)malloc(sizeof(char *) * 10);
    memset(p, 0, sizeof(char *) * 10);

    s = _("Object type: elliptical arc");

    p[0] = DUP(s);

    sprintf(buf, _("X: %.*f Y: %.*f"), pr, earc->x, pr, earc->y);
    p[1] = DUP(buf);

    sprintf(buf, _("RX/RY: %.*f/%.*f"), pr, earc->rx, pr, earc->ry);
    p[3] = DUP(buf);

    sprintf(buf, _("Gamma: %.*f"), pr, earc->g);
    p[4] = DUP(buf);

    sprintf(buf, _("Alpha/Beta: %.*f/%.*f"), pr, earc->a, pr, earc->b);
    p[2] = DUP(buf);

    sprintf(buf, _("Line style: %s"), earc->line_style);
    p[5] = DUP(buf);

    sprintf(buf, _("Line style scale: %.*f"), pr, earc->line_scale);
    p[6] = DUP(buf);

    sprintf(buf, _("Line thickness: %.*f"), pr, earc->thickness);
    p[7] = DUP(buf);

    sprintf(buf, _("Owner: %s"), ((Layer *) earc->owner)->label);
    p[8] = DUP(buf);

    return p;
}

int
earc_select_by_click(EArc * earc, double x, double y)
{
    int                 dummy;
    double              epsilon, dist;
    double              tx, th, a1, b1;

    if (earc->flags & FLAG_SELECTED)
        return 0;

    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;

    x -= earc->x;
    y -= earc->y;

    th = 0;
    if((x>0)&&(y==0)) th = 0;
    if((x>0)&&(y>0)) th = atan(y/x)/M_PI*180;
    if((x==0)&&(y>0)) th = 90;
    if((x<0)&&(y>0)) th = atan(y/x)/M_PI*180 + 180;
    if((x<0)&&(y==0)) th = 180;
    if((x<0)&&(y<0)) th = atan(y/x)/M_PI*180 + 180;
    if((x==0)&&(y<0)) th = 270;
    if((x>0)&&(y<0)) th = atan(y/x)/M_PI*180 + 360;
    a1 = earc->a+earc->g;
    b1 = earc->b+earc->g;
    while(a1>360) { a1-=360; b1-=360;}
    while(a1<0) {a1+=360; b1+=360;}

    if((th>b1)||(th<a1)) return 0;

    tx = x;

    x = x * cos(earc->g / 180 * M_PI) + y * sin(earc->g / 180 * M_PI);
    y = y * cos(earc->g / 180 * M_PI) - tx * sin(earc->g / 180 * M_PI);

    y *= earc->rx / earc->ry;

    dist = x * x + y * y;

    if (fabs(dist - earc->rx * earc->rx) > epsilon * 16)
        return 0;

    dummy = earc->flags;
    earc->flags |= FLAG_SELECTED;
    append_undo_long((void *)&(earc->flags),
                    dummy, earc->flags, CMD_SYNC, OBJ_EARC, earc);
    msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);
    return 1;
}

void
earc_select_by_rect(EArc * earc, double x, double y, double w, double h)
{
    int                 dummy;

    if (earc->flags & FLAG_SELECTED)
        return;
    
    if (( earc->rect.x1 > x )   &&
		    ( earc->rect.x2 < x+w ) &&
		    ( earc->rect.y1 > y )   &&
		    ( earc->rect.y2 < y+h ))    	    
      {
          dummy = earc->flags;
          earc->flags |= FLAG_SELECTED;
          append_undo_long((void *)&(earc->flags),
                          dummy, earc->flags, CMD_SYNC, OBJ_EARC, earc);
          msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);
      }
}

int
earc_deselect_by_click(EArc * earc, double x, double y)
{
    int                 dummy;
    double              epsilon, dist;
    double              tx, th, a1, b1;

    if (!(earc->flags & FLAG_SELECTED))
        return 0;

    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;
    
    x -= earc->x;
    y -= earc->y;
        
    th = 0;
    if((x>0)&&(y==0)) th = 0;
    if((x>0)&&(y>0)) th = atan(y/x)/M_PI*180;
    if((x==0)&&(y>0)) th = 90;
    if((x<0)&&(y>0)) th = atan(y/x)/M_PI*180 + 180;
    if((x<0)&&(y==0)) th = 180;
    if((x<0)&&(y<0)) th = atan(y/x)/M_PI*180 + 180;
    if((x==0)&&(y<0)) th = 270;
    if((x>0)&&(y<0)) th = atan(y/x)/M_PI*180 + 360;
    a1 = earc->a+earc->g;
    b1 = earc->b+earc->g;
    while(a1>360) { a1-=360; b1-=360;}
    while(a1<0) {a1+=360; b1+=360;}

    if((th>b1)||(th<a1)) return 0;
    
    tx = x;

    x = x * cos(earc->g / 180 * M_PI) + y * sin(earc->g / 180 * M_PI);
    y = y * cos(earc->g / 180 * M_PI) - tx * sin(earc->g / 180 * M_PI);

    y *= earc->rx / earc->ry;

    dist = x * x + y * y;

    if (fabs(dist - earc->rx * earc->rx) > epsilon * 16)
        return 0;

    dummy = earc->flags;
    earc->flags ^= FLAG_SELECTED;
    append_undo_long((void *)&(earc->flags),
                    dummy, earc->flags, CMD_SYNC, OBJ_EARC, earc);
    msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);
    return 1;
}

void
earc_deselect_by_rect(EArc * earc, double x, double y, double w, double h)
{
    int                 dummy;

    if (!(earc->flags & FLAG_SELECTED))
        return;

    if (( earc->rect.x1 > x )   &&
		    ( earc->rect.x2 < x+w ) &&
		    ( earc->rect.y1 > y )   &&
		    ( earc->rect.y2 < y+h ))	   
      {
          dummy = earc->flags;
          earc->flags ^= FLAG_SELECTED;
          append_undo_long((void *)&(earc->flags),
                          dummy, earc->flags, CMD_SYNC, OBJ_EARC, earc);
          msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);
      }
}

void
_earc_check_evas_objects(EArc * earc)
{
    Evas_List          *l;
    int                 need_to_clear = 0;

    if (!earc->old.line_style || strcmp(earc->old.line_style, earc->line_style))
      {
          need_to_clear = 1;
          FREE(earc->old.line_style);
          earc->old.line_style = DUP(earc->line_style);
      }

    if (earc->thickness * shell->context.show_thickness
        != earc->old.thickness * shell->context.show_thickness)
      {
          need_to_clear = 1;
          earc->old.thickness = earc->thickness;
      }

    if (drawing->scale != drawing->old.scale)
      {
          need_to_clear = 1;
      }

    if (earc->flags != earc->old.flags)
      {
/*	need_to_clear = 1;*/
          earc->old.flags = earc->flags;
      }

    if ((earc->x != earc->old.x) ||
        (earc->y != earc->old.y) ||
        (earc->rx != earc->old.rx) ||
        (earc->ry != earc->old.ry) ||
        (earc->g != earc->old.g) ||
        (earc->a != earc->old.a) || (earc->b != earc->old.b))
      {
          need_to_clear = 1;
          earc->old.x = earc->x;
          earc->old.y = earc->y;
          earc->old.rx = earc->rx;
          earc->old.ry = earc->ry;
          earc->old.g = earc->g;
          earc->old.a = earc->a;
          earc->old.b = earc->b;
      }

    if (need_to_clear)
      {
          for (l = earc->list; l; l = l->next)
              evas_object_del(l->data);
          earc->list = evas_list_free(earc->list);
      }

    _earc_refresh_evas_objects(earc);

}

void
_earc_refresh_evas_objects(EArc * earc)
{
    linestyle_set(earc->line_style);
    if (linestyle_get_tiled())
      {
          _create_tiled_earc(earc);
      }
    else
      {
          _create_scaled_earc(earc);
      }
}

void
_create_scaled_earc(EArc * earc)
{
    Evas               *e;
    Evas_List          *list = NULL, *l, *lo;
    Evas_Object        *o;
    float               len, tlen, tscale;
    int                 i, flag;
    float               x, y;
    double              x1, y1, x2, y2, ekst, ash, bsh;
    XY                 *xy;
    int                 an;
    double              da, dr, angle, rad;

    earc->rect.x1 = earc->x;
    earc->rect.y1 = earc->y;
    earc->rect.x2 = earc->x;
    earc->rect.y2 = earc->y;
    
    tlen = linestyle_get_tilelength();
    ekst = earc->ry / earc->rx;

    ash = 180 / M_PI * atan(tan(earc->a / 180 * M_PI) / ekst);
    bsh = 180 / M_PI * atan(tan(earc->b / 180 * M_PI) / ekst);

    if ((earc->a > 90) && (earc->a < 270))
        ash += 180;
    if ((earc->b > 90) && (earc->b < 270))
        bsh += 180;

    if (ash > bsh)
        bsh += 360;

    len = (bsh - ash) / 180.0 * M_PI * earc->rx;
    tscale = len / tlen;

    flag = linestyle_get_odd();

    list = linestyle_get_points(len, earc->line_scale);

    trans_scale(list, len / earc->rx / tlen, len / tlen);
    trans_move(list, ash / 180 * M_PI, earc->rx);

    e = shell->evas;

    lo = earc->list;
    for (l = list->next; l; l = l->next)
      {
          XY                 *a, *b;

          a = (XY *) l->prev->data;
          b = (XY *) l->data;
          flag = !flag;

          if (flag)
            {
                if (b->x < a->x)
                  {
                      double              dummy;

                      dummy = a->x;
                      a->x = b->x;
                      b->x = dummy;
                      dummy = a->y;
                      a->y = b->y;
                      b->y = dummy;
                  }
                an = (int)((b->x - a->x) /
                           ((bsh - ash) / 180 * M_PI / shell->arcnum));
                da = (b->x - a->x) / (an + 1);
                dr = (b->y - a->y) / (an + 1);
                an++;
                for (i = 0; i < an; i++)
                  {
                      angle = a->x + i * da;
                      rad = a->y + i * dr;

                      if (!lo || !lo->data)
                        {
                            o = _line_item(e,
                                           earc->thickness *
                                           shell->context.
                                           show_thickness * drawing->scale);
                            evas_object_layer_set(o, 10);
                            evas_object_pass_events_set(o, 1);
                            earc->list = evas_list_append(earc->list, o);
                            lo = evas_list_last(earc->list);
                        }
                      o = lo->data;
                      lo = lo->next;
                      if (earc->flags & FLAG_VISIBLE)
                        {
                            evas_object_color_set( o,
				earc->color.red*earc->color.alpha/255,
				earc->color.green*earc->color.alpha/255,
				earc->color.blue*earc->color.alpha/255, 
				earc->color.alpha);
                        }
                      else
                        {
                            evas_object_color_set( o, 0, 0, 0, 0);
                        }

                      if (earc->flags & FLAG_SELECTED)
                          evas_object_color_set( o, 
					  ALPHA5,
					  ALPHA5/5,
					  ALPHA5/5,
					  ALPHA5);
                      if (earc->flags & FLAG_DELETED)
                          evas_object_color_set( o, 0, 0, 0, 0);

                      x = rad * cos(angle);
                      y = rad * sin(angle);
                      y *= ekst;
                      x1 = x * cos(-earc->g / 180 * M_PI) +
                          y * sin(-earc->g / 180 * M_PI);
                      y1 = y * cos(-earc->g / 180 * M_PI) -
                          x * sin(-earc->g / 180 * M_PI);
                      x1 += earc->x;
                      y1 += earc->y;
		      if(earc->rect.x1>x1) earc->rect.x1 = x1;
		      if(earc->rect.x2<x1) earc->rect.x2 = x1;
		      if(earc->rect.y1>y1) earc->rect.y1 = y1;
		      if(earc->rect.y2<y1) earc->rect.y2 = y1;										
                      x1 = w2s_x(x1);
                      y1 = w2s_y(y1);
		      
                      x = (rad + dr) * cos(angle + da);
                      y = (rad + dr) * sin(angle + da);
                      y *= ekst;
                      x2 = x * cos(-earc->g / 180 * M_PI) +
                          y * sin(-earc->g / 180 * M_PI);
                      y2 = y * cos(-earc->g / 180 * M_PI) -
                          x * sin(-earc->g / 180 * M_PI);
                      x2 += earc->x;
                      y2 += earc->y;
		      if(earc->rect.x1>x2) earc->rect.x1 = x2;
		      if(earc->rect.x2<x2) earc->rect.x2 = x2;
		      if(earc->rect.y1>y2) earc->rect.y1 = y2;
		      if(earc->rect.y2<y2) earc->rect.y2 = y2;
											
                      x2 = w2s_x(x2);
                      y2 = w2s_y(y2);

                      _line_item_xy(e, o, x1, y1, x2, y2,
                                    earc->thickness *
                                    shell->context.
                                    show_thickness * drawing->scale);
                      evas_object_show(o);
                  }
            }
      }
    for (l = list; l; l = l->next)
        FREE(l->data);
    list = evas_list_free(list);
}

void
_create_tiled_earc(EArc * earc)
{
    Evas_List          *list, *l, *lo;
    int                 i, flag;
    Evas               *e;
    Evas_Object        *o;
    Drawing            *d;
    float               len, tlen;
    int                 an;
    double              da, dr, angle, rad, ash, bsh;
    double              x1, y1, x2, y2, ekst, x, y;

    earc->rect.x1 = earc->x;
    earc->rect.y1 = earc->y;
    earc->rect.x2 = earc->x;
    earc->rect.y2 = earc->y;
    
    tlen = linestyle_get_tilelength();
    ekst = earc->ry / earc->rx;

    ash = 180 / M_PI * atan(tan(earc->a / 180 * M_PI) / ekst);
    bsh = 180 / M_PI * atan(tan(earc->b / 180 * M_PI) / ekst);

    if ((earc->a > 90) && (earc->a < 270))
        ash += 180;
    if ((earc->b > 90) && (earc->b < 270))
        bsh += 180;
    if (ash > bsh)
        bsh += 360;

    flag = linestyle_get_odd();
    len = (bsh - ash) / 180.0 * M_PI * earc->rx;

    list = linestyle_get_points(len, earc->line_scale);
    ENGY_ASSERT(list);

    trans_scale(list, 1 / earc->rx, 1);
    trans_move(list, ash / 180.0 * M_PI, earc->rx);

    e = shell->evas;

    lo = earc->list;
    for (l = list->next; l; l = l->next)
      {
          XY                 *a, *b;

          a = (XY *) l->prev->data;
          b = (XY *) l->data;
          flag = !flag;

          if (flag)
            {
                if (b->x < a->x)
                  {
                      double              dummy;

                      dummy = a->x;
                      a->x = b->x;
                      b->x = dummy;
                      dummy = a->y;
                      a->y = b->y;
                      b->y = dummy;
                  }
                an = (int)((b->x - a->x) /
                           ((bsh - ash) / 180.0 * M_PI / shell->arcnum));
                da = (b->x - a->x) / (an + 1);
                dr = (b->y - a->y) / (an + 1);
                an++;
                for (i = 0; i < an; i++)
                  {
                      angle = a->x + i * da;
                      rad = a->y + i * dr;

                      if (!lo || !lo->data)
                        {
                            o = _line_item(e,
                                           earc->thickness *
                                           shell->context.
                                           show_thickness * drawing->scale);
                            evas_object_layer_set(o, 10);
                            evas_object_pass_events_set(o, 1);
                            earc->list = evas_list_append(earc->list, o);
                            lo = evas_list_last(earc->list);
                        }
                      o = lo->data;
                      lo = lo->next;
                      if (earc->flags & FLAG_VISIBLE)
                        {
                            evas_object_color_set( o,
				earc->color.red*earc->color.alpha/255,
				earc->color.green*earc->color.alpha/255,
				earc->color.blue*earc->color.alpha/255,
				earc->color.alpha);
                        }
                      else
                        {
                            evas_object_color_set( o, 0, 0, 0, 0);
                        }

                      if (earc->flags & FLAG_SELECTED)
                          evas_object_color_set( o, 
					  ALPHA5, 
					  ALPHA5/5, 
					  ALPHA5/5, 
					  ALPHA5);
                      if (earc->flags & FLAG_DELETED)
                          evas_object_color_set( o, 0, 0, 0, 0);
                      x = rad * cos(angle);
                      y = rad * sin(angle);
                      y *= ekst;
                      x1 = x * cos(-earc->g / 180 * M_PI) +
                          y * sin(-earc->g / 180 * M_PI);
                      y1 = y * cos(-earc->g / 180 * M_PI) -
                          x * sin(-earc->g / 180 * M_PI);
                      x1 += earc->x;
                      y1 += earc->y;
		      if(earc->rect.x1>x1) earc->rect.x1 = x1;
		      if(earc->rect.x2<x1) earc->rect.x2 = x1;
		      if(earc->rect.y1>y1) earc->rect.y1 = y1;
		      if(earc->rect.y2<y1) earc->rect.y2 = y1;
											
                      x1 = w2s_x(x1);
                      y1 = w2s_y(y1);
                      x = (rad + dr) * cos(angle + da);
                      y = (rad + dr) * sin(angle + da);
                      y *= ekst;
                      x2 = x * cos(-earc->g / 180 * M_PI) +
                          y * sin(-earc->g / 180 * M_PI);
                      y2 = y * cos(-earc->g / 180 * M_PI) -
                          x * sin(-earc->g / 180 * M_PI);
                      x2 += earc->x;
                      y2 += earc->y;
		      if(earc->rect.x1>x2) earc->rect.x1 = x2;
		      if(earc->rect.x2<x2) earc->rect.x2 = x2;
		      if(earc->rect.y1>y2) earc->rect.y1 = y2;
		      if(earc->rect.y2<y2) earc->rect.y2 = y2;
											
                      x2 = w2s_x(x2);
                      y2 = w2s_y(y2);

                      _line_item_xy(e, o, x1, y1, x2, y2,
                                    earc->thickness *
                                    shell->context.
                                    show_thickness * drawing->scale);
                      evas_object_show(o);
                  }
            }
      }
    for (l = list; l; l = l->next)
        FREE(l->data);
    list = evas_list_free(list);
}

void
_earc_ssync(EArc * earc)
{
    char                buf[4096];
    Drawing            *d;
    Object             *o;

    d = drawing;
    o = earc->owner;

    sprintf(buf, "/layer/%d/entity/%d", o->id, earc->id);
    E_DB_INT_SET(d->fileout, buf, earc->type);
}

void
earc_load(int id)
{
    char               *f;
    char                buf[4096];
    EArc               *earc;
    int                 res;
    float               x;

    f = drawing->filein;

    earc = _earc_create_copy(NULL);
    earc->id = id;

    sprintf(buf, "/entity/%d/flags", id);
    E_DB_INT_GET(f, buf, earc->flags, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/red", id);
    E_DB_INT_GET(f, buf, earc->color.red, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/green", id);
    E_DB_INT_GET(f, buf, earc->color.green, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/blue", id);
    E_DB_INT_GET(f, buf, earc->color.blue, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/alpha", id);
    E_DB_INT_GET(f, buf, earc->color.alpha, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/x", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    earc->x = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/y", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    earc->y = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/rx", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    earc->rx = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/ry", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    earc->ry = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/a", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    earc->a = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/b", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    earc->b = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/g", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    earc->g = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/linescale", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    earc->line_scale = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/thickness", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    earc->thickness = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/linestyle", id);
    E_DB_STR_GET(f, buf, earc->line_style, res);
    ENGY_ASSERT(res);
    if(earc->line_style && (strlen(earc->line_style)>4000))
	                earc->line_style[4000]=0;

    drawing->current_layer->objects =
        evas_list_append(drawing->current_layer->objects, earc);

    append_undo_new_object(earc, CMD_SYNC, OBJ_EARC, earc);
    msg_create_and_send(CMD_SYNC, OBJ_EARC, earc);

    earc->owner = (Object *) drawing->current_layer;
}
