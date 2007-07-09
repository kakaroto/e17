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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "engy.h"

/****************************************************************/

void
_common_move_object(Object * obj, double dx, double dy)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_move((Line *) obj, dx, dy);
          break;
      case OBJ_POINT:
          point_move((Point *) obj, dx, dy);
          break;
      case OBJ_CIRCLE:
          ci_move((Circle *) obj, dx, dy);
          break;
      case OBJ_ARC:
          arc_move((Arc *) obj, dx, dy);
          break;
      case OBJ_ELLIPSE:
          ell_move((Ellipse *) obj, dx, dy);
          break;
      case OBJ_EARC:
          earc_move((EArc *) obj, dx, dy);
          break;
      case OBJ_IMAGE:
          image_move((Image *) obj, dx, dy);
          break;
      case OBJ_TEXT:
          text_move((Text *) obj, dx, dy);
          break;
      }
}

void
common_move(double dx, double dy)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_move_object(obj, dx, dy);
            }
      }
}

/****************************************************************/

void
_common_clone_object(Object * obj, double dx, double dy)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_clone((Line *) obj, dx, dy);
          break;
      case OBJ_POINT:
          point_clone((Point *) obj, dx, dy);
          break;
      case OBJ_CIRCLE:
          ci_clone((Circle *) obj, dx, dy);
          break;
      case OBJ_ARC:
          arc_clone((Arc *) obj, dx, dy);
          break;
      case OBJ_ELLIPSE:
          ell_clone((Ellipse *) obj, dx, dy);
          break;
      case OBJ_EARC:
          earc_clone((EArc *) obj, dx, dy);
          break;
      case OBJ_IMAGE:
          image_clone((Image *) obj, dx, dy);
          break;
      case OBJ_TEXT:
          text_clone((Text *) obj, dx, dy);
          break;
      }
}

void
common_clone(double dx, double dy)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_clone_object(obj, dx, dy);
            }
      }
}

/****************************************************************/

void
_common_rotate_object(Object * obj, double x, double y, double a)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_rotate((Line *) obj, x, y, a);
          break;
      case OBJ_POINT:
          point_rotate((Point *) obj, x, y, a);
          break;
      case OBJ_CIRCLE:
          ci_rotate((Circle *) obj, x, y, a);
          break;
      case OBJ_ARC:
          arc_rotate((Arc *) obj, x, y, a);
          break;
      case OBJ_ELLIPSE:
          ell_rotate((Ellipse *) obj, x, y, a);
          break;
      case OBJ_EARC:
          earc_rotate((EArc *) obj, x, y, a);
          break;
      case OBJ_IMAGE:
          image_rotate((Image *) obj, x, y, a);
          break;
      case OBJ_TEXT:
          text_rotate((Text *) obj, x, y, a);
          break;
      }
}

void
common_rotate(double x, double y, double a)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_rotate_object(obj, x, y, a);
            }
      }
}

/****************************************************************/

void
_common_scale_object(Object * obj, double x, double y, double a)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_scale((Line *) obj, x, y, a);
          break;
      case OBJ_POINT:
          point_scale((Point *) obj, x, y, a);
          break;
      case OBJ_CIRCLE:
          ci_scale((Circle *) obj, x, y, a);
          break;
      case OBJ_ARC:
          arc_scale((Arc *) obj, x, y, a);
          break;
      case OBJ_ELLIPSE:
          ell_scale((Ellipse *) obj, x, y, a);
          break;
      case OBJ_EARC:
          earc_scale((EArc *) obj, x, y, a);
          break;
      case OBJ_IMAGE:
          image_scale((Image *) obj, x, y, a);
          break;
      case OBJ_TEXT:
          text_scale((Text *) obj, x, y, a);
          break;
      }
}

void
common_scale(double x, double y, double a)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_scale_object(obj, x, y, a);
            }
      }
}

/****************************************************************/

void
_common_scale_xy_object(Object * obj, double x, double y, double a, double b)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_scale_xy((Line *) obj, x, y, a, b);
          break;
      case OBJ_POINT:
          point_scale_xy((Point *) obj, x, y, a, b);
          break;
      case OBJ_CIRCLE:
          ci_scale_xy((Circle *) obj, x, y, a, b);
          break;
      case OBJ_ARC:
          arc_scale_xy((Arc *) obj, x, y, a, b);
          break;
      case OBJ_ELLIPSE:
          ell_scale_xy((Ellipse *) obj, x, y, a, b);
          break;
      case OBJ_EARC:
          earc_scale_xy((EArc *) obj, x, y, a, b);
          break;
      case OBJ_IMAGE:
          image_scale_xy((Image *) obj, x, y, a, b);
          break;
      case OBJ_TEXT:
          text_scale_xy((Text *) obj, x, y, a, b);
          break;
      }
}

void
common_scale_xy(double x, double y, double a, double b)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_scale_xy_object(obj, x, y, a, b);
            }
      }
}

/****************************************************************/

void
_common_mirror_ab_object(Object * obj, double a, double b)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_mirror_ab((Line *) obj, a, b);
          break;
      case OBJ_POINT:
          point_mirror_ab((Point *) obj, a, b);
          break;
      case OBJ_CIRCLE:
          ci_mirror_ab((Circle *) obj, a, b);
          break;
      case OBJ_ARC:
          arc_mirror_ab((Arc *) obj, a, b);
          break;
      case OBJ_ELLIPSE:
          ell_mirror_ab((Ellipse *) obj, a, b);
          break;
      case OBJ_EARC:
          earc_mirror_ab((EArc *) obj, a, b);
          break;
      case OBJ_IMAGE:
          image_mirror_ab((Image *) obj, a, b);
          break;
      case OBJ_TEXT:
          text_mirror_ab((Text *) obj, a, b);
          break;
      }
}

void
common_mirror_ab(double a, double b)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_mirror_ab_object(obj, a, b);
            }
      }
}

/****************************************************************/

void
_common_mirror_y_object(Object * obj, double y)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_mirror_y((Line *) obj, y);
          break;
      case OBJ_POINT:
          point_mirror_y((Point *) obj, y);
          break;
      case OBJ_CIRCLE:
          ci_mirror_y((Circle *) obj, y);
          break;
      case OBJ_ARC:
          arc_mirror_y((Arc *) obj, y);
          break;
      case OBJ_ELLIPSE:
          ell_mirror_y((Ellipse *) obj, y);
          break;
      case OBJ_EARC:
          earc_mirror_y((EArc *) obj, y);
          break;
      case OBJ_IMAGE:
          image_mirror_y((Image *) obj, y);
          break;
      case OBJ_TEXT:
          text_mirror_y((Text *) obj, y);
          break;
      }
}

void
common_mirror_y(double y)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_mirror_y_object(obj, y);
            }
      }
}

/****************************************************************/

void
_common_mirror_x_object(Object * obj, double x)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_mirror_x((Line *) obj, x);
          break;
      case OBJ_POINT:
          point_mirror_x((Point *) obj, x);
          break;
      case OBJ_CIRCLE:
          ci_mirror_x((Circle *) obj, x);
          break;
      case OBJ_ARC:
          arc_mirror_x((Arc *) obj, x);
          break;
      case OBJ_ELLIPSE:
          ell_mirror_x((Ellipse *) obj, x);
          break;
      case OBJ_EARC:
          earc_mirror_x((EArc *) obj, x);
          break;
      case OBJ_IMAGE:
          image_mirror_x((Image *) obj, x);
          break;
      case OBJ_TEXT:
          text_mirror_x((Text *) obj, x);
          break;
      }
}

void
common_mirror_x(double x)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_mirror_x_object(obj, x);
            }
      }
}

/****************************************************************/

void
_common_trim_ab_object(Object * obj, double a, double b, int side)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_trim_ab((Line *) obj, a, b, side);
          break;
      case OBJ_POINT:
          point_trim_ab((Point *) obj, a, b, side);
          break;
      case OBJ_CIRCLE:
          ci_trim_ab((Circle *) obj, a, b, side);
          break;
      case OBJ_ARC:
          arc_trim_ab((Arc *) obj, a, b, side);
          break;
      case OBJ_ELLIPSE:
          ell_trim_ab((Ellipse *) obj, a, b, side);
          break;
      case OBJ_EARC:
          earc_trim_ab((EArc *) obj, a, b, side);
          break;
      case OBJ_IMAGE:
          image_trim_ab((Image *) obj, a, b, side);
          break;
      case OBJ_TEXT:
          text_trim_ab((Text *) obj, a, b, side);
          break;
      }
}

void
common_trim_ab(double a, double b, int side)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_trim_ab_object(obj, a, b, side);
            }
      }
}

/****************************************************************/

void
_common_trim_x_object(Object * obj, double x, int side)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_trim_x((Line *) obj, x, side);
          break;
      case OBJ_POINT:
          point_trim_x((Point *) obj, x, side);
          break;
      case OBJ_CIRCLE:
          ci_trim_x((Circle *) obj, x, side);
          break;
      case OBJ_ARC:
          arc_trim_x((Arc *) obj, x, side);
          break;
      case OBJ_ELLIPSE:
          ell_trim_x((Ellipse *) obj, x, side);
          break;
      case OBJ_EARC:
          earc_trim_x((EArc *) obj, x, side);
          break;
      case OBJ_IMAGE:
          image_trim_x((Image *) obj, x, side);
          break;
      case OBJ_TEXT:
          text_trim_x((Text *) obj, x, side);
          break;
      }
}

void
common_trim_x(double x, int side)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_trim_x_object(obj, x, side);
            }
      }
}
/****************************************************************/

void
_common_trim_y_object(Object * obj, double y, int side)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_trim_y((Line *) obj, y, side);
          break;
      case OBJ_POINT:
          point_trim_y((Point *) obj, y, side);
          break;
      case OBJ_CIRCLE:
          ci_trim_y((Circle *) obj, y, side);
          break;
      case OBJ_ARC:
          arc_trim_y((Arc *) obj, y, side);
          break;
      case OBJ_ELLIPSE:
          ell_trim_y((Ellipse *) obj, y, side);
          break;
      case OBJ_EARC:
          earc_trim_y((EArc *) obj, y, side);
          break;
      case OBJ_IMAGE:
          image_trim_y((Image *) obj, y, side);
          break;
      case OBJ_TEXT:
          text_trim_y((Text *) obj, y, side);
          break;
      }
}

void
common_trim_y(double y, int side)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_trim_y_object(obj, y, side);
            }
      }
}

/****************************************************************/
void
_common_array_object(Object * obj, int nx, int ny, double dx, double dy)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_array((Line *) obj, nx, ny, dx, dy);
          break;
      case OBJ_POINT:
          point_array((Point *) obj, nx, ny, dx, dy);
          break;
      case OBJ_CIRCLE:
          ci_array((Circle *) obj, nx, ny, dx, dy);
          break;
      case OBJ_ARC:
          arc_array((Arc *) obj, nx, ny, dx, dy);
          break;
      case OBJ_ELLIPSE:
          ell_array((Ellipse *) obj, nx, ny, dx, dy);
          break;
      case OBJ_EARC:
          earc_array((EArc *) obj, nx, ny, dx, dy);
          break;
      case OBJ_IMAGE:
          image_array((Image *) obj, nx, ny, dx, dy);
          break;
      case OBJ_TEXT:
          text_array((Text *) obj, nx, ny, dx, dy);
          break;
      }
}

void
common_array(int nx, int ny, double dx, double dy)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_array_object(obj, nx, ny, dx, dy);
            }
      }
}

/****************************************************************/
void
_common_polar_object(Object * obj, double x0, double y0, int num, double da)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_array_polar((Line *) obj, x0, y0, num, da);
          break;
      case OBJ_POINT:
          point_array_polar((Point *) obj, x0, y0, num, da);
          break;
      case OBJ_CIRCLE:
          ci_array_polar((Circle *) obj, x0, y0, num, da);
          break;
      case OBJ_ARC:
          arc_array_polar((Arc *) obj, x0, y0, num, da);
          break;
      case OBJ_ELLIPSE:
          ell_array_polar((Ellipse *) obj, x0, y0, num, da);
          break;
      case OBJ_EARC:
          earc_array_polar((EArc *) obj, x0, y0, num, da);
          break;
      case OBJ_IMAGE:
          image_array_polar((Image *) obj, x0, y0, num, da);
          break;
      case OBJ_TEXT:
          text_array_polar((Text *) obj, x0, y0, num, da);
          break;
      }
}

void
common_polar(double x0, double y0, int num, double da)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_polar_object(obj, x0, y0, num, da);
            }
      }
}

/****************************************************************/
void
_common_delete_object(Object * obj)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_delete((Line *) obj);
          break;
      case OBJ_POINT:
          point_delete((Point *) obj);
          break;
      case OBJ_CIRCLE:
          ci_delete((Circle *) obj);
          break;
      case OBJ_ARC:
          arc_delete((Arc *) obj);
          break;
      case OBJ_ELLIPSE:
          ell_delete((Ellipse *) obj);
          break;
      case OBJ_EARC:
          earc_delete((EArc *) obj);
          break;
      case OBJ_IMAGE:
          image_delete((Image *) obj);
          break;
      case OBJ_TEXT:
          text_delete((Text *) obj);
          break;
      }
}

void
common_delete(void)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_delete_object(obj);
            }
      }
}

/****************************************************************/

void
_common_sel_object(Object * obj)
{
    int                 dummy;

    if ((obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    dummy = obj->flags;
    obj->flags |= FLAG_SELECTED;
/*    switch(obj->type){
	    case OBJ_LINE: obj->flags |= FLAG_SELECTED;break;
	    case OBJ_POINT: obj->flags |= FLAG_SELECTED;break;
	    case OBJ_CIRCLE: obj->flags |= FLAG_SELECTED;break;
	    case OBJ_ELLIPSE: obj->flags |= FLAG_SELECTED;break;
	    case OBJ_IMAGE: 
    }*/
    append_undo_long((void *)&(obj->flags),
                    dummy, obj->flags, CMD_SYNC, obj->type, obj);
    msg_create_and_send(CMD_SYNC, obj->type, obj);
}

void
common_all_sel(void)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_sel_object(obj);
            }
      }
    msg_create_and_send(CMD_INFO_SYNC, 0, NULL);
}

/****************************************************************/

void
_common_desel_object(Object * obj)
{
    int                 dummy;

    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    dummy = obj->flags;
    obj->flags ^= FLAG_SELECTED;
/*    switch(obj->type){
	    case OBJ_LINE: obj->flags ^=FLAG_SELECTED;break;
	    case OBJ_POINT: obj->flags ^=FLAG_SELECTED;break;
	    case OBJ_CIRCLE: obj->flags ^=FLAG_SELECTED;break;
	    case OBJ_ELLIPSE: obj->flags ^=FLAG_SELECTED;break;
    }*/
    append_undo_long((void *)&(obj->flags),
                    dummy, obj->flags, CMD_SYNC, obj->type, obj);
    msg_create_and_send(CMD_SYNC, obj->type, obj);
    msg_create_and_send(CMD_INFO_SYNC, 0, NULL);
}

void
common_all_desel(void)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_desel_object(obj);
            }
      }
}

/****************************************************************/

void
_common_inv_sel_object(Object * obj)
{
    int                 dummy;

    if (obj->flags & FLAG_DELETED)
        return;
    dummy = obj->flags;
    obj->flags ^= FLAG_SELECTED;
/*    switch(obj->type){
	    case OBJ_LINE: obj->flags ^= FLAG_SELECTED;break;
	    case OBJ_POINT: obj->flags ^= FLAG_SELECTED;break;
	    case OBJ_CIRCLE: obj->flags ^= FLAG_SELECTED;break;
	    case OBJ_ELLIPSE: obj->flags ^= FLAG_SELECTED;break;
    }                  */
    append_undo_long((void *)&(obj->flags),
                    dummy, obj->flags, CMD_SYNC, obj->type, obj);
    msg_create_and_send(CMD_SYNC, obj->type, obj);
}

void
common_inv_sel(void)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_inv_sel_object(obj);
            }
      }
    msg_create_and_send(CMD_INFO_SYNC, 0, NULL);
}

/****************************************************************/
void
common_cut(double dx, double dy)
{
    common_copy(dx, dy);
    common_delete();
}
/****************************************************************/

void
_common_copy_object(Object * obj, int sock, double dx, double dy)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_LINE:
          line_copy((Line *) obj, sock, dx, dy);
          break;
      case OBJ_POINT:
          point_copy((Point *) obj, sock, dx, dy);
          break;
      case OBJ_CIRCLE:
          ci_copy((Circle *) obj, sock, dx, dy);
          break;
      case OBJ_ARC:
          arc_copy((Arc *) obj, sock, dx, dy);
          break;
      case OBJ_ELLIPSE:
          ell_copy((Ellipse *) obj, sock, dx, dy);
          break;
      case OBJ_EARC:
          earc_copy((EArc *) obj, sock, dx, dy);
          break;
      case OBJ_IMAGE:
          image_copy((Image *) obj, sock, dx, dy);
          break;
      case OBJ_TEXT:
          text_copy((Text *) obj, sock, dx, dy);
          break;
      }
}

void
common_copy(double dx, double dy)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    int                 sock;
    int                 i, wres;
    struct sockaddr_in  server;
    struct hostent     *hp, *gethostbyname();
    char                mybuf[1024];

    if (!drawing)
        return;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
      {
          perror("opening stream socket");
          return;
      }

    server.sin_family = AF_INET;
    hp = gethostbyname("localhost");
    if (hp == (struct hostent *)0)
      {
          fprintf(stderr, "unknown host\n");
          return;
      }

    memcpy((char *)&server.sin_addr, (char *)hp->h_addr, hp->h_length);

    server.sin_port = htons(0xcadd);
    if (connect(sock, (struct sockaddr *)&server, sizeof server) == -1)
      {
          perror("connecting stream socket");
          return;
      }
    wres = write(sock, "put", strlen("put"));
    if (wres == -1)
      {
          perror("request type");
          return;
      }

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_copy_object(obj, sock, dx, dy);
            }
      }
    close(sock);
}

/****************************************************************/

int
c_p_parse(int sock, CP_Header hd, double dx, double dy)
{
    switch (hd.type)
      {
      case OBJ_POINT:
          point_paste(hd, sock, dx, dy);
          break;
      case OBJ_LINE:
          line_paste(hd, sock, dx, dy);
          break;
      case OBJ_CIRCLE:
          ci_paste(hd, sock, dx, dy);
          break;
      case OBJ_ARC:
          arc_paste(hd, sock, dx, dy);
          break;
      case OBJ_ELLIPSE:
          ell_paste(hd, sock, dx, dy);
          break;
      case OBJ_EARC:
          earc_paste(hd, sock, dx, dy);
          break;
      case OBJ_IMAGE:
          image_paste(hd, sock, dx, dy);
          break;
      case OBJ_TEXT:
          text_paste(hd, sock, dx, dy);
          break;
      default:
          return 0;
      }
    return 1;
}

void
common_paste(double dx, double dy)
{
    int                 sock;
    int                 i, wres;
    struct sockaddr_in  server;
    struct hostent     *hp, *gethostbyname();
    char                mybuf[1024];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1)
      {
          perror("opening stream socket");
          return;
      }

    server.sin_family = AF_INET;
    hp = gethostbyname("localhost");
    if (hp == (struct hostent *)0)
      {
          fprintf(stderr, "unknown host\n");
          return;
      }

    memcpy((char *)&server.sin_addr, (char *)hp->h_addr, hp->h_length);

    server.sin_port = htons(0xcadd);
    if (connect(sock, (struct sockaddr *)&server, sizeof server) == -1)
      {
          perror("connecting stream socket");
          return;
      }

    wres = write(sock, "get", strlen("get"));
    if (wres == -1)
      {
          perror("request type");
          return;
      }

    do
      {
          CP_Header           hd;

          wres = read(sock, &hd, sizeof(hd));
          if (wres == -1)
            {
                perror("reading header");
                return;
            }

          wres = c_p_parse(sock, hd, dx, dy);

      }
    while (wres > 0);

    close(sock);
}

/****************************************************************/

void
_common_linestyle_object(Object * obj, char *s)
{
    char               *dummy;

    if (!(obj->flags & FLAG_SELECTED))
        return;
    dummy = obj->line_style;

    obj->line_style = DUP(s);

    append_undo_ptr((void *)&(obj->line_style),
                    dummy, obj->line_style, CMD_SYNC, obj->type, obj);
    msg_create_and_send(CMD_SYNC, obj->type, obj);
}

void
common_linestyle(char *s)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
      {
          FREE(s);
          return;
      }

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_linestyle_object(obj, s);
            }
      }
    FREE(s);
}

/****************************************************************/

void
_common_color_object(Object * obj, int r, int g, int b, int a)
{
    int                 tr, tg, tb, ta;

    if (!(obj->flags & FLAG_SELECTED))
        return;

    tr = obj->color.red;
    tg = obj->color.green;
    tb = obj->color.blue;
    ta = obj->color.alpha;
    obj->color.red = r;
    obj->color.green = g;
    obj->color.blue = b;
    obj->color.alpha = a;
    append_undo_long((void *)&(obj->color.red), tr, r, CMD_SYNC, obj->type, obj);
    append_undo_long((void *)&(obj->color.green), tg, g, 0, 0, NULL);
    append_undo_long((void *)&(obj->color.blue), tb, b, 0, 0, NULL);
    append_undo_long((void *)&(obj->color.alpha), ta, a, 0, 0, NULL);
    msg_create_and_send(CMD_SYNC, obj->type, obj);
}

void
common_color(int r, int g, int b, int a)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_color_object(obj, r, g, b, a);
            }
      }
}

/****************************************************************/

void
_common_thickness_object(Object * obj, double w)
{
    double              dummy;

    if (!(obj->flags & FLAG_SELECTED))
        return;
    dummy = obj->thickness;
    obj->thickness = w;
    append_undo_double((void *)&(obj->thickness),
                       dummy, obj->thickness, CMD_SYNC, obj->type, obj);
    msg_create_and_send(CMD_SYNC, obj->type, obj);
}

void
common_thickness(float w)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_thickness_object(obj, w);
            }
      }
}

/****************************************************************/

void
_common_linescale_object(Object * obj, double w)
{
    double              dummy;

    if (!(obj->flags & FLAG_SELECTED))
        return;
    dummy = obj->line_scale;
    obj->line_scale = w;
    append_undo_double((void *)&(obj->line_scale),
                       dummy, obj->line_scale, CMD_SYNC, obj->type, obj);
    msg_create_and_send(CMD_SYNC, obj->type, obj);
}

void
common_linescale(float w)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_linescale_object(obj, w);
            }
      }
}

/****************************************************************/

char              **
common_get_info_object(Object * obj)
{
    switch (obj->type)
      {
      case OBJ_NONE:
          break;
      case OBJ_POINT:
          return point_get_info((Point *) obj);
      case OBJ_LINE:
          return line_get_info((Line *) obj);
      case OBJ_CIRCLE:
          return ci_get_info((Circle *) obj);
      case OBJ_ARC:
          return arc_get_info((Arc *) obj);
      case OBJ_ELLIPSE:
          return ell_get_info((Ellipse *) obj);
      case OBJ_EARC:
          return earc_get_info((EArc *) obj);
      case OBJ_IMAGE:
          return image_get_info((Image *) obj);
      case OBJ_TEXT:
          return text_get_info((Text *) obj);
      default:
          return (memset((malloc(sizeof(char *) * 10)),
                         0, sizeof(char *) * 10));
      }
}

char              **
common_get_info(void)
{
    int                 i;
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return (memset((malloc(sizeof(char *) * 10)), 0, sizeof(char *) * 10));

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                if ((obj->flags & FLAG_SELECTED) &&
                    !(obj->flags & FLAG_DELETED))
                    return (char **)common_get_info_object(obj);
            }
      }
    return (memset((malloc(sizeof(char *) * 10)), 0, sizeof(char *) * 10));
}

/****************************************************************/

Color
common_get_color(void)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;
    static Color        col;

    col.red = 0;
    col.green = 0;
    col.blue = 0;
    col.alpha = 0;

    if (!drawing)
        return col;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                if ((obj->flags & FLAG_SELECTED) &&
                    !(obj->flags & FLAG_DELETED))
                    return obj->color;
            }
      }
    return col;
}

/****************************************************************/
void
_common_gravity_object(Object * obj, int gr)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_TEXT:
          text_gravity((Text *) obj, gr);
          break;
      case OBJ_IMAGE:
          image_gravity((Image *) obj, gr);
          break;
      }
}

void
common_gravity(int gr)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_gravity_object(obj, gr);
            }
      }
}

/****************************************************************/
void
_common_text_height_object(Object * obj, double h)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_TEXT:
          text_text_height((Text *) obj, h);
          break;
      }
}

void
common_text_height(double h)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_text_height_object(obj, h);
            }
      }
}

/****************************************************************/
void
_common_text_object(Object * obj, char *s)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_TEXT:
          text_text((Text *) obj, DUP(s));
          break;
      }
}

void
common_text(char *s)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_text_object(obj, s);
            }
      }
    FREE(s);
}

/****************************************************************/
void
_common_image_scale_object(Object * obj, double s)
{
    if (!(obj->flags & FLAG_SELECTED))
        return;
    if (obj->flags & FLAG_DELETED)
        return;
    switch (obj->type)
      {
      case OBJ_IMAGE:
          image_image_scale((Image *) obj, s);
          break;
      }
}

void
common_image_scale(double s)
{
    Evas_List          *l, *l1;
    Layer              *layer;
    Object             *obj;

    if (!drawing)
        return;

    for (l = drawing->layers; l; l = l->next)
      {
          layer = (Layer *) l->data;
          for (l1 = layer->objects; l1; l1 = l1->next)
            {
                obj = (Object *) l1->data;
                _common_image_scale_object(obj, s);
            }
      }
}

/****************************************************************/

void
common_entity_load(int id, int type)
{
    switch (type)
      {
      case OBJ_POINT:
          point_load(id);
          break;
      case OBJ_LINE:
          line_load(id);
          break;
      case OBJ_CIRCLE:
          ci_load(id);
          break;
      case OBJ_ARC:
          arc_load(id);
          break;
      case OBJ_ELLIPSE:
          ell_load(id);
          break;
      case OBJ_EARC:
          earc_load(id);
          break;
      case OBJ_IMAGE:
          image_load(id);
          break;
      case OBJ_TEXT:
          text_load(id);
          break;
      }
}
