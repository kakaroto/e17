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
 
#ifndef _EXCHANGE_APPLICATION_LIST_H
#define _EXCHANGE_APPLICATION_LIST_H

/**
 * @file exchange_application_list.h
 * @brief This file contains application listing functions
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/**
 * @cond LOCAL
 */

typedef enum {
   PARSER_APPLICATION_LIST_START,
   PARSER_APPLICATION_LIST_RSP,
   PARSER_APPLICATION_LIST,
   PARSER_APPLICATION_LIST_ID,
   PARSER_APPLICATION_LIST_NAME,
   PARSER_APPLICATION_LIST_DESCRIPTION,
   PARSER_APPLICATION_LIST_URL,
   PARSER_APPLICATION_LIST_SCREENSHOT,
   PARSER_APPLICATION_LIST_USER_ID,
   PARSER_APPLICATION_LIST_UNKNOWN
} Application_List_States;

typedef struct _Application_List_Parser {
   Application_List_States state;
   Application_List_States prev_state;
} Application_List_Parser;

/**
 * @endcond
 */

/**
 * \struct _Application_List_Data
 * \brief Application_List_Data structure
 */
typedef struct _Application_List_Data { /**< Metadata from a remote application */
   int id; /** Application ID */
   char name[4096]; /** Application name */
   char description[4096]; /** Application description */
   char *url; /** Application URL */
   char *screenshot; /** Application screenshot URL */
   int user_id; /** The ID of user */ 
} Application_List_Data;

EAPI Evas_List *exchange_application_list_filter_by_user_id(int user_id, int limit, int offset);
EAPI Evas_List *exchange_application_list_filter_by_user_name(const char *user_name, int limit, int offset);

/**
 * @cond LOCAL
 */

// Callbacks for SAX parser
void _start_document_application_list_cb(Application_List_Parser *state);
void _end_document_application_list_cb(Application_List_Parser *state);
void _start_element_application_list_cb(Application_List_Parser *state, const xmlChar *name, const xmlChar **attrs);
void _end_element_application_list_cb(Application_List_Parser *state, const xmlChar *name);
void _chars_application_list_cb(Application_List_Parser *state, const xmlChar *chars, int len);
int _application_list_connect(const char *filter, int limit, int offset);
void _application_list_free_data(void);

/**
 * @endcond
 */

#endif /* _EXCHANGE_APPLICATION_LIST_H */
