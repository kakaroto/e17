/* alb_poly.c

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

#include "alb_poly.h"

static void refresh_r_cb(GtkWidget * widget,
                         gpointer * obj);
static void refresh_g_cb(GtkWidget * widget,
                         gpointer * obj);
static void refresh_b_cb(GtkWidget * widget,
                         gpointer * obj);
static void refresh_a_cb(GtkWidget * widget,
                         gpointer * obj);

alb_object *
alb_poly_new(void)
{
  alb_poly *poly;

  D_ENTER(5);

  poly = emalloc(sizeof(alb_poly));
  alb_poly_init(poly);

  alb_object_set_state(ALB_OBJECT(poly), VISIBLE);

  D_RETURN(5, ALB_OBJECT(poly));
}

void
alb_poly_init(alb_poly * poly)
{
  alb_object *obj;

  D_ENTER(5);
  memset(poly, 0, sizeof(alb_poly));
  obj = ALB_OBJECT(poly);
  alb_object_init(obj);
  obj->free = alb_poly_free;
  obj->render = alb_poly_render;
  obj->render_partial = alb_poly_render_partial;
  obj->duplicate = alb_poly_duplicate;
  obj->part_is_transparent = alb_poly_part_is_transparent;
  obj->display_props = alb_poly_display_props;
  obj->resize_event = alb_poly_resize;
  obj->rotate = alb_poly_rotate;
  obj->move = alb_poly_move;
  obj->sizemode = SIZEMODE_STRETCH;
  obj->alignment = ALIGN_NONE;
  obj->get_rendered_area = alb_poly_get_rendered_area;
  alb_object_set_type(obj, ALB_TYPE_POLY);
  obj->name = estrdup("New polygon");

  D_RETURN_(5);
}

alb_object *
alb_poly_new_from_points(gib_list * points,
                         int a,
                         int r,
                         int g,
                         int b)
{
  alb_poly *poly;
  alb_object *obj;

  D_ENTER(5);

  obj = alb_poly_new();
  poly = ALB_POLY(obj);

  poly->a = a;
  poly->b = b;
  poly->g = g;
  poly->r = r;

  poly->points = points;
  poly->need_update = TRUE;

  alb_poly_update_bounds(poly);

  D_RETURN(5, ALB_OBJECT(poly));
}

void
alb_poly_update_bounds(alb_poly * poly)
{
  alb_object *obj;
  int px1, py1, px2, py2;

  D_ENTER(3);
  obj = ALB_OBJECT(poly);

  alb_poly_update_imlib_polygon(poly);
  imlib_polygon_get_bounds(poly->poly, &px1, &py1, &px2, &py2);
  obj->x = px1;
  obj->y = py1;
  obj->w = obj->rendered_w = px2 - px1 + 2;
  obj->h = obj->rendered_h = py2 - py1 + 1;
  obj->rendered_x = 0;
  obj->rendered_y = 0;
  D(4, ("new poly bounds: %d,%d %dx%d\n", obj->x, obj->y, obj->w, obj->h));

  D_RETURN_(3);
}

void
alb_poly_get_rendered_area(alb_object * obj,
                           int *x,
                           int *y,
                           int *w,
                           int *h)
{
  D_ENTER(3);

  *x = obj->x;
  *y = obj->y;
  *w = obj->w;
  *h = obj->h;

  D(5, ("area %d,%d %dx%d\n", *x, *y, *w, *h));

  D_RETURN_(3);
}

alb_point *
alb_point_new(int x,
              int y)
{
  alb_point *point;

  D_ENTER(3);
  point = emalloc(sizeof(alb_point));
  point->x = x;
  point->y = y;

  D_RETURN(3, point);
}

void
alb_poly_add_point(alb_poly * poly,
                   int x,
                   int y)
{
  D_ENTER(3);

  if (!poly)
    D_RETURN_(3);

  if (!poly->poly)
    poly->poly = imlib_polygon_new();
  imlib_polygon_add_point(poly->poly, x, y);
  poly->points = gib_list_add_end(poly->points, alb_point_new(x, y));
  alb_poly_update_bounds(poly);

  D_RETURN_(3);
}

void
alb_poly_update_imlib_polygon(alb_poly * poly)
{
  gib_list *l;
  alb_point *p;
  alb_object *obj;

  D_ENTER(3);

  if (!poly || !poly->points || !poly->need_update)
    D_RETURN_(3);
  poly->need_update = FALSE;

  obj = ALB_OBJECT(poly);

  if (poly->poly)
    imlib_polygon_free(poly->poly);

  poly->poly = imlib_polygon_new();
  l = poly->points;
  while (l) {
    p = (alb_point *) l->data;
    imlib_polygon_add_point(poly->poly, p->x, p->y);
    l = l->next;
  }

  D_RETURN_(3);
}

void
alb_poly_free(alb_object * obj)
{
  alb_poly *poly;

  D_ENTER(5);

  poly = (alb_poly *) obj;

  if (!poly)
    D_RETURN_(5);

  imlib_polygon_free(poly->poly);
  gib_list_free_and_data(poly->points);
  efree(poly);

  D_RETURN_(5);
}

void
alb_poly_render(alb_object * obj,
                Imlib_Image dest)
{
  alb_poly *poly;

  D_ENTER(5);

  if (!alb_object_get_state(obj, VISIBLE))
    D_RETURN_(5);

  poly = ALB_POLY(obj);

  alb_poly_update_imlib_polygon(poly);

  if (poly->filled)
    gib_imlib_image_fill_polygon(dest, poly->poly, poly->r, poly->g, poly->b,
                                 poly->a, obj->alias, 0, 0, 0, 0);
  else
    gib_imlib_image_draw_polygon(dest, poly->poly, poly->r, poly->g, poly->b,
                                 poly->a, poly->closed, obj->alias, 0, 0, 0,
                                 0);

  D_RETURN_(5);
}

void
alb_poly_render_partial(alb_object * obj,
                        Imlib_Image dest,
                        int x,
                        int y,
                        int w,
                        int h)
{
  alb_poly *poly;
  int sw, sh, dw, dh, sx, sy, dx, dy;

  D_ENTER(5);

  if (!alb_object_get_state(obj, VISIBLE))
    D_RETURN_(5);

  poly = ALB_POLY(obj);

  alb_poly_update_imlib_polygon(poly);

  alb_object_get_clipped_render_areas(obj, x, y, w, h, &sx, &sy, &sw, &sh,
                                      &dx, &dy, &dw, &dh);

  D(5,
    ("partial rendering %d,%d %dx%d with %d,%d,%d,%d\n", dx, dy, dw, dh,
     poly->r, poly->g, poly->b, poly->a));

  if (poly->filled)
    gib_imlib_image_fill_polygon(dest, poly->poly, poly->r, poly->g, poly->b,
                                 poly->a, obj->alias, dx, dy, dw, dh);
  else
    gib_imlib_image_draw_polygon(dest, poly->poly, poly->r, poly->g, poly->b,
                                 poly->a, poly->closed, obj->alias, dx, dy,
                                 dw, dh);


  D_RETURN_(5);
}

alb_object *
alb_poly_duplicate(alb_object * obj)
{
  alb_object *ret;
  alb_poly *poly;

  D_ENTER(3);

  poly = ALB_POLY(obj);

  ret =
    alb_poly_new_from_points(gib_list_dup(poly->points), poly->a, poly->r,
                             poly->g, poly->b);
  ret->rendered_x = obj->rendered_x;
  ret->rendered_y = obj->rendered_y;
  ret->w = obj->w;
  ret->h = obj->h;
  if (ret) {
    ret->alias = obj->alias;
    ret->state = obj->state;
    ALB_POLY(ret)->closed = poly->closed;
    ALB_POLY(ret)->filled = poly->filled;
    ret->name =
      g_strjoin(" ", "Copy of", obj->name ? obj->name : "Untitled object",
                NULL);
  }

  D_RETURN(3, ret);
}

unsigned char
alb_poly_part_is_transparent(alb_object * obj,
                             int x,
                             int y)
{
  D_ENTER(3);
/*
   if (imlib_polygon_contains_point(ALB_POLY(obj)->poly, x, y))
   {
      D_RETURN(3, ALB_POLY(obj)->a == 0 ? 1 : 0);
   }
   else
   {
      D_RETURN(3, TRUE);
   }
 */
  D_RETURN(3, FALSE);
}

void
alb_poly_stretch(alb_object * obj,
                 int x,
                 int y)
{
  double dx, dy;
  alb_poly *poly;
  gib_list *l;
  alb_point *p;

  D_ENTER(5);

  D(5, ("resize to %d,%d\n", x, y));

  poly = (alb_poly *) obj;

  switch (obj->resize) {
    case RESIZE_RIGHT:
      /* calculate resize ratio */
      dx = (double) (x - obj->x) / (double) obj->w;

      for (l = poly->points; l; l = l->next) {
        p = (alb_point *) l->data;
        p->x = obj->x + (p->x - obj->x) * dx;
      }
      break;

    case RESIZE_LEFT:
      dx = (double) (obj->x - x + obj->w) / (double) obj->w;

      for (l = poly->points; l; l = l->next) {
        p = (alb_point *) l->data;
        p->x = obj->x + obj->w - (obj->x + obj->w - p->x) * dx;
      }
      break;

    case RESIZE_BOTTOM:
      dy = (double) (y - obj->y) / (double) obj->h;

      for (l = poly->points; l; l = l->next) {
        p = (alb_point *) l->data;
        p->y = obj->y + (p->y - obj->y) * dy;
      }
      break;

    case RESIZE_BOTTOMRIGHT:
      dx = (double) (x - obj->x) / (double) obj->w;
      dy = (double) (y - obj->y) / (double) obj->h;

      for (l = poly->points; l; l = l->next) {
        p = (alb_point *) l->data;
        p->x = obj->x + (p->x - obj->x) * dx;
        p->y = obj->y + (p->y - obj->y) * dy;
      }
      break;

    case RESIZE_BOTTOMLEFT:
      dx = (double) (obj->x - x + obj->w) / (double) obj->w;
      dy = (double) (y - obj->y) / (double) obj->h;

      for (l = poly->points; l; l = l->next) {
        p = (alb_point *) l->data;
        p->x = obj->x + obj->w - (obj->x + obj->w - p->x) * dx;
        p->y = obj->y + (p->y - obj->y) * dy;
      }
      break;

    case RESIZE_TOP:
      dy = (double) (obj->y - y + obj->h) / (double) obj->h;

      for (l = poly->points; l; l = l->next) {
        p = (alb_point *) l->data;
        p->y = obj->y + obj->h - (obj->y + obj->h - p->y) * dy;
      }
      break;

    case RESIZE_TOPRIGHT:
      dx = (double) (x - obj->x) / (double) obj->w;
      dy = (double) (obj->y - y + obj->h) / (double) obj->h;

      for (l = poly->points; l; l = l->next) {
        p = (alb_point *) l->data;
        p->x = obj->x + (p->x - obj->x) * dx;
        p->y = obj->y + obj->h - (obj->y + obj->h - p->y) * dy;
      }
      break;

    case RESIZE_TOPLEFT:
      dx = (double) (obj->x - x + obj->w) / (double) obj->w;
      dy = (double) (obj->y - y + obj->h) / (double) obj->h;

      for (l = poly->points; l; l = l->next) {
        p = (alb_point *) l->data;
        p->x = obj->x + obj->w - (obj->x + obj->w - p->x) * dx;
        p->y = obj->y + obj->w - (obj->y + obj->w - p->y) * dy;
      }
      break;

    default:
      break;
  }
  poly->need_update = TRUE;
  alb_poly_update_imlib_polygon(poly);
  alb_poly_update_bounds(poly);

  D_RETURN_(5);

}


void
alb_poly_zoom(alb_object * obj,
              int x,
              int y)
{
  double dx, center_x, center_y;
  alb_poly *poly;
  gib_list *l;
  alb_point *p;

  D_ENTER(5);

  D(5, ("resize to %d,%d\n", x, y));

  poly = (alb_poly *) obj;

  /*calculate center point in proportion to which we will stretch. Notice
   * the exceedingly hard math. Dont hurt your eyes. Till */
  center_x = (double) (obj->x + (double) obj->w / 2);
  center_y = (double) (obj->y + (double) obj->h / 2);

  /* calculate resize ratio relative to obj center point */
  dx = labs((double) x - center_x) / ((double) obj->w / 2);

  if (dx) {             /* we dont want our poly to vanish :) */
    for (l = poly->points; l; l = l->next) {
      p = (alb_point *) l->data;

      p->x = (double) obj->x + (double) obj->w / 2 + ((p->x - center_x) * dx);
      p->y = (double) obj->y + (double) obj->h / 2 + ((p->y - center_y) * dx);

    }
  }
  poly->need_update = TRUE;
  alb_poly_update_imlib_polygon(poly);
  alb_poly_update_bounds(poly);

  D_RETURN_(5);

}

void
alb_poly_resize(alb_object * obj,
                int x,
                int y)
{
  D_ENTER(3);


  switch (obj->sizemode) {
    case SIZEMODE_ZOOM:
      alb_poly_zoom(obj, x, y);
      break;
    case SIZEMODE_STRETCH:
      alb_poly_stretch(obj, x, y);
      break;

    default:
      break;
  }

  D_RETURN_(3);
}

void
alb_poly_move_points_relative(alb_poly * poly,
                              int x,
                              int y)
{
  gib_list *l;
  alb_point *p;

  D_ENTER(3);

  if (!poly || (!x && !y))
    D_RETURN_(3);

  l = poly->points;
  if (!l)
    D_RETURN_(3);

  while (l) {
    p = (alb_point *) l->data;
    if (p) {
      p->x += x;
      p->y += y;
    }
    l = l->next;
  }
  poly->need_update = TRUE;

  D_RETURN_(3);
}

void
alb_poly_move(alb_object * obj,
              int x,
              int y)
{
  int oldx, oldy, dx, dy;

  D_ENTER(3);

  ALB_POLY(obj)->need_update = TRUE;
  oldx = obj->x;
  oldy = obj->y;
  alb_object_int_move(obj, x, y);
  dx = obj->x - oldx;
  dy = obj->y - oldy;
  alb_poly_move_points_relative(ALB_POLY(obj), dx, dy);

  D_RETURN_(3);
}

static void
refresh_r_cb(GtkWidget * widget,
             gpointer * obj)
{

  ALB_POLY(obj)->r =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  alb_object_dirty(ALB_OBJECT(obj));
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}

static void
refresh_g_cb(GtkWidget * widget,
             gpointer * obj)
{

  ALB_POLY(obj)->g =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  alb_object_dirty(ALB_OBJECT(obj));
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}

static void
refresh_b_cb(GtkWidget * widget,
             gpointer * obj)
{

  ALB_POLY(obj)->b =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  alb_object_dirty(ALB_OBJECT(obj));
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}

static void
refresh_a_cb(GtkWidget * widget,
             gpointer * obj)
{

  ALB_POLY(obj)->a =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  alb_object_dirty(ALB_OBJECT(obj));
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}


GtkWidget *
alb_poly_display_props(alb_object * obj)
{

  GtkWidget *win, *table, *hbox, *cr_l, *cr, *cg_l, *cg, *cb_l, *cb, *ca_l,
    *ca;
  GtkAdjustment *a1, *a2, *a3, *a4;

  a1 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
  a2 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
  a3 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
  a4 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);

  win = gtk_hbox_new(FALSE, 0);

  table = gtk_table_new(3, 2, FALSE);
  gtk_container_set_border_width(GTK_CONTAINER(win), 5);
  gtk_container_add(GTK_CONTAINER(win), table);

  hbox = gtk_hbox_new(FALSE, 0);

  cr_l = gtk_label_new("R:");
  gtk_misc_set_alignment(GTK_MISC(cr_l), 1.0, 0.5);
  gtk_box_pack_start(GTK_BOX(hbox), cr_l, TRUE, FALSE, 2);
  gtk_widget_show(cr_l);

  cr = gtk_spin_button_new(GTK_ADJUSTMENT(a1), 1, 0);
  gtk_box_pack_start(GTK_BOX(hbox), cr, TRUE, FALSE, 2);
  gtk_widget_show(cr);


  cg_l = gtk_label_new("G:");
  gtk_misc_set_alignment(GTK_MISC(cg_l), 1.0, 0.5);
  gtk_box_pack_start(GTK_BOX(hbox), cg_l, TRUE, FALSE, 2);
  gtk_widget_show(cg_l);

  cg = gtk_spin_button_new(GTK_ADJUSTMENT(a2), 1, 0);
  gtk_box_pack_start(GTK_BOX(hbox), cg, TRUE, FALSE, 2);
  gtk_widget_show(cg);


  cb_l = gtk_label_new("B:");
  gtk_misc_set_alignment(GTK_MISC(cb_l), 1.0, 0.5);
  gtk_box_pack_start(GTK_BOX(hbox), cb_l, TRUE, FALSE, 2);
  gtk_widget_show(cb_l);

  cb = gtk_spin_button_new(GTK_ADJUSTMENT(a3), 1, 0);
  gtk_box_pack_start(GTK_BOX(hbox), cb, TRUE, FALSE, 2);
  gtk_widget_show(cb);


  ca_l = gtk_label_new("A:");
  gtk_misc_set_alignment(GTK_MISC(ca_l), 1.0, 0.5);
  gtk_box_pack_start(GTK_BOX(hbox), ca_l, TRUE, FALSE, 2);
  gtk_widget_show(ca_l);

  ca = gtk_spin_button_new(GTK_ADJUSTMENT(a4), 1, 0);
  gtk_box_pack_start(GTK_BOX(hbox), ca, TRUE, FALSE, 2);
  gtk_widget_show(ca);


  gtk_table_attach(GTK_TABLE(table), hbox, 0, 2, 1, 2, GTK_FILL | GTK_EXPAND,
                   0, 2, 2);
  gtk_widget_show(hbox);

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(cr), ALB_POLY(obj)->r);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(cg), ALB_POLY(obj)->g);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(cb), ALB_POLY(obj)->b);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ca), ALB_POLY(obj)->a);


  gtk_signal_connect(GTK_OBJECT(cr), "changed", GTK_SIGNAL_FUNC(refresh_r_cb),
                     (gpointer) obj);
  gtk_signal_connect(GTK_OBJECT(cg), "changed", GTK_SIGNAL_FUNC(refresh_g_cb),
                     (gpointer) obj);
  gtk_signal_connect(GTK_OBJECT(ca), "changed", GTK_SIGNAL_FUNC(refresh_a_cb),
                     (gpointer) obj);
  gtk_signal_connect(GTK_OBJECT(cb), "changed", GTK_SIGNAL_FUNC(refresh_b_cb),
                     (gpointer) obj);

  gtk_widget_show(table);
  return (win);

}

unsigned char
alb_poly_has_transparency(alb_object * obj)
{
  alb_poly *poly;

  D_ENTER(3);

  poly = ALB_POLY(obj);

  if (poly->a < 255) {
    D_RETURN(3, TRUE);
  } else {
    D_RETURN(3, FALSE);
  }
}


void
alb_poly_rotate(alb_object * obj,
                double angle)
{
  alb_poly *poly;
  gib_list *l;
  alb_point *p;

  /*the cartesian coordinates relative to the center point */
  double cart_x, cart_y;

  D_ENTER(3);

  poly = ALB_POLY(obj);

  angle = angle * 2 * 3.141592654 / 360;

  if (!poly || !poly->points)
    D_RETURN_(3);

  l = poly->points;

  while (l) {
    p = (alb_point *) l->data;

    cart_x = p->x - obj->w / 2 - obj->x;
    cart_y = p->y - obj->h / 2 - obj->y;

    p->x = obj->x + obj->w / 2 + cart_x * cos(angle) - cart_y * sin(angle);
    p->y = obj->y + obj->h / 2 + cart_x * sin(angle) + cart_y * cos(angle);

    l = l->next;
  }
  poly->need_update = TRUE;
  alb_poly_update_bounds(poly);

  D_RETURN_(3);
}
