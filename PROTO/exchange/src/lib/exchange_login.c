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

xmlSAXHandler LoginParser = {
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
   (startDocumentSAXFunc) _start_document_login_cb, /* startDocument */
   (endDocumentSAXFunc) _end_document_login_cb, /* endDocument */
   (startElementSAXFunc) _start_element_login_cb, /* startElement */
   (endElementSAXFunc) _end_element_login_cb, /* endElement */
   0, /* reference */
   (charactersSAXFunc) _chars_login_cb, /* characters */
   0, /* ignorableWhitespace */
   0, /* processingInstruction */
   0, /* comment */
   0, /* warning */
   0, /* error */
   0  /* fatalError */
};

Login_Data *ldata;

/**
 * @addtogroup Exchange_Login_Group Exchange Login Functions
 *
 * Function for login.
 *
 * Sample code
 *
 * @code
 * Login_Data *ldata;
 *
 * ldata = (Login_Data *)exchange_login("myusername", "mypassword");
 * printf("ID: %d\n", ldata->id);
 * printf("Name: %s\n", ldata->name);
 * printf("Email: %s\n", ldata->email);
 * printf("Role: %s\n", ldata->role);
 * printf("API key: %s\n", ldata->api_key);
 * printf("Created at: %s\n", ldata->created_at);
 * printf("Updated at: %s\n", ldata->updated_at);
 * @endcode
 *
 * @{
 */

/**
 * @param username The user name
 * @param password The password
 * @return Login_Data pointer that contains all available data, NULL otherwise. The returning Login_Data * must be freed before exit
 * @brief Login user into exchange site, and return user data
 */

EAPI Login_Data *
exchange_login(const char *username, const char *password)
{
   if ((!username)&&(!password))
      return NULL;
   ldata = calloc(1, sizeof(Login_Data));
   if (_login_connect(username, password) == 0)
   {
      return ldata;
   }
   else
      return NULL;
}

/**
 * @param username The user name
 * @param password The password
 * @return The user ID, -1 otherwise
 * @brief Login user into exchange site, and return the user ID
 */
EAPI int
exchange_login_id_get(const char *username, const char *password)
{
   if ((!username)&&(!password))
      return -1;
   ldata = calloc(1, sizeof(Login_Data));
   if (_login_connect(username, password) == 0)
      return ldata->id;
   else
      return -1;
}

/**
 * @param username The user name
 * @param password The password
 * @return The API key, NULL otherwise. The returning char * must be freed before exit
 * @brief Login user into exchange site, and return the API key
 */
EAPI char *
exchange_login_api_key_get(const char *username, const char *password)
{
   if ((!username)&&(!password))
      return NULL;
   ldata = calloc(1, sizeof(Login_Data));
   if (_login_connect(username, password) == 0)
      return ldata->api_key;
   else
      return NULL;
}

/**
 * @}
 */

/*** Private functions ***/

void
_login_free_data(void)
{
   if (ldata)
   {
      if (ldata->name)
         free(ldata->name);
      if (ldata->email)
         free(ldata->email);
      if (ldata->role)
         free(ldata->role);
      if (ldata->api_key)
         free(ldata->api_key);
      if (ldata->created_at)
         free(ldata->created_at);
      if (ldata->updated_at)
         free(ldata->updated_at);
      free(ldata);
   }
   ldata = NULL;
}

int
_login_connect(const char *username, const char *password)
{
   Login_Parser state = { 0 };
   char url[4096];
   int ret;

   xmlInitParser();
   snprintf(url, sizeof(url), "http://exchange.enlightenment.org/api/login?name=%s&password=%s", username, password);
   ret = xmlSAXUserParseFile(&LoginParser, &state, url);
   xmlCleanupParser();
   return ret;
}

void
_start_document_login_cb(Login_Parser *state)
{
   state->state = PARSER_LOGIN_START;
   state->prev_state = PARSER_LOGIN_UNKNOWN;
}

void
_end_document_login_cb(Login_Parser *state)
{
   state->state = PARSER_LOGIN_UNKNOWN;
   state->prev_state = PARSER_LOGIN_START;
}

void
_start_element_login_cb(Login_Parser *state, const xmlChar *name, const xmlChar **attrs)
{
   if (!strcmp((char *)name, "id"))
   {
      state->state = PARSER_LOGIN_ID;
      state->prev_state = PARSER_LOGIN_START;
   }
   if (!strcmp((char *)name, "name"))
   {
      state->state = PARSER_LOGIN_NAME;
      state->prev_state = PARSER_LOGIN_START;
   }
   if (!strcmp((char *)name, "email"))
   {
      state->state = PARSER_LOGIN_EMAIL;
      state->prev_state = PARSER_LOGIN_START;
   }
   if (!strcmp((char *)name, "role"))
   {
      state->state = PARSER_LOGIN_ROLE;
      state->prev_state = PARSER_LOGIN_START;
   }
   if (!strcmp((char *)name, "api_key"))
   {
      state->state = PARSER_LOGIN_API_KEY;
      state->prev_state = PARSER_LOGIN_START;
   }
   if (!strcmp((char *)name, "created_at"))
   {
      state->state = PARSER_LOGIN_CREATED;
      state->prev_state = PARSER_LOGIN_START;
   }
   if (!strcmp((char *)name, "updated_at"))
   {
      state->state = PARSER_LOGIN_UPDATED;
      state->prev_state = PARSER_LOGIN_START;
   }
}

void
_end_element_login_cb(Login_Parser *state, const xmlChar *name)
{
   if (!strcmp((char *)name, "id"))
   {
      state->state = PARSER_LOGIN_START;
      state->prev_state = PARSER_LOGIN_ID;
   }
   if (!strcmp((char *)name, "name"))
   {
      state->state = PARSER_LOGIN_START;
      state->prev_state = PARSER_LOGIN_NAME;
   }
   if (!strcmp((char *)name, "email"))
   {
      state->state = PARSER_LOGIN_START;
      state->prev_state = PARSER_LOGIN_EMAIL;
   }
   if (!strcmp((char *)name, "role"))
   {
      state->state = PARSER_LOGIN_START;
      state->prev_state = PARSER_LOGIN_ROLE;
   }
   if (!strcmp((char *)name, "api_key"))
   {
      state->state = PARSER_LOGIN_START;
      state->prev_state = PARSER_LOGIN_API_KEY;
   }
   if (!strcmp((char *)name, "created_at"))
   {
      state->state = PARSER_LOGIN_START;
      state->prev_state = PARSER_LOGIN_CREATED;
   }
   if (!strcmp((char *)name, "updated_at"))
   {
      state->state = PARSER_LOGIN_START;
      state->prev_state = PARSER_LOGIN_UPDATED;
   }
}

void
_chars_login_cb(Login_Parser *state, const xmlChar *chars, int len)
{
   char buf[4096];

   switch (state->state)
   {
      case PARSER_LOGIN_ID:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         ldata->id = atoi(buf);
         break;
      case PARSER_LOGIN_NAME:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         ldata->name = strdup(buf);
         break;
      case PARSER_LOGIN_EMAIL:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         ldata->email = strdup(buf);
         break;
      case PARSER_LOGIN_ROLE:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         ldata->role = strdup(buf);
         break;
      case PARSER_LOGIN_API_KEY:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         ldata->api_key = strdup(buf);
         break;
      case PARSER_LOGIN_CREATED:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         ldata->created_at= strdup(buf);
         break;
      case PARSER_LOGIN_UPDATED:
         buf[0] = '\0';
         strncat((char *)buf, (char *)chars, len);
         ldata->updated_at= strdup(buf);
         break;
      default:
         break;
   }
}
