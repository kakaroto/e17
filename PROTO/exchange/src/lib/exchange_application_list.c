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

xmlSAXHandler ApplicationListParser = {
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
   (startDocumentSAXFunc) _start_document_application_list_cb, /* startDocument */
   (endDocumentSAXFunc) _end_document_application_list_cb, /* endDocument */
   (startElementSAXFunc) _start_element_application_list_cb, /* startElement */
   (endElementSAXFunc) _end_element_application_list_cb, /* endElement */
   0, /* reference */
   (charactersSAXFunc) _chars_application_list_cb, /* characters */
   0, /* ignorableWhitespace */
   0, /* processingInstruction */
   0, /* comment */
   0, /* warning */
   0, /* error */
   0  /* fatalError */
};

Application_List_Data *aldata;
Evas_List *al = NULL;

/**
 * @addtogroup Exchange_Application_List_Group Exchange Application Listing Functions
 *
 * Functions that handles application listing.
 *
 * @{
 */

/**
 * @param user_id The user ID of the author of the theme
 * @param limit The max number of list entries
 * @param offset The starting number of list entries
 * @return The Evas_List * for matched application, NULL otherwise
 * @brief Get the Evas_List for applications that matches the filter. 
 */
EAPI Evas_List *
exchange_application_list_filter_by_user_id(int user_id, int limit, int offset)
{
   char text[16];

   if (!user_id)
      return NULL;
   snprintf(text, sizeof(text), "&user_id=%d", user_id);
   if (_application_list_connect(text, limit, offset) == 0)
      return al;
   return NULL;
}

/**
 * @param user_name The username of the author of the theme
 * @param limit The max number of list entries
 * @param offset The starting number of list entries
 * @return The Evas_List * for matched applications, NULL otherwise
 * @brief Get the Evas_List for applications that matches the filter. 
 */
EAPI Evas_List *
exchange_application_list_filter_by_user_name(const char *user_name, int limit, int offset)
{
   char text[1024];

   if (!user_name)
      return NULL;
   snprintf(text, sizeof(text), "&user_name=%s", user_name);
   if (_application_list_connect(text, limit, offset) == 0)
      return al;
   return NULL;
}

/**
 * @}
 */

/*** Private functions ***/

void
_application_list_free_data(void)
{
   if (al)
   {
      Evas_List *l;
      for (l = al; l; l = l->next)
      {
         Application_List_Data *ald;
         
         ald = l->data;
         if (ald->url)
            free(ald->url);
         if (ald->screenshot)
            free(ald->screenshot);
         free(ald);
         ald = NULL;
      }
      al = evas_list_free(al);
   }
}

int 
_application_list_connect(const char *filter, int limit, int offset)
{
   int ret;
   char url[4096];
   char lim[10];
   char off[11];
   Application_List_Parser state = { 0 };

   if (!filter)
      return -1;
   snprintf(url, sizeof(url), "http://exchange.enlightenment.org/api/list?object=application%s", filter);
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

   xmlInitParser();
   ret = xmlSAXUserParseFile(&ApplicationListParser, &state, url);
   xmlCleanupParser();
   return ret;
}

void
_start_document_application_list_cb(Application_List_Parser *state)
{
   state->state = PARSER_APPLICATION_LIST_START;
   state->prev_state = PARSER_APPLICATION_LIST_UNKNOWN;
}

void
_end_document_application_list_cb(Application_List_Parser *state)
{
   state->state = PARSER_APPLICATION_LIST_UNKNOWN;
   state->prev_state = PARSER_APPLICATION_LIST_START;
}

void
_start_element_application_list_cb(Application_List_Parser *state, const xmlChar *name, const xmlChar **attrs)
{
   if (!strcmp((char *)name, "application"))
   {
      aldata = calloc(1, sizeof(Application_List_Data));
      state->state = PARSER_APPLICATION_LIST;
      state->prev_state = PARSER_APPLICATION_LIST_START;
   }
   if (!strcmp((char *)name, "id"))
   {
      state->state = PARSER_APPLICATION_LIST_ID;
      state->prev_state = PARSER_APPLICATION_LIST_START;
   }
   if (!strcmp((char *)name, "name"))
   {
      state->state = PARSER_APPLICATION_LIST_NAME;
      state->prev_state = PARSER_APPLICATION_LIST_START;
   }
   if (!strcmp((char *)name, "description"))
   {
      state->state = PARSER_APPLICATION_LIST_DESCRIPTION;
      state->prev_state = PARSER_APPLICATION_LIST_START;
   }
   if (!strcmp((char *)name, "url"))
   {
      state->state = PARSER_APPLICATION_LIST_URL;
      state->prev_state = PARSER_APPLICATION_LIST_START;
   }
   if (!strcmp((char *)name, "screenshot"))
   {
      state->state = PARSER_APPLICATION_LIST_SCREENSHOT;
      state->prev_state = PARSER_APPLICATION_LIST_START;
   }
   if (!strcmp((char *)name, "user_id"))
   {
      state->state = PARSER_APPLICATION_LIST_USER_ID;
      state->prev_state = PARSER_APPLICATION_LIST_START;
   }
}

void
_end_element_application_list_cb(Application_List_Parser *state, const xmlChar *name)
{
   if (!strcmp((char *)name, "application"))
   {
      al = evas_list_append(al, (Application_List_Data *)aldata);
      state->state = PARSER_APPLICATION_LIST_START;
      state->prev_state = PARSER_APPLICATION_LIST;
   }
   if (!strcmp((char *)name, "id"))
   {
      state->state = PARSER_APPLICATION_LIST_START;
      state->prev_state = PARSER_APPLICATION_LIST_ID;
   }
   if (!strcmp((char *)name, "name"))
   {
      state->state = PARSER_APPLICATION_LIST_START;
      state->prev_state = PARSER_APPLICATION_LIST_NAME;
   }
   if (!strcmp((char *)name, "description"))
   {
      state->state = PARSER_APPLICATION_LIST_START;
      state->prev_state = PARSER_APPLICATION_LIST_DESCRIPTION;
   }
   if (!strcmp((char *)name, "url"))
   {
      state->state = PARSER_APPLICATION_LIST_START;
      state->prev_state = PARSER_APPLICATION_LIST_URL;
   }
   if (!strcmp((char *)name, "screenshot"))
   {
      state->state = PARSER_APPLICATION_LIST_START;
      state->prev_state = PARSER_APPLICATION_LIST_SCREENSHOT;
   }
   if (!strcmp((char *)name, "user_id"))
   {
      state->state = PARSER_APPLICATION_LIST_START;
      state->prev_state = PARSER_APPLICATION_LIST_USER_ID;
   }
}

void
_chars_application_list_cb(Application_List_Parser *state, const xmlChar *chars, int len)
{
   char buf[4096];

   switch (state->state)
   {
      case PARSER_APPLICATION_LIST_ID:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         aldata->id = atoi(buf);
         break;
      case PARSER_APPLICATION_LIST_NAME:
         strncat((char *)aldata->name, (char *)chars, len);
         break;
      case PARSER_APPLICATION_LIST_DESCRIPTION:
         strncat((char *)aldata->description, (char *)chars, len);
         break;
      case PARSER_APPLICATION_LIST_URL:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         aldata->url = strdup(buf);
         break;
      case PARSER_APPLICATION_LIST_SCREENSHOT:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         aldata->screenshot = strdup(buf);
         break;
      case PARSER_APPLICATION_LIST_USER_ID:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         aldata->user_id = atoi(buf);
         break;
      default:
         break;
   }
}
