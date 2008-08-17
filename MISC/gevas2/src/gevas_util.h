/*
 * Copyright (C) 2001 Ben Martin.
 *
 * Original author: Ben Martin
 *
 * See COPYING for full details of copying & use of this software.
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 *
 */

#include <glib.h>
#ifdef BUILD_EDB_CODE
#include <Edb.h>
#endif

const char* gevas_trim_prefix( const char* prefix, const char* s );
char* url_file_name_part_new( const char* _full_url );
GHashTable* url_args_to_hash( const char* args );
const char* url_args_lookup_str( GHashTable* h, const char* key, const char* def );
#ifdef BUILD_EDB_CODE
gint  edb_lookup_int( E_DB_File* edb, gint def, const char* fmt, ... );
char* edb_lookup_str( E_DB_File* edb, const char* def, const char* fmt, ... );
void edb_to_hash_int( E_DB_File* edb, GHashTable* hash,
                      const char* key, const char* prefix, gint def);
#endif
void hash_str_str_clean(GHashTable *hash);






