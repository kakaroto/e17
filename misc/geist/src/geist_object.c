#include "geist_object.h"
#include "geist_text.h"

void refresh_x_cb(GtkWidget * widget, gpointer * obj);
void refresh_y_cb(GtkWidget * widget, gpointer * obj);
void refresh_w_cb(GtkWidget * widget, gpointer * obj);
void refresh_h_cb(GtkWidget * widget, gpointer * obj);
void refresh_name_cb(GtkWidget * widget, gpointer * obj);
void refresh_sizemode_cb(GtkWidget * widget, gpointer * obj);
void refresh_alignment_cb(GtkWidget * widget, gpointer * obj);


char *object_types[] = {
   "None",
   "Image",
   "Text",
   "Rect",
   "XXXXX"
};

char *object_sizemodes[] = {
   "None",
   "Zoom",
   "Stretch",
   "XXXXX"
};

char *object_alignments[] = {
   "None",
   "Center Horizontal",
   "Center Vertical",
   "Center Both",
   "Left",
   "Right",
   "Top",
   "Bottom",
   "XXXXX"
};


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
        printf("implement me!\n");
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


GtkWidget *
geist_object_int_display_props(geist_object * obj)
{
   GtkWidget *box;

   box = gtk_hbox_new(FALSE, 0);
   return (box);
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



static void
obj_vis_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   geist_list *l = NULL;
   geist_list *list = (geist_list *) data;

   D_ENTER(3);

   if (geist_list_length(list) > 1)
   {
      for (l = list; l; l = l->next)
      {
         obj = l->data;

         if (geist_object_get_state(obj, VISIBLE))
            geist_object_hide(obj);
         else
            geist_object_show(obj);
      }
   }
   else
   {
      obj = list->data;
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
         geist_object_show(obj);
      else
         geist_object_hide(obj);
   }
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   efree(l);
   D_RETURN_(3);
}

void
refresh_name_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   geist_list *list = (geist_list *) data;

   D_ENTER(3);

   if (geist_list_length(list) > 1)
      printf("Implement me!\n");
   else
   {
      obj = list->data;
      if (obj->name)
         efree(obj->name);

      obj->name = estrdup(gtk_entry_get_text(GTK_ENTRY(widget)));
   }
   D_RETURN_(3);
}

void
refresh_sizemode_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   geist_list *l = NULL;
   geist_list *list = (geist_list *) data;

   D_ENTER(3);

   for (l = list; l; l = l->next)
   {
      obj = l->data;
      geist_object_dirty(obj);
      obj->sizemode =
         geist_object_get_sizemode_from_string(gtk_entry_get_text
                                               (GTK_ENTRY(widget)));
      geist_object_update_sizemode(obj);
      geist_object_dirty(obj);
   }
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   efree(l);
   D_RETURN_(3);
}


void
refresh_alignment_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   geist_list *l = NULL;
   geist_list *list = (geist_list *) data;

   D_ENTER(3);

   for (l = list; l; l = l->next)
   {
      obj = l->data;
      geist_object_dirty(obj);
      obj->alignment =
         geist_object_get_alignment_from_string(gtk_entry_get_text
                                                (GTK_ENTRY(widget)));
      geist_object_update_alignment(obj);
      geist_object_dirty(obj);
   }
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   efree(l);
   D_RETURN_(3);
}


void
move_up_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   geist_list *l = NULL;
   geist_list *list = (geist_list *) data;

   D_ENTER(3);

   for (l = list; l; l = l->next)
   {
      obj = l->data;
      geist_object_dirty(obj);
      obj->y = obj->y - 1;
      geist_object_dirty(obj);
   }
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   efree(l);
   D_RETURN_(3);
}

void
move_down_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   geist_list *l = NULL;
   geist_list *list = (geist_list *) data;

   D_ENTER(3);

   for (l = list; l; l = l->next)
   {
      obj = l->data;
      geist_object_dirty(obj);
      obj->y++;
      geist_object_dirty(obj);
   }
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   efree(l);
   D_RETURN_(3);
}

void
move_left_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   geist_list *l = NULL;
   geist_list *list = (geist_list *) data;

   D_ENTER(3);

   for (l = list; l; l = l->next)
   {
      obj = l->data;
      geist_object_dirty(obj);
      obj->x--;
      geist_object_dirty(obj);
   }
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   efree(l);
   D_RETURN_(3);
}

void
move_right_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   geist_list *l = NULL;
   geist_list *list = (geist_list *) data;

   D_ENTER(3);

   for (l = list; l; l = l->next)
   {
      obj = l->data;
      geist_object_dirty(obj);
      obj->x++;
      geist_object_dirty(obj);
   }
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   efree(l);
   D_RETURN_(3);
}

void
height_plus_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   geist_list *l = NULL;
   geist_list *list = (geist_list *) data;

   D_ENTER(3);

   for (l = list; l; l = l->next)
   {
      obj = l->data;
      geist_object_dirty(obj);
      obj->h++;
      geist_object_update_alignment(obj);
      geist_object_update_sizemode(obj);
      geist_object_dirty(obj);
   }
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   efree(l);
   D_RETURN_(3);
}


void
height_minus_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   geist_list *l = NULL;
   geist_list *list = (geist_list *) data;

   D_ENTER(3);

   for (l = list; l; l = l->next)
   {
      obj = l->data;
      geist_object_dirty(obj);
      obj->h--;
      geist_object_update_alignment(obj);
      geist_object_update_sizemode(obj);
      geist_object_dirty(obj);
   }
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   efree(l);
   D_RETURN_(3);
}


void
width_minus_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   geist_list *l = NULL;
   geist_list *list = (geist_list *) data;

   D_ENTER(3);

   for (l = list; l; l = l->next)
   {
      obj = l->data;
      geist_object_dirty(obj);
      obj->w--;
      geist_object_update_alignment(obj);
      geist_object_update_sizemode(obj);
      geist_object_dirty(obj);
   }
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   efree(l);
   D_RETURN_(3);
}

void
width_plus_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   geist_list *l = NULL;
   geist_list *list = (geist_list *) data;

   D_ENTER(3);

   for (l = list; l; l = l->next)
   {
      obj = l->data;
      geist_object_dirty(obj);
      obj->w++;
      geist_object_update_alignment(obj);
      geist_object_update_sizemode(obj);
      geist_object_dirty(obj);
   }
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   efree(l);
   D_RETURN_(3);
}


GtkWidget *
geist_object_generic_properties(geist_list * list)
{
   GtkWidget *gen_props;
   GtkWidget *table, *name_l;
   GtkWidget *name;
   GtkWidget *up, *down, *left, *right;
   GtkWidget *width_plus, *width_minus, *height_plus, *height_minus;
   GtkWidget *sizemode_l;
   GtkWidget *alignment_l;
   GtkWidget *sizemode_combo;
   GtkWidget *alignment_combo;
   GtkWidget *vis_toggle;
   GList *align_list = g_list_alloc();
   GList *sizemode_list = g_list_alloc();
   int i;
   geist_object *obj, *obj_first;
   geist_list *l;
   char *align_string = NULL;
   char *sizemode_string = NULL;

   D_ENTER(3);

   gen_props = gtk_hbox_new(FALSE, 0);

   table = gtk_table_new(8, 6, FALSE);

   gtk_container_set_border_width(GTK_CONTAINER(gen_props), 5);
   gtk_container_add(GTK_CONTAINER(gen_props), table);

   vis_toggle = gtk_check_button_new_with_label("Visible");
   gtk_table_attach(GTK_TABLE(table), vis_toggle, 0, 4, 0, 1,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);

   gtk_widget_show(vis_toggle);


   name_l = gtk_label_new("Name:");
   gtk_table_attach(GTK_TABLE(table), name_l, 0, 1, 1, 2,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(name_l);

   name = gtk_entry_new();
   gtk_table_attach(GTK_TABLE(table), name, 1, 6, 1, 2, GTK_FILL | GTK_EXPAND,
                    0, 2, 2);
   gtk_widget_show(name);

   sizemode_l = gtk_label_new("sizemode");
   gtk_table_attach(GTK_TABLE(table), sizemode_l, 0, 1, 2, 3,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(sizemode_l);

   sizemode_combo = gtk_combo_new();
   gtk_table_attach(GTK_TABLE(table), sizemode_combo, 1, 6, 2, 3,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_container_set_border_width(GTK_CONTAINER(sizemode_combo), 5);
   gtk_widget_show(sizemode_combo);

   alignment_l = gtk_label_new("alignment");
   gtk_table_attach(GTK_TABLE(table), alignment_l, 0, 1, 3, 4,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(alignment_l);

   alignment_combo = gtk_combo_new();
   gtk_table_attach(GTK_TABLE(table), alignment_combo, 1, 6, 3, 4,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_container_set_border_width(GTK_CONTAINER(alignment_combo), 5);
   gtk_widget_show(alignment_combo);

   up = gtk_button_new_with_label("Up");
   gtk_table_attach(GTK_TABLE(table), up, 1, 2, 4, 5, GTK_FILL | GTK_EXPAND,
                    0, 2, 2);
   gtk_widget_show(up);

   down = gtk_button_new_with_label("Down");
   gtk_table_attach(GTK_TABLE(table), down, 1, 2, 6, 7, GTK_FILL | GTK_EXPAND,
                    0, 2, 2);
   gtk_widget_show(down);

   left = gtk_button_new_with_label("Left");
   gtk_table_attach(GTK_TABLE(table), left, 0, 1, 5, 6, GTK_FILL | GTK_EXPAND,
                    0, 2, 2);
   gtk_widget_show(left);

   right = gtk_button_new_with_label("Right");
   gtk_table_attach(GTK_TABLE(table), right, 2, 3, 5, 6,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(right);


   width_plus = gtk_button_new_with_label("Width +");
   gtk_table_attach(GTK_TABLE(table), width_plus, 5, 6, 5, 6,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(width_plus);

   width_minus = gtk_button_new_with_label("Width -");
   gtk_table_attach(GTK_TABLE(table), width_minus, 3, 4, 5, 6,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(width_minus);

   height_plus = gtk_button_new_with_label("Height +");
   gtk_table_attach(GTK_TABLE(table), height_plus, 4, 5, 4, 5,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(height_plus);

   height_minus = gtk_button_new_with_label("Height -");
   gtk_table_attach(GTK_TABLE(table), height_minus, 4, 5, 6, 7,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(height_minus);

   gtk_widget_show(table);


   for (i = 0; i < ALIGN_MAX; i++)
   {
      align_list = g_list_append(align_list, object_alignments[i]);
   }

   gtk_combo_set_popdown_strings(GTK_COMBO(alignment_combo), align_list);


   for (i = 0; i < SIZEMODE_MAX; i++)
   {
      align_list = g_list_append(sizemode_list, object_sizemodes[i]);
   }

   gtk_combo_set_popdown_strings(GTK_COMBO(sizemode_combo), sizemode_list);

   if (geist_list_length(list) > 1)
   {
      /*grey out the name entry box */
      gtk_widget_set_sensitive(GTK_WIDGET(name), FALSE);

      obj_first = list->data;

      /*check wether all objects have the same alignment or sizemode, and if
         so, set the combo boxes, if not leave them empty */
      for (l = list; l; l = l->next)
      {
         obj = l->data;
         if (obj->alignment == obj_first->alignment)
            align_string = geist_object_get_alignment_string(obj);
         else
            align_string = "";

         if (obj->sizemode == obj_first->sizemode)
            sizemode_string = geist_object_get_sizemode_string(obj);
         else
            sizemode_string = "";
      }
      gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(alignment_combo)->entry),
                         align_string);
      gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sizemode_combo)->entry),
                         sizemode_string);
   }
   else
   {
      obj = list->data;


      if (obj->name)
         gtk_entry_set_text(GTK_ENTRY(name), obj->name);

      gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(alignment_combo)->entry),
                         geist_object_get_alignment_string(obj));
      gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sizemode_combo)->entry),
                         geist_object_get_sizemode_string(obj));

      if (geist_object_get_state(obj, VISIBLE))
         gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(vis_toggle), TRUE);
   }

   gtk_signal_connect(GTK_OBJECT(vis_toggle), "clicked",
                      GTK_SIGNAL_FUNC(obj_vis_cb), (gpointer) list);

   gtk_signal_connect(GTK_OBJECT(up), "clicked", GTK_SIGNAL_FUNC(move_up_cb),
                      (gpointer) list);
   gtk_signal_connect(GTK_OBJECT(down), "clicked",
                      GTK_SIGNAL_FUNC(move_down_cb), (gpointer) list);
   gtk_signal_connect(GTK_OBJECT(right), "clicked",
                      GTK_SIGNAL_FUNC(move_right_cb), (gpointer) list);
   gtk_signal_connect(GTK_OBJECT(left), "clicked",
                      GTK_SIGNAL_FUNC(move_left_cb), (gpointer) list);

   gtk_signal_connect(GTK_OBJECT(height_plus), "clicked",
                      GTK_SIGNAL_FUNC(height_plus_cb), (gpointer) list);
   gtk_signal_connect(GTK_OBJECT(height_minus), "clicked",
                      GTK_SIGNAL_FUNC(height_minus_cb), (gpointer) list);
   gtk_signal_connect(GTK_OBJECT(width_plus), "clicked",
                      GTK_SIGNAL_FUNC(width_plus_cb), (gpointer) list);
   gtk_signal_connect(GTK_OBJECT(width_minus), "clicked",
                      GTK_SIGNAL_FUNC(width_minus_cb), (gpointer) list);

   gtk_signal_connect(GTK_OBJECT(name), "changed",
                      GTK_SIGNAL_FUNC(refresh_name_cb), (gpointer) list);

   gtk_signal_connect(GTK_OBJECT(GTK_COMBO(alignment_combo)->entry),
                      "changed", GTK_SIGNAL_FUNC(refresh_alignment_cb),
                      (gpointer) list);
   gtk_signal_connect(GTK_OBJECT(GTK_COMBO(sizemode_combo)->entry), "changed",
                      GTK_SIGNAL_FUNC(refresh_sizemode_cb), (gpointer) list);

   return (gen_props);
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
