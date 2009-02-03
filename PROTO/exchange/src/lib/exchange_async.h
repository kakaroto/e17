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
 
#ifndef _EXCHANGE_THEMES_ASYNC_H
#define _EXCHANGE_THEMES_ASYNC_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/**
 * @file exchange_async.h
 * @brief This file contains the function exchange_remote_list
 *
 * TODO write doc
 *
 * Sample code
 *
 * @code
 *  TODO write doc
 * @endcode
 *
 */

//TODO doc this
typedef unsigned char (*Exchange_Complete_CB)(Eina_List *results, void *data);


/**
 * @param group_title Filter themes by group title (eg. "Border").
    Set to "Application" to get applications list, or "Modules" to get modules list.
 * @param group_name Filter themes by group name (eg. "e/desktop/background").
 * @param application_id Filter themes by application id.
 * @param module_id Filter themes by module id.
 * @param user_id Filter by user id.
 * @param user_name Filter by user name.
 * @param limit The max number of list entries
 * @param offset The starting number of list entries
 * @param complete_cb The fuction to call when the query is finished
 * @param cb_data User data passed back to the complete_cb function
 * @return 0 on errors
 * @brief Start a query for a remote list
 */
EAPI unsigned char exchange_remote_list(const char *group_title,
                                        const char *group_name,
                                        int application_id, int module_id,
                                        int user_id, const char *user_name,
                                        int limit, int offset,
                                        Exchange_Complete_CB complete_cb, void *cb_data);

/**
 * @cond LOCAL
 */


typedef struct _Async_List_Parser {
   char *url;
   char *tmp;
   Exchange_Complete_CB complete_cb;
   void *cb_data;

   Exchange_Object *current;
   Eina_List *l;

   Parser_States state;
   Parser_States prev_state;
} Async_List_Parser;

void _start_document_async_list_cb(Async_List_Parser *state);
void _end_document_async_list_cb(Async_List_Parser *state);
void _start_element_async_list_cb(Async_List_Parser *state, const xmlChar *name, const xmlChar **attrs);
void _end_element_async_list_cb(Async_List_Parser *state, const xmlChar *name);
void _chars_async_list_cb(Async_List_Parser *state, const xmlChar *chars, int len);

/**
 * @endcond
 */

#endif /* _EXCHANGE_THEMES_ASYNC_H */
