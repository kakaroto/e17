#include "geist_object.h"

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
   obj->render_selected = geist_object_int_render_selected;
   obj->render_partial = geist_object_int_render_partial;
   obj->get_rendered_image = geist_object_int_get_rendered_image;
   obj->get_selection_updates = geist_object_int_get_selection_updates;
   obj->part_is_transparent = geist_object_int_part_is_transparent;
   obj->display_props = geist_object_int_display_props;
   obj->resize_event = geist_object_int_resize;
   obj->name = estrdup("Untitled Object");

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

geist_object_type
geist_object_get_type(geist_object * obj)
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

void
geist_object_add_to_object_list(geist_object * obj)
{
   int row = 0;
   char *list[3] = { 0, 0, 0 };

   D_ENTER(3);
   list[0] = (obj->name) ? (obj->name) : "Untitled Object";
   list[1] =
      (char *)
      geist_imlib_image_get_filename(geist_object_get_rendered_image(obj));

   gtk_signal_handler_block(GTK_OBJECT(obj_list), obj_sel_handler);
   gtk_signal_handler_block(GTK_OBJECT(obj_list), obj_unsel_handler);


   row = gtk_clist_append(GTK_CLIST(obj_list), list);
   gtk_clist_set_row_data(GTK_CLIST(obj_list), row, (gpointer) obj);

   gtk_signal_handler_unblock(GTK_OBJECT(obj_list), obj_sel_handler);
   gtk_signal_handler_unblock(GTK_OBJECT(obj_list), obj_unsel_handler);


   D_RETURN_(3);
}

Imlib_Image
geist_object_get_rendered_image(geist_object * obj)
{
   D_ENTER(5);

   D_RETURN(5, obj->get_rendered_image(obj));
}


Imlib_Image
geist_object_int_get_rendered_image(geist_object * obj)
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
   geist_object_dirty(obj);

   D_RETURN_(5);
}

void
geist_object_unselect(geist_object * obj)
{
   D_ENTER(5);

   D(4, ("unsetting object state SELECTED\n"));
   geist_object_unset_state(obj, SELECTED);
   geist_object_dirty(obj);

   D_RETURN_(5);
}

void
geist_object_int_render_selected(geist_object * obj, Imlib_Image dest,
                                 unsigned char multiple)
{
   D_ENTER(5);

   if (multiple)
   {
      geist_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);

      geist_imlib_image_draw_rectangle(dest,
                                       obj->x - HALF_SEL_WIDTH + (obj->w / 2),
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT +
                                       (obj->h / 2), 2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_draw_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT +
                                       (obj->h / 2), 2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_draw_rectangle(dest,
                                       obj->x - HALF_SEL_WIDTH + (obj->w / 2),
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
   }
   else
   {
      geist_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);

      geist_imlib_image_fill_rectangle(dest,
                                       obj->x - HALF_SEL_WIDTH + (obj->w / 2),
                                       obj->y - HALF_SEL_HEIGHT,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH + obj->w,
                                       obj->y - HALF_SEL_HEIGHT +
                                       (obj->h / 2), 2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_fill_rectangle(dest, obj->x - HALF_SEL_WIDTH,
                                       obj->y - HALF_SEL_HEIGHT +
                                       (obj->h / 2), 2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);
      geist_imlib_image_fill_rectangle(dest,
                                       obj->x - HALF_SEL_WIDTH + (obj->w / 2),
                                       obj->y - HALF_SEL_HEIGHT + obj->h,
                                       2 * HALF_SEL_WIDTH,
                                       2 * HALF_SEL_HEIGHT, 0, 0, 0, 255);

   }

   D_RETURN_(5);
}

Imlib_Updates
geist_object_int_get_selection_updates(geist_object * obj)
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

Imlib_Updates
geist_object_get_selection_updates(geist_object * obj)
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

   printf("implement me!\n");

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
        printf("implement me!\n");
        break;
   }

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
        break;
     case ALIGN_VCENTER:
        obj->rendered_y = (obj->h - obj->rendered_h) / 2;
        break;
     case ALIGN_LEFT:
        break;
     case ALIGN_RIGHT:
        break;
     case ALIGN_TOP:
        break;
     case ALIGN_BOTTOM:
        break;
     default:
        printf("implement me!\n");
        break;
   }

   D_RETURN_(5);
}

void
geist_object_display_props(geist_object * obj)
{
   D_ENTER(5);
   obj->display_props(obj);
   D_RETURN_(5);
}

void
geist_object_int_display_props(geist_object * obj)
{
   D_ENTER(5);
   printf("Nothing here yet\n");
   D_RETURN_(5);
}

void
geist_object_dirty(geist_object * obj)
{
   D_ENTER(5);

   D(5,
     ("adding dirty rect %d,%d %dx%d\n", obj->x + obj->rendered_x,
      obj->y + obj->rendered_y, obj->rendered_w, obj->rendered_h));

   GEIST_OBJECT_DOC(obj)->up =
      imlib_update_append_rect(GEIST_OBJECT_DOC(obj)->up,
                               obj->x + obj->rendered_x,
                               obj->y + obj->rendered_y, obj->rendered_w,
                               obj->rendered_h);
   geist_object_dirty_selection(obj);
   D_RETURN_(5);
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
