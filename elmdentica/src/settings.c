/* vim:tabstop=4
 * Copyright © 2009-2010 Rui Miguel Silva Seabra <rms@1407.org>
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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib/gprintf.h>

#include <Elementary.h>
#include <Ecore_File.h>
#include <Ecore_X.h>
#include <Eet.h>

#include <sqlite3.h>

#include <curl.h>

#include "gettext.h"
#define _(string) gettext (string)

#include "curl.h"
#include "settings.h"
#include "elmdentica.h"

Evas_Object *settings_win=NULL, *settings_area=NULL, *account_editor=NULL, *cache_editor=NULL, *options_editor=NULL,
	    *user_data_dialog=NULL, *screen_name_entry=NULL, *password_entry=NULL, *gag_editor=NULL,
	    *domain_data_dialog=NULL, *apiroot_entry=NULL, *type_entry=NULL, *secure_entry=NULL,
	    *domain_entry=NULL, *base_url_entry=NULL, *enabled_entry=NULL, *receive_entry=NULL, *send_entry=NULL, *auto_update=NULL, *auto_update_timeout=NULL;

Elm_List_Item * current_account_li=NULL, *current_domain_li=NULL;

extern struct sqlite3 *ed_DB;
extern int debug;

Eet_File *conf_file=NULL;
char *conf_file_path=NULL;

short current_account_type = ACCOUNT_TYPE_NONE;
int current_account = 0;
int current_gag = 0;
extern char * home;
extern char * url_post;
extern char * url_friends;
extern Eina_Bool ed_statuses_update_time(void *data);

const char * browserNames[] = {
        "XDG Open",
        "Ventura",
        "Midori",
        "Woosh",
        "Dillo",
        "Eve",
	};
const char * browser_cmnds[] = {
        "/usr/bin/xdg-open %s &",
        "/usr/bin/ventura %s &",
        "/usr/bin/midori %s &",
        "/usr/bin/woosh -u %s &",
        "/usr/bin/dillo %s &",
        "/usr/bin/eve %s &",
	};
int browsersIndex=5;

const int timeout_values[] = {
        0,60,120,300,600,900,1800,3600
};
const int timeoutsIndex=7;

Settings *settings=NULL;

Eina_Hash *eet_eina_hash_add(Eina_Hash *hash, const char *key, const void *data) {
	if (!hash) hash = eina_hash_string_superfast_new(NULL);
	if (!hash) return NULL;

	eina_hash_add(hash, key, data);
	return hash;
}

static Eet_Data_Descriptor_Class settings_edd_class = {
	EET_DATA_DESCRIPTOR_CLASS_VERSION,
	"settings_edd_class",
	sizeof(Settings), {
		NULL,
		NULL,
		(char *(*)(const char *)) eina_stringshare_add,
		(void (*)(const char *)) eina_stringshare_del,
		(void *(*)(void *)) eina_list_next,
		(void *(*)(void *l, void *d)) eina_list_append,
		(void *(*)(void *)) eina_list_data_get,
		(void *(*)(void *)) eina_list_free,
		(void  (*) (void *, int (*) (void *, const char *, void *, void *), void *)) eina_hash_foreach,
		(void * (*) (void *, const char *, void *)) eet_eina_hash_add,
		(void  (*) (void *)) eina_hash_free,
		NULL,
		NULL,
		NULL,
		NULL
	}
};

Eet_Data_Descriptor *settings_edd=NULL;

extern int debug;
extern CURL * user_agent;

void settings_win_del(void *data, Evas_Object *obj, void *event_info) {
        evas_object_del(obj);
}

void user_data_dialog_win_del(void *data, Evas_Object *obj, void *event_info) {
       	evas_object_del(user_data_dialog);
}

void on_account_selected(void *data, Evas_Object *account_list, void *event_info) {
	int *id = (int*)data;

	current_account = *id;
	if (debug) printf("Selected account: %d\n", current_account);
}

extern Eina_Bool ed_statuses_update_interval(void *data);

const char *timeout_names(double val, Eina_Bool longval) {
	switch((int)val) {
		case 0: { return longval?_("never"):_("0 s"); break; }
		case 1: { return longval?_("1 minute"):_("1 min"); break; }
		case 2: { return longval?_(_("2 minutes")):_(_("2 min")); break; }
		case 3: { return longval?_("5 minutes"):_("5 min"); break; }
		case 4: { return longval?_(_("10 minutes")):_(_("10 min")); break; }
		case 5: { return longval?_("15 minutes"):_("15 min"); break; }
		case 6: { return longval?_("30 minutes"):_("30 min"); break; }
		case 7: { return longval?_("1 hour"):_("1 h"); break; }
		default: { return ""; }
	}
}

void on_update_timeout_changed(void* data, Evas_Object *obj, void *event_info) {
	int val = (int)elm_slider_value_get(obj);
	elm_slider_unit_format_set(obj, timeout_names(val, EINA_TRUE));

	if( !val && settings->update_interval_val) // disable when changing to zero
		ecore_timer_del(settings->update_timer);
	if( val && !settings->update_interval_val ) // enable when changing from zero
		settings->update_timer = ecore_timer_add(settings->update_interval, ed_statuses_update_interval, NULL);
	
	settings->update_interval = timeout_values[val];
	settings->update_interval_val = val;
	ecore_timer_interval_set(settings->update_timer, settings->update_interval);
	ecore_timer_freeze(settings->update_timer);
	ecore_timer_thaw(settings->update_timer);

	if(val)
		elm_object_text_set(obj, _("Every: "));
	else
		elm_object_text_set(obj, "");
}

static const char* indicator_format_func(double val) {
	int i = (int)val;
	return timeout_names(i, EINA_FALSE);
}

void on_account_enabled_toggle(void *data, Evas_Object *check, void *event_info) {
	int *id = (int*)data;
	int sqlite_res = 0;
	char *query=NULL, *db_err=NULL;

	if(elm_check_state_get(check)) {
		sqlite_res = asprintf(&query, "UPDATE accounts set enabled='%d' where id='%d';", 1, *id);
	} else {
		sqlite_res = asprintf(&query, "UPDATE accounts set enabled='%d' where id='%d';", 0, *id);
		ed_curl_ua_cleanup(*id);
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
	char *account_id=NULL, *query=NULL, *db_err=NULL, *tmp=NULL;
	char *screen_name=NULL, *password=NULL, *domain=NULL, *base_url=NULL, *proto=NULL;
	int enabled=1, port=443, sqlite_res = 0, *id=NULL, receive=1, send=1, res=0;
	Elm_List_Item *li;
	Eina_Strbuf *buf = NULL;

	buf = eina_strbuf_new();

	tmp = elm_entry_markup_to_utf8(elm_entry_entry_get(screen_name_entry));
	eina_strbuf_append(buf, tmp);
	eina_strbuf_replace_all(buf, "<br>", "");
	screen_name = eina_strbuf_string_steal(buf);
	eina_strbuf_reset(buf);
	free(tmp); tmp = NULL;

	tmp = elm_entry_markup_to_utf8(elm_entry_entry_get(password_entry));
	eina_strbuf_append(buf, tmp);
	eina_strbuf_replace_all(buf, "<br>", "");
	password = eina_strbuf_string_steal(buf);
	eina_strbuf_reset(buf);
	free(tmp); tmp = NULL;

	tmp = elm_entry_markup_to_utf8(elm_entry_entry_get(domain_entry));
	eina_strbuf_append(buf, tmp);
	eina_strbuf_replace_all(buf, "<br>", "");
	domain = eina_strbuf_string_steal(buf);
	eina_strbuf_reset(buf);
	free(tmp); tmp = NULL;

	tmp = elm_entry_markup_to_utf8(elm_entry_entry_get(base_url_entry));
	eina_strbuf_append(buf, tmp);
	eina_strbuf_replace_all(buf, "<br>", "");
	base_url = eina_strbuf_string_steal(buf);
	eina_strbuf_reset(buf);
	free(tmp); tmp = NULL;

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

void on_account_type_chose_identica(void *data, Evas_Object *obj, void *event_info) {
	Evas_Object * eo = NULL;
	Evas * evas = NULL;

	current_account_type=ACCOUNT_TYPE_STATUSNET;
	evas = evas_object_evas_get(obj);
	if(evas) {
		eo = evas_object_name_find(evas, "secure_entry");
		if(eo) elm_toggle_state_set(eo, TRUE);

		eo = evas_object_name_find(evas, "domain_entry");
		if(eo) elm_entry_entry_set(eo, "identi.ca");

		eo = evas_object_name_find(evas, "base_url_entry");
		if(eo) elm_entry_entry_set(eo, "/api");
	}
}

void on_account_type_chosen(void *data, Evas_Object *obj, void *event_info) {
	Evas_Object *type_hoversel = (Evas_Object*)data;
	switch(current_account_type) {
		case ACCOUNT_TYPE_STATUSNET:
		default: { elm_object_text_set(type_hoversel, "StatusNet"); break; }
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
			elm_table_homogeneous_set(table, TRUE);
			evas_object_size_hint_weight_set(table, 1, 1);
			evas_object_size_hint_align_set(table, -1, 0);

			frame = elm_frame_add(user_data_dialog);
				evas_object_size_hint_weight_set(frame, 1, 1);
				evas_object_size_hint_align_set(frame, -1, 0);
				elm_object_text_set(frame, _("Screen name"));

				screen_name_entry = elm_entry_add(user_data_dialog);
					evas_object_name_set(screen_name_entry, "screen_name_entry");
					elm_entry_single_line_set(screen_name_entry, 1);
					elm_entry_entry_set(screen_name_entry, screen_name);
					elm_frame_content_set(frame, screen_name_entry);
				evas_object_show(screen_name_entry);

				elm_table_pack(table, frame, 0, 0, 1, 1);
			evas_object_show(frame);

			frame = elm_frame_add(user_data_dialog);
				evas_object_size_hint_weight_set(frame, 1, 1);
				evas_object_size_hint_align_set(frame, -1, 0);
				elm_object_text_set(frame, _("Password"));

				password_entry = elm_entry_add(user_data_dialog);
					evas_object_name_set(password_entry, "password_entry");
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
					case ACCOUNT_TYPE_STATUSNET: { 
						current_account_type=ACCOUNT_TYPE_STATUSNET;
						elm_object_text_set(type_entry, "StatusNet");
						break;
					}
					default:	{
						current_account_type=ACCOUNT_TYPE_STATUSNET;
						elm_object_text_set(type_entry, _("Type"));
						break;
					}
				}
				elm_hoversel_hover_parent_set(type_entry, table);

				elm_hoversel_hover_begin(type_entry);
					acc_type = elm_hoversel_item_add(type_entry, "Identi.ca", NULL, ELM_ICON_NONE, on_account_type_chose_identica, type_entry);
					acc_type = elm_hoversel_item_add(type_entry, "StatusNet", NULL, ELM_ICON_NONE, on_account_type_chose_statusnet, type_entry);
					evas_object_smart_callback_add(type_entry, "selected", on_account_type_chosen, type_entry);
				elm_hoversel_hover_end(type_entry);

				elm_table_pack(table, type_entry, 0, 1, 1, 1);
			evas_object_show(type_entry);

			secure_entry = elm_toggle_add(user_data_dialog);
				evas_object_name_set(secure_entry, "secure_entry");
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
				elm_object_text_set(frame, _("Domain"));

				domain_entry = elm_entry_add(user_data_dialog);
					evas_object_name_set(domain_entry, "domain_entry");
					elm_entry_single_line_set(domain_entry, 1);
					elm_entry_entry_set(domain_entry, domain);
					elm_frame_content_set(frame, domain_entry);
				evas_object_show(domain_entry);

				elm_table_pack(table, frame, 0, 2, 1, 1);
			evas_object_show(frame);

			frame = elm_frame_add(user_data_dialog);
				evas_object_size_hint_weight_set(frame, 1, 1);
				evas_object_size_hint_align_set(frame, -1, 0);
				elm_object_text_set(frame, _("Base URL"));

				base_url_entry = elm_entry_add(user_data_dialog);
					evas_object_name_set(base_url_entry, "base_url_entry");
					elm_entry_single_line_set(base_url_entry, 1);
					elm_entry_entry_set(base_url_entry, base_url);
					elm_frame_content_set(frame, base_url_entry);
				evas_object_show(base_url_entry);

				elm_table_pack(table, frame, 1, 2, 1, 1);
			evas_object_show(frame);

			receive_entry = elm_check_add(user_data_dialog);
				evas_object_size_hint_weight_set(receive_entry, 1, 1);
				evas_object_size_hint_align_set(receive_entry, -1, 0);
				elm_object_text_set(receive_entry, _("Receive?"));
				if(receive)
					elm_check_state_set(receive_entry, TRUE);
				else
					elm_check_state_set(receive_entry, FALSE);

				elm_table_pack(table, receive_entry, 0, 3, 1, 1);
			evas_object_show(receive_entry);

			send_entry = elm_check_add(user_data_dialog);
				evas_object_size_hint_weight_set(send_entry, 1, 1);
				evas_object_size_hint_align_set(send_entry, -1, 0);
				elm_object_text_set(send_entry, _("Send?"));
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
		elm_box_homogeneous_set(button_list, TRUE);

		button = elm_button_add(user_data_dialog);
			evas_object_size_hint_weight_set(button, 1, 1);
			evas_object_size_hint_align_set(button, -1, 0);

			elm_object_text_set(button, _("OK"));
			evas_object_smart_callback_add(button, "clicked", on_user_data_ok, account_list);
			elm_box_pack_end(button_list, button);
		evas_object_show(button);

		button = elm_button_add(user_data_dialog);
			evas_object_size_hint_weight_set(button, 1, 1);
			evas_object_size_hint_align_set(button, -1, 0);
			elm_object_text_set(button, _("Cancel"));
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

	type = ACCOUNT_TYPE_STATUSNET;

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
		free(key);
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
	if(options_editor) evas_object_del(options_editor);
	if(gag_editor) evas_object_del(gag_editor);

	account_editor = elm_box_add(settings_area);
		evas_object_size_hint_weight_set(account_editor, 1, 1);
		evas_object_size_hint_align_set(account_editor, -1, -1);

		frame = elm_frame_add(settings_win);
			evas_object_size_hint_weight_set(frame, 1, 1);
			evas_object_size_hint_align_set(frame, -1, -1);
			elm_object_text_set(frame, _("Accounts..."));

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
			elm_box_homogeneous_set(button_list, 1);

			evas_object_size_hint_weight_set(button_list, 1, 0);
			evas_object_size_hint_align_set(button_list, -1, 0);

			// add contact
			icon = elm_icon_add(settings_win);
				elm_icon_standard_set(icon, "file");
			evas_object_show(icon);

			button = elm_button_add(settings_win);
				elm_button_icon_set(button, icon);
				elm_object_text_set(button, _("Add"));
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
				elm_object_text_set(button, _("Edit"));
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
				elm_object_text_set(button, _("Delete"));
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
		elm_object_text_set(label, _("No posts<br>in cache."));
	else {
		res = asprintf(&label_str, _("Currently remembering<br>%d posts."), count);
		if(res != -1) {
			elm_object_text_set(label, label_str);
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

	elm_object_text_set(label, _("No posts<br>in cache."));
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
		elm_object_text_set(label, _("No messages<br>in cache."));
	else {
		res = asprintf(&label_str, _("Currently remembering<br>%d messages."), count);
		if(res != -1) {
			elm_object_text_set(label, label_str);
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

	elm_object_text_set(label, _("No messages<br>in cache."));
}

void cache_messages_max_set(Evas_Object *label) {
	char *count=NULL;
	int res=0;

	res = asprintf(&count, " %3d  ", settings->max_messages);
	if(res != -1) {
		elm_object_text_set(label, count);
		free(count);

	}
}

void cache_messages_max_change(void *data, Evas_Object *slider, void *event_info) {
	settings->max_messages=elm_slider_value_get(slider);
}

void on_settings_cache(void *data, Evas_Object *toolbar, void *event_info) {
	Evas_Object *label=NULL, *button=NULL, *slider=NULL;
	int sqlite_res = 0;
	char *query = NULL, *db_err = NULL;

	if(account_editor) evas_object_del(account_editor);
	if(cache_editor) evas_object_del(cache_editor);
	if(options_editor) evas_object_del(options_editor);
	if(gag_editor) evas_object_del(gag_editor);

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
			elm_object_text_set(button, "Forget all");
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
			elm_object_text_set(button, "Forget all");
			evas_object_smart_callback_add(button, "clicked", cache_posts_forget, label);
			evas_object_show(button);
		elm_table_pack(cache_editor, button, 1, 1, 1, 1);

		slider = elm_slider_add(settings_win);
			evas_object_size_hint_weight_set(slider, 1, 1);
			evas_object_size_hint_align_set(slider, -1, 0.5);
			elm_slider_horizontal_set(slider, EINA_TRUE);

			elm_object_text_set(slider, _("Displayed messages"));

			elm_slider_unit_format_set(slider, "%0.f");
			elm_slider_min_max_set(slider, 0.0, 1000.0);
			elm_slider_value_set(slider, settings->max_messages);
			evas_object_smart_callback_add(slider, "changed", cache_messages_max_change, NULL);

			elm_table_pack(cache_editor, slider, 0, 2, 2, 1);
		evas_object_show(slider);

		elm_box_pack_start(settings_area, cache_editor);
	evas_object_show(cache_editor);
}

static void settings_choose_browser(void *data, Evas_Object *hoversel, void *event_info) {
	int b = (int)(long)data;

	if(b == BROWSER_XDG)
		settings->browser=BROWSER_XDG;
	else
		settings->browser=b;

	settings->browser_name = strdup(browserNames[b]);
	settings->browser_cmd = strdup(browser_cmnds[b]);
	elm_object_text_set(hoversel, settings->browser_name);
}

Evas_Object *settings_browser_hoversel(void) {
	Evas_Object *hoversel=NULL;
	struct stat buf;
	int have_browser=0;

	hoversel=elm_hoversel_add(settings_area);
		evas_object_size_hint_weight_set(hoversel, 1, 1);
		evas_object_size_hint_align_set(hoversel, -1, -1);

		elm_hoversel_hover_begin(hoversel);
		if(stat("/usr/bin/xdg-open", &buf) == 0) {
			elm_hoversel_item_add(hoversel, browserNames[BROWSER_XDG], NULL, ELM_ICON_NONE, settings_choose_browser, (void*)BROWSER_XDG);
			have_browser=1;
		}
		if(stat("/usr/bin/ventura", &buf) == 0) {
			elm_hoversel_item_add(hoversel, browserNames[BROWSER_VENTURA], NULL, ELM_ICON_NONE, settings_choose_browser, (void*)BROWSER_VENTURA);
			have_browser=1;
		}
		if(stat("/usr/bin/woosh", &buf) == 0) {
			elm_hoversel_item_add(hoversel, browserNames[BROWSER_WOOSH], NULL, ELM_ICON_NONE, settings_choose_browser, (void*)BROWSER_WOOSH);
			have_browser=1;
		}
		if(stat("/usr/bin/midori", &buf) == 0) {
			elm_hoversel_item_add(hoversel, browserNames[BROWSER_MIDORI], NULL, ELM_ICON_NONE, settings_choose_browser, (void*)BROWSER_MIDORI);
			have_browser=1;
		}
		if(stat("/usr/bin/dillo", &buf) == 0) {
			elm_hoversel_item_add(hoversel, browserNames[BROWSER_DILLO], NULL, ELM_ICON_NONE, settings_choose_browser, (void*)BROWSER_DILLO);
			have_browser=1;
		}
		if(stat("/usr/bin/eve", &buf) == 0) {
			elm_hoversel_item_add(hoversel, browserNames[BROWSER_EVE], NULL, ELM_ICON_NONE, settings_choose_browser, (void*)BROWSER_EVE);
			have_browser=1;
		}
		elm_hoversel_hover_end(hoversel);

		if(have_browser) {
			if(settings->browser >= 0 && settings->browser <= browsersIndex)
				elm_object_text_set(hoversel, browserNames[settings->browser]);
			else
				elm_object_text_set(hoversel, _("Please choose a browser"));
		} else
			elm_object_text_set(hoversel, _("No supported browser available"));

	evas_object_show(hoversel);

	return(hoversel);
}

void on_toggle_online_changed(void *data, Evas_Object *toggle, void *event_info) {
	if(elm_toggle_state_get(toggle))
		settings->online=1;
	else
		settings->online=0;
}

void on_toggle_timestamps(void *data, Evas_Object *toggle, void *event_info) {
	if(elm_toggle_state_get(toggle)) {
		settings->rel_timestamps=EINA_TRUE;
		settings->rel_ts_timer = ecore_timer_add(60, ed_statuses_update_time, NULL);
	} else {
		settings->rel_timestamps=EINA_FALSE;
		if(settings->rel_ts_timer) ecore_timer_del(settings->rel_ts_timer);
	}
}

void on_settings_options(void *data, Evas_Object *toolbar, void *event_info) {
	Evas_Object *frame=NULL, *hoversel=NULL, *toggle=NULL, *update_table;

	if(account_editor) evas_object_del(account_editor);
	if(cache_editor) evas_object_del(cache_editor);
	if(options_editor) evas_object_del(options_editor);
	if(gag_editor) evas_object_del(gag_editor);

	options_editor = elm_table_add(settings_area);
		evas_object_size_hint_weight_set(options_editor, 1, 1);
		evas_object_size_hint_align_set(options_editor, -1, 0);

		frame = elm_frame_add(settings_area);
			elm_object_text_set(frame, _("Preferred browser..."));
			hoversel = settings_browser_hoversel();
			elm_hoversel_hover_parent_set(hoversel, settings_area);
			elm_frame_content_set(frame, hoversel);
			elm_table_pack(options_editor, frame, 0, 0, 1, 1);
		evas_object_show(frame);
		
		frame = elm_frame_add(settings_area);
			elm_object_text_set(frame, _("Online mode..."));
			toggle = elm_toggle_add(settings_area);
			elm_toggle_states_labels_set(toggle, _("Online"), _("Offline"));
			elm_toggle_state_set(toggle, settings->online);
			evas_object_smart_callback_add(toggle, "changed", on_toggle_online_changed, NULL);
			elm_frame_content_set(frame, toggle);
			elm_table_pack(options_editor, frame, 1, 0, 1, 1);
		evas_object_show(frame);
		
		frame = elm_frame_add(settings_area);
			elm_object_text_set(frame, _("Timestamps..."));
			toggle = elm_toggle_add(settings_area);
			elm_toggle_states_labels_set(toggle, _("Relative"), _("Absolute"));
			elm_toggle_state_set(toggle, settings->rel_timestamps);
			evas_object_smart_callback_add(toggle, "changed", on_toggle_timestamps, NULL);
			elm_frame_content_set(frame, toggle);
			elm_table_pack(options_editor, frame, 0, 1, 1, 1);
		evas_object_show(frame);

		frame = elm_frame_add(settings_area);
			elm_object_text_set(frame, _("Auto updates"));
			update_table = elm_table_add(settings_area);

			auto_update_timeout = elm_slider_add(settings_area);
				elm_slider_min_max_set(auto_update_timeout, 0.0, timeoutsIndex);
				elm_slider_value_set(auto_update_timeout, settings->update_interval_val);
				elm_slider_unit_format_set(auto_update_timeout, timeout_names(settings->update_interval_val, EINA_TRUE));
				if(settings->update_interval_val)
					elm_object_text_set(auto_update_timeout, _("Every: "));
				else
					elm_object_text_set(auto_update_timeout, "");
				elm_slider_indicator_format_function_set(auto_update_timeout, indicator_format_func, NULL);
				evas_object_smart_callback_add(auto_update_timeout, "delay,changed", on_update_timeout_changed, NULL);
				elm_table_pack(update_table, auto_update_timeout, 1, 0, 1, 1);
				evas_object_show(auto_update_timeout);
				elm_frame_content_set(frame, update_table);
			evas_object_show(update_table);
		elm_table_pack(options_editor, frame, 0, 2, 2, 1);
		elm_box_pack_start(settings_area, options_editor);
		evas_object_show(frame);

	evas_object_show(options_editor);
}

void on_gag_enabled_toggle(void *data, Evas_Object *check, void *event_info) {
	int *id = (int*)data;
	int sqlite_res = 0;
	char *query=NULL, *db_err=NULL;

	if(elm_check_state_get(check)) {
		sqlite_res = asprintf(&query, "UPDATE gag set enabled='%d' where id='%d';", 1, *id);
	} else {
		sqlite_res = asprintf(&query, "UPDATE gag set enabled='%d' where id='%d';", 0, *id);
	}

	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
		if(sqlite_res != 0)
			printf("Can't %s: %d = %s\n", query, sqlite_res, db_err);
		sqlite3_free(db_err);
		free(query);
	}

}

void on_gag_selected(void *data, Evas_Object *gag_list, void *event_info) {
	int *id = (int*)data;

	current_gag = *id;
	if (debug) printf("Selected gag: %d\n", current_gag);
}

static int gag_list_insert(void *user_data, int argc, char **argv, char **azColName) {
	Evas_Object *list = (Evas_Object*)user_data, *check=NULL;
	Elm_List_Item * item=NULL;
	int *id = calloc(1, sizeof(int));
	char *pattern=NULL;
	int enabled=0;

	/*	In this SQL context, these are the columns:
			argv[0] == id
			argv[1] == enabled
			argv[2] == pattern
	*/

	*id = atoi(argv[0]);
	enabled = atoi(argv[1]);
    pattern = strndup(argv[2], NAME_MAX);

	check = elm_check_add(settings_win);
	if(enabled)
		elm_check_state_set(check, TRUE);
	else
		elm_check_state_set(check, FALSE);

	evas_object_smart_callback_add(check, "changed", on_gag_enabled_toggle, id);
	evas_object_show(check);

	item = elm_list_item_append(list, pattern, check, NULL, on_gag_selected, id);

	return(0);
}

void on_gag_dialog_win_del(void *data, Evas_Object *obj, void *event_info) {
	evas_object_del(obj);
}

void on_gag_pattern_add(void *data, Evas_Object *obj, void *event_info) {
	Evas *e = evas_object_evas_get(obj);
	Evas_Object *gag_list = evas_object_name_find(e, "gag_list"), *gag_pattern = evas_object_name_find(e, "gag-pattern");
	const Eina_List *list_items=NULL, *l=NULL;
	Elm_List_Item *li=NULL;
	char *query, *db_err=NULL;
	char *pattern = elm_entry_markup_to_utf8(elm_entry_entry_get(gag_pattern));
	int res=0;

	res = asprintf(&query, "INSERT INTO gag (enabled, pattern) VALUES ( 1, '%s');", pattern);
	if(res != -1) {
		res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
		if(res != 0)
			printf("Can't %s: %d = %s\n", query, res, db_err);
		else {
			free(query);

			list_items = elm_list_items_get(gag_list);
			EINA_LIST_FOREACH(list_items, l, li)
				elm_list_item_del(li);

			query = "SELECT id,enabled,pattern FROM gag;";
			res = sqlite3_exec(ed_DB, query, gag_list_insert, gag_list, &db_err);
			if(res != 0) {
				printf("Can't run %s: %s\n", query, db_err);
				sqlite3_free(db_err);
			}

			elm_list_go(gag_list);
			query=NULL;
		}
		sqlite3_free(db_err);
		if(query) free(query);
	}

	evas_object_del((Evas_Object*)data);
	if(pattern) free(pattern);
}

void on_gag_pattern_edit(void *data, Evas_Object *obj, void *event_info) {
	Evas *e = evas_object_evas_get(obj);
	Evas_Object *gag_list = evas_object_name_find(e, "gag_list"), *gag_pattern = evas_object_name_find(e, "gag-pattern");
	Elm_List_Item *li = elm_list_selected_item_get(gag_list);
	char *query, *db_err=NULL;
	char *pattern = elm_entry_markup_to_utf8(elm_entry_entry_get(gag_pattern));
	int res=0;

	res = asprintf(&query, "UPDATE gag SET pattern = '%s' where id = %d;", pattern, current_gag);
	if(res != -1) {
		res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
		if(res != 0)
			printf("Can't %s: %d = %s\n", query, res, db_err);
		else
			elm_list_item_label_set(li, pattern);
		sqlite3_free(db_err);
		free(query);
	}
	evas_object_del((Evas_Object*)data);
	if(pattern) free(pattern);
}

void on_gag_pattern_cancel(void *data, Evas_Object *obj, void *event_info) {
	evas_object_del((Evas_Object*)data);
}

Evas_Object * gag_edit_widgets(char *pattern) {
	Evas_Object *inwin=NULL, *box=NULL, *frame=NULL, *entry=NULL, *buttons=NULL, *button=NULL;

	inwin = elm_win_inwin_add(settings_win);
		elm_object_style_set(inwin, "minimal_vertical");
		evas_object_size_hint_weight_set(inwin, 1, 1);
		evas_object_size_hint_align_set(inwin, -1, 1);

		evas_object_smart_callback_add(inwin, "delete-request", on_gag_dialog_win_del, NULL);

		box = elm_box_add(settings_win);
			evas_object_size_hint_weight_set(box, 1, 1);
			evas_object_size_hint_align_set(box, -1, 1);

			frame = elm_frame_add(settings_win);
				evas_object_size_hint_weight_set(frame, 1, 1);
				evas_object_size_hint_align_set(frame, -1, 1);
				elm_object_text_set(frame, _("Pattern..."));
				entry = elm_entry_add(settings_win);
					evas_object_size_hint_weight_set(entry, 1, 1);
					evas_object_size_hint_align_set(entry, -1, 1);
					evas_object_name_set(entry, "gag-pattern");

					if(!pattern)
						elm_entry_entry_set(entry, "");
					else
						elm_entry_entry_set(entry, pattern);
				elm_frame_content_set(frame, entry);
				elm_box_pack_end(box, frame);
			evas_object_show(frame);

			buttons = elm_box_add(settings_win);
				evas_object_size_hint_weight_set(buttons, 1, 1);
				evas_object_size_hint_align_set(buttons, -1, 1);
				elm_box_horizontal_set(buttons, EINA_TRUE);

				button = elm_button_add(settings_win);
					elm_object_text_set(button, _("OK"));
					if(!pattern)
						evas_object_smart_callback_add(button, "clicked", on_gag_pattern_add, inwin);
					else
						evas_object_smart_callback_add(button, "clicked", on_gag_pattern_edit, inwin);
					elm_box_pack_end(buttons, button);
				evas_object_show(button);

				button = elm_button_add(settings_win);
					elm_object_text_set(button, _("Cancel"));
					evas_object_smart_callback_add(button, "clicked", on_gag_pattern_cancel, inwin);
					elm_box_pack_end(buttons, button);
				evas_object_show(button);

				elm_box_pack_end(box, buttons);
			evas_object_show(buttons);
		evas_object_show(box);

	elm_win_inwin_content_set(inwin, box);

	return(inwin);
}

void on_gag_edit(void *data, Evas_Object *obj, void *event_info) {
	Elm_List_Item *li = elm_list_selected_item_get((Evas_Object*)data);
	Evas_Object *inwin=NULL;
	const char *label=NULL;

	if(li) {
		label = elm_list_item_label_get(li);
	}

	inwin = gag_edit_widgets((char*)label);
	evas_object_show(inwin);
}

void on_gag_add(void *data, Evas_Object *obj, void *event_info) {
	Evas_Object *inwin = gag_edit_widgets(NULL);
	evas_object_show(inwin);
}

void on_gag_delete(void *data, Evas_Object *button, void *event_info) {
	Evas_Object *gag_list=(Evas_Object*)data;
	int sqlite_res = 0;
	char *query=NULL, *db_err=NULL;
	Elm_List_Item *li;

	if(current_gag != 0) {

		sqlite_res = asprintf(&query, "DELETE FROM gag WHERE id='%d';", current_gag);
		if(sqlite_res != -1) {
			if(debug) printf("QUERY: %s\n", query);
			sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
			if(sqlite_res != 0)
				printf("Can't %s: %d = %s\n", query, sqlite_res, db_err);
			sqlite3_free(db_err);
			free(query);

			current_gag = 0;
			li = elm_list_selected_item_get(gag_list);
			if(li) elm_list_item_del(li);
		}
	}
}

void on_settings_gag(void *data, Evas_Object *toolbar, void *event_info) {
	Evas_Object *frame=NULL, *list=NULL, *button_list=NULL, *button=NULL, *icon=NULL;
	char *query=NULL, *db_err=NULL;
	int sqlite_res = 0;

	if(account_editor) evas_object_del(account_editor);
	if(cache_editor) evas_object_del(cache_editor);
	if(options_editor) evas_object_del(options_editor);
	if(gag_editor) evas_object_del(gag_editor);

	gag_editor = elm_box_add(settings_area);
		evas_object_size_hint_weight_set(gag_editor, 1, 1);
		evas_object_size_hint_align_set(gag_editor, -1, -1);

		frame = elm_frame_add(settings_win);
			evas_object_size_hint_weight_set(frame, 1, 1);
			evas_object_size_hint_align_set(frame, -1, -1);
			elm_object_text_set(frame, _("Gag patterns..."));

			list = elm_list_add(settings_win);
				evas_object_name_set(list, "gag_list");
				evas_object_size_hint_weight_set(list, 1, 1);
				evas_object_size_hint_align_set(list, -1, -1);

				query = "SELECT id,enabled,pattern FROM gag;";
				sqlite_res = sqlite3_exec(ed_DB, query, gag_list_insert, list, &db_err);
        			if(sqlite_res != 0) {
               				printf("Can't run %s: %s\n", query, db_err);
					sqlite3_free(db_err);
        			}

			elm_list_go(list);
			evas_object_show(list);
			elm_frame_content_set(frame, list);
		evas_object_show(frame);
		elm_box_pack_end(gag_editor, frame);


		button_list = elm_box_add(settings_area);
			elm_box_horizontal_set(button_list, 1);
			elm_box_homogeneous_set(button_list, 1);

			evas_object_size_hint_weight_set(button_list, 1, 0);
			evas_object_size_hint_align_set(button_list, -1, 0);

			icon = elm_icon_add(settings_win);
				elm_icon_standard_set(icon, "file");
			evas_object_show(icon);

			button = elm_button_add(settings_win);
				elm_button_icon_set(button, icon);
				elm_object_text_set(button, _("Add"));
				evas_object_smart_callback_add(button, "clicked", on_gag_add, list);
				evas_object_size_hint_weight_set(button, 1, 1);
				evas_object_size_hint_align_set(button, -1, 0);
			elm_box_pack_end(button_list, button);
			evas_object_show(button);

			icon = elm_icon_add(settings_win);
				elm_icon_standard_set(icon, "edit");
			evas_object_show(icon);

			button = elm_button_add(settings_win);
				elm_button_icon_set(button, icon);
				elm_object_text_set(button, _("Edit"));
				evas_object_smart_callback_add(button, "clicked", on_gag_edit, list);
				evas_object_size_hint_weight_set(button, 1, 1);
				evas_object_size_hint_align_set(button, -1, 0);
			elm_box_pack_end(button_list, button);
			evas_object_show(button);

			icon = elm_icon_add(settings_win);
				elm_icon_standard_set(icon, "delete");
			evas_object_show(icon);

			button = elm_button_add(settings_win);
				elm_button_icon_set(button, icon);
				elm_object_text_set(button, _("Delete"));
				evas_object_smart_callback_add(button, "clicked", on_gag_delete, list);
				evas_object_size_hint_weight_set(button, 1, 1);
				evas_object_size_hint_align_set(button, -1, 0);
			elm_box_pack_end(button_list, button);
			evas_object_show(button);


		elm_box_pack_start(gag_editor, button_list);
		evas_object_show(button_list);

	elm_box_pack_start(settings_area, gag_editor);
	evas_object_show(gag_editor);

}

void on_settings(void *data, Evas_Object *obj, void *event_info) {
	Evas_Object *bg=NULL, *box=NULL, *toolbar=NULL;
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

			item = elm_toolbar_item_append(toolbar, "head", _("Accounts"), on_settings_accounts, NULL);

			elm_toolbar_item_append(toolbar, "folder", _("Cache"), on_settings_cache, NULL);

			elm_toolbar_item_append(toolbar, "apps", _("Options"), on_settings_options, NULL);

			elm_toolbar_item_append(toolbar, "head", _("Gag"), on_settings_gag, NULL);
		elm_box_pack_start(box, toolbar);
		evas_object_show(toolbar);
		elm_toolbar_item_selected_set(item, EINA_TRUE);

	evas_object_show(box);
	evas_object_resize(settings_win, 480, 640);
	evas_object_show(settings_win);
}

void display_help(void) {
	printf(_("\nUsage:\n"\
		"   -d   Debug mode (for extra yummy output, use multiple times)\n"
		"   -h   Help (what you're reading right now)\n"
		"   -m N Show N messages at a time\n"
		"\n")
	);
}

void ed_settings_shutdown(void) {
	if(settings && !eet_data_write(conf_file, settings_edd, "/settings", (void*)settings, 0))
			fprintf(stderr, "Failed to save settings.");
	eet_close(conf_file);
	eet_data_descriptor_free(settings_edd);
	eet_shutdown();
}

void ed_settings_init(int argc, char ** argv) {
	static char options[] = "dhm:";
	int option, res;
	long int mm=0;
	char *path;

	if(!ecore_file_init()) {
			fprintf(stderr, _("Can't run ecore_file_init()\n"));
			exit(1);
	}

	res = asprintf(&path, "%s/cache/icons", home);
	if( res == -1 || !ecore_file_mkpath(path)) {
		fprintf(stderr, (res==-1)?_("Can't create preferences directories since asprintf failed\n"):_("Can't create preferences directories since ecore_file_mkpath failed\n"));
		ecore_file_shutdown();
		exit(1);
	}
	free(path);

	res = asprintf(&conf_file_path, "%s/conf.eet", home);
	if( res==-1 ) {
		fprintf(stderr, _("Not enough memory to define conf.eet path"));
		ecore_file_shutdown();
		exit(1);
	}

	if(!eet_init()) {
		fprintf(stderr, _("Couldn't run eet_init()"));
		ecore_file_shutdown();
		exit(1);
	}

	conf_file = eet_open(conf_file_path, EET_FILE_MODE_READ_WRITE);
	if(!conf_file) {
		fprintf(stderr, _("Can't create %s"), path);
		ecore_file_shutdown();
		eet_shutdown();
		exit(1);
	}

	settings_edd = eet_data_descriptor_file_new(&settings_edd_class);
	if(!settings_edd) {
		fprintf(stderr, _("Can't setup settings_edd."));
		ecore_file_shutdown();
		eet_shutdown();
		exit(1);
	}

	EET_DATA_DESCRIPTOR_ADD_BASIC(settings_edd, Settings, "online",			online,			EET_T_INT);
	EET_DATA_DESCRIPTOR_ADD_BASIC(settings_edd, Settings, "browser",		browser,		EET_T_INT);
	EET_DATA_DESCRIPTOR_ADD_BASIC(settings_edd, Settings, "browser_name",	browser_name,	EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(settings_edd, Settings, "browser_cmd",	browser_cmd,	EET_T_STRING);
	EET_DATA_DESCRIPTOR_ADD_BASIC(settings_edd, Settings, "max_messages",	max_messages,	EET_T_INT);
	EET_DATA_DESCRIPTOR_ADD_BASIC(settings_edd, Settings, "rel_timestamps",	rel_timestamps,	EET_T_INT);
	EET_DATA_DESCRIPTOR_ADD_BASIC(settings_edd, Settings, "update_interval",update_interval_val,EET_T_INT);

	settings = eet_data_read(conf_file, settings_edd, "/settings");
	if(!settings) {
		settings = calloc(1, sizeof(Settings));
		if(!settings) {
			fprintf(stderr, _("Not enough memory for settings, no doubt for anything else either, surely..."));
			ecore_file_shutdown();
			eet_shutdown();
			exit(1);
		}

		settings->online=1;
		settings->max_messages=20;
		settings->rel_timestamps=EINA_TRUE;
		settings->rel_ts_timer=NULL;
		settings->update_interval_val = 0;
	}

	if(settings->update_interval_val < 0)
		settings->update_interval_val = 0;
	else if(settings->update_interval_val > timeoutsIndex)
		settings->update_interval_val = timeoutsIndex;

	settings->update_interval = timeout_values[settings->update_interval_val];

	while((option = getopt(argc,argv,options)) != -1) {
		switch(option) {
			case 'd': {
				debug++;
				if(debug) printf("Debug level: %d\n", debug);
				break;
			}
			case 'h': {
				display_help();
				ecore_file_shutdown();
				eet_shutdown();
				exit(0);
				break;
			}
			case 'm': {
				mm = strtol(optarg, NULL, 10);
				if(mm > INT_MIN && mm < INT_MAX)
					settings->max_messages = (int)mm;
				break;
			}
			default: {
				display_help();
				ecore_file_shutdown();
				eet_shutdown();
				exit(2);
			}
		}
	}

}
