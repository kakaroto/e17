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

#ifndef _EXCHANGE_PRIVATE_H
#define _EXCHANGE_PRIVATE_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

typedef enum {
   PARSER_START,
   PARSER_RSP,
   PARSER_LIST,
   PARSER_THEME,
   PARSER_THEME_GROUP,
   PARSER_THEME_GROUP_START,
   PARSER_ID,
   PARSER_NAME,
   PARSER_TITLE,
   PARSER_AUTHOR,
   PARSER_LICENSE,
   PARSER_VERSION,
   PARSER_DESCRIPTION,
   PARSER_URL,
   PARSER_THUMBNAIL,
   PARSER_SCREENSHOT,
   PARSER_RATING,
   PARSER_USER_ID,
   PARSER_CREATED,
   PARSER_UPDATED,
   PARSER_LIST_START,
   PARSER_FINISH,
   PARSER_KNOWN,
   PARSER_UNKNOWN,
} Parser_States;

#endif /* _EXCHANGE_PRIVATE_H */
