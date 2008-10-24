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
void                _arc_create(double, double, double, double, double);
void                _arc_check_evas_objects(Arc *);
void                _arc_refresh_evas_objects(Arc *);
void                _create_tiled_arc(Arc *);
void                _create_scaled_arc(Arc *);
void                _arc_ssync(Arc *);

void
arc_create(void)
{
    char               *s;
    float               x1 = 0, y1 = 0, x2 = 0, y2 = 0, r, a, b;
    int                 res, res1 = 0;

    serv_set_hint(DUP(_("enter center point of the arc: ")));

    serv_set_state(ST_CIRCLE1);

    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x1, &y1);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_CIRCLE2);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    pre_ci_x1y1(x1, y1);

    shell->context.fx = x1;
    shell->context.fy = y1;

    serv_set_hint(DUP(_("enter point on arcrcumference or radius: ")));
    s = serv_get_string();
    res = get_values(s, x1, y1, &x2, &y2);
    if (res == 1)
      {
          r = x2;
      }
    else
      {
          r = hypot(x2 - x1, y2 - y1);
      }

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
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

    if (a > b)
      {
          b += 360;
      }

    serv_set_state(ST_NORMAL);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    lock_data();
    _arc_create(x1, y1, r, a, b);
    unlock_data();
}

Arc                *
_arc_create_copy(Arc * src)
{
    Arc                *arc;

    arc = (Arc *) malloc(sizeof(Arc));
    ENGY_ASSERT(arc);
    memset(arc, 0, sizeof(Arc));

    arc->type = OBJ_ARC;
    arc->id = rand() + my_int++;
    arc->flags = FLAG_VISIBLE;

    arc->old.color.red = -1;
    arc->old.color.green = -1;
    arc->old.color.blue = -1;
    arc->old.color.alpha = -1;

    if (!src)
        return arc;

    arc->flags = src->flags;
    arc->color = src->color;
    arc->line_style = DUP(src->line_style);
    arc->thickness = src->thickness;
    arc->line_scale = src->line_scale;
    arc->x = src->x;
    arc->y = src->y;
    arc->r = src->r;
    arc->a = src->a;
    arc->b = src->b;

    return arc;
}

void
_arc_create(double x, double y, double r, double a, double b)
{
    Arc                *arc;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    arc = _arc_create_copy(NULL);
    arc->color = shell->context.color;
    arc->line_style = DUP(shell->context.line_style);
    arc->thickness = shell->context.thickness;
    arc->line_scale = shell->context.line_scale;
    arc->x = x;
    arc->y = y;
    arc->r = r;
    arc->a = a;
    arc->b = b;

    layer->objects = eina_list_append(layer->objects, arc);

    append_undo_new_object(arc, CMD_SYNC, OBJ_ARC, arc);
    msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);

    arc->owner = (Object *) drawing->current_layer;
    _arc_ssync(arc);

}

void
arc_redraw(Arc * arc)
{
    if (!arc)
        return;
    if (shell->psout);          /* TODO */
    magnet_attach(arc);
    _arc_check_evas_objects(arc);
}

void
arc_sync(Arc * arc)
{
    char                buf[4096];
    char               *f;

    f = drawing->fileout;

    if (arc->color.red != arc->old.color.red)
      {
          sprintf(buf, "/entity/%d/red", arc->id);
          E_DB_INT_SET(f, buf, arc->color.red);
      }

    if (arc->color.green != arc->old.color.green)
      {
          sprintf(buf, "/entity/%d/green", arc->id);
          E_DB_INT_SET(f, buf, arc->color.green);
      }

    if (arc->color.blue != arc->old.color.blue)
      {
          sprintf(buf, "/entity/%d/blue", arc->id);
          E_DB_INT_SET(f, buf, arc->color.blue);
      }
    if (arc->color.alpha != arc->old.color.alpha)
      {
          sprintf(buf, "/entity/%d/alpha", arc->id);
          E_DB_INT_SET(f, buf, arc->color.alpha);
      }

    if (arc->flags != arc->old.flags)
      {
          sprintf(buf, "/entity/%d/flags", arc->id);
          E_DB_INT_SET(f, buf, arc->flags);
      }
    if (!arc->old.line_style || strcmp(arc->old.line_style, arc->line_style))
      {
          sprintf(buf, "/entity/%d/linestyle", arc->id);
          E_DB_STR_SET(f, buf, arc->line_style);
      }
    if (arc->line_scale != arc->old.line_scale)
      {
          sprintf(buf, "/entity/%d/linescale", arc->id);
          E_DB_FLOAT_SET(f, buf, (float)arc->line_scale);
      }
    if (arc->thickness != arc->old.thickness)
      {
          sprintf(buf, "/entity/%d/thickness", arc->id);
          E_DB_FLOAT_SET(f, buf, (float)arc->thickness);
      }

    if (arc->x != arc->old.x)
      {
          sprintf(buf, "/entity/%d/x", arc->id);
          E_DB_FLOAT_SET(f, buf, (float)arc->x);
      }
    if (arc->y != arc->old.y)
      {
          sprintf(buf, "/entity/%d/y", arc->id);
          E_DB_FLOAT_SET(f, buf, (float)arc->y);
      }
    if (arc->r != arc->old.r)
      {
          sprintf(buf, "/entity/%d/r", arc->id);
          E_DB_FLOAT_SET(f, buf, (float)arc->r);
      }
    if (arc->a != arc->old.a)
      {
          sprintf(buf, "/entity/%d/a", arc->id);
          E_DB_FLOAT_SET(f, buf, (float)arc->a);
      }
    if (arc->b != arc->old.b)
      {
          sprintf(buf, "/entity/%d/b", arc->id);
          E_DB_FLOAT_SET(f, buf, (float)arc->b);
      }

    arc_redraw(arc);
}

void 
arc_destroy(Arc * arc)
{
    Eina_List          *l;

    for (l = drawing->layers; l; l = l->next)
      {
          Layer              *layer;

          layer = (Layer *) l->data;
          layer->objects = eina_list_remove(layer->objects, arc);
      }

    arc_free(arc);
}

void
arc_free(Arc * arc)
{
    Eina_List          *l;

    if (!arc)
        return;

    magnet_detach(arc);

    if (arc->line_style)
        FREE(arc->line_style);
    if (arc->old.line_style)
        FREE(arc->old.line_style);

    for (l = arc->list; l; l = l->next)
        evas_object_del(l->data);
    arc->list = eina_list_free(arc->list);

    FREE(arc);
}

void
arc_move(Arc * arc, double dx, double dy)
{

    if (!arc)
        return;

    append_undo_double((void *)(&(arc->x)),
                       arc->x, arc->x + dx, CMD_SYNC, OBJ_ARC, arc);
    append_undo_double((void *)(&(arc->y)), arc->y, arc->y + dy, 0, 0, NULL);

    arc->x += dx;
    arc->y += dy;

    msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);
}

void
arc_scale(Arc * arc, double x0, double y0, double sc)
{
    double              a, b, c;

    if (!arc)
        return;
    a = arc->x;
    b = arc->y;
    c = arc->r;

    arc->x -= x0;
    arc->y -= y0;
    arc->x *= sc;
    arc->y *= sc;
    arc->x += x0;
    arc->y += y0;
    arc->r *= sc;

    append_undo_double((void *)(&(arc->x)), a, arc->x, CMD_SYNC, OBJ_ARC, arc);
    append_undo_double((void *)(&(arc->y)), b, arc->y, 0, 0, NULL);
    append_undo_double((void *)(&(arc->r)), c, arc->r, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);
}

void
arc_scale_xy(Arc * arc, double x0, double y0, double scx, double scy)
{
    double              a, b, c;

    if (!arc)
        return;
    a = arc->x;
    b = arc->y;
    c = arc->r;

    arc->x -= x0;
    arc->y -= y0;
    arc->x *= scx;
    arc->y *= scy;
    arc->x += x0;
    arc->y += y0;
    arc->r *= (scx > scy) ? scx : scy;

    append_undo_double((void *)(&(arc->x)), a, arc->x, CMD_SYNC, OBJ_ARC, arc);
    append_undo_double((void *)(&(arc->y)), b, arc->y, 0, 0, NULL);
    append_undo_double((void *)(&(arc->r)), c, arc->r, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);
}

void
arc_rotate(Arc * arc, double x0, double y0, double angle)
{
    double              a, b, c, d;
    double              tx, ty;

    if (!arc)
        return;
    a = arc->x;
    b = arc->y;
    c = arc->a;
    d = arc->b;

    angle = -angle;
    arc->x -= x0;
    arc->y -= y0;
    tx = arc->x * cos(angle) + arc->y * sin(angle);
    ty = arc->y * cos(angle) - arc->x * sin(angle);
    arc->x = tx + x0;
    arc->y = ty + y0;
    arc->a -= angle / M_PI * 180;
    arc->b -= angle / M_PI * 180;

    append_undo_double((void *)(&(arc->x)), a, arc->x, CMD_SYNC, OBJ_ARC, arc);
    append_undo_double((void *)(&(arc->y)), b, arc->y, 0, 0, NULL);
    append_undo_double((void *)(&(arc->a)), c, arc->a, 0, 0, NULL);
    append_undo_double((void *)(&(arc->b)), d, arc->b, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);
}

void
arc_clone(Arc * src, double dx, double dy)
{
    Arc                *arc;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    arc = _arc_create_copy(src);
    arc->flags = FLAG_VISIBLE;

    arc->x += dx;
    arc->y += dy;

    layer->objects = eina_list_append(layer->objects, arc);

    append_undo_new_object(arc, CMD_SYNC, OBJ_ARC, arc);
    msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);

    arc->owner = (Object *) drawing->current_layer;
    _arc_ssync(arc);

}

void
arc_array(Arc * arc, int numx, int numy, double dx, double dy)
{
    int                 i, j;

    if (!arc)
        return;
    if (numx < 1)
        return;
    if (numy < 1)
        return;
    for (i = 0; i < numx; i++)
        for (j = 0; j < numy; j++)
            if (i || j)
                arc_clone(arc, i * dx, j * dy);
}

void
arc_array_polar(Arc * arc, double x0, double y0, int num, double da)
{
    int                 i;

    if (!arc)
        return;
    if (num < 1)
        return;
    for (i = 1; i < num; i++)
      {
          arc_clone(arc, 0, 0);
          arc_rotate(eina_list_last(drawing->current_layer->objects)->data,
                     x0, y0, i * da);
      }
}

void
arc_mirror_ab(Arc * src, double a, double b)
{
    Arc                *arc;
    Drawing            *d;
    Layer              *layer;
    double              resx, resy, tx, ty, angle;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    arc = _arc_create_copy(src);
    arc->flags = FLAG_VISIBLE;

    tx = arc->x;
    ty = arc->y;
    ty -= b;
    angle = atan(a);
    resx = tx * cos(2 * angle) + ty * sin(2 * angle);
    resy = -ty * cos(2 * angle) + tx * sin(2 * angle);
    arc->x = resx;
    arc->y = resy + b;
    tx = arc->b;
    arc->b = 2 * angle / M_PI * 180 - arc->a;
    arc->a = 2 * angle / M_PI * 180 - tx;

    layer->objects = eina_list_append(layer->objects, arc);

    append_undo_new_object(arc, CMD_SYNC, OBJ_ARC, arc);
    msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);

    arc->owner = (Object *) drawing->current_layer;
    _arc_ssync(arc);
}

void
arc_mirror_y(Arc * src, double y0)
{
    Arc                *arc;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    arc = _arc_create_copy(src);
    arc->flags = FLAG_VISIBLE;

    arc->x = src->x;
    arc->y = y0 * 2 - src->y;
    arc->a = -src->b;
    arc->b = -src->a;

    layer->objects = eina_list_append(layer->objects, arc);

    append_undo_new_object(arc, CMD_SYNC, OBJ_ARC, arc);
    msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);

    arc->owner = (Object *) drawing->current_layer;
    _arc_ssync(arc);
}

void
arc_mirror_x(Arc * src, double x0)
{
    Arc                *arc;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    arc = _arc_create_copy(src);
    arc->flags = FLAG_VISIBLE;

    arc->y = src->y;
    arc->x = x0 * 2 - src->x;
    arc->a = 180 - src->b;
    arc->b = 180 - src->a;

    layer->objects = eina_list_append(layer->objects, arc);

    append_undo_new_object(arc, CMD_SYNC, OBJ_ARC, arc);
    msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);

    arc->owner = (Object *) drawing->current_layer;
    _arc_ssync(arc);
}

void
arc_trim_ab(Arc * arc, double a, double b, int side)
{
    if (!arc)
        return;
    if ((arc->y > arc->x * a + b) ^ (side))
        arc_delete(arc);
}

void
arc_trim_y(Arc * arc, double y0, int side)
{
    if ((arc->y > y0) ^ !side)
        arc_delete(arc);
}
void
arc_trim_x(Arc * arc, double x0, int side)
{
    if ((arc->x > x0) ^ !side)
        arc_delete(arc);
}

void
arc_delete(Arc * arc)
{
    int                 dummy;

    dummy = arc->flags;
    arc->flags |= FLAG_DELETED;
    append_undo_long((void *)&(arc->flags),
                    dummy, arc->flags, CMD_SYNC, OBJ_ARC, arc);
    msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);
}

void
arc_cut(Arc * arc)
{
}

void
arc_copy(Arc * arc, int sock, double dx, double dy)
{
    CP_Header           hd;

    if (!arc)
        return;
    if (!arc->line_style)
        return;

    hd.type = OBJ_ARC;
    hd.length = sizeof(Arc);

    arc->x -= dx;
    arc->y -= dy;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, arc, hd.length);

    arc->x += dx;
    arc->y += dy;

    hd.type = OBJ_NONE;
    hd.length = strlen(arc->line_style) + 1;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, arc->line_style, hd.length);
}

void
arc_paste(CP_Header hd, int sock, double dx, double dy)
{
    Arc                *arc, *src;
    void               *data;
    int                 res;

    if (hd.type != OBJ_ARC)
        return;
    if (hd.length != sizeof(Arc))
        return;

    arc = _arc_create_copy(NULL);
    src = (Arc *) malloc(hd.length);
    res = read(sock, src, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "arc_paste: lack of bytes for Arc\n");
      }

    arc->flags = src->flags;
    arc->color = src->color;
    arc->x = src->x + dx;
    arc->y = src->y + dy;
    arc->r = src->r;
    arc->a = src->a;
    arc->b = src->b;
    arc->thickness = src->thickness;
    arc->line_scale = src->line_scale;

    read(sock, &hd, sizeof(hd));
    data = malloc(hd.length);
    res = read(sock, data, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "arc_paste: lack of bytes for line_style\n");
      }
    arc->line_style = data;
    if(arc->line_style && (strlen(arc->line_style)>4000))
	        arc->line_style[4000]=0;
    FREE(src);

    drawing->current_layer->objects =
        eina_list_append(drawing->current_layer->objects, arc);

    append_undo_new_object(arc, CMD_SYNC, OBJ_ARC, arc);
    msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);

    arc->owner = (Object *) drawing->current_layer;
    arc_sync(arc);
    _arc_ssync(arc);
}

char              **
arc_get_info(Arc * arc)
{
    char              **p;
    char               *s;
    char                buf[4096];
    int                 pr;

    pr = shell->float_prec;

    p = (char **)malloc(sizeof(char *) * 10);
    memset(p, 0, sizeof(char *) * 10);

    s = _("Object type: arc");

    p[0] = DUP(s);

    sprintf(buf, _("X: %.*f"), pr, arc->x);
    p[1] = DUP(buf);

    sprintf(buf, _("Y: %.*f"), pr, arc->y);
    p[2] = DUP(buf);

    sprintf(buf, _("R: %.*f"), pr, arc->r);
    p[3] = DUP(buf);

    sprintf(buf, _("Alpha/Beta: %.*f/%.*f"), pr, arc->a, pr, arc->b);
    p[3] = DUP(buf);

    sprintf(buf, _("Line style: %s"), arc->line_style);
    p[5] = DUP(buf);

    sprintf(buf, _("Line style scale: %.*f"), pr, arc->line_scale);
    p[6] = DUP(buf);

    sprintf(buf, _("Line thickness: %.*f"), pr, arc->thickness);
    p[7] = DUP(buf);

    sprintf(buf, _("Owner: %s"), ((Layer *) arc->owner)->label);
    p[8] = DUP(buf);

    return p;
}

int
arc_select_by_click(Arc * arc, double x, double y)
{
    int                 dummy;
    double              epsilon, dist, angle;

    if (arc->flags & FLAG_SELECTED)
        return 0;

    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;

    x -= arc->x;
    y -= arc->y;

    if (!x && !y)
        return 0;

    dist = x * x + y * y;
    if (x)
      {
          angle = atan(y / x);
      }
    else
      {
          angle = M_PI / 2 * y / fabs(y);
      }
    angle = angle / M_PI * 180;
    if (x < 0)
        angle += 180;

    if (fabs(dist - arc->r * arc->r) > (epsilon * 16))
        return 0;
    if ((angle > arc->b) || (angle < arc->a))
        return 0;

    dummy = arc->flags;
    arc->flags |= FLAG_SELECTED;
    append_undo_long((void *)&(arc->flags),
                    dummy, arc->flags, CMD_SYNC, OBJ_ARC, arc);
    msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);
    return 1;
}

void
arc_select_by_rect(Arc * arc, double x, double y, double w, double h)
{
    int                 dummy;

    if (arc->flags & FLAG_SELECTED)
        return;

    if (( arc->rect.x1 > x )   &&
        ( arc->rect.x2 < x+w ) &&
        ( arc->rect.y1 > y )   && 
	( arc->rect.y2 < y+h ))
      {
          dummy = arc->flags;
          arc->flags |= FLAG_SELECTED;
          append_undo_long((void *)&(arc->flags),
                          dummy, arc->flags, CMD_SYNC, OBJ_ARC, arc);
          msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);
      }
}

int
arc_deselect_by_click(Arc * arc, double x, double y)
{
    int                 dummy;
    double              epsilon, dist, angle;

    if (!(arc->flags & FLAG_SELECTED))
        return 0;

    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;

    x -= arc->x;
    y -= arc->y;

    if (!x && !y)
        return 0;

    dist = x * x + y * y;
    if (x)
      {
          angle = atan(y / x);
      }
    else
      {
          angle = M_PI / 2 * y / fabs(y);
      }
    angle = angle / M_PI * 180;
    if (x < 0)
        angle += 180;

    if (fabs(dist - arc->r * arc->r) > (epsilon * 16))
        return 0;
    if ((angle > arc->b) || (angle < arc->a))
        return 0;

    dummy = arc->flags;
    arc->flags ^= FLAG_SELECTED;
    append_undo_long((void *)&(arc->flags),
                    dummy, arc->flags, CMD_SYNC, OBJ_ARC, arc);
    msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);
    return 1;
}

void
arc_deselect_by_rect(Arc * arc, double x, double y, double w, double h)
{
    int                 dummy;

    if (!(arc->flags & FLAG_SELECTED))
        return;
    
    if (( arc->rect.x1 > x )   &&
		    ( arc->rect.x2 < x+w ) &&
		    ( arc->rect.y1 > y )   &&
		    ( arc->rect.y2 < y+h ))
      {
          dummy = arc->flags;
          arc->flags ^= FLAG_SELECTED;
          append_undo_long((void *)&(arc->flags),
                          dummy, arc->flags, CMD_SYNC, OBJ_ARC, arc);
          msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);
      }
}

void
_arc_check_evas_objects(Arc * arc)
{
    Eina_List          *l;
    int                 need_to_clear = 0;

    if (!arc->old.line_style || strcmp(arc->old.line_style, arc->line_style))
      {
          need_to_clear = 1;
          FREE(arc->old.line_style);
          arc->old.line_style = DUP(arc->line_style);
      }

    if (arc->thickness * shell->context.show_thickness
        != arc->old.thickness * shell->context.show_thickness)
      {
          need_to_clear = 1;
          arc->old.thickness = arc->thickness;
      }

    if (drawing->scale != drawing->old.scale)
      {
          need_to_clear = 1;
      }

    if (arc->flags != arc->old.flags)
      {
/*	need_to_clear = 1;*/
          arc->old.flags = arc->flags;
      }

    if ((arc->x != arc->old.x) ||
        (arc->y != arc->old.y) ||
        (arc->r != arc->old.r) ||
        (arc->a != arc->old.a) || (arc->b != arc->old.b))
      {
          need_to_clear = 1;
          arc->old.x = arc->x;
          arc->old.y = arc->y;
          arc->old.r = arc->r;
          arc->old.a = arc->a;
          arc->old.b = arc->b;
      }

    if (need_to_clear)
      {
          for (l = arc->list; l; l = l->next)
              evas_object_del(l->data);
          arc->list = eina_list_free(arc->list);
      }

    _arc_refresh_evas_objects(arc);

}

void
_arc_refresh_evas_objects(Arc * arc)
{
    linestyle_set(arc->line_style);
    if (linestyle_get_tiled())
      {
          _create_tiled_arc(arc);
      }
    else
      {
          _create_scaled_arc(arc);
      }
}

void
_create_scaled_arc(Arc * arc)
{
    Evas               *e;
    Eina_List          *list = NULL, *l, *lo;
    Evas_Object        *o;
    float               len, tlen, tscale;
    int                 i, flag;
    float               x, y;
    double              x1, y1, x2, y2;
    XY                 *xy;
    int                 an;
    double              da, dr, angle, rad;

    tlen = linestyle_get_tilelength();
    arc->rect.x1 = arc->x;
    arc->rect.y1 = arc->y;
    arc->rect.x2 = arc->x;
    arc->rect.y2 = arc->y;

    len = (arc->b - arc->a) / 180.0 * M_PI * arc->r;
    tscale = len / tlen;

    flag = linestyle_get_odd();

    list = linestyle_get_points(len, arc->line_scale);

    trans_scale(list, len / arc->r / tlen, len / tlen);
    trans_move(list, arc->a / 180 * M_PI, arc->r);

    e = shell->evas;

    lo = arc->list;
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
                           ((arc->b - arc->a) / 180 * M_PI / shell->arcnum));
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
                                           arc->thickness *
                                           shell->context.
                                           show_thickness * drawing->scale);
                            evas_object_layer_set(o, 10);
                            evas_object_pass_events_set(o, 1);
                            arc->list = eina_list_append(arc->list, o);
                            lo = eina_list_last(arc->list);
                        }
                      o = lo->data;
                      lo = lo->next;
                      if (arc->flags & FLAG_VISIBLE)
                        {
                            evas_object_color_set(o,
				(arc->color.red*arc->color.alpha)/255,
				(arc->color.green*arc->color.alpha)/255,
				(arc->color.blue*arc->color.alpha)/255,
				arc->color.alpha);
                        }
                      else
                        {
                            evas_object_color_set(o, 0, 0, 0, 0);
                        }
                      if (arc->flags & FLAG_SELECTED)
                          evas_object_color_set( o, 
					  ALPHA5, 
					  ALPHA5/5, 
					  ALPHA5/5, 
					  ALPHA5);
                      if (arc->flags & FLAG_DELETED)
                          evas_object_color_set( o, 0, 0, 0, 0);
                      x1 = rad * cos(angle) + arc->x;
		      if(arc->rect.x1>x1) arc->rect.x1 = x1;
		      if(arc->rect.x2<x1) arc->rect.x2 = x1;
                      x1 = w2s_x(x1);
		      
                      y1 = rad * sin(angle) + arc->y;
		      if(arc->rect.y1>y1) arc->rect.y1 = y1;
		      if(arc->rect.y2<y1) arc->rect.y2 = y1;
                      y1 = w2s_y(y1);
		      
                      x2 = (rad + dr) * cos(angle + da) + arc->x;
		      if(arc->rect.x1>x2) arc->rect.x1 = x2;
		      if(arc->rect.x2<x2) arc->rect.x2 = x2;
                      x2 = w2s_x(x2);
		      
                      y2 = (rad + dr) * sin(angle + da) + arc->y;
		      if(arc->rect.y1>y2) arc->rect.y1 = y2;
		      if(arc->rect.y2<y2) arc->rect.y2 = y2;
                      y2 = w2s_y(y2);
		      

                      _line_item_xy(e, o, x1, y1, x2, y2,
                                    arc->thickness *
                                    shell->context.
                                    show_thickness * drawing->scale);
                      evas_object_show(o);
                  }
            }
      }
    for (l = list; l; l = l->next)
        FREE(l->data);
    list = eina_list_free(list);
}

void
_create_tiled_arc(Arc * arc)
{
    Eina_List          *list, *l, *lo;
    int                 i, flag;
    Evas               *e;
    Evas_Object        *o;
    Drawing            *d;
    float               len, tlen;
    int                 an;
    double              da, dr, angle, rad;
    double              x1, y1, x2, y2;

    arc->rect.x1 = arc->x;
    arc->rect.y1 = arc->y;
    arc->rect.x2 = arc->x;
    arc->rect.y2 = arc->y;		
    
    tlen = linestyle_get_tilelength();

    flag = linestyle_get_odd();
    len = (arc->b - arc->a) / 180.0 * M_PI * arc->r;

    list = linestyle_get_points(len, arc->line_scale);
    ENGY_ASSERT(list);

    trans_scale(list, 1 / arc->r, 1);
    trans_move(list, arc->a / 180.0 * M_PI, arc->r);

    e = shell->evas;

    lo = arc->list;
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
                           ((arc->b - arc->a) / 180.0 * M_PI / shell->arcnum));
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
                                           arc->thickness *
                                           shell->context.
                                           show_thickness * drawing->scale);
                            evas_object_layer_set(o, 10);
                            evas_object_pass_events_set(o, 1);
                            arc->list = eina_list_append(arc->list, o);
                            lo = eina_list_last(arc->list);
                        }
                      o = lo->data;
                      lo = lo->next;
                      if (arc->flags & FLAG_VISIBLE)
                        {
                            evas_object_color_set(o,
				(arc->color.red*arc->color.alpha)/255,
				(arc->color.green*arc->color.alpha)/255,
				(arc->color.blue*arc->color.alpha)/255, 
				arc->color.alpha);
                        }
                      else
                        {
                            evas_object_color_set(o, 0, 0, 0, 0);
                        }
                      if (arc->flags & FLAG_SELECTED)
                          evas_object_color_set( o, 
					  ALPHA5,
					  ALPHA5/5,
					  ALPHA5/5,
					  ALPHA5);
                      if (arc->flags & FLAG_DELETED)
                          evas_object_color_set( o, 0, 0, 0, 0);
                      x1 = w2s_x(rad * cos(angle) + arc->x);
                      y1 = w2s_y(rad * sin(angle) + arc->y);
                      x2 = w2s_x((rad + dr) * cos(angle + da) + arc->x);
                      y2 = w2s_y((rad + dr) * sin(angle + da) + arc->y);

		      x1 = rad * cos(angle) + arc->x;
		      if(arc->rect.x1>x1) arc->rect.x1 = x1;
		      if(arc->rect.x2<x1) arc->rect.x2 = x1;
		      x1 = w2s_x(x1);
		      
		      y1 = rad * sin(angle) + arc->y;
		      if(arc->rect.y1>y1) arc->rect.y1 = y1;
		      if(arc->rect.y2<y1) arc->rect.y2 = y1;
		      y1 = w2s_y(y1);
		      
		      x2 = (rad + dr) * cos(angle + da) + arc->x;
		      if(arc->rect.x1>x2) arc->rect.x1 = x2;
		      if(arc->rect.x2<x2) arc->rect.x2 = x2;
		      x2 = w2s_x(x2);
		      
		      y2 = (rad + dr) * sin(angle + da) + arc->y;
		      if(arc->rect.y1>y2) arc->rect.y1 = y2;
		      if(arc->rect.y2<y2) arc->rect.y2 = y2;
		      y2 = w2s_y(y2);
		      
                      _line_item_xy(e, o, x1, y1, x2, y2,
                                    arc->thickness *
                                    shell->context.
                                    show_thickness * drawing->scale);
                      evas_object_show(o);
                  }
            }
      }
    for (l = list; l; l = l->next)
        FREE(l->data);
    list = eina_list_free(list);
}

void
_arc_ssync(Arc * arc)
{
    char                buf[4096];
    Drawing            *d;
    Object             *o;

    d = drawing;
    o = arc->owner;

    sprintf(buf, "/layer/%d/entity/%d", o->id, arc->id);
    E_DB_INT_SET(d->fileout, buf, arc->type);
}

void
arc_load(int id)
{
    char               *f;
    char                buf[4096];
    Arc                *arc;
    int                 res;
    float               x;

    f = drawing->filein;

    arc = _arc_create_copy(NULL);
    arc->id = id;

    sprintf(buf, "/entity/%d/flags", id);
    E_DB_INT_GET(f, buf, arc->flags, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/red", id);
    E_DB_INT_GET(f, buf, arc->color.red, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/green", id);
    E_DB_INT_GET(f, buf, arc->color.green, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/blue", id);
    E_DB_INT_GET(f, buf, arc->color.blue, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/alpha", id);
    E_DB_INT_GET(f, buf, arc->color.alpha, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/x", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    arc->x = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/y", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    arc->y = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/r", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    arc->r = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/a", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    arc->a = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/b", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    arc->b = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/linescale", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    arc->line_scale = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/thickness", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    arc->thickness = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/linestyle", id);
    E_DB_STR_GET(f, buf, arc->line_style, res);
    ENGY_ASSERT(res);
    if(arc->line_style && (strlen(arc->line_style)>4000))
	arc->line_style[4000]=0;

    drawing->current_layer->objects =
        eina_list_append(drawing->current_layer->objects, arc);

    append_undo_new_object(arc, CMD_SYNC, OBJ_ARC, arc);
    msg_create_and_send(CMD_SYNC, OBJ_ARC, arc);

    arc->owner = (Object *) drawing->current_layer;
}
