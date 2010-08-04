/*
 * Copyright (c) 2009  ProFUSION Embedded Systems
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES ( INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION ) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * ( INCLUDING NEGLIGENCE OR OTHERWISE ) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @author Raphael Kubo da Costa <kubo@profusion.mobi>
 */

#include "db.h"

#include <Eet.h>
#include <Eina.h>
#include <stdlib.h>
#include <string.h>

#define DB_CONFIG_DB_NAME "data"

static Eet_Data_Descriptor *_entry_edd;
static Eet_Data_Descriptor *_list_edd;
static Eet_File *_db_file;

Db_Entry *
db_append(Db *db, const char *title, const char *text)
{
   Db_Entry *entry;
   Eina_List *tmp;

   if (!db)
     return NULL;

   entry = (Db_Entry*)malloc(sizeof(Db_Entry));
   if (!entry)
     return NULL;

   entry->title = eina_stringshare_add(title);
   entry->text = eina_stringshare_add(text);
   entry->in_use = EINA_FALSE;

   tmp = eina_list_append(db->db_list, entry);
   if (!tmp)
     {
	eina_stringshare_del(entry->title);
	eina_stringshare_del(entry->text);
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

	if (db->db_list)
	  {
	     EINA_LIST_FREE(db->db_list, data)
	       {
		  eina_stringshare_del(data->title);
		  eina_stringshare_del(data->text);
		  free(data);
	       }

	     db->db_list = NULL;
	  }

	free(db);
     }
}

Db *
db_init(const char *filename)
{
   Db *db = NULL;
   Eet_File *db_file = NULL;
   Eet_Data_Descriptor *entry_edd = NULL;
   Eet_Data_Descriptor *list_edd = NULL;
   Eet_Data_Descriptor_Class eddc;

   if (!EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Db_Entry))
     return NULL;
   entry_edd = eet_data_descriptor_stream_new(&eddc);
   if (!entry_edd)
     return NULL;

   EET_DATA_DESCRIPTOR_ADD_BASIC(entry_edd, Db_Entry, "title", title, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(entry_edd, Db_Entry, "text", text, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(entry_edd, Db_Entry, "in_use", in_use, EET_T_UCHAR);

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
	if (!db)
	  {
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

   eina_stringshare_del(entry->text);
   eina_stringshare_del(entry->title);
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

