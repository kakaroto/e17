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

void                _image_create(char *name, double, double, double, double);
void                _image_ssync(Image *);

void
image_create(void)
{
    char               *s;
    char               *name;
    float               x, y, sc, dummy;
    int                 res;

    serv_set_hint(DUP(_("enter image location: ")));
    serv_set_state(ST_IMAGE1);

    name = serv_get_string();

    serv_set_hint(DUP(_("enter insertion point: ")));

    serv_set_state(ST_LINE1);

    do
      {
          s = serv_get_string();
          res = get_values(s, shell->context.fx, shell->context.fy, &x, &y);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    serv_set_state(ST_IMAGE3);

    serv_set_hint(DUP(_("enter scale factor: ")));

    s = serv_get_string();
    res = get_values(s, x, y, &sc, &dummy);

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;

    lock_data();
    _image_create(name, x, y, sc, 0.0);
    shell->context.fx = x;
    shell->context.fy = y;
    unlock_data();
}

Image              *
_image_create_copy(Image * src)
{
    Image              *im;

    im = (Image *) malloc(sizeof(Image));
    ENGY_ASSERT(im);
    memset(im, 0, sizeof(Image));

    im->type = OBJ_IMAGE;
    im->id = rand() + my_int++;
    im->flags = FLAG_VISIBLE;

    im->old.color.red = -1;
    im->old.color.green = -1;
    im->old.color.blue = -1;
    im->old.color.alpha = -1;
    im->old.g = -1;
    im->old.x = -1;
    im->old.y = -1;

    if (!src)
        return im;

    im->flags = src->flags;
    im->color = src->color;
/*    im->line_style = DUP(src->line_style);*/
    im->thickness = src->thickness;
    im->line_scale = src->line_scale;
    im->x = src->x;
    im->y = src->y;
    im->g = src->g;
    im->s = src->s;
    im->gravity = src->gravity;
    im->w = src->w;
    im->h = src->h;
    im->rw = src->rw;
    im->rh = src->rh;

    {
        Imlib_Image         prev;

        prev = imlib_context_get_image();
        imlib_context_set_image(src->image);
        im->image = imlib_clone_image();
        imlib_context_set_image(prev);
    }

    return im;
}

void
_image_create(char *name, double x, double y, double s, double g)
{
    Imlib_Image         imi;
    Image              *im;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
      {
          return;
          FREE(name);
      }

    layer = d->current_layer;
    if (!layer)
      {
          return;
          FREE(name);
      }
    imi = imlib_load_image(name);
    ENGY_ASSERTS(imi, "image loading");
    if (!imi)
        return;

    im = _image_create_copy(NULL);

    im->color = shell->context.color;
    im->gravity = shell->context.gravity;
    im->x = x;
    im->y = y;
    im->s = s;
    im->g = g;
    im->image = imi;
    {
        Imlib_Image         prev;

        prev = imlib_context_get_image();
        imlib_context_set_image(imi);
        im->w = imlib_image_get_width();
        im->h = imlib_image_get_height();
    }

    layer->objects = evas_list_append(layer->objects, im);

    append_undo_new_object(im, CMD_SYNC, OBJ_IMAGE, im);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);

    im->owner = (Object *) drawing->current_layer;
    _image_ssync(im);

}

void
_refresh_evas_object(Image * im)
{
    Evas               *e;
    Evas_Object        *o;
    Imlib_Image         prev, rot;
    DATA32             *p;
    int                 w, h;

    if (im->item)
        return;
    e = shell->evas;

    prev = imlib_context_get_image();
    imlib_context_set_image(im->image);
    im->w = imlib_image_get_width();
    im->h = imlib_image_get_height();

    rot = imlib_create_rotated_image(-im->g);
    imlib_context_set_image(rot);

    w = imlib_image_get_width();
    h = imlib_image_get_height();

    p = imlib_image_get_data_for_reading_only();
    o = evas_object_image_add(e);
    evas_object_image_alpha_set(o, 1);
    evas_object_image_size_set(o, w, h);
    evas_object_resize(o, w, h);
    evas_object_image_fill_set(o, 0, 0, w, h);
    evas_object_image_data_copy_set(o, p);
    imlib_context_set_image(rot);
    imlib_free_image();
    imlib_context_set_image(prev);

    evas_object_layer_set(o, 10);
    evas_object_pass_events_set(o, 1);
    im->item = o;
    im->rw = w;
    im->rh = h;
}

void
image_redraw(Image * im)
{
    Evas               *e;
    int                 need_to_clear = 0;
    double              nw, nh, gx, gy;
    int                 i, j;

    e = shell->evas;

    if (shell->psout);          /* TODO */

    if (im->fake_evas)
      {
          need_to_clear = 1;
          im->fake_evas = 0;
      }

    if (drawing->old.evas != e)
      {
          need_to_clear = 1;
          im->fake_evas = 1;
      }

    if (im->flags != im->old.flags)
      {
          im->old.flags = im->flags;
      }
    if (im->g != im->old.g)
      {
          need_to_clear = 1;
          im->old.g = im->g;
      }

    if (im->s != im->old.s)
      {
          need_to_clear = 1;
          im->old.s = im->s;
      }

    if (need_to_clear)
      {
          evas_object_del(im->item);
          im->item = NULL;
      }

    _refresh_evas_object(im);
    ENGY_ASSERT(im->item);
    evas_object_show(im->item);

    i = 0;
    if (im->gravity & GRAVITY_L)
        i = 1;
    if (im->gravity & GRAVITY_R)
        i = -1;
    j = 0;
    if (im->gravity & GRAVITY_T)
        j = -1;
    if (im->gravity & GRAVITY_B)
        j = 1;

    gx = i * im->w / 2 * cos(-im->g) + j * im->h / 2 * sin(-im->g);
    gx = gx * im->s;
    gy = j * im->h / 2 * cos(-im->g) - i * im->w / 2 * sin(-im->g);
    gy = gy * im->s;

    nw = im->rw * im->s;
    nh = im->rh * im->s;

    evas_object_move(im->item,
              w2s_x(im->x - nw / 2 + gx), w2s_y(im->y + nh / 2 + gy));
    evas_object_image_fill_set(im->item, 0.0, 0.0,
                        nw * drawing->scale, nh * drawing->scale);
    evas_object_resize(im->item, nw * drawing->scale, nh * drawing->scale);
    if (im->flags & FLAG_VISIBLE)
      {
          evas_object_color_set(im->item,
		(255 - im->color.red) * im->color.alpha/255,
		(255 - im->color.green) * im->color.alpha/255,
		(255 - im->color.blue) * im->color.alpha/255,
		im->color.alpha);
      }
    else
      {
          evas_object_color_set(im->item, 0, 0, 0, 0);
      }
    if (im->flags & FLAG_SELECTED)
        evas_object_color_set(im->item, 150, 70, 70, 150);
    if (im->flags & FLAG_DELETED)
        evas_object_hide(im->item);
}

void
image_sync(Image * im)
{
    char                buf[4096];
    char               *f;

    f = drawing->fileout;

    if (im->color.red != im->old.color.red)
      {
          sprintf(buf, "/entity/%d/red", im->id);
          E_DB_INT_SET(f, buf, im->color.red);
      }

    if (im->color.green != im->old.color.green)
      {
          sprintf(buf, "/entity/%d/green", im->id);
          E_DB_INT_SET(f, buf, im->color.green);
      }

    if (im->color.blue != im->old.color.blue)
      {
          sprintf(buf, "/entity/%d/blue", im->id);
          E_DB_INT_SET(f, buf, im->color.blue);
      }
    if (im->color.alpha != im->old.color.alpha)
      {
          sprintf(buf, "/entity/%d/alpha", im->id);
          E_DB_INT_SET(f, buf, im->color.alpha);
      }

    if (im->flags != im->old.flags)
      {
          sprintf(buf, "/entity/%d/flags", im->id);
          E_DB_INT_SET(f, buf, im->flags);
      }
    if (im->s != im->old.s)
      {
          sprintf(buf, "/entity/%d/s", im->id);
          E_DB_FLOAT_SET(f, buf, (float)im->s);
      }
    if (im->g != im->old.g)
      {
          sprintf(buf, "/entity/%d/g", im->id);
          E_DB_FLOAT_SET(f, buf, (float)im->g);
      }

    if (im->x != im->old.x)
      {
          sprintf(buf, "/entity/%d/x", im->id);
          E_DB_FLOAT_SET(f, buf, (float)im->x);
      }
    if (im->y != im->old.y)
      {
          sprintf(buf, "/entity/%d/y", im->id);
          E_DB_FLOAT_SET(f, buf, (float)im->y);
      }
    if (im->gravity != im->old.gravity)
      {
          sprintf(buf, "/entity/%d/gravity", im->id);
          E_DB_INT_SET(f, buf, im->gravity);
      }
    image_redraw(im);
}

void
image_destroy(Image * im)
{    
	Evas_List          *l;
	for (l = drawing->layers; l; l = l->next)
	{                                           
		Layer              *layer;
		
		layer = (Layer *) l->data;
		layer->objects = evas_list_remove(layer->objects, im);
	}
	
	image_free(im);
}


void
image_free(Image * im)
{
    Imlib_Image         prev;

    if (!im)
        return;

    IF_FREE(im->line_style);

    prev = imlib_context_get_image();
    imlib_context_set_image(im->image);
    imlib_free_image();

    imlib_context_set_image(prev);

    evas_object_del(im->item);
    FREE(im);
}

void
image_move(Image * im, double dx, double dy)
{

    if (!im)
        return;

    append_undo_double((void *)(&(im->x)),
                       im->x, im->x + dx, CMD_SYNC, OBJ_IMAGE, im);
    append_undo_double((void *)(&(im->y)), im->y, im->y + dy, 0, 0, NULL);

    im->x += dx;
    im->y += dy;

    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);
}

void
image_scale(Image * im, double x0, double y0, double sc)
{
    double              a, b, c;

    if (!im)
        return;
    a = im->x;
    b = im->y;
    c = im->s;

    im->x -= x0;
    im->y -= y0;
    im->x *= sc;
    im->y *= sc;
    im->x += x0;
    im->y += y0;
    im->s *= sc;

    append_undo_double((void *)(&(im->x)), a, im->x, CMD_SYNC, OBJ_IMAGE, im);
    append_undo_double((void *)(&(im->y)), b, im->y, 0, 0, NULL);
    append_undo_double((void *)(&(im->s)), c, im->s, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);
}

void
image_scale_xy(Image * im, double x0, double y0, double scx, double scy)
{
    double              a, b, c;

    if (!im)
        return;
    a = im->x;
    b = im->y;

    im->x -= x0;
    im->y -= y0;
    im->x *= scx;
    im->y *= scy;
    im->x += x0;
    im->y += y0;
    im->s *= (scx > scy) ? scx : scy;

    append_undo_double((void *)(&(im->x)), a, im->x, CMD_SYNC, OBJ_IMAGE, im);
    append_undo_double((void *)(&(im->y)), b, im->y, 0, 0, NULL);
    append_undo_double((void *)(&(im->s)), c, im->s, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);
}

void
image_rotate(Image * im, double x0, double y0, double angle)
{
    double              a, b, c;
    double              tx, ty;

    if (!im)
        return;
    a = im->x;
    b = im->y;
    c = im->g;

    angle = -angle;
    im->x -= x0;
    im->y -= y0;
    tx = im->x * cos(angle) + im->y * sin(angle);
    ty = im->y * cos(angle) - im->x * sin(angle);
    im->x = tx + x0;
    im->y = ty + y0;
    im->g -= angle;

    append_undo_double((void *)(&(im->x)), a, im->x, CMD_SYNC, OBJ_IMAGE, im);
    append_undo_double((void *)(&(im->y)), b, im->y, 0, 0, NULL);
    append_undo_double((void *)(&(im->g)), c, im->g, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);
}

void
image_clone(Image * src, double dx, double dy)
{
    Image              *im;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    im = _image_create_copy(src);
    im->flags = FLAG_VISIBLE;

    im->x += dx;
    im->y += dy;

    layer->objects = evas_list_append(layer->objects, im);

    append_undo_new_object(im, CMD_SYNC, OBJ_IMAGE, im);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);

    im->owner = (Object *) drawing->current_layer;
    _image_ssync(im);
}

void
image_array(Image * im, int numx, int numy, double dx, double dy)
{
    int                 i, j;

    if (!im)
        return;
    if (numx < 1)
        return;
    if (numy < 1)
        return;
    for (i = 0; i < numx; i++)
        for (j = 0; j < numy; j++)
            if (i || j)
                image_clone(im, i * dx, j * dy);
}

void
image_array_polar(Image * im, double x0, double y0, int num, double da)
{
    int                 i;

    if (!im)
        return;
    if (num < 1)
        return;
    for (i = 1; i < num; i++)
      {
          image_clone(im, 0, 0);
          image_rotate(evas_list_last(drawing->current_layer->objects)->data,
                       x0, y0, i * da);
      }
}

void
image_mirror_ab(Image * src, double a, double b)
{
    Image              *im;
    Drawing            *d;
    Layer              *layer;
    double              resx, resy, tx, ty, angle;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    im = _image_create_copy(src);
    im->flags = FLAG_VISIBLE;

    tx = im->x;
    ty = im->y;
    ty -= b;
    angle = atan(a);
    resx = tx * cos(2 * angle) + ty * sin(2 * angle);
    resy = -ty * cos(2 * angle) + tx * sin(2 * angle);
    im->x = resx;
    im->y = resy + b;
    im->g += 2 * angle * 180 / M_PI;

    layer->objects = evas_list_append(layer->objects, im);

    append_undo_new_object(im, CMD_SYNC, OBJ_IMAGE, im);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);

    im->owner = (Object *) drawing->current_layer;
    _image_ssync(im);
}

void
image_mirror_y(Image * src, double y0)
{
    Image              *im;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    im = _image_create_copy(src);
    im->flags = FLAG_VISIBLE;

    im->x = src->x;
    im->y = y0 * 2 - src->y;
    im->g = 180 - src->g;

    layer->objects = evas_list_append(layer->objects, im);

    append_undo_new_object(im, CMD_SYNC, OBJ_IMAGE, im);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);

    im->owner = (Object *) drawing->current_layer;
    _image_ssync(im);
}

void
image_mirror_x(Image * src, double x0)
{
    Image              *im;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    im = _image_create_copy(src);
    im->flags = FLAG_VISIBLE;

    im->y = src->y;
    im->x = x0 * 2 - src->x;
    im->g = -src->g;

    layer->objects = evas_list_append(layer->objects, im);

    append_undo_new_object(im, CMD_SYNC, OBJ_IMAGE, im);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);

    im->owner = (Object *) drawing->current_layer;
    _image_ssync(im);
}

void
image_trim_ab(Image * im, double a, double b, int side)
{
    if (!im)
        return;
    if ((im->y > im->x * a + b) ^ (side))
        image_delete(im);
}

void
image_trim_y(Image * im, double y0, int side)
{
    if ((im->y > y0) ^ !side)
        image_delete(im);
}

void
image_trim_x(Image * im, double x0, int side)
{
    if ((im->x > x0) ^ !side)
        image_delete(im);
}

void
image_delete(Image * im)
{
    int                 dummy;

    dummy = im->flags;
    im->flags |= FLAG_DELETED;
    append_undo_long((void *)&(im->flags),
                    dummy, im->flags, CMD_SYNC, OBJ_IMAGE, im);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);
}

void
image_cut(Image * im)
{
}

void
image_copy(Image * im, int sock, double dx, double dy)
{
    CP_Header           hd;
    DATA32             *p;
    Imlib_Image         prev;

    if (!im)
        return;
    if (!im->image)
        return;

    hd.type = OBJ_IMAGE;
    hd.length = sizeof(Image);

    im->x -= dx;
    im->y -= dy;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, im, hd.length);

    im->x += dx;
    im->y += dy;

    hd.type = OBJ_NONE;
    hd.length = 4 * im->w * im->h;

    write(sock, &hd, sizeof(CP_Header));
    prev = imlib_context_get_image();
    imlib_context_set_image(im->image);
    p = imlib_image_get_data_for_reading_only();
    imlib_context_set_image(prev);

    write(sock, p, hd.length);
}

void
image_paste(CP_Header hd, int sock, double dx, double dy)
{
    Image              *im, *src;
    Imlib_Image         prev, curr;
    void               *data;
    DATA32             *p;
    int                 res;

    if (hd.type != OBJ_CIRCLE)
        return;
    if (hd.length != sizeof(Circle))
        return;

    im = _image_create_copy(NULL);
    src = (Image *) malloc(hd.length);
    res = read(sock, src, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "image_paste: lack of bytes for Image\n");
      }

    im->flags = src->flags;
    im->color = src->color;
    im->x = src->x + dx;
    im->y = src->y + dy;
    im->g = src->g;
    im->s = src->s;
    im->gravity = src->gravity;
    im->w = src->w;
    im->h = src->h;

    read(sock, &hd, sizeof(hd));
    data = malloc(hd.length);
    res = read(sock, data, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "image_paste: lack of bytes for image\n");
      }

    curr = imlib_create_image(im->w, im->h);
    prev = imlib_context_get_image();
    imlib_context_set_image(curr);
    imlib_image_set_has_alpha(1);
    imlib_context_set_image(curr);
    p = imlib_image_get_data();
    memcpy((void *)p, (void *)data, 4 * im->w * im->h);
    imlib_image_put_back_data(p);
    FREE(src);
    FREE(data);

    drawing->current_layer->objects =
        evas_list_append(drawing->current_layer->objects, im);

    append_undo_new_object(im, CMD_SYNC, OBJ_IMAGE, im);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);

    im->owner = (Object *) drawing->current_layer;
    image_sync(im);
    _image_ssync(im);
}

char              **
image_get_info(Image * im)
{
    char              **p;
    char               *s;
    char                buf[4096];
    int                 pr;

    pr = shell->float_prec;

    p = (char **)malloc(sizeof(char *) * 10);
    memset(p, 0, sizeof(char *) * 10);

    s = _("Object type: image");

    p[0] = DUP(s);

    sprintf(buf, _("X: %.*f"), pr, im->x);
    p[1] = DUP(buf);

    sprintf(buf, _("Y: %.*f"), pr, im->y);
    p[2] = DUP(buf);

    sprintf(buf, _("Scale: %.*f"), pr, im->s);
    p[3] = DUP(buf);

    sprintf(buf, _("Gamma: %.*f"), pr, im->g);
    p[4] = DUP(buf);

    sprintf(buf, _("Width in pixels: %d"), im->w);
    p[5] = DUP(buf);

    sprintf(buf, _("Height in pixels: %d"), im->h);
    p[6] = DUP(buf);

    sprintf(buf, _("Owner: %s"), ((Layer *) im->owner)->label);
    p[7] = DUP(buf);

    return p;
}

void
image_gravity(Image * im, int gr)
{

    if (!im)
        return;

    append_undo_long((void *)&(im->gravity),
                    im->gravity, gr, CMD_SYNC, OBJ_IMAGE, im);
    im->gravity = gr;
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);
}

void
image_image_scale(Image * im, double s)
{

    if (!im)
        return;

    append_undo_double((void *)&(im->s), im->s, s, CMD_SYNC, OBJ_IMAGE, im);
    im->s = s;
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);
}

int
image_select_by_click(Image * im, double x, double y)
{
    int                 dummy;
    int                 i, j;
    double              epsilon;

    if ((im->flags & FLAG_SELECTED))
        return 0;

    epsilon = shell->prec / drawing->scale * 5;
    if (fabs(x - im->x) > epsilon)
        return 0;
    if (fabs(y - im->y) > epsilon)
        return 0;

    dummy = im->flags;
    im->flags |= FLAG_SELECTED;
    append_undo_long((void *)&(im->flags),
                    dummy, im->flags, CMD_SYNC, OBJ_IMAGE, im);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);
    return 1;
}

void
image_select_by_rect(Image * im, double x, double y, double w, double h)
{
    int                 dummy;
    int                 i, j;

    if ((im->flags & FLAG_SELECTED))
        return;

    if (im->x < x)
        return;
    if (im->x > x + w)
        return;
    if (im->y < y)
        return;
    if (im->y > y + h)
        return;

    dummy = im->flags;
    im->flags |= FLAG_SELECTED;
    append_undo_long((void *)&(im->flags),
                    dummy, im->flags, CMD_SYNC, OBJ_IMAGE, im);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);
}

int
image_deselect_by_click(Image * im, double x, double y)
{
    int                 dummy;
    int                 i, j;
    double              epsilon;

    if (!(im->flags & FLAG_SELECTED))
        return 0;

    epsilon = shell->prec / drawing->scale * 5;
    if (fabs(x - im->x) > epsilon)
        return 0;
    if (fabs(y - im->y) > epsilon)
        return 0;

    dummy = im->flags;
    im->flags ^= FLAG_SELECTED;
    append_undo_long((void *)&(im->flags),
                    dummy, im->flags, CMD_SYNC, OBJ_IMAGE, im);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);
    return 1;
}

void
image_deselect_by_rect(Image * im, double x, double y, double w, double h)
{
    int                 dummy;
    int                 i, j;

    if (!(im->flags & FLAG_SELECTED))
        return;

    if (im->x < x)
        return;
    if (im->x > x + w)
        return;
    if (im->y < y)
        return;
    if (im->y > y + h)
        return;

    dummy = im->flags;
    im->flags ^= FLAG_SELECTED;
    append_undo_long((void *)&(im->flags),
                    dummy, im->flags, CMD_SYNC, OBJ_IMAGE, im);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);
}

void
_image_ssync(Image * im)
{
    char                buf[4096];
    Drawing            *d;
    Object             *o;
    Imlib_Image         prev;

    d = drawing;
    o = im->owner;

    sprintf(buf, "/layer/%d/entity/%d", o->id, im->id);
    E_DB_INT_SET(d->fileout, buf, im->type);

    {
        E_DB_File          *edb = NULL;
        int                 num, i;
        void               *p;
        Imlib_Image         prev;

        edb = e_db_open_read(drawing->filein);
        ENGY_ASSERT(edb);

        prev = imlib_context_get_image();

        imlib_context_set_image(im->image);
        p = imlib_image_get_data_for_reading_only();

        sprintf(buf, "/entity/%d/imagedata", im->id);
        e_db_data_set(edb, buf, p, im->w * im->h * 4);
        e_db_close(edb);

        imlib_context_set_image(prev);

        sprintf(buf, "/entity/%d/w", im->id);
        E_DB_INT_SET(d->fileout, buf, im->w);

        sprintf(buf, "/entity/%d/h", im->id);
        E_DB_INT_SET(d->fileout, buf, im->h);
    }
}

void
image_load(int id)
{
    char               *f;
    char                buf[4096];
    Image              *im;
    int                 res;
    float               x;

    f = drawing->filein;

    im = _image_create_copy(NULL);
    im->id = id;

    sprintf(buf, "/entity/%d/flags", id);
    E_DB_INT_GET(f, buf, im->flags, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/gravity", id);
    E_DB_INT_GET(f, buf, im->gravity, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/red", id);
    E_DB_INT_GET(f, buf, im->color.red, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/green", id);
    E_DB_INT_GET(f, buf, im->color.green, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/blue", id);
    E_DB_INT_GET(f, buf, im->color.blue, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/alpha", id);
    E_DB_INT_GET(f, buf, im->color.alpha, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/w", id);
    E_DB_INT_GET(f, buf, im->w, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/h", id);
    E_DB_INT_GET(f, buf, im->h, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/x", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    im->x = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/y", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    im->y = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/s", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    im->s = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/g", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    im->g = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/imagedata", im->id);
    {
        E_DB_File          *edb = NULL;
        int                 num, i;
        void               *p;

        edb = e_db_open_read(drawing->filein);
        ENGY_ASSERT(edb);
        p = e_db_data_get(edb, buf, &num);
        im->image = imlib_create_image_using_data(im->w, im->h, p);
        e_db_close(edb);
    }
    ENGY_ASSERT(im->image);

    drawing->current_layer->objects =
        evas_list_append(drawing->current_layer->objects, im);

    append_undo_new_object(im, CMD_SYNC, OBJ_IMAGE, im);
    msg_create_and_send(CMD_SYNC, OBJ_IMAGE, im);

    im->owner = (Object *) drawing->current_layer;
}
