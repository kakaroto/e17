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
 
#ifndef _EXCHANGE_MODULE_LIST_H
#define _EXCHANGE_MODULE_LIST_H

/**
 * @file exchange_module_list.h
 * @brief This file contains module listing functions
 *
 * Sample code
 *
 * @code
 * Eina_List *l, *l1;
 *
 * l = exchange_module_list_all(0, 0);
 * printf("All modules available\n");
 * for (l1 = l; l1; l1 = eina_list_next(l1))
 * {
 *   if (l1->data)
 *     {
 *       Module_List_Data *mld;
 *       mld = (Module_List_Data *)l1->data;
 *       printf("ID: %d, Name: %s\n", mld->id, (char *)mld->name);
 *     }
 * }
 * @endcode
 *
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

/**
 * @cond LOCAL
 */

typedef enum {
   PARSER_MODULE_LIST_START,
   PARSER_MODULE_LIST_RSP,
   PARSER_MODULE_LIST,
   PARSER_MODULE_LIST_ID,
   PARSER_MODULE_LIST_NAME,
   PARSER_MODULE_LIST_DESCRIPTION,
   PARSER_MODULE_LIST_URL,
   PARSER_MODULE_LIST_SCREENSHOT,
   PARSER_MODULE_LIST_USER_ID,
   PARSER_MODULE_LIST_APPLICATION_ID,   
   PARSER_MODULE_LIST_UNKNOWN
} Module_List_States;

typedef struct _Module_List_Parser {
   Module_List_States state;
   Module_List_States prev_state;
} Module_List_Parser;

/**
 * @endcond
 */

/**
 * \struct _Module_List_Data
 * \brief Metadata from a remote application 
 */
typedef struct _Module_List_Data {
   int id; /**< Module ID */
   char name[4096]; /**< Module name */
   char description[4096]; /**< Module description */
   char *url; /**< Module URL */
   char *screenshot; /**< Module screenshot URL */
   int application_id; /**< The ID of application */ 
   int user_id; /**< The ID of user */ 
} Module_List_Data;

EAPI Eina_List *exchange_module_list_filter_by_user_id(int user_id, int limit, int offset);
EAPI Eina_List *exchange_module_list_filter_by_user_name(const char *user_name, int limit, int offset);
EAPI Eina_List *exchange_module_list_filter_by_application_id(int application_id, int limit, int offset);
EAPI Eina_List *exchange_module_list_all(int limit, int offset);

/**
 * @cond LOCAL
 */

// Callbacks for SAX parser
void _start_document_module_list_cb(Module_List_Parser *state);
void _end_document_module_list_cb(Module_List_Parser *state);
void _start_element_module_list_cb(Module_List_Parser *state, const xmlChar *name, const xmlChar **attrs);
void _end_element_module_list_cb(Module_List_Parser *state, const xmlChar *name);
void _chars_module_list_cb(Module_List_Parser *state, const xmlChar *chars, int len);
int _module_list_connect(const char *filter, int limit, int offset);
void _module_list_free_data(void);

/**
 * @endcond
 */

#endif /* _EXCHANGE_MODULE_LIST_H */
