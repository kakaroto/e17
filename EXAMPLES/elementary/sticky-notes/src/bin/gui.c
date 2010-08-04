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

#include "db.h"
#include "gui.h"
#include <string.h>

typedef struct _Edit_Node_Window Edit_Note_Window;

struct _Edit_Node_Window
{
   Evas_Object *entry;
   Evas_Object *title;
   Evas_Object *window;

   Db *db;
   Evas_Object *list;
   Elm_List_Item *selection;
};

static void
_edit_note_cb_delete_request(void *data, Evas_Object *obj, void *event_info)
{
   Db_Entry *entry;
   Edit_Note_Window *gui = (Edit_Note_Window *)data;
   const char *gui_text = elm_scrolled_entry_entry_get(gui->entry);
   const char *gui_title = elm_scrolled_entry_entry_get(gui->title);

   entry = elm_list_item_data_get(gui->selection);

   eina_stringshare_replace(&(entry->text), gui_text);
   eina_stringshare_replace(&(entry->title), gui_title);

   elm_list_item_label_set(gui->selection, entry->title);

   entry->in_use = EINA_FALSE;

   free(gui);
}

static void
_edit_note_cb_remove_note(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Db_Entry *entry;
   Edit_Note_Window *gui = (Edit_Note_Window *)data;

   entry = elm_list_item_data_get(gui->selection);

   if (!db_remove(gui->db, entry))
     return;

   elm_list_item_del(gui->selection);

   evas_object_del(gui->window);

   free(gui);
}

static void
_edit_note_load_entry_data(Edit_Note_Window *gui)
{
   Db_Entry *entry;
   Evas_Object *list_widget;
   Elm_List_Item *selected_item;

   list_widget = gui->list;
   selected_item = gui->selection;
   entry = elm_list_item_data_get(selected_item);

   if (!entry)
     return;

   elm_scrolled_entry_entry_set(gui->entry, entry->text);
   elm_scrolled_entry_entry_set(gui->title, entry->title);
}

void
edit_note_window_new(Evas_Object *list, Elm_List_Item *selection, Db *db)
{
   Evas_Object *background;
   Evas_Object *note_window;

   Evas_Object *ed;
   Evas_Object *layout;

   Db_Entry *db_entry = elm_list_item_data_get(selection);
   if (db_entry->in_use)
     return;
   db_entry->in_use = EINA_TRUE;

   Edit_Note_Window *gui =
     (Edit_Note_Window*)malloc(sizeof(Edit_Note_Window));

   if (!gui)
     return;

   note_window = elm_win_add(NULL, "sticky-notes-add-note", ELM_WIN_BASIC);
   elm_win_autodel_set(note_window, 1);
   elm_win_title_set(note_window, "Add Note");
   evas_object_smart_callback_add(note_window, "delete-request", _edit_note_cb_delete_request, gui);
   evas_object_resize(note_window, 200, 100);

   background = elm_bg_add(note_window);
   elm_win_resize_object_add(note_window, background);
   evas_object_color_set(background, 255, 255, 0, 255);
   evas_object_size_hint_weight_set(background, 1.0, 1.0);
   evas_object_show(background);

   layout = elm_layout_add(note_window);
   elm_win_resize_object_add(note_window, layout);
   elm_layout_file_set(layout, PACKAGE_DATA_DIR "/note-edit-gui.edj", "main");
   evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(layout);

   ed = elm_layout_edje_get(layout);
   edje_object_signal_callback_add(ed, "mouse,clicked,1", "RemoveNoteButton", _edit_note_cb_remove_note, gui);

   evas_object_show(note_window);

   gui->entry = edje_object_part_external_object_get(ed, "ContentsEntry");
   gui->title = edje_object_part_external_object_get(ed, "TitleEntry");
   gui->window = note_window;
   gui->db = db;
   gui->list = list;
   gui->selection = selection;

   _edit_note_load_entry_data(gui);
}

