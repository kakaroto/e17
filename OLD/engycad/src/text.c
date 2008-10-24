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

void                _text_create(char *, double, double, double, double);
void                _text_ssync(Text *);

void
text_create(void)
{
    char               *s;
    char               *text;
    float               x, y, h, dummy, x2, y2, angle;
    int                 res, res1;

    serv_set_hint(DUP(_("enter text: ")));
    serv_set_state(ST_TEXT1);

    text = serv_get_string();

    serv_set_state(ST_TEXT2);

    serv_set_hint(DUP(_("enter text height: ")));

    s = serv_get_string();
    res = get_values(s, x, y, &h, &dummy);

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

    serv_set_state(ST_ROTATE2);
    pre_set_x1y1(x, y);

    serv_set_hint(DUP(_("enter angle value: ")));

    serv_set_state(ST_ROTATE2);
    do
      {
          s = serv_get_string();
          x2 = 0;
          res = get_values(s, x, y, &x2, &y2);
          if (res == 2)
            {
                double              dx, dy;

                dx = x2 - x;
                dy = y2 - y;
                res1 = 1;
                if ((dy > 0) && (dx != 0))
                    angle = atan(dy / dx) * 180 / M_PI;
                if ((dy > 0) && (dx == 0))
                    angle = 90;
                if ((dy == 0) && (dx > 0))
                    angle = 0;
                if ((dy == 0) && (dx == 0))
                    angle = 0;
                if ((dy == 0) && (dx < 0))
                    angle = 180;
                if ((dy < 0) && (dx > 0))
                    angle = atan(dy / dx) * 180.0 / M_PI + 180;
                if ((dy < 0) && (dx == 0))
                    angle = -90;
                if ((dy < 0) && (dx < 0))
                    angle = atan(dy / dx) * 180 / M_PI - 180;
            }
          if (res == 1)
            {
                angle = x2;
                res1 = 1;
            }
          if (res == 0)
            {
                serv_set_state(ST_NORMAL);
                return;
            }
          if (res1 != 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res1 != 1);

    serv_set_state(ST_NORMAL);

    if (res == 0)
        return;

    lock_data();
    _text_create(text, x, y, h, angle);
    shell->context.fx = x;
    shell->context.fy = y;
    unlock_data();
}

Text               *
_text_create_copy(Text * src)
{
    Text               *te;

    te = (Text *) malloc(sizeof(Text));
    ENGY_ASSERT(te);
    memset(te, 0, sizeof(Text));

    te->type = OBJ_TEXT;
    te->flags = FLAG_VISIBLE;
    te->id = rand() + my_int++;
    te->gravity = GRAVITY_M | GRAVITY_C;
    te->old.color.red = -1;
    te->old.color.green = -1;
    te->old.color.blue = -1;
    te->old.color.alpha = -1;
    te->old.g = -1;
    if (!src)
        return te;

    te->flags = src->flags;
    te->color = src->color;
    te->line_style = (src->line_style) ? DUP(src->line_style) : NULL;
    te->thickness = src->thickness;
    te->x = src->x;
    te->y = src->y;
    te->h = src->h;
    te->g = src->g;
    te->gravity = src->gravity;
    te->text = (src->text) ? DUP(src->text) : NULL;
    te->textstyle = (src->textstyle) ? DUP(src->textstyle) : NULL;

    return te;
}

void
_text_create(char *text, double x, double y, double h, double g)
{
    Text               *te;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
      {
          return;
          FREE(text);
      }

    layer = d->current_layer;
    if (!layer)
      {
          return;
          FREE(text);
      }

    te = _text_create_copy(NULL);

    te->type = OBJ_TEXT;
    te->flags = FLAG_VISIBLE;
    te->color.red = shell->context.color.red;
    te->color.green = shell->context.color.green;
    te->color.blue = shell->context.color.blue;
    te->color.alpha = shell->context.color.alpha;
    te->gravity = shell->context.gravity;
    te->textstyle = DUP(shell->context.textstyle);
    te->x = x;
    te->y = y;
    te->h = h;
    te->g = g;
    te->text = text;

    layer->objects = eina_list_append(layer->objects, te);

    append_undo_new_object(te, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);

    te->owner = (Object *) drawing->current_layer;
    _text_ssync(te);

}

void
_refresh_te_evas_object(Text * te)
{
    Evas               *e;
    Evas_Object        *o;
    Imlib_Image         im_prev, im_curr, im_sk, rot;
    Imlib_Font          fo_prev, fo_curr;
    DATA32             *p;
    int                 w, h, fo_h;
    int                 a, d;
    char                buf[4096];
    char               *dec_s;
    double              corr;

    if (!drawing)
        return;
    if (te->item)
        return;

    e = shell->evas;

    im_prev = imlib_context_get_image();
    fo_prev = imlib_context_get_font();

    textstyle_set(te->textstyle);

    fo_h = (int)((double)shell->monitor /
                 unit(UNIT_INCH) * unit(drawing->unit) * te->h);
    sprintf(buf, "%s/%d", textstyle_get_font(), fo_h);
    fo_curr = imlib_load_font(buf);
    ENGY_ASSERT(fo_curr);

    imlib_context_set_font(fo_curr);
    imlib_context_set_TTF_encoding(textstyle_get_charmap());
    dec_s = my_iconv(textstyle_get_dcd(), te->text);
    imlib_get_text_size(dec_s, &w, &h);
    a = imlib_get_maximum_font_ascent();
    d = imlib_get_maximum_font_descent();

    corr = 1;
    im_curr = imlib_create_image(w, h);

    imlib_context_set_image(im_curr);
    imlib_image_set_has_alpha(1);
    imlib_image_clear();

    imlib_context_set_color(255, 255, 255, 255);

    imlib_context_set_image(im_curr);
    imlib_text_draw(0, 0, dec_s);
    FREE(dec_s);
    imlib_context_set_TTF_encoding(shell->charmap);

    te->im_w = w + ((double)h * corr * sin(textstyle_get_skew()));
    te->im_h = (corr * h);

    im_sk = imlib_create_image(te->im_w, te->im_h);
    imlib_context_set_image(im_sk);
    imlib_image_set_has_alpha(1);
    imlib_image_clear();

    imlib_context_set_image(im_sk);
    imlib_blend_image_onto_image_skewed(im_curr, 1,
                                        0, 0, w, h, te->im_w - w, 0,
                                        (int)(w * textstyle_get_wscale()),
                                        0, w - te->im_w, te->im_h);

    if (te->g != 0)
      {
          rot = imlib_create_rotated_image(-te->g / 180 * M_PI);
      }
    else
      {
          rot = imlib_clone_image();
      }

    imlib_context_set_image(rot);
    te->rot_w = imlib_image_get_width();
    te->rot_h = imlib_image_get_height();

    p = imlib_image_get_data_for_reading_only();

    o = evas_object_image_add(e);
    evas_object_image_alpha_set(o, 1);
    evas_object_image_fill_set(o, 0, 0, te->rot_w, te->rot_h);
    evas_object_image_size_set(o, te->rot_w, te->rot_h);
    evas_object_image_data_copy_set(o, p);
    evas_object_resize(o, te->rot_w, te->rot_h);
    evas_object_layer_set(o, 10);
    evas_object_pass_events_set(o, 1);

    imlib_context_set_image(im_sk);
    imlib_free_image();
    imlib_context_set_image(im_curr);
    imlib_free_image();
    imlib_context_set_image(rot);
    imlib_free_image();
    imlib_free_font();

    imlib_context_set_image(im_prev);
    imlib_context_set_font(fo_prev);

    te->item = o;
}

void
text_redraw(Text * te)
{
    Evas               *e;
    int                 need_to_clear = 0;
    double              gx, gy, corr;
    int                 i, j;

    e = shell->evas;

    if (shell->psout);          /* TODO */

    if (te->fake_evas)
      {
          need_to_clear = 1;
          te->fake_evas = 0;
      }

    if (drawing->old.evas != e)
      {
          need_to_clear = 1;
          te->fake_evas = 1;
      }

    if (te->g != te->old.g)
      {
          need_to_clear = 1;
          te->old.g = te->g;
      }

    if (te->h != te->old.h)
      {
          need_to_clear = 1;
          te->old.h = te->h;
      }

    if (!te->old.text || strcmp(te->old.text, te->text))
      {
          need_to_clear = 1;
          IF_FREE(te->old.text);
          te->old.text = DUP(te->text);
      }

    if (!te->old.textstyle || strcmp(te->old.textstyle, te->textstyle))
      {
          need_to_clear = 1;
          IF_FREE(te->old.textstyle);
          te->old.textstyle = DUP(te->textstyle);
      }

    if (need_to_clear)
      {
          evas_object_del(te->item);
          te->item = NULL;
      }

    _refresh_te_evas_object(te);
    ENGY_ASSERT(te->item);
    evas_object_show(te->item);

    i = 0;
    if (te->gravity & GRAVITY_L)
        i = -1;
    if (te->gravity & GRAVITY_R)
        i = 1;
    j = 0;
    if (te->gravity & GRAVITY_T)
        j = -1;
    if (te->gravity & GRAVITY_B)
        j = 1;

    corr = shell->monitor / unit(UNIT_INCH) * unit(drawing->unit);

    gx = i * te->im_w / 2 * cos(-te->g / 180 * M_PI) -
        j * te->im_h / 2 * sin(-te->g / 180 * M_PI);
    gx *= 1 /*drawing->scale */  / corr;
    gy = j * te->im_h / 2 * cos(-te->g / 180 * M_PI) +
        i * te->im_w / 2 * sin(-te->g / 180 * M_PI);
    gy *= 1 /*drawing->scale */  / corr;

    evas_object_resize(te->item,
                te->rot_w * drawing->scale / corr,
                te->rot_h * drawing->scale / corr);
    evas_object_image_fill_set(te->item, 0, 0,
                        te->rot_w * drawing->scale / corr,
                        te->rot_h * drawing->scale / corr);

    evas_object_move(te->item,
              w2s_x(te->x - te->rot_w / 2 / corr - gx),
              w2s_y(te->y + te->rot_h / 2 / corr + gy));

    if (te->flags & FLAG_VISIBLE)
      {
          evas_object_color_set(te->item,
		te->color.red*te->color.alpha/255,
		te->color.green*te->color.alpha/255, 
		te->color.blue*te->color.alpha/255, 
		te->color.alpha);
      }
    else
      {
          evas_object_color_set(te->item, 0, 0, 0, 0);
      }

    if (te->flags & FLAG_SELECTED)
        evas_object_color_set(te->item, 255, 50, 50, 255);
    if (te->flags & FLAG_DELETED)
        evas_object_hide(te->item);
}

void
text_sync(Text * te)
{
    char                buf[4096];
    char               *f;

    f = drawing->fileout;

    if (te->color.red != te->old.color.red)
      {
          sprintf(buf, "/entity/%d/red", te->id);
          E_DB_INT_SET(f, buf, te->color.red);
      }

    if (te->color.green != te->old.color.green)
      {
          sprintf(buf, "/entity/%d/green", te->id);
          E_DB_INT_SET(f, buf, te->color.green);
      }

    if (te->color.blue != te->old.color.blue)
      {
          sprintf(buf, "/entity/%d/blue", te->id);
          E_DB_INT_SET(f, buf, te->color.blue);
      }
    if (te->color.alpha != te->old.color.alpha)
      {
          sprintf(buf, "/entity/%d/alpha", te->id);
          E_DB_INT_SET(f, buf, te->color.alpha);
      }

    if (te->flags != te->old.flags)
      {
          sprintf(buf, "/entity/%d/flags", te->id);
          E_DB_INT_SET(f, buf, te->flags);
      }
    if (!te->old.textstyle || strcmp(te->old.textstyle, te->textstyle))
      {
          sprintf(buf, "/entity/%d/textstyle", te->id);
          E_DB_STR_SET(f, buf, te->textstyle);
      }
    if (!te->old.text || strcmp(te->old.text, te->text))
      {
          sprintf(buf, "/entity/%d/text", te->id);
          E_DB_STR_SET(f, buf, te->text);
      }
    if (te->gravity != te->old.gravity)
      {
          sprintf(buf, "/entity/%d/gravity", te->id);
          E_DB_INT_SET(f, buf, (float)te->gravity);
      }
    if (te->g != te->old.g)
      {
          sprintf(buf, "/entity/%d/g", te->id);
          E_DB_FLOAT_SET(f, buf, te->g);
      }

    if (te->x != te->old.x)
      {
          sprintf(buf, "/entity/%d/x", te->id);
          E_DB_FLOAT_SET(f, buf, (float)te->x);
      }
    if (te->y != te->old.y)
      {
          sprintf(buf, "/entity/%d/y", te->id);
          E_DB_FLOAT_SET(f, buf, (float)te->y);
      }
    if (te->h != te->old.h)
      {
          sprintf(buf, "/entity/%d/h", te->id);
          E_DB_FLOAT_SET(f, buf, (float)te->h);
      }
    text_redraw(te);
}

void
text_destroy(Text * te)
{
    Eina_List          *l;

    for (l = drawing->layers; l; l = l->next)
      {
          Layer              *layer;

          layer = (Layer *) l->data;
          layer->objects = eina_list_remove(layer->objects, te);
      }

    text_free(te);
}


void
text_free(Text * te)
{
    Eina_List          *l;

    if (!te)
        return;

    magnet_detach(te);

    IF_FREE(te->line_style);
    IF_FREE(te->textstyle);
    IF_FREE(te->old.textstyle);
    IF_FREE(te->text);
    IF_FREE(te->old.text);

    evas_object_del(te->item);

    FREE(te);
}

void
text_move(Text * te, double dx, double dy)
{

    if (!te)
        return;

    append_undo_double((void *)(&(te->x)),
                       te->x, te->x + dx, CMD_SYNC, OBJ_TEXT, te);
    append_undo_double((void *)(&(te->y)), te->y, te->y + dy, 0, 0, NULL);

    te->x += dx;
    te->y += dy;

    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);
}

void
text_scale(Text * te, double x0, double y0, double sc)
{
    double              a, b, c;

    if (!te)
        return;
    a = te->x;
    b = te->y;
    c = te->h;

    te->x -= x0;
    te->y -= y0;
    te->x *= sc;
    te->y *= sc;
    te->x += x0;
    te->y += y0;
    te->h *= sc;

    append_undo_double((void *)(&(te->x)), a, te->x, CMD_SYNC, OBJ_TEXT, te);
    append_undo_double((void *)(&(te->y)), b, te->y, 0, 0, NULL);
    append_undo_double((void *)(&(te->h)), c, te->h, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);
}

void
text_scale_xy(Text * te, double x0, double y0, double scx, double scy)
{
    double              a, b, c;

    if (!te)
        return;
    a = te->x;
    b = te->y;
    c = te->h;

    te->x -= x0;
    te->y -= y0;
    te->x *= scx;
    te->y *= scy;
    te->x += x0;
    te->y += y0;
    te->h *= (scx > scy) ? scx : scy;

    append_undo_double((void *)(&(te->x)), a, te->x, CMD_SYNC, OBJ_TEXT, te);
    append_undo_double((void *)(&(te->y)), b, te->y, 0, 0, NULL);
    append_undo_double((void *)(&(te->h)), c, te->h, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);
}

void
text_rotate(Text * te, double x0, double y0, double angle)
{
    double              a, b, c;
    double              tx, ty;

    if (!te)
        return;
    a = te->x;
    b = te->y;
    c = te->g;

    angle = -angle;
    te->x -= x0;
    te->y -= y0;
    tx = te->x * cos(angle) + te->y * sin(angle);
    ty = te->y * cos(angle) - te->x * sin(angle);
    te->x = tx + x0;
    te->y = ty + y0;
    te->g -= angle / M_PI * 180;

    append_undo_double((void *)(&(te->x)), a, te->x, CMD_SYNC, OBJ_TEXT, te);
    append_undo_double((void *)(&(te->y)), b, te->y, 0, 0, NULL);
    append_undo_double((void *)(&(te->g)), c, te->g, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);

}

void
text_clone(Text * src, double dx, double dy)
{
    Text               *te;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    te = _text_create_copy(src);
    te->flags = FLAG_VISIBLE;

    te->x += dx;
    te->y += dy;

    layer->objects = eina_list_append(layer->objects, te);

    append_undo_new_object(te, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);

    te->owner = (Object *) drawing->current_layer;
    _text_ssync(te);
}

void
text_array(Text * te, int numx, int numy, double dx, double dy)
{
    int                 i, j;

    if (!te)
        return;
    if (numx < 1)
        return;
    if (numy < 1)
        return;
    for (i = 0; i < numx; i++)
        for (j = 0; j < numy; j++)
            if (i || j)
                text_clone(te, i * dx, j * dy);
}

void
text_array_polar(Text * te, double x0, double y0, int num, double da)
{
    int                 i;

    if (!te)
        return;
    if (num < 1)
        return;
    for (i = 1; i < num; i++)
      {
          text_clone(te, 0, 0);
          text_rotate(eina_list_last(drawing->current_layer->objects)->data,
                      x0, y0, i * da);
      }
}

void
text_mirror_ab(Text * src, double a, double b)
{
    Text               *te;
    Drawing            *d;
    Layer              *layer;
    double              resx, resy, tx, ty, angle;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    te = _text_create_copy(src);
    te->flags = FLAG_VISIBLE;

    tx = te->x;
    ty = te->y;
    ty -= b;
    angle = atan(a);
    resx = tx * cos(2 * angle) + ty * sin(2 * angle);
    resy = -ty * cos(2 * angle) + tx * sin(2 * angle);
    te->x = resx;
    te->y = resy + b;
    te->g += 2 * angle * 180 / M_PI;

    layer->objects = eina_list_append(layer->objects, te);

    append_undo_new_object(te, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);

    te->owner = (Object *) drawing->current_layer;
    _text_ssync(te);
}

void
text_mirror_y(Text * src, double y0)
{
    Text               *te;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    te = _text_create_copy(src);
    te->flags = FLAG_VISIBLE;

    te->x = src->x;
    te->y = y0 * 2 - src->y;
    te->g = 180 - src->g;

    layer->objects = eina_list_append(layer->objects, te);

    append_undo_new_object(te, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);

    te->owner = (Object *) drawing->current_layer;
    _text_ssync(te);
}

void
text_mirror_x(Text * src, double x0)
{
    Text               *te;
    Drawing            *d;
    Layer              *layer;

    d = drawing;
    if (!d)
        return;
    layer = d->current_layer;
    if (!layer)
        return;

    te = _text_create_copy(src);
    te->flags = FLAG_VISIBLE;

    te->y = src->y;
    te->x = x0 * 2 - src->x;
    te->g = -src->g;

    layer->objects = eina_list_append(layer->objects, te);

    append_undo_new_object(te, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);

    te->owner = (Object *) drawing->current_layer;
    _text_ssync(te);
}

void
text_trim_ab(Text * te, double a, double b, int side)
{
    if (!te)
        return;
    if ((te->y > te->x * a + b) ^ (side))
        text_delete(te);
}

void
text_trim_y(Text * te, double y0, int side)
{
    if ((te->y > y0) ^ !side)
        text_delete(te);
}
void
text_trim_x(Text * te, double x0, int side)
{
    if ((te->x > x0) ^ !side)
        text_delete(te);
}

void
text_delete(Text * te)
{
    int                 dummy;

    dummy = te->flags;
    te->flags |= FLAG_DELETED;
    append_undo_long((void *)&(te->flags),
                    dummy, te->flags, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);
}
void
text_cut(Text * te)
{
}

void
text_copy(Text * te, int sock, double dx, double dy)
{
    CP_Header           hd;

    if (!te)
        return;
    if (!te->textstyle)
        return;
    if (!te->text)
        return;

    hd.type = OBJ_TEXT;
    hd.length = sizeof(Text);

    te->x -= dx;
    te->y -= dy;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, te, hd.length);

    te->x += dx;
    te->y += dy;

    hd.type = OBJ_NONE;
    hd.length = strlen(te->textstyle) + 1;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, te->textstyle, hd.length);

    hd.type = OBJ_NONE;
    hd.length = strlen(te->text) + 1;

    write(sock, &hd, sizeof(CP_Header));
    write(sock, te->text, hd.length);
}

void
text_paste(CP_Header hd, int sock, double dx, double dy)
{
    Text               *te, *src;
    void               *data;
    int                 res;
    char *s;

    if (hd.type != OBJ_TEXT)
        return;
    if (hd.length != sizeof(Text))
        return;

    te = _text_create_copy(NULL);
    src = (Text *) malloc(hd.length);
    res = read(sock, src, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "te_paste: lack of bytes for Text\n");
      }

    te->flags = src->flags;
    te->color = src->color;
    te->x = src->x + dx;
    te->y = src->y + dy;
    te->g = src->g;
    te->h = src->h;
    te->gravity = te->gravity;

    read(sock, &hd, sizeof(hd));
    data = malloc(hd.length);
    res = read(sock, data, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "text_paste: lack of bytes for text_style\n");
      }
    s = (char*)data;
    if(s && (strlen(s)>4000)) s[4000]=0;
    te->textstyle = s;

    read(sock, &hd, sizeof(hd));
    data = malloc(hd.length);
    res = read(sock, data, hd.length);
    if (res != hd.length)
      {
          fprintf(stderr, "text_paste: lack of bytes for text\n");
      }
    s = (char*)data;
    if(s && (strlen(s)>4000)) s[4000]=0;
    te->text = s;

    FREE(src);

    drawing->current_layer->objects =
        eina_list_append(drawing->current_layer->objects, te);

    append_undo_new_object(te, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);

    te->owner = (Object *) drawing->current_layer;
    _text_ssync(te);
    text_sync(te);
}

void
text_gravity(Text * te, int gr)
{

    if (!te)
        return;

    append_undo_long((void *)&(te->gravity),
                    te->gravity, gr, CMD_SYNC, OBJ_TEXT, te);
    te->gravity = gr;
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);

}

void
text_text(Text * te, char *s)
{

    if (!te)
        return;

    append_undo_ptr((void *)&(te->text), te->text, s, CMD_SYNC, OBJ_TEXT, te);
    te->text = s;
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);

}

void
text_text_style(Text * te, char *s)
{

    if (!te)
        return;

    append_undo_ptr((void *)&(te->textstyle),
                    te->textstyle, s, CMD_SYNC, OBJ_TEXT, te);
    te->textstyle = s;
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);

}

void
text_text_height(Text * te, double h)
{

    if (!te)
        return;

    append_undo_double((void *)&(te->h), te->h, h, CMD_SYNC, OBJ_TEXT, te);
    te->h = h;
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);
}

char              **
text_get_info(Text * te)
{
    char              **p;
    char               *s;
    char                buf[4096];
    int                 pr;

    pr = shell->float_prec;

    p = (char **)malloc(sizeof(char *) * 10);
    memset(p, 0, sizeof(char *) * 10);

    s = _("Object type: text");

    p[0] = DUP(s);

    sprintf(buf, _("X: %.*f"), pr, te->x);
    p[1] = DUP(buf);

    sprintf(buf, _("Y: %.*f"), pr, te->y);
    p[2] = DUP(buf);

    sprintf(buf, _("Height: %.*f"), pr, te->h);
    p[3] = DUP(buf);

    sprintf(buf, _("Text style: %s"), te->textstyle);
    p[4] = DUP(buf);

    sprintf(buf, _("Text angle: %.*f"), pr, te->g);
    p[5] = DUP(buf);

    sprintf(buf, _("Text is:"));
    p[6] = DUP(buf);

    s = my_iconv(shell->dcd, te->text);
    sprintf(buf, ("%s"), s);
    p[7] = DUP(buf);
    FREE(s);

    sprintf(buf, _("Owner: %s"), ((Layer *) te->owner)->label);
    p[8] = DUP(buf);

    return p;
}

int
text_select_by_click(Text * te, double x, double y)
{
    int                 dummy;
    int                 i, j;
    double              epsilon;

    if ((te->flags & FLAG_SELECTED))
        return 0;

    epsilon = shell->prec / drawing->scale * 5;
    if (fabs(x - te->x) > epsilon)
        return 0;
    if (fabs(y - te->y) > epsilon)
        return 0;

    dummy = te->flags;
    te->flags |= FLAG_SELECTED;
    append_undo_long((void *)&(te->flags),
                    dummy, te->flags, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);
    return 1;
}

void
text_select_by_rect(Text * te, double x, double y, double w, double h)
{
    int                 dummy;
    int                 i, j;

    if ((te->flags & FLAG_SELECTED))
        return;

    if (te->x < x)
        return;
    if (te->x > x + w)
        return;
    if (te->y < y)
        return;
    if (te->y > y + h)
        return;

    dummy = te->flags;
    te->flags |= FLAG_SELECTED;
    append_undo_long((void *)&(te->flags),
                    dummy, te->flags, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);
}

int
text_deselect_by_click(Text * te, double x, double y)
{
    int                 dummy;
    int                 i, j;
    double              epsilon;

    if (!(te->flags & FLAG_SELECTED))
        return 0;

    epsilon = shell->prec / drawing->scale * 5;
    if (fabs(x - te->x) > epsilon)
        return 0;
    if (fabs(y - te->y) > epsilon)
        return 0;

    dummy = te->flags;
    te->flags ^= FLAG_SELECTED;
    append_undo_long((void *)&(te->flags),
                    dummy, te->flags, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);
    return 1;
}

void
text_deselect_by_rect(Text * te, double x, double y, double w, double h)
{
    int                 dummy;
    int                 i, j;

    if (!(te->flags & FLAG_SELECTED))
        return;

    if (te->x < x)
        return;
    if (te->x > x + w)
        return;
    if (te->y < y)
        return;
    if (te->y > y + h)
        return;

    dummy = te->flags;
    te->flags ^= FLAG_SELECTED;
    append_undo_long((void *)&(te->flags),
                    dummy, te->flags, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);
}

void
_text_ssync(Text * te)
{
    char                buf[4096];
    Drawing            *d;
    Object             *o;

    d = drawing;
    o = te->owner;

    sprintf(buf, "/layer/%d/entity/%d", o->id, te->id);
    E_DB_INT_SET(d->fileout, buf, te->type);
}

void
text_load(int id)
{
    char               *f;
    char                buf[4096];
    Text               *te;
    int                 res;
    float               x;

    f = drawing->filein;

    te = _text_create_copy(NULL);
    te->id = id;

    sprintf(buf, "/entity/%d/flags", id);
    E_DB_INT_GET(f, buf, te->flags, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/gravity", id);
    E_DB_INT_GET(f, buf, te->gravity, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/red", id);
    E_DB_INT_GET(f, buf, te->color.red, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/green", id);
    E_DB_INT_GET(f, buf, te->color.green, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/blue", id);
    E_DB_INT_GET(f, buf, te->color.blue, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/alpha", id);
    E_DB_INT_GET(f, buf, te->color.alpha, res);
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/x", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    te->x = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/y", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    te->y = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/h", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    te->h = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/g", id);
    E_DB_FLOAT_GET(f, buf, x, res);
    te->g = x;
    ENGY_ASSERT(res);

    sprintf(buf, "/entity/%d/textstyle", id);
    E_DB_STR_GET(f, buf, te->textstyle, res);
    ENGY_ASSERT(res);
    if(te->textstyle && (strlen(te->textstyle)>4000))
	                te->textstyle[4000]=0;

    sprintf(buf, "/entity/%d/text", id);
    E_DB_STR_GET(f, buf, te->text, res);
    ENGY_ASSERT(res);
    if(te->text && (strlen(te->text)>4000)) te->text[4000]=0;

    drawing->current_layer->objects =
        eina_list_append(drawing->current_layer->objects, te);

    append_undo_new_object(te, CMD_SYNC, OBJ_TEXT, te);
    msg_create_and_send(CMD_SYNC, OBJ_TEXT, te);

    te->owner = (Object *) drawing->current_layer;
}
