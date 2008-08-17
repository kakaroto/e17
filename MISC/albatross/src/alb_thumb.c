/* alb_thumb.c

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

#include "alb.h"
#include "gib_imlib.h"
#include "alb_thumb.h"

static gboolean img_load_cancel_cb(GtkWidget * widget,
                                   gpointer data);

typedef struct _cb_data cb_data;
struct _cb_data {
  alb_object *obj;
  GtkWidget *dialog;
};

alb_object *
alb_thumb_new(void)
{
  alb_thumb *img;

  D_ENTER(5);

  img = emalloc(sizeof(alb_thumb));
  alb_thumb_init(img);

  alb_object_set_state(ALB_OBJECT(img), VISIBLE);

  D_RETURN(5, (alb_object *) img);
}

void
alb_thumb_init(alb_thumb * img)
{
  alb_object *obj;

  D_ENTER(5);
  memset(img, 0, sizeof(alb_thumb));
  obj = ALB_OBJECT(img);
  alb_object_init(obj);
  obj->free = alb_thumb_free;
  obj->render = alb_thumb_render;
  obj->render_partial = alb_thumb_render_partial;
  obj->get_rendered_image = alb_thumb_get_rendered_image;
  obj->duplicate = alb_thumb_duplicate;
  obj->resize_event = alb_thumb_resize;
  obj->update_positioning = alb_thumb_update_positioning;
  alb_object_set_type(obj, ALB_TYPE_IMAGE);
  obj->sizemode = SIZEMODE_ZOOM;
  obj->alignment = ALIGN_CENTER;
  obj->display_props = alb_thumb_display_props;
  obj->get_object_list_entry = alb_thumb_get_object_list_entry;
  obj->has_transparency = alb_thumb_has_transparency;
  obj->rotate = alb_thumb_rotate;
  obj->resize_done = alb_thumb_resize_done;
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

alb_object *
alb_thumb_new_from_file(int x,
                        int y,
                        int w,
                        int h,
                        char *filename)
{
  alb_thumb *img;
  alb_object *obj;
  char *txt;

  D_ENTER(5);

  obj = alb_thumb_new();
  img = (alb_thumb *) obj;

  img->filename = estrdup(filename);
  img->cachefilename = alb_thumb_get_cache_filename(img);

  if (!(alb_thumb_load_file(w, h, img, filename))) {
    alb_thumb_free(obj);
    D_RETURN(5, NULL);
  }

  efree(obj->name);
  if ((txt = strrchr(img->filename, '/') + 1) != NULL)
    obj->name = estrdup(txt);
  else
    obj->name = estrdup(filename);

  obj->x = x;
  obj->y = y;
  obj->rendered_x = 0;
  obj->rendered_y = 0;
  alb_object_update_positioning(ALB_OBJECT(img));

  D_RETURN(5, (alb_object *) img);
}

/* reload thumb at the currently viewedsize */
void
alb_thumb_reload_file(alb_thumb * t)
{
  int w, h;
  int ret;
  Imlib_Image tmp;

  w = gib_imlib_image_get_width(t->im);
  h = gib_imlib_image_get_height(t->im);
  if (t->im)
    gib_imlib_free_image(t->im);
  if (t->orig_im) {
    gib_imlib_free_image(t->orig_im);
    t->orig_im = NULL;
  }
  ret = alb_imlib_load_image(&tmp, t->filename);
  if (ret) {
    t->im =
      gib_imlib_create_cropped_scaled_image(tmp, 0, 0,
                                            gib_imlib_image_get_width(tmp),
                                            gib_imlib_image_get_height(tmp),
                                            w, h, 1);
  }
  gib_imlib_free_image(tmp);
}

void
alb_thumb_resize_done(alb_object * obj)
{
  alb_object_int_resize_done(obj);
  alb_thumb(obj)->cache_invalidate = 1;
  alb_thumb_reload_file(alb_thumb(obj));
}

char *
alb_thumb_get_cache_filename(alb_thumb * t)
{
  char *temp, *p;
  char *cachefilename;

  if (!t)
    return NULL;

  temp = estrdup(t->filename);
  p = strrchr(temp, '/');
  if (p && *(p + 1)) {
    *p = '\0';
    cachefilename = g_strjoin("/", temp, ".albatross", "cache", p + 1, NULL);
  } else {
    cachefilename = g_strjoin("/", ".albatross", "cache", temp, NULL);
  }
  efree(temp);
  return cachefilename;
}

void
alb_thumb_save_to_cache(alb_thumb * t)
{
  if (!t)
    return;
  if (!t->im)
    return;
  if (t->cachefilename) {
    gib_imlib_save_image(t->im, t->cachefilename);
  }
}

void
alb_thumb_free(alb_object * obj)
{
  alb_thumb *img;

  D_ENTER(5);

  img = (alb_thumb *) obj;

  if (!img)
    D_RETURN_(5);

  if (img->filename)
    efree(img->filename);
  if (img->cachefilename)
    efree(img->cachefilename);
  if (img->im)
    gib_imlib_free_image(img->im);

  efree(img);

  D_RETURN_(5);
}

void
alb_thumb_render(alb_object * obj,
                 Imlib_Image dest)
{
  alb_thumb *im;

  D_ENTER(5);

  if (!alb_object_get_state(obj, VISIBLE))
    D_RETURN_(5);

  im = (alb_thumb *) obj;
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
  alb_thumb_render_partial(obj, dest, obj->x, obj->y, obj->w, obj->h);
  D_RETURN_(5);
}

void
alb_thumb_render_partial(alb_object * obj,
                         Imlib_Image dest,
                         int x,
                         int y,
                         int w,
                         int h)
{
  alb_thumb *im;
  int sw, sh, dw, dh, sx, sy, dx, dy;

  D_ENTER(5);

  if (!alb_object_get_state(obj, VISIBLE))
    D_RETURN_(5);

  im = alb_thumb(obj);
  if (!im->im)
    D_RETURN_(5);

  alb_object_get_rendered_area(obj, &dx, &dy, &dw, &dh);
  CLIP(dx, dy, dw, dh, x, y, w, h);
  sx = dx - obj->x - obj->rendered_x;
  sy = dy - obj->y - obj->rendered_y;
  sw = dw;
  sh = dh;

#if 0
  alb_object_get_clipped_render_areas(obj, x, y, w, h, &sx, &sy, &sw, &sh,
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
alb_thumb_load_file(int w,
                    int h,
                    alb_thumb * img,
                    char *filename)
{
  alb_object *obj;
  int ret;
  Imlib_Image temp;
  struct stat st;

  D_ENTER(5);

  obj = (alb_object *) img;

  if (img->im)
    gib_imlib_free_image(img->im);

  if (!img->cache_invalidate && img->cachefilename
      && (stat(img->cachefilename, &st) != -1)) {
    ret = alb_imlib_load_image(&img->im, img->cachefilename);
  } else {
    ret = alb_imlib_load_image(&temp, filename);

    if (ret) {
      int ww, hh, www = w, hhh = h;
      double ratio;

      ww = gib_imlib_image_get_width(temp);
      hh = gib_imlib_image_get_height(temp);

      ratio = ((double) ww / hh) / ((double) www / hhh);
      if (ratio > 1.0)
        hhh = w / ratio;
      else if (ratio != 1.0)
        www = h * ratio;

      img->im =
        gib_imlib_create_cropped_scaled_image(temp, 0, 0, ww, hh, www, hhh,
                                              1);
      alb_thumb_save_to_cache(img);
      img->cache_invalidate = 0;

      gib_imlib_free_image(temp);
    }
  }
  if (ret) {
    if (img->orig_im) {
      gib_imlib_free_image(img->orig_im);
      img->orig_im = NULL;
    }
    obj->w = w;
    obj->h = h;
    obj->rendered_w = gib_imlib_image_get_width(img->im);
    obj->rendered_h = gib_imlib_image_get_height(img->im);
  }
  D_RETURN(5, ret);
}

Imlib_Image
alb_thumb_get_rendered_image(alb_object * obj)
{
  D_ENTER(3);

  D_RETURN(3, alb_thumb(obj)->im);
}

alb_object *
alb_thumb_duplicate(alb_object * obj)
{
  int i;
  alb_object *ret;
  alb_thumb *img;

  D_ENTER(3);

  img = alb_thumb(obj);

  ret =
    alb_thumb_new_from_file(obj->x, obj->y, obj->w, obj->h, img->filename);
  if (ret) {
    ret->rendered_x = obj->rendered_x;
    ret->rendered_y = obj->rendered_y;
    ret->h = obj->h;
    ret->w = obj->w;
    for (i = 0; i < 4; i++) {
      alb_thumb(ret)->image_mods[i] = img->image_mods[i];
    }
    ret->state = obj->state;
    ret->alias = obj->alias;
    ret->name =
      g_strjoin(" ", "Copy of", obj->name ? obj->name : "Untitled object",
                NULL);
    alb_object_update_positioning(ALB_OBJECT(ret));
  }

  D_RETURN(3, ret);
}

void
alb_thumb_resize(alb_object * obj,
                 int x,
                 int y)
{
  alb_thumb *img;

  D_ENTER(5);

  img = alb_thumb(obj);

  x += obj->clicked_x;
  y += obj->clicked_y;

  D(5, ("resize to %d,%d\n", x, y));
  alb_object_resize_object(obj, x, y);
  alb_object_update_positioning(obj);

  D_RETURN_(5);
}

void
alb_thumb_refresh_image_mods_cb(GtkWidget * widget,
                                gpointer * type)
{
  int p[4], i;
  gib_list *list, *l;
  alb_object *obj = NULL;

  D_ENTER(3);

  /*this works for multiple objects although we wont have that situation for
     now, but that might change somewhere along the line so there :) */

  list = alb_document_get_selected_list(current_doc);
  if (list) {
    for (l = list; l; l = l->next) {
      obj = l->data;
      for (i = 0; i < 4; i++) {
        p[i] = alb_thumb(obj)->image_mods[i];
      }
      p[GPOINTER_TO_INT(type)] =
        gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
      alb_thumb_change_image_mods(ALB_OBJECT(obj), p);
      alb_object_dirty(ALB_OBJECT(obj));
    }
  }
  gib_list_free(list);
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
  D_RETURN_(3);
}


static gboolean
img_load_cb(GtkWidget * widget,
            gpointer data)
{
  alb_object *obj = ((cb_data *) data)->obj;
  char *path;

  D_ENTER(3);

  path =
    gtk_file_selection_get_filename(GTK_FILE_SELECTION
                                    (((cb_data *) data)->dialog));

  if (path) {
    alb_object_dirty(obj);
    alb_thumb_load_file(200, 200, alb_thumb(obj), path);
    alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
    alb_object_dirty(obj);
  }
  gtk_widget_destroy((GtkWidget *) ((cb_data *) data)->dialog);
  efree(data);
  D_RETURN(3, TRUE);
}

static gboolean
img_load_cancel_cb(GtkWidget * widget,
                   gpointer data)
{
  gtk_widget_destroy((GtkWidget *) data);
  return TRUE;
}



gboolean
alb_thumb_select_file_cb(GtkWidget * widget,
                         gpointer * data)
{
  cb_data *sel_cb_data = NULL;
  alb_object *obj = ALB_OBJECT(data);
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

gboolean
alb_thumb_refresh_aa_cb(GtkWidget * widget,
                        gpointer * data)
{
  alb_object *obj = NULL;
  gib_list *l = NULL;
  gib_list *list = NULL;

  D_ENTER(3);

  list = alb_document_get_selected_list(current_doc);
  if (list) {
    for (l = list; l; l = l->next) {
      obj = l->data;
      obj->alias = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
      alb_object_dirty(obj);
      alb_object_update_positioning(obj);
      alb_object_dirty(obj);
    }
    gib_list_free(list);
    alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
  }
  D_RETURN(3, TRUE);
}


GtkWidget *
alb_thumb_display_props(alb_object * obj)
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
                     GTK_SIGNAL_FUNC(alb_thumb_refresh_aa_cb), NULL);
  gtk_container_set_border_width(GTK_CONTAINER(antialias_checkb), 10);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(antialias_checkb),
                               obj->alias);
  gtk_widget_show(antialias_checkb);

  gtk_entry_set_text(GTK_ENTRY(file_entry), alb_thumb(obj)->filename);
  gtk_signal_connect(GTK_OBJECT(sel_file_btn), "clicked",
                     GTK_SIGNAL_FUNC(alb_thumb_select_file_cb),
                     (gpointer) obj);


  imo_l = gtk_label_new("Opacity %:");
  gtk_misc_set_alignment(GTK_MISC(imo_l), 1.0, 0.5);
  gtk_table_attach(GTK_TABLE(table), imo_l, 1, 2, 1, 2, GTK_FILL | GTK_EXPAND,
                   0, 2, 2);
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
                            alb_thumb(obj)->image_mods[A]);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(cr),
                            alb_thumb(obj)->image_mods[R]);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(cg),
                            alb_thumb(obj)->image_mods[G]);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(cb),
                            alb_thumb(obj)->image_mods[B]);

  gtk_signal_connect(GTK_OBJECT(imo), "changed",
                     GTK_SIGNAL_FUNC(alb_thumb_refresh_image_mods_cb),
                     (gpointer) A);
  gtk_signal_connect(GTK_OBJECT(cr), "changed",
                     GTK_SIGNAL_FUNC(alb_thumb_refresh_image_mods_cb),
                     (gpointer) R);
  gtk_signal_connect(GTK_OBJECT(cg), "changed",
                     GTK_SIGNAL_FUNC(alb_thumb_refresh_image_mods_cb),
                     (gpointer) G);
  gtk_signal_connect(GTK_OBJECT(cb), "changed",
                     GTK_SIGNAL_FUNC(alb_thumb_refresh_image_mods_cb),
                     (gpointer) B);

  gtk_widget_show(imo);
  return (image_props);

}

void
alb_thumb_apply_image_mods(alb_object * obj)
{
  alb_thumb *img;
  int has_resized = 0;
  int w, h, i;
  double ra, ha, rr, hr, rg, hg, rb, hb;
  DATA8 atab[256], rtab[256], gtab[256], btab[256];

  D_ENTER(3);

  img = alb_thumb(obj);

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
                                             ((img->image_mods[A] !=
                                               FULL_OPACITY)
                                              || (img->image_mods[R] != 100)
                                              || (img->image_mods[G] != 100)
                                              || (img->image_mods[B] !=
                                                  100)))) {
    obj->last.alias = obj->alias;
    /* need to resize */
    if (!img->orig_im) {
      img->orig_im = gib_imlib_clone_image(img->im);
    } else {
      w = gib_imlib_image_get_width(img->orig_im);
      h = gib_imlib_image_get_height(img->orig_im);
    }
    has_resized = 1;
    gib_imlib_free_image_and_decache(img->im);
    img->im =
      gib_imlib_create_cropped_scaled_image(img->orig_im, 0, 0, w, h,
                                            obj->rendered_w, obj->rendered_h,
                                            obj->alias);
  }

  if (((img->image_mods[A] != FULL_OPACITY)
       && (img->image_mods[A] != img->last.image_mods[A]))
      || ((img->image_mods[R] != FULL_COLOR)
          && (img->image_mods[R] != img->last.image_mods[R]))
      || ((img->image_mods[G] != FULL_COLOR)
          && (img->image_mods[G] != img->last.image_mods[G]))
      || ((img->image_mods[B] != FULL_COLOR)
          && (img->image_mods[B] != img->last.image_mods[B]))
      || (has_resized)) {
    D(5, ("need to do opacity, it's %d\n", img->image_mods[A]));
    img->last.image_mods[R] = img->image_mods[R];
    img->last.image_mods[G] = img->image_mods[G];
    img->last.image_mods[B] = img->image_mods[B];
    img->last.image_mods[A] = img->image_mods[A];
    /* need to apply image mods */
    if (!has_resized) {
      if (!img->orig_im) {
        img->orig_im = gib_imlib_clone_image(img->im);
      } else {
        gib_imlib_free_image_and_decache(img->im);
        img->im = gib_imlib_clone_image(img->orig_im);
      }
    }
    w = gib_imlib_image_get_width(img->im);
    h = gib_imlib_image_get_height(img->im);

    gib_imlib_image_set_has_alpha(img->im, 1);

    for (i = 0; i < 256; i++) {
      if ((ra =
           modf((double) (i) * ((double) img->image_mods[A] / (double) 100),
                &ha)) > 0.5)
        ha++;
      atab[i] = (DATA8) (ha);

      if ((rr =
           modf((double) (i) * ((double) img->image_mods[R] / (double) 100),
                &hr)) > 0.5)
        hr++;
      rtab[i] = (DATA8) (hr);

      if ((rg =
           modf((double) (i) * ((double) img->image_mods[G] / (double) 100),
                &hg)) > 0.5)
        hg++;
      gtab[i] = (DATA8) (hg);

      if ((rb =
           modf((double) (i) * ((double) img->image_mods[B] / (double) 100),
                &hb)) > 0.5)
        hb++;
      btab[i] = (DATA8) (hb);
    }
    gib_imlib_apply_color_modifier_to_rectangle(img->im, 0, 0, w, h, rtab,
                                                gtab, btab, atab);
  }
  D_RETURN_(3);
}

void
alb_thumb_change_image_mods(alb_object * obj,
                            int op[4])
{
  int i;
  alb_thumb *im = NULL;

  D_ENTER(3);

  im = alb_thumb(obj);
  for (i = 0; i < 4; i++) {
    im->image_mods[i] = op[i];
  }
  alb_thumb_apply_image_mods(obj);

  D_RETURN_(5);
}

void
alb_thumb_update_sizemode(alb_object * obj)
{
  double ratio = 0.0;
  int ww, hh, www, hhh;
  alb_thumb *img;

  D_ENTER(3);

  img = alb_thumb(obj);

  switch (obj->sizemode) {
    case SIZEMODE_NONE:
      if (img->orig_im) {
        ww = gib_imlib_image_get_width(img->orig_im);
        hh = gib_imlib_image_get_height(img->orig_im);
      } else {
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
      if (img->orig_im) {
        ww = gib_imlib_image_get_width(img->orig_im);
        hh = gib_imlib_image_get_height(img->orig_im);
      } else {
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
alb_thumb_update_positioning(alb_object * obj)
{
  D_ENTER(3);

  alb_thumb_update_sizemode(obj);
  alb_object_update_alignment(obj);

  alb_thumb_apply_image_mods(obj);
  D_RETURN_(3);
}

char *
alb_thumb_get_object_list_entry(alb_object * obj,
                                int column)
{
  D_ENTER(3);

  D_RETURN(3, alb_thumb(obj)->filename);
}

unsigned char
alb_thumb_has_transparency(alb_object * obj)
{
  alb_thumb *img;

  D_ENTER(3);

  img = alb_thumb(obj);

  if (img->image_mods[A] != FULL_OPACITY)
    D_RETURN(3, TRUE);

  D_RETURN(3, gib_imlib_image_has_alpha(img->im));
}

void
alb_thumb_rotate(alb_object * obj,
                 double angle)
{
  D_ENTER(3);

  printf("Implement me!\n");

  D_RETURN_(3);
}

gib_list *
alb_thumb_tesselate_constrain_w(int w,
                                int *h,
                                int item_w,
                                int item_h,
                                int h_space,
                                int v_space,
                                int count)
{
  gib_list *l, *ll, *ret = NULL;
  alb_object *obj;
  int x = 5, y = 5, i;
  double rem;
  int rows, cols;
  struct point *point;

  D_ENTER(3);

  if (w < item_w + h_space)
    return NULL;

  cols = (w - (2 * h_space)) / (item_h + v_space);
  rem = (w - (2 * h_space)) % (item_h + v_space);
  rows = count / cols;
  if (rem > 0.0)
    rows++;
  *h = (v_space) + (rows * (item_h + v_space));

  for (i = 0; i < count; i++) {
    if ((x + 105) > w) {
      x = 5;
      y += 105;
    }
    point = emalloc(sizeof(struct point));
    point->x = x;
    point->y = y;
    ret = gib_list_add_end(ret, point);
    x += 105;
  }
  return ret;
}
