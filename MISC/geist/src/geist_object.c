/* geist_object.c

Copyright (C) 1999,2000 Tom Gilbert.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include "geist_object.h"
#include "geist_text.h"


geist_object *
geist_object_new(void)
{
   geist_object *obj = NULL;

   D_ENTER(5);

   obj = emalloc(sizeof(geist_object));

   D_RETURN(5, obj);
}

void
geist_object_init(geist_object * obj)
{
   D_ENTER(5);

   if (!obj)
      eprintf("bug. no object to init.");

   memset(obj, 0, sizeof(geist_object));
   geist_object_set_type(obj, GEIST_TYPE_OBJECT);
   obj->free = geist_object_int_free;
   obj->render = geist_object_int_render;
   obj->render_partial = geist_object_int_render_partial;
   obj->render_selected = geist_object_int_render_selected;
   obj->get_selection_updates = geist_object_int_get_selection_updates;
   obj->get_rendered_image = geist_object_int_get_rendered_image;
   obj->part_is_transparent = geist_object_int_part_is_transparent;
   obj->display_props = geist_object_int_display_props;
   obj->resize_event = geist_object_int_resize;
   obj->get_rendered_area = geist_object_int_get_rendered_area;
   obj->check_resize_click = geist_object_int_check_resize_click;
   obj->get_resize_box_coords = geist_object_int_get_resize_box_coords;
   obj->click_is_selection = geist_object_int_click_is_selection;
   obj->update_position_relative = geist_object_int_update_position_relative;
   obj->update_dimensions_relative =
      geist_object_int_update_dimensions_relative;
   obj->update_positioning = geist_object_int_update_positioning;
   obj->get_updates = geist_object_int_get_updates;
   obj->get_object_list_entry = geist_object_int_get_object_list_entry;
   obj->has_transparency = geist_object_int_has_transparency;
   obj->move = geist_object_int_move;
   obj->rotate = geist_object_int_rotate;
   obj->name = estrdup("Untitled Object");
   obj->last.alias = FALSE;
   obj->alias = TRUE;

   D_RETURN_(5);
}

void
geist_object_int_free(geist_object * obj)
{
   D_ENTER(5);

   if (!obj)
      D_RETURN_(5);

   if (obj->name)
      efree(obj->name);

   efree(obj);

   D_RETURN_(5);
}

void
geist_object_free(geist_object * obj)
{
   D_ENTER(5);

   if (!obj)
      D_RETURN_(5);

   obj->free(obj);

   D_RETURN_(5);
}

geist_object_type geist_object_get_type(geist_object * obj)
{
   return obj->type;
}

void
geist_object_set_type(geist_object * obj, geist_object_type type)
{
   obj->type = type;
   return;
}

void
geist_object_int_render(geist_object * obj, Imlib_Image dest)
{
   D_ENTER(5);

   printf("IMPLEMENT\n");

   D_RETURN_(5);
}

void
geist_object_int_render_partial(geist_object * obj, Imlib_Image dest, int x,
                                int y, int w, int h)
{
   D_ENTER(5);

   printf("IMPLEMENT\n");

   D_RETURN_(5);
}


void
geist_object_render(geist_object * obj, Imlib_Image dest)
{
   D_ENTER(5);

   if (!obj)
      D_RETURN_(5);

   obj->render(obj, dest);

   D_RETURN_(5);
}

void
geist_object_render_partial(geist_object * obj, Imlib_Image dest, int x,
                            int y, int w, int h)
{
   D_ENTER(5);

   D(5, ("rendering area %d,%d %dx%d\n", x, y, w, h));
   obj->render_partial(obj, dest, x, y, w, h);

   D_RETURN_(5);
}


void
geist_object_show(geist_object * obj)
{
   D_ENTER(3);

   geist_object_set_state(obj, VISIBLE);
   geist_object_dirty(obj);

   D_RETURN_(3);
}

void
geist_object_hide(geist_object * obj)
{
   D_ENTER(3);

   geist_object_unset_state(obj, VISIBLE);
   geist_object_dirty(obj);

   D_RETURN_(3);
}


void
geist_object_raise(geist_object * obj)
{
   D_ENTER(3);

   if (!obj)
      D_RETURN_(3);

   geist_layer_raise_object(obj);
   geist_object_dirty(obj);

   D_RETURN_(3);
}

void
geist_object_render_selected(geist_object * obj, Imlib_Image dest,
                             unsigned char multiple)
{
   D_ENTER(3);

   obj->render_selected(obj, dest, multiple);

   D_RETURN_(3);
}

char *
geist_object_get_object_list_entry(geist_object * obj, int column)
{
   D_ENTER(3);

   D_RETURN(3, obj->get_object_list_entry(obj, column));
}

char *
geist_object_int_get_object_list_entry(geist_object * obj, int column)
{
   D_ENTER(3);

   D_RETURN(3,
            (char *)
            gib_imlib_image_get_filename(geist_object_get_rendered_image
                                           (obj)));
}


void
geist_object_add_to_object_list(geist_object * obj)
{
   int row = 0;
   char *list[3] = { 0, 0, 0 };

   D_ENTER(3);
   list[0] = (obj->name) ? (obj->name) : "Untitled Object";
   if (geist_object_get_type(obj) == GEIST_TYPE_TEXT)
      list[1] = GEIST_TEXT(obj)->text;
   else
      list[1] = geist_object_get_object_list_entry(obj, 1);

   gtk_signal_handler_block(GTK_OBJECT(obj_list), obj_sel_handler);
   gtk_signal_handler_block(GTK_OBJECT(obj_list), obj_unsel_handler);

   row = gtk_clist_append(GTK_CLIST(obj_list), list);
   gtk_clist_set_row_data(GTK_CLIST(obj_list), row, (gpointer) obj);

   gtk_signal_handler_unblock(GTK_OBJECT(obj_list), obj_sel_handler);
   gtk_signal_handler_unblock(GTK_OBJECT(obj_list), obj_unsel_handler);


   D_RETURN_(3);
}

Imlib_Image geist_object_get_rendered_image(geist_object * obj)
{
   D_ENTER(5);

   D_RETURN(5, obj->get_rendered_image(obj));
}


Imlib_Image geist_object_int_get_rendered_image(geist_object * obj)
{
   D_ENTER(5);

   D_RETURN(5, NULL);
}

void
geist_object_select(geist_object * obj)
{
   D_ENTER(5);

   D(4, ("setting object state SELECTED\n"));
   geist_object_set_state(obj, SELECTED);
   geist_object_raise(obj);
   geist_object_dirty_selection(obj);

   D_RETURN_(5);
}

void
geist_object_unselect(geist_object * obj)
{
   D_ENTER(5);

   D(4, ("unsetting object state SELECTED\n"));
   geist_object_unset_state(obj, SELECTED);
   geist_object_dirty_selection(obj);

   D_RETURN_(5);
}

void
geist_object_int_render_selected(geist_object * obj, Imlib_Image dest,
                                 unsigned char multiple)
{
   D_ENTER(5);

   if (multiple)
   {
      gib_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      gib_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      gib_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      gib_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);

      gib_imlib_image_draw_rectangle(dest,
                                       obj->x - HALF_SEL_WIDTH + (obj->w / 2),
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      gib_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT +
                                       (obj->h / 2), 2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      gib_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT +
                                       (obj->h / 2), 2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      gib_imlib_image_draw_rectangle(dest,
                                       obj->x - HALF_SEL_WIDTH + (obj->w / 2),
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
   }
   else
   {
      gib_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      gib_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      gib_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      gib_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);

      gib_imlib_image_fill_rectangle(dest,
                                       obj->x - HALF_SEL_WIDTH + (obj->w / 2),
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      gib_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT +
                                       (obj->h / 2), 2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      gib_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT +
                                       (obj->h / 2), 2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      gib_imlib_image_fill_rectangle(dest,
                                       obj->x - HALF_SEL_WIDTH + (obj->w / 2),
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);

   }

   D_RETURN_(5);
}

Imlib_Updates geist_object_int_get_selection_updates(geist_object * obj)
{
   Imlib_Updates up = NULL;

   D_ENTER(5);

   up =
      imlib_update_append_rect(up, obj->x - HALF_SEL_WIDTH,
                               obj->y - HALF_SEL_HEIGHT, 2 * HALF_SEL_WIDTH,
                               2 * HALF_SEL_HEIGHT);
   up =
      imlib_update_append_rect(up, obj->x - HALF_SEL_WIDTH + obj->w,
                               obj->y - HALF_SEL_HEIGHT, 2 * HALF_SEL_WIDTH,
                               2 * HALF_SEL_HEIGHT);
   up =
      imlib_update_append_rect(up, obj->x - HALF_SEL_WIDTH,
                               obj->y - HALF_SEL_HEIGHT + obj->h,
                               2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT);
   up =
      imlib_update_append_rect(up, obj->x - HALF_SEL_WIDTH + obj->w,
                               obj->y - HALF_SEL_HEIGHT + obj->h,
                               2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT);

   up =
      imlib_update_append_rect(up, obj->x - HALF_SEL_WIDTH + (obj->w / 2),
                               obj->y - HALF_SEL_HEIGHT, 2 * HALF_SEL_WIDTH,
                               2 * HALF_SEL_HEIGHT);
   up =
      imlib_update_append_rect(up, obj->x - HALF_SEL_WIDTH + obj->w,
                               obj->y - HALF_SEL_HEIGHT + (obj->h / 2),
                               2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT);
   up =
      imlib_update_append_rect(up, obj->x - HALF_SEL_WIDTH,
                               obj->y - HALF_SEL_HEIGHT + (obj->h / 2),
                               2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT);
   up =
      imlib_update_append_rect(up, obj->x - HALF_SEL_WIDTH + (obj->w / 2),
                               obj->y - HALF_SEL_HEIGHT + obj->h,
                               2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT);


   D_RETURN(5, up);
}

int
geist_object_check_resize_click(geist_object * obj, int x, int y)
{
   D_ENTER(5);
   D_RETURN(5, obj->check_resize_click(obj, x, y));
}

int
geist_object_int_check_resize_click(geist_object * obj, int x, int y)
{
   D_ENTER(5);

   if (XY_IN_RECT
       (x, y, obj->x - HALF_SEL_WIDTH, obj->y - HALF_SEL_HEIGHT,
        2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT))
      D_RETURN(5, RESIZE_TOPLEFT);
   if (XY_IN_RECT
       (x, y, obj->x - HALF_SEL_WIDTH + obj->w, obj->y - HALF_SEL_HEIGHT,
        2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT))
      D_RETURN(5, RESIZE_TOPRIGHT);
   if (XY_IN_RECT
       (x, y, obj->x - HALF_SEL_WIDTH, obj->y - HALF_SEL_HEIGHT + obj->h,
        2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT))
      D_RETURN(5, RESIZE_BOTTOMLEFT);
   if (XY_IN_RECT
       (x, y, obj->x - HALF_SEL_WIDTH + obj->w,
        obj->y - HALF_SEL_HEIGHT + obj->h, 2 * HALF_SEL_WIDTH,
        2 * HALF_SEL_HEIGHT))
      D_RETURN(5, RESIZE_BOTTOMRIGHT);
   if (XY_IN_RECT
       (x, y, obj->x - HALF_SEL_WIDTH + (obj->w / 2),
        obj->y - HALF_SEL_HEIGHT, 2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT))
      D_RETURN(5, RESIZE_TOP);
   if (XY_IN_RECT
       (x, y, obj->x - HALF_SEL_WIDTH + obj->w,
        obj->y - HALF_SEL_HEIGHT + (obj->h / 2), 2 * HALF_SEL_WIDTH,
        2 * HALF_SEL_HEIGHT))
      D_RETURN(5, RESIZE_RIGHT);
   if (XY_IN_RECT
       (x, y, obj->x - HALF_SEL_WIDTH,
        obj->y - HALF_SEL_HEIGHT + (obj->h / 2), 2 * HALF_SEL_WIDTH,
        2 * HALF_SEL_HEIGHT))
      D_RETURN(5, RESIZE_LEFT);
   if (XY_IN_RECT
       (x, y, obj->x - HALF_SEL_WIDTH + (obj->w / 2),
        obj->y - HALF_SEL_HEIGHT + obj->h, 2 * HALF_SEL_WIDTH,
        2 * HALF_SEL_HEIGHT))
      D_RETURN(5, RESIZE_BOTTOM);

   D_RETURN(5, RESIZE_NONE);
}

void
geist_object_get_resize_box_coords(geist_object * obj, int resize, int *x,
                                   int *y)
{
   D_ENTER(3);

   obj->get_resize_box_coords(obj, resize, x, y);

   D_RETURN_(3);
}

void
geist_object_int_get_resize_box_coords(geist_object * obj, int resize, int *x,
                                       int *y)
{
   D_ENTER(3);

   switch (resize)
   {
     case RESIZE_RIGHT:
        *x = obj->x - HALF_SEL_WIDTH + obj->w;
        *y = obj->y - HALF_SEL_HEIGHT + (obj->h / 2);
        break;
     case RESIZE_LEFT:
        *x = obj->x - HALF_SEL_WIDTH;
        *y = obj->y - HALF_SEL_HEIGHT + (obj->h / 2);
        break;
     case RESIZE_BOTTOM:
        *x = obj->x - HALF_SEL_WIDTH + (obj->w / 2);
        *y = obj->y - HALF_SEL_HEIGHT + obj->h;
        break;
     case RESIZE_TOP:
        *x = obj->x - HALF_SEL_WIDTH + (obj->w / 2);
        *y = obj->y - HALF_SEL_HEIGHT;
        break;
     case RESIZE_TOPRIGHT:
        *x = obj->x - HALF_SEL_WIDTH + obj->w;
        *y = obj->y - HALF_SEL_HEIGHT;
        break;
     case RESIZE_BOTTOMRIGHT:
        *x = obj->x - HALF_SEL_WIDTH + obj->w;
        *y = obj->y - HALF_SEL_HEIGHT + obj->h;
        break;
     case RESIZE_BOTTOMLEFT:
        *x = obj->x - HALF_SEL_WIDTH;
        *y = obj->y - HALF_SEL_HEIGHT + obj->h;
        break;
     case RESIZE_TOPLEFT:
        *x = obj->x - HALF_SEL_WIDTH;
        *y = obj->y - HALF_SEL_HEIGHT;
        break;
     default:
        break;
   }
   D_RETURN_(3);
}

Imlib_Updates geist_object_get_selection_updates(geist_object * obj)
{
   D_ENTER(3);

   D_RETURN(3, obj->get_selection_updates(obj));
}

geist_object *
geist_object_duplicate(geist_object * obj)
{
   D_ENTER(3);

   D_RETURN(3, obj->duplicate(obj));
}

geist_object *
geist_object_int_duplicate(geist_object * obj)
{
   D_ENTER(3);

   printf("IMPLEMENT!\n");

   D_RETURN(3, NULL);
}

unsigned char
geist_object_part_is_transparent(geist_object * obj, int x, int y)
{
   D_ENTER(3);

   D_RETURN(3, obj->part_is_transparent(obj, x, y));
}

unsigned char
geist_object_int_part_is_transparent(geist_object * obj, int x, int y)
{
   D_ENTER(3);

   D_RETURN(3,
            geist_imlib_image_part_is_transparent
            (geist_object_get_rendered_image(obj), x, y));
}

void
geist_object_resize(geist_object * obj, int x, int y)
{
   D_ENTER(5);

   geist_object_dirty(obj);
   obj->resize_event(obj, x, y);
   geist_object_dirty(obj);

   D_RETURN_(5);
}

void
geist_object_move(geist_object * obj, int x, int y)
{
   D_ENTER(3);
   obj->move(obj,x,y);
   D_RETURN_(3);
}

void
geist_object_int_move(geist_object * obj, int x, int y)
{
   D_ENTER(3);

   geist_object_dirty(obj);
   obj->x = x - obj->clicked_x;
   obj->y = y - obj->clicked_y;
   geist_object_dirty(obj);

   D_RETURN_(3);
}


void
geist_object_int_resize(geist_object * obj, int x, int y)
{
   D_ENTER(5);

   printf("(int_resize) implement me!\n");

   D_RETURN_(5);
}

void
geist_object_resize_object(geist_object * obj, int x, int y)
{
   D_ENTER(5);

   switch (obj->resize)
   {
     case RESIZE_RIGHT:
        obj->w = x - obj->x;
        break;
     case RESIZE_LEFT:
        if (x < obj->x + obj->w)
        {
           obj->w = obj->x + obj->w - x;
           obj->x = x;
        }
        else
           obj->w = 1;
        break;
     case RESIZE_BOTTOM:
        obj->h = y - obj->y;
        break;
     case RESIZE_TOP:
        if (y < obj->y + obj->h)
        {
           obj->h = obj->y + obj->h - y;
           obj->y = y;
        }
        else
           obj->h = 1;
        break;
     case RESIZE_TOPRIGHT:
        obj->w = x - obj->x;
        if (y < obj->y + obj->h)
        {
           obj->h = obj->y + obj->h - y;
           obj->y = y;
        }
        else
           obj->h = 1;
        break;
     case RESIZE_BOTTOMRIGHT:
        obj->w = x - obj->x;
        obj->h = y - obj->y;
        break;
     case RESIZE_BOTTOMLEFT:
        obj->h = y - obj->y;
        if (x < obj->x + obj->w)
        {
           obj->w = obj->x + obj->w - x;
           obj->x = x;
        }
        else
           obj->w = 1;
        break;
     case RESIZE_TOPLEFT:
        if (y < obj->y + obj->h)
        {
           obj->h = obj->y + obj->h - y;
           obj->y = y;
        }
        else
           obj->h = 1;
        if (x < obj->x + obj->w)
        {
           obj->w = obj->x + obj->w - x;
           obj->x = x;
        }
        else
           obj->w = 1;
        break;
     default:
        break;
   }

   if (obj->h < 1)
      obj->h = 1;
   if (obj->w < 1)
      obj->w = 1;

   geist_object_update_positioning(obj);
   D_RETURN_(3);
}

void
geist_object_update_positioning(geist_object * obj)
{
   D_ENTER(3);

   obj->update_positioning(obj);

   D_RETURN_(3);
}

void
geist_object_int_update_positioning(geist_object * obj)
{
   D_ENTER(3);

   geist_object_update_sizemode(obj);
   geist_object_update_alignment(obj);

   D_RETURN_(3);
}

void
geist_object_update_sizemode(geist_object * obj)
{
   D_ENTER(3);

   switch (obj->sizemode)
   {
     case SIZEMODE_NONE:
        break;
     case SIZEMODE_STRETCH:
        obj->rendered_w = obj->w;
        obj->rendered_h = obj->h;
        break;
     case SIZEMODE_ZOOM:
        break;
     default:
        printf("(sizemode) implement me!\n");
        break;
   }
   D_RETURN_(3);
}


void
geist_object_update_alignment(geist_object * obj)
{
   D_ENTER(3);
   switch (obj->alignment)
   {
     case ALIGN_NONE:
        break;
     case ALIGN_CENTER:
        obj->rendered_x = (obj->w - obj->rendered_w) / 2;
        obj->rendered_y = (obj->h - obj->rendered_h) / 2;
        break;
     case ALIGN_HCENTER:
        obj->rendered_x = (obj->w - obj->rendered_w) / 2;
        obj->rendered_y = 0;
        break;
     case ALIGN_VCENTER:
        obj->rendered_x = 0;
        obj->rendered_y = (obj->h - obj->rendered_h) / 2;
        break;
     case ALIGN_LEFT:
        obj->rendered_x = 0;
        obj->rendered_y = (obj->h - obj->rendered_h) / 2;
        break;
     case ALIGN_RIGHT:
        obj->rendered_x = (obj->w - obj->rendered_w);
        obj->rendered_y = (obj->h - obj->rendered_h) / 2;
        break;
     case ALIGN_TOP:
        obj->rendered_x = (obj->w - obj->rendered_w) / 2;
        obj->rendered_y = 0;
        break;
     case ALIGN_BOTTOM:
        obj->rendered_x = (obj->w - obj->rendered_w) / 2;
        obj->rendered_y = (obj->h - obj->rendered_h);
        break;
     default:
        printf("(alignment) implement me!\n");
        break;
   }

   /*move the object up if its box becomes to small */
   if ((obj->y + obj->h) < (obj->y + obj->rendered_y + obj->rendered_h))
      obj->rendered_y = (obj->h - obj->rendered_h);

   /*move the object left if its box becomes to small */
   if ((obj->x + obj->w) < (obj->x + obj->rendered_x + obj->rendered_w))
      obj->rendered_x = (obj->w - obj->rendered_w);

   D_RETURN_(5);
}


GtkWidget *
geist_object_int_display_props(geist_object * obj)
{
   GtkWidget *box;

   box = gtk_hbox_new(FALSE, 0);
   return (box);
}

void
geist_object_get_rendered_area(geist_object * obj, int *x, int *y, int *w,
                               int *h)
{
   D_ENTER(3);

   obj->get_rendered_area(obj, x, y, w, h);

   D_RETURN_(3);
}

void
geist_object_int_get_rendered_area(geist_object * obj, int *x, int *y, int *w,
                                   int *h)
{
   D_ENTER(3);

   *x = obj->x + obj->rendered_x;
   *y = obj->y + obj->rendered_y;
   *w = obj->rendered_w;
   *h = obj->rendered_h;
   CLIP(*x, *y, *w, *h, obj->x, obj->y, obj->w, obj->h);

   D(5, ("area %d,%d %dx%d\n", *x, *y, *w, *h));

   D_RETURN_(3);
}

void
geist_object_dirty(geist_object * obj)
{
   D_ENTER(3);

   if (obj->layer)
   {
      GEIST_OBJECT_DOC(obj)->up =
         imlib_updates_append_updates(GEIST_OBJECT_DOC(obj)->up,
                                      obj->get_updates(obj));
      geist_object_dirty_selection(obj);
   }

   D_RETURN_(3);
}

Imlib_Updates geist_object_int_get_updates(geist_object * obj)
{
   int x, y, w, h;
   Imlib_Updates up;

   D_ENTER(5);

   geist_object_get_rendered_area(obj, &x, &y, &w, &h);

   D(5, ("adding dirty rect %d,%d %dx%d\n", x, y, w, h));

   up = imlib_update_append_rect(NULL, x - 1, y - 1, w + 2, h + 2);
   D_RETURN(5, up);
}

void
geist_object_dirty_selection(geist_object * obj)
{
   D_ENTER(5);

   GEIST_OBJECT_DOC(obj)->up =
      imlib_updates_append_updates(GEIST_OBJECT_DOC(obj)->up,
                                   obj->get_selection_updates(obj));

   D_RETURN_(5);
}


char *
geist_object_get_type_string(geist_object * obj)
{
   D_ENTER(3);

   D_RETURN(3, object_types[obj->type]);
}

char *
geist_object_get_sizemode_string(geist_object * obj)
{
   D_ENTER(3);

   D_RETURN(3, object_sizemodes[obj->sizemode]);
}

char *
geist_object_get_alignment_string(geist_object * obj)
{
   D_ENTER(3);

   D_RETURN(3, object_alignments[obj->alignment]);
}

int
geist_object_get_alignment_from_string(char *s)
{
   int i;

   D_ENTER(3);

   for (i = 0; i < ALIGN_MAX; i++)
   {
      if (!strcmp(object_alignments[i], s))
         D_RETURN(3, i);
   }
   D_RETURN(3, 0);
}


int
geist_object_get_sizemode_from_string(char *s)
{
   int i;

   D_ENTER(3);

   for (i = 0; i < SIZEMODE_MAX; i++)
   {
      if (!strcmp(object_sizemodes[i], s))
         D_RETURN(3, i);
   }
   D_RETURN(3, 0);
}

int
geist_object_get_type_from_string(char *s)
{
   int i;

   D_ENTER(3);

   for (i = 0; i < GEIST_TYPE_MAX; i++)
   {
      if (!strcmp(object_types[i], s))
         D_RETURN(3, i);
   }
   D_RETURN(3, 0);
}

void
geist_object_debug_print_values(geist_object * obj)
{
   printf("values: x%d y%d w%d h%d rx%d ry%d rw%d rh%d\n", obj->x, obj->y,
          obj->w, obj->h, obj->rendered_x, obj->rendered_y, obj->rendered_w,
          obj->rendered_h);
}

void
geist_object_get_clipped_render_areas(geist_object * obj, int x, int y, int w,
                                      int h, int *sx, int *sy, int *sw,
                                      int *sh, int *dx, int *dy, int *dw,
                                      int *dh)
{
   D_ENTER(3);

   if (obj->rendered_x < 0)
      *sx = x - obj->x;
   else
      *sx = x - (obj->x + obj->rendered_x);
   if (obj->rendered_y < 0)
      *sy = y - obj->y;
   else
      *sy = y - (obj->y + obj->rendered_y);

   if (*sx < 0)
      *sx = 0;
   if (*sy < 0)
      *sy = 0;

   if (obj->rendered_w > obj->w)
      *sw = obj->w - *sx;
   else
      *sw = obj->rendered_w - *sx;

   if (obj->rendered_h > obj->h)
      *sh = obj->h - *sy;
   else
      *sh = obj->rendered_h - *sy;

   if (*sw > w)
      *sw = w;
   if (*sh > h)
      *sh = h;

   if (obj->rendered_x < 0)
      *dx = obj->x + *sx;
   else
      *dx = (obj->x + obj->rendered_x) + *sx;
   if (obj->rendered_y < 0)
      *dy = obj->y + *sy;
   else
      *dy = (obj->y + obj->rendered_y) + *sy;
   *dw = *sw;
   *dh = *sh;

   D_RETURN_(3);
}

unsigned char
geist_object_click_is_selection(geist_object * obj, int x, int y)
{
   D_ENTER(3);

   D_RETURN(3, obj->click_is_selection(obj, x, y));
}

unsigned char
geist_object_int_click_is_selection(geist_object * obj, int x, int y)
{
   int ox, oy, ow, oh;

   D_ENTER(3);

   geist_object_get_rendered_area(obj, &ox, &oy, &ow, &oh);

   if (XY_IN_RECT(x, y, ox, oy, ow, oh))
   {
      if (!geist_object_part_is_transparent(obj, x - ox, y - oy))
         D_RETURN(3, 1);
   }

   D_RETURN(3, 0);
}

void
geist_object_update_dimensions_relative(geist_object * obj, int w_offset,
                                        int h_offset)
{
   D_ENTER(3);
   obj->update_dimensions_relative(obj, w_offset, h_offset);
   D_RETURN_(3);
}

void
geist_object_int_update_dimensions_relative(geist_object * obj, int w_offset,
                                            int h_offset)
{
   D_ENTER(3);
   obj->resize = RESIZE_BOTTOMRIGHT;
   w_offset += obj->x + obj->w;
   h_offset += obj->y + obj->h;
   geist_object_resize_object(obj, w_offset, h_offset);
   D_RETURN_(3);
}

void
geist_object_update_position_relative(geist_object * obj, int x_offset,
                                      int y_offset)
{
   D_ENTER(3);
   obj->update_position_relative(obj, x_offset, y_offset);
   D_RETURN_(3);
}

void
geist_object_int_update_position_relative(geist_object * obj, int x_offset,
                                          int y_offset)
{
   D_ENTER(3);
   obj->x = obj->x + x_offset;
   obj->y = obj->y + y_offset;
   D_RETURN_(3);
}

unsigned char
geist_object_has_transparency(geist_object * obj)
{
   D_ENTER(3);

   D_RETURN(3, obj->has_transparency(obj));
}

unsigned char
geist_object_int_has_transparency(geist_object * obj)
{
   D_ENTER(3);

   D_RETURN(3, TRUE);
}

void
geist_object_rotate (geist_object *obj, double angle)
{
    D_ENTER(3);

    D_RETURN(3, obj->rotate(obj, angle));
}

void
geist_object_int_rotate (geist_object *obj, double angle)
{
    D_ENTER(3);

    printf("Implement me!");
    
    D_RETURN_(3);
}
