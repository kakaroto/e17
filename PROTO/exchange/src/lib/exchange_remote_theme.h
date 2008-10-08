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

#ifndef _EXCHANGE_REMOTE_THEME_H
#define _EXCHANGE_REMOTE_THEME_H

/**
 * @file exchange_remote_theme.h
 * @brief This file contains remote theme functions
 */


#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

typedef enum {
   PARSER_START,
   PARSER_RSP,
   PARSER_THEME,
   PARSER_THEME_ID,
   PARSER_THEME_NAME,
   PARSER_THEME_AUTHOR,
   PARSER_THEME_LICENSE,
   PARSER_THEME_VERSION,
   PARSER_THEME_DESCRIPTION,
   PARSER_THEME_URL,
   PARSER_THEME_THUMBNAIL,
   PARSER_THEME_SCREENSHOT,
   PARSER_THEME_RATING,
   PARSER_THEME_USER_ID,
   PARSER_THEME_CREATED,
   PARSER_THEME_UPDATED,
   PARSER_FINISH,
   PARSER_UNKNOWN
} Theme_States;

/**
 * @cond LOCAL
 */

typedef struct _Theme_Parser {
   Theme_States state;
   Theme_States prev_state;
} Theme_Parser;

/**
 * @endcond
 */

/**
 * \struct _Theme_Data
 * \brief Theme_Data structure
 */
typedef struct _Theme_Data { /**< Metadata from a remote theme */
   int id; /** Theme id */
   char *name; /** Theme name */
   char *author; /** Theme author */
   char *license; /** Theme license */
   char *version; /** Theme version */
   char description[4096]; /** Theme description */
   char *url; /** Theme URL */
   char *thumbnail; /** Theme thumbnail URL */
   char *screenshot; /** Theme screenshot URL */
   float rating; /** Theme rating */ 
   int user_id; /** User id of theme author */
   char *created_at; /** Theme creation timestamp */
   char *updated_at; /** Theme last update timestamp */
} Theme_Data;

EAPI int  exchange_remote_theme_id_get(const char *theme_name);
EAPI char *exchange_remote_theme_author_get(const char *theme_name);
EAPI char *exchange_remote_theme_license_get(const char *theme_name);
EAPI char *exchange_remote_theme_version_get(const char *theme_name);
EAPI char *exchange_remote_theme_description_get(const char *theme_name);
EAPI char *exchange_remote_theme_url_get(const char *theme_name);
EAPI char *exchange_remote_theme_thumbnail_url_get(const char *theme_name);
EAPI char *exchange_remote_theme_screenshot_url_get(const char *theme_name);
EAPI float exchange_remote_theme_rating_get(const char *theme_name);
EAPI int  exchange_remote_theme_user_id_get(const char *theme_name);
EAPI char *exchange_remote_theme_created_get(const char *theme_name);
EAPI char *exchange_remote_theme_updated_get(const char *theme_name);
EAPI Theme_Data *exchange_remote_theme_all_data_get(const char *theme_name);

/**
 * @cond LOCAL
 */

// Callbacks for SAX parser
void _start_document_theme_cb(Theme_Parser *state);
void _end_document_theme_cb(Theme_Parser *state);
void _start_element_theme_cb(Theme_Parser *state, const xmlChar *name, const xmlChar **attrs);
void _end_element_theme_cb(Theme_Parser *state, const xmlChar *name);
void _chars_theme_cb(Theme_Parser *state, const xmlChar *chars, int len);
int _theme_connect(const char *theme);
void _theme_free_data(void);
/**
 * @endcond
 */

#endif /* _EXCHANGE_REMOTE_THEME_H */
