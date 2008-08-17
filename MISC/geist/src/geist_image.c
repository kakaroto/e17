/* geist_image.c

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

#include "geist.h"
#include "gib_imlib.h"
#include "geist_image.h"

static gboolean img_load_cancel_cb(GtkWidget * widget, gpointer data);

typedef struct _cb_data cb_data;
struct _cb_data
{
   geist_object *obj;
   GtkWidget *dialog;
};

geist_object *
geist_image_new(void)
{
   geist_image *img;

   D_ENTER(5);

   img = emalloc(sizeof(geist_image));
   geist_image_init(img);

   geist_object_set_state(GEIST_OBJECT(img), VISIBLE);

   D_RETURN(5, (geist_object *) img);
}

void
geist_image_init(geist_image * img)
{
   geist_object *obj;

   D_ENTER(5);
   memset(img, 0, sizeof(geist_image));
   obj = GEIST_OBJECT(img);
   geist_object_init(obj);
   obj->free = geist_image_free;
   obj->render = geist_image_render;
   obj->render_partial = geist_image_render_partial;
   obj->get_rendered_image = geist_image_get_rendered_image;
   obj->duplicate = geist_image_duplicate;
   obj->resize_event = geist_image_resize;
   obj->update_positioning = geist_image_update_positioning;
   geist_object_set_type(obj, GEIST_TYPE_IMAGE);
   obj->sizemode = SIZEMODE_ZOOM;
   obj->alignment = ALIGN_CENTER;
   obj->display_props = geist_image_display_props;
   obj->get_object_list_entry = geist_image_get_object_list_entry;
   obj->has_transparency = geist_image_has_transparency;
   obj->rotate = geist_image_rotate;
   img->last.image_mods[R] = 0;
   img->last.image_mods[G] = 0;
   img->last.image_mods[B] = 0;
   img->last.image_mods[A] = 0;
   img->image_mods[R] = FULL_COLOR;
   img->image_mods[G] = FULL_COLOR;
   img->image_mods[B] = FULL_COLOR;
   img->image_mods[A] = FULL_OPACITY;

   D_RETURN_(5);
}

geist_object *
geist_image_new_from_file(int x, int y, char *filename)
{
   geist_image *img;
   geist_object *obj;
   char *txt;

   D_ENTER(5);

   obj = geist_image_new();
   img = (geist_image *) obj;

   if (!(geist_image_load_file(img, filename)))
   {
      geist_image_free(obj);
      D_RETURN(5, NULL);
   }

   img->filename = estrdup(filename);

   efree(obj->name);
   if ((txt = strrchr(img->filename, '/') + 1) != NULL)
      obj->name = estrdup(txt);
   else
      obj->name = estrdup(txt);

   obj->x = x;
   obj->y = y;
   obj->rendered_x = 0;
   obj->rendered_y = 0;

   D_RETURN(5, (geist_object *) img);
}

void
geist_image_free(geist_object * obj)
{
   geist_image *img;

   D_ENTER(5);

   img = (geist_image *) obj;

   if (!img)
      D_RETURN_(5);

   if (img->filename)
      efree(img->filename);
   if (img->im)
      gib_imlib_free_image(img->im);

   efree(img);

   D_RETURN_(5);
}

void
geist_image_render(geist_object * obj, Imlib_Image dest)
{
   geist_image *im;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   im = (geist_image *) obj;
   if (!im->im)
      D_RETURN_(5);


   /*
      dw = gib_imlib_image_get_width(dest);
      dh = gib_imlib_image_get_height(dest);
      sw = gib_imlib_image_get_width(im->im);
      sh = gib_imlib_image_get_height(im->im);

      D(3, ("Rendering image %p with filename %s\n", obj, im->filename));
      gib_imlib_blend_image_onto_image(dest, im->im, 0, 0, 0, sw, sh, obj->x,
      obj->y, sw, sh, 1,
      gib_imlib_image_has_alpha(im->im),
      obj->alias);
    */

   /* just render to the full size of the object */
   geist_image_render_partial(obj, dest, obj->x, obj->y, obj->w, obj->h);
   D_RETURN_(5);
}

void
geist_image_render_partial(geist_object * obj, Imlib_Image dest, int x, int y,
                           int w, int h)
{
   geist_image *im;
   int sw, sh, dw, dh, sx, sy, dx, dy;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   im = GEIST_IMAGE(obj);
   if (!im->im)
      D_RETURN_(5);

   geist_object_get_rendered_area(obj, &dx, &dy, &dw, &dh);
   CLIP(dx, dy, dw, dh, x, y, w, h);
   sx = dx - obj->x - obj->rendered_x;
   sy = dy - obj->y - obj->rendered_y;
   sw = dw;
   sh = dh;

#if 0
   geist_object_get_clipped_render_areas(obj, x, y, w, h, &sx, &sy, &sw, &sh,
                                         &dx, &dy, &dw, &dh);

#endif
   D(5,
     ("Rendering image from:\n" "x: %d y: %d\nobj->x: %d obj->y %d\n"
      "area:\nsx: %d sy: %d\n" "sw: %d sh: %d\ndx: %d dy: %d\n"
      "dw: %d dh: %d\n", x, y, obj->x, obj->y, sx, sy, sw, sh, dx, dy, dw,
      dh));

   D(3, ("Rendering partial image %s\n", im->filename));
   gib_imlib_blend_image_onto_image(dest, im->im, 0, sx, sy, sw, sh, dx, dy,
                                      dw, dh, 1,
                                      gib_imlib_image_has_alpha(im->im),
                                      obj->alias);

   D_RETURN_(5);
}


int
geist_image_load_file(geist_image * img, char *filename)
{
   geist_object *obj;
   int ret;

   D_ENTER(5);

   if (img->im)
      gib_imlib_free_image(img->im);

   ret = geist_imlib_load_image(&img->im, filename);

   if (ret)
   {
      obj = (geist_object *) img;

      if (img->orig_im)
         gib_imlib_free_image(img->orig_im);
      obj->w = obj->rendered_w = gib_imlib_image_get_width(img->im);
      obj->h = obj->rendered_h = gib_imlib_image_get_height(img->im);
   }

   D_RETURN(5, ret);
}

Imlib_Image
geist_image_get_rendered_image(geist_object * obj)
{
   D_ENTER(3);

   D_RETURN(3, GEIST_IMAGE(obj)->im);
}

geist_object *
geist_image_duplicate(geist_object * obj)
{
   int i;
   geist_object *ret;
   geist_image *img;

   D_ENTER(3);

   img = GEIST_IMAGE(obj);

   ret = geist_image_new_from_file(obj->x, obj->y, img->filename);
   if (ret)
   {
      ret->rendered_x = obj->rendered_x;
      ret->rendered_y = obj->rendered_y;
      ret->h = obj->h;
      ret->w = obj->w;
      for (i = 0; i < 4; i++)
      {
         GEIST_IMAGE(ret)->image_mods[i] = img->image_mods[i];
      }
      ret->state = obj->state;
      ret->alias = obj->alias;
      ret->name =
         g_strjoin(" ", "Copy of", obj->name ? obj->name : "Untitled object",
                   NULL);
      geist_object_update_positioning(GEIST_OBJECT(ret));
   }

   D_RETURN(3, ret);
}

void
geist_image_resize(geist_object * obj, int x, int y)
{
   geist_image *img;

   D_ENTER(5);

   img = GEIST_IMAGE(obj);

   x += obj->clicked_x;
   y += obj->clicked_y;

   D(5, ("resize to %d,%d\n", x, y));
   geist_object_resize_object(obj, x, y);
   geist_object_update_positioning(obj);

   D_RETURN_(5);
}

void
refresh_image_mods_cb(GtkWidget * widget, gpointer * type)
{
   int p[4], i;
   gib_list *list, *l;
   geist_object *obj = NULL;

   D_ENTER(3);

   /*this works for multiple objects although we wont have that situation for
      now, but that might change somewhere along the line so there :) */

   list = geist_document_get_selected_list(current_doc);
   if (list)
   {
      for (l = list; l; l = l->next)
      {
         obj = l->data;
         for (i = 0; i < 4; i++)
         {
            p[i] = GEIST_IMAGE(obj)->image_mods[i];
         }
         p[GPOINTER_TO_INT(type)] =
            gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
         geist_image_change_image_mods(GEIST_OBJECT(obj), p);
         geist_object_dirty(GEIST_OBJECT(obj));
      }
   }
   gib_list_free(list);
   geist_document_render_updates(GEIST_OBJECT_DOC(obj),1);
   D_RETURN_(3);
}


static gboolean
img_load_cb(GtkWidget * widget, gpointer data)
{
   geist_object *obj = ((cb_data *) data)->obj;
   char *path;

   D_ENTER(3);

   path =
      gtk_file_selection_get_filename(GTK_FILE_SELECTION
                                      (((cb_data *) data)->dialog));

   if (path)
   {
      geist_object_dirty(obj);
      geist_image_load_file(GEIST_IMAGE(obj), path);
      geist_document_render_updates(GEIST_OBJECT_DOC(obj),1);
      geist_object_dirty(obj);
   }
   gtk_widget_destroy((GtkWidget *) ((cb_data *) data)->dialog);
   efree(data);
   D_RETURN(3, TRUE);
}

static gboolean
img_load_cancel_cb(GtkWidget * widget, gpointer data)
{
   gtk_widget_destroy((GtkWidget *) data);
   return TRUE;
}



gboolean
geist_image_select_file_cb(GtkWidget * widget, gpointer * data)
{
   cb_data *sel_cb_data = NULL;
   geist_object *obj = GEIST_OBJECT(data);
   GtkWidget *file_sel = gtk_file_selection_new("Select an Image");

   sel_cb_data = emalloc(sizeof(cb_data));
   sel_cb_data->obj = obj;
   sel_cb_data->dialog = (gpointer) file_sel;

   gtk_file_selection_show_fileop_buttons(GTK_FILE_SELECTION(file_sel));
   gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(file_sel)->ok_button),
                      "clicked", GTK_SIGNAL_FUNC(img_load_cb),
                      (gpointer) sel_cb_data);
   gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(file_sel)->cancel_button),
                      "clicked", GTK_SIGNAL_FUNC(img_load_cancel_cb),
                      (gpointer) file_sel);
   gtk_widget_show(file_sel);
   return TRUE;
}

gboolean refresh_aa_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj = NULL;
   gib_list *l = NULL;
   gib_list *list = NULL;

   D_ENTER(3);

   list = geist_document_get_selected_list(current_doc);
   if (list)
   {
      for (l = list; l; l = l->next)
      {
         obj = l->data;
         obj->alias = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
         geist_object_dirty(obj);
         geist_object_update_positioning(obj);
         geist_object_dirty(obj);
      }
      gib_list_free(list);
      geist_document_render_updates(GEIST_OBJECT_DOC(obj),1);
   }
   D_RETURN(3, TRUE);
}


GtkWidget *
geist_image_display_props(geist_object * obj)
{
   GtkWidget *image_props;
   GtkWidget *table;
   GtkWidget *sel_file_btn;
   GtkWidget *file_entry;
   GtkWidget *antialias_checkb;
   GtkAdjustment *ao, *ar, *ag, *ab;
   GtkWidget *imo, *imo_l;
   GtkWidget *cr_l, *cr, *cg_l, *cg, *cb_l, *cb, *hbox;

   image_props = gtk_hbox_new(FALSE, 0);

   ao = (GtkAdjustment *) gtk_adjustment_new(0, 0, 100, 1, 1, 1);
   ar = (GtkAdjustment *) gtk_adjustment_new(0, 0, 100, 1, 1, 1);
   ag = (GtkAdjustment *) gtk_adjustment_new(0, 0, 100, 1, 1, 1);
   ab = (GtkAdjustment *) gtk_adjustment_new(0, 0, 100, 1, 1, 1);

   table = gtk_table_new(3, 2, FALSE);
   gtk_widget_show(table);
   gtk_container_add(GTK_CONTAINER(image_props), table);
   gtk_container_set_border_width(GTK_CONTAINER(image_props), 5);

   sel_file_btn = gtk_button_new_with_label("Change file");
   gtk_table_attach(GTK_TABLE(table), sel_file_btn, 0, 1, 0, 1,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_container_set_border_width(GTK_CONTAINER(sel_file_btn), 10);
   gtk_widget_show(sel_file_btn);

   file_entry = gtk_entry_new();
   gtk_table_attach(GTK_TABLE(table), file_entry, 1, 2, 0, 1,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(file_entry);


   antialias_checkb = gtk_check_button_new_with_label("antialias");
   gtk_table_attach(GTK_TABLE(table), antialias_checkb, 0, 1, 1, 2,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_signal_connect(GTK_OBJECT(antialias_checkb), "clicked",
                      GTK_SIGNAL_FUNC(refresh_aa_cb), NULL);
   gtk_container_set_border_width(GTK_CONTAINER(antialias_checkb), 10);
   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(antialias_checkb),
                                obj->alias);
   gtk_widget_show(antialias_checkb);

   gtk_entry_set_text(GTK_ENTRY(file_entry), GEIST_IMAGE(obj)->filename);
   gtk_signal_connect(GTK_OBJECT(sel_file_btn), "clicked",
                      GTK_SIGNAL_FUNC(geist_image_select_file_cb),
                      (gpointer) obj);


   imo_l = gtk_label_new("Opacity %:");
   gtk_misc_set_alignment(GTK_MISC(imo_l), 1.0, 0.5);
   gtk_table_attach(GTK_TABLE(table), imo_l, 1, 2, 1, 2,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(imo_l);

   imo = gtk_spin_button_new(GTK_ADJUSTMENT(ao), 1, 0);
   gtk_table_attach(GTK_TABLE(table), imo, 2, 3, 1, 2, GTK_FILL | GTK_EXPAND,
                    0, 2, 2);
   gtk_widget_show(imo);

   hbox = gtk_hbox_new(FALSE, 0);

   cr_l = gtk_label_new("Red %:");
   gtk_misc_set_alignment(GTK_MISC(cr_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), cr_l, TRUE, FALSE, 2);
   gtk_widget_show(cr_l);

   cr = gtk_spin_button_new(GTK_ADJUSTMENT(ar), 1, 0);
   gtk_box_pack_start(GTK_BOX(hbox), cr, TRUE, FALSE, 2);
   gtk_widget_show(cr);


   cg_l = gtk_label_new("Green %:");
   gtk_misc_set_alignment(GTK_MISC(cg_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), cg_l, TRUE, FALSE, 2);
   gtk_widget_show(cg_l);

   cg = gtk_spin_button_new(GTK_ADJUSTMENT(ag), 1, 0);
   gtk_box_pack_start(GTK_BOX(hbox), cg, TRUE, FALSE, 2);
   gtk_widget_show(cg);


   cb_l = gtk_label_new("Blue %:");
   gtk_misc_set_alignment(GTK_MISC(cb_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), cb_l, TRUE, FALSE, 2);
   gtk_widget_show(cb_l);

   cb = gtk_spin_button_new(GTK_ADJUSTMENT(ab), 1, 0);
   gtk_box_pack_start(GTK_BOX(hbox), cb, TRUE, FALSE, 2);
   gtk_widget_show(cb);

   gtk_table_attach(GTK_TABLE(table), hbox, 0, 3, 2, 3, GTK_FILL | GTK_EXPAND,
                    0, 2, 2);
   gtk_widget_show(hbox);

   gtk_spin_button_set_value(GTK_SPIN_BUTTON(imo),
                             GEIST_IMAGE(obj)->image_mods[A]);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(cr),
                             GEIST_IMAGE(obj)->image_mods[R]);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(cg),
                             GEIST_IMAGE(obj)->image_mods[G]);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(cb),
                             GEIST_IMAGE(obj)->image_mods[B]);

   gtk_signal_connect(GTK_OBJECT(imo), "changed",
                      GTK_SIGNAL_FUNC(refresh_image_mods_cb), (gpointer) A);
   gtk_signal_connect(GTK_OBJECT(cr), "changed",
                      GTK_SIGNAL_FUNC(refresh_image_mods_cb), (gpointer) R);
   gtk_signal_connect(GTK_OBJECT(cg), "changed",
                      GTK_SIGNAL_FUNC(refresh_image_mods_cb), (gpointer) G);
   gtk_signal_connect(GTK_OBJECT(cb), "changed",
                      GTK_SIGNAL_FUNC(refresh_image_mods_cb), (gpointer) B);

   gtk_widget_show(imo);
   return (image_props);

}

void
geist_image_apply_image_mods(geist_object * obj)
{
   geist_image *img;
   int has_resized = 0;
   int w, h, i;
   double ra, ha, rr, hr, rg, hg, rb, hb;
   DATA8 atab[256], rtab[256], gtab[256], btab[256];

   D_ENTER(3);

   img = GEIST_IMAGE(obj);

   w = gib_imlib_image_get_width(img->im);
   h = gib_imlib_image_get_height(img->im);

   if ((obj->rendered_w != w) || (obj->rendered_h != h)
       || (obj->alias != obj->last.alias) || (img->orig_im
                                              &&
                                              ((gib_imlib_image_get_width
                                                (img->orig_im) !=
                                                obj->rendered_w)
                                               ||
                                               (gib_imlib_image_get_height
                                                (img->orig_im) !=
                                                obj->rendered_h))
                                              &&
                                              ((img->image_mods
                                                [A] != FULL_OPACITY)
                                               || (img->image_mods[R] != 100)
                                               || (img->image_mods[G] != 100)
                                               || (img->image_mods[B] !=
                                                   100))))
   {
      obj->last.alias = obj->alias;
      /* need to resize */
      if (!img->orig_im)
      {
         img->orig_im = gib_imlib_clone_image(img->im);
      }
      else
      {
         w = gib_imlib_image_get_width(img->orig_im);
         h = gib_imlib_image_get_height(img->orig_im);
      }
      has_resized = 1;
      gib_imlib_free_image_and_decache(img->im);
      img->im =
         gib_imlib_create_cropped_scaled_image(img->orig_im, 0, 0, w, h,
                                                 obj->rendered_w,
                                                 obj->rendered_h, obj->alias);
   }

   if (
       ((img->image_mods[A] != FULL_OPACITY)
        && (img->image_mods[A] != img->last.image_mods[A]))
       || ((img->image_mods[R] != FULL_COLOR)
           && (img->image_mods[R] != img->last.image_mods[R]))
       || ((img->image_mods[G] != FULL_COLOR)
           && (img->image_mods[G] != img->last.image_mods[G]))
       || ((img->image_mods[B] != FULL_COLOR)
           && (img->image_mods[B] != img->last.image_mods[B]))
       || (has_resized))
   {
      D(5, ("need to do opacity, it's %d\n", img->image_mods[A]));
      img->last.image_mods[R] = img->image_mods[R];
      img->last.image_mods[G] = img->image_mods[G];
      img->last.image_mods[B] = img->image_mods[B];
      img->last.image_mods[A] = img->image_mods[A];
      /* need to apply image mods */
      if (!has_resized)
      {
         if (!img->orig_im)
         {
            img->orig_im = gib_imlib_clone_image(img->im);
         }
         else
         {
            gib_imlib_free_image_and_decache(img->im);
            img->im = gib_imlib_clone_image(img->orig_im);
         }
      }
      w = gib_imlib_image_get_width(img->im);
      h = gib_imlib_image_get_height(img->im);

      gib_imlib_image_set_has_alpha(img->im, 1);

      for (i = 0; i < 256; i++)
      {
         if (
             (ra =
              modf((double) (i) *
                   ((double) img->image_mods[A] / (double) 100), &ha)) > 0.5)
            ha++;
         atab[i] = (DATA8) (ha);

         if (
             (rr =
              modf((double) (i) *
                   ((double) img->image_mods[R] / (double) 100), &hr)) > 0.5)
            hr++;
         rtab[i] = (DATA8) (hr);

         if (
             (rg =
              modf((double) (i) *
                   ((double) img->image_mods[G] / (double) 100), &hg)) > 0.5)
            hg++;
         gtab[i] = (DATA8) (hg);

         if (
             (rb =
              modf((double) (i) *
                   ((double) img->image_mods[B] / (double) 100), &hb)) > 0.5)
            hb++;
         btab[i] = (DATA8) (hb);
      }
      gib_imlib_apply_color_modifier_to_rectangle(img->im, 0, 0, w, h, rtab,
                                                    gtab, btab, atab);
   }
   D_RETURN_(3);
}

void
geist_image_change_image_mods(geist_object * obj, int op[4])
{
   int i;
   geist_image *im = NULL;

   D_ENTER(3);

   im = GEIST_IMAGE(obj);
   for (i = 0; i < 4; i++)
   {
      im->image_mods[i] = op[i];
   }
   geist_image_apply_image_mods(obj);

   D_RETURN_(5);
}

void
geist_image_update_sizemode(geist_object * obj)
{
   double ratio = 0.0;
   int ww, hh, www, hhh;
   geist_image *img;

   D_ENTER(3);

   img = GEIST_IMAGE(obj);

   switch (obj->sizemode)
   {
     case SIZEMODE_NONE:
        if (img->orig_im)
        {
           ww = gib_imlib_image_get_width(img->orig_im);
           hh = gib_imlib_image_get_height(img->orig_im);
        }
        else
        {
           ww = gib_imlib_image_get_width(img->im);
           hh = gib_imlib_image_get_height(img->im);
        }
        obj->rendered_w = ww;
        obj->rendered_h = hh;
        break;
     case SIZEMODE_STRETCH:
        obj->rendered_w = obj->w;
        obj->rendered_h = obj->h;
        break;
     case SIZEMODE_ZOOM:
        www = obj->w;
        hhh = obj->h;
        if (img->orig_im)
        {
           ww = gib_imlib_image_get_width(img->orig_im);
           hh = gib_imlib_image_get_height(img->orig_im);
        }
        else
        {
           ww = gib_imlib_image_get_width(img->im);
           hh = gib_imlib_image_get_height(img->im);
        }
        ratio = ((double) ww / hh) / ((double) www / hhh);
        if (ratio > 1.0)
           hhh = obj->h / ratio;
        else if (ratio != 1.0)
           www = obj->w * ratio;
        obj->rendered_w = www;
        obj->rendered_h = hhh;
        break;
     default:
        printf("implement me!\n");
        break;
   }
   D_RETURN_(3);
}

void
geist_image_update_positioning(geist_object * obj)
{
   D_ENTER(3);

   geist_image_update_sizemode(obj);
   geist_object_update_alignment(obj);

   geist_image_apply_image_mods(obj);
   D_RETURN_(3);
}

char *
geist_image_get_object_list_entry(geist_object * obj, int column)
{
   D_ENTER(3);

   D_RETURN(3, GEIST_IMAGE(obj)->filename);
}

unsigned char
geist_image_has_transparency(geist_object * obj)
{
   geist_image *img;

   D_ENTER(3);

   img = GEIST_IMAGE(obj);

   if (img->image_mods[A] != FULL_OPACITY)
      D_RETURN(3, TRUE);

   D_RETURN(3, gib_imlib_image_has_alpha(img->im));
}

void
geist_image_rotate(geist_object *obj, double angle)
{   
    D_ENTER(3);

    printf("Implement me!\n");
       
    D_RETURN_(3);
}
