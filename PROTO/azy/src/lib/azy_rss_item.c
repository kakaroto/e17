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

#include "azy_private.h"

/**
 * @defgroup Azy_Rss_Item RSS item Functions
 * @brief Functions which affect #Azy_Rss_Item objects
 * @{
 */

/*
 * @brief Create a new #Azy_Rss_Item object
 *
 * This function creates a new #Azy_Rss_Item object, returning #NULL on failure.
 * @return The new object, or #NULL on failure
 */
Azy_Rss_Item *
azy_rss_item_new(void)
{
   Azy_Rss_Item *item;

   item = calloc(1, sizeof(Azy_Rss_Item));
   EINA_SAFETY_ON_NULL_RETURN_VAL(item, NULL);

   AZY_MAGIC_SET(item, AZY_MAGIC_RSS_ITEM);
   return item;
}

/**
 * @brief Free an #Azy_Rss_Item object
 *
 * This function frees an #Azy_Rss_Item object
 * @param item The rss item object (NOT #NULL)
 */
void
azy_rss_item_free(Azy_Rss_Item *item)
{
   void *d;

   if (!item) return;
   if (!AZY_MAGIC_CHECK(item, AZY_MAGIC_RSS_ITEM))
     {
        AZY_MAGIC_FAIL(item, AZY_MAGIC_RSS_ITEM);
        return;
     }

   eina_stringshare_del(item->title);
   if (item->atom)
     {
        eina_stringshare_del(item->rights);
        eina_stringshare_del(item->summary);
        eina_stringshare_del(item->id);
        eina_stringshare_del(item->icon);
        EINA_LIST_FREE(item->categories, d)
          eina_stringshare_del(d);
        EINA_LIST_FREE(item->contributors, d)
          azy_rss_contact_free(d);
        EINA_LIST_FREE(item->authors, d)
          azy_rss_contact_free(d);
        EINA_LIST_FREE(item->atom_links, d)
          azy_rss_link_free(d);
     }
   else
     {   
        eina_stringshare_del(item->link);
        eina_stringshare_del(item->desc);
        eina_stringshare_del(item->date);
        eina_stringshare_del(item->guid);
        eina_stringshare_del(item->comment_url);
        eina_stringshare_del(item->author);
     }
   AZY_MAGIC_SET(item, AZY_MAGIC_NONE);

   free(item);
}

/**
 * @brief Retrieve the list of authors of an item object
 *
 * This function returns a list of #Azy_Rss_Contact objects belonging to @p item
 * @param item The #Azy_Rss_Item (NOT #NULL)
 * @return An #Eina_List of #Azy_Rss_Contact objects
 */
Eina_List *
azy_rss_item_authors_get(Azy_Rss_Item *item)
{
   if (!AZY_MAGIC_CHECK(item, AZY_MAGIC_RSS_ITEM))
     {
        AZY_MAGIC_FAIL(item, AZY_MAGIC_RSS_ITEM);
        return NULL;
     }
   return item->authors;
}

/**
 * @brief Retrieve the list of contributors of an item object
 *
 * This function returns a list of #Azy_Rss_Contact objects belonging to @p item
 * @param item The #Azy_Rss_Item (NOT #NULL)
 * @return An #Eina_List of #Azy_Rss_Contact objects
 */
Eina_List *
azy_rss_item_contributors_get(Azy_Rss_Item *item)
{
   if (!AZY_MAGIC_CHECK(item, AZY_MAGIC_RSS_ITEM))
     {
        AZY_MAGIC_FAIL(item, AZY_MAGIC_RSS_ITEM);
        return NULL;
     }
   return item->contributors;
}

/**
 * @brief Retrieve the list of links from an item object
 *
 * This function returns a list of #Azy_Rss_Link objects belonging to @p item
 * @param item The #Azy_Rss_Item (NOT #NULL)
 * @return An #Eina_List of #Azy_Rss_Link objects
 */
Eina_List *
azy_rss_item_links_get(Azy_Rss_Item *item)
{
   if (!AZY_MAGIC_CHECK(item, AZY_MAGIC_RSS_ITEM))
     {
        AZY_MAGIC_FAIL(item, AZY_MAGIC_RSS_ITEM);
        return NULL;
     }
   return item->atom_links;
}

/**
 * @brief Retrieve the list of categories from an item object
 *
 * This function returns a list of strings belonging to @p item
 * @param item The #Azy_Rss_Item (NOT #NULL)
 * @return An #Eina_List of stringshared strings
 */
Eina_List *
azy_rss_item_categories_get(Azy_Rss_Item *item)
{
   if (!AZY_MAGIC_CHECK(item, AZY_MAGIC_RSS_ITEM))
     {
        AZY_MAGIC_FAIL(item, AZY_MAGIC_RSS_ITEM);
        return NULL;
     }
   return item->categories;
}

#define DEF(NAME) \
/**
   @brief Retrieve the NAME of an rss item object
   This function will return the NAME of @p item.  The NAME will be stringshared,
   but still belongs to @p item.
   @param item The #Azy_Rss object (NOT #NULL)
   @return The NAME, or #NULL on failure
 */                                                  \
  const char *                                       \
  azy_rss_item_##NAME##_get(Azy_Rss_Item * item)     \
  {                                                  \
     if (!AZY_MAGIC_CHECK(item, AZY_MAGIC_RSS_ITEM)) \
       {                                             \
          AZY_MAGIC_FAIL(item, AZY_MAGIC_RSS_ITEM);  \
          return NULL;                               \
       }                                             \
     return item->NAME;                              \
  }

DEF(title)
DEF(link)
DEF(desc)
DEF(date)
DEF(guid)
DEF(comment_url)
DEF(author)
DEF(rights)
DEF(summary)
DEF(id)
DEF(icon)

#undef DEF

/**
 * @brief Print an rss item object
 *
 * This function will print an #Azy_Rss_Item object, optionally indenting
 * @p indent times using @p pre string.
 * @param pre String to indent with
 * @param indent Number of times to indent
 * @param item The rss item object (NOT #NULL)
 */
void
azy_rss_item_print(const char   *pre,
                   int           indent,
                   Azy_Rss_Item *item)
{
   int i;
   Eina_List *l;
   void *it;

   if (!AZY_MAGIC_CHECK(item, AZY_MAGIC_RSS_ITEM))
     {
        AZY_MAGIC_FAIL(item, AZY_MAGIC_RSS_ITEM);
        return;
     }

   if (!pre) pre = "\t";

#define PRINT(X) do { \
   if (item->X) \
     { \
        for (i = 0; i < indent; i++) \
          printf("%s", pre); \
        printf("%s: %s\n", #X, item->X); \
     } \
} while (0)

   PRINT(title);
   if (item->atom)
     {
        char buf[256];
        const char *str;
        PRINT(rights);
        PRINT(id);
        PRINT(summary);
        PRINT(icon);

        strftime(buf, sizeof(buf), "%FT%TZ", &item->updated);
        for (i = 0; i < indent; i++)
          printf("%s", pre);
        printf("updated: %s\n", buf);

        strftime(buf, sizeof(buf), "%FT%TZ", &item->published);
        for (i = 0; i < indent; i++)
          printf("%s", pre);
        printf("published: %s\n", buf);
        
        EINA_LIST_FOREACH(item->categories, l, str)
          {
             for (i = 0; i < indent; i++)
               printf("%s", pre);
             printf("category: %s\n", str);
          }

#define INDENT(X) do { \
        if (item->X##s) \
          { \
             for (i = 0; i < indent; i++) \
               printf("%s", pre); \
             printf("%s: \n", #X); \
          } \
} while (0)

        INDENT(contributor);
        EINA_LIST_FOREACH(item->contributors, l, it)
          {
             azy_rss_contact_print(pre, indent + 1, it);
             if (l->next) printf("\n");
          }
        INDENT(author);
        EINA_LIST_FOREACH(item->authors, l, it)
          {
             azy_rss_contact_print(pre, indent + 1, it);
             if (l->next) printf("\n");
          }
        INDENT(atom_link);
        EINA_LIST_FOREACH(item->atom_links, l, it)
          {
             azy_rss_link_print(pre, indent + 1, it);
             if (l->next) printf("\n");
          }
     }
   else
     {
        PRINT(link);
        PRINT(desc);
        PRINT(date);
        PRINT(guid);
        PRINT(comment_url);
        PRINT(author);
     }
}

/** @} */
