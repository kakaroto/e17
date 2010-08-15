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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <json.h>

#include <glib.h>
#include <glib/gprintf.h>

#include <Elementary.h>
#include <Ecore_X.h>

#include <sqlite3.h>

#include <curl/curl.h>

#include "twitter.h"

#include "gettext.h"
#define _(string) gettext (string)

#include <config.h>
#include "curl.h"
#include "elmdentica.h"

char * avatar=NULL;
extern struct sqlite3 *ed_DB;
extern int debug;
extern char *dm_to;
extern long long int reply_id;
extern long long int user_id;
extern Evas_Object *win;

long long max_status_id=0;

static int set_max_status_id(void *notUsed, int argc, char **argv, char **azColName) {
	if(argv[0] == NULL)
		max_status_id = 0;
	else
		max_status_id = atoi(argv[0]);
	return(0);
}

void message_insert(void *list_item, void *user_data) {
	struct sqlite3_stmt **insert_stmt = (struct sqlite3_stmt**)user_data;
	ub_Status *status = (ub_Status*)list_item;
	int sqlite_res=0;

	if(status->id > max_status_id) {
		max_status_id = status->id;
		sqlite3_bind_int64(*insert_stmt,  1, status->id);
		sqlite3_bind_text(*insert_stmt, 2, status->screen_name, -1, NULL);
		sqlite3_bind_text(*insert_stmt, 3, status->name, -1, NULL);
		sqlite3_bind_text(*insert_stmt, 4, status->text, -1, NULL);
		sqlite3_bind_int64(*insert_stmt, 5, status->created_at);
		sqlite3_bind_int64(*insert_stmt, 6, status->user_id);

		sqlite_res = sqlite3_step(*insert_stmt);
		if(sqlite_res != 0 && sqlite_res != 101 ) printf("ERROR: %d while inserting message:\n(%s) %s\n",sqlite_res, status->screen_name,status->text);

		sqlite3_reset(*insert_stmt);
	}
}

void messages_insert(int account_id, Eina_List *list, int timeline) {
	int sqlite_res=0;
	struct sqlite3_stmt *insert_stmt=NULL;
	const char *missed=NULL;
	char *db_err=NULL;
	char *query=NULL;
	Eina_List *l;
	void *data;

	sqlite_res = asprintf(&query, "SELECT max(status_id) FROM messages where account_id = %d and timeline = %d;", account_id, timeline);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, set_max_status_id, NULL, &db_err);
		if(sqlite_res != 0) {
			fprintf(stderr, "Can't do %s: %d means '%s' was missed in the statement.\n", query, sqlite_res, db_err);
			sqlite3_free(db_err);
		}
		free(query);
	}

	sqlite_res = asprintf(&query, "INSERT INTO messages (status_id, account_id, screen_name, name, message, date, timeline, user_id) VALUES (?, %d, ?, ?, ?, ?, %d, ?);", account_id, timeline);;
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_prepare_v2(ed_DB, query, 4096, &insert_stmt, &missed);
		if(sqlite_res == 0) {
			EINA_LIST_REVERSE_FOREACH(list, l, data)
				message_insert(data, &insert_stmt);
			sqlite3_finalize(insert_stmt);
		} else {
			fprintf(stderr, "Can't do %s: %d means '%s' was missed in the statement.\n", query, sqlite_res, missed);
		}
		free(query);
	}
}

int ed_twitter_post(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, char *msg) {
	char *ub_status=NULL;
	int res=0;
	http_request * request=NULL;

	request = calloc(1, sizeof(http_request));

	if(request && strlen(msg) > 0) {
		if(reply_id>0) {
			res = asprintf(&ub_status, "source=%s&status=%s&in_reply_to_status_id=%lld", PACKAGE, msg, reply_id);
			reply_id=0;
		} else if(user_id>0)
			res = asprintf(&ub_status, "source=%s&text=%s", PACKAGE, msg);
		else
			res = asprintf(&ub_status, "source=%s&status=%s", PACKAGE, msg);

		if(res != -1) {
			if(user_id) {
				res  = asprintf(&request->url,"%s://%s:%d%s/direct_messages/new.json?user_id=%lld", proto, domain, port, base_url, user_id);
				user_id = 0;
			} else
				res  = asprintf(&request->url,"%s://%s:%d%s/statuses/update.json", proto, domain, port, base_url);
			if(res != -1) {
				res = ed_curl_post(screen_name, password, request, ub_status, account_id);

				free(ub_status);
				free(request->url);
				free(request);
			}
		}
	}

	return(0);
}

static int ed_twitter_max_status_id_handler(void *data, int argc, char **argv, char **azColName) {
	long long int *since_id = (long long int*)data;

	if(argv[0] == NULL)
		*since_id = 0;
	else
		*since_id = atoll(argv[0]);

	return(0);
}
void ed_twitter_max_status_id(int account_id, long long int*since_id, int timeline) {
	char *query=NULL, *db_err=NULL;
	int sqlite_res=0;
	
	sqlite_res = asprintf(&query, "SELECT MAX(status_id) FROM messages WHERE account_id = %d and timeline = %d;", account_id, timeline);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, ed_twitter_max_status_id_handler, (void*)since_id, &db_err);
		if(sqlite_res != 0) {
			fprintf(stderr, "Can't run %s: %d = %s\n", query, sqlite_res, db_err);
			sqlite3_free(db_err);
		}
		free(query);
	}
}

void ed_twitter_statuses_get_avatar(char *screen_name) {
	http_request *request = NULL;
	int file, res=0;
	char * file_path=NULL;
	char * home=NULL;


	// properly check if it's already cached (FIXME: this cache doesn't support updating)
	home = getenv("HOME");
	if(home)
		res = asprintf(&file_path, "%s/.elmdentica/cache/icons/%s", home, screen_name);
	else
		res = asprintf(&file_path, ".elmdentica/cache/icons/%s", screen_name);

	if(res != 0) {
		file = open(file_path, O_RDONLY);
		// if not, then fetch the icon and write it to the cache
		if(file == -1) {
			file = open(file_path, O_WRONLY | O_CREAT, S_IRUSR|S_IWUSR);
			if(file != -1) {
				request = calloc(1, sizeof(http_request));
				request->url=avatar;
				res = ed_curl_get(NULL, NULL, request, -1);
				if((res == 0) && (request->response_code == 200))
					res=write(file, request->content.memory, request->content.size);
				close(file);

				free(request);
			} else {
				fprintf(stderr, _("Can't open %s for writing: %s\n"),file_path, strerror(errno));
			}
		} else
			close(file);

		free(file_path);

		free(avatar);

		avatar = NULL;
	}
}


void json_timeline_handle(int timeline, StatusesList *statuses, json_object *json_stream) {
	json_object *status, *id, *text, *created_at, *user, *screen_name, *name, *profile_image_url, *user_id;
	int size,pos=0;
    ub_Status *ubstatus=NULL;

	size = json_object_array_length(json_stream);

	for(pos=0; pos<size; pos++) {
    	ubstatus = (ub_Status*)calloc(1, sizeof(ub_Status));
		status = json_object_array_get_idx(json_stream, pos);

		id = json_object_object_get(status, "id");
		text = json_object_object_get(status, "text");
		created_at = json_object_object_get(status, "created_at");
		if(timeline == TIMELINE_DMSGS)
			user = json_object_object_get(status, "sender");
		else
			user = json_object_object_get(status, "user");
		name = json_object_object_get(user, "name");
		user_id = json_object_object_get(user, "id");
		screen_name = json_object_object_get(user, "screen_name");
		profile_image_url = json_object_object_get(user, "profile_image_url");

		ubstatus->id = json_object_get_int(id);
		ubstatus->user_id = json_object_get_int(user_id);
		ubstatus->name = strndup(json_object_get_string(name), PIPE_BUF);
		ubstatus->screen_name = strndup(json_object_get_string(screen_name), PIPE_BUF);
		ubstatus->text = strndup(json_object_get_string(text), PIPE_BUF);
		avatar = strndup(json_object_get_string(profile_image_url), PIPE_BUF);
		ubstatus->created_at = curl_getdate(json_object_get_string(created_at), NULL);
		ed_twitter_statuses_get_avatar(ubstatus->screen_name);

		statuses->list = eina_list_append(statuses->list, (void*)ubstatus);

		json_object_put(id);
		json_object_put(text);
		json_object_put(created_at);
		json_object_put(name);
		json_object_put(screen_name);
		json_object_put(profile_image_url);
		json_object_put(user);
	}
}

void json_timeline(int timeline, StatusesList *statuses, char *stream) {
	json_object *json_stream, *obj;
	enum json_type json_stream_type;

	json_stream = json_tokener_parse(stream);

	if(json_stream == NULL) {
		fprintf(stderr, "ERROR parsing json stream:\n%s\n", stream);
		return;
	}

	json_stream_type = json_object_get_type(json_stream);

	switch(json_stream_type) {
		case json_type_object: {
			obj = json_object_object_get(json_stream, "error");
			if(obj)
				fprintf(stderr, "ERROR: %s\n", json_object_get_string(obj));
			else
				fprintf(stderr, "ERROR unexpected content in json stream:\n%s\n", stream);
			break;
		}
		case json_type_array: {
			json_timeline_handle(timeline, statuses, json_stream);
			break;
		}
		default: {
			fprintf(stderr, "ERROR unsupported json type: %d\n%s\n", json_stream_type, stream);
		}
	}
	json_object_put(json_stream);
}

void ed_twitter_timeline_get(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, int timeline) {
	int res=0;
	long long int since_id=0;
	char *timeline_str, *notify_message;
	http_request * request=calloc(1, sizeof(http_request));
	StatusesList *statuses=(StatusesList*)calloc(1, sizeof(StatusesList));
	time_t now;
	Evas_Object *notify, *label;

	switch(timeline) {
		case TIMELINE_USER:		{ timeline_str="user";    break; }
		case TIMELINE_PUBLIC:	{ timeline_str="public";  break; }
		case TIMELINE_FRIENDS:
		default:				{ timeline_str="friends"; break; }
	}

	ed_twitter_max_status_id(account_id, &since_id, timeline);

	if(timeline < TIMELINE_FAVORITES) {
		if(since_id > 0)
			res = asprintf(&request->url, "%s://%s:%d%s/statuses/%s_timeline.json?since_id=%lld", proto, domain, port, base_url, timeline_str, since_id);
		else
			res = asprintf(&request->url, "%s://%s:%d%s/statuses/%s_timeline.json", proto, domain, port, base_url, timeline_str);
	} else if(timeline == TIMELINE_FAVORITES) {
		if(since_id > 0)
			res = asprintf(&request->url, "%s://%s:%d%s/favorites.json?since_id=%lld", proto, domain, port, base_url, since_id);
		else
			res = asprintf(&request->url, "%s://%s:%d%s/favorites.json", proto, domain, port, base_url);
	} else if(timeline == TIMELINE_MENTIONS) {
		if(since_id > 0)
			res = asprintf(&request->url, "%s://%s:%d%s/statuses/mentions.json?since_id=%lld", proto, domain, port, base_url, since_id);
		else
			res = asprintf(&request->url, "%s://%s:%d%s/statuses/mentions.json", proto, domain, port, base_url);
	} else if(timeline == TIMELINE_DMSGS) {
		if(since_id > 0)
			res = asprintf(&request->url, "%s://%s:%d%s/direct_messages.json?since_id=%lld", proto, domain, port, base_url, since_id);
		else
			res = asprintf(&request->url, "%s://%s:%d%s/direct_messages.json", proto, domain, port, base_url);
	}

	if(res != -1) {
		if (debug) printf("gnome-open %s\n", request->url);

		res = ed_curl_get(screen_name, password, request, account_id);

		if((res == 0) && (request->response_code == 200)) {
			json_timeline(timeline, statuses, request->content.memory);

			if(res != 0) {
				fprintf(stderr,_("FAILED TO SAX FRIENDS: %d\n"),res);
				if (debug) fprintf(stderr,"%s\n",request->content.memory);
			}

			now = time(NULL);
			messages_insert(account_id, statuses->list, timeline);
		} else {
			res = asprintf(&notify_message, _("%s@%s had HTTP response: %ld"), screen_name, domain, request->response_code);
			if(res != -1) {
				notify = elm_notify_add(win);
					evas_object_size_hint_weight_set(notify, 1, 1);
					evas_object_size_hint_align_set(notify, -1, -1);
					label = elm_label_add(win);
						evas_object_size_hint_weight_set(label, 1, 1);
						evas_object_size_hint_align_set(label, -1, -1);
						elm_label_label_set(label, notify_message);
						elm_label_line_wrap_set(label, EINA_TRUE);
					evas_object_show(label);
					elm_notify_content_set(notify, label);
					elm_notify_orient_set(notify, ELM_NOTIFY_ORIENT_TOP_RIGHT);
					elm_notify_parent_set(notify, win);
					elm_notify_timeout_set(notify, 5);
					elm_notify_timer_init(notify);
				evas_object_show(notify);

				free(notify_message);
			}
		}

	}

	if(request->url) free(request->url);
	if(request->content.memory) free(request->content.memory);
	if(request) free(request);
}

void ed_twitter_favorite_create(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, long int status_id) {
	http_request * request=calloc(1, sizeof(http_request));
	int res;

	res = asprintf(&request->url, "%s://%s:%d%s/favorites/create/%ld.json", proto, domain, port, base_url, status_id);
	if(res != -1) {
		ed_curl_post(screen_name, password, request, "", account_id);
		free(request->url);
	}
	if(request) free(request);
}

void ed_twitter_favorite_db_remove(int account_id, long int status_id) {
	char *query=NULL, *db_err=NULL;;
	int sqlite_res;

	sqlite_res = asprintf(&query, "DELETE FROM messages WHERE account_id = %d and status_id = %ld and timeline = %d;", account_id, status_id, TIMELINE_FAVORITES);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
		if(sqlite_res != 0) {
			fprintf(stderr, "Can't do %s: %d means '%s' was missed in the statement.\n", query, sqlite_res, db_err);
			sqlite3_free(db_err);
		}
		free(query);
	}
}

void ed_twitter_favorite_destroy(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, long int status_id) {
	http_request * request=calloc(1, sizeof(http_request));
	int res;

	ed_twitter_favorite_db_remove(account_id, status_id);
	res = asprintf(&request->url, "%s://%s:%d%s/favorites/destroy/%ld.json", proto, domain, port, base_url, status_id);
	if(res != -1) {
		ed_curl_post(screen_name, password, request, "", account_id);
		free(request->url);
	}
	if(request) free(request);
}

void json_user_show(UserProfile *user, char *stream) {
	json_object *json_stream, *obj, *screen_name_obj, *status_obj;
	enum json_type json_stream_type;

	json_stream = json_tokener_parse(stream);
	if(debug) printf("About to parse\n%s\n", stream);

	if(json_stream == NULL) {
		fprintf(stderr, "ERROR parsing json stream:\n%s\n", stream);
		return;
	}

	json_stream_type = json_object_get_type(json_stream);

	if(json_object_get_type(json_stream) == json_type_object) {
		obj = json_object_object_get(json_stream, "error");
		if(obj) {
			fprintf(stderr, "ERROR: %s\n", json_object_get_string(obj));
			return;
		}

		screen_name_obj = json_object_object_get(json_stream, "screen_name");
		if(screen_name_obj && strncmp(user->screen_name, json_object_get_string(screen_name_obj), strlen(user->screen_name)) == 0 ) {
			json_object_put(screen_name_obj);

			obj = json_object_object_get(json_stream, "name");
			if(obj) {
				user->name = strndup(json_object_get_string(obj), PIPE_BUF);
				json_object_put(obj);
			}

			obj = json_object_object_get(json_stream, "profile_image_url");
			if(obj) {
				avatar = strndup(json_object_get_string(obj), PIPE_BUF);
				ed_twitter_statuses_get_avatar(user->screen_name);
				json_object_put(obj);
			}

			obj = json_object_object_get(json_stream, "protected");
			if(obj) {
				user->protected = (Eina_Bool)json_object_get_boolean(obj);
				json_object_put(obj);
			}

			obj = json_object_object_get(json_stream, "following");
			if(obj) {
				user->following = (Eina_Bool)json_object_get_boolean(obj);
				json_object_put(obj);
			}

			obj = json_object_object_get(json_stream, "friends_count");
			if(obj) {
				user->friends_count = (int)json_object_get_int(obj);
				json_object_put(obj);
			}

			obj = json_object_object_get(json_stream, "followers_count");
			if(obj) {
				user->followers_count = (int)json_object_get_int(obj);
				json_object_put(obj);
			}

			status_obj = json_object_object_get(json_stream, "status");
			if(status_obj) {
				obj = json_object_object_get(status_obj, "text");
				if(obj) {
					user->text = strndup(json_object_get_string(obj), PIPE_BUF);
					json_object_put(obj);
				}

				obj = json_object_object_get(status_obj, "created_at");
				if(obj) {
					user->status_created_at = curl_getdate(json_object_get_string(obj), NULL);
					json_object_put(obj);
				}
				json_object_put(status_obj);
			}
		}
	}
	json_object_put(json_stream);
}

static int ed_twitter_user_get_handler(void *data, int argc, char **argv, char **azColName) {
	UserGet *ug=(UserGet*)data;
	UserProfile *user = ug->user;
    char *screen_name=NULL, *password=NULL, *proto=NULL, *domain=NULL, *base_url=NULL;
    int port=0, id=0, res;
	http_request * request=calloc(1, sizeof(http_request));
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


	if(request == NULL) return(-1);

	res = asprintf(&request->url, "%s://%s:%d%s/users/show.json?screen_name=%s", proto, domain, port, base_url, user->screen_name);

	if(res != -1) {
		if (debug) printf("gnome-open %s\n", request->url);

		res = ed_curl_get(screen_name, password, request, ug->account_id);
		if((res == 0) && (request->response_code == 200))
			json_user_show(user, request->content.memory);

	}

	if(request->url) free(request->url);
	if(request->content.memory) free(request->content.memory);
	free(request);

	return(0);
}

void ed_twitter_user_get(int account_id, UserProfile *user) {
	char *query=NULL, *db_err=NULL;
	int sqlite_res;
	UserGet ug;
	
	ug.user=user;
	ug.account_id=account_id;
	sqlite_res = asprintf(&query, "SELECT name,password,type,proto,domain,port,base_url,id FROM accounts WHERE id = %d and type = %d and enabled = 1;", account_id, ACCOUNT_TYPE_TWITTER);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, ed_twitter_user_get_handler, (void*)&ug, &db_err);
		if(sqlite_res != 0) {
			fprintf(stderr, "Can't run %s: %d = %s\n", query, sqlite_res, db_err);
			sqlite3_free(db_err);
		}
		free(query);
	}
}

void ed_twitter_user_follow(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, char *user_screen_name) {
	http_request * request=calloc(1, sizeof(http_request));
	int res;

	res = asprintf(&request->url, "%s://%s:%d%s/friendships/create.json?screen_name=%s", proto, domain, port, base_url, user_screen_name);
	if(res != -1) {
		ed_curl_post(screen_name, password, request, "", account_id);
		if(debug && request->response_code != 200)
			printf("User follow failed with response code %ld\n", request->response_code);
		free(request->url);
	}
	if(request) free(request);
}
void ed_twitter_user_abandon(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, char *user_screen_name) {
	http_request * request=calloc(1, sizeof(http_request));
	int res;

	res = asprintf(&request->url, "%s://%s:%d%s/friendships/destroy.json?screen_name=%s", proto, domain, port, base_url, user_screen_name);
	if(res != -1) {
		ed_curl_post(screen_name, password, request, "", account_id);
		if(debug && request->response_code != 200)
			printf("User abandon failed with response code %ld\n", request->response_code);
		free(request->url);
	}
	if(request) free(request);
}

static int ed_twitter_repeat_handler(void *data, int argc, char **argv, char **azColName) {
    char *screen_name=NULL, *password=NULL, *proto=NULL, *domain=NULL, *base_url=NULL;
    int port=0, id=0, res;
	http_request * request=calloc(1, sizeof(http_request));
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


	if(request == NULL) return(-1);

	res = asprintf(&request->url, "%s://%s:%d%s/statuses/retweet/%ld.json", proto, domain, port, base_url, *(long int*)data);

	if(res != -1) {
		if (debug) printf("gnome-open %s\n", request->url);

		res = ed_curl_post(screen_name, password, request, "", id);
	}

	if(request->url) free(request->url);
	if(request->content.memory) free(request->content.memory);
	free(request);

	return(0);
}

void ed_twitter_repeat(int account_id, long int status_id) {
	char *query=NULL, *db_err=NULL;
	int sqlite_res;
	
	sqlite_res = asprintf(&query, "SELECT name,password,type,proto,domain,port,base_url,id FROM accounts WHERE id = %d and type = %d and enabled = 1;", account_id, ACCOUNT_TYPE_TWITTER);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, ed_twitter_repeat_handler, (void*)&status_id, &db_err);
		if(sqlite_res != 0) {
			fprintf(stderr, "Can't run %s: %d = %s\n", query, sqlite_res, db_err);
			sqlite3_free(db_err);
		}
		free(query);
	}
}
