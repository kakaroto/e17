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

/**
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
 * @brief Free an #Azy_Rss object
 *
 * This function frees an #Azy_Rss object and all #Azy_Rss_Item subobjects.
 * @param rss The rss object (NOT #NULL)
 */
void
azy_rss_free(Azy_Rss *rss)
{
   Azy_Rss_Item *item;
   
   if (!AZY_MAGIC_CHECK(rss, AZY_MAGIC_RSS))
     {
        AZY_MAGIC_FAIL(rss, AZY_MAGIC_RSS);
        return;
     }

   eina_stringshare_del(rss->title);
   eina_stringshare_del(rss->link);
   eina_stringshare_del(rss->img_url);
   eina_stringshare_del(rss->desc);

   EINA_LIST_FREE(rss->items, item)
     azy_rss_item_free(item);

   AZY_MAGIC_SET(rss, AZY_MAGIC_NONE);

   free(rss);
}

/**
 * @brief Retrieve the list of items in an rss object
 *
 * This function returns a list of #Azy_Rss_Item objects belonging to @p rss
 * @param rss The #Azy_Rss_Object (NOT #NULL)
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

#define DEF(NAME) \
/**
@brief Retrieve the NAME of an rss object
This function will return the NAME of @p rss.  The NAME will be stringshared,
but still belongs to @p rss.
@param rss The #Azy_Rss object (NOT #NULL)
@return The NAME, or #NULL on failure
 */ \
const char * \
azy_rss_##NAME##_get(Azy_Rss *rss) \
{ \
   if (!AZY_MAGIC_CHECK(rss, AZY_MAGIC_RSS)) \
     { \
        AZY_MAGIC_FAIL(rss, AZY_MAGIC_RSS); \
        return NULL; \
     } \
   return rss->NAME; \
}

DEF(title)
DEF(link)
DEF(img_url)
DEF(desc)

#undef DEF

/**
 * @brief Print an rss object
 *
 * This function will print an #Azy_Rss object along with all sub-item #Azy_Rss_Item objects,
 * optionally indenting @p indent times using @p pre string.
 * @param pre String to indent with
 * @param indent Number of times to indent
 * @param rss The rss object (NOT #NULL)
 */
void
azy_rss_print(const char *pre, int indent, Azy_Rss *rss)
{
   int i;
   Eina_List *l;
   Azy_Rss_Item *item;

   if (!AZY_MAGIC_CHECK(rss, AZY_MAGIC_RSS))
     {
        AZY_MAGIC_FAIL(rss, AZY_MAGIC_RSS);
        return;
     }
   
   if (!pre) pre = "\t";

   for (i = 0; i < indent; i++)
     printf("%s", pre);
   printf("title: %s\n", rss->title);
   for (i = 0; i < indent; i++)
     printf("%s", pre);
   printf("link: %s\n", rss->link);
   for (i = 0; i < indent; i++)
     printf("%s", pre);
   printf("img_url: %s\n", rss->img_url);
   for (i = 0; i < indent; i++)
     printf("%s", pre);
   printf("description: %s\n", rss->desc);
   
   EINA_LIST_FOREACH(rss->items, l, item)
     {
        azy_rss_item_print(pre, indent + 1, item);
        if (l->next) printf("\n");
     }
}
/** @} */
