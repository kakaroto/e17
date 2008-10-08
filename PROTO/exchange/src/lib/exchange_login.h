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
 
#ifndef _EXCHANGE_LOGIN_H
#define _EXCHANGE_LOGIN_H

/**
 * @file exchange_login.h
 * @brief This file contains login functions
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/**
 * @cond LOCAL
 */

typedef enum {
   PARSER_LOGIN_START,
   PARSER_LOGIN_RSP,
   PARSER_LOGIN,
   PARSER_LOGIN_ID,
   PARSER_LOGIN_NAME,
   PARSER_LOGIN_EMAIL,
   PARSER_LOGIN_ROLE,
   PARSER_LOGIN_API_KEY,
   PARSER_LOGIN_CREATED,
   PARSER_LOGIN_UPDATED,
   PARSER_LOGIN_FINISH,
   PARSER_LOGIN_UNKNOWN
} Login_States;

typedef struct _Login_Parser {
   Login_States state;
   Login_States prev_state;
} Login_Parser;

/**
 * @endcond
 */

/**
 * \struct _Login_Data
 * \brief Login_Data structure
 */
typedef struct _Login_Data { /**< Metadata from a login request */
   int id; /** User id */
   char *name; /** User name */
   char *email; /** User email */
   char *role; /** User role */
   char *api_key; /** User API key */
   char *created_at; /** User creation timestamp */
   char *updated_at; /** User last update timestamp */
} Login_Data;

EAPI Login_Data *exchange_login(const char *username, const char *password);
EAPI int exchange_login_id_get(const char *username, const char *password);
EAPI char *exchange_login_api_key_get(const char *username, const char *password);

/**
 * @cond LOCAL
 */

// Callbacks for SAX parser
void _start_document_login_cb(Login_Parser *state);
void _end_document_login_cb(Login_Parser *state);
void _start_element_login_cb(Login_Parser *state, const xmlChar *name, const xmlChar **attrs);
void _end_element_login_cb(Login_Parser *state, const xmlChar *name);
void _chars_login_cb(Login_Parser *state, const xmlChar *chars, int len);
int _login_connect(const char *username, const char *password);
void _login_free_data(void);

/**
 * @endcond
 */

#endif /* _EXCHANGE_LOGIN_H */
