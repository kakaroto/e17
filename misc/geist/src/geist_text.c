#include "geist.h"
#include "geist_text.h"

typedef struct _addtext_ok_data addtext_ok_data;
struct _addtext_ok_data
{
   GtkWidget *win;
   GtkWidget *text;
   GtkWidget *font;
   GtkWidget *size;
   GtkWidget *cr;
   GtkWidget *cg;
   GtkWidget *cb;
   GtkWidget *ca;
};

static void refresh_r_cb(GtkWidget * widget, gpointer * obj);
static void refresh_g_cb(GtkWidget * widget, gpointer * obj);
static void refresh_b_cb(GtkWidget * widget, gpointer * obj);
static void refresh_a_cb(GtkWidget * widget, gpointer * obj);
static void refresh_size_cb(GtkWidget * widget, gpointer * obj);
static void refresh_text_cb(GtkWidget * widget, gpointer * obj);
static void refresh_font_cb(GtkWidget * widget, gpointer * obj);


geist_object *
geist_text_new(void)
{
   geist_text *txt;

   D_ENTER(5);

   txt = emalloc(sizeof(geist_text));
   geist_text_init(txt);

   geist_object_set_state(GEIST_OBJECT(txt), VISIBLE);

   D_RETURN(5, GEIST_OBJECT(txt));
}

void
geist_text_init(geist_text * txt)
{
   geist_object *obj;

   D_ENTER(5);
   memset(txt, 0, sizeof(geist_text));
   obj = GEIST_OBJECT(txt);
   geist_object_init(obj);
   obj->free = geist_text_free;
   obj->render = geist_text_render;
   obj->render_partial = geist_text_render_partial;
   obj->get_rendered_image = geist_text_get_rendered_image;
   obj->duplicate = geist_text_duplicate;
   obj->resize_event = geist_text_resize;
   obj->display_props = geist_text_display_props;
   obj->sizemode = SIZEMODE_NONE;
   obj->alignment = ALIGN_CENTER;
   geist_object_set_type(obj, GEIST_TYPE_TEXT);
   obj->name = estrdup("New text");

   D_RETURN_(5);
}

geist_object *
geist_text_new_with_text(int x, int y, char *fontname, int fontsize,
                         char *text, int a, int r, int g, int b)
{
   geist_text *txt;
   geist_object *obj;

   D_ENTER(5);

   obj = geist_text_new();
   txt = GEIST_TEXT(obj);

   txt->text = estrdup(text);
   if (geist_text_change_font(txt, fontname, fontsize))
   {
      geist_text_free(obj);
      D_RETURN(5, NULL);
   }

   txt->a = a;
   txt->b = b;
   txt->g = g;
   txt->r = r;

   obj->x = x;
   obj->y = y;
   obj->rendered_x = 0;
   obj->rendered_y = 0;

   txt->im = geist_text_create_image(txt, &obj->w, &obj->h);

   D_RETURN(5, GEIST_OBJECT(txt));
}

void
geist_text_free(geist_object * obj)
{
   geist_text *txt;

   D_ENTER(5);

   txt = (geist_text *) obj;

   if (!txt)
      D_RETURN_(5);

   if (txt->fontname)
      efree(txt->fontname);
   if (txt->text)
      efree(txt->text);
   if (txt->fn)
      geist_imlib_free_font(txt->fn);
   if (txt->im)
      geist_imlib_free_image(txt->im);

   efree(txt);

   D_RETURN_(5);
}

void
geist_text_render(geist_object * obj, Imlib_Image dest)
{
   geist_text *im;
   int sw, sh, dw, dh;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   im = GEIST_TEXT(obj);
   if (!im->im)
      D_RETURN_(5);

   dw = geist_imlib_image_get_width(dest);
   dh = geist_imlib_image_get_height(dest);
   sw = geist_imlib_image_get_width(im->im);
   sh = geist_imlib_image_get_height(im->im);

   D(3, ("Rendering text %p with text %s\n", obj, im->text));
   geist_imlib_blend_image_onto_image(dest, im->im, 0, 0, 0, sw, sh,
                                      obj->x + obj->rendered_x,
                                      obj->y + obj->rendered_y, sw, sh, 1, 1,
                                      im->alias);

   D_RETURN_(5);
}

void
geist_text_render_partial(geist_object * obj, Imlib_Image dest, int x, int y,
                          int w, int h)
{
   geist_text *im;
   int sw, sh, dw, dh, sx, sy, dx, dy;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   im = GEIST_TEXT(obj);
   if (!im->im)
      D_RETURN_(5);

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

   D(5, ("Rendering partial text %s\n", im->text));
   D(5,
     ("Rendering text area:\nsx: %d\tsy: %d\nsw: %d\tsh: %d\ndx: %d\tdy: %d\ndw: %d\tdh: %d\n",
      sx, sy, sw, sh, dx, dy, dw, dh));

   geist_imlib_blend_image_onto_image(dest, im->im, 0, sx, sy, sw, sh, dx, dy,
                                      dw, dh, 1, 1, im->alias);

   D_RETURN_(5);
}

void
geist_text_update_image(geist_text * txt)
{
   geist_object *obj;

   D_ENTER(3);

   obj = GEIST_OBJECT(txt);
   geist_object_dirty(obj);
   if (txt->im)
      geist_imlib_free_image_and_decache(txt->im);
   txt->im = geist_text_create_image(txt, &obj->rendered_w, &obj->rendered_h);
   obj->w = obj->rendered_w;
   obj->h = obj->rendered_h;
   obj->rendered_x = 0;
   obj->rendered_y = 0;
   geist_object_dirty(obj);
   D_RETURN_(3);
}


void
geist_text_change_text(geist_text * txt, char *newtext)
{
   D_ENTER(3);

   if (txt->text)
      efree(txt->text);
   txt->text = estrdup(newtext);

   geist_text_update_image(txt);

   D_RETURN_(3);
}

Imlib_Image geist_text_create_image(geist_text * txt, int *w, int *h)
{
   DATA8 atab[256];
   Imlib_Image im;
   geist_object *obj;

   D_ENTER(3);

   if (!txt->fn)
   {
      weprintf("no font for text.");
      D_RETURN(3, NULL);
   }
   if (!txt->text)
   {
      weprintf("no text in text object.");
      D_RETURN(3, NULL);
   }

   obj = GEIST_OBJECT(txt);
   geist_imlib_get_text_size(txt->fn, txt->text, w, h, IMLIB_TEXT_TO_RIGHT);
   obj->rendered_w = *w;
   obj->rendered_h = *h;

   im = imlib_create_image(*w, *h);
   if (!im)
   {
      weprintf("couldn't create imlib image for text area.");
      D_RETURN(3, NULL);
   }

   /* make image transparent (HACK - imlib2 should do this nicely) */
   geist_imlib_image_set_has_alpha(im, 1);
   memset(atab, 0, sizeof(atab));
   geist_imlib_apply_color_modifier_to_rectangle(im, 0, 0, obj->w, obj->h,
                                                 NULL, NULL, NULL, atab);


   /* todo - put offset, margin, color etc into txt struct */
   geist_imlib_text_draw(im, txt->fn, 0, 0, txt->text, IMLIB_TEXT_TO_RIGHT,
                         txt->r, txt->g, txt->b, txt->a);

   D_RETURN(3, im);
}

Imlib_Image
geist_text_get_rendered_image(geist_object * obj)
{
   D_ENTER(3);

   D_RETURN(3, GEIST_TEXT(obj)->im);
}

geist_object *
geist_text_duplicate(geist_object * obj)
{
   geist_object *ret;
   geist_text *txt;

   D_ENTER(3);

   txt = GEIST_TEXT(obj);

   ret =
      geist_text_new_with_text(obj->x, obj->y, txt->fontname, txt->fontsize, txt->text,
                               txt->a, txt->r, txt->g, txt->b);
   ret->rendered_x = obj->rendered_x;
   ret->rendered_y = obj->rendered_y;
   ret->w = obj->w;
   ret->h = obj->h;
   if (ret)
   {
      ret->state = obj->state;
      GEIST_IMAGE(ret)->alias = txt->alias;
      ret->name =
         g_strjoin(" ", "Copy of", obj->name ? obj->name : "Untitled object",
                   NULL);
   }

   D_RETURN(3, ret);
}

void
geist_text_resize(geist_object * obj, int x, int y)
{
   geist_text *txt;

   D_ENTER(5);

   txt = GEIST_TEXT(obj);

   D(5, ("resize to %d,%d\n", x, y));
   geist_object_resize_object(obj, x, y);

   D_RETURN_(5);
}



void
refresh_size_cb(GtkWidget * widget, gpointer * obj)
{
   int fontsize;
   geist_text *txt;

   txt = GEIST_TEXT(obj);

   fontsize = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_text_change_font(txt, txt->fontname, fontsize);
   geist_text_update_image(txt);
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

int
geist_text_change_font(geist_text * txt, char *fontname, int fontsize)
{
   Imlib_Font fn;
   char *buf;

   D_ENTER(3);

   buf = g_strdup_printf("%s/%d", fontname, fontsize);
   fn = imlib_load_font(buf);
   efree(buf);
   if (!fn)
   {
      weprintf("error loading font %s at size %d\n", fontname, fontsize);
      D_RETURN(3, 1);
   }

   /* we do this in case the func is called with txt->fontname as the fontname
    * arg */
   if (txt->fontname != fontname)
   {
      if (txt->fontname)
         efree(txt->fontname);
      txt->fontname = estrdup(fontname);
   }

   txt->fontsize = fontsize;
   if (txt->fn)
      geist_imlib_free_font(txt->fn);

   txt->fn = fn;

   D_RETURN(3, 0);
}

void
refresh_font_cb(GtkWidget * widget, gpointer * obj)
{
   char *fontname;
   geist_text *txt;

   txt = GEIST_TEXT(obj);

   fontname = gtk_entry_get_text(GTK_ENTRY(widget));

   if (!geist_text_change_font(txt, fontname, txt->fontsize))
   {
      geist_text_update_image(txt);
      geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   }
}

void
refresh_r_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_TEXT(obj)->r =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_text_update_image(GEIST_TEXT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

void
refresh_g_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_TEXT(obj)->g =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_text_update_image(GEIST_TEXT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

void
refresh_b_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_TEXT(obj)->b =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_text_update_image(GEIST_TEXT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

void
refresh_a_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_TEXT(obj)->a =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_text_update_image(GEIST_TEXT(obj));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}


void
refresh_text_cb(GtkWidget * widget, gpointer * obj)
{
   geist_text_change_text(GEIST_TEXT(obj),
                          estrdup(gtk_entry_get_text(GTK_ENTRY(widget))));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

GtkWidget *
geist_text_display_props(geist_object * obj)
{
   addtext_ok_data *ok_data = NULL;
   GtkWidget *table, *text_l, *font_l, *size_l, *hbox, *cr_l, *cg_l,
      *cb_l, *ca_l;
   int i, num;
   char **fonts;
   GList *list = g_list_alloc();
   GtkAdjustment *a1, *a2, *a3, *a4, *a5;

   a1 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
   a2 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
   a3 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
   a4 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
   a5 = (GtkAdjustment *) gtk_adjustment_new(12, 2, 96, 1, 2, 3);

   ok_data = emalloc(sizeof(addtext_ok_data));
   
   ok_data->win = gtk_hbox_new(FALSE,0);
   
   
   
   if (!ok_data)
      printf("ARGH");
   fonts = geist_imlib_list_fonts(&num);

   if (!fonts)
      printf("ARGH!\n");

   table = gtk_table_new(4, 2, FALSE);
   gtk_container_set_border_width(GTK_CONTAINER(ok_data->win), 5);
   gtk_container_add(GTK_CONTAINER(ok_data->win), table);
   
   text_l = gtk_label_new("Text:");
   gtk_misc_set_alignment(GTK_MISC(text_l), 1.0, 0.5);
   gtk_table_attach(GTK_TABLE(table), text_l, 0, 1, 1, 2,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(text_l);

   ok_data->text = gtk_entry_new();
   gtk_table_attach(GTK_TABLE(table), ok_data->text, 1, 2, 1, 2,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(ok_data->text);


   font_l = gtk_label_new("Font:");
   gtk_misc_set_alignment(GTK_MISC(font_l), 1.0, 0.5);
   gtk_table_attach(GTK_TABLE(table), font_l, 0, 1, 2, 3,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(font_l);

   ok_data->font = gtk_combo_new();
   gtk_table_attach(GTK_TABLE(table), ok_data->font, 1, 2, 2, 3,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   for (i = 0; i < num; i++)
      list = g_list_append(list, (gpointer) fonts[i]);
   gtk_combo_set_popdown_strings(GTK_COMBO(ok_data->font), list);
   gtk_widget_show(ok_data->font);

   size_l = gtk_label_new("Size:");
   gtk_misc_set_alignment(GTK_MISC(size_l), 1.0, 0.5);
   gtk_table_attach(GTK_TABLE(table), size_l, 0, 1, 3, 4,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(size_l);

   ok_data->size = gtk_spin_button_new(GTK_ADJUSTMENT(a5), 1, 0);
   gtk_table_attach(GTK_TABLE(table), ok_data->size, 1, 2, 3, 4,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(ok_data->size);

   hbox = gtk_hbox_new(FALSE, 0);
   
   gtk_table_attach(GTK_TABLE(table), hbox, 0, 2, 4, 5, GTK_FILL | GTK_EXPAND,
                    0, 2, 2);

   cr_l = gtk_label_new("R:");
   gtk_misc_set_alignment(GTK_MISC(cr_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), cr_l, TRUE, FALSE, 2);
   gtk_widget_show(cr_l);
   ok_data->cr = gtk_spin_button_new(GTK_ADJUSTMENT(a1), 1, 0);

   gtk_box_pack_start(GTK_BOX(hbox), ok_data->cr, TRUE, FALSE, 2);
   gtk_widget_show(ok_data->cr);

   cg_l = gtk_label_new("G:");
   gtk_misc_set_alignment(GTK_MISC(cg_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), cg_l, TRUE, FALSE, 2);
   gtk_widget_show(cg_l);
   ok_data->cg = gtk_spin_button_new(GTK_ADJUSTMENT(a2), 1, 0);
   gtk_box_pack_start(GTK_BOX(hbox), ok_data->cg, TRUE, FALSE, 2);
   gtk_widget_show(ok_data->cg);

   cb_l = gtk_label_new("B:");
   gtk_misc_set_alignment(GTK_MISC(cb_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), cb_l, TRUE, FALSE, 2);
   gtk_widget_show(cb_l);
   ok_data->cb = gtk_spin_button_new(GTK_ADJUSTMENT(a3), 1, 0);
   gtk_box_pack_start(GTK_BOX(hbox), ok_data->cb, TRUE, FALSE, 2);
   gtk_widget_show(ok_data->cb);

   ca_l = gtk_label_new("A:");
   gtk_misc_set_alignment(GTK_MISC(ca_l), 1.0, 0.5);
   gtk_box_pack_start(GTK_BOX(hbox), ca_l, TRUE, FALSE, 2);
   gtk_widget_show(ca_l);
   ok_data->ca = gtk_spin_button_new(GTK_ADJUSTMENT(a4), 1, 0);
   gtk_box_pack_start(GTK_BOX(hbox), ok_data->ca, TRUE, FALSE, 2);
   gtk_widget_show(ok_data->ca);

   gtk_widget_show(hbox);
   	
   gtk_widget_show(table);

   gtk_spin_button_set_value(GTK_SPIN_BUTTON(ok_data->cr),
                             GEIST_TEXT(obj)->r);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(ok_data->cg),
                             GEIST_TEXT(obj)->g);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(ok_data->cb),
                             GEIST_TEXT(obj)->b);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(ok_data->ca),
                             GEIST_TEXT(obj)->a);

   gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(ok_data->font)->entry),
                      GEIST_TEXT(obj)->fontname);
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(ok_data->size),
                             GEIST_TEXT(obj)->fontsize);
   gtk_entry_set_text(GTK_ENTRY(ok_data->text), GEIST_TEXT(obj)->text);


   gtk_signal_connect(GTK_OBJECT(ok_data->ca), "changed",
                      GTK_SIGNAL_FUNC(refresh_a_cb), (gpointer) obj);
   gtk_signal_connect(GTK_OBJECT(ok_data->cb), "changed",
                      GTK_SIGNAL_FUNC(refresh_b_cb), (gpointer) obj);
   gtk_signal_connect(GTK_OBJECT(ok_data->cg), "changed",
                      GTK_SIGNAL_FUNC(refresh_g_cb), (gpointer) obj);
   gtk_signal_connect(GTK_OBJECT(ok_data->cr), "changed",
                      GTK_SIGNAL_FUNC(refresh_r_cb), (gpointer) obj);
   gtk_signal_connect(GTK_OBJECT(ok_data->size), "changed",
                      GTK_SIGNAL_FUNC(refresh_size_cb), (gpointer) obj);

   gtk_signal_connect(GTK_OBJECT(GTK_COMBO(ok_data->font)->entry), "changed",
                      GTK_SIGNAL_FUNC(refresh_font_cb), (gpointer) obj);
   gtk_signal_connect(GTK_OBJECT(ok_data->text), "changed",
                      GTK_SIGNAL_FUNC(refresh_text_cb), (gpointer) obj);
   
   
   return (ok_data->win);
}
