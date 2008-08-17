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

#include <math.h>
#include "engy.h"

/* vars */
Evas_Object         *o1, *o2;

/* protos */
void                _ell_create(double, double, double, double, double);
void                _ell_check_evas_objects(Ellipse *);
void                _ell_refresh_evas_objects(Ellipse *);
void                _create_tiled_ell(Ellipse *);
void                _create_scaled_ell(Ellipse *);
void                pre_ell_x1y1(double, double);
void                pre_ell_rxg(double, double, double, double);
void                _ell_ssync(Ellipse *);

void
ellipse_create(void)
{
    char               *s;
    float               x1 = 0, y1 = 0, x2 = 0, y2 = 0, rx = 0, ry = 0, x3 =
        0, y3 = 0, g = 0;
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

    serv_set_state(ST_NORMAL);

    lock_data();
    _ell_create(x1, y1, rx, ry, g);
    shell->context.fx = x1;
    shell->context.fy = y1;
    unlock_data();
}

void
_ell_create(double x, double y, double rx, double ry, double g)
{
    Ellipse            *ell;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    ell = (Ellipse *) malloc(sizeof(Ellipse));
    ENGY_ASSERT(ell);
    memset(ell, 0, sizeof(Ellipse));

    ell->type = OBJ_ELLIPSE;
    ell->flags = FLAG_VISIBLE;

    ell = _ell_create_copy(NULL);
    ell->color = shell->context.color;
    ell->line_style = DUP(shell->context.line_style);
    ell->thickness = shell->context.thickness;
    ell->line_scale = shell->context.line_scale;
    ell->x = x;
    ell->y = y;
    ell->rx = rx;
    ell->ry = ry;
    ell->g = g;

    layer->objects = evas_list_append(layer->objects, ell);

    append_undo_new_object(ell, CMD_SYNC, OBJ_ELLIPSE, ell);
    msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);

    ell->owner = (Object *) drawing->current_layer;
    _ell_ssync(ell);

}

Ellipse            *
_ell_create_copy(Ellipse * src)
{
    Ellipse            *ell;

    ell = (Ellipse *) malloc(sizeof(Ellipse));
    ENGY_ASSERT(ell);
    memset(ell, 0, sizeof(Ellipse));

    ell->type = OBJ_ELLIPSE;
    ell->id = rand() + my_int++;
    ell->flags = FLAG_VISIBLE;

    ell->old.color.red = -1;
    ell->old.color.green = -1;
    ell->old.color.blue = -1;
    ell->old.color.alpha = -1;

    if (!src)
        return ell;

    ell->flags = src->flags;
    ell->color = src->color;
    ell->line_style = DUP(src->line_style);
    ell->line_scale = src->line_scale;
    ell->thickness = src->thickness;
    ell->x = src->x;
    ell->y = src->y;
    ell->rx = src->rx;
    ell->ry = src->ry;
    ell->g = src->g;

    return ell;
}

void
ell_redraw(Ellipse * ell)
{
    if (!ell)
        return;
    if (shell->psout);          /* TODO */
    magnet_attach(ell);
    _ell_check_evas_objects(ell);
}

void
ell_sync(Ellipse * ell)
{
    char                buf[4096];
    char               *f;

    f = drawing->fileout;

    if (ell->color.red != ell->old.color.red)
      {
          sprintf(buf, "/entity/%d/red", ell->id);
          E_DB_INT_SET(f, buf, ell->color.red);
      }

    if (ell->color.green != ell->old.color.green)
      {
          sprintf(buf, "/entity/%d/green", ell->id);
          E_DB_INT_SET(f, buf, ell->color.green);
      }

    if (ell->color.blue != ell->old.color.blue)
      {
          sprintf(buf, "/entity/%d/blue", ell->id);
          E_DB_INT_SET(f, buf, ell->color.blue);
      }
    if (ell->color.alpha != ell->old.color.alpha)
      {
          sprintf(buf, "/entity/%d/alpha", ell->id);
          E_DB_INT_SET(f, buf, ell->color.alpha);
      }

    if (ell->flags != ell->old.flags)
      {
          sprintf(buf, "/entity/%d/flags", ell->id);
          E_DB_INT_SET(f, buf, ell->flags);
      }
    if (!ell->old.line_style || strcmp(ell->old.line_style, ell->line_style))
      {
          sprintf(buf, "/entity/%d/linestyle", ell->id);
          E_DB_STR_SET(f, buf, ell->line_style);
      }
    if (ell->line_scale != ell->old.line_scale)
      {
          sprintf(buf, "/entity/%d/linescale", ell->id);
          E_DB_FLOAT_SET(f, buf, (float)ell->line_scale);
      }
    if (ell->thickness != ell->old.thickness)
      {
          sprintf(buf, "/entity/%d/thickness", ell->id);
          E_DB_FLOAT_SET(f, buf, (float)ell->thickness);
      }
    if (ell->x != ell->old.x)
      {
          sprintf(buf, "/entity/%d/x", ell->id);
          E_DB_FLOAT_SET(f, buf, (float)ell->x);
      }
    if (ell->y != ell->old.y)
      {
          sprintf(buf, "/entity/%d/y", ell->id);
          E_DB_FLOAT_SET(f, buf, (float)ell->y);
      }
    if (ell->rx != ell->old.rx)
      {
          sprintf(buf, "/entity/%d/rx", ell->id);
          E_DB_FLOAT_SET(f, buf, (float)ell->rx);
      }
    if (ell->ry != ell->old.ry)
      {
          sprintf(buf, "/entity/%d/ry", ell->id);
          E_DB_FLOAT_SET(f, buf, (float)ell->ry);
      }
    if (ell->g != ell->old.g)
      {
          sprintf(buf, "/entity/%d/g", ell->id);
          E_DB_FLOAT_SET(f, buf, (float)ell->g);
      }
    ell_redraw(ell);
}

void
ell_destroy(Ellipse * ell)
{
    Evas               *e;
    Evas_List          *l;

    for (l = drawing->layers; l; l = l->next)
      {
          Layer              *layer;

          layer = (Layer *) l->data;
          layer->objects = evas_list_remove(layer->objects, ell);
      }

    ell_free(ell);
}

void
ell_free(Ellipse * ell)
{
    Evas_List          *l;

    if(!ell)
        return;

    magnet_detach(ell);

    if (ell->line_style)
        FREE(ell->line_style);
    if (ell->old.line_style)
        FREE(ell->old.line_style);

    for (l = ell->list; l; l = l->next)
        evas_object_del(l->data);
    ell->list = evas_list_free(ell->list);

    FREE(ell);
}

void
ell_move(Ellipse * ell, double dx, double dy)
{

    if (!ell)
        return;

    append_undo_double((void *)(&(ell->x)),
                       ell->x, ell->x + dx, CMD_SYNC, OBJ_ELLIPSE, ell);
    append_undo_double((void *)(&(ell->y)), ell->y, ell->y + dy, 0, 0, NULL);

    ell->x += dx;
    ell->y += dy;

    msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);
}

void
ell_scale(Ellipse * ell, double x0, double y0, double sc)
{
    double              a, b, c, d;

    if (!ell)
        return;
    a = ell->x;
    b = ell->y;
    c = ell->rx;
    d = ell->ry;

    ell->x -= x0;
    ell->y -= y0;
    ell->x *= sc;
    ell->y *= sc;
    ell->x += x0;
    ell->y += y0;
    ell->rx *= sc;
    ell->ry *= sc;

    append_undo_double((void *)(&(ell->x)),
                       a, ell->x, CMD_SYNC, OBJ_ELLIPSE, ell);
    append_undo_double((void *)(&(ell->y)), b, ell->y, 0, 0, NULL);
    append_undo_double((void *)(&(ell->rx)), c, ell->rx, 0, 0, NULL);
    append_undo_double((void *)(&(ell->ry)), d, ell->ry, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);
}

void
ell_scale_xy(Ellipse * ell, double x0, double y0, double scx, double scy)
{
    double              a, b, c, d;

    if (!ell)
        return;
    a = ell->x;
    b = ell->y;
    c = ell->rx;
    d = ell->ry;

    ell->x -= x0;
    ell->y -= y0;
    ell->x *= scx;
    ell->y *= scy;
    ell->x += x0;
    ell->y += y0;
    ell->rx *= scx;
    ell->ry *= scy;

    append_undo_double((void *)(&(ell->x)),
                       a, ell->x, CMD_SYNC, OBJ_ELLIPSE, ell);
    append_undo_double((void *)(&(ell->y)), b, ell->y, 0, 0, NULL);
    append_undo_double((void *)(&(ell->rx)), c, ell->rx, 0, 0, NULL);
    append_undo_double((void *)(&(ell->ry)), d, ell->ry, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);

}

void
ell_rotate(Ellipse * ell, double x0, double y0, double angle)
{
    double              a, b, c;
    double              tx, ty;

    if (!ell)
        return;
    a = ell->x;
    b = ell->y;
    c = ell->g;

    angle = -angle;
    ell->x -= x0;
    ell->y -= y0;
    tx = ell->x * cos(angle) + ell->y * sin(angle);
    ty = ell->y * cos(angle) - ell->x * sin(angle);
    ell->x = tx + x0;
    ell->y = ty + y0;
    ell->g -= angle * 180 / M_PI;

    append_undo_double((void *)(&(ell->x)),
                       a, ell->x, CMD_SYNC, OBJ_ELLIPSE, ell);
    append_undo_double((void *)(&(ell->y)), b, ell->y, 0, 0, NULL);
    append_undo_double((void *)(&(ell->g)), c, ell->g, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);
}

void
ell_clone(Ellipse * src, double dx, double dy)
{
    Ellipse            *ell;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    ell = _ell_create_copy(src);
    ell->flags = FLAG_VISIBLE;

    ell->x += dx;
    ell->y += dy;

    layer->objects = evas_list_append(layer->objects, ell);

    append_undo_new_object(ell, CMD_SYNC, OBJ_ELLIPSE, ell);
    msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);

    ell->owner = (Object *) drawing->current_layer;
    _ell_ssync(ell);
}

void
ell_array(Ellipse * ell, int numx, int numy, double dx, double dy)
{
    int                 i, j;

    if (!ell)
        return;
    if (numx < 1)
        return;
    if (numy < 1)
        return;
    for (i = 0; i < numx; i++)
        for (j = 0; j < numy; j++)
            if (i || j)
                ell_clone(ell, i * dx, j * dy);
}

void
ell_array_polar(Ellipse * ell, double x0, double y0, int num, double da)
{
    int                 i;

    if (!ell)
        return;
    if (num < 1)
        return;
    for (i = 1; i < num; i++)
      {
          ell_clone(ell, 0, 0);
          ell_rotate(evas_list_last(drawing->current_layer->objects)->data,
                     x0, y0, i * da);
      }
}

void
ell_mirror_ab(Ellipse * src, double a, double b)
{
    Ellipse            *ell;
    Drawing            *d;
    Layer              *layer;
    double              resx, resy, tx, ty, angle;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    ell = _ell_create_copy(src);
    ell->flags = FLAG_VISIBLE;

    tx = ell->x;
    ty = ell->y;
    ty -= b;
    angle = atan(a);
    resx = tx * cos(2 * angle) + ty * sin(2 * angle);
    resy = -ty * cos(2 * angle) + tx * sin(2 * angle);
    ell->x = resx;
    ell->y = resy + b;
    ell->g = 2 * angle * 180 / M_PI - ell->g;

    layer->objects = evas_list_append(layer->objects, ell);

    append_undo_new_object(ell, CMD_SYNC, OBJ_ELLIPSE, ell);
    msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);

    ell->owner = (Object *) drawing->current_layer;
    _ell_ssync(ell);
}

void
ell_mirror_y(Ellipse * src, double y0)
{
    Ellipse            *ell;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    ell = _ell_create_copy(src);
    ell->flags = FLAG_VISIBLE;

    ell->x = src->x;
    ell->y = y0 * 2 - src->y;
    ell->g = 180 - src->g;

    layer->objects = evas_list_append(layer->objects, ell);

    append_undo_new_object(ell, CMD_SYNC, OBJ_ELLIPSE, ell);
    msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);

    ell->owner = (Object *) drawing->current_layer;
    _ell_ssync(ell);
}

void
ell_mirror_x(Ellipse * src, double x0)
{
    Ellipse            *ell;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    ell = _ell_create_copy(src);
    ell->flags = FLAG_VISIBLE;

    ell->y = src->y;
    ell->x = x0 * 2 - src->x;
    ell->g = -src->g;

    layer->objects = evas_list_append(layer->objects, ell);

    append_undo_new_object(ell, CMD_SYNC, OBJ_ELLIPSE, ell);
    msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);

    ell->owner = (Object *) drawing->current_layer;
    _ell_ssync(ell);
}

void
ell_trim_ab(Ellipse * ell, double a, double b, int side)
{
    if (!ell)
        return;
    if ((ell->y > ell->x * a + b) ^ (side))
        ell_delete(ell);
}

void
ell_trim_y(Ellipse * ell, double y0, int side)
{
    if ((ell->y > y0) ^ !side)
        ell_delete(ell);
}
void
ell_trim_x(Ellipse * ell, double x0, int side)
{
    if ((ell->x > x0) ^ !side)
        ell_delete(ell);
}

void
ell_delete(Ellipse * ell)
{
    int                 dummy;

    dummy = ell->flags;
    ell->flags |= FLAG_DELETED;
    append_undo_long((void *)&(ell->flags),
                    dummy, ell->flags, CMD_SYNC, OBJ_ELLIPSE, ell);
    msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);
}

void
ell_cut(Ellipse * ell)
{
}

void
ell_copy(Ellipse * ell, int sock, double dx, double dy)
{
    CP_Header           hd;

    if (!ell)
        return;
    if (!ell->line_style)
        return;

    hd.type = OBJ_ELLIPSE;
    hd.length = sizeof(Ellipse);

    ell->x -= dx;
    ell->y -= dy;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, ell, hd.length);

    ell->x += dx;
    ell->y += dy;

    hd.type = OBJ_NONE;
    hd.length = strlen(ell->line_style) + 1;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, ell->line_style, hd.length);
}

void
ell_paste(CP_Header hd, int sock, double dx, double dy)
{
    Ellipse            *ell, *src;
    void               *data;
    int                 res;

    if (hd.type != OBJ_ELLIPSE)
        return;
    if (hd.length != sizeof(Ellipse))
        return;

    ell = _ell_create_copy(NULL);
    src = (Ellipse *) malloc(hd.length);
    res = read(sock, src, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "ell_paste: lack of bytes for Ellipse\n");
      }

    ell->flags = src->flags;
    ell->color = src->color;
    ell->x = src->x + dx;
    ell->y = src->y + dy;
    ell->rx = src->rx;
    ell->ry = src->ry;
    ell->g = src->g;
    ell->thickness = src->thickness;
    ell->line_scale = src->line_scale;

    read(sock, &hd, sizeof(hd));
    data = malloc(hd.length);
    res = read(sock, data, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "ell_paste: lack of bytes for line_style\n");
      }
    ell->line_style = data;
    if(ell->line_style && (strlen(ell->line_style)>4000))
	                ell->line_style[4000]=0;
    FREE(src);

    drawing->current_layer->objects =
        evas_list_append(drawing->current_layer->objects, ell);

    append_undo_new_object(ell, CMD_SYNC, OBJ_ELLIPSE, ell);
    msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);

    ell->owner = (Object *) drawing->current_layer;
    ell_sync(ell);
    _ell_ssync(ell);
}

char              **
ell_get_info(Ellipse * ell)
{
    char              **p;
    char               *s;
    char                buf[4096];
    int                 pr;

    pr = shell->float_prec;

    p = (char **)malloc(sizeof(char *) * 10);
    memset(p, 0, sizeof(char *) * 10);

    s = _("Object type: ellipse");

    p[0] = DUP(s);

    sprintf(buf, _("X: %.*f"), pr, ell->x);
    p[1] = DUP(buf);

    sprintf(buf, _("Y: %.*f"), pr, ell->y);
    p[2] = DUP(buf);

    sprintf(buf, _("RX/RY: %.*f/%.*f"), pr, ell->rx, pr, ell->ry);
    p[3] = DUP(buf);

    sprintf(buf, _("Gamma: %.*f"), pr, ell->g);
    p[4] = DUP(buf);

    sprintf(buf, _("Line style: %s"), ell->line_style);
    p[5] = DUP(buf);

    sprintf(buf, _("Line style scale: %.*f"), pr, ell->line_scale);
    p[6] = DUP(buf);

    sprintf(buf, _("Line thickness: %.*f"), pr, ell->thickness);
    p[7] = DUP(buf);

    sprintf(buf, _("Owner: %s"), ((Layer *) ell->owner)->label);
    p[8] = DUP(buf);

    return p;
}

int
ell_select_by_click(Ellipse * ell, double x, double y)
{
    int                 dummy;
    double              epsilon, dist;
    double              tx;

    if (ell->flags & FLAG_SELECTED)
        return 0;

    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;

    x -= ell->x;
    y -= ell->y;

    tx = x;

    x = tx * cos(ell->g / 180 * M_PI) + y * sin(ell->g / 180 * M_PI);
    y = y * cos(ell->g / 180 * M_PI) - tx * sin(ell->g / 180 * M_PI);

    y *= ell->rx / ell->ry;

    dist = x * x + y * y;

    if (fabs(dist - ell->rx * ell->rx) > (epsilon * 16))
        return 0;

    dummy = ell->flags;
    ell->flags |= FLAG_SELECTED;
    append_undo_long((void *)&(ell->flags),
                    dummy, ell->flags, CMD_SYNC, OBJ_ELLIPSE, ell);
    msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);
    return 1;
}

void
ell_select_by_rect(Ellipse * ell, double x, double y, double w, double h)
{
    int                 dummy;

    if (ell->flags & FLAG_SELECTED)
        return;
    if (( ell->rect.x1 > x )   &&
		    ( ell->rect.x2 < x+w ) &&
		    ( ell->rect.y1 > y )   &&
		    ( ell->rect.y2 < y+h ))	    
      {
          dummy = ell->flags;
          ell->flags |= FLAG_SELECTED;
          append_undo_long((void *)&(ell->flags),
                          dummy, ell->flags, CMD_SYNC, OBJ_ELLIPSE, ell);
          msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);
      }
}

int
ell_deselect_by_click(Ellipse * ell, double x, double y)
{
    int                 dummy;
    double              epsilon, dist;
    double              tx;

    if (!(ell->flags & FLAG_SELECTED))
        return 0;

    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;

    x -= ell->x;
    y -= ell->y;

    tx = x;

    x = tx * cos(ell->g / 180 * M_PI) + y * sin(ell->g / 180 * M_PI);
    y = y * cos(ell->g / 180 * M_PI) - tx * sin(ell->g / 180 * M_PI);

    y *= ell->rx / ell->ry;

    dist = x * x + y * y;

    if (fabs(dist - ell->rx * ell->rx) > (epsilon * 16))
        return 0;

    dummy = ell->flags;
    ell->flags ^= FLAG_SELECTED;
    append_undo_long((void *)&(ell->flags),
                    dummy, ell->flags, CMD_SYNC, OBJ_ELLIPSE, ell);
    msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);
    return 1;
}

void
ell_deselect_by_rect(Ellipse * ell, double x, double y, double w, double h)
{
    int                 dummy;

    if (!(ell->flags & FLAG_SELECTED))
        return;
    
    if (( ell->rect.x1 > x )   &&
		    ( ell->rect.x2 < x+w ) &&
		    ( ell->rect.y1 > y )   &&
		    ( ell->rect.y2 < y+h ))	    
      {
          dummy = ell->flags;
          ell->flags ^= FLAG_SELECTED;
          append_undo_long((void *)&(ell->flags),
                          dummy, ell->flags, CMD_SYNC, OBJ_ELLIPSE, ell);
          msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);
      }
}

void
pre_ell_x1y1(double x1, double y1)
{
    XY                 *xy;
    Evas_List          *list = NULL;

    xy = (XY *) malloc(sizeof(XY));
    ENGY_ASSERT(xy);

    xy->x = x1;
    xy->y = y1;
    list = evas_list_append(list, xy);

    msg_create_and_send(CMD_PRE_DATA, 0, list);
}

void
pre_ell_rxg(double x1, double y1, double rx, double g)
{
    XY                 *xy;
    Evas_List          *list = NULL;

    xy = (XY *) malloc(sizeof(XY));
    ENGY_ASSERT(xy);
    xy->x = x1;
    xy->y = y1;
    list = evas_list_append(list, xy);

    xy = (XY *) malloc(sizeof(XY));
    ENGY_ASSERT(xy);
    xy->x = rx;
    xy->y = g;
    list = evas_list_append(list, xy);

    msg_create_and_send(CMD_PRE_DATA, 0, list);
}

void
ghost_ell_create(void)
{
    Evas               *e;

    e = shell->evas;
    if (!o1)
      {
          o1 = evas_object_line_add(e);
          evas_object_color_set(o1, ALPHA2, ALPHA2/5, ALPHA2/5, ALPHA2);
          evas_object_layer_set(o1, 17);
          evas_object_pass_events_set(o1, 1);
      }
    if (!o2)
      {
          o2 = evas_object_line_add(e);
          evas_object_color_set(o2, ALPHA2, ALPHA2/5, ALPHA2/5, ALPHA2);
          evas_object_layer_set(o2, 17);
          evas_object_pass_events_set(o2, 1);
      }
}

void
ghost_ell_redraw(Evas_List *data, double x, double y)
{
    XY                 *xy;
    Evas               *e;
    Drawing            *d;
    double              x1, y1, x2, y2, x3, y3, rx, g;

    e = shell->evas;
    evas_object_hide(o1);
    evas_object_hide(o2);

    if (!data)
        return;

    xy = (XY *) data->data;
    if (data->next == NULL)
      {
          x1 = w2s_x(2 * xy->x - x);
          y1 = w2s_y(2 * xy->y - y);
          x2 = w2s_x(x);
          y2 = w2s_y(y);
          evas_object_show( o1);
          evas_object_line_xy_set(o1, x1, y1, x2, y2);
      }
    else
      {
          x1 = xy->x;
          y1 = xy->y;
          xy = (XY *) data->next->data;
          rx = xy->x;
          g = xy->y;
          x2 = rx * cos(g / 180 * M_PI);
          y2 = rx * sin(g / 180 * M_PI);
          evas_object_show( o1);
          evas_object_line_xy_set(o1, w2s_x(x1 - x2), w2s_y(y1 - y2),
                           w2s_x(x1 + x2), w2s_y(y1 + y2));
          x2 = hypot(x1 - x, y1 - y);
          x3 = x2 * cos(g / 180 * M_PI + M_PI / 2) + x1;
          y3 = x2 * sin(g / 180 * M_PI + M_PI / 2) + y1;
          evas_object_show( o2);
          evas_object_line_xy_set(o2, w2s_x(x1), w2s_y(y1), w2s_x(x3), w2s_y(y3));
      }
}

void
ghost_ell_destroy(void)
{
    evas_object_hide(o1);
    evas_object_hide(o2);
}

void
_ell_check_evas_objects(Ellipse * ell)
{
    Evas_List          *l;
    int                 need_to_clear = 0;

    if (!ell->old.line_style || strcmp(ell->old.line_style, ell->line_style))
      {
          need_to_clear = 1;
          FREE(ell->old.line_style);
          ell->old.line_style = DUP(ell->line_style);
      }

    if (ell->thickness * shell->context.show_thickness
        != ell->old.thickness * shell->context.show_thickness)
      {
          need_to_clear = 1;
          ell->old.thickness = ell->thickness;
      }

    if (drawing->scale != drawing->old.scale)
      {
          need_to_clear = 1;
      }

    if (ell->flags != ell->old.flags)
      {
          //need_to_clear = 1;
          ell->old.flags = ell->flags;
      }

    if ((ell->rx != ell->old.rx) || (ell->ry != ell->ry) || (ell->g != ell->g))
      {
          need_to_clear = 1;
          ell->old.rx = ell->rx;
          ell->old.ry = ell->ry;
          ell->old.g = ell->g;
      }

    if ((ell->x != ell->old.x) || (ell->y != ell->y))
      {
          ell->old.x = ell->x;
          ell->old.y = ell->y;
      }

    if (need_to_clear)
      {
          for (l = ell->list; l; l = l->next)
              evas_object_del(l->data);
          ell->list = evas_list_free(ell->list);
      }

    _ell_refresh_evas_objects(ell);

}

void
_ell_refresh_evas_objects(Ellipse * ell)
{

    linestyle_set(ell->line_style);
    if (linestyle_get_tiled())
      {
          _create_tiled_ell(ell);
      }
    else
      {
          _create_scaled_ell(ell);
      }
}

void
_create_scaled_ell(Ellipse * ell)
{
    Evas               *e;
    Evas_List          *list = NULL, *l, *lo;
    Evas_Object        *o;
    float               len, tlen, tscale;
    int                 i, flag;
    float               x, y;
    double              x1, y1, x2, y2, ekst;
    XY                 *xy;
    int                 an;
    double              da, dr, angle, rad;

    if (ell->ry == 0.0)
        return;
    
    ell->rect.x1 = ell->x;
    ell->rect.y1 = ell->y;
    ell->rect.x2 = ell->x;
    ell->rect.y2 = ell->y;

    tlen = linestyle_get_tilelength();
    ekst = ell->ry / ell->rx;

    len = 2 * M_PI * ell->rx;
    tscale = len / tlen;

    flag = linestyle_get_odd();

    list = linestyle_get_points(len, ell->line_scale);

    trans_scale(list, 2 * M_PI / tlen, 2 * M_PI / tlen * ell->rx);
    trans_move(list, 0, ell->rx);

    e = shell->evas;

    lo = ell->list;
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
                an = (int)((b->x - a->x) / (2 * M_PI / shell->arcnum));
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
                                           ell->thickness *
                                           shell->context.
                                           show_thickness * drawing->scale);
                            evas_object_layer_set(o, 10);
                            evas_object_pass_events_set(o, 1);
                            ell->list = evas_list_append(ell->list, o);
                            lo = evas_list_last(ell->list);
                        }
                      o = lo->data;
                      lo = lo->next;
                      if (ell->flags & FLAG_VISIBLE)
                        {
                            evas_object_color_set( o,
				ell->color.red*ell->color.alpha/255,
				ell->color.green*ell->color.alpha/255,
				ell->color.blue*ell->color.alpha/255, 
				ell->color.alpha);
                        }
                      else
                        {
                            evas_object_color_set( o, 0, 0, 0, 0);
                        }

                      if (ell->flags & FLAG_SELECTED)
                          evas_object_color_set( o, 
					  ALPHA5, 
					  ALPHA5/5, 
					  ALPHA5/5, 
					  ALPHA5);
                      if (ell->flags & FLAG_DELETED)
                          evas_object_color_set( o, 0, 0, 0, 0);
                      x = rad * cos(angle);
                      y = rad * sin(angle);
                      y *= ekst;
                      x1 = x * cos(-ell->g / 180 * M_PI) +
                          y * sin(-ell->g / 180 * M_PI);
                      y1 = y * cos(-ell->g / 180 * M_PI) -
                          x * sin(-ell->g / 180 * M_PI);
                      x1 += ell->x;
                      y1 += ell->y;
		      if(ell->rect.x1>x1) ell->rect.x1 = x1;
		      if(ell->rect.x2<x1) ell->rect.x2 = x1;
		      if(ell->rect.y1>y1) ell->rect.y1 = y1;
		      if(ell->rect.y2<y1) ell->rect.y2 = y1;					    
                      x1 = w2s_x(x1);
                      y1 = w2s_y(y1);
		      
                      x = (rad + dr) * cos(angle + da);
                      y = (rad + dr) * sin(angle + da);
                      y *= ekst;
                      x2 = x * cos(-ell->g / 180 * M_PI) +
                          y * sin(-ell->g / 180 * M_PI);
                      y2 = y * cos(-ell->g / 180 * M_PI) -
                          x * sin(-ell->g / 180 * M_PI);
                      x2 += ell->x;
                      y2 += ell->y;
		      if(ell->rect.x1>x2) ell->rect.x1 = x2;
		      if(ell->rect.x2<x2) ell->rect.x2 = x2;
		      if(ell->rect.y1>y2) ell->rect.y1 = y2;
		      if(ell->rect.y2<y2) ell->rect.y2 = y2;										
                      x2 = w2s_x(x2);
                      y2 = w2s_y(y2);

                      _line_item_xy(e, o, x1, y1, x2, y2,
                                    ell->thickness *
                                    shell->context.
                                    show_thickness * drawing->scale);
                      evas_object_show( o);
                  }

            }
      }
    for (l = list; l; l = l->next)
        FREE(l->data);
    list = evas_list_free(list);
}

void
_create_tiled_ell(Ellipse * ell)
{
    Evas_List          *list, *l, *lo;
    int                 i, flag;
    Evas               *e;
    Evas_Object        *o;
    Drawing            *d;
    float               len, tlen;
    float               x, y;
    int                 an;
    double              da, dr, angle, rad;
    double              x1, y1, x2, y2, ekst;

    if (ell->ry == 0.0)
        return;
    ell->rect.x1 = ell->x;
    ell->rect.y1 = ell->y;
    ell->rect.x2 = ell->x;
    ell->rect.y2 = ell->y;

    tlen = linestyle_get_tilelength();
    ekst = ell->ry / ell->rx;

    flag = linestyle_get_odd();
    len = 2 * M_PI * ell->rx;

    list = linestyle_get_points(len, ell->line_scale);
    ENGY_ASSERT(list);

    trans_scale(list, 2 * M_PI / len, 2 * M_PI / len * ell->rx);
    trans_move(list, 0, ell->rx);

    e = shell->evas;

    lo = ell->list;

    lo = ell->list;
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
                an = (int)((b->x - a->x) / (2 * M_PI / shell->arcnum));
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
                                           ell->thickness *
                                           shell->context.
                                           show_thickness * drawing->scale);
                            evas_object_layer_set(o, 10);
                            evas_object_pass_events_set(o, 1);
                            ell->list = evas_list_append(ell->list, o);
                            lo = evas_list_last(ell->list);
                        }
                      o = lo->data;
                      lo = lo->next;
                      if (ell->flags & FLAG_VISIBLE)
                        {
                            evas_object_color_set( o,
				ell->color.red*ell->color.alpha/255,
				ell->color.green*ell->color.alpha/255,
				ell->color.blue*ell->color.alpha/255, 
				ell->color.alpha);
                        }
                      else
                        {
                            evas_object_color_set( o, 0, 0, 0, 0);
                        }

                      if (ell->flags & FLAG_SELECTED)
                          evas_object_color_set( o, 
					  ALPHA5, 
					  ALPHA5/5, 
					  ALPHA5/5, 
					  ALPHA5);
                      if (ell->flags & FLAG_DELETED)
                          evas_object_color_set( o, 0, 0, 0, 0);

                      x = rad * cos(angle);
                      y = rad * sin(angle);
                      y *= ekst;
                      x1 = x * cos(-ell->g / 180 * M_PI) +
                          y * sin(-ell->g / 180 * M_PI);
                      y1 = y * cos(-ell->g / 180 * M_PI) -
                          x * sin(-ell->g / 180 * M_PI);
                      x1 += ell->x;
                      y1 += ell->y;
		      if(ell->rect.x1>x1) ell->rect.x1 = x1;
		      if(ell->rect.x2<x1) ell->rect.x2 = x1;
		      if(ell->rect.y1>y1) ell->rect.y1 = y1;
		      if(ell->rect.y2<y1) ell->rect.y2 = y1;
											
                      x1 = w2s_x(x1);
                      y1 = w2s_y(y1);
                      x = (rad + dr) * cos(angle + da);
                      y = (rad + dr) * sin(angle + da);
                      y *= ekst;
                      x2 = x * cos(-ell->g / 180 * M_PI) +
                          y * sin(-ell->g / 180 * M_PI);
                      y2 = y * cos(-ell->g / 180 * M_PI) -
                          x * sin(-ell->g / 180 * M_PI);
                      x2 += ell->x;
                      y2 += ell->y;
		      if(ell->rect.x1>x2) ell->rect.x1 = x2;
		      if(ell->rect.x2<x2) ell->rect.x2 = x2;
		      if(ell->rect.y1>y2) ell->rect.y1 = y2;
		      if(ell->rect.y2<y2) ell->rect.y2 = y2;										
                      x2 = w2s_x(x2);
                      y2 = w2s_y(y2);

                      _line_item_xy(e, o, x1, y1, x2, y2,
                                    ell->thickness *
                                    shell->context.
                                    show_thickness * drawing->scale);
                      evas_object_show( o);
                  }
            }
      }
    for (l = list; l; l = l->next)
        FREE(l->data);
    list = evas_list_free(list);
}

void
_ell_ssync(Ellipse * ell)
{
    char                buf[4096];
    Drawing            *d;
    Object             *o;

    d = drawing;
    o = ell->owner;

    sprintf(buf, "/layer/%d/entity/%d", o->id, ell->id);
    E_DB_INT_SET(d->fileout, buf, ell->type);
}

void
ell_load(int id)
{
    char               *f;
    char                buf[4096];
    Ellipse            *ell;
    int                 res;
    float               x;

    f = drawing->filein;

    ell = _ell_create_copy(NULL);
    ell->id = id;

    sprintf(buf, "/entity/%d/flags", id);
    E_DB_INT_GET(f, buf, ell->flags, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/red", id);
    E_DB_INT_GET(f, buf, ell->color.red, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/green", id);
    E_DB_INT_GET(f, buf, ell->color.green, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/blue", id);
    E_DB_INT_GET(f, buf, ell->color.blue, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/alpha", id);
    E_DB_INT_GET(f, buf, ell->color.alpha, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/x", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    ell->x = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/y", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    ell->y = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/rx", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    ell->rx = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/ry", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    ell->ry = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/g", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    ell->g = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/linescale", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    ell->line_scale = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/thickness", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    ell->thickness = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/linestyle", id);
    E_DB_STR_GET(f, buf, ell->line_style, res);
    ENGY_ASSERT(res);
    if(ell->line_style && (strlen(ell->line_style)>4000))
	                ell->line_style[4000]=0;

    drawing->current_layer->objects =
        evas_list_append(drawing->current_layer->objects, ell);

    append_undo_new_object(ell, CMD_SYNC, OBJ_ELLIPSE, ell);
    msg_create_and_send(CMD_SYNC, OBJ_ELLIPSE, ell);

    ell->owner = (Object *) drawing->current_layer;
}
