/*
 * vim:ts=4
 * 
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
#include "twitter.h"
#include "statusnet.h"
#include "curl.h"

Evas_Object *status_list=NULL, *scroller=NULL, *status=NULL, *win=NULL, *error_win=NULL, *entry=NULL, *fs=NULL, *count=NULL, *url_win=NULL, *hv=NULL;
char * dm_to=NULL;

StatusesList	*statuses=NULL;
int debug=0, mouse_x=0, mouse_y=0;
Evas_Coord finger_size;
int first_message=1;
time_t now;

Eina_Hash * status2user=NULL;

char * reply_id=NULL;
char * url_post = NULL;
char * url_friends = NULL;
extern char * browsers[];
extern char * browserNames[];
char * follow_user=NULL;

extern Settings *settings;

extern CURL * user_agent;

double mouse_held_down=0;

struct sqlite3 *ed_DB=NULL;

GRegex *re_link=NULL, *re_link_content=NULL, *re_amp=NULL, *re_user=NULL, *re_nouser=NULL, *re_group=NULL, *re_nogroup=NULL;
GError *re_err=NULL;

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

	query = "CREATE TABLE IF NOT EXISTS messages (id INTEGER PRIMARY KEY, status_id INTEGER, account_id INTEGER CONSTRAINT account_id_ref REFERENCES accounts (id), screen_name TEXT, name TEXT, message TEXT, date INTEGER, timeline INTEGER);";
	sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
	if(sqlite_res != 0) {
		printf("Can't run %s: %d => %s\n", query, sqlite_res, db_err);
		exit(sqlite_res);
	}

	query = "ALTER TABLE messages ADD COLUMN timeline INTEGER;";
	sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);

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

	if(status_list != NULL)
		evas_object_del(status_list);
	status_list = elm_box_add(win);
	evas_object_size_hint_weight_set(status_list, 1.0, 1.0);
	evas_object_size_hint_align_set(status_list, -1, -1);

	elm_scroller_content_set(scroller, status_list);
	evas_object_show(status_list);
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

void ed_statusnet_favorite_create(int id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, int timeline) {
}

void ed_statusnet_favorite_destroy(int id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, int timeline) {
}

static int ed_mark_favorite(void *data, int argc, char **argv, char **azColName) {
	ub_Bubble * status = (ub_Bubble*)data;
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

	if(status->favorite) {
		switch(atoi(argv[2])) {
			case ACCOUNT_TYPE_TWITTER: { ed_twitter_favorite_destroy(id, screen_name, password, proto, domain, port, base_url, status->status_id); break; }
			case ACCOUNT_TYPE_STATUSNET:
			default: { ed_statusnet_favorite_destroy(id, screen_name, password, proto, domain, port, base_url, status->status_id); break; }
		}
	} else {
		switch(atoi(argv[2])) {
			case ACCOUNT_TYPE_TWITTER: { ed_twitter_favorite_create(id, screen_name, password, proto, domain, port, base_url, status->status_id); break; }
			case ACCOUNT_TYPE_STATUSNET:
			default: { ed_statusnet_favorite_create(id, screen_name, password, proto, domain, port, base_url, status->status_id); break; }
		}
	}
	return(0);
}

static void on_mark_favorite(void *data, Evas_Object *obj, void *event_info) {
	Evas *e;
	Evas_Object *hover;
	ub_Bubble * status = (ub_Bubble*)data;
	int sqlite_res=0;
	char *db_err=NULL, *query=NULL;

	if(!status) return;

	sqlite_res = asprintf(&query, "SELECT name,password,type,proto,domain,port,base_url,id FROM accounts WHERE enabled = 1 and id = %d;", status->account_id);
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
			if(hover) evas_object_del(hover);
		}
	}
}

static void on_repeat(void *data, Evas_Object *obj, void *event_info) {
	Evas *e;
	Evas_Object *hover;
	ub_Bubble * status = (ub_Bubble*)data;
	char * entry_str=NULL, *tmp=NULL, *tmp2=NULL;
	int res = 0;

	if(status) {
		if(!re_nouser)
			re_nouser = g_regex_new("<a href='user://.*?'>(@[a-zA-Z0-9_]+)</a>", 0, 0, &re_err);
		tmp = g_regex_replace(re_nouser, status->message, -1, 0, "\\1", 0, &re_err);

		if(!re_nogroup)
			re_nogroup = g_regex_new("<a href='group://.*?'>(![a-zA-Z0-9_]+)</a>", 0, 0, &re_err);
		tmp2 = g_regex_replace(re_nogroup, tmp, -1, 0, "\\1", 0, &re_err);
		free(tmp); tmp=NULL;

		if(!re_link_content)
			re_link_content = g_regex_new("<a href='(.*?)('>\\[link\\]</a>)", 0, 0, &re_err);
		tmp = g_regex_replace(re_link_content, tmp2, -1, 0, "\\1", 0, &re_err);
		free(tmp2); tmp2=NULL;

		res = asprintf(&entry_str, "♺ @%s: %s", status->screen_name, tmp);
		if(res != -1) {
			elm_entry_entry_set(entry, entry_str);
			free(entry_str);
			free(tmp);
			elm_object_focus(entry);
			elm_entry_cursor_end_set(entry);
		}
	}

	e = evas_object_evas_get(win);
	if(e) {
		hover = evas_object_name_find(e, "hover_actions");
		if(hover) evas_object_del(hover);
	}

	evas_object_show(hv);
}

static void on_reply(void *data, Evas_Object *obj, void *event_info) {
	Evas *e;
	Evas_Object *hover;
	ub_Bubble * status = (ub_Bubble*)data;
	char * entry_str=NULL;
	int res = 0;

	if(status) {
		res = asprintf(&entry_str, "@%s: ", status->screen_name);
		if(res != -1) {
			elm_entry_entry_set(entry, entry_str);
			free(entry_str);
			elm_object_focus(entry);
			reply_id=status->screen_name;
			elm_entry_cursor_end_set(entry);
		}
	}

	e = evas_object_evas_get(win);
	if(e) {
		hover = evas_object_name_find(e, "hover_actions");
		if(hover) evas_object_del(hover);
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

Evas_Object *ed_make_message(char *text, Evas_Object *bubble, Evas_Object *window) {
	Evas_Object *message = NULL;
	char *status_message = NULL, *tmp = NULL;
	int res = 0;

	message = elm_anchorblock_add(window);

		res = asprintf(&status_message, "%s", text);
		if(res == -1) {
			elm_anchorblock_text_set(message, "");
		} else {
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


static void ed_statusnet_user_get(int account_id, UserProfile *user) {
}

static void user_info_get(ub_Bubble *ubBubble, UserProfile *user) {
	switch(ubBubble->account_type) {
		case ACCOUNT_TYPE_TWITTER: { ed_twitter_user_get(ubBubble->account_id, user) ; break; }
		case ACCOUNT_TYPE_STATUSNET:
		default: { ed_statusnet_user_get(ubBubble->account_id, user); break; }
	}
}

void ed_statusnet_user_follow(int id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, char *user_screen_name) {
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
		case ACCOUNT_TYPE_TWITTER: { ed_twitter_user_follow(id, screen_name, password, proto, domain, port, base_url, follow_user); break; }
		case ACCOUNT_TYPE_STATUSNET:
		default: { ed_statusnet_user_follow(id, screen_name, password, proto, domain, port, base_url, follow_user); break; }
	}

	follow_user=NULL;

	return(0);
}
static void on_user_follow(void *data, Evas_Object *obj, void *event_info) {
	ub_Bubble * bubble = (ub_Bubble*)data;
	int sqlite_res=0;
	char *db_err=NULL, *query=NULL;

	if(!bubble) return;

	sqlite_res = asprintf(&query, "SELECT name,password,type,proto,domain,port,base_url,id FROM accounts WHERE enabled = 1 and id = %d;", bubble->account_id);
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
void ed_statusnet_user_abandon(int id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, char *user_screen_name) {
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
		case ACCOUNT_TYPE_TWITTER: { ed_twitter_user_abandon(id, screen_name, password, proto, domain, port, base_url, follow_user); break; }
		case ACCOUNT_TYPE_STATUSNET:
		default: { ed_statusnet_user_abandon(id, screen_name, password, proto, domain, port, base_url, follow_user); break; }
	}

	follow_user=NULL;

	return(0);
}
static void on_user_abandon(void *data, Evas_Object *obj, void *event_info) {
	ub_Bubble * bubble = (ub_Bubble*)data;
	int sqlite_res=0;
	char *db_err=NULL, *query=NULL;

	if(!bubble) return;

	sqlite_res = asprintf(&query, "SELECT name,password,type,proto,domain,port,base_url,id FROM accounts WHERE enabled = 1 and id = %d;", bubble->account_id);
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
	ub_Bubble * ubBubble = eina_hash_find(status2user, &data);
	char *home, *file_path = NULL;
	Evas_Object *zoom=NULL, *icon=NULL;
	int res = 0;

	home = getenv("HOME");
	if(home)
		res = asprintf(&file_path, "%s/.elmdentica/cache/icons/%s", home, ubBubble->screen_name);
	else
		res = asprintf(&file_path, ".elmdentica/cache/icons/%s", ubBubble->screen_name);

	if(res != -1) {
		zoom = elm_win_inwin_add(win);

			icon = elm_icon_add(win);
				evas_object_size_hint_weight_set(icon, 1, 1);
				evas_object_size_hint_align_set(icon, -1, -1);
				elm_icon_file_set(icon, file_path, "fubar?");
				evas_object_smart_callback_add(icon, "clicked", on_zoomed_icon_clicked, zoom);
			evas_object_show(icon);

			elm_win_inwin_content_set(zoom, icon);
		evas_object_show(zoom);
		free(file_path);
	}
}

Evas_Object *ed_make_bubble(Evas_Object *parent, char *nick, time_t date, char *corner) {
	Evas_Object *bubble = NULL, *icon = NULL;
	char *home, *file_path, datestr[19];
	struct tm date_tm;
	int res = 0;

	if(nick && (bubble = elm_bubble_add(parent))) {
		evas_object_size_hint_weight_set(bubble, 1, 0);
		evas_object_size_hint_align_set(bubble, -1, -1);

		if(localtime_r((time_t*)&date, &date_tm)) {
			strftime(datestr, sizeof(datestr), "%F %R", &date_tm);
			elm_bubble_info_set(bubble, datestr);
		}

		elm_bubble_label_set(bubble, nick);

		if(corner) elm_bubble_corner_set(bubble, corner);


		home = getenv("HOME");
		if(home)
			res = asprintf(&file_path, "%s/.elmdentica/cache/icons/%s", home, nick);
		else
			res = asprintf(&file_path, ".elmdentica/cache/icons/%s", nick);
		if(res != -1 && (icon = elm_icon_add(parent))) {
			elm_icon_file_set(icon, file_path, "fubar?");
			evas_object_show(icon);
			evas_object_smart_callback_add(icon, "clicked", on_bubble_icon_clicked, bubble);
			elm_bubble_icon_set(bubble, icon);
			free(file_path);
		}
	}
	return(bubble);
}

static void on_handle_user(void *data, Evas_Object *obj, void *event_info) {
	AnchorData *anchor = (AnchorData*)data;
	Evas_Object *user_win=NULL, *icon=NULL, *bg=NULL, *table=NULL, *button=NULL, *label=NULL, *bubble=anchor->bubble, *message=NULL;
	UserProfile *user;
	ub_Bubble * ubBubble = eina_hash_find(status2user, &bubble);
	char *description=NULL,*home, *path=NULL;
	int res=0;
	struct stat buf;

	if(!settings->online) return;

	user = calloc(1,sizeof(UserProfile));
	if(!user) return;

	memset(user, 0, sizeof(UserProfile));
	user->screen_name=anchor->url+7;
    user->name=NULL;
    user->description=NULL;
    user->tmp=NULL;

	user_info_get(ubBubble, user);

	follow_user=user->screen_name;

	user_win = elm_win_add(NULL, user->screen_name, ELM_WIN_BASIC);
		evas_object_size_hint_min_set(user_win, 480, 480);
		evas_object_size_hint_max_set(user_win, 640, 640);
		elm_win_title_set(user_win, user->screen_name);
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

			home=getenv("HOME");
			if(home)
				res = asprintf(&path, "%s/.elmdentica/cache/icons/%s", home, user->screen_name);
			else
				res = asprintf(&path, ".elmdentica/cache/icons/%s", user->screen_name);

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

				res = asprintf(&description, "%s is following %d and has %d followers.", user->name, user->friends_count, user->followers_count);
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

				if(!user->following && !user->protected) {
					elm_button_label_set(button, _("Follow"));
					evas_object_smart_callback_add(button, "clicked", on_user_follow, ubBubble);
				} else {
					elm_button_label_set(button, _("Stop following"));
					evas_object_smart_callback_add(button, "clicked", on_user_abandon, ubBubble);
				}
				elm_table_pack(table, button, 1, 1, 1, 1);
			evas_object_show(button);

			if(user->text && (button = ed_make_bubble(user_win, user->screen_name, user->status_created_at, NULL))) {
					message = ed_make_message(user->text, button, user_win);
					evas_object_show(button);
					if(message) {
						elm_bubble_content_set(button, message);
						elm_table_pack(table, button, 0, 2, 2, 1);
					} else evas_object_del(bubble);
			}

		evas_object_show(table);

	    evas_object_resize(user_win, 300, 300);
	evas_object_show(user_win);

	user_free(user);
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

static void on_group_join(void *data, Evas_Object *obj, void *event_info) {
	ub_Bubble * ubBubble = (ub_Bubble*)data;
	Evas *e = evas_object_evas_get(obj);
	Evas_Object *group_win = evas_object_name_find(e, "group_win"), *frame = evas_object_name_find(e, "group");
	printf("JOIN GROUP %s from account %d\n", elm_frame_label_get(frame), ubBubble->account_id);
	evas_object_del(group_win);
}

static void on_group_leave(void *data, Evas_Object *obj, void *event_info) {
	ub_Bubble * ubBubble = (ub_Bubble*)data;
	Evas *e = evas_object_evas_get(obj);
	Evas_Object *group_win = evas_object_name_find(e, "group_win"), *frame = evas_object_name_find(e, "group");
	printf("LEAVE GROUP %s from account %d\n", elm_frame_label_get(frame), ubBubble->account_id);
	evas_object_del(group_win);
}

static void on_handle_group(void *data, Evas_Object *obj, void *event_info) {
	AnchorData *anchor = (AnchorData*)data;
	Evas_Object *group_win=NULL, *bg=NULL, *box=NULL, *label=NULL, *s=NULL, *box2=NULL, *bubble=anchor->bubble, *icon=NULL, *button=NULL, *frame=NULL;
	ub_Bubble * ubBubble = eina_hash_find(status2user, &bubble);
	GroupProfile *gp = (GroupProfile*)calloc(1, sizeof(GroupProfile));
	char *m, *home, *path;
	int res = 0;
	struct stat buf;

	gp->name=strndup(anchor->url+8, PIPE_BUF);

	ed_statusnet_group_get(ubBubble->account_id, gp);

	group_win = elm_win_add(NULL, gp->name, ELM_WIN_BASIC);
		evas_object_name_set(frame, "group_win");
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

				home=getenv("HOME");
				if(home)
					res = asprintf(&path, "%s/.elmdentica/cache/icons/%s", home, gp->name);
				else
					res = asprintf(&path, ".elmdentica/cache/icons/%s", gp->name);

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
						evas_object_smart_callback_add(button, "clicked", on_group_leave, ubBubble);
					} else {
						elm_button_label_set(button, _("Join"));
						evas_object_smart_callback_add(button, "clicked", on_group_join, ubBubble);
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


	evas_object_resize(group_win, 300, 300);
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

	if(info->name == NULL || strlen(info->name) <= 9)
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
	ub_Bubble * ubBubble = eina_hash_find(status2user, &bubble);
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
					evas_object_smart_callback_add(button, "clicked", on_reply, ubBubble);
					elm_table_pack(table, button, 0, 0, 1, 1);
				evas_object_show(button);

				button = elm_button_add(win);
					evas_object_size_hint_weight_set(button, 1, 1);
					evas_object_size_hint_align_set(button, -1, 0);
					elm_button_label_set(button, _("Repeat"));
					evas_object_smart_callback_add(button, "clicked", on_repeat, ubBubble);
					elm_table_pack(table, button, 1, 0, 1, 1);
				evas_object_show(button);

				button = elm_button_add(win);
					evas_object_size_hint_weight_set(button, 1, 1);
					evas_object_size_hint_align_set(button, -1, 0);
					if(ubBubble->favorite)
						elm_button_label_set(button, _("Unmark favorite"));
					else
						elm_button_label_set(button, _("Mark favorite"));
					evas_object_smart_callback_add(button, "clicked", on_mark_favorite, ubBubble);
					elm_table_pack(table, button, 0, 1, 2, 1);
				evas_object_show(button);

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
		if(debug) printf(" %s\n", "Yes");
	} else if(debug) printf(" %s\n", "No");

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

static int add_status(void *data, int argc, char **argv, char **azColName) {
	char *screen_name=NULL, *name=NULL, *status_message=NULL;
	int id=0, account_id=0, type;
	Eina_Bool timeline=data?(int)(long)data:0;
	time_t date;

	ub_Bubble * ubBubble = calloc(1, sizeof(ub_Bubble));
	Evas_Object *message=NULL, *bubble=NULL;

	/* In this query handler, these are the current fields:
		argv[0] == id INTEGER
		argv[1] == status_id INTEGER
		argv[2] == account_id INTEGER
		argv[3] == screen_name TEXT
		argv[4] == name TEXT
		argv[5] == message TEXT
		argv[6] == date INTEGER
		argv[7] == type INTEGER
	*/

	id=atoi(argv[0]);
	ubBubble->status_id=atol(argv[1]);
	account_id=atoi(argv[2]);
	screen_name=argv[3];
	name=argv[4];
	date=(time_t)atoi(argv[6]);
	type=atoi(argv[7]);

	if(ed_check_gag(argv[3], argv[4], argv[5])) return(0);

	if((bubble = ed_make_bubble(win, screen_name, date, NULL))) {
		elm_box_pack_end(status_list, bubble);
		evas_object_show(bubble);

		ubBubble->account_id = account_id;
		ubBubble->account_type = type;
		if(screen_name)
			ubBubble->screen_name = strndup(screen_name, 1024);
		else
			ubBubble->screen_name = strdup("");
		ubBubble->message = status_message;

		if(timeline == TIMELINE_FAVORITES)
			ubBubble->favorite = TRUE;

		message = ed_make_message(argv[5], bubble, win);
		
		if(message) {
			elm_bubble_content_set(bubble, message);
			ubBubble->message = strdup(argv[5]);
			eina_hash_add(status2user, (void*)&bubble, (void*)ubBubble);
		} else evas_object_del(bubble);
	}

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
		case ACCOUNT_TYPE_TWITTER: { ed_twitter_timeline_get(id, screen_name, password, proto, domain, port, base_url, timeline); break; }
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

void clear_status_hash_data(void *data) {
	ub_Bubble *bubble = (ub_Bubble*)data;

	if(bubble) {
		if(bubble->screen_name) free(bubble->screen_name);
		if(bubble->message) free(bubble->message);
		free(bubble);
	}
}

void fill_message_list(int timeline) {
	int sqlite_res=0;
	char *db_err=NULL, *query=NULL;

	if(status2user) {
		eina_hash_free(status2user);
		status2user = NULL;
	}

	status2user = eina_hash_pointer_new(clear_status_hash_data);

	sqlite_res = asprintf(&query, "SELECT messages.id,messages.status_id,messages.account_id,messages.screen_name,messages.name,messages.message,messages.date,accounts.type,accounts.id as accid,accounts.enabled FROM messages,accounts where messages.timeline = %d and messages.account_id=accid and accounts.enabled=1 ORDER BY messages.date DESC LIMIT %d;", timeline, settings->max_messages);
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
	char *msg = NULL, *query = NULL;
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
	eina_strbuf_append(buf, elm_entry_entry_get(entry));
	eina_strbuf_replace_all(buf, "<br>", " ");
	msg=ed_curl_escape(eina_strbuf_string_steal(buf));
	eina_strbuf_free(buf);

	switch(type) {
		case ACCOUNT_TYPE_TWITTER: { res = ed_twitter_post(id, screen_name, password, proto, domain, port, base_url, msg) ; break; }
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

	free(msg);

	return(0);
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

	entry_text = (char*)elm_entry_entry_get(entry);

	if(entry_text == NULL) {
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
		res = asprintf(&count_str, "-%dc | ", -1*i);
	} else
		res = asprintf(&count_str, " %dc | ", i);
	if(res != -1) {
		elm_label_label_set(count, count_str);
		free(count_str);
	}
}

static void on_entry_clicked(void *data, Evas_Object *entry, void *event_info) {
	int len;
	const char *first = _("Type your status here..."), *msg=elm_entry_entry_get(entry);

	if(g_utf8_validate(first, -1, NULL))
		len = g_utf8_strlen(first, -1);
	else
		len = strlen(first);

	if(first_message && strncmp(msg,first,len) == 0) {
		elm_entry_entry_set(entry, "");
		first_message=0;
	}
}

EAPI int elm_main(int argc, char **argv)
{
	Evas_Object *bg=NULL, *box=NULL, *toolbar=NULL, *bt=NULL, *icon=NULL, *box2=NULL, *hoversel=NULL, *edit_panel=NULL, *timelines=NULL, *timelines_panel=NULL;

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

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

	/* add a vertical box object */
	box = elm_box_add(win);
	evas_object_size_hint_weight_set(box, 1.0, 1.0);
	evas_object_size_hint_align_set(box, -1, 0);
	elm_win_resize_object_add(win, box);
	elm_box_homogenous_set(box, 0);

	scroller = elm_scroller_add(box);
		evas_object_size_hint_weight_set(scroller, 1, 1);
		evas_object_size_hint_align_set(scroller, -1, -1);
		elm_scroller_bounce_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
		elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_ON);

		// Statuses list
		make_status_list(TIMELINE_FRIENDS);
		fill_message_list(TIMELINE_FRIENDS);

	evas_object_show(scroller);

	elm_box_pack_end(box, scroller);

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

	evas_object_show(toolbar);
	elm_box_pack_end(box, toolbar);

	evas_object_show(box);

	evas_object_resize(win, 300, 300);
	evas_object_show(win);


	if(settings->fullscreen) toggle_fullscreen(settings->fullscreen);

	curl_global_init(CURL_GLOBAL_ALL);

	finger_size =  elm_finger_size_get();

	elm_run();
	elm_shutdown();

	ed_settings_shutdown();

	if(re_link) g_regex_unref(re_link);
	if(re_user) g_regex_unref(re_user);
	if(re_link_content) g_regex_unref(re_link_content);
	if(re_amp) g_regex_unref(re_amp);

	return 0;
}

ELM_MAIN()
