/* vim:tabstop=4
 * Copyright © 2009 Rui Miguel Silva Seabra <rms@1407.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib/gprintf.h>
#include <gconf/gconf-client.h>

#include <Elementary.h>
#include <Ecore_X.h>

#include <sqlite3.h>

#include <curl.h>

#include <config.h>

#include "gettext.h"
#define _(string) gettext (string)

#include "curl.h"
#include "settings.h"
#include "elmdentica.h"

Evas_Object *settings_win=NULL, *settings_area=NULL, *account_editor=NULL, *cache_editor=NULL,
	    *user_data_dialog=NULL, *screen_name_entry=NULL, *password_entry=NULL,
	    *domain_data_dialog=NULL, *apiroot_entry=NULL, *type_entry=NULL, *secure_entry=NULL,
	    *domain_entry=NULL, *base_url_entry=NULL, *enabled_entry=NULL, *receive_entry=NULL, *send_entry=NULL;

Elm_List_Item * current_account_li=NULL, *current_domain_li=NULL;

extern GHashTable *my_accounts;
extern GHashTable *my_domains;

extern struct sqlite3 *ed_DB;
extern int MAX_MESSAGES;

extern GConfClient *conf_client;
int current_account_type = ACCOUNT_TYPE_NONE;
int current_account = 0;
extern char * url_post;
extern char * url_friends;
extern int debug;
extern CURL * user_agent;

void settings_win_del(void *data, Evas_Object *obj, void *event_info) {
        evas_object_del(settings_win);
}

void user_data_dialog_win_del(void *data, Evas_Object *obj, void *event_info) {
       	evas_object_del(user_data_dialog);
}

void on_account_selected(void *data, Evas_Object *account_list, void *event_info) {
	int *id = (int*)data;

	current_account = *id;
	if (debug) printf("Selected account: %d\n", current_account);
}

void on_user_data_cancel(void *data, Evas_Object *obj, void *event_info) {
       	evas_object_del(user_data_dialog);
}

void on_account_enabled_toggle(void *data, Evas_Object *check, void *event_info) {
	int *id = (int*)data;
	int sqlite_res = 0;
	char *query=NULL, *db_err=NULL;

	if(elm_check_state_get(check)) {
		sqlite_res = asprintf(&query, "UPDATE accounts set enabled='%d' where id='%d';", 1, *id);
	} else {
		sqlite_res = asprintf(&query, "UPDATE accounts set enabled='%d' where id='%d';", 0, *id);
	}

	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
		if(sqlite_res != 0)
			printf("Can't %s: %d = %s\n", query, sqlite_res, db_err);
		sqlite3_free(db_err);
		free(query);
	}

}

void drop_account(Evas_Object *account_list) {
	int sqlite_res = 0;
	char *query=NULL, *db_err=NULL;
	Elm_List_Item *li;

	sqlite_res = asprintf(&query, "DELETE FROM accounts WHERE id='%d';", current_account);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
		if(sqlite_res != 0)
			printf("Can't %s: %d = %s\n", query, sqlite_res, db_err);
		sqlite3_free(db_err);
		free(query);

		li = elm_list_selected_item_get(account_list);
		if(li) elm_list_item_del(li);
	}
}

static int get_account_id_handler(void *data, int argc, char **argv, char **azColName) {
	int *id = (int*)data;
	if(argc == 0) *id = 0;
	else *id = atoi(argv[0]);

	return(0);
}

void get_account_id(char *screen_name, char *domain, int *id) {
	char *query=NULL, *db_err=NULL;
	int sqlite_res = 0;
	sqlite_res = asprintf(&query, "SELECT id FROM accounts WHERE name = '%s' AND domain = '%s' LIMIT 1;", screen_name, domain);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, get_account_id_handler, (void*)id, &db_err);
		if(sqlite_res != 0)
			printf("Can't run %s: %s\n", query, db_err);
		sqlite3_free(db_err);
		free(query);
	}
}

void on_user_data_ok(void *data, Evas_Object *obj, void *event_info) {
	Evas_Object *account_list = (Evas_Object*)data, *check=NULL;
	char *account_id=NULL, *query=NULL, *db_err=NULL;
	char *screen_name=NULL, *password=NULL, *domain=NULL, *base_url=NULL, *proto=NULL;
	int enabled=1, port=443, sqlite_res = 0, *id=NULL, receive=1, send=1, res=0;
	Elm_List_Item *li;
	Eina_Strbuf *buf = NULL;

	buf = eina_strbuf_new();

	eina_strbuf_append(buf, elm_entry_entry_get(screen_name_entry));
	eina_strbuf_replace_all(buf, "<br>", "");
	screen_name = eina_strbuf_string_steal(buf);
	eina_strbuf_reset(buf);

	eina_strbuf_append(buf, elm_entry_entry_get(password_entry));
	eina_strbuf_replace_all(buf, "<br>", "");
	password = eina_strbuf_string_steal(buf);
	eina_strbuf_reset(buf);

	eina_strbuf_append(buf, elm_entry_entry_get(domain_entry));
	eina_strbuf_replace_all(buf, "<br>", "");
	domain = eina_strbuf_string_steal(buf);
	eina_strbuf_reset(buf);

	eina_strbuf_append(buf, elm_entry_entry_get(base_url_entry));
	eina_strbuf_replace_all(buf, "<br>", "");
	base_url = eina_strbuf_string_steal(buf);
	eina_strbuf_reset(buf);

	eina_strbuf_free(buf);

	if(elm_toggle_state_get(secure_entry)) {
		proto = "https";
		port = 443;
	} else {
		proto = "http";
		port = 80;
	}

	if(!elm_check_state_get(receive_entry)) {
		receive = 0;
	}

	if(!elm_check_state_get(send_entry)) {
		send = 0;
	}

	id = calloc(1, sizeof(int));
	get_account_id(screen_name, domain, id);

	res = asprintf(&account_id, "%s@%s", screen_name, domain);
	if(res != -1) {
		if(current_account != 0)
			drop_account(account_list);

		if(*id == 0) {
			res = asprintf(&query, "INSERT INTO accounts (enabled, name, password, type, proto, domain, port, base_url, receive, send) values (%d, '%s', '%s', %d, '%s', '%s', %d, '%s', %d, %d);",
					enabled, screen_name, password, current_account_type, proto, domain, port, base_url, receive, send);
		} else {
			res = asprintf(&query, "INSERT INTO accounts (id, enabled, name, password, type, proto, domain, port, base_url, receive, send) values (%d, %d, '%s', '%s', %d, '%s', '%s', %d, '%s', %d, %d);",
					*id, enabled, screen_name, password, current_account_type, proto, domain, port, base_url, receive, send);
		}

		if(res != -1) {
			sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
			if(sqlite_res != 0)
				printf("Can't perform %s: %d => %s\n", query, sqlite_res, db_err);
			sqlite3_free(db_err);
			free(query);
	
			free(id);
			id = calloc(1, sizeof(int));
			get_account_id(screen_name, domain, id);

			check = elm_check_add(settings_win);
			if(enabled)
				elm_check_state_set(check, TRUE);
			else
				elm_check_state_set(check, FALSE);

			evas_object_smart_callback_add(check, "changed", on_account_enabled_toggle, id);
			evas_object_show(check);

			li = elm_list_item_append(account_list, account_id, check, NULL, on_account_selected, id);

			elm_list_go(account_list);
			evas_object_del(user_data_dialog);
		 }
	}
}

void on_account_type_chose_statusnet(void *data, Evas_Object *obj, void *event_info) {
	current_account_type=ACCOUNT_TYPE_STATUSNET;
}

void on_account_type_chose_twitter(void *data, Evas_Object *obj, void *event_info) {
	current_account_type=ACCOUNT_TYPE_TWITTER;
}

void on_account_type_chosen(void *data, Evas_Object *obj, void *event_info) {
	Evas_Object *type_hoversel = (Evas_Object*)data;
	switch(current_account_type) {
		case ACCOUNT_TYPE_TWITTER: { elm_hoversel_label_set(type_hoversel, "Twitter"); break; }
		case ACCOUNT_TYPE_STATUSNET:
		default: { elm_hoversel_label_set(type_hoversel, "StatusNet"); break; }
	}
}

Evas_Object * account_dialog(Evas_Object *parent, char *screen_name, char *password, int account_type, Eina_Bool use_https, char *domain, char *base_url, Eina_Bool is_enabled, Evas_Object *account_list, int receive, int send) {
	Evas_Object *box=NULL, *table=NULL, *frame=NULL, *button_list=NULL, *button=NULL, *scroller=NULL;
	Elm_Hoversel_Item *acc_type=NULL;

	box = elm_box_add(parent);
		evas_object_size_hint_weight_set(box, 1, 1);
		evas_object_size_hint_align_set(box, -1, -1);

	scroller = elm_scroller_add(parent);
		evas_object_size_hint_weight_set(scroller, 1, 1);
		evas_object_size_hint_align_set(scroller, -1, -1);
		elm_scroller_bounce_set(scroller, 0, 1);

		table = elm_table_add(parent);
			elm_table_homogenous_set(table, TRUE);
			evas_object_size_hint_weight_set(table, 1, 1);
			evas_object_size_hint_align_set(table, -1, 0);

			frame = elm_frame_add(user_data_dialog);
				evas_object_size_hint_weight_set(frame, 1, 1);
				evas_object_size_hint_align_set(frame, -1, 0);
				elm_frame_label_set(frame, _("Screen name"));

				screen_name_entry = elm_entry_add(user_data_dialog);
					elm_entry_single_line_set(screen_name_entry, 1);
					elm_entry_entry_set(screen_name_entry, screen_name);
					elm_frame_content_set(frame, screen_name_entry);
				evas_object_show(screen_name_entry);

				elm_table_pack(table, frame, 0, 0, 1, 1);
			evas_object_show(frame);

			frame = elm_frame_add(user_data_dialog);
				evas_object_size_hint_weight_set(frame, 1, 1);
				evas_object_size_hint_align_set(frame, -1, 0);
				elm_frame_label_set(frame, _("Password"));

				password_entry = elm_entry_add(user_data_dialog);
					elm_entry_single_line_set(password_entry, 1);
					elm_entry_password_set(password_entry, 1);
					elm_entry_entry_set(password_entry, password);
					elm_frame_content_set(frame, password_entry);
				evas_object_show(password_entry);

				elm_table_pack(table, frame, 1, 0, 1, 1);
			evas_object_show(frame);

			type_entry = elm_hoversel_add(user_data_dialog);
				evas_object_size_hint_weight_set(type_entry, 1, 1);
				evas_object_size_hint_align_set(type_entry, -1, 0.5);
				switch(account_type) {
					case ACCOUNT_TYPE_TWITTER: {
						current_account_type=ACCOUNT_TYPE_TWITTER;
						elm_hoversel_label_set(type_entry, "Twitter");
						break;
					}
					case ACCOUNT_TYPE_STATUSNET: { 
						current_account_type=ACCOUNT_TYPE_STATUSNET;
						elm_hoversel_label_set(type_entry, "StatusNet");
						break;
					}
					default:	{
						current_account_type=ACCOUNT_TYPE_STATUSNET;
						elm_hoversel_label_set(type_entry, _("Type"));
						break;
					}
				}
				elm_hoversel_hover_parent_set(type_entry, table);

				elm_hoversel_hover_begin(type_entry);
					acc_type = elm_hoversel_item_add(type_entry, "StatusNet", NULL, ELM_ICON_NONE, on_account_type_chose_statusnet, type_entry);
					acc_type = elm_hoversel_item_add(type_entry, "Twitter", NULL, ELM_ICON_NONE, on_account_type_chose_twitter, type_entry);
					evas_object_smart_callback_add(type_entry, "selected", on_account_type_chosen, type_entry);
				elm_hoversel_hover_end(type_entry);

				elm_table_pack(table, type_entry, 0, 1, 1, 1);
			evas_object_show(type_entry);

			secure_entry = elm_toggle_add(user_data_dialog);
				evas_object_size_hint_weight_set(secure_entry, 1, 1);
				evas_object_size_hint_align_set(secure_entry, -1, 0.5);
				elm_toggle_states_labels_set(secure_entry, _("Secure"), _("Faster"));
				if(use_https)
					elm_toggle_state_set(secure_entry, TRUE);
				else 
					elm_toggle_state_set(secure_entry, FALSE);

				elm_table_pack(table, secure_entry, 1, 1, 1, 1);
			evas_object_show(secure_entry);

			frame = elm_frame_add(user_data_dialog);
				evas_object_size_hint_weight_set(frame, 1, 1);
				evas_object_size_hint_align_set(frame, -1, 0);
				elm_frame_label_set(frame, _("Domain"));

				domain_entry = elm_entry_add(user_data_dialog);
					elm_entry_single_line_set(domain_entry, 1);
					elm_entry_entry_set(domain_entry, domain);
					elm_frame_content_set(frame, domain_entry);
				evas_object_show(domain_entry);

				elm_table_pack(table, frame, 0, 2, 1, 1);
			evas_object_show(frame);

			frame = elm_frame_add(user_data_dialog);
				evas_object_size_hint_weight_set(frame, 1, 1);
				evas_object_size_hint_align_set(frame, -1, 0);
				elm_frame_label_set(frame, _("Base URL"));

				base_url_entry = elm_entry_add(user_data_dialog);
					elm_entry_single_line_set(base_url_entry, 1);
					elm_entry_entry_set(base_url_entry, base_url);
					elm_frame_content_set(frame, base_url_entry);
				evas_object_show(base_url_entry);

				elm_table_pack(table, frame, 1, 2, 1, 1);
			evas_object_show(frame);

			receive_entry = elm_check_add(user_data_dialog);
				evas_object_size_hint_weight_set(receive_entry, 1, 1);
				evas_object_size_hint_align_set(receive_entry, -1, 0);
				elm_check_label_set(receive_entry, _("Receive?"));
				if(receive)
					elm_check_state_set(receive_entry, TRUE);
				else
					elm_check_state_set(receive_entry, FALSE);

				elm_table_pack(table, receive_entry, 0, 3, 1, 1);
			evas_object_show(receive_entry);

			send_entry = elm_check_add(user_data_dialog);
				evas_object_size_hint_weight_set(send_entry, 1, 1);
				evas_object_size_hint_align_set(send_entry, -1, 0);
				elm_check_label_set(send_entry, _("Send?"));
				if(send)
					elm_check_state_set(send_entry, TRUE);
				else
					elm_check_state_set(send_entry, FALSE);

				elm_table_pack(table, send_entry, 1, 3, 1, 1);
			evas_object_show(send_entry);

			elm_scroller_content_set(scroller, table);
		evas_object_show(table);

		elm_box_pack_end(box, scroller);
	evas_object_show(scroller);


	button_list = elm_box_add(user_data_dialog);
		evas_object_size_hint_weight_set(button_list, 1, 0);
		evas_object_size_hint_align_set(button_list, -1, 1);
		elm_box_horizontal_set(button_list, TRUE);
		elm_box_homogenous_set(button_list, TRUE);

		button = elm_button_add(user_data_dialog);
			evas_object_size_hint_weight_set(button, 1, 1);
			evas_object_size_hint_align_set(button, -1, 0);

			elm_button_label_set(button, _("OK"));
			evas_object_smart_callback_add(button, "clicked", on_user_data_ok, account_list);
			elm_box_pack_end(button_list, button);
		evas_object_show(button);

		button = elm_button_add(user_data_dialog);
			evas_object_size_hint_weight_set(button, 1, 1);
			evas_object_size_hint_align_set(button, -1, 0);
			elm_button_label_set(button, _("Cancel"));
			evas_object_smart_callback_add(button, "clicked", user_data_dialog_win_del, NULL);
			elm_box_pack_end(button_list, button);
		evas_object_show(button);

		elm_box_pack_end(box, button_list);
	evas_object_show(button_list);

	return(box);
}

static int account_edit(void *data, int argc, char **argv, char **azColName) {
	Evas_Object *box=NULL;
	char *screen_name=NULL, *password=NULL, *domain=NULL, *base_url=NULL, *proto=NULL;
	Eina_Bool enabled;
	int type=ACCOUNT_TYPE_NONE, port, receive=1, send=1;

	/*	In this SQL context, these are the columns:
			argv[0] == id INTEGER
			argv[1] == enabled INTEGER
			argv[2] == name TEXT
			argv[3] == password TEXT
			argv[4] == type INTEGER
			argv[5] == proto TEXT
			argv[6] == domain TEXT
			argv[7] == port INTEGER
			argv[8] == base_url TEXT
			argv[9] == receive INTEGER
			argv[10] == send INTEGER
	*/

	if(0 == atoi(argv[1]))
		enabled = 0;
	else
		enabled = 1;

	screen_name = argv[2];
	password = argv[3];

	switch(atoi(argv[4])) {
		case 2: { type = ACCOUNT_TYPE_TWITTER; break; }
		case 1:
		default: { type = ACCOUNT_TYPE_STATUSNET; break; }
	}

	proto = argv[5];
	domain = argv[6];
	port = atoi(argv[7]);
	base_url = argv[8];
	receive = atoi(argv[9]);
	send = atoi(argv[10]);

	user_data_dialog = elm_win_inwin_add(settings_win);
	if(user_data_dialog) {
		evas_object_smart_callback_add(user_data_dialog, "delete-request", user_data_dialog_win_del, NULL);

		if(strlen(proto) == 5 && strncmp("https", proto, 5) == 0)
			box = account_dialog(user_data_dialog, screen_name, password, type, TRUE, domain, base_url, enabled, (Evas_Object*)data, receive, send);
		else
			box = account_dialog(user_data_dialog, screen_name, password, type, FALSE, domain, base_url, enabled, (Evas_Object*)data, receive, send);

		if(box) {
			elm_win_inwin_content_set(user_data_dialog, box);
			evas_object_show(box);
			evas_object_show(user_data_dialog);
			elm_object_focus(screen_name_entry);
		} else
			evas_object_del(user_data_dialog);
	}
	return(0);
}

void on_account_edit(void *data, Evas_Object *obj, void *event_info) {
	char *query=NULL, *db_err=NULL;
	int sqlite_res=0;

	if(current_account == 0) return;

	sqlite_res = asprintf(&query, "SELECT * FROM accounts WHERE id = '%d' LIMIT 1;", current_account);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, account_edit, data, &db_err);
		if(sqlite_res != 0)
			printf("Can't run %s: %s\n", query, db_err);
		sqlite3_free(db_err);
		free(query);
	}
}

void on_account_add(void *data, Evas_Object *obj, void *event_info) {
	Evas_Object *box=NULL;

	current_account = 0;
	current_account_type = ACCOUNT_TYPE_NONE;

	user_data_dialog = elm_win_inwin_add(settings_win);
	if(user_data_dialog) {
		evas_object_smart_callback_add(user_data_dialog, "delete-request", user_data_dialog_win_del, NULL);

		box = account_dialog(user_data_dialog, "", "", ACCOUNT_TYPE_NONE, TRUE, "", "/", TRUE, (Evas_Object*)data, 1, 1);
		if(box) {
			elm_win_inwin_content_set(user_data_dialog, box);
			evas_object_show(box);
			evas_object_show(user_data_dialog);
			elm_object_focus(screen_name_entry);
		} else
			evas_object_del(user_data_dialog);
	}
}

void on_account_delete(void *data, Evas_Object *button, void *event_info) {
	Evas_Object *account_list = (Evas_Object*)data;

	if(current_account != 0) {

		drop_account(account_list);
		current_account = 0;
		current_account_type = ACCOUNT_TYPE_NONE;
	}
}

static int accounts_list_insert(void *user_data, int argc, char **argv, char **azColName) {
	Evas_Object *list = (Evas_Object*)user_data, *check=NULL;
	Elm_List_Item * item=NULL;
	int *id = calloc(1, sizeof(int));
	char *key=NULL;
	int enabled=0, res=0;

	/*	In this SQL context, these are the columns:
			argv[0] == id
			argv[1] == screen_name
			argv[2] == domain
			argv[3] == enabled
	*/

	*id = atoi(argv[0]);
	res = asprintf(&key, "%s@%s", argv[1], argv[2]);
	if(res != 0) {
		enabled = atoi(argv[3]);

		check = elm_check_add(settings_win);
		if(enabled)
			elm_check_state_set(check, TRUE);
		else
			elm_check_state_set(check, FALSE);

		evas_object_smart_callback_add(check, "changed", on_account_enabled_toggle, id);
		evas_object_show(check);

		item = elm_list_item_append(list, (char*)key, check, NULL, on_account_selected, id);
		return(0);
	} else {
		return(1);
	}
}

void on_settings_accounts(void *data, Evas_Object *toolbar, void *event_info) {
	Evas_Object *frame=NULL, *list=NULL, *button_list=NULL, *button=NULL, *icon=NULL;
	char *query=NULL, *db_err=NULL;
	int sqlite_res = 0;

	if(account_editor) evas_object_del(account_editor);
	if(cache_editor) evas_object_del(cache_editor);
	account_editor = elm_box_add(settings_area);
		evas_object_size_hint_weight_set(account_editor, 1, 1);
		evas_object_size_hint_align_set(account_editor, -1, -1);

		frame = elm_frame_add(settings_win);
			evas_object_size_hint_weight_set(frame, 1, 1);
			evas_object_size_hint_align_set(frame, -1, -1);
			elm_frame_label_set(frame, _("Accounts..."));

			list = elm_list_add(settings_win);
				evas_object_name_set(list, "accounts_list");
				evas_object_size_hint_weight_set(list, 1, 1);
				evas_object_size_hint_align_set(list, -1, -1);

				query = "SELECT id,name,domain,enabled FROM accounts;";
				sqlite_res = sqlite3_exec(ed_DB, query, accounts_list_insert, list, &db_err);
        			if(sqlite_res != 0) {
               				printf("Can't run %s: %s\n", query, db_err);
					sqlite3_free(db_err);
        			}

			elm_list_go(list);
			evas_object_show(list);
			elm_frame_content_set(frame, list);
		evas_object_show(frame);

		elm_box_pack_end(account_editor, frame);


		button_list = elm_box_add(settings_area);
			elm_box_horizontal_set(button_list, 1);
			elm_box_homogenous_set(button_list, 1);

			evas_object_size_hint_weight_set(button_list, 1, 0);
			evas_object_size_hint_align_set(button_list, -1, 0);

			// add contact
			icon = elm_icon_add(settings_win);
				elm_icon_standard_set(icon, "apps");
			evas_object_show(icon);

			button = elm_button_add(settings_win);
				elm_button_icon_set(button, icon);
				elm_button_label_set(button, _("Add"));
				evas_object_smart_callback_add(button, "clicked", on_account_add, list);
				evas_object_size_hint_weight_set(button, 1, 1);
				evas_object_size_hint_align_set(button, -1, 0);
			elm_box_pack_end(button_list, button);
			evas_object_show(button);

			icon = elm_icon_add(settings_win);
				elm_icon_standard_set(icon, "edit");
			evas_object_show(icon);

			button = elm_button_add(settings_win);
				elm_button_icon_set(button, icon);
				elm_button_label_set(button, _("Edit"));
				evas_object_smart_callback_add(button, "clicked", on_account_edit, list);
				evas_object_size_hint_weight_set(button, 1, 1);
				evas_object_size_hint_align_set(button, -1, 0);
			elm_box_pack_end(button_list, button);
			evas_object_show(button);

			// remove contact
			icon = elm_icon_add(settings_win);
				elm_icon_standard_set(icon, "delete");
			evas_object_show(icon);

			button = elm_button_add(settings_win);
				elm_button_icon_set(button, icon);
				elm_button_label_set(button, _("Delete"));
				evas_object_smart_callback_add(button, "clicked", on_account_delete, list);
				evas_object_size_hint_weight_set(button, 1, 1);
				evas_object_size_hint_align_set(button, -1, 0);
			elm_box_pack_end(button_list, button);
			evas_object_show(button);


		elm_box_pack_start(account_editor, button_list);
		evas_object_show(button_list);

	elm_box_pack_start(settings_area, account_editor);
	evas_object_show(account_editor);

}

static int cache_posts_count(void *user_data, int argc, char **argv, char **azColName) {
	Evas_Object *label = (Evas_Object*)user_data;
	int count=0, res=0;
	char *label_str=NULL;

	/*	In this SQL context, these are the columns:
			argv[0] == count of messages
	*/

	count = atoi(argv[0]);
	if(count == 0)
		elm_label_label_set(label, _("No posts<br>in cache."));
	else {
		res = asprintf(&label_str, _("Currently remembering<br>%d posts."), count);
		if(res != -1) {
			elm_label_label_set(label, label_str);
			free(label_str);
		}
	}

	return(0);
}

void cache_posts_forget(void *data, Evas_Object *obj, void *event_info) {
	char *query = "DELETE FROM posts;", *db_err=NULL;
	int sqlite_res = 0;
	Evas_Object *label = (Evas_Object*)data;

	sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
        if(sqlite_res != 0) {
             	printf("Can't run %s: %s\n", query, db_err);
		sqlite3_free(db_err);
        }

	elm_label_label_set(label, _("No posts<br>in cache."));
}


static int cache_messages_count(void *user_data, int argc, char **argv, char **azColName) {
	Evas_Object *label = (Evas_Object*)user_data;
	int count=0, res=0;
	char *label_str=NULL;

	/*	In this SQL context, these are the columns:
			argv[0] == count of messages
	*/

	count = atoi(argv[0]);
	if(count == 0)
		elm_label_label_set(label, _("No messages<br>in cache."));
	else {
		res = asprintf(&label_str, _("Currently remembering<br>%d messages."), count);
		if(res != -1) {
			elm_label_label_set(label, label_str);
			free(label_str);
		}
	}

	return(0);
}

void cache_messages_forget(void *data, Evas_Object *obj, void *event_info) {
	char *query = "DELETE FROM messages;", *db_err=NULL;
	int sqlite_res = 0;
	Evas_Object *label = (Evas_Object*)data;

	sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
        if(sqlite_res != 0) {
             	printf("Can't run %s: %s\n", query, db_err);
		sqlite3_free(db_err);
        }

	elm_label_label_set(label, _("No messages<br>in cache."));
}

void cache_messages_max_set(Evas_Object *label) {
	char *count=NULL;
	int res=0;

	res = asprintf(&count, " %3d  ", MAX_MESSAGES);
	if(res != -1) {
		elm_label_label_set(label, count);
		free(count);
		gconf_client_set_int(conf_client, "/apps/elmdentica/max_messages", MAX_MESSAGES, NULL);
	}
}

void cache_messages_max_decrease(void *label, Evas_Object *obj, void *event_info) {
	if(MAX_MESSAGES <= 10)
		MAX_MESSAGES=0;
	else
		MAX_MESSAGES-=10;

	cache_messages_max_set((Evas_Object*)label);
}

void cache_messages_max_increase(void *label, Evas_Object *obj, void *event_info) {
	if(MAX_MESSAGES >= 90)
		MAX_MESSAGES=100;
	else
		MAX_MESSAGES+=10;

	cache_messages_max_set((Evas_Object*)label);
}

void on_settings_cache(void *data, Evas_Object *toolbar, void *event_info) {
	Evas_Object *label=NULL, *button=NULL, *box=NULL;
	int sqlite_res = 0;
	char *query = NULL, *db_err = NULL, *count=NULL;

	if(account_editor) evas_object_del(account_editor);
	if(cache_editor) evas_object_del(cache_editor);

	cache_editor = elm_table_add(settings_area);
		evas_object_size_hint_weight_set(cache_editor, 1, 1);
		evas_object_size_hint_align_set(cache_editor, -1, -1);

		label = elm_label_add(settings_win);
			evas_object_size_hint_weight_set(label, 1, 1);
			evas_object_size_hint_align_set(label, 0.5, 0.5);
			query="SELECT COUNT(*) FROM messages";
			sqlite_res = sqlite3_exec(ed_DB, query, cache_messages_count, label, &db_err);
        		if(sqlite_res != 0) {
               			printf("Can't run %s: %s\n", query, db_err);
				sqlite3_free(db_err);
        		}
			evas_object_show(label);
		elm_table_pack(cache_editor, label, 0, 0, 1, 1);

		button = elm_button_add(settings_win);
			evas_object_size_hint_weight_set(button, 1, 1);
			evas_object_size_hint_align_set(button, -1, 0.5);
			elm_button_label_set(button, "Forget all");
			evas_object_smart_callback_add(button, "clicked", cache_messages_forget, label);
			evas_object_show(button);
		elm_table_pack(cache_editor, button, 1, 0, 1, 1);

		label = elm_label_add(settings_win);
			evas_object_size_hint_weight_set(label, 1, 1);
			evas_object_size_hint_align_set(label, 0.5, 0.5);
			query="SELECT COUNT(*) FROM posts";
			sqlite_res = sqlite3_exec(ed_DB, query, cache_posts_count, label, &db_err);
        		if(sqlite_res != 0) {
               			printf("Can't run %s: %s\n", query, db_err);
				sqlite3_free(db_err);
        		}
			evas_object_show(label);
		elm_table_pack(cache_editor, label, 0, 1, 1, 1);

		button = elm_button_add(settings_win);
			evas_object_size_hint_weight_set(button, 1, 1);
			evas_object_size_hint_align_set(button, -1, 0.5);
			elm_button_label_set(button, "Forget all");
			evas_object_smart_callback_add(button, "clicked", cache_posts_forget, label);
			evas_object_show(button);
		elm_table_pack(cache_editor, button, 1, 1, 1, 1);

		label = elm_label_add(settings_win);
			evas_object_size_hint_weight_set(label, 1, 1);
			evas_object_size_hint_align_set(label, 0.5, 0.5);
			elm_label_label_set(label, _("Displayed messages:"));
			evas_object_show(label);
		elm_table_pack(cache_editor, label, 0, 2, 1, 1);

		box = elm_box_add(settings_win);
			evas_object_size_hint_weight_set(box, 1, 1);
			evas_object_size_hint_align_set(box, -1, 0.5);
			elm_box_horizontal_set(box, TRUE);

			label = elm_label_add(settings_win);
				evas_object_size_hint_weight_set(label, 1, 1);
				evas_object_size_hint_align_set(label, 0.5, 0.5);
				sqlite_res = asprintf(&count, " %3d  ", MAX_MESSAGES);
				if(sqlite_res != -1) {
					elm_label_label_set(label, count);
					free(count);
					evas_object_show(label);
				}
				sqlite_res = 0;
			elm_box_pack_end(box, label);

			button = elm_button_add(settings_win);
				evas_object_size_hint_weight_set(button, 1, 1);
				evas_object_size_hint_align_set(button, -1, -1);
				elm_button_label_set(button, "-10");
				evas_object_smart_callback_add(button, "clicked", cache_messages_max_decrease, label);
				evas_object_show(button);
			elm_box_pack_start(box, button);

			button = elm_button_add(settings_win);
				evas_object_size_hint_weight_set(button, 1, 1);
				evas_object_size_hint_align_set(button, -1, -1);
				elm_button_label_set(button, "+10");
				evas_object_smart_callback_add(button, "clicked", cache_messages_max_increase, label);
				evas_object_show(button);
			elm_box_pack_end(box, button);

			evas_object_show(box);
		elm_table_pack(cache_editor, box, 1, 2, 1, 1);

		elm_box_pack_start(settings_area, cache_editor);
	evas_object_show(cache_editor);
}

void on_settings(void *data, Evas_Object *obj, void *event_info) {
	Evas_Object *bg=NULL, *box=NULL, *toolbar=NULL, *icon=NULL;
	Elm_Toolbar_Item *item=NULL;

	current_account = 0;
	current_account_type = ACCOUNT_TYPE_NONE;

	settings_win = elm_win_add(NULL, _("Settings"), ELM_WIN_BASIC);
	elm_win_title_set(settings_win, _("ElmDentica settings..."));
	evas_object_smart_callback_add(settings_win, "delete-request", settings_win_del, NULL);

	/* Background */
	bg = elm_bg_add(settings_win);
	evas_object_size_hint_weight_set(bg, 1.0, 1.0);
	elm_win_resize_object_add(settings_win, bg);
	evas_object_show(bg);

	/* Vertical Box */
	box = elm_box_add(settings_win);
	evas_object_size_hint_weight_set(box, 1, 1);
	evas_object_size_hint_align_set(box, -1, -1);
	elm_win_resize_object_add(settings_win, box);

		/* Frame for settings component */
		settings_area = elm_box_add(settings_win);
			evas_object_size_hint_weight_set(settings_area, 1, 1);
			evas_object_size_hint_align_set(settings_area, -1, -1);
			elm_box_pack_end(box, settings_area);
		evas_object_show(settings_area);

		toolbar = elm_toolbar_add(settings_win);
		evas_object_size_hint_align_set(toolbar, -1, 0);

			icon = elm_photo_add(settings_win);
				elm_photo_file_set(icon, "head.png");
				evas_object_show(icon);
			item = elm_toolbar_item_add(toolbar, icon, _("Accounts"), on_settings_accounts, NULL);

			icon = elm_photo_add(settings_win);
				elm_icon_standard_set(icon, "apps");
				evas_object_show(icon);
			elm_toolbar_item_add(toolbar, icon, _("Cache"), on_settings_cache, NULL);

		elm_box_pack_start(box, toolbar);
		evas_object_show(toolbar);
		elm_toolbar_item_select(item);

	evas_object_show(box);
	evas_object_resize(settings_win, 300, 300);
	evas_object_show(settings_win);
}
