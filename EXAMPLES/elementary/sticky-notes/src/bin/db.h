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

#ifndef DB_H
#define DB_H

#include <Eet.h>
#include <Eina.h>
#include <stdlib.h>

typedef struct _Db_Entry Db_Entry;
typedef struct _Db Db;

struct _Db_Entry
{
   const char *title;
   const char *text;
   Eina_Bool in_use;
};

struct _Db
{
   Eina_List *db_list;
};

Db_Entry *db_append(Db *db, const char *title, const char *text);

void db_free(Db *db);

Db *db_init(const char *filename);

Eina_List *db_list_get(Db *db);

Eina_Bool db_remove(Db *db, Db_Entry *entry);

void db_shutdown(void);

Eina_Bool db_sync(Db *db);

#endif

