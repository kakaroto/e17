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

xmlSAXHandler ThemeListParser = {
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
   (startDocumentSAXFunc) _start_document_theme_list_cb, /* startDocument */
   (endDocumentSAXFunc) _end_document_theme_list_cb, /* endDocument */
   (startElementSAXFunc) _start_element_theme_list_cb, /* startElement */
   (endElementSAXFunc) _end_element_theme_list_cb, /* endElement */
   0, /* reference */
   (charactersSAXFunc) _chars_theme_list_cb, /* characters */
   0, /* ignorableWhitespace */
   0, /* processingInstruction */
   0, /* comment */
   0, /* warning */
   0, /* error */
   0  /* fatalError */
};

Exchange_Theme *tldata;
Eina_List *tl = NULL;

/**
 * @addtogroup Exchange_Theme_List_Group Exchange Theme Listing Functions
 *
 * Functions that handles theme listing.
 *
 * @{
 */

/**
 * @param user_id The user ID of the author of the theme
 * @param limit The max number of list entries
 * @param offset The starting number of list entries
 * @return The Eina_List * for matched themes, NULL otherwise
 * @brief Get the Eina_List for themes that matches the filter. 
 */
EAPI Eina_List *
exchange_theme_list_filter_by_user_id(int user_id, int limit, int offset)
{
   char text[16];

   if (!user_id)
      return NULL;
   snprintf(text, sizeof(text), "&user_id=%d", user_id);
   if (_theme_list_connect(text, limit, offset) == 0)
      return tl;
   return NULL;
}

/**
 * @param user_name The username of the author of the theme
 * @param limit The max number of list entries
 * @param offset The starting number of list entries
 * @return The Eina_List * for matched themes, NULL otherwise
 * @brief Get the Eina_List for themes that matches the filter. 
 */
EAPI Eina_List *
exchange_theme_list_filter_by_user_name(const char *user_name, int limit, int offset)
{
   char text[1024];

   if (!user_name)
      return NULL;
   snprintf(text, sizeof(text), "&user_name=%s", user_name);
   if (_theme_list_connect(text, limit, offset) == 0)
      return tl;
   return NULL;
}

/**
 * @param group_title The theme_group_title provided by the theme
 * @param limit The max number of list entries
 * @param offset The starting number of list entries
 * @return The Eina_List * for matched themes, NULL otherwise
 * @brief Get the Eina_List for themes that matches the filter. 
 */
EAPI Eina_List *
exchange_theme_list_filter_by_group_title(const char *group_title, int limit, int offset)
{
   char text[1024];

   if (!group_title)
      return NULL;
   snprintf(text, sizeof(text), "&theme_group_title=%s", group_title);
   if (_theme_list_connect(text, limit, offset) == 0)
      return tl;
   return NULL;
}

/**
 * @param group_name The theme_group_name managed by the theme
 * @param limit The max number of list entries
 * @param offset The starting number of list entries
 * @return The Eina_List * for matched themes, NULL otherwise
 * @brief Get the Eina_List for themes that matches the filter. 
 */
EAPI Eina_List *
exchange_theme_list_filter_by_group_name(const char *group_name, int limit, int offset)
{
   char text[1024];

   if (!group_name)
      return NULL;
   snprintf(text, sizeof(text), "&theme_group_name=%s", group_name);
   if (_theme_list_connect(text, limit, offset) == 0)
      return tl;
   return NULL;
}

/**
 * @param application_id The ID of application managed by the theme
 * @param limit The max number of list entries
 * @param offset The starting number of list entries
 * @return The Eina_List * for matched themes, NULL otherwise
 * @brief Get the Eina_List for themes that matches the filter. 
 */
EAPI Eina_List *
exchange_theme_list_filter_by_application_id(int application_id, int limit, int offset)
{
   char text[21];   

   if (!application_id)
      return NULL;
   snprintf(text, sizeof(text), "&application_id=%d", application_id);
   if (_theme_list_connect(text, limit, offset) == 0)
      return tl;
   return NULL;
}

/**
 * @param module_id The ID of module managed by the theme
 * @param limit The max number of list entries
 * @param offset The starting number of list entries
 * @return The Eina_List * for matched themes, NULL otherwise
 * @brief Get the Eina_List for themes that matches the filter. 
 */
EAPI Eina_List *
exchange_theme_list_filter_by_module_id(int module_id, int limit, int offset)
{
   char text[16];

   if (!module_id)
      return NULL;
   snprintf(text, sizeof(text), "&module_id=%d", module_id);
   if (_theme_list_connect(text, limit, offset) == 0)
      return tl;
   return NULL;
}

/**
 * @param limit The max number of list entries
 * @param offset The starting number of list entries
 * @return The Eina_List * for available themes, NULL otherwise
 * @brief Get the Eina_List for all available themes. 
 */
EAPI Eina_List *
exchange_theme_list_all(int limit, int offset)
{
   if (_theme_list_connect(NULL, limit, offset) == 0)
      return tl;
   return NULL;
}

/**
 * @}
 */

/*** Private functions ***/
int
_theme_list_connect(const char *filter, int limit, int offset)
{
   int ret;
   char url[4096];
   char lim[10];
   char off[11];
   Theme_List_Parser state = { 0 };
   
   if (!filter)
      snprintf(url, sizeof(url), "http://exchange.enlightenment.org/api/list?object=theme");
   else
      snprintf(url, sizeof(url), "http://exchange.enlightenment.org/api/list?object=theme%s", filter);
   if (limit > 0)
   {
      snprintf(lim, sizeof(lim), "&limit=%d", limit);
      strcat(url, lim);
   }
   if (offset > 0)
   {
      snprintf(off, sizeof(off), "&offset=%d", offset);
      strcat(url, off);
   }

   tl = NULL;
   xmlInitParser();
   ret = xmlSAXUserParseFile(&ThemeListParser, &state, url);
   xmlCleanupParser();
   return ret;
}

void
_start_document_theme_list_cb(Theme_List_Parser *state)
{
   state->state = PARSER_THEME_LIST_START;
   state->prev_state = PARSER_THEME_LIST_UNKNOWN;
}

void
_end_document_theme_list_cb(Theme_List_Parser *state)
{
   state->state = PARSER_THEME_LIST_UNKNOWN;
   state->prev_state = PARSER_THEME_LIST_START;
}

void
_start_element_theme_list_cb(Theme_List_Parser *state, const xmlChar *name, const xmlChar **attrs)
{
   if (!strcmp((char *)name, "theme"))
   {
      tldata = calloc(1, sizeof(Exchange_Theme));
      state->state = PARSER_THEME_LIST;
      state->prev_state = PARSER_THEME_LIST_START;
   }
   if (!strcmp((char *)name, "name"))
   {
      state->state = PARSER_THEME_LIST_NAME;
      state->prev_state = PARSER_THEME_LIST_START;
   }
   if (!strcmp((char *)name, "id"))
   {
      state->state = PARSER_THEME_LIST_ID;
      state->prev_state = PARSER_THEME_LIST_START;
   }
   if (!strcmp((char *)name, "version"))
   {
      state->state = PARSER_THEME_LIST_VERSION;
      state->prev_state = PARSER_THEME_LIST_START;
   }
   if (!strcmp((char *)name, "description"))
   {
      state->state = PARSER_THEME_LIST_DESCRIPTION;
      state->prev_state = PARSER_THEME_LIST_START;
   }
   if (!strcmp((char *)name, "url"))
   {
      state->state = PARSER_THEME_LIST_URL;
      state->prev_state = PARSER_THEME_LIST_START;
   }
   if (!strcmp((char *)name, "screenshot"))
   {
      state->state = PARSER_THEME_LIST_SCREENSHOT;
      state->prev_state = PARSER_THEME_LIST_START;
   }
   if (!strcmp((char *)name, "thumbnail"))
   {
      state->state = PARSER_THEME_LIST_THUMBNAIL;
      state->prev_state = PARSER_THEME_LIST_START;
   }
   if (!strcmp((char *)name, "rating"))
   {
      state->state = PARSER_THEME_LIST_RATING;
      state->prev_state = PARSER_THEME_LIST_START;
   }
}

void
_end_element_theme_list_cb(Theme_List_Parser *state, const xmlChar *name)
{
   if (!strcmp((char *)name, "theme"))
   {
      tl = eina_list_append(tl, (Exchange_Theme *)tldata);
      state->state = PARSER_THEME_LIST_START;
      state->prev_state = PARSER_THEME_LIST;
   }
   if (!strcmp((char *)name, "name"))
   {
      state->state = PARSER_THEME_LIST_START;
      state->prev_state = PARSER_THEME_LIST_NAME;
   }
   if (!strcmp((char *)name, "id"))
   {
      state->state = PARSER_THEME_LIST_START;
      state->prev_state = PARSER_THEME_LIST_ID;
   }
   if (!strcmp((char *)name, "version"))
   {
      state->state = PARSER_THEME_LIST_START;
      state->prev_state = PARSER_THEME_LIST_VERSION;
   }
   if (!strcmp((char *)name, "description"))
   {
      state->state = PARSER_THEME_LIST_START;
      state->prev_state = PARSER_THEME_LIST_DESCRIPTION;
   }
   if (!strcmp((char *)name, "url"))
   {
      state->state = PARSER_THEME_LIST_START;
      state->prev_state = PARSER_THEME_LIST_URL;
   }
   if (!strcmp((char *)name, "screenshot"))
   {
      state->state = PARSER_THEME_LIST_START;
      state->prev_state = PARSER_THEME_LIST_SCREENSHOT;
   }
   if (!strcmp((char *)name, "thunmbail"))
   {
      state->state = PARSER_THEME_LIST_START;
      state->prev_state = PARSER_THEME_LIST_THUMBNAIL;
   }
   if (!strcmp((char *)name, "rating"))
   {
      state->state = PARSER_THEME_LIST_START;
      state->prev_state = PARSER_THEME_LIST_RATING;
   }
}

void
_chars_theme_list_cb(Theme_List_Parser *state, const xmlChar *chars, int len)
{
   char buf[4096];
   
   switch (state->state)
   {
      case PARSER_THEME_LIST_NAME:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tldata->name = strdup(buf);  //TODO Better to use stringshare here
         break;
      case PARSER_THEME_LIST_ID:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tldata->id = atoi(buf);
      case PARSER_THEME_LIST_VERSION:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tldata->version = strdup(buf);
         break;
      case PARSER_THEME_LIST_DESCRIPTION:
         strncat((char *)tldata->description, (char *)chars, len);
         break;
      case PARSER_THEME_LIST_URL:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tldata->url = strdup(buf);
         break;
      case PARSER_THEME_LIST_SCREENSHOT:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tldata->screenshot = strdup(buf);
         break;
      case PARSER_THEME_LIST_THUMBNAIL:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tldata->thumbnail = strdup(buf);
         break;
      case PARSER_THEME_LIST_RATING:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tldata->rating = atof(buf);
         break;
      default:
         break;
   }
}
