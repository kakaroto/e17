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
void                _point_create(double, double);
void                point_ssync(Point *);

void
point_create(void)
{
    char               *s;
    float               x, y;
    int                 res = 0;

    serv_set_state(ST_POINT);

    serv_set_hint(DUP(_("enter coordinates: ")));
    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x, &y);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_NORMAL);

    lock_data();
    _point_create(x, y);
    shell->context.fx = x;
    shell->context.fy = y;
    unlock_data();
}

Point              *
_point_create_copy(Point * src)
{
    Point              *po;
    int                 id;

    po = (Point *) malloc(sizeof(Point));
    ENGY_ASSERT(po);
    memset(po, 0, sizeof(Point));

    po->type = OBJ_POINT;
    id = rand() + my_int++;
    po->id = id;
    po->flags = FLAG_VISIBLE;
    po->old.color.red = -1;
    po->old.color.green = -1;
    po->old.color.blue = -1;
    po->old.color.alpha = -1;
    po->old.x = 1.0 * rand();
    po->old.y = 1.0 * rand();
    if (!src)
        return po;

    po->x = src->x;
    po->y = src->y;
    po->flags = src->flags;
    po->color = src->color;
    po->thickness = src->thickness;
    po->id = id;
/*    po->line_style = DUP(src->line_style);*/
    po->point_style = DUP(src->point_style);
    return po;
}

void
_point_create(double x, double y)
{
    Point              *point;
    Evas_List          *l;

    if (!drawing)
        return;
    if (!drawing->current_layer)
        return;

    point = _point_create_copy(NULL);
    point->color = shell->context.color;
/*    point->line_style = DUP(shell->context.line_style);*/
    point->thickness = shell->context.thickness;
    point->x = x;
    point->y = y;
    point->point_style = DUP(shell->context.point_style);

    drawing->current_layer->objects =
        evas_list_append(drawing->current_layer->objects, point);

    append_undo_new_object(point, CMD_SYNC, OBJ_POINT, point);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, point);

    point->owner = (Object *) drawing->current_layer;
    point_ssync(point);
}

void
create_evas_objects(Point * point)
{
    Evas               *e;
    Evas_Object        *o;
    char                buf[4096];
    int                 i, res, cnt;
    int         	x1, y1, x2, y2;

    e = shell->evas;
    sprintf(buf, "/pointstyle/%s/count", point->point_style);
    E_DB_INT_GET(shell->point_styles_file, buf, cnt, res);
    ENGY_ASSERT(res);
    for (i = 0; i < cnt; i++)
      {
          sprintf(buf, "/pointstyle/%s/%d/x1", point->point_style, i);
          E_DB_INT_GET(shell->point_styles_file, buf, x1, res);
          ENGY_ASSERT(res);
          sprintf(buf, "/pointstyle/%s/%d/y1", point->point_style, i);
          E_DB_INT_GET(shell->point_styles_file, buf, y1, res);
          ENGY_ASSERT(res);
          sprintf(buf, "/pointstyle/%s/%d/x2", point->point_style, i);
          E_DB_INT_GET(shell->point_styles_file, buf, x2, res);
          ENGY_ASSERT(res);
          sprintf(buf, "/pointstyle/%s/%d/y2", point->point_style, i);
          E_DB_INT_GET(shell->point_styles_file, buf, y2, res);
          ENGY_ASSERT(res);
          o = evas_object_line_add(e);
          evas_object_color_set(o, 
		point->color.red*point->color.alpha/255,
		point->color.green*point->color.alpha/255,
		point->color.blue*point->color.alpha/255, 
		point->color.alpha);
          evas_object_layer_set(o, 10);
          evas_object_data_set(o, "x1", (void *)(long)x1);
          evas_object_data_set(o, "y1", (void *)(long)y1);
          evas_object_data_set(o, "x2", (void *)(long)x2);
          evas_object_data_set(o, "y2", (void *)(long)y2);
          point->list = evas_list_append(point->list, o);
      }
}

void
_point_check_evas_objects(Point * point)
{
    Evas               *e;
    Evas_List          *l;
    static int          fake_point = 0;
    int                 need_to_clear = 0;

    e = shell->evas;

    if (fake_point)
      {
          need_to_clear = 1;
          fake_point = 0;
      }

    if (e != drawing->old.evas)
      {
          need_to_clear = 1;
          fake_point = 1;
      }

    if (!point->old.point_style ||
        (strcmp(point->point_style, point->old.point_style)))
        need_to_clear = 1;

    if (!need_to_clear)
        return;

    for (l = point->list; l; l = l->next)
        evas_object_del(l->data);
    point->list = evas_list_free(point->list);

    create_evas_objects(point);

    if (point->old.point_style)
        FREE(point->old.point_style);
    point->old.point_style = (char *)DUP(point->point_style);
}

void
point_sync(Point * point)
{
    char                buf[4096];
    char               *f;

    f = drawing->fileout;

    if (point->color.red != point->old.color.red)
      {
          sprintf(buf, "/entity/%d/red", point->id);
          E_DB_INT_SET(f, buf, point->color.red);
      }

    if (point->color.green != point->old.color.green)
      {
          sprintf(buf, "/entity/%d/green", point->id);
          E_DB_INT_SET(f, buf, point->color.green);
      }
    if (point->color.blue != point->old.color.blue)
      {
          sprintf(buf, "/entity/%d/blue", point->id);
          E_DB_INT_SET(f, buf, point->color.blue);
      }
    if (point->color.alpha != point->old.color.alpha)
      {
          sprintf(buf, "/entity/%d/alpha", point->id);
          E_DB_INT_SET(f, buf, point->color.alpha);
      }
    if (point->x != point->old.x)
      {
          sprintf(buf, "/entity/%d/x", point->id);
          E_DB_FLOAT_SET(f, buf, point->x);
      }
    if (point->y != point->old.y)
      {
          sprintf(buf, "/entity/%d/y", point->id);
          E_DB_FLOAT_SET(f, buf, point->y);
      }
    if (!point->old.point_style ||
        strcmp(point->point_style, point->old.point_style))
      {
          sprintf(buf, "/entity/%d/point_style", point->id);
          E_DB_STR_SET(f, buf, point->point_style);
      }

    if (point->flags != point->old.flags)
      {
          sprintf(buf, "/entity/%d/flags", point->id);
          E_DB_INT_SET(f, buf, point->flags);
      }
    point_redraw(point);
}

void
point_redraw(Point * point)
{
    Drawing            *d;
    Evas               *e;
    Evas_List          *l;
    Evas_Coord          x, y, w, h;
    Evas_Coord          x1, y1;

    if (!point)
        return;
    if (point->type != OBJ_POINT)
        return;

    if (shell->psout);          /* TODO */

    magnet_attach(point);
    if (point->flags & FLAG_DELETED)
        magnet_detach(point);
    _point_check_evas_objects(point);

    point->old.color = point->color;
    point->old.flags = point->flags;
    point->old.x = point->x;
    point->old.y = point->y;
    if (!point->old.point_style ||
        strcmp(point->old.point_style, point->point_style))
      {
          IF_FREE(point->old.point_style);
          point->old.point_style = DUP(point->point_style);
      }

    e = shell->evas;
    d = drawing;

    evas_output_viewport_get(e, &x, &y, &w, &h);

    x1 = (int)((point->x + d->x) * d->scale);
    y1 = (int)((point->y + d->y) * d->scale);

    for (l = point->list; l; l = l->next)
      {
          Evas_Object        *o;
          long                 a, b, c, d;

          o = l->data;
          a = (long)evas_object_data_get(o, "x1");
          b = (long)evas_object_data_get(o, "y1");
          c = (long)evas_object_data_get(o, "x2");
          d = (long)evas_object_data_get(o, "y2");
          evas_object_line_xy_set(o, x1 + a, y + h - y1 + b, x1 + c,
                           y + h - y1 + d);
          if (point->flags & FLAG_VISIBLE)
            {
                evas_object_color_set(o, 
			point->color.red*point->color.alpha/255,
			point->color.green*point->color.alpha/255,
			point->color.blue*point->color.alpha/255,
			point->color.alpha);
            }
          else
            {
                evas_object_color_set(o, 0, 0, 0, 0);
            }
          if (point->flags & FLAG_SELECTED)
              evas_object_color_set(o, ALPHA5, ALPHA5/5, ALPHA5/5, ALPHA5);
          if (point->flags & FLAG_DELETED)
              evas_object_color_set(o, 0, 0, 0, 0);
          evas_object_show(o);
      }
}

void 
point_destroy(Point * point)
{
	Evas_List          *l;
	for (l = drawing->layers; l; l = l->next)
	{
		Layer              *layer;
		
		layer = (Layer *) l->data;
		layer->objects = evas_list_remove(layer->objects, point);
	}

	point_free(point);
}

void
point_free(Point * point)
{
    Evas_List          *l;

    if (!point)
        return;

    magnet_detach(point);

    IF_FREE(point->line_style);
    FREE(point->point_style);

    for (l = point->list; l; l = l->next)
        evas_object_del(l->data);
    point->list = evas_list_free(point->list);
    FREE(point);
}

void
point_move(Point * po, double dx, double dy)
{
    double              a, b;

    if (!po)
        return;
    a = po->x;
    b = po->y;
    po->x += dx;
    po->y += dy;
    append_undo_double((void *)(&(po->x)), a, po->x, CMD_SYNC, OBJ_POINT, po);
    append_undo_double((void *)(&(po->y)), b, po->y, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, po);
}

void
point_scale(Point * po, double x0, double y0, double sc)
{
    double              a, b;

    if (!po)
        return;
    a = po->x;
    b = po->y;

    po->x -= x0;
    po->y -= y0;
    po->x *= sc;
    po->y *= sc;
    po->x += x0;
    po->y += y0;
    append_undo_double((void *)(&(po->x)), a, po->x, CMD_SYNC, OBJ_POINT, po);
    append_undo_double((void *)(&(po->y)), b, po->y, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, po);
}

void
point_scale_xy(Point * po, double x0, double y0, double xsc, double ysc)
{
    double              a, b;

    if (!po)
        return;
    a = po->x;
    b = po->y;

    po->x -= x0;
    po->y -= y0;
    po->x *= xsc;
    po->y *= ysc;
    po->x += x0;
    po->y += y0;
    append_undo_double((void *)(&(po->x)), a, po->x, CMD_SYNC, OBJ_POINT, po);
    append_undo_double((void *)(&(po->y)), b, po->y, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, po);
}
void
point_rotate(Point * po, double x0, double y0, double angle)
{
    double              tx, ty;
    double              a, b;

    if (!po)
        return;
    a = po->x;
    b = po->y;

    angle = -angle;
    po->x -= x0;
    po->y -= y0;
    tx = po->x * cos(angle) + po->y * sin(angle);
    ty = po->y * cos(angle) - po->x * sin(angle);
    po->x = tx + x0;
    po->y = ty + y0;
    append_undo_double((void *)(&(po->x)), a, po->x, CMD_SYNC, OBJ_POINT, po);
    append_undo_double((void *)(&(po->y)), b, po->y, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, po);
}

void
point_clone(Point * oldpo, double dx, double dy)
{
    Point              *point;
    Evas_List          *l;

    if (!drawing)
        return;
    if (!drawing->current_layer)
        return;

    point = _point_create_copy(oldpo);
    point->flags = FLAG_VISIBLE;
    point->x += dx;
    point->y += dy;

    drawing->current_layer->objects =
        evas_list_append(drawing->current_layer->objects, point);
    append_undo_new_object(point, CMD_SYNC, OBJ_POINT, point);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, point);

    point->owner = (Object *) drawing->current_layer;
    point_ssync(point);
}

void
point_array(Point * po, int nx, int ny, double dx, double dy)
{
    int                 i, j;

    if (!po)
        return;
    if (nx < 1)
        return;
    if (ny < 1)
        return;
    for (i = 0; i < nx; i++)
        for (j = 0; j < ny; j++)
            if (i || j)
                point_clone(po, i * dx, j * dy);
}

void
point_array_polar(Point * po, double x0, double y0, int num, double da)
{
    int                 i;

    if (!po)
        return;
    if (num < 1)
        return;
    for (i = 1; i < num; i++)
      {
          point_clone(po, 0, 0);
          point_rotate(evas_list_last(drawing->current_layer->objects)->data,
                       x0, y0, i * da);
      }
}

void
point_mirror_ab(Point * po, double a, double b)
{
    Point              *point;
    Evas_List          *l;
    double              angle, resx, resy, tx, ty;

    if (!drawing)
        return;
    if (!drawing->current_layer)
        return;

    point = _point_create_copy(po);

    point->flags = FLAG_VISIBLE;
    angle = atan(a);
    tx = po->x;
    ty = po->y;
    ty -= b;
    resx = tx * cos(2 * angle) + ty * sin(2 * angle);
    resy = -ty * cos(2 * angle) + tx * sin(2 * angle);
    point->x = resx;
    point->y = resy + b;

    drawing->current_layer->objects =
        evas_list_append(drawing->current_layer->objects, point);

    append_undo_new_object(point, CMD_SYNC, OBJ_POINT, point);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, point);

    point->owner = (Object *) drawing->current_layer;
    point_ssync(point);
}

void
point_mirror_y(Point * po, double y0)
{
    Point              *point;
    Evas_List          *l;
    double              angle, resx, resy;

    if (!drawing)
        return;
    if (!drawing->current_layer)
        return;

    point = _point_create_copy(po);

    point->flags = FLAG_VISIBLE;
    point->x = po->x;
    point->y = 2 * y0 - po->y;

    drawing->current_layer->objects =
        evas_list_append(drawing->current_layer->objects, point);

    append_undo_new_object(point, CMD_SYNC, OBJ_POINT, point);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, point);

    point->owner = (Object *) drawing->current_layer;
    point_ssync(point);
}

void
point_mirror_x(Point * po, double x0)
{
    Point              *point;
    Evas_List          *l;
    double              angle, resx, resy;

    if (!drawing)
        return;
    if (!drawing->current_layer)
        return;

    point = _point_create_copy(po);

    point->flags = FLAG_VISIBLE;
    point->x = 2 * x0 - po->x;
    point->y = po->y;

    drawing->current_layer->objects =
        evas_list_append(drawing->current_layer->objects, point);

    append_undo_new_object(point, CMD_SYNC, OBJ_POINT, point);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, point);

    point->owner = (Object *) drawing->current_layer;
    point_ssync(point);
}

void
point_trim_ab(Point * po, double a0, double b0, int side)
{

    if (!po)
        return;
    if ((po->y > po->x * a0 + b0) ^ (side))
        point_delete(po);
}

void
point_trim_y(Point * po, double y0, int side)
{
    if ((po->y > y0) ^ !side)
        point_delete(po);
}
void
point_trim_x(Point * po, double x0, int side)
{
    if ((po->x > x0) ^ !side)
        point_delete(po);
}

void
point_delete(Point * po)
{
    int                 dummy;

    if (!po)
        return;
    dummy = po->flags;
    po->flags |= FLAG_DELETED;
    append_undo_long((void *)&(po->flags),
                    dummy, po->flags, CMD_SYNC, OBJ_POINT, po);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, po);
}

void
point_cut(Point * po)
{
}

void
point_copy(Point * po, int sock, double dx, double dy)
{
    CP_Header           hd;

    if (!po)
        return;
    if (!po->point_style)
        return;

    hd.type = OBJ_POINT;
    hd.length = sizeof(Point);

    po->x -= dx;
    po->y -= dy;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, po, hd.length);

    po->x += dx;
    po->y += dy;

    hd.type = OBJ_NONE;
    hd.length = strlen(po->point_style) + 1;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, po->point_style, hd.length);
}

void
point_paste(CP_Header hd, int sock, double dx, double dy)
{
    Point              *po, *src;
    void               *data;
    int                 res;
    char *s;

    if (hd.type != OBJ_POINT)
        return;
    if (hd.length != sizeof(Point))
        return;

    po = _point_create_copy(NULL);
    src = (Point *) malloc(hd.length);
    res = read(sock, src, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "point_paste: lack of bytes for Point\n");
      }

    po->flags = src->flags;
    po->color = src->color;
    po->x = src->x + dx;
    po->y = src->y + dy;

/*    po->line_style = DUP(shell->context.line_style);*/
    po->thickness = shell->context.thickness;

    read(sock, &hd, sizeof(hd));
    data = malloc(hd.length);
    res = read(sock, data, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "point_paste: lack of bytes for point_style\n");
      }
    s = (char*)data;
    if(s && (strlen(s)>4000)) s[4000]=0;
    po->point_style = data;
    FREE(src);

    drawing->current_layer->objects =
        evas_list_append(drawing->current_layer->objects, po);

    append_undo_new_object(po, CMD_SYNC, OBJ_POINT, po);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, po);

    po->owner = (Object *) drawing->current_layer;
    point_ssync(po);
}

char              **
point_get_info(Point * po)
{
    char              **p;
    char                buf[4096];
    int                 pr;

    pr = shell->float_prec;

    p = (char **)malloc(sizeof(char *) * 10);
    memset(p, 0, sizeof(char *) * 10);

    p[0] = DUP(_("Object type: point"));

    sprintf(buf, _("X: %.*f"), pr, po->x);
    p[1] = DUP(buf);

    sprintf(buf, _("Y: %.*f"), pr, po->y);
    p[2] = DUP(buf);

    sprintf(buf, _("Point style: %s"), po->point_style);
    p[3] = DUP(buf);

    sprintf(buf, _("Owner: %s"), ((Layer *) po->owner)->label);
    p[4] = DUP(buf);

    return p;
}

int
point_select_by_click(Point * po, double x0, double y0)
{
    double              epsilon, dist;
    int                 dummy;

    if (po->flags & FLAG_SELECTED)
        return 0;
    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;
    dist = (po->x - x0) * (po->x - x0) + (po->y - y0) * (po->y - y0);
    if (dist > epsilon)
        return 0;

    dummy = po->flags;
    po->flags |= FLAG_SELECTED;
    append_undo_long((void *)&(po->flags),
                    dummy, po->flags, CMD_SYNC, OBJ_POINT, po);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, po);
}

void
point_select_by_rect(Point * po, double x, double y, double w, double h)
{
    int                 dummy;

    if (po->flags & FLAG_SELECTED)
        return;
    if ((po->x < x) || (po->x > x + w) || (po->y < y) || (po->y > y + h))
        return;
    dummy = po->flags;
    po->flags |= FLAG_SELECTED;
    append_undo_long((void *)&(po->flags),
                    dummy, po->flags, CMD_SYNC, OBJ_POINT, po);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, po);
}

int
point_deselect_by_click(Point * po, double x0, double y0)
{
    double              epsilon, dist;
    int                 dummy;

    if (!(po->flags & FLAG_SELECTED))
        return 0;
    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;
    dist = (po->x - x0) * (po->x - x0) + (po->y - y0) * (po->y - y0);
    if (dist > epsilon)
        return 0;

    dummy = po->flags;
    po->flags ^= FLAG_SELECTED;
    append_undo_long((void *)&(po->flags),
                    dummy, po->flags, CMD_SYNC, OBJ_POINT, po);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, po);
}

void
point_deselect_by_rect(Point * po, double x, double y, double w, double h)
{
    int                 dummy;

    if (!(po->flags & FLAG_SELECTED))
        return;
    if ((po->x < x) || (po->x > x + w) || (po->y < y) || (po->y > y + h))
        return;
    dummy = po->flags;
    po->flags ^= FLAG_SELECTED;
    append_undo_long((void *)&(po->flags),
                    dummy, po->flags, CMD_SYNC, OBJ_POINT, po);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, po);
}

void
point_ssync(Point * point)
{
    char                buf[4096];
    Drawing            *d;
    Object             *o;

    d = drawing;
    o = point->owner;

    sprintf(buf, "/layer/%d/entity/%d", o->id, point->id);
    E_DB_INT_SET(d->fileout, buf, point->type);
}

void
point_load(int id)
{
    char               *f;
    char                buf[4096];
    Point              *po;
    int                 res;
    float               x;

    f = drawing->filein;

    po = _point_create_copy(NULL);
    po->id = id;

    sprintf(buf, "/entity/%d/flags", id);
    E_DB_INT_GET(f, buf, po->flags, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/red", id);
    E_DB_INT_GET(f, buf, po->color.red, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/green", id);
    E_DB_INT_GET(f, buf, po->color.green, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/blue", id);
    E_DB_INT_GET(f, buf, po->color.blue, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/alpha", id);
    E_DB_INT_GET(f, buf, po->color.alpha, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/x", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    po->x = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/y", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    po->y = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/point_style", id);
    E_DB_STR_GET(f, buf, po->point_style, res);
    ENGY_ASSERT(res);
    if(po->point_style && (strlen(po->point_style)>4000))
	                po->point_style[4000]=0;

    drawing->current_layer->objects =
        evas_list_append(drawing->current_layer->objects, po);

    append_undo_new_object(po, CMD_SYNC, OBJ_POINT, po);
    msg_create_and_send(CMD_SYNC, OBJ_POINT, po);

    po->owner = (Object *) drawing->current_layer;
}
