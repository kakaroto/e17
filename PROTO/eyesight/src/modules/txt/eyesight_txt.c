/*
 * Eyesight - EFL-based document renderer
 * Copyright (C) 2010 Vincent Torri <vtorri at univ-evry dot fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Evas.h>

#include "Eyesight_Module_Txt.h"
#include "Eyesight.h"
#include "eyesight_private.h"
#include "eyesight_txt.h"


#define DBG(...) EINA_LOG_DOM_DBG(_eyesight_txt_log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_eyesight_txt_log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_eyesight_txt_log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_eyesight_txt_log_domain, __VA_ARGS__)
#define CRIT(...) EINA_LOG_DOM_CRIT(_eyesight_txt_log_domain, __VA_ARGS__)


static int _eyesight_txt_log_domain = -1;

/*
 * TODO: consider resizing pages with resizing of the textblock
 * if pages are dynamically resized, this function should be
 * called at each resize, otherwise only once for each document
 */

/*
 * TODO: consider storing pages array instead of ebt->text
 * in the backend struct
 */

static Eina_Bool
_eyesight_txt_page_generate(void *eb){
  Eyesight_Backend_Txt  *ebt;
  Evas_Textblock_Cursor *c1;
  Evas_Textblock_Cursor *c2;
  Evas_Object           *tb;
  int                    w;
  int                    h;
  int                    page = 1;
  char                  *s;

  if (!eb)
    return EINA_FALSE;

  ebt = (Eyesight_Backend_Txt *)eb;

  tb = ebt->obj;
  evas_object_textblock_clear(tb);
  w = 600;
  h = 700;
  evas_object_textblock_text_markup_set(tb, ebt->text);
  evas_object_resize(tb, w, h);
  evas_object_show(tb);

  ebt->pages = eina_array_new(10);
  if (!(ebt->pages))
    return EINA_FALSE;

  c1 = evas_object_textblock_cursor_new(tb);
  c2 = evas_object_textblock_cursor_new(tb);

  evas_textblock_cursor_paragraph_first(c1);
  evas_textblock_cursor_line_coord_set(c2, h - 1);

  do{
    evas_textblock_cursor_line_char_last(c2);
    s = evas_textblock_cursor_range_text_get(c1, c2, EVAS_TEXTBLOCK_TEXT_MARKUP);
    eina_array_push(ebt->pages, s);

    page++;
    evas_textblock_cursor_copy(c2, c1);
    evas_textblock_cursor_char_next(c1);
    evas_textblock_cursor_line_coord_set(c2, page * h - 1);
  }while(evas_textblock_cursor_compare(c1, c2) < 0);

  evas_textblock_cursor_paragraph_last(c2);
  if (evas_textblock_cursor_compare(c1, c2) < 0)
  {
    s = evas_textblock_cursor_range_text_get(c1, c2, EVAS_TEXTBLOCK_TEXT_MARKUP);
    eina_array_push(ebt->pages, s);
  }

  evas_textblock_cursor_free(c2);
  evas_textblock_cursor_free(c1);

  return EINA_TRUE;
}

static Eina_Bool
em_init (Evas *evas, Evas_Object **obj, void **eyesight_backend)
{
  Eyesight_Backend_Txt *ebt;
  Evas_Textblock_Style  *st;

  if (!eyesight_backend)
    return EINA_FALSE;

  ebt = calloc(1, sizeof(Eyesight_Backend_Txt));
  if (!ebt)
    return EINA_FALSE;

  ebt->obj = evas_object_textblock_add(evas);
  if (!ebt->obj)
    goto free_ebt;

  st = evas_textblock_style_new();
  evas_textblock_style_set(st, "DEFAULT='font=Sans font_size=12 color=#ffffffff wrap=word left_margin=+12 right_margin=+12'br='\n'");
  evas_object_textblock_style_set(ebt->obj, st);
  evas_textblock_style_free(st);
  evas_object_textblock_clear(ebt->obj);


  *obj = ebt->obj;
  *eyesight_backend = ebt;

  return EINA_TRUE;

 free_ebt:
  free(ebt);

  return EINA_FALSE;
}

static void
em_shutdown(void *eb)
{
  Eyesight_Backend_Txt *ebt;

  if (!eb)
    return;

  ebt = (Eyesight_Backend_Txt *)eb;
  evas_object_del(ebt->obj);
  free(ebt);
}

static void *
em_file_open (void *eb, const char *filename)
{
  char line[80];
  Eyesight_Backend_Txt *ebt;
  FILE *f;
  Eina_Strbuf *sb;
  Eyesight_Document_Txt *doc;

  if (!eb || !filename || !*filename)
    return NULL;

  DBG("Open file %s", filename);

  ebt = (Eyesight_Backend_Txt *)eb;
  ebt->filename = strdup(filename);
  if (!ebt->filename)
    return NULL;

  f = fopen(ebt->filename, "rb");
  if (!f)
    {
      DBG("Could not open file %s", ebt->filename);
      goto free_filename;
    }

  sb = eina_strbuf_new();
  if (!sb)
    goto close_f;

  while (fgets(line, 80, f) != NULL)
    {
      eina_strbuf_append(sb, line);
    }

  eina_strbuf_replace_all(sb, "&", "&amp;");
  eina_strbuf_replace_all(sb, "<", "&lt;");
  eina_strbuf_replace_all(sb, ">", "&gt;");
  eina_strbuf_replace_all(sb, "\n", "<br>");

  /* TODO: use eina_strbuf*() to avoid the duplication */
  ebt->text = strdup(eina_strbuf_string_get(sb));
  eina_strbuf_free(sb);

  if (!ebt->text)
    goto close_f;

  doc = (Eyesight_Document_Txt *)malloc(sizeof(Eyesight_Document_Txt));
  if (!doc)
    goto free_text;

  doc->filename = filename;
  ebt->document = doc;

  ebt->page.hscale = 1.0;
  ebt->page.vscale = 1.0;
  ebt->page.orientation = EYESIGHT_ORIENTATION_PORTRAIT;

  _eyesight_txt_page_generate(eb);

  return doc;

 free_text:
  free(ebt->text);
 close_f:
  fclose(f);
 free_filename:
  free(ebt->filename);
  ebt->filename = NULL;

  return NULL;
}

static void
em_file_close (void *eb)
{
  Eyesight_Backend_Txt *ebt;
  Eina_Array_Iterator   iterator;
  char                 *page;
  unsigned int          counter;

  if (!eb)
    return;

  ebt = (Eyesight_Backend_Txt *)eb;

  DBG("Close file %s", ebt->filename);

  EINA_ARRAY_ITER_NEXT(ebt->pages, counter, page, iterator)
    {
      free(page);
    }
  eina_array_free(ebt->pages);

  if (ebt->document)
    {
      free(ebt->document);
      ebt->document = NULL;
    }
  free(ebt->text);
  if (ebt->filename)
    {
      free(ebt->filename);
      ebt->filename = NULL;
    }
}

static const Eina_List *
em_toc_get(void *eb)
{
  return NULL;
}

static int
em_page_count(void *eb)
{
  Eyesight_Backend_Txt *ebt;

  if (!eb)
    return 0;

  ebt = (Eyesight_Backend_Txt *)eb;

  return eina_array_count_get(ebt->pages);
}

static void
em_page_set(void *eb, int page)
{
  Eyesight_Backend_Txt *ebt;
  const char                 *s;

  if (!eb)
    return;

  ebt = (Eyesight_Backend_Txt *)eb;

  ebt->current_page = page;
  s = eina_array_data_get(ebt->pages, page);
  evas_object_textblock_text_markup_set(ebt->obj, s);
}

static int
em_page_get(void *eb)
{
  Eyesight_Backend_Txt *ebt;

  if (!eb)
    return 0;

  return ebt->current_page;
}

static void
em_page_scale_set(void *eb, double hscale, double vscale)
{
  Eyesight_Backend_Txt *ebt;

  if (!eb)
    return;

  ebt = (Eyesight_Backend_Txt *)eb;

  DBG("horizontal scale=%f vertical scale=%f", hscale, vscale);

  ebt->page.hscale = hscale;
  ebt->page.vscale = vscale;
}

static void
em_page_scale_get(void *eb, double *hscale, double *vscale)
{
  Eyesight_Backend_Txt *ebt;

  if (!eb)
    {
      if (hscale) *hscale = 1.0;
      if (vscale) *vscale = 1.0;
    }

  ebt = (Eyesight_Backend_Txt *)eb;

  if (hscale) *hscale = ebt->page.hscale;
  if (vscale) *vscale = ebt->page.vscale;
}

static void
em_page_orientation_set(void *eb, Eyesight_Orientation orientation)
{
  Eyesight_Backend_Txt *ebt;

  if (!eb)
    return;

  DBG("orientation=%d", orientation);

  ebt = (Eyesight_Backend_Txt *)eb;

  ebt->page.orientation = orientation;
}

static Eyesight_Orientation
em_page_orientation_get(void *eb)
{
  Eyesight_Backend_Txt *ebt;

  if (!eb)
    return EYESIGHT_ORIENTATION_PORTRAIT;

  ebt = (Eyesight_Backend_Txt *)eb;

  return ebt->page.orientation;
}

void
em_page_size_get(void *eb, int *width, int *height)
{
  if (width) *width = 0;
  if (height) *height = 0;
}

static void
em_page_render(void *eb)
{
  Eyesight_Backend_Txt *ebt;

  if (!eb)
    return;

  ebt = (Eyesight_Backend_Txt *)eb;
  printf ("%s\n", ebt->text);
  evas_object_resize(ebt->obj, 600, 700);
/*   evas_object_textblock_text_markup_set(ebt->obj, ebt->text); */
}

char *
em_page_text_get(void *eb, Eina_Rectangle rect)
{
  /* FIXME: get cursors from coords and get the text between cursors with range_text_get() */
  return NULL;
}

Eina_List *
em_page_text_find(void *eb, const char *text, Eina_Bool is_case_sensitive, Eina_Bool backward)
{
  return NULL;
}

Eina_List *
em_page_links_get(void *eb)
{
  return NULL;
}

static Eyesight_Module _eyesight_module_txt =
{
  em_init,
  em_shutdown,
  em_file_open,
  em_file_close,
  em_toc_get,
  em_page_count,
  em_page_set,
  em_page_get,
  em_page_scale_set,
  em_page_scale_get,
  em_page_orientation_set,
  em_page_orientation_get,
  em_page_size_get,
  em_page_render,
  em_page_text_get,
  em_page_text_find,
  em_page_links_get,

  NULL
};

static Eina_Bool
module_open(Evas *evas, Evas_Object **obj, const Eyesight_Module **module, void **backend)
{
   if (!module)
      return EINA_FALSE;

   if (_eyesight_txt_log_domain < 0)
     {
        _eyesight_txt_log_domain = eina_log_domain_register("eyesight-txt", EINA_COLOR_LIGHTCYAN);
        if (_eyesight_txt_log_domain < 0)
          {
             EINA_LOG_CRIT("Could not register log domain 'eyesight-txt'");
             return EINA_FALSE;
          }
     }

   if (!_eyesight_module_txt.init(evas, obj, backend))
     {
        ERR("Could not initialize module");
        eina_log_domain_unregister(_eyesight_txt_log_domain);
        _eyesight_txt_log_domain = -1;
       return EINA_FALSE;
     }

   *module = &_eyesight_module_txt;
   return EINA_TRUE;
}

static void
module_close(Eyesight_Module *module, void *backend)
{
   eina_log_domain_unregister(_eyesight_txt_log_domain);
   _eyesight_txt_log_domain = -1;
   _eyesight_module_txt.shutdown(backend);
}

Eina_Bool
txt_module_init(void)
{
   return _eyesight_module_register("txt", module_open, module_close);
}

void
txt_module_shutdown(void)
{
   _eyesight_module_unregister("txt");
}

#ifndef EYESIGHT_STATIC_BUILD_TXT

EINA_MODULE_INIT(txt_module_init);
EINA_MODULE_SHUTDOWN(txt_module_shutdown);

#endif
