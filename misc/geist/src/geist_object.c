#include "geist_object.h"
#include "geist_text.h"

void refresh_x_cb(GtkWidget * widget, gpointer * obj);
void refresh_y_cb(GtkWidget * widget, gpointer * obj);
void refresh_w_cb(GtkWidget * widget, gpointer * obj);
void refresh_h_cb(GtkWidget * widget, gpointer * obj);
void refresh_name_cb(GtkWidget * widget, gpointer * obj);
void refresh_sizemode_cb (GtkWidget * widget, gpointer * obj);
void refresh_alignment_cb (GtkWidget * widget, gpointer * obj);

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

void
geist_object_display_props(geist_object * obj)
{
   D_ENTER(5);
   if (!obj->props_window)
      geist_object_show_properties(obj);
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
obj_vis_cb(GtkWidget * widget, gpointer * obj)
{
   D_ENTER(3);
   if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
         geist_object_show(GEIST_OBJECT(obj));
   else
         geist_object_hide(GEIST_OBJECT(obj));

   geist_document_render_updates( GEIST_OBJECT_DOC(GEIST_OBJECT(obj)) );
   D_RETURN_(3);
}

void
refresh_name_cb(GtkWidget * widget, gpointer * obj)
{
   D_ENTER(3);
   if (GEIST_OBJECT(obj)->name)
      efree(GEIST_OBJECT(obj)->name);
   GEIST_OBJECT(obj)->name = estrdup(gtk_entry_get_text(GTK_ENTRY(widget)));
   D_RETURN_(3);
}

void
refresh_sizemode_cb (GtkWidget * widget, gpointer * obj)
{
	D_ENTER(3);
   geist_object_dirty(GEIST_OBJECT(obj));

   geist_object_update_sizemode(GEIST_OBJECT(obj));
   geist_object_dirty(GEIST_OBJECT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   D_RETURN_(3);
}


void
refresh_alignment_cb (GtkWidget * widget, gpointer * obj)
{
   D_ENTER(3);
   geist_object_dirty(GEIST_OBJECT(obj));

   geist_object_update_sizemode(GEIST_OBJECT(obj));
   geist_object_dirty(GEIST_OBJECT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   D_RETURN_(3);
}


void
refresh_x_cb(GtkWidget * widget, gpointer * obj)
{
   D_ENTER(3);
   geist_object_dirty(GEIST_OBJECT(obj));
   GEIST_OBJECT(obj)->x =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_object_dirty(GEIST_OBJECT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   D_RETURN_(3);
}

void
refresh_y_cb(GtkWidget * widget, gpointer * obj)
{
   D_ENTER(3);
   geist_object_dirty(GEIST_OBJECT(obj));
   GEIST_OBJECT(obj)->y =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_object_dirty(GEIST_OBJECT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   D_RETURN_(3);
}

void
refresh_w_cb(GtkWidget * widget, gpointer * obj)
{
   D_ENTER(3);
   geist_object_dirty(GEIST_OBJECT(obj));
   GEIST_OBJECT(obj)->w =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_object_update_alignment(GEIST_OBJECT(obj));
   geist_object_update_sizemode(GEIST_OBJECT(obj));
   geist_object_dirty(GEIST_OBJECT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   D_RETURN_(3);
}

void
refresh_h_cb(GtkWidget * widget, gpointer * obj)
{
   D_ENTER(3);
   geist_object_dirty(GEIST_OBJECT(obj));
   GEIST_OBJECT(obj)->h =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_object_update_alignment(GEIST_OBJECT(obj));
   geist_object_update_sizemode(GEIST_OBJECT(obj));
   geist_object_dirty(GEIST_OBJECT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   D_RETURN_(3);
}

static gboolean
delete_event_cb(GtkWidget * widget, GdkEvent * event, gpointer * data)
{
   gtk_widget_destroy((GtkWidget *) GEIST_OBJECT(data)->props_window);
   GEIST_OBJECT(data)->props_window = NULL;
   return TRUE;
}

gboolean
props_ok_cb(GtkWidget * widget, gpointer * data)
{
   gtk_widget_destroy((GtkWidget *) GEIST_OBJECT(data)->props_window);
   GEIST_OBJECT(data)->props_window = NULL;
   return TRUE;
}


void
geist_object_show_properties(geist_object * obj)
{
   GtkWidget *generic_props;
   GtkWidget *table, *obj_hbox, *name_l, *hbox, *x_l, *y_l, *w_l, *h_l,

      *ok_btn;
   GtkWidget *name;
   GtkWidget *x;
   GtkWidget *y;
   GtkWidget *w;
   GtkWidget *h;
   GtkAdjustment *a1, *a2, *a3, *a4;
   GtkWidget *sizemode_l;
   GtkWidget *alignment_l;
   GtkWidget *sizemode_combo;
   GtkWidget *alignment_combo;
	GtkWidget *vis_toggle;
   GList *align_list = g_list_alloc();
	char *align_array[ALIGN_MAX];
   GList *sizemode_list = g_list_alloc();
	char *sizemode_array[SIZEMODE_MAX];
	int i;
     
   D_ENTER(3);

   a1 =
      (GtkAdjustment *) gtk_adjustment_new(0, 0, GEIST_OBJECT_DOC(obj)->w, 1,
                                           2, 3);
   a2 =
      (GtkAdjustment *) gtk_adjustment_new(0, 0, GEIST_OBJECT_DOC(obj)->h, 1,
                                           2, 3);
   a3 =
      (GtkAdjustment *) gtk_adjustment_new(0, 0, GEIST_OBJECT_DOC(obj)->w, 1,
                                           2, 3);
   a4 =
      (GtkAdjustment *) gtk_adjustment_new(0, 0, GEIST_OBJECT_DOC(obj)->h, 1,
                                           2, 3);

   generic_props = gtk_window_new(GTK_WINDOW_TOPLEVEL);

   table = gtk_table_new(4, 4, FALSE);

   gtk_container_set_border_width(GTK_CONTAINER(generic_props), 5);
   gtk_container_add(GTK_CONTAINER(generic_props), table);

	vis_toggle = gtk_check_button_new_with_label("Visible");
   gtk_table_attach(GTK_TABLE(table), vis_toggle, 0, 1, 0, 1,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
	
	gtk_widget_show(vis_toggle);

	
   name_l = gtk_label_new("Name:");
   gtk_table_attach(GTK_TABLE(table), name_l, 0, 1, 1, 2,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(name_l);

   name = gtk_entry_new();
   gtk_table_attach(GTK_TABLE(table), name, 1, 4, 1, 2	, GTK_FILL | GTK_EXPAND,
                    0, 2, 2);
   gtk_widget_show(name);

   sizemode_l = gtk_label_new("sizemode");
   gtk_table_attach(GTK_TABLE(table), sizemode_l, 0, 1, 2, 3,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(sizemode_l);

   sizemode_combo = gtk_combo_new();
   gtk_table_attach(GTK_TABLE(table), sizemode_combo, 1, 4, 2, 3,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_container_set_border_width(GTK_CONTAINER(sizemode_combo), 5);
   gtk_widget_show(sizemode_combo);

   alignment_l = gtk_label_new("alignment");
   gtk_table_attach(GTK_TABLE(table), alignment_l, 0, 1, 3, 4,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(alignment_l);

   alignment_combo = gtk_combo_new();
   gtk_table_attach(GTK_TABLE(table), alignment_combo, 1, 4, 3, 4,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_container_set_border_width(GTK_CONTAINER(alignment_combo), 5);
   gtk_widget_show(alignment_combo);

   hbox = gtk_hbox_new(FALSE, 0);
   gtk_table_attach(GTK_TABLE(table), hbox, 0, 4, 4, 5, GTK_FILL | GTK_EXPAND,
                    0, 2, 2);

   x_l = gtk_label_new("x:");
   gtk_misc_set_alignment(GTK_MISC(x_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), x_l, TRUE, FALSE, 2);
   gtk_widget_show(x_l);
   x = gtk_spin_button_new(GTK_ADJUSTMENT(a1), 1, 0);

   gtk_box_pack_start(GTK_BOX(hbox), x, TRUE, FALSE, 2);
   gtk_widget_show(x);


   y_l = gtk_label_new("y:");
   gtk_misc_set_alignment(GTK_MISC(y_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), y_l, TRUE, FALSE, 2);
   gtk_widget_show(y_l);
   y = gtk_spin_button_new(GTK_ADJUSTMENT(a3), 1, 0);
   gtk_box_pack_start(GTK_BOX(hbox), y, TRUE, FALSE, 2);
   gtk_widget_show(y);

   w_l = gtk_label_new("w:");
   gtk_misc_set_alignment(GTK_MISC(w_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), w_l, TRUE, FALSE, 2);
   gtk_widget_show(w_l);
   w = gtk_spin_button_new(GTK_ADJUSTMENT(a2), 1, 0);
   gtk_box_pack_start(GTK_BOX(hbox), w, TRUE, FALSE, 2);
   gtk_widget_show(w);


   h_l = gtk_label_new("h:");
   gtk_misc_set_alignment(GTK_MISC(h_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), h_l, TRUE, FALSE, 2);
   gtk_widget_show(h_l);
   h = gtk_spin_button_new(GTK_ADJUSTMENT(a4), 1, 0);
   gtk_box_pack_start(GTK_BOX(hbox), h, TRUE, FALSE, 2);
   gtk_widget_show(h);

   gtk_container_set_border_width(GTK_CONTAINER(hbox), 5);

   gtk_widget_show(hbox);
	
   /*Import the object type specific properties */
   obj_hbox = obj->display_props(obj);

   /*Assign the window to the opbject */
   obj->props_window = generic_props;
 
   gtk_table_attach(GTK_TABLE(table), obj_hbox, 0, 4, 5, 6, GTK_FILL | GTK_EXPAND, 0, 2, 2);

   ok_btn = gtk_button_new_with_label("Ok");     
   gtk_table_attach(GTK_TABLE(table), ok_btn, 1, 3, 6, 7, GTK_FILL | GTK_EXPAND, 0, 2, 2);
   
   gtk_widget_show (obj_hbox);
   gtk_widget_show (ok_btn);

   gtk_widget_show(table);

   gtk_spin_button_set_value(GTK_SPIN_BUTTON(x), obj->x);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(y), obj->y);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(w), obj->w);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(h), obj->h);

   
	align_array[ALIGN_NONE] = "No alignment";
	align_array[ALIGN_HCENTER] = "Center horizontally";
	align_array[ALIGN_VCENTER] = "Center vertically";
	align_array[ALIGN_CENTER] = "Center";
	align_array[ALIGN_LEFT] = "align left";
	align_array[ALIGN_RIGHT] = "align right";
	align_array[ALIGN_TOP] = "align top";
	align_array[ALIGN_BOTTOM] = "align bottom";
	
	for (i=0; i<ALIGN_MAX; i++)
	{
		align_list = g_list_append(align_list, align_array[i]);
	}
	
	gtk_combo_set_popdown_strings(GTK_COMBO(alignment_combo), align_list);
	
	
	sizemode_array[SIZEMODE_NONE] = "none";
	sizemode_array[SIZEMODE_ZOOM] = "Zoom on resize";
	sizemode_array[SIZEMODE_STRETCH] = "Strech on Resize";
	
	for (i=0; i<SIZEMODE_MAX; i++)
	{
		align_list = g_list_append(sizemode_list, sizemode_array[i]);
	}
	
	gtk_combo_set_popdown_strings(GTK_COMBO(sizemode_combo), sizemode_list);
	
	if (geist_object_get_state(obj, VISIBLE))
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(vis_toggle), TRUE);
	
   if (obj->name)
      gtk_entry_set_text(GTK_ENTRY(name), obj->name);
	
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(alignment_combo)->entry),
							 align_array[obj->alignment]);
	gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(sizemode_combo)->entry),
							 sizemode_array[obj->sizemode]);
	
	
	gtk_signal_connect(GTK_OBJECT(vis_toggle), "clicked", 
							 GTK_SIGNAL_FUNC(obj_vis_cb), (gpointer) obj);
	
   gtk_signal_connect(GTK_OBJECT(x), "changed", GTK_SIGNAL_FUNC(refresh_x_cb),
                      (gpointer) obj);
   gtk_signal_connect(GTK_OBJECT(y), "changed", GTK_SIGNAL_FUNC(refresh_y_cb),
                      (gpointer) obj);
   gtk_signal_connect(GTK_OBJECT(w), "changed", GTK_SIGNAL_FUNC(refresh_w_cb),
                      (gpointer) obj);
   gtk_signal_connect(GTK_OBJECT(h), "changed", GTK_SIGNAL_FUNC(refresh_h_cb),
                      (gpointer) obj);
   gtk_signal_connect(GTK_OBJECT(name), "changed",
                      GTK_SIGNAL_FUNC(refresh_name_cb), (gpointer) obj);
	
	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(alignment_combo)->entry), "changed",
                      GTK_SIGNAL_FUNC(refresh_alignment_cb), (gpointer) obj);
	gtk_signal_connect(GTK_OBJECT(GTK_COMBO(sizemode_combo)->entry), "changed",
                      GTK_SIGNAL_FUNC(refresh_sizemode_cb), (gpointer) obj);
 

   gtk_signal_connect(GTK_OBJECT(ok_btn), "clicked",
                      GTK_SIGNAL_FUNC(props_ok_cb), (gpointer) obj);

   gtk_signal_connect(GTK_OBJECT(generic_props), "delete_event",
                      GTK_SIGNAL_FUNC(delete_event_cb), (gpointer) obj);

   gtk_widget_show(generic_props);

}
