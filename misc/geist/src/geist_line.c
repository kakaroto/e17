#include "geist_line.h"

static void refresh_r_cb(GtkWidget * widget, gpointer * obj);
static void refresh_g_cb(GtkWidget * widget, gpointer * obj);
static void refresh_b_cb(GtkWidget * widget, gpointer * obj);
static void refresh_a_cb(GtkWidget * widget, gpointer * obj);

geist_object *
geist_line_new(void)
{
   geist_line *line;

   D_ENTER(5);

   line = emalloc(sizeof(geist_line));
   geist_line_init(line);

   geist_object_set_state(GEIST_OBJECT(line), VISIBLE);

   D_RETURN(5, GEIST_OBJECT(line));
}

void
geist_line_init(geist_line * line)
{
   geist_object *obj;

   D_ENTER(5);
   memset(line, 0, sizeof(geist_line));
   obj = GEIST_OBJECT(line);
   geist_object_init(obj);
   obj->free = geist_line_free;
   obj->render = geist_line_render;
   obj->render_partial = geist_line_render_partial;
   obj->duplicate = geist_line_duplicate;
   obj->part_is_transparent = geist_line_part_is_transparent;
   obj->display_props = geist_line_display_props;
   obj->resize_event = geist_line_resize;
   obj->sizemode = SIZEMODE_STRETCH;
   obj->alignment = ALIGN_NONE;
   geist_object_set_type(obj, GEIST_TYPE_LINE);
   obj->name = estrdup("New line");

   D_RETURN_(5);
}

geist_object *
geist_line_new_from_to(int start_x, int start_y, int end_x, int end_y, int a,
                       int r, int g, int b)
{
   geist_line *line;
   geist_object *obj;

   D_ENTER(5);

   obj = geist_line_new();
   line = GEIST_LINE(obj);

   line->a = a;
   line->b = b;
   line->g = g;
   line->r = r;

   obj->x = MIN(start_x, end_x);
   obj->y = MIN(start_y, end_y);
   obj->w = obj->rendered_w = MAX(start_x, end_x) - obj->x;
   obj->h = obj->rendered_h = MAX(start_y, end_y) - obj->x;
   obj->rendered_x = 0;
   obj->rendered_y = 0;

   line->start.x = start_x - obj->x;
   line->start.y = start_y - obj->y;
   line->end.x = end_x - obj->x;
   line->end.y = end_y - obj->y;

   D_RETURN(5, GEIST_OBJECT(line));
}

void
geist_line_free(geist_object * obj)
{
   geist_line *line;

   D_ENTER(5);

   line = (geist_line *) obj;

   if (!line)
      D_RETURN_(5);

   efree(line);

   D_RETURN_(5);
}

void
geist_line_render(geist_object * obj, Imlib_Image dest)
{
   geist_line *line;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   line = GEIST_LINE(obj);

   geist_line_render_partial(obj, dest, obj->x, obj->y, obj->w, obj->h);

   D_RETURN_(5);
}

void
geist_line_render_partial(geist_object * obj, Imlib_Image dest, int x, int y,
                          int w, int h)
{
   geist_line *line;
   int sw, sh, dw, dh, sx, sy, dx, dy;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   line = GEIST_LINE(obj);

#if 0
   int ox, oy, ow, oh;

   geist_object_get_rendered_area(obj, &ox, &oy, &ow, &oh);
   printf("\nrendered area %d,%d %dx%d\n", ox, oy, ow, oh);
   printf("clipping against %d,%d %dx%d\n", x, y, w, h);
   CLIP(ox, oy, ow, oh, x, y, w, h);

   printf("clipped area %d,%d %dx%d\n", ox, oy, ow, oh);

   geist_imlib_line_clip_and_draw(dest,
                                  line->start.x + obj->rendered_x + obj->x,
                                  line->start.y + obj->rendered_y + obj->y,
                                  line->end.x + obj->rendered_x + obj->x,
                                  line->end.y + obj->rendered_y + obj->y, ox,
                                  ox + ow, oy, oy + oh, line->r, line->g,
                                  line->b, line->a);
#endif

   if (obj->rendered_x < 0)
      sx = x - obj->x;
   else
      sx = x - (obj->x + obj->rendered_x);
   if (obj->rendered_y < 0)
      sy = y - obj->y;
   else
      sy = y - (obj->y + obj->rendered_y);

   if (sx < 0)
      sx = 0;
   if (sy < 0)
      sy = 0;

   if (obj->rendered_w > obj->w)
      sw = obj->w - sx;
   else
      sw = obj->rendered_w - sx;

   if (obj->rendered_h > obj->h)
      sh = obj->h - sy;
   else
      sh = obj->rendered_h - sy;

   if (sw > w)
      sw = w;
   if (sh > h)
      sh = h;

   if (obj->rendered_x < 0)
      dx = obj->x + sx;
   else
      dx = (obj->x + obj->rendered_x) + sx;
   if (obj->rendered_y < 0)
      dy = obj->y + sy;
   else
      dy = (obj->y + obj->rendered_y) + sy;
   dw = sw;
   dh = sh;

   geist_imlib_line_clip_and_draw(dest,
                                  line->start.x + obj->rendered_x + obj->x,
                                  line->start.y + obj->rendered_y + obj->y,
                                  line->end.x + obj->rendered_x + obj->x,
                                  line->end.y + obj->rendered_y + obj->y, dx,
                                  dx + dw, dy, dy + dh, line->r, line->g,
                                  line->b, line->a);

   D_RETURN_(5);
}

geist_object *
geist_line_duplicate(geist_object * obj)
{
   geist_object *ret;
   geist_line *line;

   D_ENTER(3);

   line = GEIST_LINE(obj);

   ret =
      geist_line_new_from_to(line->start.x, line->start.y, line->end.x,
                             line->end.y, line->a, line->r, line->g, line->b);
   ret->w = obj->w;
   ret->h = obj->h;
   if (ret)
   {
      ret->alias = obj->alias;
      ret->state = obj->state;
      ret->name =
         g_strjoin(" ", "Copy of", obj->name ? obj->name : "Untitled object",
                   NULL);
   }

   D_RETURN(3, ret);
}

unsigned char
geist_line_part_is_transparent(geist_object * obj, int x, int y)
{
   D_ENTER(3);

   /* TODO Check if point crosses line */

   D_RETURN(3, GEIST_LINE(obj)->a == 0 ? 1 : 0);
}

void
geist_line_resize(geist_object * obj, int x, int y)
{
   D_ENTER(5);

   D(5, ("resize to %d,%d\n", x, y));

   /* FIXME update start and end points */
   geist_object_resize_object(obj, x, y);

   D_RETURN_(5);
}


static void
refresh_r_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_LINE(obj)->r =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_object_dirty(GEIST_OBJECT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

static void
refresh_g_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_LINE(obj)->g =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_object_dirty(GEIST_OBJECT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

static void
refresh_b_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_LINE(obj)->b =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_object_dirty(GEIST_OBJECT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

static void
refresh_a_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_LINE(obj)->a =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_object_dirty(GEIST_OBJECT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}


GtkWidget *
geist_line_display_props(geist_object * obj)
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

   gtk_spin_button_set_value(GTK_SPIN_BUTTON(cr), GEIST_LINE(obj)->r);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(cg), GEIST_LINE(obj)->g);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(cb), GEIST_LINE(obj)->b);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(ca), GEIST_LINE(obj)->a);


   gtk_signal_connect(GTK_OBJECT(cr), "changed",
                      GTK_SIGNAL_FUNC(refresh_r_cb), (gpointer) obj);
   gtk_signal_connect(GTK_OBJECT(cg), "changed",
                      GTK_SIGNAL_FUNC(refresh_g_cb), (gpointer) obj);
   gtk_signal_connect(GTK_OBJECT(ca), "changed",
                      GTK_SIGNAL_FUNC(refresh_a_cb), (gpointer) obj);
   gtk_signal_connect(GTK_OBJECT(cb), "changed",
                      GTK_SIGNAL_FUNC(refresh_b_cb), (gpointer) obj);

   gtk_widget_show(table);
   return (win);

}
