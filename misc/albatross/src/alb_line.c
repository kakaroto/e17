/* alb_line.c

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

#include "alb_line.h"

#define SWAP(a,b) {int _tmp; _tmp = a; a = b; b = _tmp;}

static void refresh_r_cb(GtkWidget * widget,
                         gpointer * obj);
static void refresh_g_cb(GtkWidget * widget,
                         gpointer * obj);
static void refresh_b_cb(GtkWidget * widget,
                         gpointer * obj);
static void refresh_a_cb(GtkWidget * widget,
                         gpointer * obj);

alb_object *
alb_line_new(void)
{
  alb_line *line;

  D_ENTER(5);

  line = emalloc(sizeof(alb_line));
  alb_line_init(line);

  alb_object_set_state(ALB_OBJECT(line), VISIBLE);

  D_RETURN(5, ALB_OBJECT(line));
}

void
alb_line_init(alb_line * line)
{
  alb_object *obj;

  D_ENTER(5);
  memset(line, 0, sizeof(alb_line));
  obj = ALB_OBJECT(line);
  alb_object_init(obj);
  obj->free = alb_line_free;
  obj->render = alb_line_render;
  obj->render_partial = alb_line_render_partial;
  obj->render_selected = alb_line_render_selected;
  obj->get_selection_updates = alb_line_get_selection_updates;
  obj->duplicate = alb_line_duplicate;
  obj->part_is_transparent = alb_line_part_is_transparent;
  obj->display_props = alb_line_display_props;
  obj->resize_event = alb_line_resize;
  obj->get_resize_box_coords = alb_line_get_resize_box_coords;
  obj->check_resize_click = alb_line_check_resize_click;
  obj->click_is_selection = alb_line_click_is_selection;
  obj->update_dimensions_relative = alb_line_update_dimensions_relative;
  obj->get_updates = alb_line_get_updates;
  obj->get_rendered_area = alb_line_get_rendered_area;
  obj->rotate = alb_line_rotate;
  obj->sizemode = SIZEMODE_STRETCH;
  obj->alignment = ALIGN_NONE;
  alb_object_set_type(obj, ALB_TYPE_LINE);
  obj->name = estrdup("New line");

  D_RETURN_(5);
}

alb_object *
alb_line_new_from_to(int start_x,
                     int start_y,
                     int end_x,
                     int end_y,
                     int a,
                     int r,
                     int g,
                     int b)
{
  alb_line *line;
  alb_object *obj;

  D_ENTER(5);

  obj = alb_line_new();
  line = ALB_LINE(obj);

  line->a = a;
  line->b = b;
  line->g = g;
  line->r = r;

  alb_line_change_from_to(line, start_x, start_y, end_x, end_y);

  D_RETURN(5, ALB_OBJECT(line));
}

void
alb_line_free(alb_object * obj)
{
  alb_line *line;

  D_ENTER(5);

  line = (alb_line *) obj;

  if (!line)
    D_RETURN_(5);

  efree(line);

  D_RETURN_(5);
}

void
alb_line_render(alb_object * obj,
                Imlib_Image dest)
{
  alb_line *line;

  D_ENTER(5);

  if (!alb_object_get_state(obj, VISIBLE))
    D_RETURN_(5);

  line = ALB_LINE(obj);

  alb_line_render_partial(obj, dest, obj->x, obj->y, obj->w + 1, obj->h + 1);

  D_RETURN_(5);
}

void
alb_line_render_partial(alb_object * obj,
                        Imlib_Image dest,
                        int x,
                        int y,
                        int w,
                        int h)
{
  alb_line *line;

  D_ENTER(5);

  if (!alb_object_get_state(obj, VISIBLE))
    D_RETURN_(5);

  line = ALB_LINE(obj);

  gib_imlib_line_clip_and_draw(dest, line->start.x + obj->x,
                               line->start.y + obj->y, line->end.x + obj->x,
                               line->end.y + obj->y, x, y, w, h, line->r,
                               line->g, line->b, line->a);

  D_RETURN_(5);
}

alb_object *
alb_line_duplicate(alb_object * obj)
{
  alb_object *ret;
  alb_line *line;

  D_ENTER(3);

  line = ALB_LINE(obj);

  ret =
    alb_line_new_from_to(line->start.x, line->start.y, line->end.x,
                         line->end.y, line->a, line->r, line->g, line->b);
  if (ret) {
    ret->x = obj->x;
    ret->y = obj->y;
    ret->alias = obj->alias;
    ret->state = obj->state;
    ret->name =
      g_strjoin(" ", "Copy of", obj->name ? obj->name : "Untitled object",
                NULL);
  }

  D_RETURN(3, ret);
}

unsigned char
alb_line_part_is_transparent(alb_object * obj,
                             int x,
                             int y)
{
  D_ENTER(3);

  /* TODO Check if point crosses line */

  D_RETURN(3, ALB_LINE(obj)->a == 0 ? 1 : 0);
}

void
alb_line_resize(alb_object * obj,
                int x,
                int y)
{
  alb_line *line;
  int start_x, start_y, end_x, end_y;

  D_ENTER(5);

  line = ALB_LINE(obj);

  D(5, ("resize to %d,%d\n", x, y));

  x += obj->clicked_x;
  y += obj->clicked_y;

  start_x = line->start.x + obj->x;
  start_y = line->start.y + obj->y;
  end_x = line->end.x + obj->x;
  end_y = line->end.y + obj->y;

  switch (obj->resize) {
    case RESIZE_RIGHT:
      end_x = x;
      end_y = y;
      break;
    case RESIZE_LEFT:
      start_x = x;
      start_y = y;
      break;
    default:
      printf("eeeeek\n");
      break;
  }
  alb_line_change_from_to(line, start_x, start_y, end_x, end_y);
  D_RETURN_(3);
}

void
alb_line_change_from_to(alb_line * line,
                        int start_x,
                        int start_y,
                        int end_x,
                        int end_y)
{
  alb_object *obj;

  D_ENTER(3);

  obj = ALB_OBJECT(line);

  obj->x = start_x;
  obj->y = start_y;
  obj->w = end_x - start_x;
  obj->h = end_y - start_y;

  if (obj->w == 0)
    obj->w = 1;
  if (obj->h == 0)
    obj->h = 1;

  if (obj->w < 0) {
    obj->x += obj->w;
    obj->w = 0 - obj->w;
  }
  if (obj->h < 0) {
    obj->y += obj->h;
    obj->h = 0 - obj->h;
  }

  line->start.x = start_x - obj->x;
  line->start.y = start_y - obj->y;
  line->end.x = end_x - obj->x;
  line->end.y = end_y - obj->y;

  obj->rendered_x = 0;
  obj->rendered_y = 0;

  D_RETURN_(5);
}


static void
refresh_r_cb(GtkWidget * widget,
             gpointer * obj)
{

  ALB_LINE(obj)->r =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  alb_object_dirty(ALB_OBJECT(obj));
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}

static void
refresh_g_cb(GtkWidget * widget,
             gpointer * obj)
{

  ALB_LINE(obj)->g =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  alb_object_dirty(ALB_OBJECT(obj));
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}

static void
refresh_b_cb(GtkWidget * widget,
             gpointer * obj)
{

  ALB_LINE(obj)->b =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  alb_object_dirty(ALB_OBJECT(obj));
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}

static void
refresh_a_cb(GtkWidget * widget,
             gpointer * obj)
{

  ALB_LINE(obj)->a =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  alb_object_dirty(ALB_OBJECT(obj));
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}


GtkWidget *
alb_line_display_props(alb_object * obj)
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

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(cr), ALB_LINE(obj)->r);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(cg), ALB_LINE(obj)->g);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(cb), ALB_LINE(obj)->b);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ca), ALB_LINE(obj)->a);


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

int
alb_line_get_clipped_line(alb_line * line,
                          int *clip_x0,
                          int *clip_y0,
                          int *clip_x1,
                          int *clip_y1)
{
  alb_object *obj;

  D_ENTER(3);
  obj = ALB_OBJECT(line);

  D_RETURN(3,
           imlib_clip_line(line->start.x + obj->rendered_x + obj->x,
                           line->start.y + obj->rendered_y + obj->y,
                           line->end.x + obj->rendered_x + obj->x,
                           line->end.y + obj->rendered_y + obj->y, obj->x,
                           obj->x + obj->w, obj->y, obj->y + obj->h, clip_x0,
                           clip_y0, clip_x1, clip_y1));
}

void
alb_line_render_selected(alb_object * obj,
                         Imlib_Image dest,
                         unsigned char multiple)
{
  int clip_x0, clip_y0, clip_x1, clip_y1;
  alb_line *line;

  D_ENTER(5);

  line = ALB_LINE(obj);

  if (!alb_line_get_clipped_line
      (ALB_LINE(line), &clip_x0, &clip_y0, &clip_x1, &clip_y1))
    D_RETURN_(5);

  if (multiple) {
    gib_imlib_image_draw_rectangle(dest, clip_x0 - HALF_SEL_WIDTH,
                                   clip_y0 - HALF_SEL_HEIGHT,
                                   2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT, 0,
                                   0, 0, 255);
    gib_imlib_image_draw_rectangle(dest, clip_x1 - HALF_SEL_WIDTH,
                                   clip_y1 - HALF_SEL_HEIGHT,
                                   2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT, 0,
                                   0, 0, 255);
  } else {
    gib_imlib_image_fill_rectangle(dest, clip_x0 - HALF_SEL_WIDTH,
                                   clip_y0 - HALF_SEL_HEIGHT,
                                   2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT, 0,
                                   0, 0, 255);
    gib_imlib_image_fill_rectangle(dest, clip_x1 - HALF_SEL_WIDTH,
                                   clip_y1 - HALF_SEL_HEIGHT,
                                   2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT, 0,
                                   0, 0, 255);
  }
}

Imlib_Updates
alb_line_get_selection_updates(alb_object * obj)
{
  Imlib_Updates up = NULL;
  int clip_x0, clip_y0, clip_x1, clip_y1;
  alb_line *line;

  D_ENTER(5);

  line = ALB_LINE(obj);

  if (!alb_line_get_clipped_line
      (ALB_LINE(line), &clip_x0, &clip_y0, &clip_x1, &clip_y1))
    D_RETURN(5, NULL);

  up =
    imlib_update_append_rect(up, clip_x0 - HALF_SEL_WIDTH,
                             clip_y0 - HALF_SEL_HEIGHT, 2 * HALF_SEL_WIDTH,
                             2 * HALF_SEL_HEIGHT);
  up =
    imlib_update_append_rect(up, clip_x1 - HALF_SEL_WIDTH,
                             clip_y1 - HALF_SEL_HEIGHT, 2 * HALF_SEL_WIDTH,
                             2 * HALF_SEL_HEIGHT);

  D_RETURN(5, up);
}

void
alb_line_get_resize_box_coords(alb_object * obj,
                               int resize,
                               int *x,
                               int *y)
{
  int clip_x0, clip_y0, clip_x1, clip_y1;
  alb_line *line;

  D_ENTER(3);

  line = ALB_LINE(obj);

  if (!alb_line_get_clipped_line
      (ALB_LINE(line), &clip_x0, &clip_y0, &clip_x1, &clip_y1))
    D_RETURN_(3);

  switch (resize) {
    case RESIZE_LEFT:
      *x = clip_x0;
      *y = clip_y0;
      break;
    case RESIZE_RIGHT:
      *x = clip_x1;
      *y = clip_y1;
      break;
    default:
      break;
  }
  D_RETURN_(3);
}

int
alb_line_check_resize_click(alb_object * obj,
                            int x,
                            int y)
{
  int clip_x0, clip_y0, clip_x1, clip_y1;
  alb_line *line;

  D_ENTER(5);

  line = ALB_LINE(obj);

  if (!alb_line_get_clipped_line
      (ALB_LINE(line), &clip_x0, &clip_y0, &clip_x1, &clip_y1))
    D_RETURN(5, RESIZE_NONE);


  if (XY_IN_RECT
      (x, y, clip_x0 - HALF_SEL_WIDTH, clip_y0 - HALF_SEL_HEIGHT,
       2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT))
    D_RETURN(5, RESIZE_LEFT);

  if (XY_IN_RECT
      (x, y, clip_x1 - HALF_SEL_WIDTH, clip_y1 - HALF_SEL_HEIGHT,
       2 * HALF_SEL_WIDTH, 2 * HALF_SEL_HEIGHT))
    D_RETURN(5, RESIZE_RIGHT);

  D_RETURN(5, RESIZE_NONE);
}

unsigned char
alb_line_click_is_selection(alb_object * obj,
                            int x,
                            int y)
{
  int clip_x0, clip_y0, clip_x1, clip_y1;
  alb_line *line;
  int i;
  double gradient;
  int line_y, line_x;
  int ox, oy, ow, oh, dy, dx;

  D_ENTER(3);

  line = ALB_LINE(obj);

  /* handle simple case of translucent line */
  if (line->a <= TRANS_THRESHOLD)
    D_RETURN(3, 0);

  /* check whether click is anywhere near line */
  alb_object_get_rendered_area(obj, &ox, &oy, &ow, &oh);
  if (!XY_IN_RECT(x, y, ox - 5, oy - 5, ow + 10, oh + 10)) {
    D(5, ("xy not in rect\n"));
    D_RETURN(3, 0);
  }

  /* Here, we need to see if the click is within a few pixels of the line in
     either direction */
  if (!alb_line_get_clipped_line
      (line, &clip_x0, &clip_y0, &clip_x1, &clip_y1)) {
    D(5, ("no clipped line returned\n"));
    D_RETURN(3, 0);
  }

  /* need to handle special cases of vertical lines, otherwise the gradient
     is infinite. This obviously leads to tricky maths ;-) */
  if ((clip_x1 >= clip_x0 - 5) && (clip_x1 <= clip_x0 + 5)) {
    if ((x >= clip_x0 - 5) && (x <= clip_x0 + 5) && (y >= clip_y0 - 5)
        && (y <= clip_y1 + 5))
      D_RETURN(3, 1);
  }

  gradient = ((double) clip_y1 - clip_y0) / ((double) clip_x1 - clip_x0);
  D(5, ("gradient %f\n", gradient));
  if (clip_x0 < clip_x1) {
    if (clip_y0 < clip_y1) {
      /*  a 
       *   \
       *    \
       *     \
       *      b
       */
      dy = clip_y1 - clip_y0;
      dx = clip_x1 - clip_x0;
      if (dx > dy) {
        /* shallow */
        D(5, ("shallow downwards line left to right\n"));
        for (i = x - 5; i < x + 6; i++) {
          line_y = (gradient * (i - clip_x0)) + clip_y0;
          if ((y >= line_y - 5) && (y <= line_y + 5))
            D_RETURN(3, 1);
        }
      } else {
        /* steep */
        D(5, ("steep downwards line left to right\n"));
        for (i = y - 5; i < y + 6; i++) {
          line_x = ((1 / gradient) * (i - clip_y0)) + clip_x0;
          if ((x >= line_x - 5) && (x <= line_x + 5))
            D_RETURN(3, 1);
        }
      }
    } else {
      /*      b
       *     /
       *    /
       *   /
       *  a
       */
      dy = clip_y0 - clip_y1;
      dx = clip_x1 - clip_x0;
      if (dx > dy) {
        /* shallow */
        D(5, ("shallow upwards line left to right\n"));
        for (i = x - 5; i < x + 6; i++) {
          line_y = (gradient * (i - clip_x0)) + clip_y0;
          if ((y >= line_y - 5) && (y <= line_y + 5))
            D_RETURN(3, 1);
        }
      } else {
        /* steep */
        D(5, ("steep upwards line left to right\n"));
        for (i = y - 5; i < y + 6; i++) {
          line_x = ((1 / gradient) * (i - clip_y0)) + clip_x0;
          if ((x >= line_x - 5) && (x <= line_x + 5))
            D_RETURN(3, 1);
        }
      }
    }
  } else {
    if (clip_y0 < clip_y1) {
      /*      a
       *     /
       *    /
       *   /
       *  b
       */
      dy = clip_y1 - clip_y0;
      dx = clip_x0 - clip_x1;
      if (dx > dy) {
        /* shallow */
        D(5, ("shallow downwards line right to left\n"));
        for (i = x - 5; i < x + 6; i++) {
          line_y = (gradient * (i - clip_x0)) + clip_y0;
          if ((y >= line_y - 5) && (y <= line_y + 5))
            D_RETURN(3, 1);
        }
      } else {
        /* steep */
        D(5, ("steep downwards line right to left\n"));
        for (i = y - 5; i < y + 6; i++) {
          line_x = ((1 / gradient) * (i - clip_y0)) + clip_x0;
          if ((x >= line_x - 5) && (x <= line_x + 5))
            D_RETURN(3, 1);
        }
      }
    } else {
      /*  b
       *   \
       *    \
       *     \
       *      a
       */
      dy = clip_y0 - clip_y1;
      dx = clip_x0 - clip_x1;
      if (dx > dy) {
        /* shallow */
        D(5, ("shallow upwards line right to left\n"));
        for (i = x - 5; i < x + 6; i++) {
          line_y = (gradient * (i - clip_x0)) + clip_y0;
          if ((y >= line_y - 5) && (y <= line_y + 5))
            D_RETURN(3, 1);
        }
      } else {
        /* steep */
        D(5, ("steep upwards line right to left\n"));
        for (i = y - 5; i < y + 6; i++) {
          line_x = ((1 / gradient) * (i - clip_y0)) + clip_x0;
          if ((x >= line_x - 5) && (x <= line_x + 5))
            D_RETURN(3, 1);
        }
      }
    }
  }

  D_RETURN(3, 0);
}

Imlib_Updates
alb_line_get_updates(alb_object * obj)
{
  Imlib_Updates up = NULL;
  int clip_x0, clip_y0, clip_x1, clip_y1;
  alb_line *line;
  int i;
  double gradient;
  int line_y, line_x;
  int dy, dx;

  D_ENTER(3);

  line = ALB_LINE(obj);

  if (!alb_line_get_clipped_line
      (line, &clip_x0, &clip_y0, &clip_x1, &clip_y1)) {
    D(5, ("no clipped line returned\n"));
    D_RETURN(3, 0);
  }


  /* need to handle special cases of vertical lines, otherwise the gradient
     is infinite. This obviously leads to tricky maths ;-) */
  if (clip_x1 == clip_x0) {
    up =
      imlib_update_append_rect(up, clip_x0 - 1, clip_y0 - 1,
                               clip_x1 - clip_x0 + 2, clip_y1 - clip_y0 + 2);
  } else if (clip_y1 == clip_y0) {
    up =
      imlib_update_append_rect(up, clip_x0 - 1, clip_y0 - 1,
                               clip_x1 - clip_x0 + 2, clip_y1 - clip_y0 + 2);

  } else {
    gradient = ((double) clip_y1 - clip_y0) / ((double) clip_x1 - clip_x0);
    D(5, ("gradient %f\n", gradient));
    if (clip_x0 < clip_x1) {
      if (clip_y0 < clip_y1) {
        /*  a 
         *   \
         *    \
         *     \
         *      b
         */
        dy = clip_y1 - clip_y0;
        dx = clip_x1 - clip_x0;
        if (dx > dy) {
          /* shallow */
          D(5, ("shallow downwards line left to right\n"));
          for (i = clip_x0; i <= clip_x1; i++) {
            line_y = (gradient * (i - clip_x0)) + clip_y0;
            up = imlib_update_append_rect(up, i - 2, line_y - 2, 4, 4);
          }
        } else {
          /* steep */
          D(5, ("steep downwards line left to right\n"));
          for (i = clip_y0; i <= clip_y1; i++) {
            line_x = ((1 / gradient) * (i - clip_y0)) + clip_x0;
            up = imlib_update_append_rect(up, line_x - 2, i - 2, 4, 4);
          }
        }
      } else {

        /*      b
         *     /
         *    /
         *   /
         *  a
         */
        dy = clip_y0 - clip_y1;
        dx = clip_x1 - clip_x0;
        if (dx > dy) {
          /* shallow */
          D(5, ("shallow upwards line left to right\n"));
          for (i = clip_x0; i <= clip_x1; i++) {
            line_y = (gradient * (i - clip_x0)) + clip_y0;
            up = imlib_update_append_rect(up, i - 2, line_y - 2, 4, 4);
          }
        } else {
          /* steep */
          D(5, ("steep upwards line left to right\n"));
          for (i = clip_y1; i <= clip_y0; i++) {
            line_x = ((1 / gradient) * (i - clip_y0)) + clip_x0;
            up = imlib_update_append_rect(up, line_x - 2, i - 2, 4, 4);
          }
        }
      }
    } else {
      if (clip_y0 < clip_y1) {
        /*      a
         *     /
         *    /
         *   /
         *  b
         */
        dy = clip_y1 - clip_y0;
        dx = clip_x0 - clip_x1;
        if (dx > dy) {
          /* shallow */
          D(5, ("shallow downwards line right to left\n"));
          for (i = clip_x1; i <= clip_x0; i++) {
            line_y = (gradient * (i - clip_x0)) + clip_y0;
            up = imlib_update_append_rect(up, i - 2, line_y - 2, 4, 4);
          }
        } else {
          /* steep */
          D(5, ("steep downwards line right to left\n"));
          for (i = clip_y0; i <= clip_y1; i++) {
            line_x = ((1 / gradient) * (i - clip_y0)) + clip_x0;
            up = imlib_update_append_rect(up, line_x - 2, i - 2, 4, 4);
          }
        }
      } else {
        /*  b
         *   \
         *    \
         *     \
         *      a
         */
        dy = clip_y0 - clip_y1;
        dx = clip_x0 - clip_x1;
        if (dx > dy) {
          /* shallow */
          D(5, ("shallow upwards line right to left\n"));
          for (i = clip_x1; i <= clip_x0; i++) {
            line_y = (gradient * (i - clip_x0)) + clip_y0;
            up = imlib_update_append_rect(up, i - 2, line_y - 2, 4, 4);
          }
        } else {
          /* steep */
          D(5, ("steep upwards line right to left\n"));
          for (i = clip_y1; i <= clip_y0; i++) {
            line_x = ((1 / gradient) * (i - clip_y0)) + clip_x0;
            up = imlib_update_append_rect(up, line_x - 2, i - 2, 4, 4);
          }
        }
      }
    }
  }

  D_RETURN(3, up);
}

void
alb_line_update_dimensions_relative(alb_object * obj,
                                    int w_offset,
                                    int h_offset)
{
  alb_line *line;
  int x, y;

  D_ENTER(3);
  obj->resize = RESIZE_RIGHT;
  line = ALB_LINE(obj);

  if (line->end.x > line->start.x)
    x = obj->x + line->end.x + w_offset;
  else
    x = obj->x + line->end.x - w_offset;

  if (line->end.y > line->start.y)
    y = obj->y + line->end.y + h_offset;
  else
    y = obj->y + line->end.y - h_offset;

  alb_line_resize(obj, x, y);

  D_RETURN_(3);
}

void
alb_line_rotate(alb_object * obj,
                double angle)
{
  alb_line *line;
  double rot_start_x, rot_start_y, rot_end_x, rot_end_y, center_x, center_y;

  /*the cartesian coordinates relative to the center point */
  int end_X, end_Y, start_X, start_Y;

  D_ENTER(3);

  line = ALB_LINE(obj);

  center_x = obj->x + (obj->w / 2);
  center_y = obj->y + (obj->h / 2);

  angle = angle * 2 * 3.141592654 / 360;

  if (line->start.x < line->end.x) {
    start_X = obj->x - center_x;
    end_X = obj->x + obj->w - center_x;

    if (line->start.y < line->end.y) {
      /*  a 
       *   \
       *    \
       *     \
       *      b
       */
      start_Y = obj->y - center_y;
      end_Y = obj->y + obj->h - center_y;
    } else {
      /*      b
       *     /
       *    /
       *   /
       *  a
       */
      start_Y = obj->y + obj->h - center_y;
      end_Y = obj->y - center_y;
    }
  } else {
    start_X = obj->x + obj->w - center_x;
    end_X = obj->x - center_x;

    if (line->start.y < line->end.y) {
      /*      a
       *     /
       *    /
       *   /
       *  b
       */
      start_Y = obj->y - center_y;
      end_Y = obj->y + obj->h - center_y;
    } else {
      /*  b
       *   \
       *    \
       *     \
       *      a
       */
      start_Y = obj->y + obj->h - center_y;
      end_Y = obj->y - center_y;
    }
  }

  rot_start_x =
    rint(center_x + (start_X * cos(angle) - start_Y * sin(angle)));
  rot_start_y =
    rint(center_y + (start_X * sin(angle) + start_Y * cos(angle)));

  rot_end_x = rint(center_x + (end_X * cos(angle) - end_Y * sin(angle)));
  rot_end_y = rint(center_y + (end_X * sin(angle) + end_Y * cos(angle)));

  alb_object_dirty(obj);
  alb_line_change_from_to(line, rot_start_x, rot_start_y, rot_end_x,
                          rot_end_y);

  D_RETURN_(3);
}

void
alb_line_get_rendered_area(alb_object * obj,
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
