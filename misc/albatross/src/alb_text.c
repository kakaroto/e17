/* alb_text.c

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
#include "alb_text.h"


int fontstyler_active = 0;

/* temp copy of the style thats edited*/
gib_style *working_copy, *temp;
alb_object *current_obj;
int cur = 0;

/*fontstyler cb*/
static void select_row_cb(GtkWidget * clist,
                          gint rw,
                          gint col,
                          GdkEventButton * event,
                          gpointer data);
static void spinner_changed_cb(gpointer data);
static void save_style_cb(GtkWidget * widget);
static void load_style_cb(GtkWidget * widget);
static gboolean save_style_ok_cb(GtkWidget * widget,
                                 gpointer * data);
static gboolean load_style_ok_cb(GtkWidget * widget,
                                 gpointer * data);


/* fontstyler widgets */
GtkWidget *window;
GtkWidget *e1, *e2, *e3, *e4, *e5, *e6;
GtkWidget *list, *combo, *open_sel, *save_sel;

typedef struct _addtext_ok_data addtext_ok_data;
struct _addtext_ok_data {
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

static void refresh_r_cb(GtkWidget * widget,
                         gpointer * obj);
static void refresh_g_cb(GtkWidget * widget,
                         gpointer * obj);
static void refresh_b_cb(GtkWidget * widget,
                         gpointer * obj);
static void refresh_a_cb(GtkWidget * widget,
                         gpointer * obj);
static void refresh_size_cb(GtkWidget * widget,
                            gpointer * obj);
static void refresh_text_cb(GtkWidget * widget,
                            gpointer * obj);
static void refresh_font_cb(GtkWidget * widget,
                            gpointer * obj);
static void refresh_just_cb(GtkWidget * widget,
                            gpointer * obj);
static void refresh_wordwrap_cb(GtkWidget * widget,
                                gpointer * data);

char *text_justifications[] = {
  "Left",
  "Right",
  "Center",
  "Block",
  "XXXXX"
};

alb_object *
alb_text_new(void)
{
  alb_text *txt;

  D_ENTER(5);

  txt = emalloc(sizeof(alb_text));
  alb_text_init(txt);

  alb_object_set_state(ALB_OBJECT(txt), VISIBLE);

  D_RETURN(5, ALB_OBJECT(txt));
}

void
alb_text_init(alb_text * txt)
{
  alb_object *obj;

  D_ENTER(5);
  memset(txt, 0, sizeof(alb_text));
  obj = ALB_OBJECT(txt);
  alb_object_init(obj);
  obj->free = alb_text_free;
  obj->render = alb_text_render;
  obj->render_partial = alb_text_render_partial;
  obj->get_rendered_image = alb_text_get_rendered_image;
  obj->duplicate = alb_text_duplicate;
  obj->resize_event = alb_text_resize;
  obj->display_props = alb_text_display_props;
  obj->sizemode = SIZEMODE_NONE;
  obj->alignment = ALIGN_CENTER;
  obj->update_positioning = alb_text_update_positioning;
  alb_object_set_type(obj, ALB_TYPE_TEXT);
  obj->name = estrdup("New text");
  txt->justification = JUST_LEFT;
  txt->wordwrap = TRUE;

  D_RETURN_(5);
}

alb_object *
alb_text_new_with_text(int x,
                       int y,
                       char *fontname,
                       int fontsize,
                       char *text,
                       int justification,
                       unsigned char wordwrap,
                       int a,
                       int r,
                       int g,
                       int b)
{
  alb_text *txt;
  alb_object *obj;

  D_ENTER(5);

  obj = alb_text_new();
  txt = ALB_TEXT(obj);

  txt->text = estrdup(text);
  if (alb_text_change_font(txt, fontname, fontsize)) {
    alb_text_free(obj);
    D_RETURN(5, NULL);
  }

  txt->r = r;
  txt->g = g;
  txt->b = b;
  txt->a = a;
  txt->style = gib_style_new("Default");
  txt->style->bits =
    gib_list_add_end(txt->style->bits, gib_style_bit_new(0, 0, 0, 0, 0, 0));

  txt->justification = justification;
  txt->wordwrap = wordwrap;

  obj->x = x;
  obj->y = y;
  obj->rendered_x = 0;
  obj->rendered_y = 0;

  alb_text_create_image(txt);
  obj->w = obj->rendered_w;
  obj->h = obj->rendered_h;

  D_RETURN(5, ALB_OBJECT(txt));
}

void
alb_text_free(alb_object * obj)
{
  alb_text *txt;

  D_ENTER(5);

  txt = (alb_text *) obj;

  if (!txt)
    D_RETURN_(5);

  if (txt->fontname)
    efree(txt->fontname);
  if (txt->text)
    efree(txt->text);
  if (txt->fn)
    gib_imlib_free_font(txt->fn);
  if (txt->im)
    gib_imlib_free_image(txt->im);
  if (txt->lines)
    alb_text_free_lines(txt);
  if (txt->style)
    gib_style_free(txt->style);

  efree(txt);

  D_RETURN_(5);
}

void
alb_text_render(alb_object * obj,
                Imlib_Image dest)
{
  alb_text *im;

  D_ENTER(5);

  if (!alb_object_get_state(obj, VISIBLE))
    D_RETURN_(5);

  im = ALB_TEXT(obj);
  if (!im->im)
    D_RETURN_(5);
/*
   dw = gib_imlib_image_get_width(dest);
   dh = gib_imlib_image_get_height(dest);
   sw = gib_imlib_image_get_width(im->im);
   sh = gib_imlib_image_get_height(im->im);

   D(3, ("Rendering text %p with text %s\n", obj, im->text));
   gib_imlib_blend_image_onto_image(dest, im->im, 0, 0, 0, sw, sh,
                                      obj->x + obj->rendered_x,
                                      obj->y + obj->rendered_y, sw, sh, 1, 1,
                                      obj->alias);
*/
  /* just render to the full size of the object */
  alb_text_render_partial(obj, dest, obj->x, obj->y, obj->w, obj->h);

  D_RETURN_(5);
}

void
alb_text_render_partial(alb_object * obj,
                        Imlib_Image dest,
                        int x,
                        int y,
                        int w,
                        int h)
{
  alb_text *im;
  int sw, sh, dw, dh, sx, sy, dx, dy;

  D_ENTER(5);

  if (!alb_object_get_state(obj, VISIBLE))
    D_RETURN_(5);

  im = ALB_TEXT(obj);
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
  D(5, ("Rendering partial text %s\n", im->text));
  D(5,
    ("Rendering text area:\nsx: %d\tsy: %d\nsw: %d\tsh: %d\ndx: %d\tdy: %d\ndw: %d\tdh: %d\n",
     sx, sy, sw, sh, dx, dy, dw, dh));

  gib_imlib_blend_image_onto_image(dest, im->im, 0, sx, sy, sw, sh, dx, dy,
                                   dw, dh, 1, 1, 0);

  D_RETURN_(5);
}

void
alb_text_update_image(alb_text * txt,
                      unsigned char resize)
{
  alb_object *obj;

  D_ENTER(3);

  obj = ALB_OBJECT(txt);
  alb_object_dirty(obj);
  if (txt->im)
    gib_imlib_free_image_and_decache(txt->im);
  txt->im = NULL;
  alb_text_create_image(txt);
  if (resize) {
    if (obj->w < obj->rendered_w)
      obj->w = obj->rendered_w;
    if (obj->h < obj->rendered_h)
      obj->h = obj->rendered_h;
  }

  obj->rendered_x = 0;
  obj->rendered_y = 0;

  alb_object_dirty(obj);
  D_RETURN_(3);
}


void
alb_text_change_text(alb_text * txt,
                     char *newtext)
{
  D_ENTER(3);

  if (txt->text)
    efree(txt->text);
  txt->text = estrdup(newtext);

  alb_text_update_image(txt, TRUE);

  D_RETURN_(3);
}

void
alb_text_create_image(alb_text * txt)
{
  DATA8 atab[256];
  Imlib_Image im;
  alb_object *obj;
  gib_list *l, *ll, *words;
  char *p, *pp;
  int w = 0, h = 0, ww, hh;
  int x = 0, y = 0;

  D_ENTER(3);
  obj = ALB_OBJECT(txt);

  obj->rendered_w = 0;
  obj->rendered_h = 0;

  if (!txt->fn) {
    weprintf("no font for text.");
    D_RETURN_(3);
  }

  alb_text_free_lines(txt);
  alb_text_calculate_lines(txt);

  if (!txt->lines)
    D_RETURN_(3);

  l = txt->lines;
  while (l) {
    p = (char *) l->data;
    gib_imlib_get_text_size(txt->fn, p, txt->style, &ww, &hh,
                            IMLIB_TEXT_TO_RIGHT);
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
  if (!im) {
    weprintf("couldn't create imlib image for text area.");
    D_RETURN_(3);
  }

  /* make image transparent (HACK - imlib2 should do this nicely) */
  gib_imlib_image_set_has_alpha(im, 1);
  memset(atab, 0, sizeof(atab));
  gib_imlib_apply_color_modifier_to_rectangle(im, 0, 0, obj->w, obj->h, NULL,
                                              NULL, NULL, atab);

  /* todo - put offset, margin etc into txt struct */
  l = txt->lines;
  while (l) {
    p = (char *) l->data;
    gib_imlib_get_text_size(txt->fn, p, txt->style, &ww, &hh,
                            IMLIB_TEXT_TO_RIGHT);
    switch (txt->justification) {
      case JUST_LEFT:
        x = 0;
        gib_imlib_text_draw(im, txt->fn, txt->style, x, y, p,
                            IMLIB_TEXT_TO_RIGHT, txt->r, txt->g, txt->b,
                            txt->a);
        break;
      case JUST_CENTER:
        x = (w - ww) / 2;
        gib_imlib_text_draw(im, txt->fn, txt->style, x, y, p,
                            IMLIB_TEXT_TO_RIGHT, txt->r, txt->g, txt->b,
                            txt->a);
        break;
      case JUST_RIGHT:
        x = w - ww;
        gib_imlib_text_draw(im, txt->fn, txt->style, x, y, p,
                            IMLIB_TEXT_TO_RIGHT, txt->r, txt->g, txt->b,
                            txt->a);

        break;
      case JUST_BLOCK:
        words = alb_string_split(p, " ");
        if (words) {
          int wordcnt, word_spacing, line_w;
          int t_width, m_width, space_width, offset = 0;

          wordcnt = gib_list_length(words);
          gib_imlib_get_text_size(txt->fn, p, txt->style, &line_w, NULL,
                                  IMLIB_TEXT_TO_RIGHT);
          gib_imlib_get_text_size(txt->fn, "M M", txt->style, &t_width, NULL,
                                  IMLIB_TEXT_TO_RIGHT);
          gib_imlib_get_text_size(txt->fn, "M", txt->style, &m_width, NULL,
                                  IMLIB_TEXT_TO_RIGHT);
          space_width = t_width - (2 * m_width);

          if (wordcnt > 1)
            word_spacing = (obj->w - line_w) / (wordcnt - 1);
          else
            word_spacing = (obj->w - line_w);

          ll = words;
          while (ll) {
            pp = (char *) ll->data;

            D(1, ("got word %s\n", pp));
            if (strcmp(pp, " ")) {
              int wordw;

              gib_imlib_text_draw(im, txt->fn, txt->style, x + offset, y, pp,
                                  IMLIB_TEXT_TO_RIGHT, txt->r, txt->g, txt->b,
                                  txt->a);
              gib_imlib_get_text_size(txt->fn, pp, txt->style, &wordw, NULL,
                                      IMLIB_TEXT_TO_RIGHT);
              offset += (wordw + space_width + word_spacing);
            }
            ll = ll->next;
          }
          gib_list_free_and_data(words);
        }
        break;

      default:
        break;
    }
    y += hh + TEXT_LINE_SPACING;
    l = l->next;
  }
  txt->im = im;
  D_RETURN_(3);
}

void
alb_text_free_lines(alb_text * txt)
{
  D_ENTER(3);

  if (!txt->lines)
    return;

  gib_list_free_and_data(txt->lines);
  txt->lines = NULL;

  D_RETURN_(3);
}

void
alb_text_calculate_lines(alb_text * txt)
{
  alb_object *obj;
  gib_list *ll, *lines = NULL, *list = NULL, *words;
  gib_list *l = NULL;
  char delim[2] = { '\n', '\0' };
  int w, line_width;
  int tw, th;
  char *p, *pp;
  char *line = NULL;
  char *temp;
  int space_width = 0, m_width = 0, t_width = 0, new_width = 0;

  D_ENTER(3);

  obj = ALB_OBJECT(txt);

  if (!txt->text)
    D_RETURN_(3);

  lines = alb_string_split(txt->text, delim);

  if (txt->wordwrap && obj->w) {
    gib_imlib_get_text_size(txt->fn, "M M", txt->style, &t_width, NULL,
                            IMLIB_TEXT_TO_RIGHT);
    gib_imlib_get_text_size(txt->fn, "M", txt->style, &m_width, NULL,
                            IMLIB_TEXT_TO_RIGHT);
    space_width = t_width - (2 * m_width);
    w = obj->w;
    l = lines;
    while (l) {
      line_width = 0;
      p = (char *) l->data;
      D(1, ("got line %s\n", p));
      /* quick check to see if whole line fits okay */
      gib_imlib_get_text_size(txt->fn, p, txt->style, &tw, &th,
                              IMLIB_TEXT_TO_RIGHT);
      if (tw <= w)
        list = gib_list_add_end(list, estrdup(p));
      else if (strlen(p) == 0)
        list = gib_list_add_end(list, estrdup(""));
      else if (!strcmp(p, " "))
        list = gib_list_add_end(list, estrdup(" "));
      else {
        words = alb_string_split(p, " ");
        if (words) {
          ll = words;
          while (ll) {
            pp = (char *) ll->data;
            if (strcmp(pp, " ")) {
              D(1, ("got word %s\n", pp));
              gib_imlib_get_text_size(txt->fn, pp, txt->style, &tw, &th,
                                      IMLIB_TEXT_TO_RIGHT);
              if (line_width == 0)
                new_width = tw;
              else
                new_width = line_width + space_width + tw;
              if (new_width <= w) {
                /* add word to line */
                if (line) {
                  temp = g_strdup_printf("%s %s", line, pp);
                  efree(line);
                  line = temp;
                } else
                  line = estrdup(pp);
                line_width = new_width;
              } else if (line_width == 0) {
                /* can't fit single word in :/
                   increase width limit to width of word and jam the bastard
                   in anyhow */
                w = tw;
                line = estrdup(pp);
                line_width = new_width;
              } else {
                /* finish this line, start next and add word there */
                if (line) {
                  list = gib_list_add_end(list, estrdup(line));
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
          if (line) {
            /* finish last line */
            list = gib_list_add_end(list, estrdup(line));
            D(5, ("line finished, is: %s\n", line));
            efree(line);
            line = NULL;
            line_width = 0;
          }
          gib_list_free_and_data(words);
        }
      }
      l = l->next;
    }
    gib_list_free_and_data(lines);
    txt->lines = list;
  } else
    txt->lines = lines;

  D_RETURN_(3);
}

Imlib_Image
alb_text_get_rendered_image(alb_object * obj)
{
  D_ENTER(3);

  D_RETURN(3, ALB_TEXT(obj)->im);
}

alb_object *
alb_text_duplicate(alb_object * obj)
{
  alb_object *ret;
  alb_text *txt;

  D_ENTER(3);

  txt = ALB_TEXT(obj);

  ret =
    alb_text_new_with_text(obj->x, obj->y, txt->fontname, txt->fontsize,
                           txt->text, JUST_LEFT, TRUE, 0, 0, 0, 0);
  if (ret) {
    ret->rendered_x = obj->rendered_x;
    ret->rendered_y = obj->rendered_y;
    ret->w = obj->w;
    ret->h = obj->h;
    ret->state = obj->state;
    ret->alias = obj->alias;
    ALB_TEXT(ret)->justification = txt->justification;
    ALB_TEXT(ret)->wordwrap = txt->wordwrap;
    ALB_TEXT(ret)->style = gib_style_dup(txt->style);
    ALB_TEXT(ret)->r = txt->r;
    ALB_TEXT(ret)->g = txt->g;
    ALB_TEXT(ret)->b = txt->b;
    ALB_TEXT(ret)->a = txt->a;
    ret->name =
      g_strjoin(" ", "Copy of", obj->name ? obj->name : "Untitled object",
                NULL);
    /*  ALB_TEXT(ret)->im = gib_imlib_clone_image(txt->im); */
  }
  alb_text_update_image(txt, FALSE);

  D_RETURN(3, ret);
}

void
alb_text_resize(alb_object * obj,
                int x,
                int y)
{
  alb_text *txt;

  D_ENTER(5);

  txt = ALB_TEXT(obj);

  x += obj->clicked_x;
  y += obj->clicked_y;

  D(5, ("resize to %d,%d\n", x, y));
  alb_object_resize_object(obj, x, y);
  alb_text_update_image(txt, FALSE);

  D_RETURN_(5);
}


void
refresh_size_cb(GtkWidget * widget,
                gpointer * obj)
{
  int fontsize;
  alb_text *txt;

  txt = ALB_TEXT(obj);

  fontsize = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  alb_text_change_font(txt, txt->fontname, fontsize);
  alb_text_update_image(txt, TRUE);
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}

int
alb_text_change_font(alb_text * txt,
                     char *fontname,
                     int fontsize)
{
  Imlib_Font fn;
  char *buf;

  D_ENTER(3);

  buf = g_strdup_printf("%s/%d", fontname, fontsize);
  fn = imlib_load_font(buf);
  efree(buf);
  if (!fn) {
    weprintf("error loading font %s at size %d\n", fontname, fontsize);
    D_RETURN(3, 1);
  }

  /* we do this in case the func is called with txt->fontname as the fontname
   * arg */
  if (txt->fontname != fontname) {
    if (txt->fontname)
      efree(txt->fontname);
    txt->fontname = estrdup(fontname);
  }

  txt->fontsize = fontsize;
  if (txt->fn)
    gib_imlib_free_font(txt->fn);

  txt->fn = fn;

  D_RETURN(3, 0);
}

void
refresh_font_cb(GtkWidget * widget,
                gpointer * obj)
{
  char *fontname;
  alb_text *txt;

  txt = ALB_TEXT(obj);

  fontname = gtk_entry_get_text(GTK_ENTRY(widget));

  if (!alb_text_change_font(txt, fontname, txt->fontsize)) {
    alb_text_update_image(txt, TRUE);
    alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
  }
}

void
refresh_just_cb(GtkWidget * widget,
                gpointer * obj)
{
  char *just;
  alb_text *txt;

  txt = ALB_TEXT(obj);

  just = gtk_entry_get_text(GTK_ENTRY(widget));

  txt->justification = alb_text_get_justification_from_string(just);
  alb_text_update_image(txt, FALSE);
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}


void
refresh_r_cb(GtkWidget * widget,
             gpointer * obj)
{

  ALB_TEXT(obj)->r =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  alb_text_update_image(ALB_TEXT(obj), FALSE);
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}

void
refresh_g_cb(GtkWidget * widget,
             gpointer * obj)
{

  ALB_TEXT(obj)->g =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  alb_text_update_image(ALB_TEXT(obj), FALSE);
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}

void
refresh_b_cb(GtkWidget * widget,
             gpointer * obj)
{

  ALB_TEXT(obj)->b =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  alb_text_update_image(ALB_TEXT(obj), FALSE);
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}

void
refresh_a_cb(GtkWidget * widget,
             gpointer * obj)
{

  ALB_TEXT(obj)->a =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
  alb_text_update_image(ALB_TEXT(obj), FALSE);
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}


void
refresh_text_cb(GtkWidget * widget,
                gpointer * obj)
{
  alb_text_change_text(ALB_TEXT(obj),
                       gtk_editable_get_chars(GTK_EDITABLE(widget), 0, -1));
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);
}

GtkWidget *
alb_text_display_props(alb_object * obj)
{
  addtext_ok_data *ok_data = NULL;
  GtkWidget *table, *text_l, *font_l, *size_l, *hbox, *cr_l, *cg_l, *cb_l,
    *ca_l, *just_l, *wordwrap_t, *style_button;
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
  fonts = gib_imlib_list_fonts(&num);

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
      g_list_append(list, (gpointer) alb_text_get_justification_string(i));
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

  style_button = gtk_button_new_with_label("Edit style");
  gtk_table_attach(GTK_TABLE(table), style_button, 0, 2, 7, 8,
                   GTK_FILL | GTK_EXPAND, 0, 2, 2);
  gtk_widget_show(style_button);


  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wordwrap_t),
                               ALB_TEXT(obj)->wordwrap);

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ok_data->cr), ALB_TEXT(obj)->r);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ok_data->cg), ALB_TEXT(obj)->g);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ok_data->cb), ALB_TEXT(obj)->b);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ok_data->ca), ALB_TEXT(obj)->a);

  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(ok_data->font)->entry),
                     ALB_TEXT(obj)->fontname);
  gtk_entry_set_text(GTK_ENTRY(GTK_COMBO(ok_data->just)->entry),
                     alb_text_get_justification_string(ALB_TEXT(obj)->
                                                       justification));
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ok_data->size),
                            ALB_TEXT(obj)->fontsize);
  gtk_text_forward_delete(GTK_TEXT(ok_data->text), -1);
  gtk_text_insert(GTK_TEXT(ok_data->text), NULL, &ok_data->text->style->black,
                  NULL, ALB_TEXT(obj)->text, -1);



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


  gtk_signal_connect(GTK_OBJECT(style_button), "clicked",
                     GTK_SIGNAL_FUNC(alb_display_fontstyler_window),
                     (gpointer) obj);


  return (ok_data->win);
}

char *
alb_text_get_justification_string(int just)
{
  D_ENTER(3);

  D_RETURN(3, text_justifications[just]);
}

int
alb_text_get_justification_from_string(char *s)
{
  int i;

  D_ENTER(3);

  for (i = 0; i < JUST_MAX; i++) {
    if (!strcmp(text_justifications[i], s))
      D_RETURN(3, i);
  }
  D_RETURN(3, 0);
}

static void
refresh_wordwrap_cb(GtkWidget * widget,
                    gpointer * data)
{
  alb_object *obj;

  D_ENTER(3);

  obj = ALB_OBJECT(data);
  ALB_TEXT(obj)->wordwrap =
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
  alb_text_update_image(ALB_TEXT(obj), FALSE);
  alb_document_render_updates(ALB_OBJECT_DOC(obj), 1);

  D_RETURN_(3);
}

void
alb_text_update_positioning(alb_object * obj)
{
  D_ENTER(3);

  alb_object_update_sizemode(obj);
  alb_object_update_alignment(obj);
  alb_text_update_image(ALB_TEXT(obj), FALSE);

  D_RETURN_(3);
}


/*font styler dialog. should be moved to its own file I guess. Till*/

void
render_style(gib_style * style)
{
  /*we render the text object with the current style, but reset the style
   * immidiately afterwards, so the object doesnt really change until OK
   * is clicked */

  D_ENTER(3);

  temp = ALB_TEXT(current_obj)->style;

  ALB_TEXT(current_obj)->style = style;

  alb_object_dirty(current_obj);
  alb_text_update_image(ALB_TEXT(current_obj), FALSE);
  alb_document_render_updates(current_doc, 1);

  ALB_TEXT(current_obj)->style = temp;

  D_RETURN_(3);
}

void
load_style(gib_style * style)
{
  gib_list *bit_list, *l;
  int i = 0;

  D_ENTER(3);

  bit_list = style->bits;

  gtk_clist_freeze(GTK_CLIST(list));
  gtk_clist_clear(GTK_CLIST(list));
  gtk_signal_handler_block_by_func(GTK_OBJECT(list), select_row_cb, NULL);


  for (l = bit_list; l; l = l->next) {
    gib_style_bit *bit = l->data;

    gchar *buf[6];

    buf[0] = g_strdup_printf("%d", bit->x_offset);
    buf[1] = g_strdup_printf("%d", bit->y_offset);
    buf[2] = g_strdup_printf("%d", bit->r);
    buf[3] = g_strdup_printf("%d", bit->g);
    buf[4] = g_strdup_printf("%d", bit->b);
    buf[5] = g_strdup_printf("%d", bit->a);

    gtk_clist_append(GTK_CLIST(list), buf);
    gtk_clist_set_row_data(GTK_CLIST(list), i, bit);
    i++;
  }
  gtk_clist_thaw(GTK_CLIST(list));
  gtk_signal_handler_unblock_by_func(GTK_OBJECT(list), select_row_cb, NULL);
  render_style(style);

  D_RETURN_(3);
}


static void
select_row_cb(GtkWidget * clist,
              gint rw,
              gint col,
              GdkEventButton * event,
              gpointer data)
{
  gchar *buf = NULL;

  D_ENTER(3);

  gtk_signal_handler_block_by_func(GTK_OBJECT(e1), spinner_changed_cb, "1");
  gtk_signal_handler_block_by_func(GTK_OBJECT(e2), spinner_changed_cb, "2");
  gtk_signal_handler_block_by_func(GTK_OBJECT(e3), spinner_changed_cb, "3");
  gtk_signal_handler_block_by_func(GTK_OBJECT(e4), spinner_changed_cb, "4");
  gtk_signal_handler_block_by_func(GTK_OBJECT(e5), spinner_changed_cb, "5");
  gtk_signal_handler_block_by_func(GTK_OBJECT(e6), spinner_changed_cb, "6");

  gtk_clist_get_text(GTK_CLIST(clist), rw, 0, &buf);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(e1), atof(buf));

  gtk_clist_get_text(GTK_CLIST(clist), rw, 1, &buf);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(e2), atof(buf));

  gtk_clist_get_text(GTK_CLIST(clist), rw, 2, &buf);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(e3), atof(buf));

  gtk_clist_get_text(GTK_CLIST(clist), rw, 3, &buf);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(e4), atof(buf));

  gtk_clist_get_text(GTK_CLIST(clist), rw, 4, &buf);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(e5), atof(buf));

  gtk_clist_get_text(GTK_CLIST(clist), rw, 5, &buf);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(e6), atof(buf));

  gtk_signal_handler_unblock_by_func(GTK_OBJECT(e1), spinner_changed_cb, "1");
  gtk_signal_handler_unblock_by_func(GTK_OBJECT(e2), spinner_changed_cb, "2");
  gtk_signal_handler_unblock_by_func(GTK_OBJECT(e3), spinner_changed_cb, "3");
  gtk_signal_handler_unblock_by_func(GTK_OBJECT(e4), spinner_changed_cb, "4");
  gtk_signal_handler_unblock_by_func(GTK_OBJECT(e5), spinner_changed_cb, "5");
  gtk_signal_handler_unblock_by_func(GTK_OBJECT(e6), spinner_changed_cb, "6");
  cur = rw;

  D_RETURN_(3);
}


static void
spinner_changed_cb(gpointer data)
{
  gib_style_bit *bit;

  D_ENTER(3);

  bit = GIB_STYLE_BIT(gtk_clist_get_row_data(GTK_CLIST(list), cur));

  bit->x_offset = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(e1));
  bit->y_offset = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(e2));

  bit->r = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(e3));
  bit->g = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(e4));
  bit->b = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(e5));
  bit->a = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(e6));

  load_style(working_copy);
  render_style(working_copy);

  D_RETURN_(3);
}


void
ok_cb(GtkWidget * widget,
      gpointer * data)
{
  D_ENTER(3);

  gib_style_free(ALB_TEXT(current_obj)->style);
  ALB_TEXT(current_obj)->style = working_copy;

  alb_object_dirty(current_obj);
  alb_text_update_image(ALB_TEXT(current_obj), FALSE);
  alb_document_render_updates(current_doc, 1);

  gtk_widget_destroy(window);

  D_RETURN_(3);
}


void
cancel_cb(GtkWidget * widget,
          gpointer * data)
{

  D_ENTER(3);

  gib_style_free(working_copy);

  alb_object_dirty(current_obj);
  alb_text_update_image(ALB_TEXT(current_obj), FALSE);
  alb_document_render_updates(current_doc, 1);

  gtk_widget_destroy(window);

  D_RETURN_(3);
}

void
new_cb(GtkWidget * widget,
       gpointer * data)
{

  gib_list *bit_list;
  gib_style_bit *new_bit;

  D_ENTER(3);
  new_bit = gib_style_bit_new(0, 0, 255, 255, 255, 255);
  bit_list = working_copy->bits;

  working_copy->bits = gib_list_add_at_pos(bit_list, cur + 1, new_bit);

  load_style(working_copy);
  gtk_clist_select_row(GTK_CLIST(list), cur + 1, 0);

  D_RETURN_(3);
}

void
delete_cb(GtkWidget * widget,
          gpointer * data)
{

  gib_list *bit_list, *temp;

  D_ENTER(3);
  bit_list = working_copy->bits;

  temp = gib_list_nth(bit_list, cur);
  working_copy->bits = gib_list_unlink(bit_list, temp);
  gib_style_bit_free(GIB_STYLE_BIT(temp));

  load_style(working_copy);
  if (cur != gib_list_length(working_copy->bits))
    gtk_clist_select_row(GTK_CLIST(list), cur, 0);
  else
    gtk_clist_select_row(GTK_CLIST(list), cur - 1, 0);

  D_RETURN_(3);
}

void
raise_cb(GtkWidget * widget,
         gpointer * data)
{
  gib_list *bit_list;

  D_ENTER(3);

  bit_list = working_copy->bits;
  working_copy->bits =
    gib_list_move_down_by_one(bit_list, gib_list_nth(bit_list, cur));
  load_style(working_copy);
  if (cur + 1 != gib_list_length(working_copy->bits))
    gtk_clist_select_row(GTK_CLIST(list), cur + 1, 0);
  else
    gtk_clist_select_row(GTK_CLIST(list), cur, 0);


  D_RETURN_(3);
}

void
lower_cb(GtkWidget * widget,
         gpointer * data)
{
  gib_list *bit_list;

  D_ENTER(3);

  bit_list = working_copy->bits;
  working_copy->bits =
    gib_list_move_up_by_one(bit_list, gib_list_nth(bit_list, cur));
  load_style(working_copy);
  gtk_clist_select_row(GTK_CLIST(list), cur - 1, 0);

  D_RETURN_(3);
}



void
alb_display_fontstyler_window(GtkWidget * widget,
                              gpointer * obj)
{
  GtkWidget *b, *box1, *box2, *box3, *scroll;
  GtkWidget *label, *frame;
  GtkWidget *h1, *h2, *h3, *h4, *h6, *h7, *h8;
  GtkWidget *box4, *box5, *box6;
  GtkWidget *raise_btn, *lower_btn, *delete_btn, *new_btn, *cancel_btn,
    *ok_btn;
  GtkAdjustment *a1, *a2, *a3, *a4, *a5, *a6;
  gchar *listt[6] = { "  X  ", "  Y  ", "  R   ", "  G  ", "  B  ", "  A  " };

  D_ENTER(3);

  if (fontstyler_active) {
    alb_update_fontstyler_window(ALB_OBJECT(obj));
    D_RETURN_(3);
  }

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "GeistFontStyler");
  gtk_container_set_border_width(GTK_CONTAINER(window), 2);

  /* adjustments for spinners */
  a1 = (GtkAdjustment *) gtk_adjustment_new(0, -20, 20, 1, 2, 3);
  a2 = (GtkAdjustment *) gtk_adjustment_new(0, -20, 20, 1, 2, 3);
  a3 = (GtkAdjustment *) gtk_adjustment_new(255, 0, 255, 1, 2, 3);
  a4 = (GtkAdjustment *) gtk_adjustment_new(255, 0, 255, 1, 2, 3);
  a5 = (GtkAdjustment *) gtk_adjustment_new(255, 0, 255, 1, 2, 3);
  a6 = (GtkAdjustment *) gtk_adjustment_new(255, 0, 255, 1, 2, 3);


  box5 = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), box5);
  box1 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box5), box1, FALSE, TRUE, 2);
  box2 = gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box1), box2, FALSE, TRUE, 2);
  box3 = gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box1), box3, FALSE, FALSE, 0);

  b = gtk_button_new_with_label("Load Style");
  gtk_box_pack_start(GTK_BOX(box2), b, FALSE, FALSE, 1);
  gtk_signal_connect(GTK_OBJECT(b), "clicked", GTK_SIGNAL_FUNC(load_style_cb),
                     NULL);
  gtk_widget_show(b);

  b = gtk_button_new_with_label("Save Style");
  gtk_box_pack_start(GTK_BOX(box2), b, FALSE, FALSE, 1);
  gtk_signal_connect(GTK_OBJECT(b), "clicked", GTK_SIGNAL_FUNC(save_style_cb),
                     NULL);
  gtk_widget_show(b);

  frame = gtk_frame_new("Edit Options");
  gtk_container_set_border_width(GTK_CONTAINER(frame), 1);
  gtk_box_pack_start(GTK_BOX(box2), frame, FALSE, FALSE, 1);
  gtk_widget_show(frame);


  h1 = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(h1), 3);
  gtk_container_add(GTK_CONTAINER(frame), h1);
  gtk_widget_show(h1);


  h2 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(h1), h2, FALSE, FALSE, 1);
  gtk_widget_show(h2);
  label = gtk_label_new("X: ");
  gtk_box_pack_start(GTK_BOX(h2), label, FALSE, FALSE, 0);
  gtk_widget_show(label);

  e1 = gtk_spin_button_new(GTK_ADJUSTMENT(a1), 1, 0);
  gtk_box_pack_start(GTK_BOX(h2), e1, TRUE, TRUE, 0);
  gtk_widget_show(e1);


  h3 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(h1), h3, FALSE, FALSE, 1);
  gtk_widget_show(h3);

  label = gtk_label_new("Y: ");
  gtk_box_pack_start(GTK_BOX(h3), label, FALSE, FALSE, 0);
  gtk_widget_show(label);

  e2 = gtk_spin_button_new(GTK_ADJUSTMENT(a2), 1, 0);
  gtk_box_pack_start(GTK_BOX(h3), e2, TRUE, TRUE, 0);
  gtk_widget_show(e2);


  h4 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(h1), h4, FALSE, FALSE, 1);
  gtk_widget_show(h4);

  label = gtk_label_new("R: ");
  gtk_box_pack_start(GTK_BOX(h4), label, FALSE, FALSE, 0);
  gtk_widget_show(label);

  e3 = gtk_spin_button_new(GTK_ADJUSTMENT(a3), 1, 0);
  gtk_box_pack_start(GTK_BOX(h4), e3, TRUE, TRUE, 0);
  gtk_widget_show(e3);


  h6 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(h1), h6, FALSE, FALSE, 1);
  gtk_widget_show(h6);

  label = gtk_label_new("G: ");
  gtk_box_pack_start(GTK_BOX(h6), label, FALSE, FALSE, 0);
  gtk_widget_show(label);

  e4 = gtk_spin_button_new(GTK_ADJUSTMENT(a4), 1, 0);
  gtk_box_pack_start(GTK_BOX(h6), e4, TRUE, TRUE, 0);
  gtk_widget_show(e4);


  h7 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(h1), h7, FALSE, FALSE, 1);
  gtk_widget_show(h7);

  label = gtk_label_new("B: ");
  gtk_box_pack_start(GTK_BOX(h7), label, FALSE, FALSE, 0);
  gtk_widget_show(label);

  e5 = gtk_spin_button_new(GTK_ADJUSTMENT(a5), 1, 0);
  gtk_box_pack_start(GTK_BOX(h7), e5, TRUE, TRUE, 0);
  gtk_widget_show(e5);


  h8 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(h1), h8, FALSE, FALSE, 1);
  gtk_widget_show(h8);

  label = gtk_label_new("A: ");
  gtk_box_pack_start(GTK_BOX(h8), label, FALSE, FALSE, 0);
  gtk_widget_show(label);

  e6 = gtk_spin_button_new(GTK_ADJUSTMENT(a6), 1, 0);
  gtk_box_pack_start(GTK_BOX(h8), e6, TRUE, TRUE, 0);
  gtk_widget_show(e6);

  scroll = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
                                 GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  gtk_box_pack_start(GTK_BOX(box3), scroll, TRUE, TRUE, 1);
  gtk_widget_show(scroll);

  list = gtk_clist_new_with_titles(6, listt);
  gtk_clist_set_selection_mode(GTK_CLIST(list), GTK_SELECTION_BROWSE);
  gtk_clist_set_column_justification(GTK_CLIST(list), 0, GTK_JUSTIFY_LEFT);
  gtk_container_add(GTK_CONTAINER(scroll), list);
  gtk_widget_show(list);

  box4 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box3), box4, FALSE, FALSE, 1);
  gtk_widget_show(box4);

  new_btn = gtk_button_new_with_label("New");
  gtk_box_pack_start(GTK_BOX(box4), new_btn, TRUE, TRUE, 1);
  gtk_widget_show(new_btn);

  raise_btn = gtk_button_new_with_label("Raise");
  gtk_box_pack_start(GTK_BOX(box4), raise_btn, TRUE, TRUE, 1);
  gtk_widget_show(raise_btn);

  lower_btn = gtk_button_new_with_label("Lower");
  gtk_box_pack_start(GTK_BOX(box4), lower_btn, TRUE, TRUE, 1);
  gtk_widget_show(lower_btn);

  delete_btn = gtk_button_new_with_label("Delete");
  gtk_box_pack_start(GTK_BOX(box4), delete_btn, TRUE, TRUE, 1);
  gtk_widget_show(delete_btn);


  box6 = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box5), box6, TRUE, TRUE, 1);


  ok_btn = gtk_button_new_with_label("Ok");
  gtk_box_pack_start(GTK_BOX(box6), ok_btn, TRUE, TRUE, 1);
  gtk_widget_show(ok_btn);

  cancel_btn = gtk_button_new_with_label("Cancel");
  gtk_box_pack_start(GTK_BOX(box6), cancel_btn, TRUE, TRUE, 1);
  gtk_widget_show(cancel_btn);

  gtk_widget_show(box1);
  gtk_widget_show(box2);
  gtk_widget_show(box3);
  gtk_widget_show(box5);
  gtk_widget_show(box6);



  gtk_widget_show(window);

  /*connect to callbacks */
  gtk_signal_connect(GTK_OBJECT(window), "delete_event",
                     GTK_SIGNAL_FUNC(alb_hide_fontstyler_window), NULL);
  gtk_signal_connect(GTK_OBJECT(list), "select_row",
                     GTK_SIGNAL_FUNC(select_row_cb), NULL);

  gtk_signal_connect(GTK_OBJECT(e1), "changed",
                     GTK_SIGNAL_FUNC(spinner_changed_cb), "1");

  gtk_signal_connect(GTK_OBJECT(e2), "changed",
                     GTK_SIGNAL_FUNC(spinner_changed_cb), "2");
  gtk_signal_connect(GTK_OBJECT(e3), "changed",
                     GTK_SIGNAL_FUNC(spinner_changed_cb), "3");
  gtk_signal_connect(GTK_OBJECT(e4), "changed",
                     GTK_SIGNAL_FUNC(spinner_changed_cb), "4");
  gtk_signal_connect(GTK_OBJECT(e5), "changed",
                     GTK_SIGNAL_FUNC(spinner_changed_cb), "5");
  gtk_signal_connect(GTK_OBJECT(e6), "changed",
                     GTK_SIGNAL_FUNC(spinner_changed_cb), "6");

  gtk_signal_connect(GTK_OBJECT(ok_btn), "clicked", GTK_SIGNAL_FUNC(ok_cb),
                     NULL);

  gtk_signal_connect(GTK_OBJECT(cancel_btn), "clicked",
                     GTK_SIGNAL_FUNC(cancel_cb), NULL);

  gtk_signal_connect(GTK_OBJECT(ok_btn), "clicked", GTK_SIGNAL_FUNC(ok_cb),
                     NULL);

  gtk_signal_connect(GTK_OBJECT(new_btn), "clicked", GTK_SIGNAL_FUNC(new_cb),
                     NULL);
  gtk_signal_connect(GTK_OBJECT(delete_btn), "clicked",
                     GTK_SIGNAL_FUNC(delete_cb), NULL);
  gtk_signal_connect(GTK_OBJECT(raise_btn), "clicked",
                     GTK_SIGNAL_FUNC(raise_cb), NULL);
  gtk_signal_connect(GTK_OBJECT(lower_btn), "clicked",
                     GTK_SIGNAL_FUNC(lower_cb), NULL);

  alb_update_fontstyler_window(ALB_OBJECT(obj));

}

void
alb_hide_fontstyler_window()
{
  D_ENTER(3);

  gtk_widget_destroy(window);

  D_RETURN_(3);
}

void
alb_update_fontstyler_window(alb_object * obj)
{

  D_ENTER(3);

  current_obj = obj;
  working_copy = gib_style_dup(ALB_TEXT(obj)->style);
  load_style(working_copy);
  gtk_clist_select_row(GTK_CLIST(list),
                       gib_list_length(working_copy->bits) - 1, 0);

  D_RETURN_(3);
}

static void
save_style_cb(GtkWidget * widget)
{
  GtkWidget *fs;
  gib_style *style;
  char *name;

  D_ENTER(3);

  style = working_copy;
  fs = gtk_file_selection_new("Save style as...");
  gtk_signal_connect(GTK_OBJECT(fs), "destroy",
                     (GtkSignalFunc) gtk_widget_destroy, GTK_OBJECT(fs));

  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button), "clicked",
                     (GtkSignalFunc) save_style_ok_cb, fs);

  gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(fs)->cancel_button),
                            "clicked", (GtkSignalFunc) gtk_widget_destroy,
                            GTK_OBJECT(fs));

  if (style->name)
    name = g_strdup_printf("%s.style", style->name);
  else
    name = estrdup("current.style");
  gtk_file_selection_set_filename(GTK_FILE_SELECTION(fs), name);
  gtk_object_set_data(GTK_OBJECT(fs), "style", style);
  gtk_widget_show(fs);

  D_RETURN_(3);
}

static gboolean
save_style_ok_cb(GtkWidget * widget,
                 gpointer * data)
{
  char *filename;
  gib_style *style;

  D_ENTER(3);

  filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));
  if (!filename)
    D_RETURN(3, TRUE);

  style = gtk_object_get_data(GTK_OBJECT(data), "style");
  if (style) {
    gib_style_save_ascii(style, filename);
  }
  gtk_widget_destroy(GTK_WIDGET(data));

  D_RETURN(3, TRUE);
}


static void
load_style_cb(GtkWidget * widget)
{
  GtkWidget *fs;

  D_ENTER(3);

  fs = gtk_file_selection_new("Load style...");
  gtk_signal_connect(GTK_OBJECT(fs), "destroy",
                     (GtkSignalFunc) gtk_widget_destroy, GTK_OBJECT(fs));

  gtk_signal_connect(GTK_OBJECT(GTK_FILE_SELECTION(fs)->ok_button), "clicked",
                     (GtkSignalFunc) load_style_ok_cb, fs);

  gtk_signal_connect_object(GTK_OBJECT(GTK_FILE_SELECTION(fs)->cancel_button),
                            "clicked", (GtkSignalFunc) gtk_widget_destroy,
                            GTK_OBJECT(fs));

  gtk_file_selection_set_filename(GTK_FILE_SELECTION(fs), "current.style");
  gtk_widget_show(fs);

  D_RETURN_(3);
}

static gboolean
load_style_ok_cb(GtkWidget * widget,
                 gpointer * data)
{
  char *filename;
  gib_style *style;

  D_ENTER(3);

  filename = gtk_file_selection_get_filename(GTK_FILE_SELECTION(data));
  if (!filename)
    D_RETURN(3, TRUE);

  style = gib_style_new_from_ascii(filename);
  if (!style) {
    weprintf("failed to load style file %s\n", filename);
    gtk_widget_destroy(GTK_WIDGET(data));
    D_RETURN(3, TRUE);
  } else {
    D(2, ("style file %s loaded okay\n", filename));
  }
  working_copy = style;
  load_style(working_copy);
  render_style(working_copy);
  gtk_widget_destroy(GTK_WIDGET(data));

  D_RETURN(3, TRUE);
}
