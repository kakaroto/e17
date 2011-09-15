/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "Azy.h"
#include "azy_private.h"

/**
 * @defgroup Azy_Rss RSS Functions
 * @brief Functions which affect #Azy_Rss objects
 * @{
 */

/*
 * @brief Create a new #Azy_Rss object
 *
 * This function creates a new #Azy_Rss object, returning #NULL on failure.
 * @return The new object, or #NULL on failure
 */
Azy_Rss *
azy_rss_new(void)
{
   Azy_Rss *rss;

   rss = calloc(1, sizeof(Azy_Rss));
   EINA_SAFETY_ON_NULL_RETURN_VAL(rss, NULL);

   AZY_MAGIC_SET(rss, AZY_MAGIC_RSS);
   return rss;
}

/**
 * @brief Free an #Azy_Rss_Contact object
 *
 * This function frees an #Azy_Rss_Contact object.
 * @param c The object (NOT #NULL)
 */
void
azy_rss_contact_free(Azy_Rss_Contact *c)
{
   if (!c) return;

   eina_stringshare_del(c->name);
   eina_stringshare_del(c->uri);
   eina_stringshare_del(c->email);
   free(c);
}

/**
 * @brief Free an #Azy_Rss_Link object
 *
 * This function frees an #Azy_Rss_Link object.
 * @param li The object (NOT #NULL)
 */
void
azy_rss_link_free(Azy_Rss_Link *li)
{
   if (!li) return;
   eina_stringshare_del(li->href);
   eina_stringshare_del(li->rel);
   eina_stringshare_del(li->type);
   eina_stringshare_del(li->hreflang);
   eina_stringshare_del(li->title);
   free(li);
}

/**
 * @brief Free an #Azy_Rss object
 *
 * This function frees an #Azy_Rss object and all #Azy_Rss_Item subobjects.
 * @param rss The rss object (NOT #NULL)
 */
void
azy_rss_free(Azy_Rss *rss)
{
   void *item;

   if (!rss) return;
   if (!AZY_MAGIC_CHECK(rss, AZY_MAGIC_RSS))
     {
        AZY_MAGIC_FAIL(rss, AZY_MAGIC_RSS);
        return;
     }

   eina_stringshare_del(rss->title);
   eina_stringshare_del(rss->img_url);
   if (rss->atom)
     {
        eina_stringshare_del(rss->rights);
        eina_stringshare_del(rss->id);
        eina_stringshare_del(rss->logo);
        eina_stringshare_del(rss->generator);
        eina_stringshare_del(rss->subtitle);
        EINA_LIST_FREE(rss->categories, item)
          eina_stringshare_del(item);
        EINA_LIST_FREE(rss->contributors, item)
          azy_rss_contact_free(item);
        EINA_LIST_FREE(rss->authors, item)
          azy_rss_contact_free(item);
        EINA_LIST_FREE(rss->atom_links, item)
          azy_rss_link_free(item);
     }
   else
     {
        eina_stringshare_del(rss->link);
        eina_stringshare_del(rss->desc);
     }
   EINA_LIST_FREE(rss->items, item)
     azy_rss_item_free(item);

   AZY_MAGIC_SET(rss, AZY_MAGIC_NONE);

   free(rss);
}

/**
 * @brief Retrieve the list of items in an rss object
 *
 * This function returns a list of #Azy_Rss_Item objects belonging to @p rss
 * @param rss The #Azy_Rss (NOT #NULL)
 * @return An #Eina_List of #Azy_Rss_Item objects
 */
Eina_List *
azy_rss_items_get(Azy_Rss *rss)
{
   if (!AZY_MAGIC_CHECK(rss, AZY_MAGIC_RSS))
     {
        AZY_MAGIC_FAIL(rss, AZY_MAGIC_RSS);
        return NULL;
     }
   return rss->items;
}

/**
 * @brief Retrieve the list of authors of an rss object
 *
 * This function returns a list of #Azy_Rss_Contact objects belonging to @p rss
 * @param rss The #Azy_Rss (NOT #NULL)
 * @return An #Eina_List of #Azy_Rss_Contact objects
 */
Eina_List *
azy_rss_authors_get(Azy_Rss *rss)
{
   if (!AZY_MAGIC_CHECK(rss, AZY_MAGIC_RSS))
     {
        AZY_MAGIC_FAIL(rss, AZY_MAGIC_RSS);
        return NULL;
     }
   return rss->authors;
}

/**
 * @brief Retrieve the list of contributors of an rss object
 *
 * This function returns a list of #Azy_Rss_Contact objects belonging to @p rss
 * @param rss The #Azy_Rss (NOT #NULL)
 * @return An #Eina_List of #Azy_Rss_Contact objects
 */
Eina_List *
azy_rss_contributors_get(Azy_Rss *rss)
{
   if (!AZY_MAGIC_CHECK(rss, AZY_MAGIC_RSS))
     {
        AZY_MAGIC_FAIL(rss, AZY_MAGIC_RSS);
        return NULL;
     }
   return rss->contributors;
}

/**
 * @brief Retrieve the list of links from an rss object
 *
 * This function returns a list of #Azy_Rss_Link objects belonging to @p rss
 * @param rss The #Azy_Rss (NOT #NULL)
 * @return An #Eina_List of #Azy_Rss_Link objects
 */
Eina_List *
azy_rss_links_get(Azy_Rss *rss)
{
   if (!AZY_MAGIC_CHECK(rss, AZY_MAGIC_RSS))
     {
        AZY_MAGIC_FAIL(rss, AZY_MAGIC_RSS);
        return NULL;
     }
   return rss->atom_links;
}

/**
 * @brief Retrieve the list of categories from an rss object
 *
 * This function returns a list of strings belonging to @p rss
 * @param rss The #Azy_Rss (NOT #NULL)
 * @return An #Eina_List of stringshared strings
 */
Eina_List *
azy_rss_categories_get(Azy_Rss *rss)
{
   if (!AZY_MAGIC_CHECK(rss, AZY_MAGIC_RSS))
     {
        AZY_MAGIC_FAIL(rss, AZY_MAGIC_RSS);
        return NULL;
     }
   return rss->categories;
}

#define DEF(NAME) \
/**
   @brief Retrieve the NAME of an rss object
   This function will return the NAME of @p rss.  The NAME will be stringshared,
   but still belongs to @p rss.
   @param rss The #Azy_Rss object (NOT #NULL)
   @return The NAME, or #NULL on failure
 */                                            \
  const char *                                 \
  azy_rss_##NAME##_get(Azy_Rss * rss)          \
  {                                            \
     if (!AZY_MAGIC_CHECK(rss, AZY_MAGIC_RSS)) \
       {                                       \
          AZY_MAGIC_FAIL(rss, AZY_MAGIC_RSS);  \
          return NULL;                         \
       }                                       \
     return rss->NAME;                         \
  }

DEF(title)
DEF(link)
DEF(img_url)
DEF(desc)
DEF(rights)
DEF(id)
DEF(logo)
DEF(generator)
DEF(subtitle)

#undef DEF

/**
 * @brief Print an rss contact object
 *
 * This function will print an #Azy_Rss_Contact object,
 * optionally indenting @p indent times using @p pre string.
 * @param pre String to indent with
 * @param indent Number of times to indent
 * @param li The rss link object (NOT #NULL)
 */
void
azy_rss_contact_print(const char *pre,
                      int         indent,
                      Azy_Rss_Contact *c)
{
   int i;
   if (!c) return;

   if (!pre) pre = "\t";

#define PRINT(X) do { \
   if (c->X) \
     { \
        for (i = 0; i < indent; i++) \
          printf("%s", pre); \
        printf("%s: %s\n", #X, c->X); \
     } \
} while (0)

   PRINT(name);
   PRINT(uri);
   PRINT(email);
#undef PRINT
}

/**
 * @brief Print an rss link object
 *
 * This function will print an #Azy_Rss_Link object,
 * optionally indenting @p indent times using @p pre string.
 * @param pre String to indent with
 * @param indent Number of times to indent
 * @param li The rss link object (NOT #NULL)
 */
void
azy_rss_link_print(const char *pre,
                   int         indent,
                   Azy_Rss_Link *li)
{
   int i;
   if (!li) return;

   if (!pre) pre = "\t";

#define PRINT(X) do { \
   if (li->X) \
     { \
        for (i = 0; i < indent; i++) \
          printf("%s", pre); \
        printf("%s: %s\n", #X, li->X); \
     } \
} while (0)

   PRINT(title);
   PRINT(href);
   PRINT(rel);
   PRINT(type);
   PRINT(hreflang);
#undef PRINT
   if (li->length)
     {
        for (i = 0; i < indent; i++)
          printf("%s", pre);
        printf("length: %zu\n", li->length);
     }
}

/**
 * @brief Print an rss object
 *
 * This function will print an #Azy_Rss object along with all sub-objects,
 * optionally indenting @p indent times using @p pre string.
 * @param pre String to indent with
 * @param indent Number of times to indent
 * @param rss The rss object (NOT #NULL)
 */
void
azy_rss_print(const char *pre,
              int         indent,
              Azy_Rss    *rss)
{
   int i;
   const char *str;
   Eina_List *l;
   void *item;

   if (!AZY_MAGIC_CHECK(rss, AZY_MAGIC_RSS))
     {
        AZY_MAGIC_FAIL(rss, AZY_MAGIC_RSS);
        return;
     }

   if (!pre) pre = "\t";

#define PRINT(X) do { \
   if (rss->X) \
     { \
        for (i = 0; i < indent; i++) \
          printf("%s", pre); \
        printf("%s: %s\n", #X, rss->X); \
     } \
} while (0)

   PRINT(title);
   PRINT(img_url);
   
   if (rss->atom)
     {
        PRINT(rights);
        PRINT(id);
        PRINT(logo);
        PRINT(generator);
        PRINT(subtitle);
        EINA_LIST_FOREACH(rss->categories, l, str)
          {
             for (i = 0; i < indent; i++)
               printf("%s", pre);
             printf("category: %s\n", str);
          }

#define INDENT(X) do { \
        if (rss->X##s) \
          { \
             for (i = 0; i < indent; i++) \
               printf("%s", pre); \
             printf("%s: \n", #X); \
          } \
} while (0)

        INDENT(contributor);
        EINA_LIST_FOREACH(rss->contributors, l, item)
          {
             azy_rss_contact_print(pre, indent + 1, item);
             if (l->next) printf("\n");
          }
        INDENT(author);
        EINA_LIST_FOREACH(rss->authors, l, item)
          {
             azy_rss_contact_print(pre, indent + 1, item);
             if (l->next) printf("\n");
          }
        INDENT(atom_link);
        EINA_LIST_FOREACH(rss->atom_links, l, item)
          {
             azy_rss_link_print(pre, indent + 1, item);
             if (l->next) printf("\n");
          }
     }
   else
     {
        PRINT(link);
        PRINT(desc);
     }

   INDENT(item);
   EINA_LIST_FOREACH(rss->items, l, item)
     {
        azy_rss_item_print(pre, indent + 1, item);
        if (l->next) printf("\n");
     }
}

/** @} */
