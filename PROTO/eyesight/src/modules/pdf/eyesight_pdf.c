/*
 * Eyesight - EFL-based document renderer
 * Copyright (C) 2010-2012 Vincent Torri <vtorri at univ-evry dot fr>
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

#include <fitz.h>
#include <mupdf.h>

#include <Evas.h>

#include "Eyesight_Module_Pdf.h"
#include "Eyesight.h"
#include "eyesight_private.h"
#include "eyesight_pdf.h"

#define DBG(...) EINA_LOG_DOM_DBG(_eyesight_pdf_log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_eyesight_pdf_log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_eyesight_pdf_log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_eyesight_pdf_log_domain, __VA_ARGS__)
#define CRIT(...) EINA_LOG_DOM_CRIT(_eyesight_pdf_log_domain, __VA_ARGS__)


static int _eyesight_pdf_count = 0;
static int _eyesight_pdf_log_domain = -1;

static char *
_eyesight_document_property_get(pdf_document *doc, char *prop)
{
  char buf[1024];
  fz_document *d;

  d = (fz_document *)doc;
  *(char **)buf = prop;
  if (fz_meta(d, FZ_META_INFO, buf, 256) <= 0)
    return NULL;
  return strdup(buf);
}

/* FIXME: handle minutes and seconds ? */
static char *
_eyesight_document_date_get(pdf_document *doc, char *prop)
{
  char *date;
  char *fmt;

  date = _eyesight_document_property_get(doc, prop);
  if (!date)
    return NULL;

  fmt = (char *)malloc(21);
  if (fmt)
    {
      memcpy(fmt, date + 2, 4);
      fmt[4] = '-';
      memcpy(fmt + 5, date + 6, 2);
      fmt[7] = '-';
      memcpy(fmt + 8, date + 8, 2);
      fmt[10] = ',';
      fmt[11] = ' ';
      memcpy(fmt + 12, date + 10, 2);
      fmt[14] = ':';
      memcpy(fmt + 15, date + 12, 2);
      fmt[17] = ':';
      memcpy(fmt + 18, date + 14, 2);
      fmt[20] = '\0';
      free(date);
      return fmt;
    }

  return fmt;
}

static Eina_List *
_eyesight_index_fill(pdf_document *doc,
                     fz_outline *outline,
                     Eina_List   *items)
{
  if (!outline)
    return items;

  while (outline)
    {
      Eyesight_Index_Item *item;

      item = eyesight_index_item_new();
      if (!item) goto next_item;

      item ->title = strdup(outline->title);
      /* if title is NULL, we still continue */
      switch (outline->dest.kind)
        {
        case FZ_LINK_GOTO:
          item->action = EYESIGHT_LINK_ACTION_GOTO;
          break;
        case FZ_LINK_URI:
          item->action = EYESIGHT_LINK_ACTION_URI;
          break;
        case FZ_LINK_LAUNCH:
          item->action = EYESIGHT_LINK_ACTION_LAUNCH;
          break;
        case FZ_LINK_NAMED:
          item->action = EYESIGHT_LINK_ACTION_NAMED;
          break;
        case FZ_LINK_GOTOR:
          item->action = EYESIGHT_LINK_ACTION_GOTO_REMOTE_FILE;
          break;
        default:
          /* this case should not be reached */
          item->action = EYESIGHT_LINK_ACTION_UNKNOWN;
          break;
        }

      /* FIXME: page outlines are allowed to have a /A key (action) which can be a URL */
      if (outline->dest.kind == FZ_LINK_GOTO)
        item->page = outline->dest.ld.gotor.page;
      /* FIXME: not implemented in mupdf yet */
      item->is_open = EINA_TRUE;

      if (outline->down)
        item->children = _eyesight_index_fill(doc, outline->down, item->children);

      items = eina_list_append (items, item);

    next_item:
      outline = outline->next;
    }

  return items;
}

static void
_eyesight_index_unfill (Eina_List *items)
{
  if (!items)
    return;

  while (items)
    {
      Eyesight_Index_Item *item;

      item = (Eyesight_Index_Item *)eina_list_data_get(items);

      if (item->title)
        free (item->title);

      if (item->children)
        _eyesight_index_unfill (item->children);

      free (item);

      items = eina_list_remove_list(items, items);
    }
}

static Eyesight_Link_Action_Goto
_eyesight_link_action_goto_fill(fz_link *lnk)
{
  Eyesight_Link_Action_Goto action_goto;

  action_goto.page = lnk->dest.ld.gotor.page;

  /* FIXME: to complete */

  return action_goto;
}

static Eyesight_Link_Action_Goto_Remote_File
_eyesight_link_action_goto_remote_file_fill(fz_link *lnk)
{
  Eyesight_Link_Action_Goto_Remote_File action_goto;

  action_goto.filename = strdup(lnk->dest.ld.gotor.file_spec);

  /* FIXME: to complete */

  return action_goto;
}

static Eyesight_Link_Action_Uri
_eyesight_link_action_uri_fill(fz_link *lnk)
{
  Eyesight_Link_Action_Uri action_uri;

  action_uri.uri = strdup(lnk->dest.ld.uri.uri);
  /* we don't need any check on the success of strdup() */

  return action_uri;
}

static Eyesight_Link_Action_Launch
_eyesight_link_action_launch_fill(fz_link *lnk)
{
  Eyesight_Link_Action_Launch action_launch;

  action_launch.app = strdup(lnk->dest.ld.launch.file_spec);
  /* we don't need any check on the success of strdup() */

  return action_launch;
}

static Eyesight_Link_Action_Named
_eyesight_link_action_named_fill(fz_link *lnk)
{
  Eyesight_Link_Action_Named action_named;

  action_named.name = strdup(lnk->dest.ld.named.named);
  /* we don't need any check on the success of strdup() */

  return action_named;
}

static Eyesight_Link_Action_Unknown
_eyesight_link_action_unknown_fill(fz_link *lnk __UNUSED__)
{
  Eyesight_Link_Action_Unknown action_unknown;

  action_unknown.action = NULL;

  return action_unknown;
}

static Eina_List *
_eyesight_page_links_get(Eyesight_Backend_Pdf *ebp)
{
  Eina_List *links_list = NULL;
  fz_link *lnk;

  if (!ebp->page.page)
    return NULL;

  /* FIXME: mupdf should provide the number of links */
  lnk = pdf_load_links(ebp->doc.doc, ebp->page.page);
  while (lnk)
    {
      Eyesight_Link *link_item;

      link_item = (Eyesight_Link *)calloc(1, sizeof(Eyesight_Link));
      if (!link_item)
        goto next_link;

      switch (lnk->dest.kind)
        {
        case FZ_LINK_GOTO:
          link_item->action = EYESIGHT_LINK_ACTION_GOTO;
          link_item->dest.action_goto = _eyesight_link_action_goto_fill(lnk);
          break;
        case FZ_LINK_URI:
          link_item->action = EYESIGHT_LINK_ACTION_URI;
          link_item->dest.action_uri = _eyesight_link_action_uri_fill(lnk);
          break;
        case FZ_LINK_LAUNCH:
          link_item->action = EYESIGHT_LINK_ACTION_LAUNCH;
          link_item->dest.action_launch = _eyesight_link_action_launch_fill(lnk);
          break;
        case FZ_LINK_NAMED:
          link_item->action = EYESIGHT_LINK_ACTION_NAMED;
          link_item->dest.action_named = _eyesight_link_action_named_fill(lnk);
          break;
        case FZ_LINK_GOTOR:
          link_item->action = EYESIGHT_LINK_ACTION_GOTO_REMOTE_FILE;
          link_item->dest.action_goto_remote_file = _eyesight_link_action_goto_remote_file_fill(lnk);
          break;
        default:
          /* this case should not be reached */
          link_item->action = EYESIGHT_LINK_ACTION_UNKNOWN;
          link_item->dest.action_unknown = _eyesight_link_action_unknown_fill(lnk);
          break;
        }

      switch (ebp->page.orientation % 360)
        {
/*         case 90: */
/*           link_item->rect.x = (int)x1; */
/*           link_item->rect.y = (int)(ebp->page.page->getCropWidth() - x2); */
/*           link_item->rect.w = (int)(y2 - y1); */
/*           link_item->rect.h = (int)(x2 - x1); */
/*           break; */
/*         case 180: */
/*           link_item->rect.x = (int)(ebp->page.page->getCropWidth() - x2); */
/*           link_item->rect.y = (int)(ebp->page.page->getCropHeight() - y2); */
/*           link_item->rect.w = (int)(x2 - x1); */
/*           link_item->rect.h = (int)(y2 - y1); */
/*           break; */
/*         case 270: */
/*           link_item->rect.x = (int)(ebp->page.page->getCropHeight() - y2); */
/*           link_item->rect.y = (int)x1; */
/*           link_item->rect.w = (int)(y2 - y1); */
/*           link_item->rect.h = (int)(x2 - x1); */
/*           break; */
        default:
          link_item->rect.x = round(lnk->rect.x0);
          link_item->rect.y = round(ebp->page.height - lnk->rect.y1);
          link_item->rect.w = round(lnk->rect.x1 - lnk->rect.x0);
          link_item->rect.h = round(lnk->rect.y1 - lnk->rect.y0);
          break;
        }

      links_list = eina_list_prepend(links_list, link_item);

    next_link:
      lnk = lnk->next;
    }

  return links_list;
}

static void
_eyesight_page_links_free(Eyesight_Backend_Pdf *ebp)
{
  Eina_List *l = NULL;
  void *data;

  if (!ebp->page.links)
    return;

  EINA_LIST_FOREACH(ebp->page.links, l, data)
    {
      Eyesight_Link *lnk = (Eyesight_Link *)data;

      switch (lnk->action)
        {
        case EYESIGHT_LINK_ACTION_GOTO_REMOTE_FILE:
          if (lnk->dest.action_goto_remote_file.filename)
            free(lnk->dest.action_goto_remote_file.filename);
          break;
        case EYESIGHT_LINK_ACTION_URI:
          if (lnk->dest.action_uri.uri)
            free(lnk->dest.action_uri.uri);
          break;
        case EYESIGHT_LINK_ACTION_LAUNCH:
          if (lnk->dest.action_launch.app)
            free(lnk->dest.action_launch.app);
          break;
        case EYESIGHT_LINK_ACTION_NAMED:
          if (lnk->dest.action_named.name)
            free(lnk->dest.action_named.name);
          break;
        case EYESIGHT_LINK_ACTION_UNKNOWN:
          if (lnk->dest.action_unknown.action)
            free(lnk->dest.action_unknown.action);
          break;
        default:
          break;
        }
      free(lnk);
    }
}

static Eina_Bool
em_init(Evas *evas, Evas_Object **obj, void **eyesight_backend)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eyesight_backend)
    return EINA_FALSE;

  ebp = (Eyesight_Backend_Pdf *)calloc(1, sizeof(Eyesight_Backend_Pdf));
  if (!ebp)
    return EINA_FALSE;

  ebp->obj = evas_object_image_add(evas);
  if (!ebp->obj)
    goto free_ebp;

  /* FIXME:
   * 1st parameter: custom memory allocator ?
   * 2nd parameter: locks/unlocks for multithreading
   */
  ebp->doc.ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
  if (!ebp->doc.ctx)
    {
      ERR("Could not create context");
      goto del_obj;
    }

  *obj = ebp->obj;
  *eyesight_backend = ebp;

  return EINA_TRUE;

 del_obj:
  evas_object_del(ebp->obj);
 free_ebp:
  free(ebp);

  return EINA_FALSE;
}

static void
em_shutdown(void *eb)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    return;

  ebp = (Eyesight_Backend_Pdf *)eb;
  fz_free_context(ebp->doc.ctx);
  evas_object_del(ebp->obj);
  free(eb);
}

static void *
em_file_open(void *eb, const char *filename)
{
  char buf[64];
  char *tmp;
  Eyesight_Backend_Pdf  *ebp;
  Eyesight_Document_Pdf *doc;
  fz_device *dev;
  fz_bbox bbox;
  fz_rect rect;

  if (!eb || !filename || !*filename)
    return NULL;

  DBG("Open file %s", filename);

  ebp = (Eyesight_Backend_Pdf *)eb;
  ebp->filename = strdup(filename);
  if (!ebp->filename)
    return NULL;

  ebp->doc.doc = pdf_open_document(ebp->doc.ctx, filename);
  if (!ebp->doc.doc)
    {
      ERR("Could not open file %s", ebp->filename);
      goto free_filename;
    }

  doc = (Eyesight_Document_Pdf *)calloc(1, sizeof(Eyesight_Document_Pdf));
  if (!doc)
    goto free_xref;

  if (fz_meta((fz_document *)ebp->doc.doc, FZ_META_FORMAT_INFO, buf, sizeof(buf)) >= 0)
    {
      tmp = strchr(buf + 4, '.');
      *tmp = '\0';
      doc->version_maj = atoi(buf + 4);
      doc->version_min = atoi(tmp + 1);
    }
  doc->filename = ebp->filename;

  doc->title = _eyesight_document_property_get(ebp->doc.doc, "Title");
  doc->author = _eyesight_document_property_get(ebp->doc.doc, "Author");
  doc->subject = _eyesight_document_property_get(ebp->doc.doc, "Subject");
  doc->keywords = _eyesight_document_property_get(ebp->doc.doc, "Keywords");
  doc->creator = _eyesight_document_property_get(ebp->doc.doc, "Creator");
  doc->producer = _eyesight_document_property_get(ebp->doc.doc, "Producer");
  doc->date_creation = _eyesight_document_date_get(ebp->doc.doc, "CreationDate");
  doc->date_modification = _eyesight_document_date_get(ebp->doc.doc, "ModDate");

/*   doc->mode = _eyesight_document_page_mode_get(ebp->doc.pdfdoc); */
/*   doc->layout = _eyesight_document_page_layout_get(ebp->doc.pdfdoc); */

/*   doc->linearized = ebp->doc.pdfdoc->isLinearized(); */
  doc->printable = fz_meta((fz_document *)ebp->doc.doc, FZ_META_HAS_PERMISSION, NULL, FZ_PERMISSION_PRINT) == 0;
  doc->changeable = fz_meta((fz_document *)ebp->doc.doc, FZ_META_HAS_PERMISSION, NULL, FZ_PERMISSION_CHANGE) == 0;
  doc->copyable = fz_meta((fz_document *)ebp->doc.doc, FZ_META_HAS_PERMISSION, NULL, FZ_PERMISSION_COPY) == 0;
  doc->notable = fz_meta((fz_document *)ebp->doc.doc, FZ_META_HAS_PERMISSION, NULL, FZ_PERMISSION_NOTES) == 0;
  ebp->document = doc;

  ebp->page.page = pdf_load_page(ebp->doc.doc, 0);
  if (!ebp->page.page)
    {
       ERR("Could not retrieve first page from the document");
       goto free_xref;
    }

  /* FIXME: cache dev */
  ebp->page.display_list = fz_new_display_list(ebp->doc.ctx);
  dev = fz_new_list_device(ebp->doc.ctx, ebp->page.display_list);
  /* FIXME: last param for multi threading */
  pdf_run_page(ebp->doc.doc, ebp->page.page, dev, fz_identity, NULL);
  fz_free_device(dev);

  rect = pdf_bound_page(ebp->doc.doc, ebp->page.page);
  bbox = fz_round_rect(fz_transform_rect(fz_identity, rect));
  ebp->page.width = bbox.x1 - bbox.x0;
  ebp->page.height = bbox.y1 - bbox.y0;
  printf("mb y1 : %f\n", rect.y1);
  printf("(%f,%f) (%f,%f)\n",
         (double)ebp->page.width, (double)ebp->page.height,
         rect.x1 - rect.x0,
         rect.y1 - rect.y0);

  ebp->page.links = _eyesight_page_links_get(ebp);

  ebp->page.page_nbr = 0;
  ebp->page.hscale = 1.0;
  ebp->page.vscale = 1.0;
  ebp->page.orientation = EYESIGHT_ORIENTATION_PORTRAIT;

  return doc;

 free_xref:
  pdf_close_document(ebp->doc.doc);
 free_filename:
  free(ebp->filename);
  ebp->filename = NULL;

  return NULL;
}

static void
em_file_close(void *eb)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    return;

  ebp = (Eyesight_Backend_Pdf *)eb;

  DBG("Close file %s", ebp->filename);

  if (ebp->page.page)
    {
      pdf_free_page(ebp->doc.doc, ebp->page.page);
      ebp->page.page = NULL;
    }

  _eyesight_page_links_free(ebp);
  _eyesight_index_unfill(ebp->doc.toc);
  if (ebp->document)
    {
      if (ebp->document->title) free(ebp->document->title);
      if (ebp->document->author) free(ebp->document->author);
      if (ebp->document->subject) free(ebp->document->subject);
      if (ebp->document->keywords) free(ebp->document->keywords);
      if (ebp->document->creator) free(ebp->document->creator);
      if (ebp->document->producer) free(ebp->document->producer);
      if (ebp->document->date_creation) free(ebp->document->date_creation);
      if (ebp->document->date_modification) free(ebp->document->date_modification);
      free(ebp->document);
      ebp->document = NULL;
    }
  if (ebp->doc.doc)
    {
      pdf_close_document(ebp->doc.doc);
      ebp->doc.doc = NULL;
    }
  if (ebp->filename)
    {
      free(ebp->filename);
      ebp->filename = NULL;
    }
}

static Eina_Bool
em_is_locked(void *eb)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    return EINA_FALSE;

  ebp = (Eyesight_Backend_Pdf *)eb;

  return pdf_needs_password(ebp->doc.doc) ? EINA_TRUE : EINA_FALSE;
}

static Eina_Bool
em_password_set(void *eb, const char *password)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    return EINA_FALSE;

  ebp = (Eyesight_Backend_Pdf *)eb;
  return pdf_authenticate_password(ebp->doc.doc, (char *)password) ? EINA_TRUE : EINA_FALSE;
}

static const Eina_List *
em_toc_get(void *eb)
{
  Eyesight_Backend_Pdf *ebp;
  fz_outline *outline;

  if (!eb)
    return NULL;

  ebp = (Eyesight_Backend_Pdf *)eb;

  outline = pdf_load_outline(ebp->doc.doc);
  if (outline)
    {
      ebp->doc.toc = _eyesight_index_fill(ebp->doc.doc, outline, NULL);
      fz_free_outline(ebp->doc.ctx, outline);
    }

  return ebp->doc.toc;
}

static int
em_page_count(void *eb)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    return 0;

  ebp = (Eyesight_Backend_Pdf *)eb;

  return pdf_count_pages(ebp->doc.doc);
}

static void
em_page_set(void *eb, int page)
{
  Eyesight_Backend_Pdf *ebp;
  pdf_page *p;
  fz_device *dev;
  fz_matrix ctm;
  fz_rect rect;
  fz_bbox bbox;

  if (!eb || (page < 0))
    return;

  ebp = (Eyesight_Backend_Pdf *)eb;

  p = pdf_load_page(ebp->doc.doc, page);
  if (!p)
    {
      ERR("Could not retrieve page %d from the document", page);
      return;
    }

  /* FIXME: cache list and dev */
  if (ebp->page.display_list)
    fz_free_display_list(ebp->doc.ctx, ebp->page.display_list);
  ebp->page.display_list = fz_new_display_list(ebp->doc.ctx);
  dev = fz_new_list_device(ebp->doc.ctx, ebp->page.display_list);
  /* FIXME: last param for multi threading */
  pdf_run_page(ebp->doc.doc, p, dev, fz_identity, NULL);
  fz_free_device(dev);

  rect = pdf_bound_page(ebp->doc.doc, ebp->page.page);
/*   ctm = fz_translate(0, -rect.y1); */
  ctm = fz_concat(ctm, fz_scale(ebp->page.hscale, -ebp->page.vscale));
  ctm = fz_concat(ctm, fz_rotate(ebp->page.orientation));
  bbox = fz_round_rect(fz_transform_rect(ctm, rect));
  ebp->page.width = bbox.x1 - bbox.x0;
  ebp->page.height = bbox.y1 - bbox.y0;

  if (ebp->page.page)
    pdf_free_page(ebp->doc.doc, ebp->page.page);

  ebp->page.page = p;
  ebp->page.page_nbr = page;

  ebp->page.links = _eyesight_page_links_get(ebp);
}

static int
em_page_get(void *eb)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    return 0;

  ebp = (Eyesight_Backend_Pdf *)eb;

  return ebp->page.page_nbr;
}

static void
em_page_scale_set(void *eb, double hscale, double vscale)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    return;

  DBG("horizontal scale=%f vertical scale=%f", hscale, vscale);

  ebp = (Eyesight_Backend_Pdf *)eb;

  ebp->page.hscale = hscale;
  ebp->page.vscale = vscale;
}

static void
em_page_scale_get(void *eb, double *hscale, double *vscale)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    {
      if (hscale) *hscale = 1.0;
      if (vscale) *vscale = 1.0;
    }

  ebp = (Eyesight_Backend_Pdf *)eb;

  if (hscale) *hscale = ebp->page.hscale;
  if (vscale) *vscale = ebp->page.vscale;
}

static void
em_page_orientation_set(void *eb, Eyesight_Orientation orientation)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    return;

  DBG("orientation=%d", orientation);

  ebp = (Eyesight_Backend_Pdf *)eb;

  ebp->page.orientation = orientation;
}

static Eyesight_Orientation
em_page_orientation_get(void *eb)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    return EYESIGHT_ORIENTATION_PORTRAIT;

  ebp = (Eyesight_Backend_Pdf *)eb;

  return ebp->page.orientation;
}

static void
em_page_size_get(void *eb, int *width, int *height)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    {
      if (width) *width = 0;
      if (height) *height = 0;
      return;
    }

  ebp = (Eyesight_Backend_Pdf *)eb;

  if (width) *width = ebp->page.width;
  if (height) *height = ebp->page.height;
}

static void
em_page_render(void *eb)
{
  Eyesight_Backend_Pdf *ebp;
  unsigned int    *m = NULL;
  fz_matrix ctm;
  fz_bbox bbox;
  fz_device *dev;
  fz_text_span *text;
  fz_pixmap *image;
  fz_rect rect;
  int width;
  int height;

  if (!eb)
    return;

  ebp = (Eyesight_Backend_Pdf *)eb;

  rect = pdf_bound_page(ebp->doc.doc, ebp->page.page);
/*   ctm = fz_identity; */
/*   ctm = fz_concat(ctm, fz_translate(0, -rect.y1)); */
  ctm = fz_scale(ebp->page.hscale, ebp->page.vscale);
  ctm = fz_concat(ctm, fz_rotate(ebp->page.orientation));
  bbox = fz_round_rect(fz_transform_rect(ctm, rect));

  /* FIXME: cache list and dev */
  if (!ebp->page.display_list)
    ebp->page.display_list = fz_new_display_list(ebp->doc.ctx);
  dev = fz_new_list_device(ebp->doc.ctx, ebp->page.display_list);
  /* FIXME: last param for multi threading */
  pdf_run_page(ebp->doc.doc, ebp->page.page, dev, fz_identity, NULL);
  fz_free_device(dev);

  width = bbox.x1 - bbox.x0;
  height = bbox.y1 - bbox.y0;

  evas_object_image_size_set(ebp->obj, width, height);
  evas_object_image_fill_set(ebp->obj, 0, 0, width, height);
  m = (unsigned int *)evas_object_image_data_get(ebp->obj, 1);
  if (!m)
    {
       ERR("Could not retrieve data from the Evas Object");
       fz_drop_pixmap(ebp->doc.ctx, image);
       return;
    }

  image = fz_new_pixmap_with_data(ebp->doc.ctx, fz_device_rgb, width, height, (unsigned char *)m);
  fz_clear_pixmap_with_value(ebp->doc.ctx, image, 0xff);
  dev = fz_new_draw_device(ebp->doc.ctx, image);
  fz_run_display_list(ebp->page.display_list, dev, ctm, fz_infinite_bbox, NULL);
  fz_free_device(dev);

  evas_object_image_data_set(ebp->obj, m);
  evas_object_image_data_update_add(ebp->obj, 0, 0,
                                    fz_pixmap_width(ebp->doc.ctx, image),
                                    fz_pixmap_width(ebp->doc.ctx, image));
  evas_object_resize(ebp->obj,
                     fz_pixmap_width(ebp->doc.ctx, image),
                     fz_pixmap_width(ebp->doc.ctx, image));
  fz_drop_pixmap(ebp->doc.ctx, image);
}

static char *
em_page_text_get(void *eb, Eina_Rectangle rect)
{
  /* FIXME: todo */
  return NULL;
}

static Eina_List *
em_page_text_find(void *eb, const char *text, Eina_Bool is_case_sensitive, Eina_Bool backward)
{
  /* FIXME: todo */
  return NULL;
}

static Eina_List *
em_page_links_get(void *eb)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    return NULL;

  ebp = (Eyesight_Backend_Pdf *)eb;

  return ebp->page.links;
}

static Eyesight_Module _eyesight_module_pdf =
{
  em_init,
  em_shutdown,
  em_file_open,
  em_file_close,
  em_is_locked,
  em_password_set,
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

   if (_eyesight_pdf_count == 0)
     {
       if (_eyesight_pdf_log_domain < 0)
         {
           _eyesight_pdf_log_domain = eina_log_domain_register("eyesight-pdf", EINA_COLOR_LIGHTCYAN);
           if (_eyesight_pdf_log_domain < 0)
             {
               EINA_LOG_CRIT("Could not register log domain 'eyesight-pdf'");
               return EINA_FALSE;
             }
         }
     }

   _eyesight_pdf_count++;

   if (!_eyesight_module_pdf.init(evas, obj, backend))
     {
        ERR("Could not initialize module");
        eina_log_domain_unregister(_eyesight_pdf_log_domain);
        _eyesight_pdf_log_domain = -1;
        _eyesight_pdf_count--;
       return EINA_FALSE;
     }

   *module = &_eyesight_module_pdf;
   return EINA_TRUE;
}

static void
module_close(Eyesight_Module *module, void *backend)
{
  _eyesight_pdf_count--;
  if (_eyesight_pdf_count == 0)
    {
      eina_log_domain_unregister(_eyesight_pdf_log_domain);
      _eyesight_pdf_log_domain = -1;
    }
   _eyesight_module_pdf.shutdown(backend);
}

Eina_Bool
pdf_module_init(void)
{
   return _eyesight_module_register("pdf", module_open, module_close);
}

void
pdf_module_shutdown(void)
{
   _eyesight_module_unregister("pdf");
}

#ifndef EYESIGHT_STATIC_BUILD_PDF

EINA_MODULE_INIT(pdf_module_init);
EINA_MODULE_SHUTDOWN(pdf_module_shutdown);

#endif
