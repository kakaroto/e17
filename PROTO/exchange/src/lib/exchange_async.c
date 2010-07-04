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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
# include <io.h>
#else
# include <unistd.h>
#endif

#include "Exchange.h"
#include "exchange_private.h"


typedef enum _Exchange_Download_Type Exchange_Download_Type;
enum _Exchange_Download_Type{
   DOWNLOAD_THUMBNAIL,
   DOWNLOAD_SCREENSHOT,
   DOWNLOAD_ENTRY
};

typedef struct _Exchange_Download_Data Exchange_Download_Data;
struct _Exchange_Download_Data {
   Exchange_Object *obj;
   Exchange_Download_Type type;
   Exchange_Download_CB *cb;
   void *data;
   const char *dst;
};


/********************   SAX PARSER   **********************************/
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
         //EINA_ERROR_PDBG("name: %s\n", buf);
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

/********************   Internals   **********************************/

static void
_query_download_complete_cb(void *data, const char *file, int status)
{
   Async_List_Parser *state = data;

   //EINA_ERROR_PDBG("DOWNLOAD COMPLETE (status %d)\n", status);

   if (status == 0)
   {
      xmlInitParser();
      xmlSAXUserParseFile(&AsyncListParser, state, state->tmp);
   }

   state->query_cb(state->l, state->cb_data);
   ecore_file_unlink(state->tmp);

   free(state->url);
   free(state->tmp);
   free(state);
}

/********************   Public API   **********************************/

EAPI Ecore_File_Download_Job *
exchange_query(const char *group_title, const char *group_name,
               int application_id, int module_id,
               int user_id, const char *user_name,
               int limit, int offset,
               Exchange_Query_CB query_cb, void *data)
{
   Async_List_Parser *state;
   char url[4096];
   char buf[32];
   char tmpf[] = "/tmp/exchXXXXXX";
   Ecore_File_Download_Job *job = NULL;

   state = malloc(sizeof(Async_List_Parser));
   if (!state) return NULL;
   if (!mktemp(tmpf)) return NULL;

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

   ecore_file_download(url, tmpf, _query_download_complete_cb,
                       NULL, state, &job);
   if (!job)
   {
      if (state) free(state);
      return NULL;
   }

   state->url = strdup(url);
   state->tmp = strdup(tmpf);
   state->query_cb = query_cb;
   state->cb_data = data;
   state->state = PARSER_LIST_START;
   state->prev_state = PARSER_LIST_START;
   state->l = NULL;
   state->current = NULL;

   //EINA_ERROR_PDBG("URL: %s\n", state->url);
   //EINA_ERROR_PDBG("TMP: %s\n", state->tmp);
   //TODO start a timer for timeout control
   return job;
}

EAPI int
exchange_obj_id_get(Exchange_Object *obj)
{
   return obj ? obj->id : -1;
}

EAPI const char *
exchange_obj_name_get(Exchange_Object *obj)
{
   return obj ? obj->name : NULL;
}

EAPI const char *
exchange_obj_file_name_get(Exchange_Object *obj)
{
   char buf[1024];

   if (!obj) return NULL;
   if (obj->file_name) return obj->file_name;

   if (obj->name)
   {
      /* Exchange.org dont report the original file name...
       * ...assume its an edje file */
      snprintf(buf, sizeof(buf), "%s.edj", obj->name);
      obj->file_name = eina_stringshare_add(buf);
      return obj->file_name;
   }
   return NULL;
}

EAPI const char *
exchange_obj_author_get(Exchange_Object *obj)
{
   return obj ? obj->author : NULL;
}

EAPI const char *
exchange_obj_license_get(Exchange_Object *obj)
{
   return obj ? obj->license : NULL;
}

EAPI const char *
exchange_obj_version_get(Exchange_Object *obj)
{
   return obj ? obj->version : NULL;
}

EAPI const char *
exchange_obj_description_get(Exchange_Object *obj)
{
   return obj ? obj->description : NULL;
}

void
_download_complete_cb(void *data, const char *file, int status)
{
   Exchange_Download_Data *dd = data;

   if (!dd || !dd->obj) return;

   if (dd->type == DOWNLOAD_THUMBNAIL)
      dd->obj->job_thumbnail = NULL;
   else if (dd->type == DOWNLOAD_SCREENSHOT)
      dd->obj->job_screenshot = NULL;
   else if (dd->type == DOWNLOAD_ENTRY)
      dd->obj->job_entry = NULL;

   if (status == 0 && dd->dst)
   {
      if (ecore_file_exists(dd->dst))
         ecore_file_unlink(dd->dst);
      ecore_file_mv(file, dd->dst);
      if (dd->cb) dd->cb(dd->obj, dd->dst, dd->data);
   }

   if (dd->dst) eina_stringshare_del(dd->dst);
   free(dd);
}

EAPI Eina_Bool
exchange_obj_thumbnail_get(Exchange_Object *obj, Exchange_Download_CB *cb, void *data)
{
   Exchange_Download_Data *dd;
   char dst[4096];
   char tmpf[] = "/tmp/exchXXXXXX";

   if (!obj || !cb || !obj->thumbnail || obj->id < 0 || !mktemp(tmpf))
      return EINA_FALSE;

   if (obj->job_thumbnail) return EINA_FALSE;
   
   if (strstr(obj->thumbnail, "/files/module/"))
      snprintf(dst, sizeof(dst), "%s/module.%d.thumb.png",
                    exchange_cache_dir_get(), obj->id);
   else if (strstr(obj->thumbnail, "/files/application/"))
      snprintf(dst, sizeof(dst), "%s/application.%d.thumb.png",
                    exchange_cache_dir_get(), obj->id);
   else if (strstr(obj->thumbnail, "/files/theme/"))
      snprintf(dst, sizeof(dst), "%s/theme.%d.thumb.png",
                    exchange_cache_dir_get(), obj->id);
   else
      snprintf(dst, sizeof(dst), "%s/unknow.%d.thumb.png",
                    exchange_cache_dir_get(), obj->id);

   eina_stringshare_replace(&obj->thumbnail_local, dst);

   //check if we have a copy in cache...
   if (ecore_file_exists(dst)) //TODO check if the thumb is updated
   {
      // exectute the cb now
      cb(obj, obj->thumbnail_local, data);
      return EINA_TRUE;
   }

   //...else start downloading
   dd = calloc(1, sizeof(Exchange_Download_Data));
   if (!dd) return EINA_FALSE;
   dd->obj = obj;
   dd->type = DOWNLOAD_THUMBNAIL;
   dd->cb = cb;
   dd->data = data;
   dd->dst = eina_stringshare_ref(obj->thumbnail_local);
   ecore_file_download(obj->thumbnail, tmpf,
                       _download_complete_cb, NULL, dd, &obj->job_thumbnail);
   if (!obj->job_thumbnail)
   {
      eina_stringshare_del(dd->dst);
      free(dd);
      return EINA_FALSE;
   }

   return EINA_TRUE;
}

EAPI Eina_Bool
exchange_obj_screenshot_get(Exchange_Object *obj, Exchange_Download_CB *cb, void *data)
{
   Exchange_Download_Data *dd;
   char dst[4096]; //TODO PATH_MAX
   char tmpf[] = "/tmp/exchXXXXXX";

   if (!obj || !cb || !obj->screenshot || obj->id < 0 || !mktemp(tmpf))
      return EINA_FALSE;

   if (obj->job_screenshot) return EINA_FALSE;

   if (strstr(obj->screenshot, "/files/module/"))
      snprintf(dst, sizeof(dst), "%s/module.%d.sshot.png",
                    exchange_cache_dir_get(), obj->id);
   else if (strstr(obj->screenshot, "/files/application/"))
      snprintf(dst, sizeof(dst), "%s/application.%d.sshot.png",
                    exchange_cache_dir_get(), obj->id);
   else if (strstr(obj->screenshot, "/files/theme/"))
      snprintf(dst, sizeof(dst), "%s/theme.%d.sshot.png",
                    exchange_cache_dir_get(), obj->id);
   else
      snprintf(dst, sizeof(dst), "%s/unknow.%d.sshot.png",
                    exchange_cache_dir_get(), obj->id);

   eina_stringshare_replace(&obj->screenshot_local, dst);

   // check if we have a copy in cache...
   if (ecore_file_exists(dst)) //TODO check if the shot is updated
   {
      // exectute the cb now
      cb(obj, obj->screenshot_local, data);
      return EINA_TRUE;
   }

   // ...else start downloading
   dd = calloc(1, sizeof(Exchange_Download_Data));
   if (!dd) return EINA_FALSE;
   dd->obj = obj;
   dd->type = DOWNLOAD_SCREENSHOT;
   dd->cb = cb;
   dd->data = data;
   dd->dst = eina_stringshare_ref(obj->screenshot_local);
   ecore_file_download(obj->screenshot, tmpf,
                       _download_complete_cb, NULL, dd, &obj->job_screenshot);

   if (!obj->job_screenshot)
   {
      eina_stringshare_del(dd->dst);
      free(dd);
      return EINA_FALSE;
   }

   return EINA_TRUE;
}

EAPI Eina_Bool
exchange_obj_download(Exchange_Object *obj, const char *path, Exchange_Download_CB *cb, void *data)
{
   Exchange_Download_Data *dd;
   char dst[4096]; //TODO PATH_MAX
   char tmpf[] = "/tmp/exchXXXXXX";

   if (!obj || !path || !cb || !obj->url || obj->id < 0 || !mktemp(tmpf))
      return EINA_FALSE;

   if (obj->job_entry) return EINA_FALSE;

   snprintf(dst, sizeof(dst), "%s/%s", path, exchange_obj_file_name_get(obj));

   // start download
   dd = calloc(1, sizeof(Exchange_Download_Data));
   if (!dd) return EINA_FALSE;
   dd->obj = obj;
   dd->type = DOWNLOAD_ENTRY;
   dd->cb = cb;
   dd->data = data;
   dd->dst = eina_stringshare_add(dst);

   ecore_file_download(obj->url, tmpf,
                       _download_complete_cb, NULL, dd, &obj->job_entry);
   if (!obj->job_entry)
   {
      eina_stringshare_del(dd->dst);
      free(dd);
      return EINA_FALSE;
   }

   return EINA_TRUE;
}


EAPI const char *
exchange_obj_url_get(Exchange_Object *obj)
{
   return obj ? obj->url : NULL;
}

EAPI float
exchange_obj_rating_get(Exchange_Object *obj)
{
   return obj ? obj->rating : -1.0;
}

EAPI const char *
exchange_obj_created_get(Exchange_Object *obj)
{
   return obj ? obj->created_at : NULL;
}

EAPI const char *
exchange_obj_updated_get(Exchange_Object *obj)
{
   return obj ? obj->updated_at : NULL;
}

EAPI Eina_Bool
exchange_obj_update_available(Exchange_Object *obj, const char *path)
{
   char dst[4096];
   char *local_version;

   if (!obj || !ecore_file_is_dir(path))
      return EINA_FALSE;

   snprintf(dst, sizeof(dst), "%s/%s", path, exchange_obj_file_name_get(obj));

   // local not exist, update available !
   if (!ecore_file_exists(dst))
      return EINA_TRUE;

   // no version available, assume the theme is updated
   local_version = exchange_local_theme_version_get(dst);
   if (!obj->version || !local_version)
      return EINA_FALSE;

   // different version, update available !
   if (strcmp(local_version, obj->version))// TODO when ->version will be a stringshare we can use pointer comparison here
      return EINA_TRUE;

   // theme seems updated
   return EINA_FALSE;
}

EAPI void
exchange_obj_data_set(Exchange_Object *obj, void *data)
{
   if (obj) obj->user_data = data;
}

EAPI void *
exchange_obj_data_get(Exchange_Object *obj)
{
   return obj ? obj->user_data : NULL;
}
