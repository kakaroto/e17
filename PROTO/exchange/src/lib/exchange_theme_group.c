/* EXCHANGE - a library to interact with exchange.enlightenment.org
 * Copyright (C) 2008 Massimiliano Calamelli
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
 * License along with this library;
 * if not, see <http://www.gnu.org/licenses/>.
 */

#include "Exchange.h"

#include <string.h>

xmlSAXHandler ThemeGroupParser = {
   0, /* internalSubset */
   0, /* isStandalone */
   0, /* hasInternalSubset */
   0, /* hasExternalSubset */
   0, /* resolveEntity */
   0, /* getEntity */
   0, /* entityDecl */
   0, /* notationDecl */
   0, /* attributeDecl */
   0, /* elementDecl */
   0, /* unparsedEntityDecl */
   0, /* setDocumentLocator */
   (startDocumentSAXFunc) _start_document_theme_group_cb, /* startDocument */
   (endDocumentSAXFunc) _end_document_theme_group_cb, /* endDocument */
   (startElementSAXFunc) _start_element_theme_group_cb, /* startElement */
   (endElementSAXFunc) _end_element_theme_group_cb, /* endElement */
   0, /* reference */
   (charactersSAXFunc) _chars_theme_group_cb, /* characters */
   0, /* ignorableWhitespace */
   0, /* processingInstruction */
   0, /* comment */
   0, /* warning */
   0, /* error */
   0  /* fatalError */
};

Theme_Group_Data *tgdata;
Eina_List *fel = NULL;

/**
 * @addtogroup Exchange_Theme_Group_Group Exchange Theme_Group Functions
 *
 * Function for theme_groups.
 *
 * Sample code
 *
 * @code
 * Eina_List *l, *l1;
 * l = exchange_theme_group_list_available();
 * printf("Available theme_groups (%d):\n", eina_list_count(l));
 * for (l1 = l; l1; l1 = eina_list_next(l1))
 * {
 *   if (l1->data)
 *   {
 *     Theme_Group_Data *tgd;
 *     tgd = (Theme_Group_Data *)l1->data;
 *     printf("[Name] %s, [Title] %s\n", (char *)tgd->name, (char *)tgd->title);
 *   }
 * }
 * @endcode
 *
 * @{
 */

/**
 * @return An Eina_List *, NULL otherwise. The list contains the available theme_gropus
 * @brief Get a list of available theme_groups.
 */
EAPI Eina_List *
exchange_theme_group_list_available(void)
{
   if (_theme_group_connect() == 0)
      return fel;
   else
      return NULL;
}

/**
 * @}
 */

/*** Private functions ***/

void
_theme_group_free_data(void)
{
   if (fel)
   {
      Eina_List *l;
      for (l = fel; l; l = l->next)
      {
         Theme_Group_Data *tgd;
         
         tgd = l->data;
         free(tgd);
         tgd = NULL;
      }
      fel = eina_list_free(fel);
   }
}

int
_theme_group_connect(void)
{
   Theme_Group_Parser state = { 0 };
   char url[4096];
   int ret;

   xmlInitParser();
   snprintf(url, sizeof(url), "http://exchange.enlightenment.org/api/list?object=theme_group");
   //snprintf(url, sizeof(url), "http://localhost/theme_group.xml");
   ret = xmlSAXUserParseFile(&ThemeGroupParser, &state, url);
   xmlCleanupParser();
   return ret;
}

void
_start_document_theme_group_cb(Theme_Group_Parser *state)
{
   state->state = PARSER_START;
   state->prev_state = PARSER_UNKNOWN;
}

void
_end_document_theme_group_cb(Theme_Group_Parser *state)
{
   state->state = PARSER_UNKNOWN;
   state->prev_state = PARSER_START;
}

void
_start_element_theme_group_cb(Theme_Group_Parser *state, const xmlChar *name, const xmlChar **attrs)
{
   if (!strcmp((char *)name, "theme_group"))
   {
      tgdata = calloc(1, sizeof(Theme_Group_Data));
      state->state = PARSER_THEME_GROUP;
      state->prev_state = PARSER_THEME_GROUP_START;      
   }
   if (!strcmp((char *)name, "name"))
   {
      state->state = PARSER_NAME;
      state->prev_state = PARSER_THEME_GROUP_START;
   }
   if (!strcmp((char *)name, "title"))
   {
      state->state = PARSER_TITLE;
      state->prev_state = PARSER_THEME_GROUP_START;
   }   
   if (!strcmp((char *)name, "known"))
   {
      state->state = PARSER_KNOWN;
      state->prev_state = PARSER_THEME_GROUP_START;
   }
}

void
_end_element_theme_group_cb(Theme_Group_Parser *state, const xmlChar *name)
{
   if (!strcmp((char *)name, "theme_group"))
   {
      if (tgdata->known == 1)
      {
         fel = eina_list_append(fel, (Theme_Group_Data *)tgdata);
      }
      else
      {
         free(tgdata);
         tgdata = NULL;
      }
      state->state = PARSER_THEME_GROUP_START;
      state->prev_state = PARSER_THEME_GROUP;
   }
   if (!strcmp((char *)name, "name"))
   {
      state->state = PARSER_THEME_GROUP_START;
      state->prev_state = PARSER_NAME;
   }
   if (!strcmp((char *)name, "title"))
   {
      state->state = PARSER_THEME_GROUP_START;
      state->prev_state = PARSER_TITLE;
   }   
   if (!strcmp((char *)name, "known"))
   {
      state->state = PARSER_THEME_GROUP_START;
      state->prev_state = PARSER_KNOWN;
   }
}

void
_chars_theme_group_cb(Theme_Group_Parser *state, const xmlChar *chars, int len)
{
   char buf[4096];
   static char name[4096];
   static char title[4096];

   switch (state->state)
   {
      case PARSER_NAME:
         name[0] = '\0';
         strncat((char *)name, (char *)chars, len);
         break;
      case PARSER_TITLE:
         title[0] = '\0';
         strncat((char *)title, (char *)chars, len);
         break;
      case PARSER_KNOWN:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tgdata->known = atoi(buf);
         if (atoi(buf) == 1)
         {
            strcpy(tgdata->name, name);
            strcpy(tgdata->title, title);
         }
         break;
      default:
         break;
   }
}
