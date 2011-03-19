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
 * @defgroup Azy_Rss_Item RSS item Functions
 * @brief Functions which affect #Azy_Rss_Item objects
 * @{
 */

/**
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
   if (!AZY_MAGIC_CHECK(item, AZY_MAGIC_RSS_ITEM))
     {
        AZY_MAGIC_FAIL(item, AZY_MAGIC_RSS_ITEM);
        return;
     }

   eina_stringshare_del(item->title);
   eina_stringshare_del(item->link);
   eina_stringshare_del(item->desc);
   eina_stringshare_del(item->date);
   eina_stringshare_del(item->guid);
   eina_stringshare_del(item->comment_url);
   eina_stringshare_del(item->author);

   AZY_MAGIC_SET(item, AZY_MAGIC_NONE);

   free(item);
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

#undef DEF

/**
 * @brief Print an rss item object
 *
 * This function will print an #Azy_Rss_Item object, optionally indenting
 * @p indent times using @p pre string.
 * @param pre String to indent with
 * @param indent Number of times to indent
 * @param rss The rss item object (NOT #NULL)
 */
void
azy_rss_item_print(const char   *pre,
                   int           indent,
                   Azy_Rss_Item *item)
{
   int i;

   if (!AZY_MAGIC_CHECK(item, AZY_MAGIC_RSS_ITEM))
     {
        AZY_MAGIC_FAIL(item, AZY_MAGIC_RSS_ITEM);
        return;
     }

   if (!pre) pre = "\t";

   for (i = 0; i < indent; i++)
     printf("%s", pre);
   printf("title: %s\n", item->title);
   for (i = 0; i < indent; i++)
     printf("%s", pre);
   printf("link: %s\n", item->link);
   for (i = 0; i < indent; i++)
     printf("%s", pre);
   printf("description: %s\n", item->desc);
   for (i = 0; i < indent; i++)
     printf("%s", pre);
   printf("date: %s\n", item->date);
   for (i = 0; i < indent; i++)
     printf("%s", pre);
   printf("guid: %s\n", item->guid);
   for (i = 0; i < indent; i++)
     printf("%s", pre);
   printf("comment_url: %s\n", item->comment_url);
   for (i = 0; i < indent; i++)
     printf("%s", pre);
   printf("author: %s\n", item->author);
}

/** @} */
