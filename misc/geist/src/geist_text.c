#include "geist.h"
#include "geist_text.h"

typedef struct _addtext_ok_data addtext_ok_data;
struct _addtext_ok_data
{
   GtkWidget *win;
   GtkWidget *text;
   GtkWidget *font;
   GtkWidget *just;
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
static void refresh_just_cb(GtkWidget * widget, gpointer * obj);
static void refresh_wordwrap_cb(GtkWidget * widget, gpointer * data);

char *text_justifications[] = {
   "Left",
   "Right",
   "Center",
   "XXXXX"
};

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
   obj->update_positioning = geist_text_update_positioning;
   geist_object_set_type(obj, GEIST_TYPE_TEXT);
   obj->name = estrdup("New text");
   txt->justification = JUST_LEFT;
   txt->wordwrap = TRUE;

   D_RETURN_(5);
}

geist_object *
geist_text_new_with_text(int x, int y, char *fontname, int fontsize,
                         char *text, int justification,
                         unsigned char wordwrap, int a, int r, int g, int b)
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

   txt->justification = justification;
   txt->wordwrap = wordwrap;

   obj->x = x;
   obj->y = y;
   obj->rendered_x = 0;
   obj->rendered_y = 0;

   geist_text_create_image(txt);
   obj->w = obj->rendered_w;
   obj->h = obj->rendered_h;

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
   if (txt->lines)
      geist_text_free_lines(txt);

   efree(txt);

   D_RETURN_(5);
}

void
geist_text_render(geist_object * obj, Imlib_Image dest)
{
   geist_text *im;

   D_ENTER(5);

   if (!geist_object_get_state(obj, VISIBLE))
      D_RETURN_(5);

   im = GEIST_TEXT(obj);
   if (!im->im)
      D_RETURN_(5);
/*
   dw = geist_imlib_image_get_width(dest);
   dh = geist_imlib_image_get_height(dest);
   sw = geist_imlib_image_get_width(im->im);
   sh = geist_imlib_image_get_height(im->im);

   D(3, ("Rendering text %p with text %s\n", obj, im->text));
   geist_imlib_blend_image_onto_image(dest, im->im, 0, 0, 0, sw, sh,
                                      obj->x + obj->rendered_x,
                                      obj->y + obj->rendered_y, sw, sh, 1, 1,
                                      obj->alias);
*/
   /* just render to the full size of the object */
   geist_text_render_partial(obj, dest, obj->x, obj->y, obj->w, obj->h);

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

   geist_object_get_clipped_render_areas(obj, x, y, w, h, &sx, &sy, &sw, &sh,
                                         &dx, &dy, &dw, &dh);

   D(5, ("Rendering partial text %s\n", im->text));
   D(5,
     ("Rendering text area:\nsx: %d\tsy: %d\nsw: %d\tsh: %d\ndx: %d\tdy: %d\ndw: %d\tdh: %d\n",
      sx, sy, sw, sh, dx, dy, dw, dh));

   geist_imlib_blend_image_onto_image(dest, im->im, 0, sx, sy, sw, sh, dx, dy,
                                      dw, dh, 1, 1, 0);

   D_RETURN_(5);
}

void
geist_text_update_image(geist_text * txt, unsigned char resize)
{
   geist_object *obj;

   D_ENTER(3);

   obj = GEIST_OBJECT(txt);
   geist_object_dirty(obj);
   if (txt->im)
      geist_imlib_free_image_and_decache(txt->im);
   txt->im = NULL;
   geist_text_create_image(txt);
   if (resize)
   {
      obj->w = obj->rendered_w;
      obj->h = obj->rendered_h;
   }
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

   geist_text_update_image(txt, TRUE);

   D_RETURN_(3);
}

void
geist_text_create_image(geist_text * txt)
{
   DATA8 atab[256];
   Imlib_Image im;
   geist_object *obj;
   geist_list *l;
   char *p;
   int w = 0, h = 0, ww, hh;
   int x = 0, y = 0;

   D_ENTER(3);
   obj = GEIST_OBJECT(txt);

   obj->rendered_w = 0;
   obj->rendered_h = 0;

   if (!txt->fn)
   {
      weprintf("no font for text.");
      D_RETURN_(3);
   }
   if (!txt->text)
   {
      weprintf("no text in text object.");
      D_RETURN_(3);
   }
   if (strlen(txt->text) == 0)
   {
      D(2, ("empty text in text object\n"));
      D_RETURN_(3);
   }

   geist_text_free_lines(txt);

   geist_text_calculate_lines(txt);
   if (!txt->lines)
      D_RETURN_(3);

   l = txt->lines;
   while (l)
   {
      p = (char *) l->data;
      geist_imlib_get_text_size(txt->fn, p, &ww, &hh, IMLIB_TEXT_TO_RIGHT);
      if (ww > w)
         w = ww;
      h += hh + TEXT_LINE_SPACING;
      l = l->next;
   }

   obj->rendered_w = w;
   obj->rendered_h = h;

   if (!w)
      D_RETURN_(3);

   im = imlib_create_image(w, h);
   if (!im)
   {
      weprintf("couldn't create imlib image for text area.");
      D_RETURN_(3);
   }

   /* make image transparent (HACK - imlib2 should do this nicely) */
   geist_imlib_image_set_has_alpha(im, 1);
   memset(atab, 0, sizeof(atab));
   geist_imlib_apply_color_modifier_to_rectangle(im, 0, 0, obj->w, obj->h,
                                                 NULL, NULL, NULL, atab);

   /* todo - put offset, margin etc into txt struct */
   l = txt->lines;
   while (l)
   {
      p = (char *) l->data;
      geist_imlib_get_text_size(txt->fn, p, &ww, &hh, IMLIB_TEXT_TO_RIGHT);
      switch (txt->justification)
      {
        case JUST_LEFT:
           x = 0;
           break;
        case JUST_CENTER:
           x = (w - ww) / 2;
           break;
        case JUST_RIGHT:
           x = w - ww;
           break;
        default:
           break;
      }
      geist_imlib_text_draw(im, txt->fn, x, y, p, IMLIB_TEXT_TO_RIGHT, txt->r,
                            txt->g, txt->b, txt->a);
      y += hh + TEXT_LINE_SPACING;
      l = l->next;
   }
   txt->im = im;

   D_RETURN_(3);
}

void
geist_text_free_lines(geist_text * txt)
{
   D_ENTER(3);

   if (!txt->lines)
      return;

   geist_list_free_and_data(txt->lines);
   txt->lines = NULL;

   D_RETURN_(3);
}

void
geist_text_calculate_lines(geist_text * txt)
{
   geist_object *obj;
   geist_list *ll, *lines = NULL, *list = NULL, *words;
   geist_list *l = NULL;
   char delim[2] = { '\n', '\0' };
   int w, line_width;
   int tw, th;
   char *p, *pp;
   char *line = NULL;
   char *temp;
   int space_width = 0, m_width = 0, t_width = 0, new_width = 0;

   D_ENTER(3);

   obj = GEIST_OBJECT(txt);

   if (!txt->text)
      D_RETURN_(3);

   lines = geist_string_split(txt->text, delim);

   if (txt->wordwrap && obj->w)
   {
      geist_imlib_get_text_size(txt->fn, "M M", &t_width, NULL,
                                IMLIB_TEXT_TO_RIGHT);
      geist_imlib_get_text_size(txt->fn, "M", &m_width, NULL,
                                IMLIB_TEXT_TO_RIGHT);
      space_width = t_width - (2 * m_width);
      w = obj->w;
      l = lines;
      while (l)
      {
         line_width = 0;
         p = (char *) l->data;
         D(1, ("got line %s\n", p));
         /* quick check to see if whole line fits okay */
         geist_imlib_get_text_size(txt->fn, p, &tw, &th, IMLIB_TEXT_TO_RIGHT);
         if (tw <= w)
            list = geist_list_add_end(list, estrdup(p));
         else if (strlen(p) == 0)
            list = geist_list_add_end(list, estrdup(""));
         else if (!strcmp(p, " "))
            list = geist_list_add_end(list, estrdup(" "));
         else
         {
            words = geist_string_split(p, " ");
            if (words)
            {
               ll = words;
               while (ll)
               {
                  pp = (char *) ll->data;
                  if (strcmp(pp, " "))
                  {
                     D(1, ("got word %s\n", pp));
                     geist_imlib_get_text_size(txt->fn, pp, &tw, &th,
                                               IMLIB_TEXT_TO_RIGHT);
                     if (line_width == 0)
                        new_width = tw;
                     else
                        new_width = line_width + space_width + tw;
                     if (new_width <= w)
                     {
                        /* add word to line */
                        if (line)
                        {
                           temp = g_strdup_printf("%s %s", line, pp);
                           efree(line);
                           line = temp;
                        }
                        else
                           line = estrdup(pp);
                        line_width = new_width;
                     }
                     else if (line_width == 0)
                     {
                        /* can't fit single word in :/
                           increase width limit to width of word and jam the bastard
                           in anyhow */
                        w = tw;
                        line = estrdup(pp);
                        line_width = new_width;
                     }
                     else
                     {
                        /* finish this line, start next and add word there */
                        if (line)
                        {
                           list = geist_list_add_end(list, estrdup(line));
                           D(5, ("line finished, is: %s\n", line));
                           efree(line);
                           line = NULL;
                        }
                        line = estrdup(pp);
                        line_width = tw;
                     }
                  }
                  ll = ll->next;
               }
               if (line)
               {
                  /* finish last line */
                  list = geist_list_add_end(list, estrdup(line));
                  D(5, ("line finished, is: %s\n", line));
                  efree(line);
                  line = NULL;
                  line_width = 0;
               }
               geist_list_free_and_data(words);
            }
         }
         l = l->next;
      }
      geist_list_free_and_data(lines);
      txt->lines = list;
   }
   else
      txt->lines = lines;

   D_RETURN_(3);
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
      geist_text_new_with_text(obj->x, obj->y, txt->fontname, txt->fontsize,
                               txt->text, JUST_LEFT, TRUE, txt->a, txt->r,
                               txt->g, txt->b);
   ret->rendered_x = obj->rendered_x;
   ret->rendered_y = obj->rendered_y;
   ret->w = obj->w;
   ret->h = obj->h;
   if (ret)
   {
      ret->state = obj->state;
      ret->alias = obj->alias;
      GEIST_TEXT(ret)->justification = txt->justification;
      GEIST_TEXT(ret)->wordwrap = txt->wordwrap;
      ret->name =
         g_strjoin(" ", "Copy of", obj->name ? obj->name : "Untitled object",
                   NULL);
   }
   geist_text_update_image(txt, FALSE);

   D_RETURN(3, ret);
}

void
geist_text_resize(geist_object * obj, int x, int y)
{
   geist_text *txt;

   D_ENTER(5);

   txt = GEIST_TEXT(obj);

   x += obj->clicked_x;
   y += obj->clicked_y;

   D(5, ("resize to %d,%d\n", x, y));
   geist_object_resize_object(obj, x, y);
   geist_text_update_image(txt, FALSE);

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
   geist_text_update_image(txt, TRUE);
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
      geist_text_update_image(txt, TRUE);
      geist_document_render_updates(GEIST_OBJECT_DOC(obj));
   }
}

void
refresh_just_cb(GtkWidget * widget, gpointer * obj)
{
   char *just;
   geist_text *txt;

   txt = GEIST_TEXT(obj);

   just = gtk_entry_get_text(GTK_ENTRY(widget));

   txt->justification = geist_text_get_justification_from_string(just);
   geist_text_update_image(txt, FALSE);
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}


void
refresh_r_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_TEXT(obj)->r =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_text_update_image(GEIST_TEXT(obj), FALSE);
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

void
refresh_g_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_TEXT(obj)->g =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_text_update_image(GEIST_TEXT(obj), FALSE);
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

void
refresh_b_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_TEXT(obj)->b =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_text_update_image(GEIST_TEXT(obj), FALSE);
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

void
refresh_a_cb(GtkWidget * widget, gpointer * obj)
{

   GEIST_TEXT(obj)->a =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
   geist_text_update_image(GEIST_TEXT(obj), FALSE);
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}


void
refresh_text_cb(GtkWidget * widget, gpointer * obj)
{
   geist_text_change_text(GEIST_TEXT(obj),
                          gtk_editable_get_chars(GTK_EDITABLE(widget), 0,
                                                 -1));
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));
}

GtkWidget *
geist_text_display_props(geist_object * obj)
{
   addtext_ok_data *ok_data = NULL;
   GtkWidget *table, *text_l, *font_l, *size_l, *hbox, *cr_l, *cg_l, *cb_l,
      *ca_l, *just_l, *wordwrap_t;
   int i, num;
   char **fonts;
   GList *list = g_list_alloc();
   GList *list2 = g_list_alloc();
   GtkAdjustment *a1, *a2, *a3, *a4, *a5;

   a1 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
   a2 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
   a3 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
   a4 = (GtkAdjustment *) gtk_adjustment_new(0, 0, 255, 1, 2, 3);
   a5 = (GtkAdjustment *) gtk_adjustment_new(12, 2, 128, 1, 2, 3);

   ok_data = emalloc(sizeof(addtext_ok_data));

   ok_data->win = gtk_hbox_new(FALSE, 0);

   if (!ok_data)
      printf("ARGH");
   fonts = geist_imlib_list_fonts(&num);

   if (!fonts)
      printf("ARGH!\n");

   table = gtk_table_new(6, 2, FALSE);
   gtk_container_set_border_width(GTK_CONTAINER(ok_data->win), 5);
   gtk_container_add(GTK_CONTAINER(ok_data->win), table);

   text_l = gtk_label_new("Text:");
   gtk_misc_set_alignment(GTK_MISC(text_l), 1.0, 0.5);
   gtk_table_attach(GTK_TABLE(table), text_l, 0, 1, 1, 2,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(text_l);

   ok_data->text = gtk_text_new(NULL, NULL);
   gtk_text_set_editable(GTK_TEXT(ok_data->text), TRUE);
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
      list2 = g_list_append(list2, (gpointer) fonts[i]);
   gtk_combo_set_popdown_strings(GTK_COMBO(ok_data->font), list2);
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


   just_l = gtk_label_new("Justification:");
   gtk_misc_set_alignment(GTK_MISC(just_l), 1.0, 0.5);
   gtk_table_attach(GTK_TABLE(table), just_l, 0, 1, 4, 5,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(just_l);

   ok_data->just = gtk_combo_new();
   gtk_table_attach(GTK_TABLE(table), ok_data->just, 1, 2, 4, 5,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   for (i = 0; i < JUST_MAX; i++)
      list =
         g_list_append(list,
                       (gpointer) geist_text_get_justification_string(i));
   gtk_combo_set_popdown_strings(GTK_COMBO(ok_data->just), list);
   gtk_widget_show(ok_data->just);

   wordwrap_t = gtk_check_button_new_with_label("Wordwrap");
   gtk_table_attach(GTK_TABLE(table), wordwrap_t, 0, 2, 5, 6,
                    GTK_FILL | GTK_EXPAND, 0, 2, 2);
   gtk_widget_show(wordwrap_t);

   gtk_widget_show(table);


   hbox = gtk_hbox_new(FALSE, 0);

   gtk_table_attach(GTK_TABLE(table), hbox, 0, 2, 6, 7, GTK_FILL | GTK_EXPAND,
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

   gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wordwrap_t),
                                GEIST_TEXT(obj)->wordwrap);

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
   gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(ok_data->just)->entry),
                      geist_text_get_justification_string(GEIST_TEXT(obj)->
                                                          justification));
   gtk_spin_button_set_value(GTK_SPIN_BUTTON(ok_data->size),
                             GEIST_TEXT(obj)->fontsize);
   gtk_text_forward_delete(GTK_TEXT(ok_data->text), -1);
   gtk_text_insert(GTK_TEXT(ok_data->text), NULL,
                   &ok_data->text->style->black, NULL, GEIST_TEXT(obj)->text,
                   -1);



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
   gtk_signal_connect(GTK_OBJECT(wordwrap_t), "clicked",
                      GTK_SIGNAL_FUNC(refresh_wordwrap_cb), (gpointer) obj);


   gtk_signal_connect(GTK_OBJECT(GTK_COMBO(ok_data->font)->entry), "changed",
                      GTK_SIGNAL_FUNC(refresh_font_cb), (gpointer) obj);
   gtk_signal_connect(GTK_OBJECT(GTK_COMBO(ok_data->just)->entry), "changed",
                      GTK_SIGNAL_FUNC(refresh_just_cb), (gpointer) obj);
   gtk_signal_connect(GTK_OBJECT(ok_data->text), "changed",
                      GTK_SIGNAL_FUNC(refresh_text_cb), (gpointer) obj);


   return (ok_data->win);
}

char *
geist_text_get_justification_string(int just)
{
   D_ENTER(3);

   D_RETURN(3, text_justifications[just]);
}

int
geist_text_get_justification_from_string(char *s)
{
   int i;

   D_ENTER(3);

   for (i = 0; i < JUST_MAX; i++)
   {
      if (!strcmp(text_justifications[i], s))
         D_RETURN(3, i);
   }
   D_RETURN(3, 0);
}

static void
refresh_wordwrap_cb(GtkWidget * widget, gpointer * data)
{
   geist_object *obj;

   D_ENTER(3);

   obj = GEIST_OBJECT(data);
   GEIST_TEXT(obj)->wordwrap =
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
   geist_text_update_image(GEIST_TEXT(obj), FALSE);
   geist_document_render_updates(GEIST_OBJECT_DOC(obj));

   D_RETURN_(3);
}

void geist_text_update_positioning(geist_object *obj)
{
   D_ENTER(3);

   geist_object_update_sizemode(obj);
   geist_object_update_alignment(obj);
   geist_text_update_image(GEIST_TEXT(obj), FALSE);

   D_RETURN_(3);
}
