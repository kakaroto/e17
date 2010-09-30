/*
 * vim:ts=4
 * 
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

#include <config.h>

#include <signal.h>
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <pthread.h>

#ifdef HAVE_LIBFRAMEWORD_GLIB

#include <dbus/dbus.h>

#endif

#include <Elementary.h>
#include <Ecore_X.h>

#include <sqlite3.h>

#include <curl/curl.h>
#include <glib.h>
#include <glib/gprintf.h>

#include <time.h>

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "elmdentica.h"
#include "settings.h"
#include "statusnet.h"
#include "curl.h"

Evas_Object *status_list=NULL, *scroller=NULL, *status=NULL, *win=NULL, *error_win=NULL, *entry=NULL, *fs=NULL, *count=NULL, *url_win=NULL, *hv=NULL, *related_inwin=NULL, *ly=NULL;
char * dm_to=NULL;

StatusesList	*statuses=NULL;
int debug=0, mouse_x=0, mouse_y=0;
Evas_Coord finger_size;
int first_message=1;
time_t now;

Eina_Hash * bubble2status=NULL;

long long int reply_id=0;
long long int user_id=0;
char * url_post = NULL;
char * url_friends = NULL;
extern char * browsers[];
extern char * browserNames[];
char * follow_user=NULL;
char * home=NULL;
extern Eina_Hash* statusHash;
extern Eina_Hash* userHash;
extern Settings *settings;

extern CURL * user_agent;

double mouse_held_down=0;

struct sqlite3 *ed_DB=NULL;

GRegex *re_link=NULL, *re_link_content=NULL, *re_entities=NULL, *re_user=NULL, *re_nouser=NULL, *re_group=NULL, *re_nogroup=NULL, *re_amp=NULL, *re_nl;
GError *re_err=NULL;

static Elm_Genlist_Item_Class itc1;

static int count_accounts(void *notUsed, int argc, char **argv, char **azColName) {
	int count = atoi(argv[0]);

	if(count == 0)
		on_settings(NULL, NULL, NULL);

	return(0);
}

void elmdentica_init(void) {
	char *db_path=NULL, *db_err=NULL;
	int sqlite_res=0;
	char *query = NULL;

	sqlite_res = asprintf(&db_path, "%s/.elmdentica/db", getenv("HOME"));
	if(sqlite_res == -1) {
		perror("Too little free memory to even define the db's path\n");
		exit(2);
	}
	sqlite_res = sqlite3_open(db_path, &ed_DB);
	if(sqlite_res != 0) {
		printf("Can't open DB at %s: %d\n", db_path, sqlite_res);
		exit(sqlite_res);
	}
	chmod(db_path, S_IRUSR|S_IWUSR);
	free(db_path);

	query = "CREATE TABLE IF NOT EXISTS gag (id INTEGER PRIMARY KEY, enabled INTEGER, pattern TEXT);";
	sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
	if(sqlite_res != 0) {
		printf("Can't run %s: %d => %s\n", query, sqlite_res, db_err);
		exit(sqlite_res);
	}

	query = "CREATE TABLE IF NOT EXISTS accounts (id INTEGER PRIMARY KEY, enabled INTEGER, name TEXT, password TEXT, type INTEGER, proto TEXT, domain TEXT, port INTEGER, base_url TEXT, receive INTEGER, send INTEGER );";
	sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
	if(sqlite_res != 0) {
		printf("Can't run %s: %d => %s\n", query, sqlite_res, db_err);
		exit(sqlite_res);
	}

	query = "CREATE TABLE IF NOT EXISTS messages (id INTEGER PRIMARY KEY, status_id INTEGER, account_id INTEGER CONSTRAINT account_id_ref REFERENCES accounts (id), timeline INTEGER, s_text TEXT, s_truncated INTEGER, s_created_at INTEGER, s_in_reply_to_status_id INTEGER, s_source TEXT, s_in_reply_to_user_id INTEGER, s_favorited INTEGER, s_user INTEGER);";
	sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
	if(sqlite_res != 0) {
		printf("Can't run %s: %d => %s\n", query, sqlite_res, db_err);
		exit(sqlite_res);
	}

	query = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, uid INTEGER, account_id INTEGER CONSTRAINT account_id_ref REFERENCES accounts (id), name TEXT, screen_name TEXT, location TEXT, description TEXT, profile_image_url TEXT, url TEXT, protected INTEGER, followers_count INTEGER, friends_count INTEGER, created_at INTEGER, favorites_count INTEGER, statuses_count INTEGER, following INTEGER, statusnet_blocking INTEGER);";
	sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
	if(sqlite_res != 0) {
		printf("Can't run %s: %d => %s\n", query, sqlite_res, db_err);
		exit(sqlite_res);
	}

	query = "CREATE TABLE IF NOT EXISTS posts (account_id INTEGER CONSTRAINT account_id_ref REFERENCES accounts (id), dm_to TEXT, message TEXT);";
	sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
	if(sqlite_res != 0) {
		printf("Can't run %s: %d => %s\n", query, sqlite_res, db_err);
		exit(sqlite_res);
	}

	sqlite3_exec(ed_DB, "SELECT count(*) FROM accounts;", count_accounts, NULL, &db_err);
	if(sqlite_res != 0) {
		printf("Can't get current account: %d => %s\n", sqlite_res, db_err);
	}
	sqlite3_free(db_err);

}

void toggle_fullscreen(Eina_Bool new_fullscreen) {
	elm_win_fullscreen_set(win, new_fullscreen);
	settings->fullscreen=new_fullscreen;
}

void make_status_list(int timeline) {
	char *label;

	switch(timeline) {
		case TIMELINE_USER:		{	label = _("Last messages...");		break; }
		case TIMELINE_PUBLIC:	{	label = _("Everyone...");	break; }
		case TIMELINE_FAVORITES: {	label = _("My favorites...");	break; }
		case TIMELINE_MENTIONS: {	label = _("Replies / mentions...");	break; }
		case TIMELINE_FRIENDS:
		default:				{	label = _("My friends...");	break; }
	}

	elm_win_title_set(win, label);
}

void print_status(gpointer data, gpointer user_data) {
        ub_Status *status = (ub_Status*)data;

        printf("Name: '%s'\n", (char*)status->name);
        printf("Screen Name: '%s'\n", (char*)status->screen_name);
        printf("Created at: '%s'\n", (char*)status->created_at);
        printf("Text: '%s'\n", (char*)status->text);
}

void error_win_del(void *data, Evas_Object *zbr, void *event_info) {
	evas_object_del(error_win);
}

static int ed_mark_favorite(void *data, int argc, char **argv, char **azColName) {
	Evas_Object *bubble = (Evas_Object*)data;
	aStatus *as = eina_hash_find(bubble2status, &bubble);
	char *screen_name=NULL, *password=NULL, *proto=NULL, *domain=NULL, *base_url=NULL;
	int port=0, id=0;

	/* In this query handler, these are the current fields:
		argv[0] == name TEXT
		argv[1] == password TEXT
		argv[2] == type INTEGER
		argv[3] == proto TEXT
		argv[4] == domain TEXT
		argv[5] == port INTEGER
		argv[6] == base_url TEXT
		argv[7] == id INTEGER
	*/

	screen_name = argv[0];
	password = argv[1];
	proto = argv[3];
	domain = argv[4];
	port = atoi(argv[5]);
	base_url = argv[6];
	id = atoi(argv[7]);

	if(as->favorited) {
		switch(atoi(argv[2])) {
			case ACCOUNT_TYPE_STATUSNET:
			default: { ed_statusnet_favorite_destroy(id, screen_name, password, proto, domain, port, base_url, as->sid); break; }
		}
	} else {
		switch(atoi(argv[2])) {
			case ACCOUNT_TYPE_STATUSNET:
			default: { ed_statusnet_favorite_create(id, screen_name, password, proto, domain, port, base_url, as->sid); break; }
		}
	}
	return(0);
}

static void on_mark_favorite(void *data, Evas_Object *obj, void *event_info) {
	Evas *e;
	Evas_Object *hover, *bubble = (Evas_Object*)data;
	aStatus *as = eina_hash_find(bubble2status, &bubble);
	int sqlite_res=0;
	char *db_err=NULL, *query=NULL;

	if(!as) return;

	sqlite_res = asprintf(&query, "SELECT name,password,type,proto,domain,port,base_url,id FROM accounts WHERE enabled = 1 and id = %d;", as->account_id);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, ed_mark_favorite, data, &db_err);
		if(sqlite_res != 0) {
			printf("Can't run %s: %d => %s\n", query, sqlite_res, db_err);
		}
		sqlite3_free(db_err);
		free(query);

		e = evas_object_evas_get(win);
		if(e) {
			hover = evas_object_name_find(e, "hover_actions");
			if(hover) evas_object_hide(hover);
		}
	}
}

static void on_repeat(void *data, Evas_Object *obj, void *event_info) {
	Evas *e;
	Evas_Object *hover, *bubble = (Evas_Object*)data;
	aStatus *as = eina_hash_find(bubble2status, &bubble);

	if(as) {
		switch(as->account_type) {
			case ACCOUNT_TYPE_STATUSNET:
			default: { ed_statusnet_repeat(as->account_id, as->sid); break; }
		}
	}

	e = evas_object_evas_get(win);
	if(e) {
		hover = evas_object_name_find(e, "hover_actions");
		if(hover) evas_object_hide(hover);
	}
}

static void on_close_popup_status(void *data, Evas_Object *obj, void *event_info) {
	evas_object_del(related_inwin);
	related_inwin = NULL;
}

void ed_popup_status(aStatus *as) {
	Evas *e=NULL;
    Evas_Object *box=NULL, *rel_scroll=NULL, *box2=NULL, *bubble=NULL, *button=NULL;
	anUser *au=NULL;
	char *uid_str=NULL;
	int res=0;

	if(!as) return;

	res = asprintf(&uid_str, "%lld", as->user);
	if(res != -1) {
		au = eina_hash_find(userHash, uid_str);
		free(uid_str);
	} else return;

	bubble = ed_make_bubble(win, as, au);
		evas_object_size_hint_weight_set(bubble, 1, 1);
		evas_object_size_hint_align_set(bubble, -1, -1);
	evas_object_show(bubble);

	if(!related_inwin) {
		related_inwin = elm_win_inwin_add(win);
			box = elm_box_add(win);
				evas_object_size_hint_weight_set(box, 1, 1);
				evas_object_size_hint_align_set(box, -1, -1);
				rel_scroll = elm_scroller_add(win);
					evas_object_size_hint_weight_set(rel_scroll, 1, 1);
					evas_object_size_hint_align_set(rel_scroll, -1, -1);
					elm_scroller_bounce_set(rel_scroll, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
					elm_scroller_policy_set(rel_scroll, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_ON);

					box2 = elm_box_add(win);
						evas_object_size_hint_weight_set(box2, 1, 1);
						evas_object_size_hint_align_set(box2, -1, -1);
						evas_object_name_set(box2, "related_box");
		
            		evas_object_show(box2);

					elm_scroller_content_set(rel_scroll, box2);
					elm_box_pack_end(box, rel_scroll);
            	evas_object_show(rel_scroll);

				button = elm_button_add(win);
					elm_button_label_set(button, _("Close"));
					evas_object_smart_callback_add(button, "clicked", on_close_popup_status, NULL);
					elm_box_pack_end(box, button);
				evas_object_show(button);
        	elm_win_inwin_content_set(related_inwin, box);
		
    	evas_object_show(related_inwin);
	} else {

		if(!box2) {
			e = evas_object_evas_get(win);
			if(e) box2 = evas_object_name_find(e, "related_box");
		}
	}

	if(box2) elm_box_pack_end(box2, bubble);

	eina_hash_add(bubble2status, &bubble, as);
}

static void on_view_related(void *data, Evas_Object *obj, void *event_info) {
	Evas *e;
	Evas_Object *hover, *bubble = (Evas_Object*)data;
	aStatus *as = eina_hash_find(bubble2status, &bubble), *related_status=NULL;

	if(as) {
		switch(as->account_type) {
			case ACCOUNT_TYPE_STATUSNET:
			default: { ed_statusnet_status_get(as->account_id, as->in_reply_to_status_id, &related_status); break; }
		}
	}

	if(related_status)
		ed_popup_status(related_status);
	else
		printf(_("Error importing related status\n"));

	e = evas_object_evas_get(win);
	if(e) {
		hover = evas_object_name_find(e, "hover_actions");
		if(hover) evas_object_hide(hover);
	}

}

static void on_reply(void *data, Evas_Object *obj, void *event_info) {
	Evas *e;
	Evas_Object *hover, *bubble = (Evas_Object*)data;
	aStatus *as = eina_hash_find(bubble2status, &bubble);
	anUser *au = NULL;
	char * entry_str=NULL, *uid_str=NULL;
	int res = 0;

	if(as) {
		res = asprintf(&uid_str, "%lld", as->user);
		if(res != -1) {
			au = eina_hash_find(userHash, uid_str);
			if(au) {
				res = asprintf(&entry_str, "@%s: ", au->screen_name);
				if(res != -1) {
					elm_entry_entry_set(entry, entry_str);
					free(entry_str);
					elm_object_focus(entry);
					reply_id=as->sid;
					elm_entry_cursor_end_set(entry);
				}
			}
			free(uid_str);
		}
	}

	e = evas_object_evas_get(win);
	if(e) {
		hover = evas_object_name_find(e, "hover_actions");
		if(hover) evas_object_hide(hover);
	}

	evas_object_show(hv);
}

static void on_dm(void *data, Evas_Object *obj, void *event_info) {
	Evas *e;
	Evas_Object *hover, *bubble = (Evas_Object*)data;
	aStatus *as = eina_hash_find(bubble2status, &bubble);

	if(as) {
		elm_object_focus(entry);
		user_id=as->user;
	}

	e = evas_object_evas_get(win);
	if(e) {
		hover = evas_object_name_find(e, "hover_actions");
		if(hover) evas_object_hide(hover);
	}

	evas_object_show(hv);
}


static void url_win_del(void *data, Evas_Object *obj, void *event_info) {
	if(data)
		free(data);
	evas_object_del(url_win);
}

static void on_open_url(void *data, Evas_Object *obj, void *event_info) {
	char * url = (char*)data;
	char * cmd = NULL;
	int sys_result = 0;


	url_win_del(NULL, NULL, NULL);

	if (settings->browser < 0) {
		if(data) free(data);
		return;
	}

	sys_result = asprintf(&cmd, settings->browser_cmd, url);
	if(sys_result != -1) {
		sys_result = system(cmd);
		if(sys_result == -1) {
			fprintf(stderr, _("Error %d: %s\n"), errno, strerror(errno));
		} else if(sys_result != 0) {
			if(! WIFEXITED(sys_result)) {
				fprintf(stderr, _("System failed, child exited with status %d\n"), WEXITSTATUS(sys_result));
			}
		}
		free(cmd);
		free(data);
	}
}

static void on_message_anchor_clicked(void *data, Evas_Object *obj, void *event_info);
static void on_bubble_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void on_bubble_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);

gboolean ed_replace_entities(const GMatchInfo *match_info, GString *result, gpointer user_data) {
	char *matched_str = (char*)g_match_info_fetch(match_info, 1);
	char *replacement = NULL;

	if(matched_str) {
		if(strncmp(matched_str, "&quot;", 6) == 0)
			replacement = "\"";
		else if(strncmp(matched_str, "&apos;", 6) == 0)
			replacement = "\"";
		else if(strncmp(matched_str, "&lt;", 4) == 0)
			replacement = "<";
		else if(strncmp(matched_str, "&gt;", 4) == 0)
			replacement = ">";
		else if(strncmp(matched_str, "&laquo;", 7) == 0)
			replacement = "«";
		else if(strncmp(matched_str, "&raquo;", 7) == 0)
			replacement = "»";
		else
			replacement = matched_str;
	}
	if(replacement) {
		g_string_append(result, replacement);
		if(debug > 2) printf("Replaced %s with %s\n", matched_str, replacement);
	}

	return(EINA_TRUE);
}

Evas_Object *ed_make_message(char *text, Evas_Object *bubble, Evas_Object *window) {
	Evas_Object *message = NULL;
	char *status_message = NULL, *tmp = NULL;
	int res = 0;

	message = elm_anchorblock_add(window);

		res = asprintf(&status_message, "%s", text);
		if(res == -1) {
			elm_anchorblock_text_set(message, "");
		} else {
			if(!re_nl)
				re_nl = g_regex_new("[\r\n]+", G_REGEX_MULTILINE, 0, &re_err);
			tmp = g_regex_replace(re_nl, status_message, -1, 0, "<br>", 0, &re_err);
			if(tmp) {
				free(status_message);
				status_message = tmp;
			}

			if(!re_entities)
				re_entities = g_regex_new("(&[a-zA-Z0-9]+;)", 0, 0, &re_err);
			tmp = g_regex_replace_eval(re_entities, status_message, -1, 0, 0, ed_replace_entities, NULL, &re_err);
			if(tmp) {
				free(status_message);
				status_message = tmp;
			}

			if(!re_amp)
				re_amp = g_regex_new("&(?!amp;)", 0, 0, &re_err);
			tmp = g_regex_replace(re_amp, status_message, -1, 0, "&amp;", 0, &re_err);
			if(tmp) {
				free(status_message);
				status_message = tmp;
			}

			if(!re_link)
				re_link = g_regex_new("([a-z]+://.*?)(\\s|$)", 0, 0, &re_err);
			tmp = g_regex_replace(re_link, status_message, -1, 0, "<a href='\\1'>[link]</a>\\2", 0, &re_err);
			if(tmp) {
				free(status_message);
				status_message = tmp;
			}

			if(!re_user)
				re_user = g_regex_new("@([a-zA-Z0-9_]+)", 0, 0, &re_err);
			tmp = g_regex_replace(re_user, status_message, -1, 0, "<a href='user://\\1'>@\\1</a>", 0, &re_err);
			if(tmp) {
				free(status_message);
				status_message = tmp;
			}

			if(!re_group)
				re_group = g_regex_new("!([a-zA-Z0-9_]+)", 0, 0, &re_err);
			tmp = g_regex_replace(re_group, status_message, -1, 0, "<a href='group://\\1'>!\\1</a>", 0, &re_err);
			if(tmp) {
				free(status_message);
				status_message = tmp;
			}

			elm_anchorblock_text_set(message, status_message);
			evas_object_smart_callback_add(message, "anchor,clicked", on_message_anchor_clicked, bubble);
 			evas_object_event_callback_add(message, EVAS_CALLBACK_MOUSE_DOWN, on_bubble_mouse_down, bubble);
 			evas_object_event_callback_add(message, EVAS_CALLBACK_MOUSE_UP, on_bubble_mouse_up, bubble);
		}
	evas_object_show(message);
	return(message);
}


anUser *user_info_get(aStatus *as, UserProfile *user) {
	anUser *au=NULL;

	switch(as->account_type) {
		case ACCOUNT_TYPE_STATUSNET:
		default: { au = ed_statusnet_user_get(as->account_id, user); break; }
	}

	return(au);
}

static int ed_user_follow(void *data, int argc, char **argv, char **azColName) {
	char *screen_name=NULL, *password=NULL, *proto=NULL, *domain=NULL, *base_url=NULL;
	int port=0, id=0;

	/* In this query handler, these are the current fields:
		argv[0] == name TEXT
		argv[1] == password TEXT
		argv[2] == type INTEGER
		argv[3] == proto TEXT
		argv[4] == domain TEXT
		argv[5] == port INTEGER
		argv[6] == base_url TEXT
		argv[7] == id INTEGER
	*/

	screen_name = argv[0];
	password = argv[1];
	proto = argv[3];
	domain = argv[4];
	port = atoi(argv[5]);
	base_url = argv[6];
	id = atoi(argv[7]);

	switch(atoi(argv[2])) {
		case ACCOUNT_TYPE_STATUSNET:
		default: { ed_statusnet_user_follow(id, screen_name, password, proto, domain, port, base_url, follow_user); break; }
	}

	follow_user=NULL;

	return(0);
}
static void on_user_follow(void *data, Evas_Object *obj, void *event_info) {
	aStatus *as = eina_hash_find(bubble2status, &data);
	int sqlite_res=0;
	char *db_err=NULL, *query=NULL;

	if(!as) return;

	sqlite_res = asprintf(&query, "SELECT name,password,type,proto,domain,port,base_url,id FROM accounts WHERE enabled = 1 and id = %d;", as->account_id);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, ed_user_follow, NULL, &db_err);
		if(sqlite_res != 0) {
			printf("Can't run %s: %d => %s\n", query, sqlite_res, db_err);
		}
		sqlite3_free(db_err);
		free(query);
		evas_object_del(url_win);
	}
}

static int ed_user_abandon(void *data, int argc, char **argv, char **azColName) {
	char *screen_name=NULL, *password=NULL, *proto=NULL, *domain=NULL, *base_url=NULL;
	int port=0, id=0;

	/* In this query handler, these are the current fields:
		argv[0] == name TEXT
		argv[1] == password TEXT
		argv[2] == type INTEGER
		argv[3] == proto TEXT
		argv[4] == domain TEXT
		argv[5] == port INTEGER
		argv[6] == base_url TEXT
		argv[7] == id INTEGER
	*/

	screen_name = argv[0];
	password = argv[1];
	proto = argv[3];
	domain = argv[4];
	port = atoi(argv[5]);
	base_url = argv[6];
	id = atoi(argv[7]);

	switch(atoi(argv[2])) {
		case ACCOUNT_TYPE_STATUSNET:
		default: { ed_statusnet_user_abandon(id, screen_name, password, proto, domain, port, base_url, follow_user); break; }
	}

	follow_user=NULL;

	return(0);
}
static void on_user_abandon(void *data, Evas_Object *obj, void *event_info) {
	aStatus *as = eina_hash_find(bubble2status, &data);
	int sqlite_res=0;
	char *db_err=NULL, *query=NULL;

	if(!as) return;

	sqlite_res = asprintf(&query, "SELECT name,password,type,proto,domain,port,base_url,id FROM accounts WHERE enabled = 1 and id = %d;", as->account_id);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, ed_user_abandon, NULL, &db_err);
		if(sqlite_res != 0) {
			printf("Can't run %s: %d => %s\n", query, sqlite_res, db_err);
		}
		sqlite3_free(db_err);
		free(query);
		evas_object_del(url_win);
	}
}

static void user_free(UserProfile *user) {
	if(user) {
		if(user->name) free(user->name);
		if(user->tmp) free(user->tmp);
		if(user->text) free(user->text);

		free(user);
	}
}

static void on_zoomed_icon_clicked(void *data, Evas_Object *obj, void *event_info) {
	Evas_Object *zoom = (Evas_Object*)data;

	if(zoom)
		evas_object_del(zoom);
}

static void on_bubble_icon_clicked(void *data, Evas_Object *obj, void *event_info) {
	aStatus *as = (aStatus*)data;
	anUser *au=NULL;
	char *file_path=NULL, *uid_str=NULL;
	Evas_Object *zoom=NULL, *box=NULL, *label=NULL, *icon=NULL;
	int res = 0;

	res = asprintf(&file_path, "%s/cache/icons/%lld", home, as->user);

	if(res != -1) {
		zoom = elm_win_inwin_add(win);
			box = elm_box_add(win);
				evas_object_size_hint_weight_set(box, 1, 1);
				evas_object_size_hint_align_set(box, -1, -1);
				elm_box_homogenous_set(box, EINA_FALSE);
				res = asprintf(&uid_str, "%lld", as->user);
				if(res != -1) {
					au = eina_hash_find(userHash, uid_str);
					if(au) {
							free(uid_str);
							res = asprintf(&uid_str, "Avatar of <b>%s</b> (<b>@%s</b>)", au->name, au->screen_name);
							if(res != -1) {
								label = elm_label_add(win);
									evas_object_size_hint_weight_set(label, 1, 0);
									evas_object_size_hint_align_set(label, -1, 0);
									elm_label_line_wrap_set(label, EINA_TRUE);
									elm_label_label_set(label, uid_str);
									free(uid_str);
									elm_box_pack_end(box, label);
								evas_object_show(label);
							}
					}
				}
				icon = elm_icon_add(win);
					evas_object_size_hint_weight_set(icon, 1, 1);
					evas_object_size_hint_align_set(icon, -1, -1);
					elm_icon_file_set(icon, file_path, "fubar?");
					evas_object_smart_callback_add(icon, "clicked", on_zoomed_icon_clicked, zoom);
					elm_box_pack_end(box, icon);
				evas_object_show(icon);

			elm_win_inwin_content_set(zoom, box);
		evas_object_show(zoom);
		free(file_path);
	}
}

Evas_Object *ed_get_icon(long long int id, Evas_Object *parent) {
	Evas_Object *icon=NULL;
	int res=0;
	char *file_path=NULL;

	res = asprintf(&file_path, "%s/cache/icons/%lld", home, id);
	if(res != -1 && (icon = elm_icon_add(parent))) {
		elm_icon_file_set(icon, file_path, "fubar?");
		free(file_path);
	}
	return(icon);
}

Evas_Object *ed_make_bubble(Evas_Object *parent, aStatus* as, anUser* au) {
	Evas_Object *bubble = NULL, *icon = NULL, *message = NULL;
	char datestr[19];
	struct tm date_tm;

	if((bubble = elm_bubble_add(parent))) {
		evas_object_size_hint_weight_set(bubble, 1, 0);
		evas_object_size_hint_align_set(bubble, -1, -1);

		if(localtime_r(&(as->created_at), &date_tm)) {
			strftime(datestr, sizeof(datestr), "%F %R", &date_tm);
			elm_bubble_info_set(bubble, datestr);
		}

		if(au) elm_bubble_label_set(bubble, au->name);

		icon = ed_get_icon(as->user, parent);
		if(icon) {
			elm_bubble_icon_set(bubble, icon);
			evas_object_smart_callback_add(icon, "clicked", on_bubble_icon_clicked, bubble);
			evas_object_show(icon);
		}

		message = ed_make_message(as->text, bubble, win);
		
		if(message) {
			elm_bubble_content_set(bubble, message);
		} else evas_object_del(bubble);
	}
	return(bubble);
}

static void on_handle_user(void *data, Evas_Object *obj, void *event_info) {
	AnchorData *anchor = (AnchorData*)data;
	Evas_Object *user_win=NULL, *icon=NULL, *bg=NULL, *table=NULL, *button=NULL, *label=NULL, *bubble=anchor->bubble;
	UserProfile *user=NULL;
	aStatus *as = eina_hash_find(bubble2status, &bubble);
	char *description=NULL, *path=NULL;
	int res=0;
	struct stat buf;
	anUser *au=NULL;

	if(!settings->online) return;

	user = calloc(1,sizeof(UserProfile));
	user->screen_name=anchor->url+7;
	au = user_info_get(as, user);

	if(!au) return;

	follow_user=au->screen_name;

	user_win = elm_win_add(NULL, au->screen_name, ELM_WIN_BASIC);
		evas_object_size_hint_min_set(user_win, 480, 480);
		evas_object_size_hint_max_set(user_win, 640, 640);
		elm_win_title_set(user_win, au->screen_name);
		elm_win_autodel_set(user_win, EINA_TRUE);

		bg = elm_bg_add(user_win);
			evas_object_size_hint_weight_set(bg, 1, 1);
			evas_object_size_hint_align_set(bg, -1, -1);
			elm_win_resize_object_add(user_win, bg);
		evas_object_show(bg);

		table=elm_table_add(user_win);
			evas_object_size_hint_weight_set(table, 1, 0);
			evas_object_size_hint_align_set(table, -1, 0);
			elm_win_resize_object_add(user_win, table);
			elm_table_padding_set(table, 20, 20);

			res = asprintf(&path, "%s/cache/icons/%s", home, au->screen_name);

			if(res!=-1 && stat(path, &buf) == 0 ) {
				icon = elm_icon_add(user_win);
					evas_object_size_hint_weight_set(icon, 1, 1);
					evas_object_size_hint_align_set(icon, -1, -1);
					elm_icon_file_set(icon, path, "fubar?");
					elm_table_pack(table, icon, 0, 0, 1, 2);
				evas_object_show(icon);
				free(path);
			}

			label = elm_entry_add(user_win);
				evas_object_size_hint_weight_set(label, 1, 1);
				evas_object_size_hint_align_set(label, -1, -1);

				res = asprintf(&description, "%s is following %d and has %d followers.", au->name, au->friends_count, au->followers_count);
				elm_entry_line_wrap_set(label, EINA_TRUE);
				if(res!=-1) {
					elm_entry_entry_set(label, description);
					free(description);
				} else
					elm_entry_entry_set(label, _("Couldn't parse user info message..."));

				elm_table_pack(table, label, 1, 0, 1, 1);
			evas_object_show(label);

			button = elm_button_add(user_win);
				evas_object_size_hint_weight_set(button, 1, 1);
				evas_object_size_hint_align_set(button, 0.5, 0);

				if(!au->following && !au->protected) {
					elm_button_label_set(button, _("Follow"));
					evas_object_smart_callback_add(button, "clicked", on_user_follow, bubble);
				} else {
					elm_button_label_set(button, _("Stop following"));
					evas_object_smart_callback_add(button, "clicked", on_user_abandon, bubble);
				}
				elm_table_pack(table, button, 1, 1, 1, 1);
			evas_object_show(button);

			if(au->description) {
					label = elm_label_add(user_win);
						evas_object_size_hint_weight_set(label, 1, 1);
						evas_object_size_hint_align_set(label, -1, -1);
						elm_label_line_wrap_set(label, EINA_TRUE);
						elm_label_label_set(label, au->description);
					evas_object_show(label);
					elm_table_pack(table, label, 0, 2, 2, 1);
			}

		evas_object_show(table);

	    evas_object_resize(user_win, 480, 640);
	evas_object_show(user_win);

	if(user) user_free(user);
}

static void on_group_messages_view(void *data, Evas_Object *obj, void *event_info) {
	Evas_Object *s = (Evas_Object*)data, *label=NULL, *group_win = evas_object_smart_parent_get(obj);

	label = elm_label_add(group_win);
		evas_object_size_hint_weight_set(label, 1, 1);
		evas_object_size_hint_align_set(label, -1, -1);
		elm_label_line_wrap_set(label, EINA_TRUE);
		elm_label_label_set(label, _("Not implemented yet"));
		elm_scroller_content_set(s, label);
	evas_object_show(s);
}

static void on_group_leave(void *data, Evas_Object *obj, void *event_info);

static void on_group_join(void *data, Evas_Object *obj, void *event_info) {
	Evas_Object *bubble = (Evas_Object*)data;
	aStatus *as = eina_hash_find(bubble2status, &bubble);
	Evas *e = evas_object_evas_get(obj);
	Evas_Object *frame = evas_object_name_find(e, "group"), *group_desc = NULL;
	GroupProfile *gp = (GroupProfile*)calloc(1, sizeof(GroupProfile));
	char *m;
	int res = 0;

	if(!as) return;

	gp->name=strndup(elm_frame_label_get(frame), PIPE_BUF);

	ed_statusnet_group_join(as->account_id, gp);

	if(gp->failed) {
		printf("Error joining group %s: %s\n", gp->name, gp->error);
	} else {
		if(gp->member)
			res = asprintf(&m, _("You are a member of group %s along with %d other people.<br>«%s»"), gp->fullname, gp->member_count -1, gp->description);
		else
			res = asprintf(&m, _("You are not a member of group %s but %d people are.<br>«%s»"), gp->fullname, gp->member_count, gp->description);

		if(res != -1) {
			group_desc = evas_object_name_find(e, "group_desc");
			elm_label_label_set(group_desc, m);
			elm_button_label_set(obj, _("Leave"));
			evas_object_smart_callback_add(obj, "clicked", on_group_leave, bubble);
			free(m);
		}
	}

	ed_statusnet_group_free(gp);
}

static void on_group_leave(void *data, Evas_Object *obj, void *event_info) {
	Evas_Object *bubble = (Evas_Object*)data;
	aStatus *as = eina_hash_find(bubble2status, &bubble);
	Evas *e = evas_object_evas_get(obj);
	Evas_Object *frame = evas_object_name_find(e, "group"), *group_desc = NULL;
	GroupProfile *gp = (GroupProfile*)calloc(1, sizeof(GroupProfile));
	char *m;
	int res = 0;

	if(!as) return;

	gp->name=strndup(elm_frame_label_get(frame), PIPE_BUF);

	ed_statusnet_group_leave(as->account_id, gp);

	if(gp->failed && debug)
		printf("Error leaving group %s: %s\n", gp->name, gp->error);
	else {
		if(gp->member)
			res = asprintf(&m, _("You are a member of group %s along with %d other people.<br>«%s»"), gp->fullname, gp->member_count -1, gp->description);
		else
			res = asprintf(&m, _("You are not a member of group %s but %d people are.<br>«%s»"), gp->fullname, gp->member_count, gp->description);

		if(res != -1) {
			group_desc = evas_object_name_find(e, "group_desc");
			elm_label_label_set(group_desc, m);
			elm_button_label_set(obj, _("Join"));
			evas_object_smart_callback_add(obj, "clicked", on_group_join, bubble);
			free(m);
		}
	}
}

static void on_handle_group(void *data, Evas_Object *obj, void *event_info) {
	AnchorData *anchor = (AnchorData*)data;
	Evas_Object *group_win=NULL, *bg=NULL, *box=NULL, *label=NULL, *notify=NULL, *s=NULL, *box2=NULL, *bubble=anchor->bubble, *icon=NULL, *button=NULL, *frame=NULL;
	aStatus *as = eina_hash_find(bubble2status, &bubble);
	GroupProfile *gp = (GroupProfile*)calloc(1, sizeof(GroupProfile));
	char *m, *path;
	int res = 0;
	struct stat buf;

	gp->name=strndup(anchor->url+8, PIPE_BUF);

	ed_statusnet_group_get(as->account_id, gp);
	if(gp->failed) {
		notify = elm_notify_add(win);
			evas_object_size_hint_weight_set(notify, 1, 1);
			evas_object_size_hint_align_set(notify, -1, -1);
			label = elm_label_add(win);
				evas_object_size_hint_weight_set(label, 1, 1);
				evas_object_size_hint_align_set(label, -1, -1);
				elm_label_label_set(label, gp->error);
				elm_label_line_wrap_set(label, EINA_TRUE);
			evas_object_show(label);
			elm_notify_content_set(notify, label);
			elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_TOP_RIGHT);
			elm_notify_parent_set(notify, win);
			elm_notify_timeout_set(notify, 5);
			elm_notify_timer_init(notify);
		evas_object_show(notify);

		return;
	}

	group_win = elm_win_add(NULL, gp->name, ELM_WIN_BASIC);
		evas_object_name_set(group_win, "group_win");
		elm_win_title_set(group_win, gp->name);
		elm_win_autodel_set(group_win, EINA_TRUE);

		bg = elm_bg_add(group_win);
			evas_object_size_hint_weight_set(bg, 1, 1);
			evas_object_size_hint_align_set(bg, -1, 0);
			elm_win_resize_object_add(group_win, bg);
		evas_object_show(bg);

		box = elm_box_add(group_win);
			evas_object_size_hint_weight_set(box, 1, 1);
			evas_object_size_hint_align_set(box, -1, 0);
			elm_win_resize_object_add(group_win, box);

			frame = elm_frame_add(group_win);
				evas_object_name_set(frame, "group");
				evas_object_size_hint_weight_set(frame, 1, 1);
				evas_object_size_hint_align_set(frame, -1, 0);
				elm_frame_label_set(frame, gp->name);

			box2 = elm_box_add(group_win);
				evas_object_size_hint_weight_set(box2, 1, 1);
				evas_object_size_hint_align_set(box2, -1, 0);
				elm_box_horizontal_set(box2, EINA_TRUE);

				res = asprintf(&path, "%s/cache/icons/%s", home, gp->name);

				if(res != -1 && stat(path, &buf) == 0 ) {
					icon = elm_icon_add(group_win);
						evas_object_size_hint_weight_set(icon, 1, 1);
						evas_object_size_hint_align_set(icon, -1, -1);
						elm_icon_file_set(icon, path, "fubar?");
						elm_box_pack_end(box2, icon);
						free(path);
					evas_object_show(icon);
				}

				if(gp->member)
					res = asprintf(&m, _("You are a member of group %s along with %d other people.<br>«%s»"), gp->fullname, gp->member_count -1, gp->description);
				else
					res = asprintf(&m, _("You are not a member of group %s but %d people are.<br>«%s»"), gp->fullname, gp->member_count, gp->description);

				if(res != -1) {
					label = elm_label_add(group_win);
						evas_object_name_set(label, "group_desc");
						evas_object_size_hint_weight_set(label, 1, 1);
						evas_object_size_hint_align_set(label, -1, -1);
						elm_label_line_wrap_set(label, EINA_TRUE);

						elm_label_label_set(label, m);
						free(m);
						elm_box_pack_end(box2, label);
					evas_object_show(label);

				}
				elm_frame_content_set(frame, box2);
				elm_box_pack_end(box, frame);
			evas_object_show(frame);

			s = elm_scroller_add(group_win);
				evas_object_size_hint_weight_set(s, 1, 1);
				evas_object_size_hint_align_set(s, -1, -1);


			box2 = elm_box_add(group_win);
				evas_object_size_hint_weight_set(box2, 1, 1);
				evas_object_size_hint_align_set(box2, -1, 0);
				elm_box_horizontal_set(box2, EINA_TRUE);

				button = elm_button_add(group_win);
					evas_object_size_hint_weight_set(button, 1, 1);
					evas_object_size_hint_align_set(button, -1, 0);
					if(gp->member) {
						elm_button_label_set(button, _("Leave"));
						evas_object_smart_callback_add(button, "clicked", on_group_leave, bubble);
					} else {
						elm_button_label_set(button, _("Join"));
						evas_object_smart_callback_add(button, "clicked", on_group_join, bubble);
					}
					elm_box_pack_end(box2, button);
				evas_object_show(button);
				
				res = asprintf(&m, _("Last %d messages"), settings->max_messages);
				if(res != -1) {
					button = elm_button_add(group_win);
						evas_object_size_hint_weight_set(button, 1, 1);
						evas_object_size_hint_align_set(button, -1, 0);
						elm_button_label_set(button, m);
						evas_object_smart_callback_add(button, "clicked", on_group_messages_view, s);
						elm_box_pack_end(box2, button);
					evas_object_show(button);
					free(m);
				}

				elm_box_pack_start(box, box2);
				elm_box_pack_end(box, s);
			evas_object_show(box2);


		evas_object_show(box);


	evas_object_resize(group_win, 480, 640);
	evas_object_show(group_win);

	ed_statusnet_group_free(gp);
}

static void on_handle_url(void *data, Evas_Object *obj, void *event_info) {
	Elm_Entry_Anchorblock_Info * info = (Elm_Entry_Anchorblock_Info*)event_info;
	char *url=NULL, *frame_label=NULL;
	const char *screen_name=NULL;
	Evas_Object *box=NULL, *box2=NULL, *button=NULL, *buttons=NULL, *frame=NULL, *entry=NULL, *bubble=(Evas_Object*)data;
	int res = 0;

	url = strndup(1+(char*)info->name,strlen((char*)info->name)-2);

	url_win = elm_win_inwin_add(win);
		elm_object_style_set(url_win, "minimal_vertical");

		box = elm_box_add(win);
			evas_object_size_hint_weight_set(box, 1, 1);
			evas_object_size_hint_align_set(box, -1, 0);

			frame = elm_frame_add(win);
				evas_object_size_hint_weight_set(frame, 1, 1);
				evas_object_size_hint_align_set(frame, -1, -1);

				elm_frame_label_set(frame, _("Visit website?"));

				box2 = elm_box_add(win);
					evas_object_size_hint_weight_set(box2, 1, 1);
					evas_object_size_hint_align_set(box2, -1, -1);
					entry = elm_entry_add(win);
						evas_object_size_hint_weight_set(entry, 1, 1);
						evas_object_size_hint_align_set(entry, -1, 0);
						elm_entry_editable_set(entry, FALSE);

						screen_name = elm_bubble_label_get(bubble);
						res = asprintf(&frame_label, _("%s posted the following URL...<br>"), screen_name);
						if(res != -1) {
							elm_entry_entry_set(entry, frame_label);
							elm_box_pack_end(box2, entry);
							evas_object_show(entry);
							free(frame_label);
						} else {
							evas_object_del(box2);
						}
					entry = elm_entry_add(win);
						elm_entry_editable_set(entry, FALSE);
						evas_object_size_hint_weight_set(entry, 1, 1);
						evas_object_size_hint_align_set(entry, -1, 0);
						elm_entry_line_char_wrap_set(entry, TRUE);
						elm_entry_entry_set(entry, url);
						elm_box_pack_end(box2, entry);
					evas_object_show(entry);
				evas_object_show(box2);

				elm_frame_content_set(frame, box2);
				elm_box_pack_end(box, frame);
			evas_object_show(frame);

			buttons = elm_box_add(win);
				evas_object_size_hint_weight_set(buttons, 1, 1);
				evas_object_size_hint_align_set(buttons, -1, -1);
				elm_box_horizontal_set(buttons, TRUE);

				button = elm_button_add(win);
					evas_object_size_hint_weight_set(button, 1, 1);
					evas_object_size_hint_align_set(button, -1, -1);
					elm_button_label_set(button, _("Check it out!"));
					evas_object_smart_callback_add(button, "clicked", on_open_url, url);
					elm_box_pack_end(buttons, button);
				evas_object_show(button);

				button = elm_button_add(win);
					evas_object_size_hint_weight_set(button, 1, 1);
					evas_object_size_hint_align_set(button, -1, -1);
					elm_button_label_set(button, _("Close"));
					evas_object_smart_callback_add(button, "clicked", url_win_del, url);
					elm_box_pack_end(buttons, button);
				evas_object_show(button);

				elm_box_pack_end(box, buttons);
			evas_object_show(buttons);
		evas_object_show(box);

		elm_win_inwin_content_set(url_win, box);
	evas_object_show(url_win);
}

static void on_message_anchor_clicked(void *data, Evas_Object *obj, void *event_info) {
	Elm_Entry_Anchorblock_Info * info = (Elm_Entry_Anchorblock_Info*)event_info;
	Evas_Object *frame, *button, *bubble=(Evas_Object*)data;
	AnchorData  *anchor=calloc(1, sizeof(AnchorData));
	char *url=NULL;

	if(!info->name || strlen(info->name) <= 9)
		return;

	url = strndup(1+(char*)info->name,strlen((char*)info->name)-2);

	if(strncmp(url, "user://", 7) == 0) {
		frame = elm_frame_add(win);
			evas_object_size_hint_weight_set(frame, 1, 1);
			evas_object_size_hint_align_set(frame, 0.5, 0);

			elm_frame_label_set(frame, _("View details?"));

			anchor->bubble = bubble;
			anchor->url = url;

			button = elm_button_add(win);
				elm_button_label_set(button, anchor->url+7);
				evas_object_smart_callback_add(button, "clicked", on_handle_user, anchor);
			evas_object_show(button);

			elm_frame_content_set(frame, button);
			elm_hover_content_set(info->hover, "middle", frame);
		evas_object_show(frame);
	} else if(strncmp(url, "group://", 8) == 0) {
		frame = elm_frame_add(win);
			evas_object_size_hint_weight_set(frame, 1, 1);
			evas_object_size_hint_align_set(frame, 0.5, 0);

			elm_frame_label_set(frame, _("View group?"));

			anchor->bubble = bubble;
			anchor->url = url;

			button = elm_button_add(win);
				elm_button_label_set(button, anchor->url+8);
				evas_object_smart_callback_add(button, "clicked", on_handle_group, anchor);
			evas_object_show(button);

			elm_frame_content_set(frame, button);
			elm_hover_content_set(info->hover, "middle", frame);
		evas_object_show(frame);
	} else {
		on_handle_url(data, obj, event_info);
	}
}

static void on_bubble_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info) {
	struct timeval tv;

	gettimeofday(&tv, NULL);
	
	mouse_held_down = (double)tv.tv_sec + (double)tv.tv_usec/1000000;
	evas_pointer_output_xy_get(e, &mouse_x, &mouse_y);
}

static void on_bubble_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info) {
	Evas_Object *hover=NULL, *box=NULL, *table=NULL, *button=NULL, *bubble=(Evas_Object*)data;
	aStatus *as = eina_hash_find(bubble2status, &bubble);
	double time_delta;
	struct timeval tv;
	int m_x=0, m_y=0;

	gettimeofday(&tv, NULL);
	time_delta = ((double)tv.tv_sec + (double)tv.tv_usec/1000000) - mouse_held_down;

	evas_pointer_output_xy_get(e, &m_x, &m_y);

	if( (abs(mouse_x-m_x) >= finger_size) || (abs(mouse_y-m_y) >= finger_size)) {
		mouse_x=m_x;
		mouse_y=m_y;
		return;
	}

	mouse_x=m_x;
	mouse_y=m_y;

	if( time_delta < 0.25 || time_delta >= 0.8 ) return;
	else mouse_held_down=0;

	hover = elm_hover_add(win);
		evas_object_name_set(hover, "hover_actions");
		box = elm_box_add(win);
			table = elm_table_add(win);

				button = elm_button_add(win);
					evas_object_size_hint_weight_set(button, 1, 1);
					evas_object_size_hint_align_set(button, -1, 0);
					elm_button_label_set(button, _("Reply"));
					evas_object_smart_callback_add(button, "clicked", on_reply, bubble);
					elm_table_pack(table, button, 0, 0, 1, 1);
				evas_object_show(button);

				button = elm_button_add(win);
					evas_object_size_hint_weight_set(button, 1, 1);
					evas_object_size_hint_align_set(button, -1, 0);
					elm_button_label_set(button, _("Repeat"));
					evas_object_smart_callback_add(button, "clicked", on_repeat, bubble);
					elm_table_pack(table, button, 1, 0, 1, 1);
				evas_object_show(button);

				button = elm_button_add(win);
					evas_object_size_hint_weight_set(button, 1, 1);
					evas_object_size_hint_align_set(button, -1, 0);
					elm_button_label_set(button, _("DM"));
					evas_object_smart_callback_add(button, "clicked", on_dm, bubble);
					elm_table_pack(table, button, 2, 0, 1, 1);
				evas_object_show(button);

				button = elm_button_add(win);
					evas_object_size_hint_weight_set(button, 1, 1);
					evas_object_size_hint_align_set(button, -1, 0);
					if(as->favorited)
						elm_button_label_set(button, _("Unmark favorite"));
					else
						elm_button_label_set(button, _("Mark favorite"));
					evas_object_smart_callback_add(button, "clicked", on_mark_favorite, bubble);
					elm_table_pack(table, button, 0, 1, 3, 1);
				evas_object_show(button);

				if(as->in_reply_to_status_id != 0) {
					button = elm_button_add(win);
						evas_object_size_hint_weight_set(button, 1, 1);
						evas_object_size_hint_align_set(button, -1, 0);
						elm_button_label_set(button, _("View related"));
						evas_object_smart_callback_add(button, "clicked", on_view_related, bubble);
						elm_table_pack(table, button, 0, 2, 3, 1);
					evas_object_show(button);
				}

				evas_object_show(table);
				elm_box_pack_end(box, table);

		evas_object_show(box);

		elm_hover_parent_set(hover, win);
		elm_hover_target_set(hover, scroller);
		elm_hover_content_set(hover, "middle", box);
	evas_object_show(hover);

}

static int ed_check_gag_message(void *user_data, int argc, char **argv, char **azColName) {
	GagData *gd = (GagData*)user_data;
	char *sn = NULL, *pattern=argv[0];
	int res = -1;

	/* In this query handler, these are the current fields:
		argv[0] == patern STRING
	*/

	if(strchr(pattern, '@')) {
		res = asprintf(&sn, "@%s", gd->screen_name);
		if(res == -1)
			sn = gd->screen_name;
	}

	if(debug > 2) printf("(%s|%s|%s) ~ %s ?", sn, gd->name, gd->message, pattern);

	// only do costly matches if there's isn't a match already
	if(	gd->match == EINA_FALSE &&
			(g_regex_match_simple(pattern, sn, G_REGEX_CASELESS, 0) ||
			 g_regex_match_simple(pattern, gd->name, G_REGEX_CASELESS, 0)        ||
			 g_regex_match_simple(pattern, gd->message, G_REGEX_CASELESS, 0))) {
		gd->match = EINA_TRUE;
		if(debug > 2) printf(" %s\n", "Yes");
	} else if(debug > 2) printf(" %s\n", "No");

	if(res != -1) free(sn);

	return(0);
}

Eina_Bool ed_check_gag(char *screen_name, char *name, char *message) {
	GagData gd;
	char *query, *db_err=NULL;
	int sqlite_res = 0;

	gd.screen_name = screen_name;
	gd.name = name;
	gd.message = message;
	gd.match = EINA_FALSE;

	query = "SELECT pattern FROM gag where enabled = 1;";
	sqlite_res = sqlite3_exec(ed_DB, query, ed_check_gag_message, &gd, &db_err);
	if(sqlite_res != 0) {
		printf("Can't run %s: %s\n", query, db_err);
		sqlite3_free(db_err);
	}

	return(gd.match);
}

static int fetch_user_from_db_handler(void *user_data, int argc, char **argv, char **azColName) {
	anUser **pau = (anUser**)user_data;
	anUser *au=NULL;

	if(*pau) return(-1);
	*pau = calloc(1, sizeof(anUser));

	au = *pau;

/*
 argv[0]  := id                 INTEGER
 argv[1]  := uid                INTEGER
 argv[2]  := account_id         INTEGER
 argv[3]  := name               TEXT
 argv[4]  := screen_name        TEXT
 argv[5]  := location           TEXT
 argv[6]  := description        TEXT
 argv[7]  := profile_image_url  TEXT
 argv[8]  := url                TEXT
 argv[9]  := protected          INTEGER
 argv[10] := followers_count    INTEGER
 argv[11] := friends_count      INTEGER
 argv[12] := created_at         INTEGER
 argv[13] := favorites_count    INTEGER
 argv[14] := statuses_count     INTEGER
 argv[15] := following          INTEGER
 argv[16] := statusnet_blocking INTEGER

*/

	if(argv[3]) au->name = strndup(argv[3], PIPE_BUF);
	if(argv[4]) au->screen_name = strndup(argv[4], PIPE_BUF);
	if(argv[5]) au->location = strndup(argv[5], PIPE_BUF);
	if(argv[6]) au->description = strndup(argv[6], PIPE_BUF);
	if(argv[7]) au->profile_image_url = strndup(argv[7], PIPE_BUF);
	if(argv[8]) au->url = strndup(argv[8], PIPE_BUF);
	if(argv[9]) au->protected = atoi(argv[9]);
	if(argv[10]) au->followers_count = atoi(argv[10]);
	if(argv[11]) au->friends_count = atoi(argv[11]);
	if(argv[12]) au->created_at = atoi(argv[12]);
	if(argv[13]) au->favorites_count = atoi(argv[13]);
	if(argv[14]) au->statuses_count = atoi(argv[14]);
	if(argv[15]) au->following = atoi(argv[15]);
	if(argv[16]) au->statusnet_blocking = atoi(argv[16]);
	au->in_db = EINA_TRUE;

	return(0);
}

anUser *fetch_user_from_db(long long int uid) {
	char *query, *db_err=NULL;
	int sqlite_res = 0, res = 0;
	anUser *au = NULL;

	res = asprintf(&query, "SELECT users.* FROM users WHERE uid = %lld LIMIT 1;", uid);
	if(res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, fetch_user_from_db_handler, &au, &db_err);
		if(sqlite_res != 0) {
			printf("Can't run %s: %s\n", query, db_err);
			sqlite3_free(db_err);
		}
	} 
	return(au);
}

char *ed_status_label_get(void *data, Evas_Object *obj, const char *part) {
	char buf[256], datetime[19], *key=NULL;
	aStatus *as = (aStatus *)data;
	int res=0;
	anUser *au;
	struct tm date_tm;

	if (!strcmp(part, "elm.text")) {
		snprintf(buf, sizeof(buf), "%s", as->text);
	} else if (!strcmp(part, "elm.name")) {
		res = asprintf(&key, "%lld", as->user);
		if(res != -1)  {
			au = eina_hash_find(userHash, key);
			snprintf(buf, sizeof(buf), "%s", au->name);
		} else snprintf(buf, sizeof(buf), "unknown");
	} else if (!strcmp(part, "elm.date")) {
		if(localtime_r(&(as->created_at), &date_tm)) {
			strftime(datetime, sizeof(datetime), "%F %R", &date_tm);
			snprintf(buf, sizeof(buf), datetime);
		} else snprintf(buf, 8, "unknown");
	}

	if(res != -1 && key) free(key);

	return(strdup(buf));
}

Evas_Object *ed_status_icon_get(void *data, Evas_Object *obj, const char *part) {
	aStatus *as = (aStatus *)data;
	Evas_Object *icon=NULL;

	if (!strcmp(part, "elm.swallow.icon")) {
		icon = ed_get_icon(as->user, win);
		evas_object_smart_callback_add(icon, "clicked", on_bubble_icon_clicked, as);
	}
	return(icon);
}

Eina_Bool ed_status_state_get(void *data, Evas_Object *obj, const char *part) {
	return(EINA_FALSE);
}

void ed_status_del(void *data, Evas_Object *obj) {
}

static void ed_status_action(void *data, Evas_Object *obj, void *event_info) {
	Elm_Genlist_Item *li = (Elm_Genlist_Item*)event_info;
	aStatus *as = (aStatus*)elm_genlist_item_data_get(li);
	printf("DO SOMETHING WITH THIS STATUS: %s\n", as->text);
}

static int add_status(void *data, int argc, char **argv, char **azColName) {
	anUser *au=NULL;
	aStatus *as=NULL;

	Elm_Genlist_Item *li=NULL;
	Evas_Object *icon=NULL;
	char *uid_str=NULL, *sid_str=NULL;

	/* In this query handler, these are the current fields:
        argv[0]  := messages.id                       INTEGER
        argv[1]  := messages.s_id                     INTEGER
        argv[2]  := messages.account_id               INTEGER
        argv[3]  := messages.timeline                 INTEGER
        argv[4]  := messages.s_text                   TEXT
        argv[5]  := messages.s_truncated              INTEGER
        argv[6]  := messages.s_created_at             INTEGER
        argv[7]  := messages.s_in_reply_to_status_id  INTEGER
        argv[8]  := messages.s_source                 TEXT
        argv[9]  := messages.s_in_reply_to_user_id    INTEGER
        argv[10] := messages.s_favorited              INTEGER
        argv[11] := messages.s_user                   INTEGER
        argv[12] := accounts.type                     INTEGER
		argv[13] := accounts.id                       INTEGER
		argv[14] := accounts.enabled                  INTEGER
	*/

	sid_str = strndup(argv[1], PIPE_BUF);
	as = eina_hash_find(statusHash, sid_str);
	if(!as) {
		as = (aStatus*)calloc(1, sizeof(aStatus));
		if(!as) return(-1);

		as->sid = strtoll(argv[1], NULL, 10);
		as->text = strndup(argv[4], PIPE_BUF);
		as->created_at = atoi(argv[6]);
		as->in_reply_to_status_id = strtoll(argv[7], NULL, 10);
		as->in_reply_to_user_id = strtoll(argv[9], NULL, 10);
		as->favorited = atoi(argv[10]);
		as->user = strtoll(argv[11], NULL, 10);
		as->account_type = atoi(argv[12]);
		as->account_id = atoi(argv[13]);
		as->in_db = EINA_TRUE;
		eina_hash_add(statusHash, sid_str, as);
	}

	uid_str = strndup(argv[11], PIPE_BUF);
	if(uid_str) {
		au = eina_hash_find(userHash, uid_str);
		if(!au) {
			au = fetch_user_from_db(as->user);
			if(au) eina_hash_add(userHash, uid_str, au);
			else return(-3);
		}
	} else return(-2);

	if(ed_check_gag(au->screen_name, au->name, as->text)) return(0);

	icon = ed_get_icon(as->user, win);

	itc1.item_style		= "elmdentica";
	itc1.func.label_get	= ed_status_label_get;
	itc1.func.icon_get	= ed_status_icon_get;
	itc1.func.state_get	= ed_status_state_get;
	itc1.func.del		= ed_status_del;

	li = elm_genlist_item_append(scroller, &itc1, as, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	eina_hash_add(bubble2status, &li, as);

	return(0);
}

void show_error(StatusesList * statuses) {
	Evas_Object *bg=NULL, *box=NULL, *frame1=NULL, *frame2=NULL, *label=NULL, *button=NULL;

	/* Error Window */
	error_win = elm_win_add(NULL, _("Error"), ELM_WIN_DIALOG_BASIC);
	elm_win_title_set(error_win, _("µ-blog Error"));
	evas_object_smart_callback_add(error_win, "delete-request", error_win_del, (void*)error_win);

	/* Background */
	bg = elm_bg_add(error_win);
	evas_object_size_hint_weight_set(bg, 1.0, 1.0);
	elm_win_resize_object_add(error_win, bg);
	evas_object_show(bg);

	/* Vertical Box */
	box = elm_box_add(error_win);
	evas_object_size_hint_weight_set(box, 1, 1);
	elm_win_resize_object_add(error_win, box);
	evas_object_show(box);
	
		/* First frame (with message) */
		frame1 = elm_frame_add(box);
			elm_frame_label_set(frame1, statuses->hash_request);
			label = elm_label_add(frame1);
				elm_label_label_set(label, statuses->hash_error);
				elm_frame_content_set(frame1, label);
			evas_object_show(label);
		elm_box_pack_end(box, frame1);
		evas_object_show(frame1);

		/* Second frame (with close button) */
		frame2 = elm_frame_add(box);
			button = elm_button_add(frame2);
				evas_object_smart_callback_add(button, "clicked", error_win_del, NULL);
				elm_button_label_set(button, _("Close"));
				elm_frame_content_set(frame2, button);
			evas_object_show(button);
		elm_box_pack_end(box, frame2);
		evas_object_show(frame2);

	evas_object_show(error_win);
}

void del_status(gpointer data, gpointer user_data) {
	ub_Status	* status=(ub_Status*)data;

	free(status->screen_name);
	free(status->name);
	free(status->text);
	free(status);

}

static int get_messages_for_account(void *pTimeline, int argc, char **argv, char **azColName) {
	char *screen_name=NULL, *password=NULL, *proto=NULL, *domain=NULL, *base_url=NULL;
	int port=0, id=0;
	int timeline = *(int*)pTimeline;
	/* In this query handler, these are the current fields:
		argv[0] == name TEXT
		argv[1] == password TEXT
		argv[2] == type INTEGER
		argv[3] == proto TEXT
		argv[4] == domain TEXT
		argv[5] == port INTEGER
		argv[6] == base_url TEXT
		argv[7] == id INTEGER
	*/

	screen_name = argv[0];
	password = argv[1];
	proto = argv[3];
	domain = argv[4];
	port = atoi(argv[5]);
	base_url = argv[6];
	id = atoi(argv[7]);

	switch(atoi(argv[2])) {
		case ACCOUNT_TYPE_STATUSNET:
		default: { ed_statusnet_timeline_get(id, screen_name, password, proto, domain, port, base_url, timeline); break; }
	}

	return(0);
}

static void get_messages(int timeline) {
	int sqlite_res=0;
	char *db_err=NULL, *query=NULL;

	query = "SELECT name,password,type,proto,domain,port,base_url,id FROM accounts WHERE enabled = 1 and receive = 1;";
	sqlite3_exec(ed_DB, query, get_messages_for_account, &timeline, &db_err);
	if(sqlite_res != 0) {
		printf("Can't run %s: %d => %s\n", query, sqlite_res, db_err);
	}
	sqlite3_free(db_err);
}

void fill_message_list(int timeline) {
	int sqlite_res=0;
	char *db_err=NULL, *query=NULL;

	if(bubble2status) {
		eina_hash_free(bubble2status);
		bubble2status = NULL;
	}

	bubble2status = eina_hash_pointer_new(NULL);
	if(!statusHash) statusHash = eina_hash_string_superfast_new(status_hash_data_free);
    if(!userHash) userHash = eina_hash_string_superfast_new(user_hash_data_free);


	sqlite_res = asprintf(&query, "SELECT messages.*, accounts.type, accounts.id, accounts.enabled FROM messages,accounts where messages.timeline = %d and messages.account_id=accounts.id and accounts.enabled=1 ORDER BY messages.s_created_at DESC LIMIT %d;", timeline, settings->max_messages);
	if(sqlite_res != -1) {
		sqlite_res = 0;
		sqlite3_exec(ed_DB, query, add_status, (void*)(long)timeline, &db_err);
		if(sqlite_res != 0) {
			printf("Can't run %s: %d => %s\n", query, sqlite_res, db_err);
		}
		sqlite3_free(db_err);
		free(query);
	}
}



/* ********** CALLBACKS *********** */
static void on_timeline_friends_reload(void *data, Evas_Object *obj, void *event_info) {
	if(settings->online) get_messages(TIMELINE_FRIENDS);
	make_status_list(TIMELINE_FRIENDS);
	fill_message_list(TIMELINE_FRIENDS);
	evas_object_hide((Evas_Object*)data);
}

static void on_timeline_mentions_reload(void *data, Evas_Object *obj, void *event_info) {
	if(settings->online) get_messages(TIMELINE_MENTIONS);
	make_status_list(TIMELINE_MENTIONS);
	fill_message_list(TIMELINE_MENTIONS);
	evas_object_hide((Evas_Object*)data);
}

static void on_timeline_user_reload(void *data, Evas_Object *obj, void *event_info) {
	if(settings->online) get_messages(TIMELINE_USER);
	make_status_list(TIMELINE_USER);
	fill_message_list(TIMELINE_USER);
	evas_object_hide((Evas_Object*)data);
}

static void on_timeline_dmsgs_reload(void *data, Evas_Object *obj, void *event_info) {
	if(settings->online) get_messages(TIMELINE_DMSGS);
	make_status_list(TIMELINE_DMSGS);
	fill_message_list(TIMELINE_DMSGS);
	evas_object_hide((Evas_Object*)data);
}

static void on_timeline_public_reload(void *data, Evas_Object *obj, void *event_info) {
	if(settings->online) get_messages(TIMELINE_PUBLIC);
	make_status_list(TIMELINE_PUBLIC);
	fill_message_list(TIMELINE_PUBLIC);
	evas_object_hide((Evas_Object*)data);
}

static void on_timeline_favorites_reload(void *data, Evas_Object *obj, void *event_info) {
	if(settings->online) get_messages(TIMELINE_FAVORITES);
	make_status_list(TIMELINE_FAVORITES);
	fill_message_list(TIMELINE_FAVORITES);
	evas_object_hide((Evas_Object*)data);
}

static void on_fs(void *data, Evas_Object *obj, void *event_info) {
	if(settings->fullscreen)
		settings->fullscreen=FALSE;
	else
		settings->fullscreen=TRUE;

	toggle_fullscreen(settings->fullscreen);
}

static int do_post(void *notUsed, int argc, char **argv, char **azColName) {
	struct sqlite3_stmt *post_stmt=NULL;
	char *msg = NULL, *query = NULL, *entry_str=NULL;
	const char *missed = NULL;
	int sqlite_res = 0, res=0;
	Eina_Strbuf *buf = NULL;

	char *screen_name=NULL, *password=NULL, *proto=NULL, *domain=NULL, *base_url=NULL;
	int port=0, id=0, type=0;
	/* In this query handler, these are the current fields:
		argv[0] == id INTEGER
		argv[1] == enabled INTEGER
		argv[2] == name TEXT
		argv[3] == password TEXT
		argv[4] == type INTEGER
		argv[5] == proto TEXT
		argv[6] == domain TEXT
		argv[7] == port INTEGER
		argv[8] == base_url TEXT
	*/

	id = atoi(argv[0]);
	screen_name = argv[2];
	password = argv[3];
	type = atoi(argv[4]);
	proto = argv[5];
	domain = argv[6];
	port = atoi(argv[7]);
	base_url = argv[8];

	buf = eina_strbuf_new();
	entry_str = elm_entry_markup_to_utf8(elm_entry_entry_get(entry));
	eina_strbuf_append(buf, entry_str);
	eina_strbuf_replace_all(buf, "<br>", " ");
	msg=ed_curl_escape(eina_strbuf_string_steal(buf));
	eina_strbuf_free(buf);

	switch(type) {
		case ACCOUNT_TYPE_STATUSNET:
		default: { res = ed_statusnet_post(id, screen_name, password, proto, domain, port, base_url, msg); break; }
	}

	if(res != 0) {
		if( msg && strlen(msg) > 0 )  {
			if(dm_to) {
				sqlite_res = asprintf(&query, "INSERT INTO posts (account_id, dm_to, message) VALUES (%d, '%s', ?);", id, dm_to);
				dm_to=NULL;
			} else {
				sqlite_res = asprintf(&query, "INSERT INTO posts (account_id, message) VALUES (%d, ?);", id);
			}

			if(sqlite_res != -1) {
				sqlite_res = sqlite3_prepare(ed_DB, query, 4096, &post_stmt, &missed);
				if(sqlite_res == 0) {
					sqlite3_bind_text(post_stmt, 1, msg, -1, NULL);
					sqlite3_step(post_stmt);
					sqlite3_reset(post_stmt);
					sqlite3_finalize(post_stmt);
					} else
						fprintf(stderr, "Can't do %s: %d means '%s' was missed in the statement.\n", query, sqlite_res, missed);
				free(query);
			}
		}
	}

	if(msg) free(msg);
	if(entry_str) free(entry_str);

	return(0);
}

static void on_post_dm_set(void *data, Evas_Object *obj, void *event_info) {
	Evas *e = evas_object_evas_get(obj);
	Evas_Object *dm_entry = e?evas_object_name_find(e, "dm_entry"):NULL;
	char* dm_entry_str = dm_entry?elm_entry_markup_to_utf8(elm_entry_entry_get(dm_entry)):NULL;

	if(dm_entry_str && strlen(dm_entry_str) > 0) {
		dm_to = strdup(dm_entry_str);
		if(debug) printf("Sending a DM to %s\n", dm_to);
	}
	evas_object_del((Evas_Object*)data);
	if(dm_entry_str) free(dm_entry_str);
}

static void on_post_dm_cancel(void *data, Evas_Object *obj, void *event_info) {
	evas_object_del((Evas_Object*)data);
}

static void on_post_dm(void *data, Evas_Object *obj, void *event_info) {
	//Evas_Object *entry = data;
	Evas_Object *inwin=NULL, *frame=NULL, *entry=NULL, *box=NULL, *buttons=NULL, *button=NULL;

	inwin = elm_win_inwin_add(win);
		elm_object_style_set(inwin, "minimal_vertical");

		frame = elm_frame_add(win);
			elm_frame_label_set(frame, _("Send a DM to..."));
			box = elm_box_add(win);
				entry = elm_entry_add(win);
					evas_object_name_set(entry, "dm_entry");
					elm_box_pack_end(box, entry);
				evas_object_show(entry);

				buttons = elm_box_add(win);
					elm_box_horizontal_set(buttons, EINA_TRUE);
					elm_box_homogenous_set(buttons, EINA_TRUE);

					button = elm_button_add(win);
						elm_button_label_set(button, _("OK"));
						evas_object_smart_callback_add(button, "clicked", on_post_dm_set, inwin);
						elm_box_pack_end(buttons, button);
					evas_object_show(button);

					button = elm_button_add(win);
						elm_button_label_set(button, _("Cancel"));
						evas_object_smart_callback_add(button, "clicked", on_post_dm_cancel, inwin);
						elm_box_pack_end(buttons, button);
					evas_object_show(button);

					elm_box_pack_end(box, buttons);
				evas_object_show(buttons);
			elm_frame_content_set(frame, box);
		elm_win_inwin_content_set(inwin, frame);
	evas_object_show(inwin);
}

static void on_post_clear(void *data, Evas_Object *obj, void *event_info) {
	Evas_Object *entry = data;
	elm_entry_entry_set(entry, "");
}

static void on_post(void *data, Evas_Object *obj, void *event_info) {
	char *query = NULL, *db_err=NULL;
	int sqlite_res = 0;

	query = "SELECT * FROM accounts WHERE enabled = 1 and send = 1;";
	sqlite_res = sqlite3_exec(ed_DB, query, do_post, NULL, &db_err);
	if(sqlite_res == 0) {
		if(dm_to) dm_to = NULL;
	} else
		fprintf(stderr, "Can't run %s: %d = %s\n", query, sqlite_res, db_err);

	elm_entry_entry_set(entry, "");
	evas_object_focus_set(entry, 0);

	evas_object_hide(hv);
}

static void on_timelines_hv(void *data, Evas_Object *obj, void *event_info) {
	evas_object_show((Evas_Object*)data);
}

static void on_post_hv(void *data, Evas_Object *obj, void *event_info) {
	evas_object_show(hv);
}

static void win_del(void *data, Evas_Object *obj, void *event_info) {
	elm_exit();
}

static void on_entry_changed(void *data, Evas_Object *entry, void *event_info) {
	int i = 140, len=0, res=0;
	char * count_str = NULL;
	char * entry_text = NULL;

	entry_text = elm_entry_markup_to_utf8(elm_entry_entry_get(entry));

	if(!entry_text) {
		len=0;
		if(debug) printf("Entry content was NULL?!\n");
	} else {
 		if(g_utf8_validate(entry_text, -1, NULL))
			len = g_utf8_strlen(entry_text, -1);
		else
			len = strlen(entry_text);
	}

	i-=len;

	if(i<0) {
		res = asprintf(&count_str, "-% dc | ", -1*i);
	} else
		res = asprintf(&count_str, " % dc | ", i);
	if(res != -1) {
		elm_label_label_set(count, count_str);
		free(count_str);
	}
	free(entry_text);
}

static void on_entry_clicked(void *data, Evas_Object *entry, void *event_info) {
	int len;
	const char *first = _("Type your status here...");
	char *msg=elm_entry_markup_to_utf8(elm_entry_entry_get(entry));

	if(g_utf8_validate(first, -1, NULL))
		len = g_utf8_strlen(first, -1);
	else
		len = strlen(first);

	if(first_message && strncmp(msg,first,len) == 0) {
		elm_entry_entry_set(entry, "");
		first_message=0;
	}

	if(msg) free(msg);
}

EAPI int elm_main(int argc, char **argv)
{
	Evas_Object *bg=NULL, *toolbar=NULL, *bt=NULL, *icon=NULL, *box2=NULL, *hoversel=NULL, *edit_panel=NULL, *timelines=NULL, *timelines_panel=NULL;
	char *tmp=NULL;
	int res = 0;
	char buf[PATH_MAX];

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	if((tmp = getenv("HOME")))
		res = asprintf(&home, "%s/.elmdentica", tmp);
	else
		home = ".elmdentica";
	if(res == -1)
		home = ".elmdentica";

	ed_settings_init(argc, argv);

	elmdentica_init();

	win = elm_win_add(NULL, "elmdentica", ELM_WIN_BASIC);
	evas_object_smart_callback_add(win, "delete-request", win_del, NULL);
	evas_object_size_hint_min_set(win, 480, 480);
	evas_object_size_hint_max_set(win, 640, 640);

	bg = elm_bg_add(win);
	evas_object_size_hint_weight_set(bg, 1.0, 1.0);
	elm_win_resize_object_add(win, bg);
	evas_object_show(bg);

	snprintf(buf, sizeof(buf), "%s/themes/default.edj", PKGDATADIR);
	elm_theme_extension_add(NULL, buf);
	ly = elm_layout_add(win);
		elm_layout_file_set(ly, buf, "elmdentica/vertical_layout");
		//elm_layout_theme_set(ly, "elmdentica", "elm/genlist", "default");
		evas_object_size_hint_weight_set(ly, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		elm_win_resize_object_add(win, ly);
	evas_object_show(ly);

	scroller = elm_genlist_add(win);
		evas_object_size_hint_weight_set(scroller, 1, 1);
		evas_object_size_hint_align_set(scroller, -1, -1);
		elm_genlist_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
		elm_genlist_no_select_mode_set(scroller, EINA_TRUE);

		evas_object_smart_callback_add(scroller, "longpressed", ed_status_action, NULL);
		// Statuses list
		make_status_list(TIMELINE_FRIENDS);
		fill_message_list(TIMELINE_FRIENDS);

		elm_layout_content_set(ly, "timeline", scroller);
	evas_object_show(scroller);

	edit_panel = elm_box_add(win);
		elm_box_homogenous_set(edit_panel, 0);
		elm_box_horizontal_set(edit_panel, 1);
		evas_object_size_hint_align_set(edit_panel, -1, 1);

		count = elm_label_add(win);
			elm_label_label_set(count, " 140c | ");
			evas_object_size_hint_weight_set(count, 0, 1);
			evas_object_size_hint_align_set(count, 0, 0);
		evas_object_show(count);
		elm_box_pack_end(edit_panel, count);

		entry = elm_entry_add(win);
			elm_entry_entry_set(entry, _("Type your status here..."));
			elm_entry_single_line_set(entry, 0);
			elm_entry_line_wrap_set(entry, 1);
			evas_object_size_hint_weight_set(entry, 1, 0);
			evas_object_size_hint_align_set(entry, -1, 1);
			evas_object_smart_callback_add(entry, "cursor,changed", on_entry_clicked, NULL);
			evas_object_smart_callback_add(entry, "changed", on_entry_changed, NULL);
		elm_box_pack_end(edit_panel, entry);
		evas_object_show(entry);
	evas_object_show(edit_panel);

	timelines = elm_hover_add(win);
		evas_object_size_hint_weight_set(timelines, 1, 1);
		evas_object_size_hint_align_set(timelines, -1, 0);
	elm_hover_parent_set(timelines, win);

	timelines_panel = elm_box_add(win);
		elm_box_homogenous_set(timelines_panel, 1);
		evas_object_size_hint_align_set(timelines_panel, -1, 0);

		bt = elm_button_add(win);
			evas_object_size_hint_align_set(bt, -1, 0);
			elm_button_label_set(bt, _("Direct Messages"));
			evas_object_smart_callback_add(bt, "clicked", on_timeline_dmsgs_reload, NULL);
			elm_box_pack_end(timelines_panel, bt);
		evas_object_show(bt);

		bt = elm_button_add(win);
			evas_object_size_hint_align_set(bt, -1, 0);
			elm_button_label_set(bt, _("Everyone"));
			evas_object_smart_callback_add(bt, "clicked", on_timeline_public_reload, NULL);
			elm_box_pack_end(timelines_panel, bt);
		evas_object_show(bt);

		bt = elm_button_add(win);
			evas_object_size_hint_align_set(bt, -1, 0);
			elm_button_label_set(bt, _("Just me"));
			evas_object_smart_callback_add(bt, "clicked", on_timeline_user_reload, timelines);
			elm_box_pack_end(timelines_panel, bt);
		evas_object_show(bt);

		bt = elm_button_add(win);
			evas_object_size_hint_align_set(bt, -1, 0);
			elm_button_label_set(bt, _("Favorites"));
			evas_object_smart_callback_add(bt, "clicked", on_timeline_favorites_reload, timelines);
			elm_box_pack_end(timelines_panel, bt);
		evas_object_show(bt);

		bt = elm_button_add(win);
			evas_object_size_hint_align_set(bt, -1, 0);
			elm_button_label_set(bt, _("Replies/Mentions"));
			evas_object_smart_callback_add(bt, "clicked", on_timeline_mentions_reload, timelines);
			elm_box_pack_end(timelines_panel, bt);
		evas_object_show(bt);

		bt = elm_button_add(win);
			evas_object_size_hint_align_set(bt, -1, 0);
			elm_button_label_set(bt, _("Friends & I"));
			evas_object_smart_callback_add(bt, "clicked", on_timeline_friends_reload, timelines);
			elm_box_pack_end(timelines_panel, bt);
		evas_object_show(bt);

		elm_hover_content_set(timelines, "top", timelines_panel);
	evas_object_show(timelines_panel);

	/* toolbar (horizontal box object) */
	toolbar = elm_box_add(win);
		evas_object_size_hint_weight_set(toolbar, 1.0, 0.0);
		evas_object_size_hint_align_set(toolbar, -1, 0);
		elm_box_homogenous_set(toolbar, 0);
		elm_box_horizontal_set(toolbar, 1);


		icon = elm_icon_add(win);
		elm_icon_standard_set(icon, "chat");
		evas_object_show(icon);

		bt = elm_button_add(win);
			evas_object_size_hint_weight_set(bt, 1, 1);
			evas_object_size_hint_align_set(bt, -1, 0);
			elm_button_label_set(bt, _("Timelines"));
			elm_button_icon_set(bt, icon);
			evas_object_smart_callback_add(bt, "clicked", on_timelines_hv, timelines);
			elm_box_pack_end(toolbar, bt);

			elm_hover_target_set(timelines, bt);
		evas_object_show(bt);


		hv = elm_hover_add(win);
			elm_object_style_set(hv, "popout");
			elm_hover_parent_set(hv, win);
			elm_hover_content_set(hv, "top", edit_panel);

			icon = elm_icon_add(win);
			elm_icon_standard_set(icon, "edit");
			evas_object_show(icon);

			bt = elm_button_add(win);
				evas_object_size_hint_weight_set(bt, 1, 1);
				evas_object_size_hint_align_set(bt, -1, 0);
				elm_button_label_set(bt, _("Post"));
				elm_button_icon_set(bt, icon);
				evas_object_smart_callback_add(bt, "clicked", on_post_hv, NULL);
				elm_box_pack_end(toolbar, bt);
			evas_object_show(bt);

		elm_hover_target_set(hv, toolbar);

		box2 = elm_box_add(win);
		elm_box_horizontal_set(box2, EINA_TRUE);

			icon = elm_icon_add(win);
			elm_icon_standard_set(icon, "edit");
			evas_object_show(icon);

			bt = elm_button_add(win);
				evas_object_size_hint_weight_set(bt, 1, 1);
				evas_object_size_hint_align_set(bt, -1, 0);
				elm_button_label_set(bt, _("Send"));
				elm_button_icon_set(bt, icon);
				evas_object_smart_callback_add(bt, "clicked", on_post, hv);
				elm_box_pack_end(box2, bt);
			evas_object_show(bt);

			icon = elm_photo_add(win);
			elm_photo_file_set(icon, "head.png");
			evas_object_show(icon);

			bt = elm_button_add(win);
				evas_object_size_hint_weight_set(bt, 1, 1);
				evas_object_size_hint_align_set(bt, -1, 0);
				elm_button_label_set(bt, _("DM"));
				elm_button_icon_set(bt, icon);
				evas_object_smart_callback_add(bt, "clicked", on_post_dm, hv);
				elm_box_pack_end(box2, bt);
			evas_object_show(bt);

			icon = elm_icon_add(win);
			elm_icon_standard_set(icon, "delete");
			evas_object_show(icon);

			bt = elm_button_add(win);
				evas_object_size_hint_weight_set(bt, 1, 1);
				evas_object_size_hint_align_set(bt, -1, 0);
				elm_button_label_set(bt, _("Clear"));
				elm_button_icon_set(bt, icon);
				evas_object_smart_callback_add(bt, "clicked", on_post_clear, entry);
				elm_box_pack_end(box2, bt);
			evas_object_show(bt);

			elm_hover_content_set(hv, "middle", box2);
		evas_object_show(box2);

		hoversel = elm_hoversel_add(win);
			evas_object_size_hint_weight_set(hoversel, 1, 1);
			evas_object_size_hint_align_set(hoversel, -1, 0);
			elm_hoversel_hover_begin(hoversel);
			elm_hoversel_hover_parent_set(hoversel, win);
			elm_hoversel_label_set(hoversel, _("More..."));
			
			elm_hoversel_item_add(hoversel, _("Fullscreen"), NULL, ELM_ICON_NONE, on_fs, NULL);
			elm_hoversel_item_add(hoversel, _("Settings"), NULL, ELM_ICON_NONE, on_settings, NULL);
			
			elm_hoversel_hover_end(hoversel);
		elm_box_pack_end(toolbar, hoversel);
		evas_object_show(hoversel);

	elm_layout_content_set(ly, "toolbar", toolbar);
	evas_object_show(toolbar);

	evas_object_resize(win, 480, 640);
	evas_object_show(win);


	if(settings->fullscreen) toggle_fullscreen(settings->fullscreen);

	curl_global_init(CURL_GLOBAL_ALL);

	finger_size =  elm_finger_size_get();

	elm_run();
	elm_shutdown();

	ed_settings_shutdown();

	if(re_amp) g_regex_unref(re_amp);
	if(re_link) g_regex_unref(re_link);
	if(re_user) g_regex_unref(re_user);
	if(re_link_content) g_regex_unref(re_link_content);
	if(re_nl) g_regex_unref(re_nl);
	if(re_entities) g_regex_unref(re_entities);

	return 0;
}

ELM_MAIN()
