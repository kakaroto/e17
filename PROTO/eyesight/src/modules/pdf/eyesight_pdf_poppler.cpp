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

#include <cmath>

#include <Evas.h>

#include <GlobalParams.h>
#include <PDFDoc.h>
#include <FontInfo.h>
#include <SplashOutputDev.h>
#include <TextOutputDev.h>
#include <splash/SplashBitmap.h>
#include <Outline.h>
#include <Gfx.h>
#include <PDFDocEncoding.h>
#include <UnicodeMap.h>
#include <ErrorCodes.h>
#include <Link.h>

#include "Eyesight_Module_Pdf.h"
#include "Eyesight.h"
#include "eyesight_private.h"
#include "eyesight_pdf_poppler.h"


#define DBG(...) EINA_LOG_DOM_DBG(_eyesight_pdf_log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_eyesight_pdf_log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_eyesight_pdf_log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_eyesight_pdf_log_domain, __VA_ARGS__)
#define CRIT(...) EINA_LOG_DOM_CRIT(_eyesight_pdf_log_domain, __VA_ARGS__)


static int _eyesight_pdf_log_domain = -1;

static int
_eyesight_link_action_page_get(PDFDoc *doc, LinkAction *action)
{
  int page;

  if (!action->isOk())
    return -1;

  GooString *named_dest = ((LinkGoTo *)action)->getNamedDest();
  LinkDest *dest = ((LinkGoTo *)action)->getDest();

  if (!dest && named_dest)
    {
      dest = doc->findDest(named_dest);
    }
  if (dest && dest->isOk())
    {
      if (dest->isPageRef())
        {
          page = doc->findPage(dest->getPageRef().num,
                               dest->getPageRef().gen) - 1;
        }
      else
        {
          page = dest->getPageNum() - 1;
        }
    }

  return page;
}

static Eyesight_Link_Action_Unknown
_eyesight_link_action_unknown_fill(LinkUnknown *action)
{
  Eyesight_Link_Action_Unknown action_unknown;

  action_unknown.action = NULL;
  if (!action->isOk())
    return action_unknown;

  /* FIXME: to complete */
  action_unknown.action = strdup(action->getAction()->getCString());
  /* we don't need any check on the success of strdup() */

  return action_unknown;
}

static Eyesight_Link_Action_Goto
_eyesight_link_action_goto_fill(PDFDoc *doc, LinkGoTo *action)
{
  Eyesight_Link_Action_Goto action_goto;

  action_goto.page = -1;

  if (!action->isOk())
    return action_goto;

  GooString *named_dest = (action)->getNamedDest();
  LinkDest *dest = (action)->getDest();

  if (!dest && named_dest)
    {
      dest = doc->findDest(named_dest);
    }
  if (dest && dest->isOk())
    {
      if (dest->isPageRef())
        {
          action_goto.page = doc->findPage(dest->getPageRef().num,
                                           dest->getPageRef().gen) - 1;
        }
      else
        {
          action_goto.page = dest->getPageNum() - 1;
        }
    }

  /* FIXME: to complete */

  return action_goto;
}

static Eyesight_Link_Action_Goto_New_File
_eyesight_link_action_goto_new_file_fill(LinkGoToR *action)
{
  Eyesight_Link_Action_Goto_New_File action_goto_new_file;

  action_goto_new_file.filename = NULL;
  if (!action->isOk())
    return action_goto_new_file;

  /* FIXME: to do */

  return action_goto_new_file;
}

static Eyesight_Link_Action_Launch
_eyesight_link_action_launch_fill(LinkLaunch *action)
{
  Eyesight_Link_Action_Launch action_launch;

  action_launch.app = NULL;
  action_launch.params = NULL;
  if (!action->isOk())
    return action_launch;

  action_launch.app = strdup(action->getFileName()->getCString());
  if (!action_launch.app)
    return action_launch;

  action_launch.params = strdup(action->getParams()->getCString());
  /* we don't need any check on the success of strdup() */

  return action_launch;
}

static Eyesight_Link_Action_Uri
_eyesight_link_action_uri_fill(LinkURI *action)
{
  Eyesight_Link_Action_Uri action_uri;

  action_uri.uri = NULL;
  if (!action->isOk())
    return action_uri;

  action_uri.uri = strdup(action->getURI()->getCString());
  /* we don't need any check on the success of strdup() */

  return action_uri;
}

static Eyesight_Link_Action_Named
_eyesight_link_action_named_fill(LinkNamed *action)
{
  Eyesight_Link_Action_Named action_named;

  action_named.name = NULL;
  if (!action->isOk())
    return action_named;

  action_named.name = strdup(action->getName()->getCString());
  /* we don't need any check on the success of strdup() */

  return action_named;
}

static Eyesight_Link_Action_Movie
_eyesight_link_action_movie_fill(LinkMovie *action)
{
  Eyesight_Link_Action_Movie action_movie;

  action_movie.operation = EYESIGHT_MOVIE_OPERATION_PLAY;
  action_movie.title = NULL;
  if (!action->isOk())
    return action_movie;

  switch (action->getOperation())
    {
    case LinkMovie::operationTypePlay:
      action_movie.operation = EYESIGHT_MOVIE_OPERATION_PLAY;
      break;
    case LinkMovie::operationTypePause:
      action_movie.operation = EYESIGHT_MOVIE_OPERATION_PAUSE;
      break;
    case LinkMovie::operationTypeResume:
      action_movie.operation = EYESIGHT_MOVIE_OPERATION_RESUME;
      break;
    case LinkMovie::operationTypeStop:
      action_movie.operation = EYESIGHT_MOVIE_OPERATION_STOP;
      break;
    }
  /* FIXME: to complete (annotRef not taken into account) */
  action_movie.title = strdup(action->getAnnotTitle()->getCString());
  /* we don't need any check on the success of strdup() */

  return action_movie;
}

static Eyesight_Link_Action_Rendition
_eyesight_link_action_rendition_fill(LinkRendition *action)
{
  Eyesight_Link_Action_Rendition action_rendition;

  action_rendition.script = NULL;
  if (!action->isOk())
    return action_rendition;

#ifdef HAVE_POPPLER_0_14

  /* FIXME: to complete */
  action_rendition.script = strdup(action->getScript()->getCString());
  /* we don't need any check on the success of strdup() */

#endif

  return action_rendition;
}

static Eyesight_Link_Action_Sound
_eyesight_link_action_sound_fill(LinkSound *action)
{
  Eyesight_Link_Action_Sound action_sound;

  action_sound.volume = 0.0;
  action_sound.sync = EINA_FALSE;
  action_sound.repeat = EINA_FALSE;
  action_sound.mix = EINA_FALSE;
  if (!action->isOk())
    return action_sound;

  /* FIXME: to complete (Sound) */
  action_sound.volume = action->getVolume();
  action_sound.sync = action->getSynchronous();
  action_sound.repeat = action->getRepeat();
  action_sound.mix = action->getMix();

  return action_sound;
}

static Eyesight_Link_Action_Javascript
_eyesight_link_action_javascript_fill(LinkJavaScript *action)
{
  Eyesight_Link_Action_Javascript action_javascript;

  action_javascript.script = NULL;
  if (!action->isOk())
    return action_javascript;

  /* FIXME: to complete */
  action_javascript.script = strdup(action->getScript()->getCString());
  /* we don't need any check on the success of strdup() */

  return action_javascript;
}

#ifdef HAVE_POPPLER_0_14

static Eyesight_Link_Action_OCG_State
_eyesight_link_action_ocg_state_fill(LinkOCGState *action)
{
  Eyesight_Link_Action_OCG_State action_ocg;

  action_ocg.preserved = EINA_FALSE;
  if (!action->isOk())
    return action_ocg;

  /* FIXME: to complete */
  action_ocg.preserved = action->getPreserveRB();

  return action_ocg;
}

#endif

static Eina_List *
_eyesight_page_links_get(Eyesight_Backend_Pdf *ebp)
{
  Eina_List *links_list = NULL;
  Object obj;

  Links links(ebp->page.page->getAnnots(&obj),
              ebp->doc.pdfdoc->getCatalog()->getBaseURI());

  obj.free();

  for (int i = 0; i < links.getNumLinks(); i++)
    {
      Eyesight_Link *link_item;
      Link *link;
      LinkAction *link_action;
      double x1, x2, y1, y2;

      link = links.getLink(i);
      if (!link->isOk())
        continue;

      link_item = (Eyesight_Link *)calloc(1, sizeof(Eyesight_Link));
      if (!link_item)
        continue;

      link_action = link->getAction();

      switch (link_action->getKind())
        {
        case actionGoTo:
          link_item->action = EYESIGHT_LINK_ACTION_GOTO;
          link_item->dest.action_goto = _eyesight_link_action_goto_fill(ebp->doc.pdfdoc, (LinkGoTo *)link_action);
            break;
        case actionGoToR:
          link_item->action = EYESIGHT_LINK_ACTION_GOTO_NEW_FILE;
          link_item->dest.action_goto_new_file = _eyesight_link_action_goto_new_file_fill((LinkGoToR *)link_action);
          break;
        case actionLaunch:
          link_item->action = EYESIGHT_LINK_ACTION_LAUNCH;
          link_item->dest.action_launch = _eyesight_link_action_launch_fill((LinkLaunch *)link_action);
          break;
        case actionURI:
          link_item->action = EYESIGHT_LINK_ACTION_URI;
          link_item->dest.action_uri = _eyesight_link_action_uri_fill((LinkURI *)link_action);
          break;
        case actionNamed:
          link_item->action = EYESIGHT_LINK_ACTION_NAMED;
          link_item->dest.action_named = _eyesight_link_action_named_fill((LinkNamed *)link_action);
          break;
        case actionMovie:
          link_item->action = EYESIGHT_LINK_ACTION_MOVIE;
          link_item->dest.action_movie = _eyesight_link_action_movie_fill((LinkMovie *)link_action);
          break;
        case actionRendition:
          link_item->action = EYESIGHT_LINK_ACTION_RENDITION;
          link_item->dest.action_rendition = _eyesight_link_action_rendition_fill((LinkRendition *)link_action);
          break;
        case actionSound:
          link_item->action = EYESIGHT_LINK_ACTION_SOUND;
          link_item->dest.action_sound = _eyesight_link_action_sound_fill((LinkSound *)link_action);
          break;
        case actionJavaScript:
          link_item->action = EYESIGHT_LINK_ACTION_JAVASCRIPT;
          link_item->dest.action_javascript = _eyesight_link_action_javascript_fill((LinkJavaScript *)link_action);
          break;
#ifdef HAVE_POPPLER_0_14
        case actionOCGState:
          link_item->action = EYESIGHT_LINK_ACTION_OCG_STATE;
          link_item->dest.action_ocg_state = _eyesight_link_action_ocg_state_fill((LinkOCGState *)link_action);
          break;
#endif
        case actionUnknown:
          link_item->action = EYESIGHT_LINK_ACTION_UNKNOWN;
          link_item->dest.action_unknown = _eyesight_link_action_unknown_fill((LinkUnknown *)link_action);
          break;
        }

      link->getRect (&x1, &y1, &x2, &y2);
      x1 -= ebp->page.page->getCropBox()->x1;
      x2 -= ebp->page.page->getCropBox()->x1;
      y1 -= ebp->page.page->getCropBox()->y1;
      y2 -= ebp->page.page->getCropBox()->y1;

      switch (ebp->page.page->getRotate())
        {
        case 90:
          link_item->rect.x = (int)x1;
          link_item->rect.y = (int)(ebp->page.page->getCropWidth() - x2);
          link_item->rect.w = (int)(y2 - y1);
          link_item->rect.h = (int)(x2 - x1);
          break;
        case 180:
          link_item->rect.x = (int)(ebp->page.page->getCropWidth() - x2);
          link_item->rect.y = (int)(ebp->page.page->getCropHeight() - y2);
          link_item->rect.w = (int)(x2 - x1);
          link_item->rect.h = (int)(y2 - y1);
          break;
        case 270:
          link_item->rect.x = (int)(ebp->page.page->getCropHeight() - y2);
          link_item->rect.y = (int)x1;
          link_item->rect.w = (int)(y2 - y1);
          link_item->rect.h = (int)(x2 - x1);
          break;
        default:
          printf (" %f %f %f %f\n", x1, ebp->page.page->getCropHeight() - y1, x2-x1, y2-y1);
          link_item->rect.x = round(x1);
          link_item->rect.y = round(ebp->page.page->getCropHeight() - y2);
          link_item->rect.w = round(x2 - x1);
          link_item->rect.h = round(y2 - y1);
          break;
        }

      links_list = eina_list_prepend(links_list, link_item);
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
      Eyesight_Link *link = (Eyesight_Link *)data;

      switch (link->action)
        {
        case EYESIGHT_LINK_ACTION_GOTO:
          /* nothing */
          break;
        case EYESIGHT_LINK_ACTION_GOTO_NEW_FILE:
          if (link->dest.action_goto_new_file.filename)
            free(link->dest.action_goto_new_file.filename);
          break;
        case EYESIGHT_LINK_ACTION_LAUNCH:
          if (link->dest.action_launch.app)
            free(link->dest.action_launch.app);
          if (link->dest.action_launch.params)
            free(link->dest.action_launch.params);
          break;
        case EYESIGHT_LINK_ACTION_URI:
          if (link->dest.action_uri.uri)
            free(link->dest.action_uri.uri);
          break;
        case EYESIGHT_LINK_ACTION_NAMED:
          if (link->dest.action_named.name)
            free(link->dest.action_named.name);
          break;
        case EYESIGHT_LINK_ACTION_MOVIE:
          if (link->dest.action_movie.title)
            free(link->dest.action_movie.title);
          break;
        case EYESIGHT_LINK_ACTION_RENDITION:
          if (link->dest.action_rendition.script)
            free(link->dest.action_rendition.script);
          break;
        case EYESIGHT_LINK_ACTION_SOUND:
          /* nothing to do */
          break;
        case EYESIGHT_LINK_ACTION_JAVASCRIPT:
          if (link->dest.action_javascript.script)
            free(link->dest.action_javascript.script);
          break;
#ifdef HAVE_POPPLER_0_14
        case EYESIGHT_LINK_ACTION_OCG_STATE:
          /* nothing to do */
          break;
#endif
        case EYESIGHT_LINK_ACTION_UNKNOWN:
          if (link->dest.action_unknown.action)
            free(link->dest.action_unknown.action);
          break;
        }
      free(link);
    }
}

static char *
_unicode_to_char (Unicode *unicode,
                  int      len)
{
  static UnicodeMap *uMap = NULL;

  if (uMap == NULL)
    {
      GooString *enc = new GooString("UTF-8");
      uMap = globalParams->getUnicodeMap(enc);
      uMap->incRefCnt();
      delete enc;
    }

  GooString gstr;
  char buf[8]; /* 8 is enough for mapping an unicode char to a string */
  int n;

  for (int i = 0; i < len; ++i)
    {
      n = uMap->mapUnicode(unicode[i], buf, sizeof(buf));
      gstr.append(buf, n);
    }

  return strdup (gstr.getCString());
}

static Eina_List *
_eyesight_index_fill(PDFDoc    *doc,
                     Eina_List *items,
                     GooList   *gitems)
{
  if (!gitems)
    return items;

  for (int i = 0; i < gitems->getLength(); i++) {
    Eyesight_Index_Item *item;
    OutlineItem     *oitem = (OutlineItem *)gitems->get(i);
    Unicode         *utitle = oitem->getTitle ();

    item = eyesight_index_item_new();
    item->title = _unicode_to_char(utitle, oitem->getTitleLength());
    switch (oitem->getAction()->getKind())
      {
      case actionGoTo:
        item->action = EYESIGHT_LINK_ACTION_GOTO;
        break;
      case actionGoToR:
        item->action = EYESIGHT_LINK_ACTION_GOTO_NEW_FILE;
        break;
      case actionLaunch:
        item->action = EYESIGHT_LINK_ACTION_LAUNCH;
        break;
      case actionURI:
        item->action = EYESIGHT_LINK_ACTION_URI;
        break;
      case actionNamed:
        item->action = EYESIGHT_LINK_ACTION_NAMED;
        break;
      case actionMovie:
        item->action = EYESIGHT_LINK_ACTION_MOVIE;
        break;
      case actionRendition:
        item->action = EYESIGHT_LINK_ACTION_RENDITION;
        break;
      case actionSound:
        item->action = EYESIGHT_LINK_ACTION_SOUND;
        break;
      case actionJavaScript:
        item->action = EYESIGHT_LINK_ACTION_JAVASCRIPT;
        break;
#ifdef HAVE_POPPLER_0_14
      case actionOCGState:
        item->action = EYESIGHT_LINK_ACTION_OCG_STATE;
        break;
#endif
      case actionUnknown:
        item->action = EYESIGHT_LINK_ACTION_UNKNOWN;
        break;
      }
    item->page = _eyesight_link_action_page_get(doc, oitem->getAction());
    item->is_open = oitem->isOpen();

    oitem->open ();
    if (oitem->hasKids() && oitem->getKids())
      item->children = _eyesight_index_fill(doc, item->children, oitem->getKids ());
    items = eina_list_append (items, item);
  }
  return items;
}

static void
_eyesight_index_unfill (Eina_List *items)
{
  Eyesight_Index_Item *item;

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

static char *
_eyesight_document_property_get (Dict *dict, const char *property)
{
  Object     obj;
  GooString *goo_string;
  char      *title = NULL;

  if (!dict->lookup((char *)property, &obj)->isString())
    {
      obj.free();
      return NULL;
    }

  goo_string = obj.getString();
  /* Unicode */
  if (((goo_string->getChar(0) & 0xff) == 0xfe) &&
      ((goo_string->getChar(1) & 0xff) == 0xff))
    {
      /* FIXME: to do */
      printf("Unicode !\n");
    }
  else
    {
      int   length;

      length = goo_string->getLength();
      title = (char *)malloc(sizeof(char) * (length + 1));
      if (!title)
        {
          obj.free();
          return NULL;
        }
      for (int i = 0; i < length; i++)
        title[i] = pdfDocEncoding[(unsigned char)goo_string->getChar(i)];
      title[length] = '\0';
    }
  obj.free();

  return title;
}

static char *
_eyesight_document_date_get (Dict *dict, const char *property)
{
  Object     obj;
  GooString *goo_string;
  char      *date = NULL;
  char       res[64];
  int        year;
  int        month;
  int        day;
  int        hour;
  int        minute;
  int        second;
  int        scanned_items;

  if (!dict->lookup((char *)property, &obj)->isString())
    {
      obj.free();
      return NULL;
    }

  goo_string = obj.getString();
  /* Unicode */
  if (((goo_string->getChar(0) & 0xff) == 0xfe) &&
      ((goo_string->getChar(1) & 0xff) == 0xff))
    {
      /* FIXME: to do */
      printf("Unicode !\n");
    }
  else
    {
      int   length;

      length = goo_string->getLength();
      date = (char *)malloc(sizeof(char) * (length + 1));
      if (!date)
        {
          obj.free();
          return NULL;
        }
      memcpy (date, goo_string->getCString (), length + 1);
    }

  /* See PDF Reference 1.3, Section 3.8.2 for PDF Date representation */

  if (date [0] == 'D' && date [1] == ':')
    scanned_items = sscanf (date + 2, "%4d%2d%2d%2d%2d%2d",
                            &year, &month, &day, &hour, &minute, &second);
  else
    scanned_items = sscanf (date, "%4d%2d%2d%2d%2d%2d",
                            &year, &month, &day, &hour, &minute, &second);
  if (scanned_items != 6)
    {
      free (date);
      obj.free ();

      return NULL;
    }

  /* Workaround for y2k bug in Distiller 3, hoping that it won't
   * be used after y2.2k */
  if ((year < 1930) &&
      (strlen (date) > 14)) {
    int century;
    int years_since_1900;

    scanned_items = sscanf (date, "%2d%3d%2d%2d%2d%2d%2d",
                            &century, &years_since_1900, &month, &day, &hour, &minute, &second);

    if (scanned_items != 7) {
      free (date);
      obj.free ();

      return NULL;
    }

    year = century * 100 + years_since_1900;
  }

  snprintf (res, 64, "%d-%d-%d, %d:%d:%d", year, month, day, hour, minute, second);
  free (date);
  obj.free ();

  return strdup (res);
}

Eyesight_Document_Pdf_Page_Mode
_eyesight_document_page_mode_get (PDFDoc *pdfdoc)
{
  switch (pdfdoc->getCatalog()->getPageMode())
    {
    case Catalog::pageModeNone:
      return EYESIGHT_DOCUMENT_PDF_PAGE_MODE_USE_NONE;
    case Catalog::pageModeOutlines:
      return EYESIGHT_DOCUMENT_PDF_PAGE_MODE_USE_OUTLINES;
    case Catalog::pageModeThumbs:
      return EYESIGHT_DOCUMENT_PDF_PAGE_MODE_USE_THUMBS;
    case Catalog::pageModeFullScreen:
      return EYESIGHT_DOCUMENT_PDF_PAGE_MODE_FULLSCREEN;
    case Catalog::pageModeOC:
      return EYESIGHT_DOCUMENT_PDF_PAGE_MODE_USE_OC;
    default:
      return EYESIGHT_DOCUMENT_PDF_PAGE_MODE_USE_NONE;
    }
}

Eyesight_Document_Pdf_Page_Layout
_eyesight_document_page_layout_get (PDFDoc *pdfdoc)
{
  switch (pdfdoc->getCatalog()->getPageLayout())
    {
    case Catalog::pageLayoutNone:
      return EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_NONE;
    case Catalog::pageLayoutSinglePage:
      return EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_SINGLE_PAGE;
    case Catalog::pageLayoutOneColumn:
      return EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_ONE_COLUMN;
    case Catalog::pageLayoutTwoColumnLeft:
      return EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_TWO_COLUMN_LEFT;
    case Catalog::pageLayoutTwoColumnRight:
      return EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_TWO_COLUMN_RIGHT;
    case Catalog::pageLayoutTwoPageLeft:
      return EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_TWO_PAGE_LEFT;
    case Catalog::pageLayoutTwoPageRight:
      return EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_TWO_PAGE_RIGHT;
    default:
      return EYESIGHT_DOCUMENT_PDF_PAGE_LAYOUT_NONE;
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

  if (!(globalParams = new GlobalParams()))
    goto free_ebp;

  ebp->obj = evas_object_image_add(evas);
  if (!ebp->obj)
    goto delete_params;

  *obj = ebp->obj;
  *eyesight_backend = ebp;

  return EINA_TRUE;

 delete_params:
  delete globalParams;
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
  evas_object_del(ebp->obj);
  delete globalParams;
  free(eb);
}

static void *
em_file_open(void *eb, const char *filename)
{
  Eyesight_Backend_Pdf  *ebp;
  Eyesight_Document_Pdf *doc;
  Object                 obj;
  Outline               *outline;
  GooList               *gitems;

  if (!eb || !filename || !*filename)
    return NULL;

  DBG("Open file %s", filename);

  ebp = (Eyesight_Backend_Pdf *)eb;
  ebp->filename = strdup(filename);
  if (!ebp->filename)
    return NULL;

  ebp->doc.pdfdoc = new PDFDoc(new GooString(filename), NULL);
  if (!ebp->doc.pdfdoc)
    goto free_filename;

  if (!ebp->doc.pdfdoc->isOk() &&
      ebp->doc.pdfdoc->getErrorCode() != errEncrypted)
    {
      ERR("PDF file %s illformed", filename);
      goto delete_pdfdoc;
    }

  ebp->doc.scanner = new FontInfoScanner(ebp->doc.pdfdoc);
  if (!ebp->doc.scanner)
    goto delete_pdfdoc;

  ebp->doc.pdfdoc->getDocInfo(&obj);
  if (!obj.isDict())
    goto delete_scanner;

  ebp->doc.dict = obj.getDict();
  if (!ebp->doc.dict)
    goto delete_scanner;

  doc = (Eyesight_Document_Pdf *)malloc(sizeof(Eyesight_Document_Pdf));
  if (!doc)
    goto delete_dict;

  doc->version_maj = ebp->doc.pdfdoc->getPDFMajorVersion();
  doc->version_min = ebp->doc.pdfdoc->getPDFMinorVersion();
  doc->filename = ebp->filename;

  doc->title = _eyesight_document_property_get(ebp->doc.dict, "Title");
  doc->author = _eyesight_document_property_get(ebp->doc.dict, "Author");
  doc->subject = _eyesight_document_property_get(ebp->doc.dict, "Subject");
  doc->keywords = _eyesight_document_property_get(ebp->doc.dict, "Keywords");
  doc->creator = _eyesight_document_property_get(ebp->doc.dict, "Creator");
  doc->producer = _eyesight_document_property_get(ebp->doc.dict, "Producer");

  doc->date_creation = _eyesight_document_date_get(ebp->doc.dict, "CreationDate");
  doc->date_modification = _eyesight_document_date_get(ebp->doc.dict, "ModDate");

  doc->mode = _eyesight_document_page_mode_get(ebp->doc.pdfdoc);
  doc->layout = _eyesight_document_page_layout_get(ebp->doc.pdfdoc);

  doc->locked = (ebp->doc.pdfdoc->getErrorCode() == errEncrypted) ? EINA_TRUE : EINA_FALSE;
  doc->encrypted = ebp->doc.pdfdoc->isEncrypted();
  doc->linearized = ebp->doc.pdfdoc->isLinearized();
  doc->printable = ebp->doc.pdfdoc->okToPrint();
  doc->changeable = ebp->doc.pdfdoc->okToChange();
  doc->copyable = ebp->doc.pdfdoc->okToCopy();
  doc->notable = ebp->doc.pdfdoc->okToAddNotes();
  ebp->document = doc;

  outline = ebp->doc.pdfdoc->getOutline();
  if (!outline)
    goto no_index;

  gitems = outline->getItems();
  if (!gitems)
    goto no_index;

  ebp->doc.toc = _eyesight_index_fill (ebp->doc.pdfdoc, NULL, gitems);

 no_index:
  ebp->page.page = ebp->doc.pdfdoc->getCatalog()->getPage(1);
  if (!ebp->page.page || !ebp->page.page->isOk())
    {
       ERR("Could not retrieve first page from the document");
       goto free_doc;
    }

  ebp->page.links = _eyesight_page_links_get(ebp);

  ebp->page.hscale = 1.0;
  ebp->page.vscale = 1.0;
  ebp->page.orientation = EYESIGHT_ORIENTATION_PORTRAIT;

  return doc;

 free_doc:
  _eyesight_index_unfill(ebp->doc.toc);
  free(doc);
  ebp->document = NULL;
 delete_dict:
  delete ebp->doc.dict;
 delete_scanner:
  obj.free();
  delete ebp->doc.scanner;
 delete_pdfdoc:
  delete ebp->doc.pdfdoc;
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
  delete ebp->doc.dict;
  delete ebp->doc.scanner;
  delete ebp->doc.pdfdoc;
  if (ebp->filename)
    {
      free(ebp->filename);
      ebp->filename = NULL;
    }
}

static const Eina_List *
em_toc_get(void *eb)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    return NULL;

  ebp = (Eyesight_Backend_Pdf *)eb;

  return ebp->doc.toc;
}

static int
em_page_count(void *eb)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    return 0;

  ebp = (Eyesight_Backend_Pdf *)eb;

  return ebp->doc.pdfdoc->getNumPages();
}

static void
em_page_set(void *eb, int page)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    return;

  ebp = (Eyesight_Backend_Pdf *)eb;

  if (ebp->page.page->getNum() == (page + 1))
    return;

  if (page < 0)
    {
       ERR("Page number is negative");
       return;
    }

  if (page >= ebp->doc.pdfdoc->getNumPages())
    {
      ERR("Page number is beyond the maximal number of pages");
      return;
    }

  Page *p = ebp->doc.pdfdoc->getCatalog()->getPage(page + 1);
  if (!p || !p->isOk())
    {
       ERR("Can not retrieve page %d", page);
       return;
    }

  _eyesight_page_links_free(ebp);

  DBG("page=%d", page);

  ebp->page.page = p;
  ebp->page.links = _eyesight_page_links_get(ebp);
}

static int
em_page_get(void *eb)
{
  Eyesight_Backend_Pdf *ebp;

  if (!eb)
    return 0;

  ebp = (Eyesight_Backend_Pdf *)eb;

  return ebp->page.page->getNum() - 1;
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
  int rotate;
  int w;
  int h;

  if (!eb)
    {
      if (width) *width = 0;
      if (height) *height = 0;
      return;
    }

  ebp = (Eyesight_Backend_Pdf *)eb;

  rotate = ebp->page.page->getRotate();
  if ((rotate == 0) || (rotate == 180))
    {
      if (width) *width = round(ebp->page.page->getCropWidth());
      if (height) *height = round(ebp->page.page->getCropHeight());
      return;
    }
  else
    {
      if (width) *width = round(ebp->page.page->getCropHeight());
      if (height) *height = round(ebp->page.page->getCropWidth());
      return;
    }
}

static void
em_page_render(void *eb)
{
  Eyesight_Backend_Pdf *ebp;
  unsigned int    *m = NULL;
  SplashColor      white;
  SplashColorPtr   color_ptr;
  int              width;
  int              height;

  if (!eb)
    return;

  ebp = (Eyesight_Backend_Pdf *)eb;

  if (!ebp->page.page || !ebp->page.page->isOk ())
    {
      ERR("Current page invalid");
      return;
    }

  white[0] = 255;
  white[1] = 255;
  white[2] = 255;
  white[3] = 255;

  SplashOutputDev output_dev(splashModeXBGR8, 4, gFalse, white);
  output_dev.startDoc(ebp->doc.pdfdoc->getXRef());

  ebp->page.page->display(&output_dev,
                          72.0 * ebp->page.hscale,
                          72.0 * ebp->page.vscale,
                          ebp->page.orientation,
                          false, false, false,
                          ebp->doc.pdfdoc->getCatalog());
  color_ptr = output_dev.getBitmap()->getDataPtr();

  width = output_dev.getBitmap()->getWidth();
  height = output_dev.getBitmap()->getHeight();

  evas_object_image_size_set(ebp->obj, width, height);
  evas_object_image_fill_set(ebp->obj, 0, 0, width, height);
  m = (unsigned int *)evas_object_image_data_get(ebp->obj, 1);
  if (!m)
    {
       ERR("Could not retrieve data from the Evas Object");
       return;
    }

  memcpy(m, color_ptr, height * width * 4);
  evas_object_image_data_set(ebp->obj, m);
  evas_object_image_data_update_add(ebp->obj, 0, 0, width, height);
  evas_object_resize(ebp->obj, width, height);
}

static char *
em_page_text_get(void *eb, Eina_Rectangle rect)
{
  Eyesight_Backend_Pdf *ebp;
  GooString     *sel_text;
  Gfx           *gfx;
  PDFRectangle   pdf_selection;
  char *res;

  if (!eb)
    return NULL;

  DBG("rect=(x=%d, y=%d, w=%d, h=%d)", rect.x, rect.y, rect.w, rect.h);

  ebp = (Eyesight_Backend_Pdf *)eb;

  TextOutputDev text_dev(NULL, 1, 0, 0);

  gfx = ebp->page.page->createGfx(&text_dev,
                                  72.0, 72.0, 0,
                                  false, /* useMediaBox */
                                  true, /* Crop */
                                  -1, -1, -1, -1,
                                  false, /* printing */
                                  ebp->doc.pdfdoc->getCatalog (),
                                  NULL, NULL, NULL, NULL);
  if (!gfx)
    return NULL;

  ebp->page.page->display(gfx);

  delete gfx;

  text_dev.endPage();

  pdf_selection.x1 = rect.x;
  pdf_selection.y1 = rect.y;
  pdf_selection.x2 = rect.x + rect.w;
  pdf_selection.y2 = rect.y + rect.h;

  sel_text = new GooString;
  if (!sel_text)
    return NULL;
  /* added selectionStyleGlyph to catch up with poppler 0.6. Is that correct
     or should we rather use selectionStyleLine or selectionStyleWord? :M: */
  sel_text = text_dev.getSelectionText (&pdf_selection, selectionStyleGlyph);
  res = strdup (sel_text->getCString ());
  delete sel_text;

  return res;
}

static Eina_List *
em_page_text_find(void *eb, const char *text, Eina_Bool is_case_sensitive, Eina_Bool backward)
{
  Eyesight_Backend_Pdf *ebp;
  Eina_Rectangle *match;
  Eina_List      *matches = NULL;
  TextOutputDev   output_dev(NULL, 1, 0, 0);
  double          xMin, yMin, xMax, yMax;
  int             rotate;
  int             length;
  int             height;

  if (!eb)
    return NULL;

  DBG("text=%s", text);

  if (!text || !*text)
    return NULL;

  ebp = (Eyesight_Backend_Pdf *)eb;

#if 1
  GooString tmp (text);
  Unicode  *s;
  bool      anyNonEncoded = false;

  length = tmp.getLength();
  s = (Unicode *)malloc(sizeof(Unicode) * length);
  if (!s)
    return NULL;
  for (int j = 0; j < length && !anyNonEncoded; ++j)
    {
      s[j] = pdfDocEncoding[tmp.getChar(j) & 0xff];
      if (!s[j]) anyNonEncoded = true;
    }
  if (anyNonEncoded)
    {
      for (int j = 0; j < length; ++j)
        s[j] = tmp.getChar(j);
    }
#endif

  length = strlen (text);

  rotate = ebp->page.page->getRotate ();
  if (rotate == 90 || rotate == 270)
    height = round(ebp->page.page->getCropWidth());
  else
    height = round(ebp->page.page->getCropHeight());

  /* FIXME: take into account the scale ?? So, save output_dev ?? */
  ebp->page.page->display(&output_dev, 72, 72, 0, false,
                          true, false,
                          ebp->doc.pdfdoc->getCatalog());

  xMin = 0;
  yMin = 0;
  while (output_dev.findText(s, tmp.getLength (),
                             0, 1, // startAtTop, stopAtBottom
                             1, 0, // startAtLast, stopAtLast
                             is_case_sensitive, backward, // caseSensitive, backwards
                             &xMin, &yMin, &xMax, &yMax)) {
    match = (Eina_Rectangle *)malloc (sizeof (Eina_Rectangle));
    if (match)
      {
        match->x = round(xMin);
        match->y = round(yMin); //h - yMax;
        match->w = round(xMax - xMin);
        match->h = round(yMax - yMin); //h - yMin;
        matches = eina_list_append (matches, match);
      }
  }

  return matches;
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

   if (_eyesight_pdf_log_domain < 0)
     {
        _eyesight_pdf_log_domain = eina_log_domain_register("eyesight-pdf", EINA_COLOR_LIGHTCYAN);
        if (_eyesight_pdf_log_domain < 0)
          {
             EINA_LOG_CRIT("Could not register log domain 'eyesight-pdf'");
             return EINA_FALSE;
          }
     }

   if (!_eyesight_module_pdf.init(evas, obj, backend))
     {
        ERR("Could not initialize module");
        eina_log_domain_unregister(_eyesight_pdf_log_domain);
        _eyesight_pdf_log_domain = -1;
       return EINA_FALSE;
     }

   *module = &_eyesight_module_pdf;
   return EINA_TRUE;
}

static void
module_close(Eyesight_Module *module, void *backend)
{
   eina_log_domain_unregister(_eyesight_pdf_log_domain);
   _eyesight_pdf_log_domain = -1;
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
