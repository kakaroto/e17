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

#include <stdio.h>
#include <string.h>
#include <Elementary.h>
#include <Evas.h>
#include "elm_widget_helper.h"
#include "db.h"

#define DB_FILENAME "/contacts.db"

#define GET_ED_FROM_PAGE(PAGE)			\
  Evas_Object *ed;				\
  ed = elm_layout_edje_get(App.info.PAGE)

typedef struct _Pginfo Pginfo;
struct _Pginfo
{
   Evas_Object *pager, *pgMain, *pgRegister, *pgBrowse;
};

struct _app_objects
{
   Db *_db;
   Evas_Object *contactlist;
   Pginfo info;
   Eina_Bool editing;
   Elm_List_Item *selection;
   Eina_Bool loading : EINA_TRUE;
} App;

static int _log_domain = -1;
#define CRITICAL(...) EINA_LOG_DOM_CRIT(_log_domain, __VA_ARGS__)

static void delete_clicked (void *data, Evas_Object *obj, const char *emission, const char *source);
static void save_clicked (void *data, Evas_Object *obj, const char *emission, const char *source);
static void cancel_clicked (void *data, Evas_Object *obj, const char *emission, const char *source);
static void entry_type(void *data, Evas_Object *obj, void *event_info);
static void new_clicked (void *data, Evas_Object *obj, const char *emission, const char *source);
static void previous_clicked (void *data, Evas_Object *obj, const char *emission, const char *source);
static void next_clicked (void *data, Evas_Object *obj, const char *emission, const char *source);
static void list_selected_cb(void *data, Evas_Object *obj, void *event_info);
static void edit_clicked (void *data, Evas_Object *obj, const char *emission, const char *source);
static void win_del(void *data, Evas_Object *obj, void *event_info);
static void add_update_user(Db_Entry NewContact);
static void list_load ();
static void load_values(Db_Entry *NewUser);
static void list_filter_set(Evas_Object *obj, const char *value, Eina_List *items);

/* Data manager functions */

/* Add or Update Contact details */
static void
add_update_user(Db_Entry NewContact)
{
   Db_Entry *entry;

   if (App.editing)
     {
	Db_Entry *contact;
	contact = elm_list_item_data_get(App.selection);

	/* Replace strings on memory */
	eina_stringshare_replace(&(contact->Name), NewContact.Name);
	eina_stringshare_replace(&(contact->Email), NewContact.Email);
	eina_stringshare_replace(&(contact->Phone), NewContact.Phone);
	eina_stringshare_replace(&(contact->Street), NewContact.Street);
	eina_stringshare_replace(&(contact->Neighborhood),
				 NewContact.Neighborhood);
	contact->Gender = NewContact.Gender;

	/* Update Elementary List */
	elm_list_item_label_set(App.selection, NewContact.Name);
     }
   else
     {
	/* Add New Entry on DB */
	entry = db_append(App._db, NewContact);
	elm_list_item_append(App.contactlist, entry->Name, NULL, NULL, NULL,
			     entry);
	elm_list_go(App.contactlist);
     }
}

/* Load Contact Details into Browse Page*/
static void
load_values(Db_Entry *NewUser)
{
   elm_pager_content_promote(App.info.pager, App.info.pgBrowse);

   GET_ED_FROM_PAGE(pgBrowse);

   edje_object_part_text_set(ed,"TextName",NewUser->Name);
   edje_object_part_text_set(ed,"TextEmail",NewUser->Email);
   edje_object_part_text_set(ed,"TextPhone",NewUser->Phone);
   edje_object_part_text_set(ed,"TextStreet",NewUser->Street);
   edje_object_part_text_set(ed,"TextNeighborhood",NewUser->Neighborhood);
   radio_value_set(ed, "RadioGender", NewUser->Gender);
}

/* Load Contact Details into Contactlist (Elementary List) */
static void
list_load ()
{
   const Eina_List *list;
   Db_Entry *entry;

   elm_list_clear(App.contactlist);
   EINA_LIST_FOREACH(db_list_get(App._db), list, entry)
     elm_list_item_append(App.contactlist, entry->Name , NULL, NULL, NULL,
			  entry);

   elm_list_go(App.contactlist);
}

/* Callbacks  */

/* Button "Delete" from Register Page */
static void
delete_clicked (void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Db_Entry *contact;

   contact = elm_list_item_data_get(App.selection);
   elm_list_item_del(App.selection);
   db_remove(App._db, contact);
   elm_pager_content_promote(App.info.pager, App.info.pgMain);
}

/* Button "Save" from Register Page */
static void
save_clicked (void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Db_Entry ContactReg;

   ContactReg.Name = entry_value_get(obj, "EntryName");
   ContactReg.Email = entry_value_get(obj, "EntryEmail");
   ContactReg.Phone = entry_value_get(obj, "EntryPhone");
   ContactReg.Street = entry_value_get(obj, "EntryStreet");
   ContactReg.Neighborhood = entry_value_get(obj, "EntryNeighborhood");
   ContactReg.Gender = radio_value_get(obj, "RadioGender");

   add_update_user(ContactReg);

   elm_pager_content_promote(App.info.pager, App.info.pgMain);

   if (App.selection)
     elm_list_item_selected_set(App.selection, EINA_FALSE);
}

/* Button "Cancel" from Register Page */
static void
cancel_clicked (void *data, Evas_Object *obj, const char *emission, const char *source)
{
   elm_pager_content_promote(App.info.pager, App.info.pgMain);

   if (App.selection)
     elm_list_item_selected_set(App.selection, EINA_FALSE);
}

/* Button "New" from Main Page */
static void
new_clicked (void *data, Evas_Object *obj, const char *emission, const char *source)
{

   elm_pager_content_promote(App.info.pager, App.info.pgRegister);

   GET_ED_FROM_PAGE(pgRegister);

   button_hide(ed,"BtnDelete");

   entry_value_set(ed, "EntryName", "");
   entry_value_set(ed, "EntryEmail", "");
   entry_value_set(ed, "EntryPhone", "");
   entry_value_set(ed, "EntryStreet", "");
   entry_value_set(ed, "EntryNeighborhood", "");
   radio_value_set(ed, "RadioGender", 0);

   App.editing = EINA_FALSE;
}

/* Button "Previous" from Browse Page */
static void
previous_clicked (void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Db_Entry *Contact;
   Elm_List_Item *prev;

   prev = elm_list_item_prev(App.selection);

   if (prev)
     {
	App.selection = prev;
	elm_list_item_selected_set(App.selection, EINA_TRUE);
	Contact = (Db_Entry*) elm_list_item_data_get(App.selection);
	load_values(Contact);
     }
}

/* Button "Next" from Browse Page */
static void
next_clicked (void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Db_Entry *Contact;
   Elm_List_Item *next;

   next = elm_list_item_next(App.selection);

   if (next)
     {
	App.selection = next;
	elm_list_item_selected_set(App.selection, EINA_TRUE);
	Contact = (Db_Entry*) elm_list_item_data_get(App.selection);
	load_values(Contact);
     }
}

/* Button "Edit" from Browse Page */
static void
edit_clicked (void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Db_Entry *contact;

   elm_pager_content_promote(App.info.pager, App.info.pgRegister);

   contact = elm_list_item_data_get(App.selection);

   GET_ED_FROM_PAGE(pgRegister);
   entry_value_set(ed,"EntryName", contact->Name);
   entry_value_set(ed,"EntryEmail", contact->Email);
   entry_value_set(ed,"EntryPhone", contact->Phone);
   entry_value_set(ed,"EntryStreet", contact->Street);
   entry_value_set(ed,"EntryNeighborhood", contact->Neighborhood);
   radio_value_set(ed, "RadioGender", contact->Gender);

   button_show(ed,"BtnDelete");

   App.editing = EINA_TRUE;
}

static void
list_filter_set(Evas_Object *obj, const char *value, Eina_List *items)
{
   Db_Entry *it;
   const Eina_List *iter;
   char *filter,*item_name;

   filter = strdup(value);
   filter[0] = tolower(filter[0]);

   elm_list_clear(obj);
   EINA_LIST_FOREACH(items, iter, it)
     {
	item_name = strdup(it->Name);
	item_name[0] = tolower(item_name[0]);
	if (strstr(item_name, filter))
	  elm_list_item_append(obj, it->Name , NULL, NULL, NULL, it);
     }
   free(filter);
   elm_list_go(obj);
}

/* Entry for Search from Main Page */
static void
entry_type(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *edje = data;
   const char *search;
   search = entry_value_get(edje, "EntrySearch");
   if (!App.loading)
     {
	App.loading = EINA_TRUE;
     }
   else
     list_filter_set(App.contactlist, search, db_list_get(App._db));
}

/* Remove filter */
static void
entry_unfocused(void *data, Evas_Object *obj, void *event_info)
{
   GET_ED_FROM_PAGE(pgMain);
   App.loading = EINA_FALSE;
   entry_value_set(ed, "EntrySearch", "");
   list_load();
}

/* List Item selected */
static void
list_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
   const char *target_name;

   App.selection = elm_list_selected_item_get(App.contactlist);
   target_name = elm_list_item_label_get(App.selection);

   load_values(elm_list_item_data_get(App.selection));
}

static void
on_win_del_req(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

static void
create_win(){
   Evas_Object *layout;
   Evas_Object *win, *ed, *entrysearch;
   Evas_Object *bg;
   Evas_Object *pg;

   /* Create a Window */
   win = elm_win_add(NULL, "EntryButtonExample", ELM_WIN_BASIC);
   evas_object_resize(win, 510, 510);

   elm_win_title_set(win, "Phone Book");

   /* Register the callback for close window function */
   evas_object_smart_callback_add(win, "delete-request", on_win_del_req, NULL);

   /* Add Back Ground */
   bg = elm_bg_add(win);
   elm_win_resize_object_add(win, bg);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);

   pg = elm_pager_add(win);
   elm_win_resize_object_add(win, pg);
   evas_object_size_hint_weight_set(pg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(pg, 0.0,0.0);
   evas_object_show(pg);
   App.info.pager = pg;

   /* Main - Page */

   /* Set layout Edje File */
   layout = elm_layout_add(win);

   if(!elm_layout_file_set(layout, PACKAGE_DATA_DIR "/default.edj","Main"))
     {
	CRITICAL("Can't load Edje Layout");
	exit(0);
     }

   ed = elm_layout_edje_get(layout);

   /* Set callback functions */
   edje_object_signal_callback_add(ed, "new,signal", "BtnRegister", new_clicked,
				   &App.info);
   entrysearch = edje_object_part_external_object_get(ed, "EntrySearch");
   evas_object_smart_callback_add(entrysearch, "changed", entry_type, ed);
   evas_object_smart_callback_add(entrysearch, "unfocused", entry_unfocused,
				  ed);
   evas_object_show(layout);
   elm_pager_content_push(pg, layout);
   App.info.pgMain = layout;

   App.contactlist = edje_object_part_external_object_get(ed, "ContactList");

   evas_object_smart_callback_add(App.contactlist, "selected", list_selected_cb,
				  NULL);
   evas_object_smart_callback_add(App.contactlist, "clicked", list_selected_cb,
				  NULL);
   list_load();
   evas_object_show(App.contactlist);

   /* Register - Page */

   /* Set layout Edje File */
   layout = elm_layout_add(win);

   if(!elm_layout_file_set(layout, PACKAGE_DATA_DIR "/default.edj","Register"))
     {
	CRITICAL("Can't load Edje Layout");
	exit(0);
     }

   ed = elm_layout_edje_get(layout);

   /* Set callback functions */
   edje_object_signal_callback_add(ed, "create,signal", "BtnCreate",
				   save_clicked, NULL);
   edje_object_signal_callback_add(ed, "cancel,signal", "BtnCancel",
				   cancel_clicked, NULL);
   edje_object_signal_callback_add(ed, "delete,signal", "BtnDelete",
				   delete_clicked, NULL);
   evas_object_show(layout);
   elm_pager_content_push(pg, layout);
   App.info.pgRegister = layout;

   /* Browse - Page */

   /* Set layout Edje File */
   layout = elm_layout_add(win);
   if(!elm_layout_file_set(layout, PACKAGE_DATA_DIR "/default.edj","Browse"))
     {
	CRITICAL("Can't load Edje Layout");
	exit(0);
     }

   ed = elm_layout_edje_get(layout);

   /* Set callback functions */
   edje_object_signal_callback_add(ed, "next,signal", "BtnNext", next_clicked,
				   NULL);
   edje_object_signal_callback_add(ed, "previous,signal", "BtnPrevious",
				   previous_clicked, NULL);
   edje_object_signal_callback_add(ed, "back,signal", "BtnBack", cancel_clicked,
				   NULL);
   edje_object_signal_callback_add(ed, "edit,signal", "BtnEdit", edit_clicked,
				   NULL);
   evas_object_show(layout);
   elm_pager_content_push(pg, layout);
   App.info.pgBrowse = layout;

   /* Show up the Main Page */
   elm_pager_content_promote(App.info.pager, App.info.pgMain);

   /* Show Main Window */
   evas_object_show(win);
}

/* Main */
EAPI int
elm_main(int argc, char **argv)
{
   char filename[40] = "";

   _log_domain = eina_log_domain_register("Phone_Book_Sample", NULL);
   if (_log_domain < 0)
     {
	EINA_LOG_CRIT("could not create log domain 'Phone_Book_Sample'.");
	return -1;
     }

   App.contactlist = NULL;

   /* Create database directory in home dir */

   mkdir(strcat(strcat(filename, getenv("HOME")),"/.PhoneBook"), 0777);
   strcat(filename, DB_FILENAME);

   _subsystems_init(&App._db, filename);


   create_win();

   /* Run Elementary Main Loop */
   elm_run();

   eina_log_domain_unregister(_log_domain);
   _log_domain = -1;

   elm_shutdown();
   _subsystems_shutdown(&App._db);

   return 0;
}
ELM_MAIN()
