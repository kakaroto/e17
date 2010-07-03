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

extern int __exchange_log_domain;
#define DBG(...) EINA_LOG_DOM_DBG(__exchange_log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(__exchange_log_domain, __VA_ARGS__)

/* typedef enum {
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
} Parser_States; */

struct _Exchange_Object {
   int id; /**< id */
   const char *name; /**< name */
   const char *file_name; /**< name of the edje file */
   char *author; /**< author */
   char *license; /**< license */
   char *version; /**< version */
   char description[4096]; /**< description */
   char *url; /**< URL */
   char *thumbnail; /**< thumbnail URL */
   const char *thumbnail_local; /**< full path to the thumbnail in cache */
   char *screenshot; /**< screenshot URL */
   const char *screenshot_local; /**< full path to the screenshot in cache */
   float rating; /**< rating */ 
   int user_id; /**< User id of the author */
   char *created_at; /**< creation timestamp */
   char *updated_at; /**< last update timestamp */
   void *user_data; /**< user attached data */
   Ecore_File_Download_Job *job_thumbnail; /**< download job for thumbnail */
   Ecore_File_Download_Job *job_screenshot; /**< download job for screenshot */
   Ecore_File_Download_Job *job_entry; /**< download job for the entry */
};

#endif /* _EXCHANGE_PRIVATE_H */
