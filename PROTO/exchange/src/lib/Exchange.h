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

#ifndef _EXCHANGE_H
#define _EXCHANGE_H

/**
 * @file Exchange.h
 * @brief The file that must be included by any project wishing to use
 * Exchange. Exchange.h provides all of the necessary headers and includes to
 * work with Exchange.
 */

/**
 * @mainpage Exchange
 * @author Massimiliano Calamelli
 * @date 09/11/2008
 *
 * @section intro Introduction
 *
 * Exchange is a library designed to help apps to interact with
 * exchange.enlightenment.org contents.
 * Exchange provides : \n
 * @li @link Exchange.h Core functions @endlink
 * @li @link exchange_local_theme.h Functions that work with local themes. @endlink
 * @li @link exchange_remote_theme.h Functions that work with remote themes. @endlink
 * @li @link exchange_login.h Functions to login into exchange website. @endlink
 * @li @link exchange_theme_group.h Functions to work with theme_groups. @endlink
 * @li @link exchange_async.h Functions that list remote themes, modules and apps @endlink
 * @li @link exchange_smart.h The Exchange Smart Object @endlink
 */

#include <libxml/parser.h>
#include <Edje.h>

/**
 * \struct _Exchange_Object
 * \brief Metadata for themes, applications and modules
 */
typedef struct _Exchange_Object {
   int id; /**< id */
   char *name; /**< name */
   char *author; /**< author */
   char *license; /**< license */
   char *version; /**< version */
   char description[4096]; /**< description */
   char *url; /**< URL */
   char *thumbnail; /**< thumbnail URL */
   char *screenshot; /**< screenshot URL */
   float rating; /**< rating */ 
   int user_id; /**< User id of the author */
   char *created_at; /**< creation timestamp */
   char *updated_at; /**< last update timestamp */
} Exchange_Object;

#include "exchange_private.h"
#include "exchange_local_theme.h"
#include "exchange_remote_theme.h"
#include "exchange_login.h"
#include "exchange_theme_group.h"
#include "exchange_smart.h"
#include "exchange_async.h"


#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_EXCHANGE_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EXCHANGE_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif /* ! _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif

EAPI int  exchange_init(void);
EAPI int  exchange_shutdown(void);

EAPI unsigned char exchange_object_free(Exchange_Object *obj);

#ifdef __cplusplus
}
#endif

#endif /* _EXCHANGE_H */
