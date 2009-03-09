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

#include <string.h>
#include <Ecore_File.h>
#include <stdlib.h>
#include "Exchange.h"


xmlSAXHandler AsyncListParser = {
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
   (startDocumentSAXFunc) _start_document_async_list_cb, /* startDocument */
   (endDocumentSAXFunc) _end_document_async_list_cb, /* endDocument */
   (startElementSAXFunc) _start_element_async_list_cb, /* startElement */
   (endElementSAXFunc) _end_element_async_list_cb, /* endElement */
   0, /* reference */
   (charactersSAXFunc) _chars_async_list_cb, /* characters */
   0, /* ignorableWhitespace */
   0, /* processingInstruction */
   0, /* comment */
   0, /* warning */
   0, /* error */
   0  /* fatalError */
};

void
_start_document_async_list_cb(Async_List_Parser *state)
{
   state->state = PARSER_LIST_START;
   state->prev_state = PARSER_UNKNOWN;
}

void
_end_document_async_list_cb(Async_List_Parser *state)
{
   state->state = PARSER_UNKNOWN;
   state->prev_state = PARSER_START;
}

void
_start_element_async_list_cb(Async_List_Parser *state, const xmlChar *name, const xmlChar **attrs)
{
   if (!strcmp((char *)name, "theme") ||
       !strcmp((char *)name, "application") ||
       !strcmp((char *)name, "module"))
   {
      state->current = calloc(1, sizeof(Exchange_Object));
      state->state = PARSER_LIST;
      state->prev_state = PARSER_LIST_START;
   }
   if (!strcmp((char *)name, "name"))
   {
      state->state = PARSER_NAME;
      state->prev_state = PARSER_LIST_START;
   }
   if (!strcmp((char *)name, "id"))
   {
      state->state = PARSER_ID;
      state->prev_state = PARSER_LIST_START;
   }
   if (!strcmp((char *)name, "version"))
   {
      state->state = PARSER_VERSION;
      state->prev_state = PARSER_LIST_START;
   }
   if (!strcmp((char *)name, "description"))
   {
      state->state = PARSER_DESCRIPTION;
      state->prev_state = PARSER_LIST_START;
   }
   if (!strcmp((char *)name, "url"))
   {
      state->state = PARSER_URL;
      state->prev_state = PARSER_LIST_START;
   }
   if (!strcmp((char *)name, "screenshot"))
   {
      state->state = PARSER_SCREENSHOT;
      state->prev_state = PARSER_LIST_START;
   }
   if (!strcmp((char *)name, "thumbnail"))
   {
      state->state = PARSER_THUMBNAIL;
      state->prev_state = PARSER_LIST_START;
   }
   if (!strcmp((char *)name, "rating"))
   {
      state->state = PARSER_RATING;
      state->prev_state = PARSER_LIST_START;
   }
}

void
_end_element_async_list_cb(Async_List_Parser *state, const xmlChar *name)
{
   if (!strcmp((char *)name, "theme") ||
       !strcmp((char *)name, "application") ||
       !strcmp((char *)name, "module"))
   {
      state->l = eina_list_append(state->l, state->current);
      state->state = PARSER_START;
      state->prev_state = PARSER_LIST;
   }
   if (!strcmp((char *)name, "name"))
   {
      state->state = PARSER_LIST_START;
      state->prev_state = PARSER_NAME;
   }
   if (!strcmp((char *)name, "id"))
   {
      state->state = PARSER_LIST_START;
      state->prev_state = PARSER_ID;
   }
   if (!strcmp((char *)name, "version"))
   {
      state->state = PARSER_LIST_START;
      state->prev_state = PARSER_VERSION;
   }
   if (!strcmp((char *)name, "description"))
   {
      state->state = PARSER_LIST_START;
      state->prev_state = PARSER_DESCRIPTION;
   }
   if (!strcmp((char *)name, "url"))
   {
      state->state = PARSER_LIST_START;
      state->prev_state = PARSER_URL;
   }
   if (!strcmp((char *)name, "screenshot"))
   {
      state->state = PARSER_LIST_START;
      state->prev_state = PARSER_SCREENSHOT;
   }
   if (!strcmp((char *)name, "thunmbail"))
   {
      state->state = PARSER_LIST_START;
      state->prev_state = PARSER_THUMBNAIL;
   }
   if (!strcmp((char *)name, "rating"))
   {
      state->state = PARSER_LIST_START;
      state->prev_state = PARSER_RATING;
   }
}

void
_chars_async_list_cb(Async_List_Parser *state, const xmlChar *chars, int len)
{
   char buf[4096];
   
   switch (state->state)
   {
      case PARSER_NAME:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         printf("name: %s\n", buf);
         state->current->name = eina_stringshare_add(buf);
         break;
      case PARSER_ID:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         state->current->id = atoi(buf);
         break;
      case PARSER_VERSION:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         state->current->version = strdup(buf);
         break;
      case PARSER_DESCRIPTION:
         strncat((char *)state->current->description, (char *)chars, len);
         break;
      case PARSER_URL:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         state->current->url = strdup(buf);
         break;
      case PARSER_SCREENSHOT:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         state->current->screenshot = strdup(buf);
         break;
      case PARSER_THUMBNAIL:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         state->current->thumbnail = strdup(buf);
         break;
      case PARSER_RATING:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         state->current->rating = atof(buf);
         break;
      default:
         break;
   }
}

/******************************************************/
void
_download_complete_cb(void *data, const char *file, int status)
{
   Async_List_Parser *state = data;
   int ret;
   printf("DOWNLOAD COMPLETE (status %d)\n", status);
   //TODO check status ??
   
   xmlInitParser();
   ret = xmlSAXUserParseFile(&AsyncListParser, state, state->tmp);
   xmlCleanupParser();
   
   printf("END %d\n", eina_list_count(state->l));
   state->complete_cb(state->l, state->cb_data);
   
   
   ecore_file_unlink(state->tmp);
   
   free(state->url);
   free(state->tmp);
   //free(state); FIXME
}






EAPI unsigned char
exchange_remote_list(const char *group_title,
                     const char *group_name,
                     int application_id, int module_id,
                     int user_id, const char *user_name,
                     int limit, int offset,
                     Exchange_Complete_CB complete_cb, void *data)
{
   Async_List_Parser *state;
   char url[4096];
   char buf[1024];
   int ret;
   char f_templ[] = "/tmp/exchXXXXXX";

   printf("GET THEMES LIST\n");
   
   state = malloc(sizeof(Async_List_Parser));
   if (!state) return 0;
   
   //TODO check internet connection here
   
   snprintf(url, sizeof(url), "http://exchange.enlightenment.org/api/list?object=");

   if (group_title && !strcmp(group_title, "Applications"))
      strcat(url, "application");
   else if (group_title && !strcmp(group_title, "Modules"))
      strcat(url, "module");
   else
   {
      strcat(url, "theme");
      if (group_title)
      {
         strcat(url, "&theme_group_title=");
         strcat(url, group_title);
      }
   }

   if (group_name)
   {
      strcat(url, "&theme_group_name=");
      strcat(url, group_name);
   }

   if (application_id)
   {
      snprintf(buf, sizeof(buf), "&application_id=%d", application_id);
      strcat(url, buf);
   }

   if (module_id)
   {
      snprintf(buf, sizeof(buf), "&module_id=%d", module_id);
      strcat(url, buf);
   }

   if (user_id)
   {
      snprintf(buf, sizeof(buf), "&user_id=%d", user_id);
      strcat(url, buf);
   }

   if (user_name)
   {
      strcat(url, "&user_name=");
      strcat(url, user_name);
   }

   if (limit)
   {
      snprintf(buf, sizeof(buf), "&limit=%d", limit);
      strcat(url, buf);
   }

   if (offset)
   {
      snprintf(buf, sizeof(buf), "&offset=%d", offset);
      strcat(url, buf);
   }

   state->url = strdup(url);
   state->tmp = strdup(mktemp(f_templ));
   state->complete_cb = complete_cb;
   state->cb_data = data;
   state->state = PARSER_LIST_START;
   state->prev_state = PARSER_LIST_START;
   state->l = NULL;
   state->current = NULL;
   
   printf("URL: %s\n", state->url);
   printf("TMP: %s\n", state->tmp);

   ret = ecore_file_download(state->url, state->tmp, _download_complete_cb, NULL, state);
   if (!ret) return 0;
   //TODO start a timer for timeout control

   return 1;
}


