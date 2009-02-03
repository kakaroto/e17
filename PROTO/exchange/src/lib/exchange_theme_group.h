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
 
#ifndef _EXCHANGE_THEME_GROUP_H
#define _EXCHANGE_THEME_GROUP_H

/**
 * @file exchange_theme_group.h
 * @brief This file contains theme_group functions
 *
 * Sample code
 *
 * @code
 * Eina_List *l, *l1;
 * l = exchange_theme_group_list_available();
 * printf("Available theme_groups (%d):\n", eina_list_count(l));
 * for (l1 = l; l1; l1 = eina_list_next(l1))
 * {
 *   if (l1->data)
 *   {
 *     Theme_Group_Data *tgd;
 *     tgd = (Theme_Group_Data *)l1->data;
 *     printf("[Name] %s, [Title] %s\n", (char *)tgd->name, (char *)tgd->title);
 *   }
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

typedef struct _Theme_Group_Parser {
   Parser_States state;
   Parser_States prev_state;
} Theme_Group_Parser;

/**
 * @endcond
 */

/**
 * \struct _Theme_Group_Data
 * \brief Metadata for a Theme_Group 
 */
typedef struct _Theme_Group_Data {
   int id; /**< Theme group id */
   char name[4096]; /**< Theme group name */
   char title[4096]; /**< Theme group title */
   int known; /**< Is it known? */
} Theme_Group_Data;

EAPI Eina_List *exchange_theme_group_list_available(void);

/**
 * @cond LOCAL
 */

// Callbacks for SAX parser
void _start_document_theme_group_cb(Theme_Group_Parser *state);
void _end_document_theme_group_cb(Theme_Group_Parser *state);
void _start_element_theme_group_cb(Theme_Group_Parser *state, const xmlChar *name, const xmlChar **attrs);
void _end_element_theme_group_cb(Theme_Group_Parser *state, const xmlChar *name);
void _chars_theme_group_cb(Theme_Group_Parser *state, const xmlChar *chars, int len);
int _theme_group_connect(void);
void _theme_group_free_data(void);

/**
 * @endcond
 */

#endif /* _EXCHANGE_THEME_GROUP_H */
