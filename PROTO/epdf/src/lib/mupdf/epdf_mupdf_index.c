#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#include <fitz.h>
#include <mupdf.h>

#include "Epdf.h"
#include "epdf_mupdf_private.h"


static void epdf_index_fill(Eina_List **items, pdf_outline *entry);
static void epdf_index_unfill(Eina_List *items);

/* Index item */
Epdf_Index_Item *
epdf_index_item_new()
{
   Epdf_Index_Item *item;

   item = (Epdf_Index_Item*)malloc(sizeof(Epdf_Index_Item));
   if (!item)
     return NULL;

   item->title = NULL;
   item->link = NULL;
   item->children = NULL;

   return item;
}

void
epdf_index_item_delete(Epdf_Index_Item *item)
{
   if (!item)
     return;

   if (item->title)
     free(item->title);
   if (item->children)
     {
        while (item->children)
          {
             Epdf_Index_Item *item;

             item = (Epdf_Index_Item *)eina_list_data_get(item->children);
             epdf_index_item_delete (item);
             item->children = eina_list_remove_list(item->children, item->children);
          }
     }
   free(item);
}

const char *
epdf_index_item_title_get(const Epdf_Index_Item *item)
{
   if (!item)
     return NULL;

   return item->title;
}

Eina_List *
epdf_index_item_children_get(const Epdf_Index_Item *item)
{
   if (!item)
     return NULL;

   return item->children;
}

Epdf_Link_Action_Kind
epdf_index_item_action_kind_get(const Epdf_Index_Item *item)
{
   if (!item || !item->link)
     return EPDF_LINK_ACTION_UNKNOWN;

   if (PDF_LURI == item->link->kind)
     return EPDF_LINK_ACTION_URI;
   else if (PDF_LGOTO == item->link->kind)
     return EPDF_LINK_ACTION_GOTO;
}

int
epdf_index_item_page_get(const Epdf_Document *doc, const Epdf_Index_Item *item)
{
   fz_obj *dest;
   int p;
   int n;
   int g;

   if (!item || !item->link)
     return -1;

   if (PDF_LGOTO != item->link->kind)
     return -1;

   dest = item->link->dest;
   p = 0;
   if (fz_isint(dest))
     {
        p = fz_toint(dest);
        return p;
     }
   if (fz_isdict(dest))
     {
        /* The destination is linked from a Go-To action's D array */
       fz_obj *D;

       D = fz_dictgets(dest, "D");
       if (D && fz_isarray(D))
         dest = fz_arrayget(D, 0);
     }

   n = fz_tonum(dest);
   g = fz_togen(dest);

   for (p = 1; p <= epdf_document_page_count_get(doc); p++)
     {
        fz_obj *page;
        int np;
        int gp;

        page = pdf_getpageobject(doc->xref, p);
        if (!page)
          continue;

        np = fz_tonum(page);
        gp = fz_togen(page);
        if (n == np && g == gp)
          return p-1;
    }

   return 0;
}

/* Index */

Eina_List *
epdf_index_new(const Epdf_Document *doc)
{
   Eina_List *index = NULL;

   if (!doc)
     return index;

   if (!doc->outline)
     return index;

   epdf_index_fill(&index, doc->outline);

   return index;
}

void
epdf_index_delete(Eina_List *index)
{
   if (!index)
     return;

   epdf_index_unfill(index);
}

static void
epdf_index_fill(Eina_List **items, pdf_outline *entry)
{
   Epdf_Index_Item *item;

   item = epdf_index_item_new();
   item->title = entry->title;
   item->link = entry->link;

   *items = eina_list_append (*items, item);

   if (entry->child)
     epdf_index_fill(&item->children, entry->child);

   if (entry->next)
     epdf_index_fill(items, entry->next);
}

static void
epdf_index_unfill(Eina_List *items)
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
