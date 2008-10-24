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

/* proto */
void                _line_create(double, double, double, double);
void                pre_line_x1y1(double, double);
void                _create_scaled_line(Line *);
void                _create_tiled_line(Line *);
void                line_ssync(Line *);

/* var */
Evas_Object        *o_line = NULL;

void
line_create(void)
{
    char               *s;
    float               x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    int                 res;

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

    serv_set_state(ST_LINE2);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
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
                return;
            }

          lock_data();
          _line_create(x1, y1, x2, y2);
          shell->context.fx = x2;
          shell->context.fy = y2;
          unlock_data();
          x1 = x2;
          y1 = y2;
      }
    while (1);
}

void
pre_line_x1y1(double x1, double y1)
{
    XY                 *xy;
    Eina_List          *list = NULL;

    xy = (XY *) malloc(sizeof(XY));
    ENGY_ASSERT(xy);

    xy->x = x1;
    xy->y = y1;
    list = eina_list_append(list, xy);

    msg_create_and_send(CMD_PRE_DATA, 0, list);
}

Line               *
_line_create_copy(Line * src)
{
    Line               *li;

    li = (Line *) malloc(sizeof(Line));
    ENGY_ASSERT(li);
    memset(li, 0, sizeof(Line));

    li->type = OBJ_LINE;
    li->id = rand() + my_int++;
    li->flags = FLAG_VISIBLE;

    li->old.color.red = -1;
    li->old.color.green = -1;
    li->old.color.blue = -1;
    li->old.color.alpha = -1;

    if (!src)
        return li;
    li->flags = li->flags;
    li->color = src->color;
    li->line_style = DUP(src->line_style);
    li->thickness = src->thickness;
    li->line_scale = src->line_scale;
    li->x1 = src->x1;
    li->y1 = src->y1;
    li->x2 = src->x2;
    li->y2 = src->y2;

    return li;
}

void
_line_create(double x1, double y1, double x2, double y2)
{
    Line               *line;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    line = _line_create_copy(NULL);

    line->color = shell->context.color;
    line->line_style = DUP(shell->context.line_style);
    line->thickness = shell->context.thickness;
    line->line_scale = shell->context.line_scale;
    line->x1 = x1;
    line->x2 = x2;
    line->y1 = y1;
    line->y2 = y2;

    layer->objects = eina_list_append(layer->objects, line);

    append_undo_new_object(line, CMD_SYNC, OBJ_LINE, line);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, line);

    line->owner = (Object *) drawing->current_layer;
    line_ssync(line);
}

void
_line_refresh_evas_objects(Line * line)
{
    int                 tiled;

    linestyle_set(line->line_style);
    if (line->flags & FLAG_HATCH_ITEM)
      {
          hatch_item_redraw(line);
          return;
      }
    tiled = linestyle_get_tiled();
    if (tiled)
      {
          _create_tiled_line(line);
      }
    else
      {
          _create_scaled_line(line);
      }
}

void
_line_check_evas_objects(Line * line)
{
    Evas               *e;
    Eina_List          *l;
    int                 need_to_clear = 0;

    e = shell->evas;

    if (!line->old.line_style || strcmp(line->old.line_style, line->line_style))
      {
          need_to_clear = 1;
          FREE(line->old.line_style);
          line->old.line_style = DUP(line->line_style);
      }

    if (line->thickness * shell->context.show_thickness
        != line->old.thickness * shell->context.show_thickness)
      {
          need_to_clear = 1;
          line->old.thickness = line->thickness;
      }

    if (drawing->scale != drawing->old.scale)
      {
          need_to_clear = 1;
      }

    if (line->line_scale != line->old.line_scale)
      {
          need_to_clear = 1;
          line->old.line_scale = line->line_scale;
      }

    if ((line->flags == line->old.flags) && (line->flags & FLAG_DELETED))
      {
          return;
      }

    if (line->flags != line->old.flags)
      {
          need_to_clear = 1;
          line->old.flags = line->flags;
      }

    if (hypot(line->x2 - line->x1, line->y2 - line->y1) !=
        hypot(line->old.x2 - line->old.x1, line->old.y2 - line->old.y1))
      {
          need_to_clear = 1;
          line->old.x1 = line->x1;
          line->old.y1 = line->y1;
          line->old.x2 = line->x2;
          line->old.y2 = line->y2;
      }

    if (need_to_clear)
      {
          for (l = line->list; l; l = l->next)
              evas_object_del(l->data);
          line->list = eina_list_free(line->list);
      }

    _line_refresh_evas_objects(line);

}

void
line_sync(Line * li)
{
    char                buf[4096];
    char               *f;

    f = drawing->fileout;

    if (li->color.red != li->old.color.red)
      {
          sprintf(buf, "/entity/%d/red", li->id);
          E_DB_INT_SET(f, buf, li->color.red);
      }

    if (li->color.green != li->old.color.green)
      {
          sprintf(buf, "/entity/%d/green", li->id);
          E_DB_INT_SET(f, buf, li->color.green);
      }

    if (li->color.blue != li->old.color.blue)
      {
          sprintf(buf, "/entity/%d/blue", li->id);
          E_DB_INT_SET(f, buf, li->color.blue);
      }
    if (li->color.alpha != li->old.color.alpha)
      {
          sprintf(buf, "/entity/%d/alpha", li->id);
          E_DB_INT_SET(f, buf, li->color.alpha);
      }

    if (li->flags != li->old.flags)
      {
          sprintf(buf, "/entity/%d/flags", li->id);
          E_DB_INT_SET(f, buf, li->flags);
      }
    if (!li->old.line_style || strcmp(li->old.line_style, li->line_style))
      {
          sprintf(buf, "/entity/%d/linestyle", li->id);
          E_DB_STR_SET(f, buf, li->line_style);
      }
    if (li->line_scale != li->old.line_scale)
      {
          sprintf(buf, "/entity/%d/linescale", li->id);
          E_DB_FLOAT_SET(f, buf, (float)li->line_scale);
      }
    if (li->thickness != li->old.thickness)
      {
          sprintf(buf, "/entity/%d/thickness", li->id);
          E_DB_FLOAT_SET(f, buf, (float)li->thickness);
      }

    if (li->x1 != li->old.x1)
      {
          sprintf(buf, "/entity/%d/x1", li->id);
          E_DB_FLOAT_SET(f, buf, (float)li->x1);
      }
    if (li->y1 != li->old.y1)
      {
          sprintf(buf, "/entity/%d/y1", li->id);
          E_DB_FLOAT_SET(f, buf, (float)li->y1);
      }
    if (li->x2 != li->old.x2)
      {
          sprintf(buf, "/entity/%d/x2", li->id);
          E_DB_FLOAT_SET(f, buf, (float)li->x2);
      }
    if (li->y2 != li->old.y2)
      {
          sprintf(buf, "/entity/%d/y2", li->id);
          E_DB_FLOAT_SET(f, buf, (float)li->y2);
      }

    line_redraw(li);
}

void
line_redraw(Line * line)
{

    if (!line)
        return;
    if (shell->psout);          /* TODO */
    if (!(line->flags & FLAG_HATCH_ITEM))
        magnet_attach(line);
    if (line->flags & FLAG_DELETED)
        magnet_detach(line);
    _line_check_evas_objects(line);
}

void
line_destroy(Line *line)
{
	Eina_List          *l;
	for (l = drawing->layers; l; l = l->next)
	{                                           
		Layer              *layer;
		
		layer = (Layer *) l->data;
		layer->objects = eina_list_remove(layer->objects, line);
	}

	line_free(line);
}

void
line_free(Line * line)
{
    Eina_List          *l;

    if (!line)
        return;

    magnet_detach(line);

    if (line->line_style)
        FREE(line->line_style);
    if (line->old.line_style)
        FREE(line->old.line_style);

    for (l = line->list; l; l = l->next)
        evas_object_del(l->data);
    line->list = eina_list_free(line->list);

    FREE(line);
}

void
line_move(Line * line, double dx, double dy)
{
    double              a, b, c, d;

    if (!line)
        return;
    a = line->x1;
    b = line->y1;
    c = line->x2;
    d = line->y2;
    line->x1 += dx;
    line->y1 += dy;
    line->x2 += dx;
    line->y2 += dy;
    append_undo_double((void *)(&(line->x1)),
                       a, line->x1, CMD_SYNC, OBJ_LINE, line);
    append_undo_double((void *)(&(line->y1)), b, line->y1, 0, 0, NULL);
    append_undo_double((void *)(&(line->x2)), c, line->x2, 0, 0, NULL);
    append_undo_double((void *)(&(line->y2)), d, line->y2, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, line);
}

void
line_rotate(Line * line, double x0, double y0, double angle)
{
    double              tx, ty;
    double              a, b, c, d;

    if (!line)
        return;
    a = line->x1;
    b = line->y1;
    c = line->x2;
    d = line->y2;

    angle = -angle;
    line->x1 -= x0;
    line->y1 -= y0;
    tx = line->x1 * cos(angle) + line->y1 * sin(angle);
    ty = line->y1 * cos(angle) - line->x1 * sin(angle);
    line->x1 = tx + x0;
    line->y1 = ty + y0;

    line->x2 -= x0;
    line->y2 -= y0;
    tx = line->x2 * cos(angle) + line->y2 * sin(angle);
    ty = line->y2 * cos(angle) - line->x2 * sin(angle);
    line->x2 = tx + x0;
    line->y2 = ty + y0;
    append_undo_double((void *)(&(line->x1)),
                       a, line->x1, CMD_SYNC, OBJ_LINE, line);
    append_undo_double((void *)(&(line->y1)), b, line->y1, 0, 0, NULL);
    append_undo_double((void *)(&(line->x2)), c, line->x2, 0, 0, NULL);
    append_undo_double((void *)(&(line->y2)), d, line->y2, 0, 0, NULL);

    msg_create_and_send(CMD_SYNC, OBJ_LINE, line);
}

void
line_scale(Line * line, double x0, double y0, double sc)
{
    double              a, b, c, d;

    if (!line)
        return;
    a = line->x1;
    b = line->y1;
    c = line->x2;
    d = line->y2;

    line->x1 -= x0;
    line->y1 -= y0;
    line->x1 *= sc;
    line->y1 *= sc;
    line->x1 += x0;
    line->y1 += y0;

    line->x2 -= x0;
    line->y2 -= y0;
    line->x2 *= sc;
    line->y2 *= sc;
    line->x2 += x0;
    line->y2 += y0;
    append_undo_double((void *)(&(line->x1)),
                       a, line->x1, CMD_SYNC, OBJ_LINE, line);
    append_undo_double((void *)(&(line->y1)), b, line->y1, 0, 0, NULL);
    append_undo_double((void *)(&(line->x2)), c, line->x2, 0, 0, NULL);
    append_undo_double((void *)(&(line->y2)), d, line->y2, 0, 0, NULL);

    msg_create_and_send(CMD_SYNC, OBJ_LINE, line);
}

void
line_scale_xy(Line * line, double x0, double y0, double scx, double scy)
{
    double              a, b, c, d;

    if (!line)
        return;
    a = line->x1;
    b = line->y1;
    c = line->x2;
    d = line->y2;

    line->x1 -= x0;
    line->y1 -= y0;
    line->x1 *= scx;
    line->y1 *= scy;
    line->x1 += x0;
    line->y1 += y0;

    line->x2 -= x0;
    line->y2 -= y0;
    line->x2 *= scx;
    line->y2 *= scy;
    line->x2 += x0;
    line->y2 += y0;
    append_undo_double((void *)(&(line->x1)),
                       a, line->x1, CMD_SYNC, OBJ_LINE, line);
    append_undo_double((void *)(&(line->y1)), b, line->y1, 0, 0, NULL);
    append_undo_double((void *)(&(line->x2)), c, line->x2, 0, 0, NULL);
    append_undo_double((void *)(&(line->y2)), d, line->y2, 0, 0, NULL);

    msg_create_and_send(CMD_SYNC, OBJ_LINE, line);
}

void
line_clone(Line * oldline, double dx, double dy)
{
    Line               *line;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    line = _line_create_copy(oldline);
    line->flags = FLAG_VISIBLE;

    line->x1 += dx;
    line->x2 += dx;
    line->y1 += dy;
    line->y2 += dy;

    layer->objects = eina_list_append(layer->objects, line);

    append_undo_new_object(line, CMD_SYNC, OBJ_LINE, line);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, line);

    line->owner = (Object *) drawing->current_layer;
    line_ssync(line);
}

void
line_array(Line * line, int nx, int ny, double dx, double dy)
{
    int                 i, j;

    if (!line)
        return;
    if (nx < 1)
        return;
    if (ny < 1)
        return;
    for (i = 0; i < nx; i++)
        for (j = 0; j < ny; j++)
            if (i || j)
                line_clone(line, i * dx, j * dy);
}

void
line_array_polar(Line * line, double x0, double y0, int num, double dalpha)
{
    int                 i;

    if (!line)
        return;
    if (num < 1)
        return;
    for (i = 1; i < num; i++)
      {
          line_clone(line, 0, 0);
          line_rotate(eina_list_last(drawing->current_layer->objects)->data,
                      x0, y0, i * dalpha);
      }
}

void
line_mirror_ab(Line * oldline, double a, double b)
{
    Line               *line;
    Drawing            *d;
    Layer              *layer;
    double              resx, resy, tx, ty, angle;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    line = _line_create_copy(oldline);
    line->flags = FLAG_VISIBLE;

    tx = oldline->x1;
    ty = oldline->y1;
    ty -= b;
    angle = atan(a);
    resx = tx * cos(2 * angle) + ty * sin(2 * angle);
    resy = -ty * cos(2 * angle) + tx * sin(2 * angle);
    line->x1 = resx;
    line->y1 = resy + b;

    tx = oldline->x2;
    ty = oldline->y2;
    ty -= b;
    resx = tx * cos(2 * angle) + ty * sin(2 * angle);
    resy = -ty * cos(2 * angle) + tx * sin(2 * angle);
    line->x2 = resx;
    line->y2 = resy + b;

    line->owner = (Object *) drawing->current_layer;
    layer->objects = eina_list_append(layer->objects, line);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, line);
    append_undo_new_object(line, CMD_SYNC, OBJ_LINE, line);
    line_ssync(line);
}

void
line_mirror_y(Line * oldline, double y0)
{
    Line               *line;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    line = _line_create_copy(oldline);

    line->x1 = oldline->x1;
    line->x2 = oldline->x2;
    line->y1 = 2 * y0 - oldline->y1;
    line->y2 = 2 * y0 - oldline->y2;
    line->owner = (Object *) drawing->current_layer;

    layer->objects = eina_list_append(layer->objects, line);
    append_undo_new_object(line, CMD_SYNC, OBJ_LINE, line);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, line);
    line_ssync(line);
}

void
line_mirror_x(Line * oldline, double x0)
{
    Line               *line;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    line = _line_create_copy(oldline);

    line->x1 = x0 * 2 - oldline->x1;
    line->x2 = x0 * 2 - oldline->x2;
    line->y1 = oldline->y1;
    line->y2 = oldline->y2;
    line->owner = (Object *) drawing->current_layer;

    layer->objects = eina_list_append(layer->objects, line);
    append_undo_new_object(line, CMD_SYNC, OBJ_LINE, line);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, line);
    line_ssync(line);
}

void
line_trim_ab(Line * l, double a0, double b0, int side)
{
    double              x0, y0;
    double              a, b;
    double              dx;
    double              tx1, tx2, ty1, ty2;

    tx1 = l->x1;
    tx2 = l->x2;
    ty1 = l->y1;
    ty2 = l->y2;

    dx = tx2 - tx1;
    if (dx == 0.0)
      {
          x0 = tx1;
          y0 = tx1 * a0 + b0;
      }
    else
      {
          a = (l->y2 - l->y1) / (dx);
          b = l->y1 - a * l->x1;
          if ((a == a0) && side)
            {
                line_delete(l);
                return;
            };
          if ((a == a0) && !side)
              return;
          x0 = (b0 - b) / (a - a0);
          y0 = a0 * x0 + b0;
      }

    if ((tx1 * a + b > tx1 * a0 + b0) && (tx2 * a + b > tx2 * a0 + b0) && side)
      {
          line_delete(l);
          return;
      }
    if ((tx1 * a + b > tx1 * a0 + b0) && (tx2 * a + b > tx2 * a0 + b0) && !side)
        return;

    if ((tx1 * a + b <= tx1 * a0 + b0) && (tx2 * a + b <= tx2 * a0 + b0)
        && !side)
      {
          line_delete(l);
          return;
      }
    if ((tx1 * a + b <= tx1 * a0 + b0) && (tx2 * a + b <= tx2 * a0 + b0)
        && side)
        return;

    if (((tx1 * a + b > tx1 * a0 + b0) && side) ||
        ((tx1 * a + b <= tx1 * a0 + b0) && !side))
      {
          append_undo_double((void *)(&(l->x1)),
                             l->x1, x0, CMD_SYNC, OBJ_LINE, l);
          append_undo_double((void *)(&(l->y1)), l->y1, y0, 0, 0, NULL);
          l->x1 = x0;
          l->y1 = y0;
      }
    if (((tx2 * a + b > tx2 * a0 + b0) && side) ||
        ((tx2 * a + b <= tx2 * a0 + b0) && !side))
      {
          append_undo_double((void *)(&(l->x2)),
                             l->x2, x0, CMD_SYNC, OBJ_LINE, l);
          append_undo_double((void *)(&(l->y2)), l->y2, y0, 0, 0, NULL);
          l->x2 = x0;
          l->y2 = y0;
      }
    msg_create_and_send(CMD_SYNC, OBJ_LINE, l);
}

void
line_trim_y(Line * l, double y0, int side)
{
    double              x0;
    double              old_x, old_y;

    if (!l)
        return;

    if (side)
      {
          if ((l->y1 >= y0) && (l->y2 >= y0))
            {
                line_delete(l);
                return;
            }
          if ((l->y1 <= y0) && (l->y2 <= y0))
              return;
      }
    else
      {
          if ((l->y1 <= y0) && (l->y2 <= y0))
            {
                line_delete(l);
                return;
            }
          if ((l->y1 >= y0) && (l->y2 >= y0))
              return;
      }

    x0 = (y0 - l->y1) / (l->x2 - l->x1) * (l->y2 - l->y1) + l->x1;

    if ((side && (l->y1 > l->y2)) || (!side && !(l->y1 > l->y2)))
      {
          old_x = l->x1;
          old_y = l->y1;
          l->x1 = x0;
          l->y1 = y0;
          append_undo_double((void *)(&(l->x1)),
                             old_x, l->x1, CMD_SYNC, OBJ_LINE, l);
          append_undo_double((void *)(&(l->y1)), old_y, l->y1, 0, 0, NULL);
      }
    else
      {
          old_x = l->x2;
          old_y = l->y2;
          l->x2 = x0;
          l->y2 = y0;
          append_undo_double((void *)(&(l->x2)),
                             old_x, l->x2, CMD_SYNC, OBJ_LINE, l);
          append_undo_double((void *)(&(l->y2)), old_y, l->y2, 0, 0, NULL);
      }
    msg_create_and_send(CMD_SYNC, OBJ_LINE, l);
}

void
line_trim_x(Line * l, double x0, int side)
{
    double              y0;
    double              old_y, old_x;

    if (!l)
        return;

    if (side)
      {
          if ((l->x1 >= x0) && (l->x2 >= x0))
            {
                line_delete(l);
                return;
            }
          if ((l->x1 <= x0) && (l->x2 <= x0))
              return;
      }
    else
      {
          if ((l->x1 <= x0) && (l->x2 <= x0))
            {
                line_delete(l);
                return;
            }
          if ((l->x1 >= x0) && (l->x2 >= x0))
              return;
      }

    y0 = (x0 - l->x1) / (l->y2 - l->y1) * (l->x2 - l->x1) + l->y1;

    if ((side && (l->x1 > l->x2)) || (!side && !(l->x1 > l->x2)))
      {
          old_y = l->y1;
          old_x = l->x1;
          l->y1 = y0;
          l->x1 = x0;
          append_undo_double((void *)(&(l->y1)),
                             old_y, l->y1, CMD_SYNC, OBJ_LINE, l);
          append_undo_double((void *)(&(l->x1)), old_x, l->x1, 0, 0, NULL);
      }
    else
      {
          old_y = l->y2;
          old_x = l->x2;
          l->y2 = y0;
          l->x2 = x0;
          append_undo_double((void *)(&(l->y2)),
                             old_y, l->y2, CMD_SYNC, OBJ_LINE, l);
          append_undo_double((void *)(&(l->x2)), old_x, l->x2, 0, 0, NULL);
      }
    msg_create_and_send(CMD_SYNC, OBJ_LINE, l);
}

void
line_delete(Line * line)
{
    int                 dummy;

    dummy = line->flags;
    line->flags |= FLAG_DELETED;
    append_undo_long((void *)&(line->flags),
                    dummy, line->flags, CMD_SYNC, OBJ_LINE, line);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, line);
}

void
line_cut(Line * line)
{
}

void
line_copy(Line * li, int sock, double dx, double dy)
{
    CP_Header           hd;

    if (!li)
        return;
    if (!li->line_style)
        return;

    hd.type = OBJ_LINE;
    hd.length = sizeof(Line);

    li->x1 -= dx;
    li->x2 -= dx;
    li->y1 -= dy;
    li->y2 -= dy;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, li, hd.length);

    li->x1 += dx;
    li->x2 += dx;
    li->y1 += dy;
    li->y2 += dy;

    hd.type = OBJ_NONE;
    hd.length = strlen(li->line_style) + 1;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, li->line_style, hd.length);
}

void
line_paste(CP_Header hd, int sock, double dx, double dy)
{
    Line               *li, *src;
    void               *data;
    int                 res;

    if (hd.type != OBJ_LINE)
        return;
    if (hd.length != sizeof(Line))
        return;

    li = _line_create_copy(NULL);
    src = (Line *) malloc(hd.length);
    res = read(sock, src, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "line_paste: lack of bytes for Line\n");
      }

    li->flags = src->flags;
    li->color = src->color;
    li->x1 = src->x1 + dx;
    li->y1 = src->y1 + dy;
    li->x2 = src->x2 + dx;
    li->y2 = src->y2 + dy;

    li->thickness = src->thickness;
    li->line_scale = src->line_scale;

    read(sock, &hd, sizeof(hd));
    data = malloc(hd.length);
    res = read(sock, data, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "line_paste: lack of bytes for line_style\n");
      }
    li->line_style = data;
    if(li->line_style && (strlen(li->line_style)>4000))
	                li->line_style[4000]=0;
    FREE(src);

    drawing->current_layer->objects =
        eina_list_append(drawing->current_layer->objects, li);

    append_undo_new_object(li, CMD_SYNC, OBJ_LINE, li);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, li);

    li->owner = (Object *) drawing->current_layer;
    line_ssync(li);
    line_sync(li);

}

char              **
line_get_info(Line * li)
{
    char              **p;
    char               *s;
    char                buf[4096];
    int                 pr;

    pr = shell->float_prec;

    p = (char **)malloc(sizeof(char *) * 10);
    memset(p, 0, sizeof(char *) * 10);

    if (li->flags & FLAG_HATCH_ITEM)
      {
          p[0] = DUP(_("Object type: hatch item"));
          return p;
      }

    s = _("Object type: line");
    if (li->x1 - li->x2 == 0)
        s = _("Object type: line (vertical)");
    if (li->y1 - li->y2 == 0)
        s = _("Object type: line (horizontal)");

    p[0] = DUP(s);

    sprintf(buf, _("X1: %.*f"), pr, li->x1);
    p[1] = DUP(buf);

    sprintf(buf, _("Y1: %.*f"), pr, li->y1);
    p[2] = DUP(buf);

    sprintf(buf, _("X2: %.*f"), pr, li->x2);
    p[3] = DUP(buf);

    sprintf(buf, _("Y2: %.*f"), pr, li->y2);
    p[4] = DUP(buf);

    sprintf(buf, _("Line style: %s"), li->line_style);
    p[5] = DUP(buf);

    sprintf(buf, _("Line style scale: %.*f"), pr, li->line_scale);
    p[6] = DUP(buf);

    sprintf(buf, _("Line thickness: %.*f"), pr, li->thickness);
    p[7] = DUP(buf);

    sprintf(buf, _("Owner: %s"), ((Layer *) li->owner)->label);
    p[8] = DUP(buf);

    return p;

}

/*********************** tools ***************************/

Evas_Object *
_line_item(Evas *e, double w)
{
    Evas_Object        *o;

    if (w > 1)
      {
          o = evas_object_polygon_add(e);
      }
    else
      {
          o = evas_object_line_add(e);
      }
    return o;
}

void
_line_item_xy(Evas *e,
              Evas_Object *o,
              double x1, double y1, double x2, double y2, double w)
{
    int                 sn;
    int                 i;
    int                 is_poly = 0;

    if ((x1 == x2) && (y1 == y2))
      {
          evas_object_hide(o);
          return;
      }

    is_poly = (w > 1);

    sn = shell->segnum;
    sn = (w < sn) ? w / 2 : sn;
    if (!sn)
        sn = 1;

    if (is_poly)
      {
          double              xx1, yy1, xx2, yy2, an, x, y;

          if (x2 > x1)
            {
                xx1 = x1;
                yy1 = y1;
                xx2 = x2;
                yy2 = y2;
            }
          else
            {
                xx1 = x2;
                yy1 = y2;
                xx2 = x1;
                yy2 = y1;
            }
          if (x2 - x1 != 0.0)
            {
                an = atan((y2 - y1) / (x2 - x1)) + M_PI / 2;
            }
          else
            {
                if (y2 - y1 <= 0)
                  {
                      an = M_PI;
                  }
                else
                  {
                      an = 0;
                  }
            }
          evas_object_polygon_points_clear(o);

          for (i = 0; i < sn + 1; i++)
            {
                x = xx1 + w / 2 * cos(an + i * M_PI / sn);
                y = yy1 + w / 2 * sin(an + i * M_PI / sn);
                evas_object_polygon_point_add(o, x, y);
            }
          for (i = sn; i >= 0; i--)
            {
                x = xx2 + w / 2 * cos(an - i * M_PI / sn);
                y = yy2 + w / 2 * sin(an - i * M_PI / sn);
                evas_object_polygon_point_add(o, x, y);
            }
      }
    else
      {
          evas_object_line_xy_set(o, x1, y1, x2, y2);
      }
}

void
_create_scaled_line(Line * line)
{
    Evas               *e;
    Evas_Object        *o;
    Eina_List          *list = NULL, *l, *lo;
    Drawing            *d;
    int                 flag, sign = 1;
    float               len, tlen, tscale;
    double              x1, y1, x2, y2;
    float               dx, dy, x, y;
    XY                 *xy;

    tlen = linestyle_get_tilelength();

    dx = line->x2 - line->x1;
    if (dx == 0)
        dx = 1e-40;
    sign = fabs(dx) / dx;
    dy = line->y2 - line->y1;
    len = hypot(dx, dy);
    tscale = len / tlen;

    flag = linestyle_get_odd();

    list = linestyle_get_points(len, line->line_scale);
    ENGY_ASSERT(list);
    if (!list)
        return;
    if (!list->next)
        return;

    trans_scale(list, tscale, tscale);
    trans_rotate(list, atan(-dy / dx) + M_PI * (sign - 1) / 2);
    trans_move(list, line->x1, line->y1);

    d = drawing;
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
                      evas_object_layer_set(o, 10);
                      evas_object_pass_events_set(o, 1);
                      line->list = eina_list_append(line->list, o);
                      lo = eina_list_last(line->list);
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
    list = eina_list_free(list);
}

void
_create_tiled_line(Line * line)
{
    int                 flag, sign = 1;
    Eina_List          *list = NULL, *l, *lo;
    Evas               *e;
    Evas_Object        *o;
    Drawing            *d;
    float               x, y, dx, dy, len;
    double              x1, y1, x2, y2;
    XY                 *xy;

    d = drawing;

    dx = line->x2 - line->x1;
    if (dx == 0)
        dx = 1e-40;             /* ok for 1200 dpi? */
    sign = fabs(dx) / dx;

    dy = line->y2 - line->y1;
    len = hypot(dx, dy);

    flag = linestyle_get_odd();

    list = linestyle_get_points(len, line->line_scale);
    ENGY_ASSERT(list);

    trans_rotate(list, atan(-dy / dx) + M_PI * (sign - 1) / 2);
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
                      evas_object_layer_set(o, 10);
                      evas_object_pass_events_set(o, 1);
                      line->list = eina_list_append(line->list, o);
                      lo = eina_list_last(line->list);
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
    list = eina_list_free(list);
}


int
line_select_by_click(Line * li, double _x, double _y)
{
    double              epsilon;
    double              dist;
    double              x0, y0;
    double a1, b1, t;
    int                 dummy;
    
    if (li->flags & FLAG_SELECTED)
	return 0;
    
    
    a1 = li->x2-li->x1;
    b1 = li->y2-li->y1;
    
    if(!(a1*a1+b1*b1)) return 0;
    
    t = a1*(_x - li->x1) + b1*(_y - li->y1);
    t /= (a1*a1+b1*b1);

    if( (t<0) || (t>1)) return 0;
    
    x0 = a1*t + li->x1;
    y0 = b1*t + li->y1;
    
    dist = (_x - x0) * (_x - x0) + (_y - y0) * (_y - y0);
    
    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;
    
    if (dist > epsilon)
	return 0;

    dummy = li->flags;
    li->flags |= FLAG_SELECTED;
    append_undo_long((void *)&(li->flags),
		    dummy, li->flags, CMD_SYNC, OBJ_LINE, li);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, li);
    return 1;
}

int
line_deselect_by_click(Line * li, double _x, double _y)
{
    double              a1, b1, t;
    double              epsilon;
    double              dist;
    double              x0, y0;
    int                 dummy;

    if (!(li->flags & FLAG_SELECTED))
	return 0;
    
    a1 = li->x2-li->x1;
    b1 = li->y2-li->y1;
    
    if(!(a1*a1+b1*b1)) return 0;
    
    t = a1*(_x - li->x1) + b1*(_y - li->y1);
    t /= (a1*a1+b1*b1);

    if( (t<0) || (t>1)) return 0;
    
    x0 = a1*t + li->x1;
    y0 = b1*t + li->y1;
    
    dist = (_x - x0) * (_x - x0) + (_y - y0) * (_y - y0);
    
    
    
    epsilon = shell->prec / drawing->scale;
    epsilon *= epsilon;
    
    
    if (dist > epsilon)
	return 0;
    
    
    
    li->flags ^= FLAG_SELECTED;
    append_undo_long((void *)&(li->flags),
		    dummy, li->flags, CMD_SYNC, OBJ_LINE, li);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, li);
    return 1;
}

void
line_select_by_rect(Line * line, double x, double y, double w, double h)
{
    int                 dummy;
    double              x1, y1, x2, y2;

    if (line->flags & FLAG_SELECTED)
        return;

    x1 = (line->x1 < line->x2) ? line->x1 : line->x2;
    x2 = (line->x1 >= line->x2) ? line->x1 : line->x2;
    y1 = (line->y1 < line->y2) ? line->y1 : line->y2;
    y2 = (line->y1 >= line->y2) ? line->y1 : line->y2;

    if ((x1 < x) || (x2 > x + w) || (y1 < y) || (y2 > y + h))
        return;

    dummy = line->flags;
    line->flags |= FLAG_SELECTED;
    append_undo_long((void *)&(line->flags),
                    dummy, line->flags, CMD_SYNC, OBJ_LINE, line);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, line);
}

void
line_deselect_by_rect(Line * line, double x, double y, double w, double h)
{
    int                 dummy;
    double              x1, y1, x2, y2;

    if (!(line->flags & FLAG_SELECTED))
        return;

    x1 = (line->x1 < line->x2) ? line->x1 : line->x2;
    x2 = (line->x1 >= line->x2) ? line->x1 : line->x2;
    y1 = (line->y1 < line->y2) ? line->y1 : line->y2;
    y2 = (line->y1 >= line->y2) ? line->y1 : line->y2;

    if ((x1 < x) || (x2 > x + w) || (y1 < y) || (y2 > y + h))
        return;

    dummy = line->flags;
    line->flags ^= FLAG_SELECTED;
    append_undo_long((void *)&(line->flags),
                    dummy, line->flags, CMD_SYNC, OBJ_LINE, line);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, line);
}

/** ghost line **/
void
ghost_line_create(void)
{
    Evas               *e;

    e = shell->evas;
    if (!o_line)
      {
          o_line = evas_object_line_add(e);
          evas_object_color_set(o_line, ALPHA2, ALPHA2/5, ALPHA2/5, ALPHA2);
          evas_object_layer_set(o_line, 17);
          evas_object_pass_events_set(o_line, 1);
      }
}

void
ghost_line_redraw(Eina_List *data, double x, double y)
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

    evas_object_show(o_line);
    evas_object_line_xy_set(o_line, x1, y1, x2, y2);
}

void
ghost_line_destroy(void)
{
    evas_object_hide(o_line);
}

void
line_ssync(Line * li)
{
    char                buf[4096];
    Drawing            *d;
    Object             *o;

    d = drawing;
    o = li->owner;

    sprintf(buf, "/layer/%d/entity/%d", o->id, li->id);
    E_DB_INT_SET(d->fileout, buf, li->type);

    sprintf(buf, "/entity/%d/hatchoffset", li->id);
    E_DB_FLOAT_SET(d->fileout, buf, li->hatch_offset);
}

void
line_load(int id)
{
    char               *f;
    char                buf[4096];
    Line               *li;
    int                 res;
    float               x;

    f = drawing->filein;

    li = _line_create_copy(NULL);
    li->id = id;

    sprintf(buf, "/entity/%d/flags", id);
    E_DB_INT_GET(f, buf, li->flags, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/red", id);
    E_DB_INT_GET(f, buf, li->color.red, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/green", id);
    E_DB_INT_GET(f, buf, li->color.green, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/blue", id);
    E_DB_INT_GET(f, buf, li->color.blue, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/alpha", id);
    E_DB_INT_GET(f, buf, li->color.alpha, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/x1", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    li->x1 = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/y1", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    li->y1 = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/x2", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    li->x2 = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/y2", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    li->y2 = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/linescale", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    li->line_scale = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/thickness", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    li->thickness = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/hatchoffset", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    li->hatch_offset = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/linestyle", id);
    E_DB_STR_GET(f, buf, li->line_style, res);
    ENGY_ASSERT(res);
    if(li->line_style && (strlen(li->line_style)>4000))
	                li->line_style[4000]=0;

    drawing->current_layer->objects =
        eina_list_append(drawing->current_layer->objects, li);

    append_undo_new_object(li, CMD_SYNC, OBJ_LINE, li);
    msg_create_and_send(CMD_SYNC, OBJ_LINE, li);

    li->owner = (Object *) drawing->current_layer;
}
