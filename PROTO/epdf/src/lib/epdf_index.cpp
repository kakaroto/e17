#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include <GlobalParams.h>
#include <PDFDoc.h>
#include <Outline.h>
#include <UnicodeMap.h>
#include <goo/GooList.h>
#include <Link.h>

#include "Epdf.h"
#include "epdf_private.h"


static char *unicode_to_char   (Unicode   *unicode,
                                int        len);
static void  epdf_index_fill   (Eina_List *items,
                                GooList   *gitems);
static void  epdf_index_unfill (Eina_List *items);

/* Index item */
Epdf_Index_Item *
epdf_index_item_new ()
{
  Epdf_Index_Item *item;

  item = (Epdf_Index_Item *)malloc (sizeof (Epdf_Index_Item));
  if (!item)
    return NULL;

  item->title = NULL;
  item->action = NULL;
  item->children = NULL;

  return item;
}

void
epdf_index_item_delete (Epdf_Index_Item *item)
{
  if (!item)
    return;

  if (item->title)
    free (item->title);
  if (item->children) {
    Epdf_Index_Item *i;

    while (item->children)
      {
        Epdf_Index_Item *item;

        item = (Epdf_Index_Item *)eina_list_data_get(item->children);
        epdf_index_item_delete (item);
	item->children = eina_list_remove_list(item->children, item->children);
      }
  }
  free (item);
}

const char *
epdf_index_item_title_get (const Epdf_Index_Item *item)
{
  if (!item)
    return NULL;

  return item->title;
}

Eina_List *
epdf_index_item_children_get (const Epdf_Index_Item *item)
{
  if (!item)
    return NULL;

  return item->children;
}

Epdf_Link_Action_Kind
epdf_index_item_action_kind_get (const Epdf_Index_Item *item)
{
  if (!item || !item->action || !item->action->isOk ())
    return EPDF_LINK_ACTION_UNKNOWN;

  switch (item->action->getKind ()) {
  case actionGoTo: return EPDF_LINK_ACTION_GOTO;
  case actionGoToR: return EPDF_LINK_ACTION_GOTO_NEW_FILE;
  case actionLaunch: return EPDF_LINK_ACTION_LAUNCH;
  case actionURI: return EPDF_LINK_ACTION_URI;
  case actionNamed: return EPDF_LINK_ACTION_NAMED;
  case actionMovie: return EPDF_LINK_ACTION_MOVIE;
  default: return EPDF_LINK_ACTION_UNKNOWN;
  }
}

int
epdf_index_item_page_get (const Epdf_Document *document, const Epdf_Index_Item *item)
{
  bool delete_dest = false;

  if (!item || !item->action || !item->action->isOk ())
    return -1;

  if (item->action->getKind () != actionGoTo)
    return -1;

  GooString *named_dest = ((LinkGoTo *)item->action)->getNamedDest ();
  LinkDest *dest = ((LinkGoTo *)item->action)->getDest ();

  if (!dest && named_dest) {
    dest = document->pdfdoc->findDest (named_dest);
    if (dest) delete_dest = true;
  }

  // we really can't find a destination
  if (!dest || !dest->isOk ())
    return -1;

  if (dest->isPageRef ()) {
    int page = document->pdfdoc->findPage (dest->getPageRef ().num, dest->getPageRef ().gen) - 1;
    if (delete_dest) delete dest;
    return page;
  }

  int page = dest->getPageNum () - 1;

  if (delete_dest) delete dest;

  return page;
}

/* Index */

Eina_List *
epdf_index_new (const Epdf_Document *document)
{
  Outline   *outline;
  GooList   *gitems;
  Eina_List *index = NULL;

  if (!document)
    return index;

  outline = document->pdfdoc->getOutline ();
  if (outline == NULL)
    return index;

  gitems = outline->getItems ();
  if (gitems == NULL)
    return index;

  epdf_index_fill (index, gitems);

  return index;
}

void
epdf_index_delete (Eina_List *index)
{
  Eina_List       *items = index;
  Epdf_Index_Item *item;

  if (!index)
    return;

  epdf_index_unfill (index);
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
epdf_index_fill (Eina_List *items,
                 GooList   *gitems)
{
  if (!items || !gitems)
    return;

  for (int i = 0; i < gitems->getLength (); i++) {
    Epdf_Index_Item *item;
    OutlineItem     *oitem = (OutlineItem *)gitems->get (i);
    Unicode         *utitle = oitem->getTitle ();

    item = epdf_index_item_new ();
    item->title = unicode_to_char (utitle, oitem->getTitleLength ());
    item->action = oitem->getAction ();
    oitem->open ();
    if (oitem->hasKids () && oitem->getKids ())
      epdf_index_fill (item->children, oitem->getKids ());
    items = eina_list_append (items, item);
  }
}

static void
epdf_index_unfill (Eina_List *items)
{
  Epdf_Index_Item *item;

  if (!items)
    return;

  while (items)
    {
      Epdf_Index_Item *item;

      item = (Epdf_Index_Item *)eina_list_data_get(items);

      if (item->title)
        free (item->title);

      if (item->children)
        epdf_index_unfill (item->children);

      free (item);

      items = eina_list_remove_list(items, items);
    }
}
