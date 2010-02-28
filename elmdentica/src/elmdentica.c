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
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <glib.h>
#include <glib/gprintf.h>

#include <gconf/gconf-client.h>

#include <time.h>

#include <locale.h>
#include "gettext.h"
#define _(string) gettext (string)

#include "elmdentica.h"
#include "settings.h"
#include "twitter.h"
#include "curl.h"

Evas_Object *status_list=NULL, *scroller=NULL, *status=NULL, *win=NULL, *error_win=NULL, *entry=NULL, *fs=NULL, *count=NULL, *url_win=NULL, *zoom=NULL;
Eina_Bool fullscreen=FALSE;
char * dm_to=NULL;

StatusesList	*statuses=NULL;
int debug=0;
int first_message=1;
int MAX_MESSAGES=20;
time_t now;

Eina_Hash * status2user=NULL;
char * reply_id=NULL;

char * url_post = NULL;
char * url_friends = NULL;

extern xmlSAXHandler friends_saxHandler;

extern CURL * user_agent;

double mouse_held_down=0;
double icon_zoom_init=0;

struct sqlite3 *ed_DB=NULL;

GConfClient *conf_client = NULL;
GConfEngine *conf_engine = NULL;

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

	query = "CREATE TABLE IF NOT EXISTS accounts (id INTEGER PRIMARY KEY, enabled INTEGER, name TEXT, password TEXT, type INTEGER, proto TEXT, domain TEXT, port INTEGER, base_url TEXT, receive INTEGER, send INTEGER );";
	sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
	if(sqlite_res != 0) {
		printf("Can't run %s: %d => %s\n", query, sqlite_res, db_err);
		exit(sqlite_res);
	}

	query = "CREATE TABLE IF NOT EXISTS messages (id INTEGER PRIMARY KEY, status_id INTEGER, account_id INTEGER CONSTRAINT account_id_ref REFERENCES accounts (id), screen_name TEXT, name TEXT, message TEXT, date INTEGER);";
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
	char * fs_icon;

	if(new_fullscreen)
		fs_icon="arrow_down";
	else
		fs_icon="arrow_up";

	elm_win_fullscreen_set(win, new_fullscreen);

}

void make_status_list(void) {
	if(status_list != NULL)
		evas_object_del(status_list);
	status_list = elm_box_add(win);
	evas_object_size_hint_weight_set(status_list, 1.0, 1.0);
	evas_object_size_hint_align_set(status_list, -1, 1.0);

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

static void on_direct_message(void *data, Evas_Object *obj, void *event_info) {
	ub_Bubble * status = (ub_Bubble*)data;
	char * entry_str;
	int res = 0;

	if(status) {
		res = asprintf(&entry_str, "@%s: ", status->screen_name);
		if(res != -1) {
			elm_entry_entry_set(entry, entry_str);
			dm_to=status->screen_name;
			free(entry_str);
			elm_object_focus(entry);
		}
	}
}

static void on_repeat(void *data, Evas_Object *obj, void *event_info) {
	ub_Bubble * status = (ub_Bubble*)data;
	char * entry_str=NULL, *tmp=NULL;
	GRegex * re=NULL;
	GError * err=NULL;
	int res = 0;

	if(status) {
		re = g_regex_new("<a href='(.*?)('>\\[link\\]</a>)", 0, 0, &err);
		tmp = g_regex_replace(re, status->message, -1, 0, "\\1", 0, &err);
		g_regex_unref(re);

		res = asprintf(&entry_str, "♺ @%s: %s", status->screen_name, tmp);
		if(res != -1) {
			elm_entry_entry_set(entry, entry_str);
			free(entry_str);
			free(tmp);
			elm_object_focus(entry);
			elm_entry_cursor_end_set(entry);
		}
	}
}

static void on_reply(void *data, Evas_Object *obj, void *event_info) {
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
}


static void url_win_del(void *data, Evas_Object *obj, void *event_info) {
	if(data)
		free(data);
	evas_object_del(url_win);
}

static void on_open_url(void *data, Evas_Object *obj, void *event_info) {
	char * url = (char*)data;
	char * cmd = NULL;
	struct stat buf;
	int sys_result = 0;

	if(stat("/usr/bin/ventura", &buf) == 0) {
		sys_result = asprintf(&cmd, "/usr/bin/ventura -u %s &", url);
	} else if(stat("/usr/bin/woosh", &buf) == 0) {
		sys_result = asprintf(&cmd, "/usr/bin/woosh -u %s &", url);
	} else if(stat("/usr/bin/midori", &buf) == 0) {
		sys_result = asprintf(&cmd, "/usr/bin/midori %s &", url);
	} else if(stat("/usr/bin/dillo", &buf) == 0) {
		sys_result = asprintf(&cmd, "/usr/bin/dillo %s &", url);
	} else if(stat("/usr/bin/xdg-open", &buf) == 0) {
		sys_result = asprintf(&cmd, "/usr/bin/xdg-open %s &", url);
	} else {
		url_win_del(NULL, NULL, NULL);
		if(data) free(data);
		return;
	}

	url_win_del(NULL, NULL, NULL);
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
static void on_message_anchor_clicked(void *data, Evas_Object *obj, void *event_info) {
	Elm_Entry_Anchorblock_Info * info = (Elm_Entry_Anchorblock_Info*)event_info;
	char * url=NULL, *frame_label=NULL, *url2=NULL;
	Evas_Object *box=NULL, *button=NULL, *buttons=NULL, *frame=NULL, *entry=NULL, *bubble=(Evas_Object*)data;
	int res = 0;

	if(info->name == NULL || strlen(info->name) <= 9)
		return;

	url = strndup((char*)info->name, 1024);
	url_win = elm_win_inwin_add(win);
		elm_win_inwin_style_set(url_win, "minimal_vertical");

		box = elm_box_add(win);
			evas_object_size_hint_weight_set(box, 1, 1);
			evas_object_size_hint_align_set(box, -1, 0);

			frame = elm_frame_add(win);
				evas_object_size_hint_weight_set(frame, 1, 1);
				evas_object_size_hint_align_set(frame, -1, -1);

				res = asprintf(&frame_label, _("%s posted this link..."), elm_bubble_label_get(bubble));
				if(res != -1) {
					elm_frame_label_set(frame, frame_label);
					free(frame_label);
				}

				entry = elm_entry_add(win);
					elm_entry_line_char_wrap_set(entry, TRUE);
					elm_entry_editable_set(entry, FALSE);
					url2 = strndup(url+1,strlen(url)-2);
					elm_entry_entry_set(entry, url2);
					free(url2);
				evas_object_show(entry);
				elm_frame_content_set(frame, entry);
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

static void on_zoomed_icon_clicked(void *data, Evas_Object *obj, void *event_info) {
	if(zoom)
		evas_object_del(zoom);
	zoom=NULL;
}

static void zoom_icon(void *data) {
	char *file_path = (char*)data;
	Evas_Object *icon=NULL;

	zoom = elm_win_inwin_add(win);

		icon = elm_icon_add(win);
			evas_object_size_hint_weight_set(icon, 1, 1);
			evas_object_size_hint_align_set(icon, -1, -1);
			elm_icon_file_set(icon, file_path, "fubar?");
			evas_object_smart_callback_add(icon, "clicked", on_zoomed_icon_clicked, NULL);
		evas_object_show(icon);

		elm_win_inwin_content_set(zoom, icon);
	evas_object_show(zoom);
}

static void on_bubble_icon_mouse_down(void *data, Evas *e, Evas_Object *icon, void *event_info) {
	struct timeval tv;

	gettimeofday(&tv, NULL);
	icon_zoom_init = (double)tv.tv_sec + (double)tv.tv_usec/1000000;
}
static void on_bubble_icon_mouse_up(void *data, Evas *e, Evas_Object *icon, void *event_info) {
	struct timeval tv;
	double now=0, delta;

	gettimeofday(&tv, NULL);
	now = (double)tv.tv_sec + (double)tv.tv_usec/1000000;
	delta = now - icon_zoom_init;
	if( (delta >= 0.5) && (delta <= 1.25) )
		zoom_icon(data);
	icon_zoom_init = 0;
}

static void on_bubble_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info) {
	struct timeval tv;

	gettimeofday(&tv, NULL);
	
	mouse_held_down = (double)tv.tv_sec + (double)tv.tv_usec/1000000;
}

static void on_bubble_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info) {
	Evas_Object *hover=NULL, *box=NULL, *table=NULL, *button=NULL, *bubble=(Evas_Object*)data;
	ub_Bubble * ubBubble = eina_hash_find(status2user, &bubble);
	double time_delta;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	time_delta = ((double)tv.tv_sec + (double)tv.tv_usec/1000000) - mouse_held_down;

	if( time_delta < 0.5 || time_delta >= 1.25 ) return;
	else mouse_held_down=0;

	hover = elm_hover_add(win);
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
					elm_button_label_set(button, _("Direct message"));
					evas_object_smart_callback_add(button, "clicked", on_direct_message, ubBubble);
					elm_table_pack(table, button, 0, 1, 2, 1);
				evas_object_show(button);

				evas_object_show(table);
				elm_box_pack_end(box, table);

		evas_object_show(box);

		elm_hover_parent_set(hover, win);
		elm_hover_target_set(hover, bubble);
		elm_hover_content_set(hover, "middle", box);
	evas_object_show(hover);

}

static int add_status(void *notUsed, int argc, char **argv, char **azColName) {
	char *screen_name=NULL, *name=NULL, *status_message=NULL;
	int id=0, account_id=0, status_id=0, res=0;
	time_t date;

	ub_Bubble * ubBubble = calloc(1, sizeof(ub_Bubble));
	Evas_Object *message=NULL, *bubble=NULL, *icon=NULL, *box=NULL;
	time_t now,status_time,time_delta;
	char *tmp;
	char * file_path=NULL, *home=NULL, *timestr = NULL;
	GRegex * re=NULL;
	GError * err=NULL;

	/* In this query handler, these are the current fields:
		argv[0] == id INTEGER
		argv[1] == status_id INTEGER
		argv[2] == account_id INTEGER
		argv[3] == screen_name TEXT
		argv[4] == name TEXT
		argv[5] == message TEXT
		argv[6] == date INTEGER
	*/

	id=atoi(argv[0]);
	status_id=atoi(argv[0]);
	account_id=atoi(argv[2]);
	screen_name=argv[3];
	name=argv[4];
	status_message=argv[5];
	date=(time_t)atoi(argv[6]);

	box = elm_box_add(win);
	evas_object_size_hint_weight_set(box, 1, 0);
	evas_object_size_hint_align_set(box, -1, 0);

	bubble = elm_bubble_add(win);
	evas_object_size_hint_weight_set(bubble, 1, 0);
	evas_object_size_hint_align_set(bubble, -1, 0);

	// make a simplified time display
	now = time(NULL);
	status_time=date;
	time_delta=now-status_time;
	if(time_delta < 0) time_delta=0;
	elm_bubble_label_set(bubble, name);
	if(time_delta < 60) {
		res = asprintf(&timestr, _("%d s ago..."), (int)time_delta);
		if(res != -1)
			elm_bubble_info_set(bubble, timestr);
	} else if(time_delta < 3600) {
		res = asprintf(&timestr, _("± %d min ago..."), (int)time_delta/60);
		if(res != -1)
			elm_bubble_info_set(bubble, timestr);
	} else if(time_delta < 86400) {
		if(time_delta < 7200)
			elm_bubble_info_set(bubble, _("> 1 h ago..."));
		else {
			res = asprintf(&timestr, _("± %d hs ago..."), (int)time_delta/3600);
			if(res != -1)
				elm_bubble_info_set(bubble, timestr);
		}
	} else if(time_delta < 604800) {
		if(time_delta < 172800)
			elm_bubble_info_set(bubble, _("> a day ago..."));
		else {
			res = asprintf(&timestr, _("± %d days ago..."), (int)time_delta/86400);
			if(res != -1)
				elm_bubble_info_set(bubble, timestr);
		}
	} else
		elm_bubble_info_set(bubble, _("a long ago..."));

	if(res != -1 && timestr) free(timestr);

	icon = elm_icon_add(win);

	home = getenv("HOME");
	res = asprintf(&file_path, "%s/.elmdentica/cache/icons/%s", home, screen_name);
	if(res != -1) {
		elm_icon_file_set(icon, file_path, "fubar?");
		evas_object_show(icon);
 		evas_object_event_callback_add(icon, EVAS_CALLBACK_MOUSE_DOWN, on_bubble_icon_mouse_down, file_path);
 		evas_object_event_callback_add(icon, EVAS_CALLBACK_MOUSE_UP, on_bubble_icon_mouse_up, file_path);
		elm_bubble_icon_set(bubble, icon);
	}

	message = elm_anchorblock_add(win);
		evas_object_size_hint_align_set(message, -1, 1);

		re = g_regex_new("&(?!amp;)", 0, 0, &err);
		tmp = g_regex_replace(re, status_message, -1, 0, "&amp;", 0, &err);
		g_regex_unref(re);

		re = g_regex_new("([a-z]+://.*?)(\\s|$)", 0, 0, &err);
		//status_message = g_regex_replace(re, tmp, -1, 0, "<a href='\\1'>\\1</a>\\2", 0, &err);
		status_message = g_regex_replace(re, tmp, -1, 0, "<a href='\\1'>[link]</a>\\2", 0, &err);
		g_regex_unref(re);
		free(tmp);

		elm_anchorblock_text_set(message, status_message);
		evas_object_smart_callback_add(message, "anchor,clicked", on_message_anchor_clicked, bubble);
 		evas_object_event_callback_add(message, EVAS_CALLBACK_MOUSE_DOWN, on_bubble_mouse_down, bubble);
 		evas_object_event_callback_add(message, EVAS_CALLBACK_MOUSE_UP, on_bubble_mouse_up, bubble);
	evas_object_show(message);

	elm_bubble_content_set(bubble, message);

 	//evas_object_event_callback_add(bubble, EVAS_CALLBACK_MOUSE_DOWN, on_bubble_mouse_down, bubble);
 	//evas_object_event_callback_add(bubble, EVAS_CALLBACK_MOUSE_UP, on_bubble_mouse_up, bubble);

	elm_box_pack_end(box, bubble);

	evas_object_show(bubble);

	evas_object_show(box);
	elm_box_pack_end(status_list, box);

	ubBubble->box=box;
	ubBubble->account_id = account_id;
	if(screen_name)
		ubBubble->screen_name = strndup(screen_name, 1024);
	else
		ubBubble->screen_name = strdup("");
	ubBubble->message = status_message;

	eina_hash_add(status2user, (void*)&bubble, (void*)ubBubble);

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

	free(status->id_str);
	free(status->screen_name);
	free(status->name);
	free(status->text);
	free(status->created_at_str);
	free(status);

}

int ed_statusnet_post(int id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, char *msg) {
return(0);
}
void ed_statusnet_timeline_friends_get(int id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url) {
}

static int get_messages_for_account(void *notUsed, int argc, char **argv, char **azColName) {
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
		case ACCOUNT_TYPE_TWITTER: { ed_twitter_timeline_friends_get(id, screen_name, password, proto, domain, port, base_url) ; break; }
		case ACCOUNT_TYPE_STATUSNET:
		default: { ed_statusnet_timeline_friends_get(id, screen_name, password, proto, domain, port, base_url); break; }
	}
	return(0);
}

static void get_messages() {
	int sqlite_res=0;
	char *db_err=NULL, *query=NULL;

	query = "SELECT name,password,type,proto,domain,port,base_url,id FROM accounts WHERE enabled = 1 and receive = 1;";
	sqlite3_exec(ed_DB, query, get_messages_for_account, NULL, &db_err);
	if(sqlite_res != 0) {
		printf("Can't run %s: %d => %s\n", query, sqlite_res, db_err);
	}
	sqlite3_free(db_err);
}

void fill_message_list() {
	int sqlite_res=0;
	char *db_err=NULL, *query=NULL;

	if(status2user) {
	} else {
		status2user = eina_hash_pointer_new(free);
	}
	sqlite_res = asprintf(&query, "SELECT * FROM messages ORDER BY date DESC LIMIT %d;", MAX_MESSAGES);
	if(sqlite_res != -1) {
		sqlite_res = 0;
		sqlite3_exec(ed_DB, query, add_status, NULL, &db_err);
		if(sqlite_res != 0) {
			printf("Can't run %s: %d => %s\n", query, sqlite_res, db_err);
		}
		sqlite3_free(db_err);
		free(query);
	}
}



/* ********** CALLBACKS *********** */
static void on_reload(void *data, Evas_Object *obj, void *event_info)
{
	get_messages();
	make_status_list();
	fill_message_list();

	// alarm(5); in the future, setup an alarm for auto reload? a seperate thread? for 1st release it'll depend on explicit user action...
}

static void on_fs(void *data, Evas_Object *obj, void *event_info) {
	if(fullscreen)
		fullscreen=FALSE;
	else
		fullscreen=TRUE;

	toggle_fullscreen(fullscreen);
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
	const char *first = _("Press here to start typing your message..."), *msg=elm_entry_entry_get(entry);

	if(g_utf8_validate(first, -1, NULL))
		len = g_utf8_strlen(first, -1);
	else
		len = strlen(first);

	if(first_message && strncmp(msg,first,len) == 0) {
		elm_entry_entry_set(entry, "");
		first_message=0;
	}
}

void display_help(void) {
	printf(_("\nUsage:\n"\
		"	-d	Debug mode (extra yummy output)\n"
		"	-h	Help (what you're reading right now)\n"
		"\n")
	);
}

EAPI int elm_main(int argc, char **argv)
{

	static char options[] = "dhm:";
	int option, res=0;
	GConfValue *max_msgs;
	Evas_Object *bg=NULL, *box=NULL, *toolbar=NULL, *bt=NULL, *icon=NULL, *label=NULL, *box2=NULL, *hoversel=NULL;
	GError * err=NULL;
	char * home=NULL;
	char * path=NULL;

	LIBXML_TEST_VERSION

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	conf_engine = gconf_engine_get_default();
	g_type_init();
	gconf_init(argc, argv, NULL);
	conf_client = gconf_client_get_default();

	home=getenv("HOME");
	res = asprintf(&path, "%s/.elmdentica", home);
	if(res != -1) {
		mkdir(path, S_IRWXU);
		free(path);
		res = asprintf(&path, "%s/.elmdentica/cache", home);
		if(res != -1) {
			mkdir(path, S_IRWXU);
			free(path);
			res = asprintf(&path, "%s/.elmdentica/cache/icons", home);
			if(res != -1) {
				mkdir(path, S_IRWXU);
				free(path);
			}
		}
	}

	max_msgs = gconf_client_get(conf_client, "/apps/elmdentica/max_messages", &err);
	if(err) {
		g_error_free(err);
		err=NULL;
		gconf_client_set_int(conf_client, "/apps/elmdentica/max_messages", MAX_MESSAGES, NULL);
	} else if(max_msgs) {
			MAX_MESSAGES = gconf_value_get_int(max_msgs);;
			gconf_value_free(max_msgs);
		}

	while((option = getopt(argc,argv,options)) != -1) {
		switch(option) {
			case 'd': {
				debug=1;
				break;
			}
			case 'h': {
				display_help();
				exit(0);
				break;
			}
			case 'm': {
				MAX_MESSAGES = atoi(optarg);
				break;
			}
			default: {
				display_help();
				exit(1);
			}
		}
	}

	elmdentica_init();

	win = elm_win_add(NULL, "elmdentica", ELM_WIN_BASIC);
	elm_win_title_set(win, _("ElmDentica: µ-blog at your fingertips"));
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
			elm_button_label_set(bt, _("Timeline"));
			elm_button_icon_set(bt, icon);
			evas_object_smart_callback_add(bt, "clicked", on_reload, NULL);
			evas_object_size_hint_weight_set(bt, 1, 1);
			evas_object_size_hint_align_set(bt, -1, 0);
		elm_box_pack_end(toolbar, bt);
		evas_object_show(bt);

		icon = elm_icon_add(win);
		elm_icon_standard_set(icon, "edit");
		evas_object_show(icon);
		bt = elm_button_add(win);
			elm_button_label_set(bt, _("Post"));
			elm_button_icon_set(bt, icon);
			evas_object_smart_callback_add(bt, "clicked", on_post, NULL);
			evas_object_size_hint_weight_set(bt, 1, 1);
			evas_object_size_hint_align_set(bt, -1, 0);
		elm_box_pack_end(toolbar, bt);
		evas_object_show(bt);

		hoversel = elm_hoversel_add(win);
		elm_object_scale_set(hoversel, 1); // FIXME: temp workaround to hoversel bug in shr-u
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

	box2 = elm_box_add(win);
	elm_box_homogenous_set(box2, 0);
	elm_box_horizontal_set(box2, 1);
	evas_object_size_hint_align_set(box2, -1, 0);

		count = elm_label_add(win);
			elm_label_label_set(count, " 140c | ");
			evas_object_size_hint_weight_set(count, 0, 1);
			evas_object_size_hint_align_set(count, 0, 0);
		evas_object_show(count);
		elm_box_pack_end(box2, count);

		entry = elm_entry_add(win);
			elm_entry_entry_set(entry, _("Press here to start typing your message..."));
			elm_entry_single_line_set(entry, 0);
			elm_entry_line_wrap_set(entry, 1);
			evas_object_size_hint_weight_set(entry, 1, 0);
			evas_object_size_hint_align_set(entry, -1, 1);
			evas_object_smart_callback_add(entry, "cursor,changed", on_entry_clicked, NULL);
			evas_object_smart_callback_add(entry, "changed", on_entry_changed, NULL);
		elm_box_pack_end(box2, entry);
		evas_object_show(entry);

	evas_object_show(box2);
	elm_box_pack_end(box, box2);

	label = elm_label_add(box);
	elm_label_label_set(label, _(" <b>Friends timeline...</b>"));
	evas_object_size_hint_align_set(label, 0, 0);
	elm_box_pack_end(box, label);
	evas_object_show(label);

	scroller = elm_scroller_add(box);
		evas_object_size_hint_weight_set(scroller, 1, 1);
		evas_object_size_hint_align_set(scroller, -1, -1);

		// Statuses list
		make_status_list();
		fill_message_list();

	evas_object_show(scroller);

	elm_box_pack_end(box, scroller);

	evas_object_show(box);

	evas_object_resize(win, 300, 300);
	evas_object_show(win);


	fullscreen = gconf_client_get_bool(conf_client, "/apps/elmdentica/fullscreen", &err);
	if(err) {
		g_error_free(err);
		err=NULL;
		gconf_client_set_bool(conf_client, "/apps/elmdentica/fullscreen", 0, NULL);
		fullscreen=0;
	}
	toggle_fullscreen(fullscreen);


	elm_run();
	elm_shutdown();

	gconf_engine_unref(conf_engine);
	xmlCleanupParser();

	return 0;
}

ELM_MAIN()
