#include "geist_rect.h"

void refresh_r_cb(GtkWidget * widget, gpointer * obj);
void refresh_g_cb(GtkWidget * widget, gpointer * obj);
void refresh_b_cb(GtkWidget * widget, gpointer * obj);
void refresh_a_cb(GtkWidget * widget, gpointer * obj);
void obj_addrect_ok_cb(GtkWidget * widget, gpointer * data);

geist_object *
geist_rect_new(void)
{
   geist_rect *rec;

   D_ENTER(5);

   rec = emalloc(sizeof(geist_rect));
   geist_rect_init(rec);

   geist_object_set_state(GEIST_OBJECT(rec), VISIBLE);

   D_RETURN(5, GEIST_OBJECT(rec));
}

void
geist_rect_init(geist_rect * rec)
{
   geist_object *obj;

   D_ENTER(5);
   memset(rec, 0, sizeof(geist_rect));
   obj = GEIST_OBJECT(rec);
   geist_object_init(obj);
   obj->free = geist_rect_free;
   obj->render = geist_rect_render;
   obj->render_partial = geist_rect_render_partial;
   obj->duplicate = geist_rect_duplicate;
   obj->part_is_transparent = geist_text_part_is_transparent;
   obj->display_props = geist_rect_display_props;
   obj->resize_event = geist_rect_resize;
   obj->sizemode = SIZEMODE_STRETCH;
   obj->alignment = ALIGN_NONE;
   geist_object_set_type(obj, GEIST_TYPE_RECT);
   obj->name = estrdup("New rectangle");

   D_RETURN_(5);
}

geist_object *
geist_rect_new_of_size(int x, int y, int w, int h, int a, int r, int g, int b)
{
   geist_rect *rec;
   geist_object *obj;

   D_ENTER(5);

   obj = geist_rect_new();
   rec = GEIST_RECT(obj);

   rec->a = a;
   rec->b = b;
   rec->g = g;
   rec->r = r;

   obj->x = x;
   obj->y = y;
   obj->w = obj->rendered_w = w;
   obj->h = obj->rendered_h = h;
   obj->rendered_x = 0;
   obj->rendered_y = 0;

   D_RETURN(5, GEIST_OBJECT(rec));
}

void
geist_rect_free(geist_object * obj)
{
   geist_rect *rec;

   D_ENTER(5);

   rec = (geist_rect *) obj;

   if (!rec)
      D_RETURN_(5);

   efree(rec);

   D_RETURN_(5);
}

void
geist_rect_render(geist_object * obj, Imlib_Image dest)
{
   geist_rect *rec;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   rec = GEIST_RECT(obj);
   D(5,
     ("rendering %d,%d %dx%d with %d,%d,%d,%d\n", obj->x, obj->y, obj->w,
      obj->h, rec->r, rec->g, rec->b, rec->a));

   geist_imlib_image_fill_rectangle(dest, obj->x, obj->y, obj->w, obj->h,
                                    rec->r, rec->g, rec->b, rec->a);

   D_RETURN_(5);
}

void
geist_rect_render_partial(geist_object * obj, Imlib_Image dest, int x, int y,
                          int w, int h)
{
   geist_rect *rec;
   int sw, sh, dw, dh, sx, sy, dx, dy;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   rec = GEIST_RECT(obj);

   sx = x - obj->x;
   sy = y - obj->y;

   if (sx < 0)
      sx = 0;
   if (sy < 0)
      sy = 0;

   sw = obj->w - sx;
   sh = obj->h - sy;

   if (sw > w)
      sw = w;
   if (sh > h)
      sh = h;

   dx = obj->x + sx;
   dy = obj->y + sy;
   dw = sw;
   dh = sh;

   D(5,
     ("partial rendering %d,%d %dx%d with %d,%d,%d,%d\n", dx, dy, dw, dh,
      rec->r, rec->g, rec->b, rec->a));

   geist_imlib_image_fill_rectangle(dest, dx, dy, dw, dh, rec->r, rec->g,
                                    rec->b, rec->a);

   D_RETURN_(5);
}

geist_object *
geist_rect_duplicate(geist_object * obj)
{
   geist_object *ret;
   geist_rect *rec;

   D_ENTER(3);

   rec = GEIST_RECT(obj);

   ret =
      geist_rect_new_of_size(obj->x, obj->y, obj->rendered_w, obj->rendered_h,
                             rec->a, rec->r, rec->g, rec->b);
   ret->rendered_x = obj->rendered_x;
   ret->rendered_y = obj->rendered_y;
   ret->w = obj->w;
   ret->h = obj->h;
   if (ret)
   {
      ret->state = obj->state;
      ret->name =
         g_strjoin(" ", "Copy of", obj->name ? obj->name : "Untitled object",
                   NULL);
   }

   D_RETURN(3, ret);
}

unsigned char
geist_text_part_is_transparent(geist_object * obj, int x, int y)
{
   D_ENTER(3);

   D_RETURN(3, GEIST_RECT(obj)->a == 0 ? 1 : 0);
}

void
geist_rect_resize(geist_object * obj, int x, int y)
{
   D_ENTER(5);

   D(5, ("resize to %d,%d\n", x, y));

   geist_object_resize_object(obj, x, y);

   D_RETURN_(5);
}

void
obj_addrect_ok_cb(GtkWidget * widget, gpointer * data)
{
   gtk_widget_destroy((GtkWidget *) data);
}

void
refresh_r_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_RECT(obj)->r =
		   gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_object_dirty(GEIST_OBJECT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

void
refresh_g_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_RECT(obj)->g =
		   gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_object_dirty(GEIST_OBJECT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

void
refresh_b_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_RECT(obj)->b =
		   gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_object_dirty(GEIST_OBJECT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

void
refresh_a_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_RECT(obj)->a =
		   gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_object_dirty(GEIST_OBJECT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

void
refresh_name_cb(GtkWidget *widget, gpointer *obj)
{
   GEIST_RECT(obj)->name = estrdup(gtk_entry_get_text(GTK_ENTRY(widget)));
}

void
geist_rect_display_props(geist_object * obj)
{

   GtkWidget *win, *table, *title_l, *title_entry, *hbox, *cr_l, *cr, *cg_l,
      *cg, *cb_l, *cb, *ca_l, *ca, *ok;
   GtkAdjustment *a1, *a2, *a3, *a4;


   a1 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
   a2 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
   a3 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
   a4 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);

   win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
   table = gtk_table_new(2, 4, FALSE);
   gtk_container_set_border_width(GTK_CONTAINER(win), 5);
   gtk_container_add(GTK_CONTAINER(win), table);

   title_l = gtk_label_new("name:");
   gtk_misc_set_alignment(GTK_MISC(title_l), 1.0, 0.5);
   gtk_table_attach(GTK_TABLE(table), title_l, 0, 1, 0, 1,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(title_l);

   title_entry = gtk_entry_new();
   gtk_table_attach(GTK_TABLE(table), title_entry, 1, 2, 0, 1,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_signal_connect(GTK_OBJECT(title_entry), "changed",
			GTK_SIGNAL_FUNC(refresh_name_cb), (gpointer) obj);
   gtk_widget_show(title_entry);

   hbox = gtk_hbox_new(FALSE, 0);

   cr_l = gtk_label_new("R:");
   gtk_misc_set_alignment(GTK_MISC(cr_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), cr_l, TRUE, FALSE, 2);
   gtk_widget_show(cr_l);

   cr = gtk_spin_button_new(GTK_ADJUSTMENT(a1), 1, 0);
   gtk_signal_connect(GTK_OBJECT(cr), "changed",
                      GTK_SIGNAL_FUNC(refresh_r_cb), (gpointer) obj);
   gtk_box_pack_start(GTK_BOX(hbox), cr, TRUE, FALSE, 2);
   gtk_widget_show(cr);

   cg_l = gtk_label_new("G:");
   gtk_misc_set_alignment(GTK_MISC(cg_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), cg_l, TRUE, FALSE, 2);
   gtk_widget_show(cg_l);
   cg = gtk_spin_button_new(GTK_ADJUSTMENT(a2), 1, 0);
   gtk_signal_connect(GTK_OBJECT(cg), "changed",
                      GTK_SIGNAL_FUNC(refresh_g_cb), (gpointer) obj);
   gtk_box_pack_start(GTK_BOX(hbox), cg, TRUE, FALSE, 2);
   gtk_widget_show(cg);

   cb_l = gtk_label_new("B:");
   gtk_misc_set_alignment(GTK_MISC(cb_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), cb_l, TRUE, FALSE, 2);
   gtk_widget_show(cb_l);
   cb = gtk_spin_button_new(GTK_ADJUSTMENT(a3), 1, 0);
   gtk_signal_connect(GTK_OBJECT(cb), "changed",
                      GTK_SIGNAL_FUNC(refresh_b_cb), (gpointer) obj);
   gtk_box_pack_start(GTK_BOX(hbox), cb, TRUE, FALSE, 2);
   gtk_widget_show(cb);

   ca_l = gtk_label_new("A:");
   gtk_misc_set_alignment(GTK_MISC(ca_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), ca_l, TRUE, FALSE, 2);
   gtk_widget_show(ca_l);
   ca = gtk_spin_button_new(GTK_ADJUSTMENT(a4), 1, 0);
   gtk_signal_connect(GTK_OBJECT(ca), "changed",
                      GTK_SIGNAL_FUNC(refresh_a_cb), (gpointer) obj);
   gtk_box_pack_start(GTK_BOX(hbox), ca, TRUE, FALSE, 2);
   gtk_widget_show(ca);

   gtk_table_attach(GTK_TABLE(table), hbox, 0, 2, 3, 4, GTK_FILL | GTK_EXPAND,
                    0, 2, 2);
   gtk_widget_show(hbox);

   ok = gtk_button_new_with_label("Ok");
   gtk_table_attach(GTK_TABLE(table), ok, 0, 2, 4, 5, GTK_FILL | GTK_EXPAND,
                    0, 2, 2);
   gtk_signal_connect(GTK_OBJECT(ok), "clicked",
                      GTK_SIGNAL_FUNC(obj_addrect_ok_cb), (gpointer) win);
   gtk_widget_show(ok);


   gtk_spin_button_set_value (cr, GEIST_RECT(obj)->r);
   gtk_spin_button_set_value (cg, GEIST_RECT(obj)->g);
   gtk_spin_button_set_value (cb, GEIST_RECT(obj)->b);
   gtk_spin_button_set_value (ca, GEIST_RECT(obj)->a);
   
   if (GEIST_RECT(obj)->name)
      gtk_entry_set_text(GTK_ENTRY(title_entry), GEIST_RECT(obj)->name);


   gtk_widget_show(table);
   gtk_widget_show(win);


   D_RETURN_(5);

}
