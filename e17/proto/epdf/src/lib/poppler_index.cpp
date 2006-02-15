#include <stdlib.h>

#include <GlobalParams.h>
#include <PDFDoc.h>
#include <Outline.h>
#include <UnicodeMap.h>
#include <goo/GooList.h>
#include <Link.h>

#include "poppler_enum.h"
#include "poppler_private.h"
#include "poppler_index.h"
#include "poppler_document.h"


static char *unicode_to_char (Unicode *unicode,
                              int      len);
static void  evas_poppler_index_fill (Ecore_List *items,
                                      GooList    *gitems);
static void  evas_poppler_index_unfill (Ecore_List *items);

/* Index item */
Evas_Poppler_Index_Item *
evas_poppler_index_item_new ()
{
  Evas_Poppler_Index_Item *item;

  item = (Evas_Poppler_Index_Item *)malloc (sizeof (Evas_Poppler_Index_Item));
  if (!item)
    return NULL;

  item->title = NULL;
  item->action = NULL;
  item->children = NULL;

  return item;
}

void
evas_poppler_index_item_delete (Evas_Poppler_Index_Item *item)
{
  if (!item)
    return;

  if (item->title)
    free (item->title);
  if (item->children)
    {
      Evas_Poppler_Index_Item *i;

      ecore_list_goto_first (item->children);
      while ((i = (Evas_Poppler_Index_Item *)ecore_list_next (item->children)))
        {
          evas_poppler_index_item_delete (i);
        }
    }
  free (item);
}

const char *
evas_poppler_index_item_title_get (Evas_Poppler_Index_Item *item)
{
  if (!item)
    return NULL;

  return item->title;
}

Ecore_List *
evas_poppler_index_item_children_get (Evas_Poppler_Index_Item *item)
{
  if (!item)
    return NULL;

  return item->children;
}

Evas_Poppler_Link_Action_Kind
evas_poppler_index_item_action_kind_get (Evas_Poppler_Index_Item *item)
{
  if (!item || !item->action || !item->action->isOk ())
    return EVAS_POPPLER_LINK_ACTION_UNKNOWN;

  switch (item->action->getKind ())
    {
    case actionGoTo: return EVAS_POPPLER_LINK_ACTION_GOTO;
    case actionGoToR: return EVAS_POPPLER_LINK_ACTION_GOTO_NEW_FILE;
    case actionLaunch: return EVAS_POPPLER_LINK_ACTION_LAUNCH;
    case actionURI: return EVAS_POPPLER_LINK_ACTION_URI;
    case actionNamed: return EVAS_POPPLER_LINK_ACTION_NAMED;
    case actionMovie: return EVAS_POPPLER_LINK_ACTION_MOVIE;
    default: return EVAS_POPPLER_LINK_ACTION_UNKNOWN;
    }
}

int
evas_poppler_index_item_page_get (Evas_Poppler_Document *document, Evas_Poppler_Index_Item *item)
{
  if (!item || !item->action || !item->action->isOk ())
    return -1;

  if (item->action->getKind () != actionGoTo)
    return -1;

  LinkDest *dest = ((LinkGoTo *)item->action)->getDest ();

  if (!dest->isOk ())
    return -1;
  
  if (dest->isPageRef ())
    return document->pdfdoc->findPage (dest->getPageRef ().num, dest->getPageRef ().gen) - 1;

  return dest->getPageNum () - 1;
}

/* Index */

Ecore_List *
evas_poppler_index_new (Evas_Poppler_Document *document)
{
  Outline    *outline;
  GooList    *gitems;
  Ecore_List *index = NULL;

  if (!document)
    return index;

  outline = document->pdfdoc->getOutline ();
  if (outline == NULL)
    return index;

  gitems = outline->getItems ();
  if (gitems == NULL)
    return index;

  index = ecore_list_new ();
  evas_poppler_index_fill (index, gitems);

  return index;
}

void
evas_poppler_index_delete (Ecore_List *index)
{
  Ecore_List              *items = index;
  Evas_Poppler_Index_Item *item;

  if (!index)
    return;

  evas_poppler_index_unfill (index);
}

static char *
unicode_to_char (Unicode *unicode,
		 int      len)
{
  static UnicodeMap *uMap = NULL;

  if (uMap == NULL) {
    GooString *enc = new GooString ("UTF-8");
    uMap = globalParams->getUnicodeMap (enc);
    uMap->incRefCnt ();
    delete enc;
  }

  GooString gstr;
  char buf[8]; /* 8 is enough for mapping an unicode char to a string */
  int i, n;

  for (i = 0; i < len; ++i) {
    n = uMap->mapUnicode (unicode[i], buf, sizeof (buf));
    gstr.append (buf, n);
  }

  return strdup (gstr.getCString ());
}

static void
evas_poppler_index_fill (Ecore_List *items,
                         GooList    *gitems)
{
  if (!items || !gitems)
    return;

  for (int i = 0; i < gitems->getLength (); i++)
    {
      Evas_Poppler_Index_Item *item;
      OutlineItem *oitem = (OutlineItem *)gitems->get (i);
      Unicode *utitle = oitem->getTitle ();

      item = evas_poppler_index_item_new ();
      item->title = unicode_to_char (utitle, oitem->getTitleLength ());
      item->action = oitem->getAction ();
      oitem->open ();
      if (oitem->hasKids () && oitem->getKids ())
        {
          item->children = ecore_list_new ();
          evas_poppler_index_fill (item->children, oitem->getKids ());
        }
      ecore_list_append (items, item);
    }
}

static void
evas_poppler_index_unfill (Ecore_List *items)
{
  Evas_Poppler_Index_Item *item;

  if (!items)
    return;

  ecore_list_goto_first (items);
  while ((item = (Evas_Poppler_Index_Item *)ecore_list_next (items)))
    {
      if (item->title)
        free (item->title);

      if (item->children)
        {
          evas_poppler_index_unfill (item->children);
        }
      free (item);
    }
  ecore_list_destroy (items);
}
