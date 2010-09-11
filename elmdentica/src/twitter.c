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

#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "cJSON.h"

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

extern struct sqlite3 *ed_DB;
extern int debug;
extern char *home;
extern char *dm_to;
extern long long int reply_id;
extern long long int user_id;
extern Evas_Object *win;

long long max_status_id=0;

Eina_Hash *userHash=NULL;
Eina_Hash *statusHash=NULL;

void status_hash_data_free(void *data) {
	aStatus *s = (aStatus*)data;

    if(s) {
		if(s->text) free(s->text);
    	if(s->source) free(s->source);
		free(s);
	}
}

void user_hash_data_free(void *data) {
	anUser *u = (anUser*)data;

	if(u) {
		if(u->name) free(u->name);
		if(u->screen_name) free(u->screen_name);
		if(u->location) free(u->location);
		if(u->description) free(u->description);
		if(u->profile_image_url) free(u->profile_image_url);
		if(u->url) free(u->url);
		free(u);
	}
}

static int set_max_status_id(void *notUsed, int argc, char **argv, char **azColName) {
	if(!argv[0])
		max_status_id = 0;
	else
		max_status_id = atoi(argv[0]);
	return(0);
}

void message_insert(void *list_item, void *user_data) {
	struct sqlite3_stmt **insert_stmt = (struct sqlite3_stmt**)user_data;
	char *sid_str = (char*)list_item, *uid_str=NULL;
	aStatus *s=NULL;
	anUser *user=NULL;
	long long int sid=0;
	int sqlite_res=0, res=0;

	s = eina_hash_find(statusHash, sid_str);
	if(!s) {
		printf(_("Oops, there should be a status here, for %s!\n"), sid_str);
		return;
	}

	res = asprintf(&uid_str, "%lld", s->user);
	if(res != -1) {
		user = eina_hash_find(userHash, uid_str);
		if(!user) {
			printf(_("Oops, there should be an user here for %lld!\n"), s->user);
			return;
		}
		free(uid_str);
	}

	sid = strtoll(sid_str, NULL, 10);
	if(sid > max_status_id) {
		max_status_id = sid;
		sqlite3_bind_int64(*insert_stmt, 1, sid);
		sqlite3_bind_text(*insert_stmt,  2, s->text, -1, NULL);
		sqlite3_bind_int64(*insert_stmt, 3, s->truncated);
		sqlite3_bind_int64(*insert_stmt, 4, s->created_at);
		sqlite3_bind_int64(*insert_stmt, 5, s->in_reply_to_status_id);
		sqlite3_bind_text(*insert_stmt,  6, s->source, -1, NULL);
		sqlite3_bind_int64(*insert_stmt, 7, s->in_reply_to_user_id);
		sqlite3_bind_int64(*insert_stmt, 8, s->favorited);
		sqlite3_bind_int64(*insert_stmt, 9, s->user);

		sqlite_res = sqlite3_step(*insert_stmt);
		if(sqlite_res != 0 && sqlite_res != 101 ) printf("ERROR: %d while inserting message:\n(%s) %s\n", sqlite_res, user->screen_name,s->text);
		else s->in_db = EINA_TRUE;

		sqlite3_reset(*insert_stmt);
	}
}

Eina_Bool user_insert(const Eina_Hash *hash, const void *key, void *data, void *fdata) {
	anUser *au = (anUser*)data;
	long long int account_id = *(long long int*)fdata;
	int sqlite_res=0;
	struct sqlite3_stmt *insert_stmt=NULL;
	const char *missed=NULL;
	char *query=NULL;

	if(!au) return(EINA_FALSE);

	if(au->in_db == EINA_TRUE) return(EINA_TRUE);

	sqlite_res = asprintf(&query, "insert into users (uid, account_id, name, screen_name, location, description, profile_image_url, url, protected, followers_count, friends_count, created_at, favorites_count, statuses_count, following, statusnet_blocking) values (%s, %lld, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);", (char*)key, account_id);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_prepare_v2(ed_DB, query, 4096, &insert_stmt, &missed);
		if(sqlite_res == 0) {
			sqlite3_bind_text(insert_stmt,  1,  au->name, -1, NULL);
			sqlite3_bind_text(insert_stmt,  2,  au->screen_name, -1, NULL);
			sqlite3_bind_text(insert_stmt,  3,  au->location, -1, NULL);
			sqlite3_bind_text(insert_stmt,  4,  au->description, -1, NULL);
			sqlite3_bind_text(insert_stmt,  5,  au->profile_image_url, -1, NULL);
			sqlite3_bind_text(insert_stmt,  6,  au->url, -1, NULL);
			sqlite3_bind_int64(insert_stmt, 7,  au->protected);
			sqlite3_bind_int64(insert_stmt, 8,  au->followers_count);
			sqlite3_bind_int64(insert_stmt, 9,  au->friends_count);
			sqlite3_bind_int64(insert_stmt, 10, au->created_at);
			sqlite3_bind_int64(insert_stmt, 11, au->favorites_count);
			sqlite3_bind_int64(insert_stmt, 12, au->statuses_count);
			sqlite3_bind_int64(insert_stmt, 13, au->following);
			sqlite3_bind_int64(insert_stmt, 14, au->statusnet_blocking);
			sqlite_res = sqlite3_step(insert_stmt);
			if(sqlite_res != 0 && sqlite_res != 101 ) printf("ERROR: %d while inserting user:\n(%s)\n", sqlite_res, au->screen_name);
			else au->in_db = EINA_TRUE;

			sqlite3_reset(insert_stmt);
			sqlite3_finalize(insert_stmt);
			return(EINA_TRUE);
		} else {
			fprintf(stderr, "Can't do %s: %d means '%s' was missed in the statement.\n", query, sqlite_res, missed);
		}
		free(query);
	}
	return(EINA_FALSE);

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

	sqlite_res = asprintf(&query, "insert into messages (status_id, account_id, timeline, s_text, s_truncated, s_created_at, s_in_reply_to_status_id, s_source, s_in_reply_to_user_id, s_favorited, s_user) values (?, %d, %d, ?, ?, ?, ?, ?, ?, ?, ?);", account_id, timeline);;
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_prepare_v2(ed_DB, query, 4096, &insert_stmt, &missed);
		if(sqlite_res == 0) {
			EINA_LIST_REVERSE_FOREACH(list, l, data) {
				if(debug > 3) printf("Inserting: %s\n", (char*)data);
				message_insert(data, &insert_stmt);
			}
			eina_hash_foreach(userHash, user_insert, &account_id);
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
		} else if(user_id>0 || dm_to)
			res = asprintf(&ub_status, "source=%s&text=%s", PACKAGE, msg);
		else
			res = asprintf(&ub_status, "source=%s&status=%s", PACKAGE, msg);

		if(res != -1) {
			if(user_id) {
				res  = asprintf(&request->url,"%s://%s:%d%s/direct_messages/new.json?user_id=%lld", proto, domain, port, base_url, user_id);
				user_id = 0;
			} else if(dm_to) {
				res  = asprintf(&request->url,"%s://%s:%d%s/direct_messages/new.json?screen_name=%s", proto, domain, port, base_url, dm_to);
				dm_to = NULL;
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

	if(!argv[0])
		*since_id = 0;
	else
		*since_id = strtoll(argv[0], NULL, 10);

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

void ed_twitter_statuses_get_avatar(char *id, char *url) {
	int res=0;
	char * file_path=NULL;

	if(!url || !id) return;

	res = asprintf(&file_path, "%s/cache/icons/%s", home, id);

	if(res != -1) {
		ed_curl_dump_url_to_file(url, file_path);
		free(file_path);
	}
}


anUser *json_timeline_user_parse(cJSON *user) {
	anUser *u;
	cJSON *jo = NULL;

	if(!user) return(NULL);

	u = calloc(1, sizeof(anUser));

	if(!u) return(NULL);

	jo = cJSON_GetObjectItem(user, "id");
	if(jo && jo->type == cJSON_Number)
		u->uid = (long long int)jo->valuedouble;
	else {
		free(u);
		return(NULL);
	}

	jo = cJSON_GetObjectItem(user, "name");
	if(jo && jo->type == cJSON_String)
		u->name = strndup(jo->valuestring, PIPE_BUF);

	jo = cJSON_GetObjectItem(user, "screen_name");
	if(jo && jo->type == cJSON_String)
		u->screen_name = strndup(jo->valuestring, PIPE_BUF);

	jo = cJSON_GetObjectItem(user, "location");
	if(jo && jo->type == cJSON_String)
		u->location = strndup(jo->valuestring, PIPE_BUF);

	jo = cJSON_GetObjectItem(user, "description");
	if(jo && jo->type == cJSON_String)
		u->description = strndup(jo->valuestring, PIPE_BUF);

	jo = cJSON_GetObjectItem(user, "profile_image_url");
	if(jo && jo->type == cJSON_String)
		u->profile_image_url = strndup((char*)jo->valuestring, PIPE_BUF);

	jo = cJSON_GetObjectItem(user, "url");
	if(jo && jo->type == cJSON_String)
		u->url = strndup((char*)jo->valuestring, PIPE_BUF);

	jo = cJSON_GetObjectItem(user, "protected");
	if(jo && (jo->type == cJSON_True || jo->type == cJSON_False))
		u->protected = jo->valueint;

	jo = cJSON_GetObjectItem(user, "followers_count");
	if(jo && jo->type == cJSON_Number)
		u->followers_count = jo->valueint;

	jo = cJSON_GetObjectItem(user, "friends_count");
	if(jo && jo->type == cJSON_Number)
		u->friends_count = jo->valueint;

	jo = cJSON_GetObjectItem(user, "created_at");
	if(jo && jo->type == cJSON_String)
		u->created_at = curl_getdate(jo->valuestring, NULL);

	jo = cJSON_GetObjectItem(user, "favorites_count");
	if(jo && jo->type == cJSON_Number)
		u->favorites_count = jo->valueint;

	jo = cJSON_GetObjectItem(user, "statuses_count");
	if(jo && jo->type == cJSON_Number)
		u->statuses_count = jo->valueint;

	jo = cJSON_GetObjectItem(user, "following");
	if(jo && (jo->type == cJSON_True || jo->type == cJSON_False))
		u->following = jo->valueint;

	jo = cJSON_GetObjectItem(user, "statusnet:blocking");
	if(jo && (jo->type == cJSON_True || jo->type == cJSON_False))
		u->statusnet_blocking = jo->valueint;

	return(u);
}

aStatus *json_timeline_handle_single(int timeline, StatusesList *statuses, cJSON *astatus) {
	cJSON *jo, *user=NULL;
	anUser *u=NULL;
	aStatus *s=NULL;
	char *tmp=NULL, *uid_str=NULL, *sid_str=NULL;
	int res=0;
	long long int uid=0;

	if(!astatus) return(NULL);

	if(statusHash == NULL)
		statusHash = eina_hash_string_superfast_new(status_hash_data_free);
	if(userHash == NULL)
		userHash = eina_hash_string_superfast_new(user_hash_data_free);

	if(timeline == TIMELINE_DMSGS)
		user = cJSON_GetObjectItem(astatus, "sender");
	else
		user = cJSON_GetObjectItem(astatus, "user");

	if(user && user->type == cJSON_Object) {
		jo = cJSON_GetObjectItem(user, "id");
		if(jo && jo->type == cJSON_Number) {
			uid = (long long int)jo->valuedouble;
			if(debug > 3) printf("Status user is: %lld\n", uid);

			res = asprintf(&uid_str, "%lld", uid);
			if(res != -1) {
				u = eina_hash_find(userHash, uid_str);
				if(!u) {
					if(debug>3) printf("User is not already known, parsing it's data\n");
					u = json_timeline_user_parse(user);
					if(u && eina_hash_add(userHash, uid_str, (void*)u)) {
						if(debug>3) printf("Fetching avatar for %s at %s\n", uid_str, u->profile_image_url);
						ed_twitter_statuses_get_avatar(uid_str, u->profile_image_url);
					} else
						printf("Failed to parsed data for user %lld\n", uid);
				} else if(debug > 3) printf("User already known, not parsing it's data\n");
			} else uid_str=NULL;
		}
	}

	s = calloc(1, sizeof(aStatus));
	if(debug>3) printf("Pointer allocated for a status: %ld\n", (long int)s);
	if(!s) return(NULL);
	
	jo = cJSON_GetObjectItem(astatus, "id");
	printf("Tipo: %d\n", jo->type);

	if(jo && jo->type == cJSON_Number) {
		s->sid = (long long int)jo->valuedouble;
		if(debug>3) printf("Getting status id: %lld\n", s->sid);
	}

	jo = cJSON_GetObjectItem(astatus, "text");
	if(jo && jo->type == cJSON_String) {
		s->text = strndup((char*)jo->valuestring, PIPE_BUF);
		if(debug>3) printf("Getting status text: %s\n", s->text);
	}

	jo = cJSON_GetObjectItem(astatus, "created_at");
	if(jo && jo->type == cJSON_String) {
		tmp = (char*)jo->valuestring;
		s->created_at = curl_getdate(tmp, NULL);
		if(debug>3) printf("Getting status created_at: %d\n", (int)s->created_at);
	}

	jo = cJSON_GetObjectItem(astatus, "favorited");
	if(jo && (jo->type == cJSON_True || jo->type == cJSON_False)) {
		s->favorited = jo->valueint;
		if(debug>3) printf("Getting status favorited: %d\n", (int)s->favorited);
	}

	jo = cJSON_GetObjectItem(astatus, "in_reply_to_user_id");
	if(jo && jo->type == cJSON_Number) {
		s->in_reply_to_user_id = (long long int)jo->valuedouble;
		if(debug>3) printf("Getting status in_reply_to_user_id: %d\n", (int)s->in_reply_to_user_id);
	}

	jo = cJSON_GetObjectItem(astatus, "in_reply_to_status_id");
	if(jo && jo->type == cJSON_Number) {
		s->in_reply_to_status_id = (long long int)jo->valuedouble;
		if(debug>3) printf("Getting status in_reply_to_status_id: %d\n", (int)s->in_reply_to_status_id);
	}

	if(uid) s->user = uid;
	if(uid_str) free(uid_str);

	res = asprintf(&sid_str, "%lld", s->sid);
	if(res != -1) {
		if(eina_hash_add(statusHash, sid_str, (void*)s) && debug>3)
			printf("Added status %s to statusHash\n", sid_str);
		else if(debug >3)  printf("Failed to add status %s to statusHash\n", sid_str);
		if(statuses) statuses->list = eina_list_append(statuses->list, (void*)sid_str);
	}

	return(s);
}

void json_timeline_handle(int timeline, StatusesList *statuses, cJSON *json_stream) {
	cJSON *astatus;
	int size,pos=0;

	size = cJSON_GetArraySize(json_stream);

	for(pos=0; pos<size; pos++) {
		if(debug > 3) printf("Parsing status %d of %d\n", pos+1, size);
		astatus = cJSON_GetArrayItem(json_stream, pos);
		if(astatus) {
			json_timeline_handle_single(timeline, statuses, astatus);
		}
	}
}

void json_timeline(int timeline, StatusesList *statuses, char *stream) {
	cJSON *json_stream, *obj;

	json_stream = cJSON_Parse(stream);

	if(!json_stream) {
		fprintf(stderr, "ERROR parsing json stream:\n%s\n", stream);
		return;
	}

	switch(json_stream->type) {
		case cJSON_Array: {
			if(debug > 3) printf("Got an array of json objects\n");
			json_timeline_handle(timeline, statuses, json_stream);
			break;
		}
		case cJSON_Object: {
			obj = cJSON_GetObjectItem(json_stream, "error");
			if(obj)
				fprintf(stderr, "ERROR: %s\n", obj->valuestring);
			else if(!json_timeline_handle_single(timeline, statuses, json_stream))
				fprintf(stderr, "ERROR unexpected content in json stream:\n%s\n", stream);
			break;
		}
		default: {
			fprintf(stderr, "ERROR unsupported json type: %d\n%s\n", json_stream->type, stream);
		}
	}
	cJSON_Delete(json_stream);
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

void ed_twitter_toggle_favorite(int account_id, long long int status_id, Eina_Bool favorite) {
	char *query=NULL, *db_err=NULL, *sid_str=NULL;
	int sqlite_res;
	aStatus *as=NULL;

	sqlite_res = asprintf(&query, "UPDATE messages SET s_favorited=%d WHERE account_id = %d and status_id = %lld;", favorite, account_id, status_id);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
		if(sqlite_res != 0) {
			fprintf(stderr, "Can't do %s: %d means '%s' was missed in the statement.\n", query, sqlite_res, db_err);
			sqlite3_free(db_err);
		}
		free(query);
	}

	sqlite_res = asprintf(&sid_str, "%lld", status_id);
	if(sqlite_res != -1) {
		as = eina_hash_find(statusHash, sid_str);
		if(as) as->favorited=favorite;
		free(sid_str);
	}
}

void ed_twitter_favorite_create(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, long long int status_id) {
	http_request * request=calloc(1, sizeof(http_request));
	int res;

	res = asprintf(&request->url, "%s://%s:%d%s/favorites/create/%lld.json", proto, domain, port, base_url, status_id);
	if(res != -1) {
		ed_curl_post(screen_name, password, request, "", account_id);
		free(request->url);
		if(request->response_code == 200) ed_twitter_toggle_favorite(account_id, status_id, EINA_TRUE);
	}
	if(request) free(request);
}

void ed_twitter_favorite_db_remove(int account_id, long long int status_id) {
	char *query=NULL, *db_err=NULL;;
	int sqlite_res;

	sqlite_res = asprintf(&query, "DELETE FROM messages WHERE account_id = %d and status_id = %lld and timeline = %d;", account_id, status_id, TIMELINE_FAVORITES);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, NULL, NULL, &db_err);
		if(sqlite_res != 0) {
			fprintf(stderr, "Can't do %s: %d means '%s' was missed in the statement.\n", query, sqlite_res, db_err);
			sqlite3_free(db_err);
		}
		free(query);
	}
}

void ed_twitter_favorite_destroy(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, long long int status_id) {
	http_request * request=calloc(1, sizeof(http_request));
	int res;

	ed_twitter_favorite_db_remove(account_id, status_id);
	res = asprintf(&request->url, "%s://%s:%d%s/favorites/destroy/%lld.json", proto, domain, port, base_url, status_id);
	if(res != -1) {
		ed_curl_post(screen_name, password, request, "", account_id);
		free(request->url);
		if(request->response_code == 200) ed_twitter_toggle_favorite(account_id, status_id, EINA_FALSE);
	}
	if(request) free(request);
}

void json_user_show(UserGet *ug, char *stream) {
	cJSON *json_stream, *obj;

	json_stream = cJSON_Parse(stream);
	if(debug) printf("About to parse\n%s\n", stream);

	if(!json_stream) {
		fprintf(stderr, "ERROR parsing json stream:\n%s\n", stream);
		return;
	}

	if(json_stream->type == cJSON_Object) {
		obj = cJSON_GetObjectItem(json_stream, "error");
		if(obj) fprintf(stderr, "ERROR: %s\n", obj->valuestring);
		else ug->au = json_timeline_user_parse(json_stream);
	}
	cJSON_Delete(json_stream);
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


	if(!request) return(-1);

	res = asprintf(&request->url, "%s://%s:%d%s/users/show.json?screen_name=%s", proto, domain, port, base_url, user->screen_name);

	if(res != -1) {
		if (debug) printf("gnome-open %s\n", request->url);

		res = ed_curl_get(screen_name, password, request, ug->account_id);
		if((res == 0) && (request->response_code == 200))
			json_user_show(ug, request->content.memory);

	}

	if(request->url) free(request->url);
	if(request->content.memory) free(request->content.memory);
	free(request);

	return(0);
}

anUser *ed_twitter_user_get(int account_id, UserProfile *user) {
	char *query=NULL, *db_err=NULL;
	int sqlite_res;
	UserGet *ug=calloc(1, sizeof(UserGet));
	anUser *au=NULL;
	
	ug->user=user;
	ug->account_id=account_id;
	sqlite_res = asprintf(&query, "SELECT name,password,type,proto,domain,port,base_url,id FROM accounts WHERE id = %d and type = %d and enabled = 1;", account_id, ACCOUNT_TYPE_TWITTER);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, ed_twitter_user_get_handler, (void*)ug, &db_err);
		if(sqlite_res != 0) {
			fprintf(stderr, "Can't run %s: %d = %s\n", query, sqlite_res, db_err);
			sqlite3_free(db_err);
		}
		free(query);
	}

	au=ug->au;
	free(ug);
	return(ug->au);
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


	if(!request) return(-1);

	res = asprintf(&request->url, "%s://%s:%d%s/statuses/retweet/%lld.json", proto, domain, port, base_url, *(long long int*)data);

	if(res != -1) {
		if (debug) printf("gnome-open %s\n", request->url);

		res = ed_curl_post(screen_name, password, request, "", id);
	}

	if(request->url) free(request->url);
	if(request->content.memory) free(request->content.memory);
	free(request);

	return(0);
}

void ed_twitter_repeat(int account_id, long long int status_id) {
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

static int ed_twitter_status_get_handler(void *data, int argc, char **argv, char **azColName) {
    char *screen_name=NULL, *password=NULL, *proto=NULL, *domain=NULL, *base_url=NULL, *notify_message=NULL;
    int port=0, id=0, res;
	long long int in_reply_to;
	cJSON *json_stream = NULL;
	http_request * request=calloc(1, sizeof(http_request));
	Evas_Object *notify, *label;
	aStatus **prelated_status = (aStatus**)data;

    /* In this query handler, these are the current fields:
        argv[0] == name TEXT
        argv[1] == password TEXT
        argv[2] == type INTEGER
        argv[3] == proto TEXT
        argv[4] == domain TEXT
        argv[5] == port INTEGER
        argv[6] == base_url TEXT
        argv[7] == id INTEGER
        argv[8] == in_reply_to INTEGER
    */

    screen_name = argv[0];
    password = argv[1];
    proto = argv[3];
    domain = argv[4];
    port = atoi(argv[5]);
    base_url = argv[6];
    id = atoi(argv[7]);
    in_reply_to = strtoll(argv[8], NULL, 10);

	if(request == NULL) return(-1);

	res = asprintf(&request->url, "%s://%s:%d%s/statuses/show/%lld.json", proto, domain, port, base_url, in_reply_to);

	if(res != -1) {
		if (debug) printf("gnome-open %s\n", request->url);

		res = ed_curl_get(screen_name, password, request, id);
	
		if((res == 0) && (request->response_code == 200)) {
			printf("Parsing status %lld got...\n%s\n", in_reply_to, request->content.memory);
			json_stream = cJSON_Parse(request->content.memory);


			if(!json_stream) {
				fprintf(stderr, "ERROR parsing json stream:\n%s\n", request->content.memory);
				return(-1);
			}
			*prelated_status = json_timeline_handle_single(-1, NULL, json_stream);
			if(*prelated_status) {
				(*prelated_status)->account_id = id;
				(*prelated_status)->account_type = atoi(argv[2]);
			}
			cJSON_Delete(json_stream);
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
	free(request);

	return(0);
}
void ed_twitter_status_get(int account_id, long long int in_reply_to, aStatus **prelated_status) {
	char *query=NULL, *db_err=NULL;
	int sqlite_res;
	
	sqlite_res = asprintf(&query, "SELECT name,password,type,proto,domain,port,base_url,id,%lld FROM accounts WHERE id = %d and type = %d and enabled = 1;", in_reply_to, account_id, ACCOUNT_TYPE_TWITTER);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, ed_twitter_status_get_handler, (void**)prelated_status, &db_err);
		if(sqlite_res != 0) {
			fprintf(stderr, "Can't run %s: %d = %s\n", query, sqlite_res, db_err);
			sqlite3_free(db_err);
		}
		free(query);
	}
}
