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
 * @author Rafael Kubo da Costa <kubo@profusion.mobi>
 */

#include "db.h"

#include <Eet.h>
#include <Eina.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define DB_CONFIG_DB_NAME "data"

static Eet_Data_Descriptor *_entry_edd;
static Eet_Data_Descriptor *_list_edd;
static Eet_File *_db_file;

Eina_Bool
_subsystems_init(Db **_db, char *dbfilename)
{
   eina_init();
   eet_init();

   *_db = db_init(dbfilename);
   if (!*_db)
     {
	fprintf(stderr, "Could not load the notes file.\n");
	return EINA_FALSE;
     }
   return EINA_TRUE;
}

void
_subsystems_shutdown(Db **_db)
{
   db_free(*_db);
   db_shutdown();
   eet_shutdown();
   eina_shutdown();
}

static int
eina_str_cmp(const void *a, const void *b)
{
   const char *ia = a;
   const char *ib = b;

   return strcasecmp(ia,ib);
}

Db_Entry
*db_append(Db *db, Db_Entry NewContact)
{
   Db_Entry *entry;
   Eina_List *tmp;

   if (!db)
     return NULL;

   entry = (Db_Entry*)malloc(sizeof(Db_Entry));

   if (!entry)
     return NULL;

   entry->Name = eina_stringshare_add(NewContact.Name);
   entry->Email = eina_stringshare_add(NewContact.Email);
   entry->Phone = eina_stringshare_add(NewContact.Phone);
   entry->Street = eina_stringshare_add(NewContact.Street);
   entry->Neighborhood = eina_stringshare_add(NewContact.Neighborhood);
   entry->Gender = NewContact.Gender;

   tmp = eina_list_sorted_insert(db->db_list, eina_str_cmp, entry);

   if (!tmp)
     {
	eina_stringshare_del(entry->Name);
	eina_stringshare_del(entry->Email);
	eina_stringshare_del(entry->Phone);
	eina_stringshare_del(entry->Street);
	eina_stringshare_del(entry->Neighborhood);
	free(entry);

	return NULL;
     }

   db->db_list = tmp;

   return entry;
}

void
db_free(Db *db)
{
   Db_Entry *data;

   if (db)
     {
	/* Sync before closing */
	db_sync(db);

	if (db->db_list) {
	   EINA_LIST_FREE(db->db_list, data)
	     {
		eina_stringshare_del(data->Name);
		eina_stringshare_del(data->Email);
		eina_stringshare_del(data->Phone);
		eina_stringshare_del(data->Street);
		eina_stringshare_del(data->Neighborhood);
		free(data);
	     }

	   db->db_list = NULL;
	}

	free(db);
     }
}

Db *db_init(const char *filename)
{
   Db *db = NULL;
   Eet_File *db_file = NULL;
   Eet_Data_Descriptor *entry_edd = NULL;
   Eet_Data_Descriptor *list_edd = NULL;
   Eet_Data_Descriptor_Class eddc;

   if (!EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Db_Entry))
     {
	return NULL;
     }
   entry_edd = eet_data_descriptor_stream_new(&eddc);
   if (!entry_edd)
     {
	return NULL;
     }

   EET_DATA_DESCRIPTOR_ADD_BASIC(entry_edd, Db_Entry, "Name", Name,
				 EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(entry_edd, Db_Entry, "Email", Email,
				 EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(entry_edd, Db_Entry, "Phone", Phone,
				 EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(entry_edd, Db_Entry, "Street", Street,
				 EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(entry_edd, Db_Entry, "Neighborhood",
				 Neighborhood, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(entry_edd, Db_Entry, "Gender", Gender,
				 EET_T_INT);

   if (!EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Db))
     {
	eet_data_descriptor_free(entry_edd);
	return NULL;
     }
   list_edd = eet_data_descriptor_stream_new(&eddc);
   if (!list_edd)
     {
	eet_data_descriptor_free(entry_edd);
	return NULL;
     }

   EET_DATA_DESCRIPTOR_ADD_LIST(list_edd, Db, "db_list", db_list, entry_edd);

   db_file = eet_open(filename, EET_FILE_MODE_READ_WRITE);
   if (!db_file)
     {
	eet_data_descriptor_free(entry_edd);
	eet_data_descriptor_free(list_edd);
	return NULL;
     }

   db = eet_data_read(db_file, list_edd, DB_CONFIG_DB_NAME);
   if (!db)
     {
	db = (Db*)malloc(sizeof(Db));
	if (!db) {
	   eet_data_descriptor_free(entry_edd);
	   eet_data_descriptor_free(list_edd);
	   eet_close(db_file);
	   return NULL;
	}

	db->db_list = NULL;
     }

   _entry_edd = entry_edd;
   _list_edd = list_edd;
   _db_file = db_file;

   return db;
}

Eina_List
*db_list_get(Db *db)
{
   if (!db)
     return NULL;

   return db->db_list;
}

Eina_Bool
db_remove(Db *db, Db_Entry *entry)
{
   if ((!db) || (!entry))
     return EINA_FALSE;

   db->db_list = eina_list_remove(db->db_list, entry);

   eina_stringshare_del(entry->Name);
   eina_stringshare_del(entry->Email);
   eina_stringshare_del(entry->Phone);
   eina_stringshare_del(entry->Street);
   eina_stringshare_del(entry->Neighborhood);

   free(entry);

   return EINA_TRUE;
}

void
db_shutdown(void)
{
   if (_entry_edd)
     eet_data_descriptor_free(_entry_edd);

   if (_list_edd)
     eet_data_descriptor_free(_list_edd);

   if (_db_file)
     eet_close(_db_file);

   _entry_edd = NULL;
   _list_edd = NULL;
   _db_file = NULL;
}

Eina_Bool
db_sync(Db *db)
{
   return eet_data_write
     (_db_file, _list_edd, DB_CONFIG_DB_NAME, db, 1);
}
