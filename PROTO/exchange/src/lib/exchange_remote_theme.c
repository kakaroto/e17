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

xmlSAXHandler ThemeParser = {
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
   (startDocumentSAXFunc) _start_document_theme_cb, /* startDocument */
   (endDocumentSAXFunc) _end_document_theme_cb, /* endDocument */
   (startElementSAXFunc) _start_element_theme_cb, /* startElement */
   (endElementSAXFunc) _end_element_theme_cb, /* endElement */
   0, /* reference */
   (charactersSAXFunc) _chars_theme_cb, /* characters */
   0, /* ignorableWhitespace */
   0, /* processingInstruction */
   0, /* comment */
   0, /* warning */
   0, /* error */
   0  /* fatalError */
};

Exchange_Theme *tdata;

/**
 * @addtogroup Exchange_Remote_Theme_Group Exchange Remote Theme Functions
 *
 * Functions that handles remote theme metadata.
 *
 * @{
 */

/**
 * @param theme_name The name to the theme
 * @return ID of the theme, -1 otherwise
 * @brief Get remote theme id from the theme name. 
 */
EAPI int
exchange_remote_theme_id_get(const char *theme_name)
{
   if (!theme_name)
      return -1;
   tdata = calloc(1, sizeof(Exchange_Theme));
   if (_theme_connect(theme_name) == 0)
      return tdata->id;
   else
      return -1;

}

/**
 * @param theme_name The name to the theme
 * @return Author of the theme, NULL otherwise. The returning char * must be freed before exit
 * @brief Get remote theme author from the theme name. 
 */
EAPI char *
exchange_remote_theme_author_get(const char *theme_name)
{
   if (!theme_name)
      return NULL;
   tdata = calloc(1, sizeof(Exchange_Theme));
   if (_theme_connect(theme_name) == 0)
      return tdata->author;
   else
      return NULL;
}

/**
 * @param theme_name The name to the theme
 * @return License of the theme, NULL otherwise. The returning char * must be freed before exit
 * @brief Get remote theme license from the theme name. 
 */
EAPI char *
exchange_remote_theme_license_get(const char *theme_name)
{
   if (!theme_name)
      return NULL;
   tdata = calloc(1, sizeof(Exchange_Theme));
   if (_theme_connect(theme_name) == 0)
      return tdata->license;
   else
      return NULL;

}

/**
 * @param theme_name The name to the theme
 * @return Version of the theme, NULL otherwise. The returning char * must be freed before exit
 * @brief Get remote theme version from the theme name. 
 */
EAPI char *
exchange_remote_theme_version_get(const char *theme_name)
{
   if (!theme_name)
      return NULL;
   tdata = calloc(1, sizeof(Exchange_Theme));
   if (_theme_connect(theme_name) == 0)
      return tdata->version;
   else
      return NULL;
}

/**
 * @param theme_name The name to the theme
 * @return Description of the theme, NULL otherwise
 * @brief Get remote theme description from the theme name. 
 */
EAPI char *
exchange_remote_theme_description_get(const char *theme_name)
{
   if (!theme_name)
      return NULL;
   tdata = calloc(1, sizeof(Exchange_Theme));
   if (_theme_connect(theme_name) == 0)
      return tdata->description;
   else
      return NULL;
}

/**
 * @param theme_name The name to the theme
 * @return URL of the theme, NULL otherwise. The returning char * must be freed before exit
 * @brief Get remote theme URL from the theme name. 
 */
EAPI char *
exchange_remote_theme_url_get(const char *theme_name)
{
   if (!theme_name)
      return NULL;
   tdata = calloc(1, sizeof(Exchange_Theme));
   if (_theme_connect(theme_name) == 0)
      return tdata->url;
   else
      return NULL;
}

/**
 * @param theme_name The name to the theme
 * @return Thumbnail's URL of the theme, NULL otherwise. The returning char * must be freed before exit
 * @brief Get remote theme thumbnail's URL from the theme name. 
 */
EAPI char *
exchange_remote_theme_thumbnail_url_get(const char *theme_name)
{
   if (!theme_name)
      return NULL;
   tdata = calloc(1, sizeof(Exchange_Theme));
   if (_theme_connect(theme_name) == 0)
      return tdata->thumbnail;
   else
      return NULL;
}

/**
 * @param theme_name The name to the theme
 * @return Screenshot's URL of the theme, NULL otherwise. The returning char * must be freed before exit
 * @brief Get remote theme screenshot's URL from the theme name. 
 */
EAPI char *
exchange_remote_theme_screenshot_url_get(const char *theme_name)
{
   if (!theme_name)
      return NULL;
   tdata = calloc(1, sizeof(Exchange_Theme));
   if (_theme_connect(theme_name) == 0)
      return tdata->screenshot;
   else
      return NULL;
}

/**
 * @param theme_name The name to the theme
 * @return Rating of the theme, -1 otherwise
 * @brief Get remote theme rating from the theme name. 
 */
EAPI float
exchange_remote_theme_rating_get(const char *theme_name)
{
   if (!theme_name)
      return -1;
   tdata = calloc(1, sizeof(Exchange_Theme));
   if (_theme_connect(theme_name) == 0)
      return tdata->rating;
   else
      return -1;  
}

/**
 * @param theme_name The name to the theme
 * @return Author's id of the theme, -1 otherwise
 * @brief Get remote theme author's id from the theme name. 
 */
EAPI int
exchange_remote_theme_user_id_get(const char *theme_name)
{
   if (!theme_name)
      return -1;
   tdata = calloc(1, sizeof(Exchange_Theme));
   if (_theme_connect(theme_name) == 0)
      return tdata->user_id;
   else
      return -1;

}

/**
 * @param theme_name The name to the theme
 * @return The last update's timestamp of the theme, NULL otherwise. The returning char * must be freed before exit
 * @brief Get remote theme last update's timestamp from the theme name. 
 */
EAPI char *
exchange_remote_theme_updated_get(const char *theme_name)
{
   if (!theme_name)
      return NULL;
   tdata = calloc(1, sizeof(Exchange_Theme));
   if (_theme_connect(theme_name) == 0)
      return tdata->updated_at;
   else
      return NULL;
}

/**
 * @param theme_name The name to the theme
 * @return The creation's timestamp of the theme, NULL otherwise. The returning char * must be freed before exit
 * @brief Get remote theme creation timestamp from the theme name. 
 */
EAPI char *
exchange_remote_theme_created_get(const char *theme_name)
{
   if (!theme_name)
      return NULL;
   tdata = calloc(1, sizeof(Exchange_Theme));
   if (_theme_connect(theme_name) == 0)
      return tdata->created_at;
   else
      return NULL;
}

/**
 * @param theme_name The name to the theme
 * @return Theme_Data pointer that contains all available data, NULL otherwise. The returning Theme_Data * must be freed before exit
 * @brief Get all available remote theme data from the theme name. 
 */
EAPI Exchange_Theme *
exchange_remote_theme_all_data_get(const char *theme_name)
{
   if (!theme_name)
      return NULL;
   tdata = calloc(1, sizeof(Exchange_Theme));
   if (_theme_connect(theme_name) == 0)
      return tdata;
   else
      return NULL;
}

/**
 * @}
 */

/*** Private functions ***/

void 
_theme_free_data(void)
{
   if (tdata)
   {
      if (tdata->author)
         free(tdata->author);
      if (tdata->license)
         free(tdata->license);
      if (tdata->version)
         free(tdata->version);
      if (tdata->url)
         free(tdata->url);
      if (tdata->thumbnail)
         free(tdata->thumbnail);
      if (tdata->screenshot)
         free(tdata->screenshot);
      if (tdata->created_at)
         free(tdata->created_at);
      if (tdata->updated_at)
         free(tdata->updated_at);
      free(tdata);
   }
   tdata = NULL;
}

int
_theme_connect(const char *theme)
{
   Theme_Parser state = { 0 };
   char url[4096];
   int ret;
   
   xmlInitParser();
   tdata->name = (char *)theme;
   tdata->local = 0;
   snprintf(url, sizeof(url), "http://exchange.enlightenment.org/api/read?object=theme&name=%s", theme);
   //snprintf(url, sizeof(url), "http://localhost/cerium.xml", theme);
   ret = xmlSAXUserParseFile(&ThemeParser, &state, url);
   xmlCleanupParser();
   return ret;
}

void
_start_document_theme_cb(Theme_Parser *state)
{
   state->state = PARSER_START;
   state->prev_state = PARSER_UNKNOWN;
}

void
_end_document_theme_cb(Theme_Parser *state)
{
   state->state = PARSER_UNKNOWN;
   state->prev_state = PARSER_START;
}

void
_start_element_theme_cb(Theme_Parser *state, const xmlChar *name, const xmlChar **attrs)
{
   if (!strcmp((char *)name, "id"))
   {
      state->state = PARSER_THEME_ID;
      state->prev_state = PARSER_START;
   }
   if (!strcmp((char *)name, "author"))
   {
      state->state = PARSER_THEME_AUTHOR;
      state->prev_state = PARSER_START;
   }
   if (!strcmp((char *)name, "license"))
   {
      state->state = PARSER_THEME_LICENSE;
      state->prev_state = PARSER_START;
   }
   if (!strcmp((char *)name, "version"))
   {
      state->state = PARSER_THEME_VERSION;
      state->prev_state = PARSER_START;
   }
   if (!strcmp((char *)name, "description"))
   {
      state->state = PARSER_THEME_DESCRIPTION;
      state->prev_state = PARSER_START;
   }
   if (!strcmp((char *)name, "url"))
   {
      state->state = PARSER_THEME_URL;
      state->prev_state = PARSER_START;
   }
   if (!strcmp((char *)name, "thumbnail"))
   {
      state->state = PARSER_THEME_THUMBNAIL;
      state->prev_state = PARSER_START;
   }
   if (!strcmp((char *)name, "screenshot"))
   {
      state->state = PARSER_THEME_SCREENSHOT;
      state->prev_state = PARSER_START;
   }
   if (!strcmp((char *)name, "rating"))
   {
      state->state = PARSER_THEME_RATING;
      state->prev_state = PARSER_START;
   }
   if (!strcmp((char *)name, "user_id"))
   {
      state->state = PARSER_THEME_USER_ID;
      state->prev_state = PARSER_START;
   }
   if (!strcmp((char *)name, "created_at"))
   {
      state->state = PARSER_THEME_CREATED;
      state->prev_state = PARSER_START;
   }
   if (!strcmp((char *)name, "updated_at"))
   {
      state->state = PARSER_THEME_UPDATED;
      state->prev_state = PARSER_START;
   }
}

void
_end_element_theme_cb(Theme_Parser *state, const xmlChar *name)
{
   if (!strcmp((char *)name, "id"))
   {
      state->state = PARSER_START;
      state->prev_state = PARSER_THEME_ID;
   }
   if (!strcmp((char *)name, "author"))
   {
      state->state = PARSER_START;
      state->prev_state = PARSER_THEME_AUTHOR;
   }
   if (!strcmp((char *)name, "license"))
   {
      state->state = PARSER_START;
      state->prev_state = PARSER_THEME_LICENSE;
   }
   if (!strcmp((char *)name, "version"))
   {
      state->state = PARSER_START;
      state->prev_state = PARSER_THEME_VERSION;
   }
   if (!strcmp((char *)name, "description"))
   {
      state->state = PARSER_START;
      state->prev_state = PARSER_THEME_DESCRIPTION;
   }
   if (!strcmp((char *)name, "url"))
   {
      state->state = PARSER_START;
      state->prev_state = PARSER_THEME_URL;
   }
   if (!strcmp((char *)name, "thumbnail"))
   {
      state->state = PARSER_START;
      state->prev_state = PARSER_THEME_THUMBNAIL;
   }
   if (!strcmp((char *)name, "screenshot"))
   {
      state->state = PARSER_START;
      state->prev_state = PARSER_THEME_SCREENSHOT;
   }
   if (!strcmp((char *)name, "rating"))
   {
      state->state = PARSER_START;
      state->prev_state = PARSER_THEME_RATING;
   }
   if (!strcmp((char *)name, "user_id"))
   {
      state->state = PARSER_START;
      state->prev_state = PARSER_THEME_USER_ID;
   }
   if (!strcmp((char *)name, "created_at"))
   {
      state->state = PARSER_START;
      state->prev_state = PARSER_THEME_CREATED;
   }
   if (!strcmp((char *)name, "updated_at"))
   {
      state->state = PARSER_START;
      state->prev_state = PARSER_THEME_UPDATED;
   }
}

void
_chars_theme_cb(Theme_Parser *state, const xmlChar *chars, int len)
{
   char buf[4096];

   switch (state->state)
   {
      case PARSER_THEME_ID:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tdata->id = atoi(buf);
         break;
      case PARSER_THEME_AUTHOR:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tdata->author = strdup(buf);
         break;
      case PARSER_THEME_LICENSE:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tdata->license = strdup(buf);
         break;
      case PARSER_THEME_VERSION:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tdata->version = strdup(buf);
         break;
      case PARSER_THEME_DESCRIPTION:
         strncat((char *)tdata->description, (char *)chars, len);
         break;
      case PARSER_THEME_URL:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tdata->url = strdup(buf);
         break;
      case PARSER_THEME_THUMBNAIL:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tdata->thumbnail = strdup(buf);
         break;
      case PARSER_THEME_SCREENSHOT:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tdata->screenshot = strdup(buf);
         break;
      case PARSER_THEME_RATING:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tdata->rating = atof(buf);
         break;
      case PARSER_THEME_USER_ID:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tdata->user_id = atoi(buf);
         break;
      case PARSER_THEME_CREATED:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tdata->created_at= strdup(buf);
         break;
      case PARSER_THEME_UPDATED:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         tdata->updated_at= strdup(buf);
         break;
      default:
         break;
   }
}
