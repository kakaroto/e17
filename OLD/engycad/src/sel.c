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

/** vars **/
Evas_Object         *sel_box, *sel1, *sel2, *sel3, *sel4;

/* protos */
void                _sel_rect(int, double, double, double, double);
void                pre_sel_x1y1(double, double);

int
sel_click_layer(int mode, Layer * layer, double _x, double _y)
{
    Eina_List          *l;
    Object             *obj;
    int                 flag = 0;

    for (l = layer->objects; l; l = l->next)
      {
          obj = (Object *) l->data;
          if (mode)
            {
                switch (obj->type)
                  {
                  case OBJ_LINE:
                      flag |= line_deselect_by_click((Line *) obj, _x, _y);
                      break;
                  case OBJ_POINT:
                      flag |= point_deselect_by_click((Point *) obj, _x, _y);
                      break;

                  case OBJ_CIRCLE:
                      flag |= ci_deselect_by_click((Circle *) obj, _x, _y);
                      break;
                  case OBJ_ARC:
                      flag |= arc_deselect_by_click((Arc *) obj, _x, _y);
                      break;
                  case OBJ_ELLIPSE:
                      flag |= ell_deselect_by_click((Ellipse *) obj, _x, _y);
                      break;
                  case OBJ_EARC:
                      flag |= earc_deselect_by_click((EArc *) obj, _x, _y);
                      break;
                  case OBJ_IMAGE:
                      flag |= image_deselect_by_click((Image *) obj, _x, _y);
                      break;
                  case OBJ_TEXT:
                      flag |= text_deselect_by_click((Text *) obj, _x, _y);
                      break;

                  }
            }
          else
            {
                switch (obj->type)
                  {
                  case OBJ_LINE:
                      flag |= line_select_by_click((Line *) obj, _x, _y);
                      break;
                  case OBJ_POINT:
                      flag |= point_select_by_click((Point *) obj, _x, _y);
                      break;

                  case OBJ_CIRCLE:
                      flag |= ci_select_by_click((Circle *) obj, _x, _y);
                      break;
                  case OBJ_ARC:
                      flag |= arc_select_by_click((Arc *) obj, _x, _y);
                      break;
                  case OBJ_ELLIPSE:
                      flag |= ell_select_by_click((Ellipse *) obj, _x, _y);
                      break;
                  case OBJ_EARC:
                      flag |= earc_select_by_click((EArc *) obj, _x, _y);
                      break;
                  case OBJ_IMAGE:
                      flag |= image_select_by_click((Image *) obj, _x, _y);
                      break;
                  case OBJ_TEXT:
                      flag |= text_select_by_click((Text *) obj, _x, _y);
                  }
            }
      }
    return flag;
}

void
sel_click(double _x, double _y)
{
    Eina_List          *l, *l1;
    Layer              *layer;
    Drawing            *d;
    int                 flag = 0, mode = 0;
    char                buf[4096];

    d = drawing;
    mode = shell->is_shifted;
    lock_data();
    for (l = d->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          if (!(layer->flags & FLAG_FROZEN))
              flag |= sel_click_layer(mode, layer, _x, _y);
      }
    msg_create_and_send(CMD_INFO_SYNC, 0, NULL);
    unlock_data();
    if (flag)
        return;

    if (mode)
      {
          sprintf(buf, "start_desel|%.*f %.*f", shell->float_prec, _x,
                  shell->float_prec, _y);
      }
    else
      {
          sprintf(buf, "start_sel|%.*f %.*f", shell->float_prec, _x,
                  shell->float_prec, _y);
      }
    gui_put_string(DUP(buf));
}

void
sel_rect(void)
{
    float               x1, y1, x2, y2;
    char               *s;
    int                 res;

    serv_set_hint(DUP(_("enter top-left point: ")));

    serv_set_state(ST_SEL1);
    do
      {
          s = serv_get_string();
          res = get_values(s, 0.0, 0.0, &x1, &y1);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_SEL2);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    pre_sel_x1y1(x1, y1);

    serv_set_hint(DUP(_("enter bottom-right point: ")));
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
    _sel_rect(0, x1, y1, x2, y2);
    unlock_data();

    serv_set_state(ST_NORMAL);
    msg_create_and_send(CMD_INFO_SYNC, 0, NULL);
}

void
desel_rect(void)
{
    float               x1, y1, x2, y2;
    char               *s;
    int                 res;

    serv_set_hint(DUP(_("enter upper left point: ")));

    serv_set_state(ST_SEL1);
    do
      {
          s = serv_get_string();
          res = get_values(s, 0.0, 0.0, &x1, &y1);
          if (res == 1)
              serv_set_hint(DUP(_("error, please reenter: ")));
      }
    while (res == 1);

    serv_set_state(ST_SEL2);

    if (res == 0)
      {
          serv_set_state(ST_NORMAL);
          return;
      }

    pre_sel_x1y1(x1, y1);

    serv_set_hint(DUP(_("enter lower right point: ")));
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
    _sel_rect(1, x1, y1, x2, y2);
    unlock_data();

    serv_set_state(ST_NORMAL);
    msg_create_and_send(CMD_INFO_SYNC, 0, NULL);
}

void
_sel_rect_layer(int mode, Layer * layer, double x, double y, double w, double h)
{
    Eina_List          *l;
    Object             *obj;

    for (l = layer->objects; l; l = l->next)
      {
          obj = (Object *) l->data;
          if (mode)
            {
                switch (obj->type)
                  {
                  case OBJ_LINE:
                      line_deselect_by_rect((Line *) obj, x, y, w, h);
                      break;
                  case OBJ_POINT:
                      point_deselect_by_rect((Point *) obj, x, y, w, h);
                      break;
                  case OBJ_CIRCLE:
                      ci_deselect_by_rect((Circle *) obj, x, y, w, h);
                      break;
                  case OBJ_ARC:
                      arc_deselect_by_rect((Arc *) obj, x, y, w, h);
                      break;
                  case OBJ_ELLIPSE:
                      ell_deselect_by_rect((Ellipse *) obj, x, y, w, h);
                      break;
                  case OBJ_EARC:
                      earc_deselect_by_rect((EArc *) obj, x, y, w, h);
                      break;
                  case OBJ_TEXT:
                      text_deselect_by_rect((Text *) obj, x, y, w, h);
                      break;
                  case OBJ_IMAGE:
                      image_deselect_by_rect((Image *) obj, x, y, w, h);
                      break;

                  }
            }
          else
            {
                switch (obj->type)
                  {
                  case OBJ_LINE:
                      line_select_by_rect((Line *) obj, x, y, w, h);
                      break;
                  case OBJ_POINT:
                      point_select_by_rect((Point *) obj, x, y, w, h);
                      break;
                  case OBJ_CIRCLE:
                      ci_select_by_rect((Circle *) obj, x, y, w, h);
                      break;
                  case OBJ_ARC:
                      arc_select_by_rect((Arc *) obj, x, y, w, h);
                      break;
                  case OBJ_ELLIPSE:
                      ell_select_by_rect((Ellipse *) obj, x, y, w, h);
                      break;
                  case OBJ_EARC:
                      earc_select_by_rect((EArc *) obj, x, y, w, h);
                      break;
                  case OBJ_TEXT:
                      text_select_by_rect((Text *) obj, x, y, w, h);
                      break;
                  case OBJ_IMAGE:
                      image_select_by_rect((Image *) obj, x, y, w, h);
                      break;
                  }
            }
      }
}

void
_sel_rect(int mode, double x1, double y1, double x2, double y2)
{
    Eina_List          *l;
    Layer              *layer;
    Drawing            *d;
    double              x, y, w, h;

    d = drawing;
    if (!d)
        return;

    x = (x1 < x2) ? x1 : x2;
    y = (y1 < y2) ? y1 : y2;
    w = fabs(x2 - x1);
    h = fabs(y2 - y1);

    for (l = d->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          if (layer && !(layer->flags & FLAG_FROZEN))
              _sel_rect_layer(mode, layer, x, y, w, h);
      }
}

void
pre_sel_x1y1(double x1, double y1)
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

void
sel_box_create(void)
{
    Evas               *e;

    e = shell->evas;
    if (!sel_box)
      {
          sel_box = evas_object_rectangle_add(e);
          evas_object_color_set(sel_box, 100, 80, 90, 100);
          evas_object_layer_set(sel_box, 17);
          evas_object_pass_events_set(sel_box, 1);
          sel1 = evas_object_line_add(e);
          sel2 = evas_object_line_add(e);
          sel3 = evas_object_line_add(e);
          sel4 = evas_object_line_add(e);
          evas_object_color_set(sel1, 155, 100, 150, 255);
          evas_object_color_set(sel2, 155, 100, 150, 255);
          evas_object_color_set(sel3, 155, 100, 150, 255);
          evas_object_color_set(sel4, 155, 100, 150, 255);
          evas_object_layer_set(sel1, 17);
          evas_object_layer_set(sel2, 17);
          evas_object_layer_set(sel3, 17);
          evas_object_layer_set(sel4, 17);
          evas_object_pass_events_set(sel1, 1);
          evas_object_pass_events_set(sel2, 1);
          evas_object_pass_events_set(sel3, 1);
          evas_object_pass_events_set(sel4, 1);
      }
    evas_object_resize(sel_box, 0, 0);
}

void
sel_box_redraw(Eina_List *data, double _x, double _y)
{
    XY                 *xy;
    Evas               *e;
    Drawing            *d;
    double              x1, y1, x2, y2;
    double              x, y, w, h;

    if (!data)
        return;
    e = shell->evas;
    d = drawing;
    if (!d)
        return;

    evas_object_show(sel_box);
    evas_object_show(sel1);
    evas_object_show(sel2);
    evas_object_show(sel3);
    evas_object_show(sel4);

    xy = (XY *) eina_list_last(data)->data;
    x1 = w2s_x(xy->x);
    y1 = w2s_y(xy->y);
    x2 = w2s_x(_x);
    y2 = w2s_y(_y);

    x = (x1 < x2) ? x1 : x2;
    y = (y1 < y2) ? y1 : y2;
    w = fabs(x2 - x1);
    h = fabs(y2 - y1);

    evas_object_move(sel_box, x, y);
    evas_object_resize(sel_box, w, h);

    evas_object_line_xy_set(sel1, x, y, x + w, y);
    evas_object_line_xy_set(sel2, x, y, x, y + h);
    evas_object_line_xy_set(sel3, x + w, y, x + w, y + h);
    evas_object_line_xy_set(sel4, x, y + h, x + w, y + h);
}

void
sel_box_destroy(void)
{
    evas_object_hide(sel_box);
    evas_object_hide(sel1);
    evas_object_hide(sel2);
    evas_object_hide(sel3);
    evas_object_hide(sel4);
}
