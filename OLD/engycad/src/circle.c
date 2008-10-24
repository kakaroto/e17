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

Evas_Object        *o_ci[20];

/* protos */
void                _ci_create(double, double, double);
void                _ci_check_evas_objects(Circle *);
void                _ci_refresh_evas_objects(Circle *);
void                _create_tiled_ci(Circle *);
void                _create_scaled_ci(Circle *);
void                _ci_ssync(Circle *);

void
pre_ci_x1y1(double x, double y)
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
circle_create(void)
{
    char               *s;
    float               x1 = 0, y1 = 0, x2 = 0, y2 = 0, r;
    int                 res, res1 = 0;

    serv_set_hint(DUP(_("enter center point of the circle: ")));

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

    serv_set_hint(DUP(_("enter point on circumference or radius: ")));
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

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;

    lock_data();
    _ci_create(x1, y1, r);
    shell->context.fx = x2;
    shell->context.fy = y2;
    unlock_data();
}

Circle             *
_circle_create_copy(Circle * src)
{
    Circle             *ci;
    int                 id;

    ci = (Circle *) malloc(sizeof(Circle));
    ENGY_ASSERT(ci);
    memset(ci, 0, sizeof(Circle));

    ci->type = OBJ_CIRCLE;
    id = rand() + my_int++;
    ci->id = id;
    ci->flags = FLAG_VISIBLE;
    ci->old.color.red = -1;
    ci->old.color.green = -1;
    ci->old.color.blue = -1;
    ci->old.color.alpha = -1;

    if (!src)
        return ci;

    ci->flags = src->flags;
    ci->color = src->color;
    ci->line_style = DUP(src->line_style);
    ci->thickness = src->thickness;
    ci->line_scale = src->line_scale;
    ci->x = src->x;
    ci->y = src->y;
    ci->r = src->r;

    return ci;
}

void
_ci_create(double x, double y, double r)
{
    Circle             *ci;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    ci = _circle_create_copy(NULL);

    ci->color = shell->context.color;
    ci->line_style = DUP(shell->context.line_style);
    ci->thickness = shell->context.thickness;
    ci->line_scale = shell->context.line_scale;
    ci->x = x;
    ci->y = y;
    ci->r = r;

    layer->objects = eina_list_append(layer->objects, ci);

    append_undo_new_object(ci, CMD_SYNC, OBJ_CIRCLE, ci);
    msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);

    ci->owner = (Object *) drawing->current_layer;
    _ci_ssync(ci);
}

void
ci_redraw(Circle * ci)
{
    if (!ci)
        return;
    if (shell->psout);          /* TODO */
    magnet_attach(ci);
    _ci_check_evas_objects(ci);
}

void
ci_sync(Circle * ci)
{
    char                buf[4096];
    char               *f;

    f = drawing->fileout;

    if (ci->color.red != ci->old.color.red)
      {
          sprintf(buf, "/entity/%d/red", ci->id);
          E_DB_INT_SET(f, buf, ci->color.red);
      }

    if (ci->color.green != ci->old.color.green)
      {
          sprintf(buf, "/entity/%d/green", ci->id);
          E_DB_INT_SET(f, buf, ci->color.green);
      }

    if (ci->color.blue != ci->old.color.blue)
      {
          sprintf(buf, "/entity/%d/blue", ci->id);
          E_DB_INT_SET(f, buf, ci->color.blue);
      }
    if (ci->color.alpha != ci->old.color.alpha)
      {
          sprintf(buf, "/entity/%d/alpha", ci->id);
          E_DB_INT_SET(f, buf, ci->color.alpha);
      }

    if (ci->flags != ci->old.flags)
      {
          sprintf(buf, "/entity/%d/flags", ci->id);
          E_DB_INT_SET(f, buf, ci->flags);
      }
    if (!ci->old.line_style || strcmp(ci->old.line_style, ci->line_style))
      {
          sprintf(buf, "/entity/%d/linestyle", ci->id);
          E_DB_STR_SET(f, buf, ci->line_style);
      }
    if (ci->line_scale != ci->old.line_scale)
      {
          sprintf(buf, "/entity/%d/linescale", ci->id);
          E_DB_FLOAT_SET(f, buf, (float)ci->line_scale);
      }
    if (ci->thickness != ci->old.thickness)
      {
          sprintf(buf, "/entity/%d/thickness", ci->id);
          E_DB_FLOAT_SET(f, buf, (float)ci->thickness);
      }

    if (ci->x != ci->old.x)
      {
          sprintf(buf, "/entity/%d/x", ci->id);
          E_DB_FLOAT_SET(f, buf, (float)ci->x);
      }
    if (ci->y != ci->old.y)
      {
          sprintf(buf, "/entity/%d/y", ci->id);
          E_DB_FLOAT_SET(f, buf, (float)ci->y);
      }
    if (ci->r != ci->old.r)
      {
          sprintf(buf, "/entity/%d/r", ci->id);
          E_DB_FLOAT_SET(f, buf, (float)ci->r);
      }

    ci_redraw(ci);
}

void 
ci_destroy(Circle * ci)
{
	Eina_List          *l;
	for (l = drawing->layers; l; l = l->next)
	{
		Layer              *layer;
		
		layer = (Layer *) l->data;
		layer->objects = eina_list_remove(layer->objects, ci);
	}
	ci_free(ci);
}

void
ci_free(Circle * ci)
{
    Eina_List          *l;

    if (!ci)
        return;

    magnet_detach(ci);

    if (ci->line_style)
        FREE(ci->line_style);
    if (ci->old.line_style)
        FREE(ci->old.line_style);

    for (l = ci->list; l; l = l->next)
        evas_object_del(l->data);
    ci->list = eina_list_free(ci->list);

    FREE(ci);
}

void
ci_move(Circle * ci, double dx, double dy)
{

    if (!ci)
        return;

    append_undo_double((void *)(&(ci->x)),
                       ci->x, ci->x + dx, CMD_SYNC, OBJ_CIRCLE, ci);
    append_undo_double((void *)(&(ci->y)), ci->y, ci->y + dy, 0, 0, NULL);

    ci->x += dx;
    ci->y += dy;

    msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);
}

void
ci_scale(Circle * ci, double x0, double y0, double sc)
{
    double              a, b, c;

    if (!ci)
        return;
    a = ci->x;
    b = ci->y;
    c = ci->r;

    ci->x -= x0;
    ci->y -= y0;
    ci->x *= sc;
    ci->y *= sc;
    ci->x += x0;
    ci->y += y0;
    ci->r *= sc;

    append_undo_double((void *)(&(ci->x)), a, ci->x, CMD_SYNC, OBJ_CIRCLE, ci);
    append_undo_double((void *)(&(ci->y)), b, ci->y, 0, 0, NULL);
    append_undo_double((void *)(&(ci->r)), c, ci->r, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);
}

void
ci_scale_xy(Circle * ci, double x0, double y0, double scx, double scy)
{
    double              a, b, c;

    if (!ci)
        return;
    a = ci->x;
    b = ci->y;
    c = ci->r;

    ci->x -= x0;
    ci->y -= y0;
    ci->x *= scx;
    ci->y *= scy;
    ci->x += x0;
    ci->y += y0;
    ci->r *= (scx > scy) ? scx : scy;

    append_undo_double((void *)(&(ci->x)), a, ci->x, CMD_SYNC, OBJ_CIRCLE, ci);
    append_undo_double((void *)(&(ci->y)), b, ci->y, 0, 0, NULL);
    append_undo_double((void *)(&(ci->r)), c, ci->r, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);
}

void
ci_rotate(Circle * ci, double x0, double y0, double angle)
{
    double              a, b;
    double              tx, ty;

    if (!ci)
        return;
    a = ci->x;
    b = ci->y;

    angle = -angle;
    ci->x -= x0;
    ci->y -= y0;
    tx = ci->x * cos(angle) + ci->y * sin(angle);
    ty = ci->y * cos(angle) - ci->x * sin(angle);
    ci->x = tx + x0;
    ci->y = ty + y0;

    append_undo_double((void *)(&(ci->x)), a, ci->x, CMD_SYNC, OBJ_CIRCLE, ci);
    append_undo_double((void *)(&(ci->y)), b, ci->y, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);
}

void
ci_clone(Circle * src, double dx, double dy)
{
    Circle             *ci;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    ci = _circle_create_copy(src);
    ci->flags = FLAG_VISIBLE;
    ci->x += dx;
    ci->y += dy;

    layer->objects = eina_list_append(layer->objects, ci);

    append_undo_new_object(ci, CMD_SYNC, OBJ_CIRCLE, ci);
    msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);

    ci->owner = (Object *) drawing->current_layer;
    _ci_ssync(ci);
}

void
ci_array(Circle * ci, int numx, int numy, double dx, double dy)
{
    int                 i, j;

    if (!ci)
        return;
    if (numx < 1)
        return;
    if (numy < 1)
        return;
    for (i = 0; i < numx; i++)
        for (j = 0; j < numy; j++)
            if (i || j)
                ci_clone(ci, i * dx, j * dy);
}

void
ci_array_polar(Circle * ci, double x0, double y0, int num, double da)
{
    int                 i;

    if (!ci)
        return;
    if (num < 1)
        return;
    for (i = 1; i < num; i++)
      {
          ci_clone(ci, 0, 0);
          ci_rotate(eina_list_last(drawing->current_layer->objects)->data,
                    x0, y0, i * da);
      }
}

void
ci_mirror_ab(Circle * src, double a, double b)
{
    Circle             *ci;
    Drawing            *d;
    Layer              *layer;
    double              resx, resy, tx, ty, angle;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    ci = _circle_create_copy(src);
    ci->flags = FLAG_VISIBLE;

    tx = ci->x;
    ty = ci->y;
    ty -= b;
    angle = atan(a);
    resx = tx * cos(2 * angle) + ty * sin(2 * angle);
    resy = -ty * cos(2 * angle) + tx * sin(2 * angle);
    ci->x = resx;
    ci->y = resy + b;

    layer->objects = eina_list_append(layer->objects, ci);

    append_undo_new_object(ci, CMD_SYNC, OBJ_CIRCLE, ci);
    msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);

    ci->owner = (Object *) drawing->current_layer;
    _ci_ssync(ci);
}

void
ci_mirror_y(Circle * src, double y0)
{
    Circle             *ci;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    ci = _circle_create_copy(src);
    ci->flags = FLAG_VISIBLE;

    ci->x = src->x;
    ci->y = y0 * 2 - src->y;

    layer->objects = eina_list_append(layer->objects, ci);

    append_undo_new_object(ci, CMD_SYNC, OBJ_CIRCLE, ci);
    msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);

    ci->owner = (Object *) drawing->current_layer;
    _ci_ssync(ci);
}

void
ci_mirror_x(Circle * src, double x0)
{
    Circle             *ci;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    ci = _circle_create_copy(src);
    ci->flags = FLAG_VISIBLE;

    ci->y = src->y;
    ci->x = x0 * 2 - src->x;

    layer->objects = eina_list_append(layer->objects, ci);

    append_undo_new_object(ci, CMD_SYNC, OBJ_CIRCLE, ci);
    msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);

    ci->owner = (Object *) drawing->current_layer;
    _ci_ssync(ci);
}

void
ci_trim_ab(Circle * ci, double a, double b, int side)
{
    if (!ci)
        return;
    if ((ci->y > ci->x * a + b) ^ (side))
        ci_delete(ci);
}

void
ci_trim_y(Circle * ci, double y, int side)
{
    if ((ci->y > y) ^ !side)
        ci_delete(ci);
}

void
ci_trim_x(Circle * ci, double x0, int side)
{
    if ((ci->x > x0) ^ !side)
        ci_delete(ci);
}

void
ci_delete(Circle * ci)
{
    int                 dummy;

    dummy = ci->flags;
    ci->flags |= FLAG_DELETED;
    append_undo_long((void *)&(ci->flags),
                    dummy, ci->flags, CMD_SYNC, OBJ_CIRCLE, ci);
    msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);
}

void
ci_cut(Circle * ci)
{
}

void
ci_copy(Circle * ci, int sock, double dx, double dy)
{
    CP_Header           hd;

    if (!ci)
        return;
    if (!ci->line_style)
        return;

    hd.type = OBJ_CIRCLE;
    hd.length = sizeof(Circle);

    ci->x -= dx;
    ci->y -= dy;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, ci, hd.length);

    ci->x += dx;
    ci->y += dy;

    hd.type = OBJ_NONE;
    hd.length = strlen(ci->line_style) + 1;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, ci->line_style, hd.length);
}

void
ci_paste(CP_Header hd, int sock, double dx, double dy)
{
    Circle             *ci, *src;
    void               *data;
    int                 res;

    if (hd.type != OBJ_CIRCLE)
        return;
    if (hd.length != sizeof(Circle))
        return;

    ci = _circle_create_copy(NULL);
    src = (Circle *) malloc(hd.length);
    res = read(sock, src, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "ci_paste: lack of bytes for Circle\n");
      }

    ci->flags = src->flags;
    ci->color = src->color;
    ci->x = src->x + dx;
    ci->y = src->y + dy;
    ci->r = src->r;
    ci->thickness = src->thickness;
    ci->line_scale = src->line_scale;

    read(sock, &hd, sizeof(hd));
    data = malloc(hd.length);
    res = read(sock, data, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "ci_paste: lack of bytes for line_style\n");
      }
    ci->line_style = data;
    if(ci->line_style && (strlen(ci->line_style)>4000))
	        ci->line_style[4000]=0;
    FREE(src);

    drawing->current_layer->objects =
        eina_list_append(drawing->current_layer->objects, ci);

    append_undo_new_object(ci, CMD_SYNC, OBJ_CIRCLE, ci);
    msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);

    ci->owner = (Object *) drawing->current_layer;
    ci_sync(ci);
    _ci_ssync(ci);
}

char              **
ci_get_info(Circle * ci)
{
    char              **p;
    char               *s;
    char                buf[4096];
    int                 pr;

    pr = shell->float_prec;

    p = (char **)malloc(sizeof(char *) * 10);
    memset(p, 0, sizeof(char *) * 10);

    s = _("Object type: circle");

    p[0] = DUP(s);

    sprintf(buf, _("X: %.*f"), pr, ci->x);
    p[1] = DUP(buf);

    sprintf(buf, _("Y: %.*f"), pr, ci->y);
    p[2] = DUP(buf);

    sprintf(buf, _("R: %.*f"), pr, ci->r);
    p[3] = DUP(buf);

    sprintf(buf, _("Line style: %s"), ci->line_style);
    p[4] = DUP(buf);

    sprintf(buf, _("Line style scale: %.*f"), pr, ci->line_scale);
    p[5] = DUP(buf);

    sprintf(buf, _("Line thickness: %.*f"), pr, ci->thickness);
    p[6] = DUP(buf);

    sprintf(buf, _("Owner: %s"), ((Layer *) ci->owner)->label);
    p[7] = DUP(buf);

    return p;
}

int
ci_select_by_click(Circle * ci, double x, double y)
{
    int                 dummy;
    double              epsilon, dist;

    if (ci->flags & FLAG_SELECTED)
        return;

    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;

    dist = (ci->x - x) * (ci->x - x) + (ci->y - y) * (ci->y - y);
    if (fabs(dist - ci->r * ci->r) > (epsilon * 16))
        return 0;

    dummy = ci->flags;
    ci->flags |= FLAG_SELECTED;
    append_undo_long((void *)&(ci->flags),
                    dummy, ci->flags, CMD_SYNC, OBJ_CIRCLE, ci);
    msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);
    return 1;
}

void
ci_select_by_rect(Circle * ci, double x, double y, double w, double h)
{
    int                 dummy;

    if (ci->flags & FLAG_SELECTED)
        return;

    if ((x + ci->r < ci->x) &&
        (x + w - ci->r > ci->x) &&
        (y + ci->r < ci->y) && (y + h - ci->r > ci->y))
      {
          dummy = ci->flags;
          ci->flags |= FLAG_SELECTED;
          append_undo_long((void *)&(ci->flags),
                          dummy, ci->flags, CMD_SYNC, OBJ_CIRCLE, ci);
          msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);
      }
}

int
ci_deselect_by_click(Circle * ci, double x, double y)
{
    int                 dummy;
    double              epsilon, dist;

    if (!(ci->flags & FLAG_SELECTED))
        return;

    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;

    dist = (ci->x - x) * (ci->x - x) + (ci->y - y) * (ci->y - y);
    if (fabs(dist - ci->r * ci->r) > (epsilon * 16))
        return 0;

    dummy = ci->flags;
    ci->flags ^= FLAG_SELECTED;
    append_undo_long((void *)&(ci->flags),
                    dummy, ci->flags, CMD_SYNC, OBJ_CIRCLE, ci);
    msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);
}

void
ci_deselect_by_rect(Circle * ci, double x, double y, double w, double h)
{
    int                 dummy;

    if (!(ci->flags & FLAG_SELECTED))
        return;

    if ((x + ci->r < ci->x) &&
        (x + w - ci->r > ci->x) &&
        (y + ci->r < ci->y) && (y + h - ci->r > ci->y))
      {
          dummy = ci->flags;
          ci->flags ^= FLAG_SELECTED;
          append_undo_long((void *)&(ci->flags),
                          dummy, ci->flags, CMD_SYNC, OBJ_CIRCLE, ci);
          msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);
      }
}

void
ghost_ci_create(void)
{
    Evas               *e;
    int                 i;
    static              init_flag = 0;

    e = shell->evas;
    for (i = 0; i < 20; i++)
      {
          if (!init_flag)
            {
                o_ci[i] = evas_object_line_add(e);
                evas_object_color_set(o_ci[i],
				ALPHA2,
				ALPHA2/5,
				ALPHA2/5,
				ALPHA2);
                evas_object_layer_set(o_ci[i], 17);
                evas_object_pass_events_set(o_ci[i], 1);
            }
          evas_object_show(o_ci[i]);
      }
    init_flag = 1;
}

void
ghost_ci_redraw(Eina_List *data, double x2, double y2)
{
    XY                 *xy;
    Evas               *e;
    Drawing            *d;
    double              x, y, r;
    int                 i;

    if (!data)
        return;
    e = shell->evas;
    d = drawing;
    if (!d)
        return;
    xy = (XY *) eina_list_last(data)->data;
    x = w2s_x(xy->x);
    y = w2s_y(xy->y);
    r = hypot(x2 - xy->x, y2 - xy->y);
    r = w2s_x(r) - w2s_x(0);

    for (i = 0; i < 20; i++)
      {
          evas_object_line_xy_set(o_ci[i], r * cos(M_PI / 10 * i) + x,
                           r * sin(M_PI / 10 * i) + y,
                           r * cos(M_PI / 10 * (1 + i)) + x,
                           r * sin(M_PI / 10 * (1 + i)) + y);

      }
}

void
ghost_ci_destroy(void)
{
    int                 i;

    for (i = 0; i < 20; i++)
      {
          evas_object_hide(o_ci[i]);
      }
}

void
_ci_check_evas_objects(Circle * ci)
{
    Eina_List          *l;
    int                 need_to_clear = 0;

    if (!ci->old.line_style || strcmp(ci->old.line_style, ci->line_style))
      {
          need_to_clear = 1;
          FREE(ci->old.line_style);
          ci->old.line_style = DUP(ci->line_style);
      }

    if (ci->thickness * shell->context.show_thickness
        != ci->old.thickness * shell->context.show_thickness)
      {
          need_to_clear = 1;
          ci->old.thickness = ci->thickness;
      }

    if (drawing->scale != drawing->old.scale)
      {
          need_to_clear = 1;
      }

    if (ci->flags != ci->old.flags)
      {
          need_to_clear = 1;
          ci->old.flags = ci->flags;
      }

    if ((ci->x != ci->old.x) || (ci->y != ci->old.y) || (ci->r != ci->old.r))
      {
          if ((ci->r != ci->r))
              need_to_clear = 1;
          ci->old.x = ci->x;
          ci->old.y = ci->y;
          ci->old.r = ci->r;
      }

    if (need_to_clear)
      {
          for (l = ci->list; l; l = l->next)
              evas_object_del(l->data);
          ci->list = eina_list_free(ci->list);
      }

    _ci_refresh_evas_objects(ci);
}

void
_ci_refresh_evas_objects(Circle * ci)
{

    linestyle_set(ci->line_style);
    if (linestyle_get_tiled())
      {
          _create_tiled_ci(ci);
      }
    else
      {
          _create_scaled_ci(ci);
      }
}

void
_create_scaled_ci(Circle * ci)
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

    len = 2 * M_PI * ci->r;
    tscale = len / tlen;

    flag = linestyle_get_odd();

    list = linestyle_get_points(len, ci->line_scale);

    trans_scale(list, 2 * M_PI / tlen, 2 * M_PI / tlen * ci->r);
    trans_move(list, 0, ci->r);

    e = shell->evas;

    lo = ci->list;
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
                                           ci->thickness *
                                           shell->context.
                                           show_thickness * drawing->scale);
                            evas_object_layer_set(o, 10);
                            evas_object_pass_events_set(o, 1);
                            ci->list = eina_list_append(ci->list, o);
                            lo = eina_list_last(ci->list);
                        }
                      o = lo->data;
                      lo = lo->next;
                      if (ci->flags & FLAG_VISIBLE)
                        {
                            evas_object_color_set(o,
				(ci->color.red*ci->color.alpha)/255,
				(ci->color.green*ci->color.alpha)/255,
				(ci->color.blue*ci->color.alpha)/255, 
				ci->color.alpha);
                        }
                      else
                        {
                            evas_object_color_set(o, 0, 0, 0, 0);
                        }
                      if (ci->flags & FLAG_SELECTED)
                          evas_object_color_set( o, 
					  ALPHA5, 
					  ALPHA5/5, 
					  ALPHA5/5, 
					  ALPHA5);
                      if (ci->flags & FLAG_DELETED)
                          evas_object_color_set( o, 0, 0, 0, 0);
                      x1 = w2s_x(rad * cos(angle) + ci->x);
                      y1 = w2s_y(rad * sin(angle) + ci->y);
                      x2 = w2s_x((rad + dr) * cos(angle + da) + ci->x);
                      y2 = w2s_y((rad + dr) * sin(angle + da) + ci->y);

                      _line_item_xy(e, o, x1, y1, x2, y2,
                                    ci->thickness *
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
_create_tiled_ci(Circle * ci)
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

    tlen = linestyle_get_tilelength();

    flag = linestyle_get_odd();
    len = 2 * M_PI * ci->r;

    list = linestyle_get_points(len, ci->line_scale);
    ENGY_ASSERT(list);

    trans_scale(list, 2 * M_PI / len, 2 * M_PI / len * ci->r);
    trans_move(list, 0, ci->r);

    e = shell->evas;

    lo = ci->list;

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
                                           ci->thickness *
                                           shell->context.
                                           show_thickness * drawing->scale);
                            evas_object_layer_set(o, 10);
                            evas_object_pass_events_set(o, 1);
                            ci->list = eina_list_append(ci->list, o);
                            lo = eina_list_last(ci->list);
                        }
                      o = lo->data;
                      lo = lo->next;
                      if (ci->flags & FLAG_VISIBLE)
                        {
                            evas_object_color_set(o,
				(ci->color.red*ci->color.alpha)/255,
                                (ci->color.green*ci->color.alpha)/255,
				(ci->color.blue*ci->color.alpha)/255, 
				ci->color.alpha);
                        }
                      else
                        {
                            evas_object_color_set(o, 0, 0, 0, 0);
                        }

                      if (ci->flags & FLAG_SELECTED)
                          evas_object_color_set( o, 
					  ALPHA5, 
					  ALPHA5/5, 
					  ALPHA5/5, 
					  ALPHA5);
                      if (ci->flags & FLAG_DELETED)
                          evas_object_color_set( o, 0, 0, 0, 0);
                      x1 = w2s_x(rad * cos(angle) + ci->x);
                      y1 = w2s_y(rad * sin(angle) + ci->y);
                      x2 = w2s_x((rad + dr) * cos(angle + da) + ci->x);
                      y2 = w2s_y((rad + dr) * sin(angle + da) + ci->y);

                      _line_item_xy(e, o, x1, y1, x2, y2,
                                    ci->thickness *
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
_ci_ssync(Circle * ci)
{
    char                buf[4096];
    Drawing            *d;
    Object             *o;

    d = drawing;
    o = ci->owner;

    sprintf(buf, "/layer/%d/entity/%d", o->id, ci->id);
    E_DB_INT_SET(d->fileout, buf, ci->type);
}

void
ci_load(int id)
{
    char               *f;
    char                buf[4096];
    Circle             *ci;
    int                 res;
    float               x;

    f = drawing->filein;

    ci = _circle_create_copy(NULL);
    ci->id = id;

    sprintf(buf, "/entity/%d/flags", id);
    E_DB_INT_GET(f, buf, ci->flags, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/red", id);
    E_DB_INT_GET(f, buf, ci->color.red, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/green", id);
    E_DB_INT_GET(f, buf, ci->color.green, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/blue", id);
    E_DB_INT_GET(f, buf, ci->color.blue, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/alpha", id);
    E_DB_INT_GET(f, buf, ci->color.alpha, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/x", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    ci->x = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/y", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    ci->y = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/r", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    ci->r = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/linescale", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    ci->line_scale = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/thickness", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    ci->thickness = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/linestyle", id);
    E_DB_STR_GET(f, buf, ci->line_style, res);
    ENGY_ASSERT(res);
    if(ci->line_style && (strlen(ci->line_style)>4000))
	        ci->line_style[4000]=0;

    drawing->current_layer->objects =
        eina_list_append(drawing->current_layer->objects, ci);

    append_undo_new_object(ci, CMD_SYNC, OBJ_CIRCLE, ci);
    msg_create_and_send(CMD_SYNC, OBJ_CIRCLE, ci);

    ci->owner = (Object *) drawing->current_layer;
}
