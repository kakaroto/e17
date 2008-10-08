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

#ifndef _EXCHANGE_LOCAL_THEME_H
#define _EXCHANGE_LOCAL_THEME_H

/**
 * @file exchange_local_theme.h
 * @brief This file contains local theme functions
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

EAPI char *exchange_local_theme_name_get(const char *file);
EAPI char *exchange_local_theme_author_get(const char *file);
EAPI char *exchange_local_theme_license_get(const char *file);
EAPI char *exchange_local_theme_version_get(const char *file);
EAPI int  exchange_local_theme_check_update(const char *file);

#endif /* _EXCHANGE_LOCAL_THEME_H */
