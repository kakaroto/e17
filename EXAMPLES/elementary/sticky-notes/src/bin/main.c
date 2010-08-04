/*
 * Copyright (c) 2009-2010  ProFUSION Embedded Systems
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

#include "sticky_notes_private.h"

#include "db.h"
#include "gui.h"
#include <Elementary.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#ifdef PATH_MAX
# define DB_PATH_MAX PATH_MAX
#else
# define DB_PATH_MAX 1024
#endif

#define DB_FILENAME ".sticky-notes.db"

static Db *_db = NULL;

int _log_dom = -1;

static void
_main_window_delete_request(void *data, Evas_Object *obj, void *event_info)
{
    elm_exit();
}

static Eina_Bool
_subsystems_init(void)
{
   char db_path[DB_PATH_MAX];
   int ret;

   eina_init();
   eet_init();

   if (_log_dom < 0)
     {
	_log_dom = eina_log_domain_register("sticky-notes-edje", NULL);
	if (_log_dom < 0)
	  {
	     EINA_LOG_CRIT("Could not register log domain for"
			   " sticky-notes-edje");
	     return EINA_FALSE;
	  }
     }

   ret = snprintf(db_path, DB_PATH_MAX, "%s/%s", getenv("HOME"), DB_FILENAME);
   if (ret < 0 || ret >= DB_PATH_MAX)
     {
	ERR("Could not determine the notes file correctly.");
	return EINA_FALSE;
     }

   _db = db_init(db_path);
   if (!_db)
     {
	ERR("Could not load the notes file.");
	return EINA_FALSE;
     }

   return EINA_TRUE;
}

static void
_subsystems_shutdown(void)
{
   if (_log_dom >= 0)
     {
	eina_log_domain_unregister(_log_dom);
	_log_dom = -1;
     }

   db_free(_db);
   db_shutdown();
   eet_shutdown();
   eina_shutdown();
}

static void
_note_add_button_clicked(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Elm_List_Item *list_item = NULL;
   Evas_Object *list_widget = (Evas_Object*)data;
   Db_Entry *entry;

   entry = db_append(_db, "", "");
   if (!entry)
     return;

   list_item = elm_list_item_append(list_widget, entry->title, NULL, NULL,
				    NULL, entry);
   elm_list_go(list_widget);

   edit_note_window_new(list_widget, list_item, _db);
}

static void
_notes_list_clicked(void *data, Evas_Object *obj, void *event_info)
{
   Elm_List_Item *list_item;

   list_item = elm_list_selected_item_get(obj);

   edit_note_window_new(obj, list_item, _db);
}

static void
_load_list_from_db(Evas_Object *list, Db *db)
{
   Eina_List *iter;
   Db_Entry *entry;
   Eina_List *db_list = db_list_get(db);

   if (!db_list)
     return;

   EINA_LIST_FOREACH(db_list, iter, entry)
     {
	elm_list_item_append(list, entry->title, NULL, NULL, NULL, entry);
     }

   elm_list_go(list);
}

int
elm_main(int argc, char **argv)
{
   Evas_Object *background;
   Evas_Object *main_window;
   Evas_Object *notes_list;

   Evas_Object *ed;
   Evas_Object *layout;

   if (!_subsystems_init())
     {
	_subsystems_shutdown();
	return 1;
     }

   main_window = elm_win_add(NULL, "sticky-notes-list", ELM_WIN_BASIC);
   elm_win_title_set(main_window, "Sticky Notes");

   background = elm_bg_add(main_window);
   elm_win_resize_object_add(main_window, background);
   evas_object_size_hint_weight_set(background, 1.0, 1.0);
   evas_object_smart_callback_add(main_window, "delete-request",
				  _main_window_delete_request, NULL);
   evas_object_show(background);

   layout = elm_layout_add(main_window);
   elm_win_resize_object_add(main_window, layout);
   elm_layout_file_set(layout, PACKAGE_DATA_DIR "/main-gui.edj", "main");
   evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(layout);

   ed = elm_layout_edje_get(layout);
   notes_list = edje_object_part_external_object_get(ed, "NoteList");
   _load_list_from_db(notes_list, _db);
   evas_object_smart_callback_add(notes_list, "clicked", _notes_list_clicked,
				  notes_list);

   edje_object_signal_callback_add(ed, "mouse,clicked,1", "AddNoteButton",
				   _note_add_button_clicked, notes_list);

   evas_object_show(main_window);

   elm_run();
   elm_shutdown();

   _subsystems_shutdown();

   return 0;
}
ELM_MAIN()

