/**
 * @file
 *
 * Copyright (C) 2010 by ProFUSION embedded systems
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,  but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the  GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 * @author Thiago Ribeiro Masaki <masaki@profusion.mobi>
 */
#ifndef DB_H
#define DB_H

#include <Eet.h>
#include <Eina.h>
#include <stdlib.h>

typedef struct _user {
   const char *Name,
     *Email,
     *Phone,
     *Street,
     *Neighborhood;
   int  Gender;
} Db_Entry;

typedef struct {
   Eina_List *db_list;
} Db;

Db_Entry *db_append(Db *db, Db_Entry NewContact);

Eina_Bool _subsystems_init(Db **_db, char *dbfilename);

void _subsystems_shutdown(Db **_db);

void db_free(Db *db);

Db *db_init(const char *filename);

Eina_List *db_list_get(Db *db);

Eina_Bool db_remove(Db *db, Db_Entry *entry);

void db_shutdown(void);

Eina_Bool db_sync(Db *db);

#endif
