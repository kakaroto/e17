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

#include <libxml/parser.h>
#include <libxml/tree.h>

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

xmlSAXHandler saxHandler;
char * avatar=NULL;
extern struct sqlite3 *ed_DB;
extern int debug;
extern char *dm_to, *reply_id;

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

	sqlite_res = asprintf(&query, "INSERT INTO messages (status_id, account_id, screen_name, name, message, date, timeline) VALUES (?, %d, ?, ?, ?, ?, %d);", account_id, timeline);;
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

	if(dm_to) {
		printf("Direct message to %s\n", dm_to);
	}

	request = calloc(1, sizeof(http_request));

	if(request && strlen(msg) > 0) {
		if(reply_id) {
			res = asprintf(&ub_status, "source=%s&status=%s&in_reply_to_status_id=%s", PACKAGE, msg, reply_id);
			reply_id=NULL;
		} else
			res = asprintf(&ub_status, "source=%s&status=%s", PACKAGE, msg);

		if(res != -1) {
			res  = asprintf(&request->url,"%s://%s:%d%s/statuses/update.xml", proto, domain, port, base_url);
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

void ed_twitter_timeline_get(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, int timeline) {
	int xml_res=0;
	long long int since_id=0;
	char *timeline_str;
	http_request * request=calloc(1, sizeof(http_request));
	StatusesList *statuses=(StatusesList*)g_malloc(sizeof(StatusesList));
	time_t now;

	switch(timeline) {
		case TIMELINE_USER:		{ timeline_str="user";    break; }
		case TIMELINE_PUBLIC:	{ timeline_str="public";  break; }
		case TIMELINE_FRIENDS:
		default:				{ timeline_str="friends"; break; }
	}

	memset(statuses, 0, sizeof(StatusesList));
	statuses->state = FT_NULL;

	ed_twitter_max_status_id(account_id, &since_id, timeline);

	if(timeline < TIMELINE_FAVORITES) {
		if(since_id > 0)
			xml_res = asprintf(&request->url, "%s://%s:%d%s/statuses/%s_timeline.xml?since_id=%lld", proto, domain, port, base_url, timeline_str, since_id);
		else
			xml_res = asprintf(&request->url, "%s://%s:%d%s/statuses/%s_timeline.xml", proto, domain, port, base_url, timeline_str);
	} else if(timeline == TIMELINE_FAVORITES) {
		if(since_id > 0)
			xml_res = asprintf(&request->url, "%s://%s:%d%s/favorites.xml?since_id=%lld", proto, domain, port, base_url, since_id);
		else
			xml_res = asprintf(&request->url, "%s://%s:%d%s/favorites.xml", proto, domain, port, base_url);
	}

	if(xml_res != -1) {
		if (debug) printf("gnome-open %s\n", request->url);

		ed_curl_get(screen_name, password, request, account_id);

		if(request->response_code == 200) {
			ed_twitter_init_friends();
			xmlSubstituteEntitiesDefault(1);

			xml_res = xmlSAXUserParseMemory(&saxHandler, (void*)statuses, request->content.memory, request->content.size);

			if(xml_res != 0) {
				fprintf(stderr,_("FAILED TO SAX FRIENDS: %d\n"),xml_res);
				if (debug) fprintf(stderr,"%s\n",request->content.memory);
			}

			if(statuses->state != HASH) {
				now = time(NULL);
				messages_insert(account_id, statuses->list, timeline);
			} else {
				//show_error(statuses);
			}
		} else {
			printf("http response code was %ld\n", request->response_code);
		}

	}

	if(request->url) free(request->url);
	if(request->content.memory) free(request->content.memory);
	if(request) free(request);

}

void ed_twitter_favorite_create(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, long int status_id) {
	http_request * request=calloc(1, sizeof(http_request));
	int res;

	res = asprintf(&request->url, "%s://%s:%d%s/favorites/create/%ld.xml", proto, domain, port, base_url, status_id);
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
	res = asprintf(&request->url, "%s://%s:%d%s/favorites/destroy/%ld.xml", proto, domain, port, base_url, status_id);
	if(res != -1) {
		ed_curl_post(screen_name, password, request, "", account_id);
		free(request->url);
	}
	if(request) free(request);
}

void ed_twitter_init_friends(void) {
	memset(&saxHandler, '\0', sizeof(xmlSAXHandler));

	saxHandler.startDocument		= ed_twitter_friends_startDocument;
	saxHandler.endDocument			= ed_twitter_friends_endDocument;
	saxHandler.startElement			= ed_twitter_friends_startElement;
	saxHandler.endElement			= ed_twitter_friends_endElement;
	saxHandler.characters			= ed_twitter_friends_characters;
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
				if(res == 0)
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

void ed_twitter_friends_startDocument(void *user_data) {
}

void ed_twitter_friends_endDocument(void *user_data) {
}

void ed_twitter_friends_startElement(void *user_data, const xmlChar * name, const xmlChar ** attrs) {
	StatusesList *statuses = (StatusesList*)user_data;
	ub_Status *status=NULL;

	if(strncmp((char*)name, "hash", 4) == 0 && strlen((char*)name) == 4) {
		statuses->state = HASH;
	} else if(strncmp((char*)name, "error", 5) == 0 && statuses->state == HASH) {
		statuses->state = HASH_ERROR;
	} else if(strncmp((char*)name, "request", 7) == 0 && statuses->state == HASH) {
		statuses->state = HASH_REQUEST;
	} else if(strlen((char*)name) == 8 && strncmp((char*)name, "statuses", 8) == 0) {
		statuses->state = FT_NULL;
	} else if(strlen((char*)name) == 2 && strncmp((char*)name, "id", 2) == 0 && statuses->state == FT_STATUS) {
		statuses->state = FT_ID;
	} else if(strlen((char*)name) == 6 && strncmp((char*)name, "status", 6) == 0) {
		statuses->state = FT_STATUS;

		status = (ub_Status*)malloc(sizeof(ub_Status));
		memset(status, '\0', sizeof(ub_Status));

		statuses->current = status;
	} else if(strncmp((char*)name, "name", 4) == 0)
		statuses->state = FT_NAME;
	else if(strncmp((char*)name, "user", 4) == 0)
		statuses->state = FT_USER;
	else if(strncmp((char*)name, "profile_image_url", 17) == 0)
		statuses->state = FT_AVATAR;
	else if(strncmp((char*)name, "screen_name", 11) == 0)
		statuses->state = FT_SCREEN_NAME;
	else if(statuses->state == FT_STATUS && strncmp((char*)name, "created_at", 10) == 0)
		statuses->state = FT_CREATED_AT;
	else if(strncmp((char*)name, "text", 4) == 0)
		statuses->state = FT_TEXT;
}
void ed_twitter_friends_endElement(void *user_data, const xmlChar * name) {
	StatusesList *statuses = (StatusesList*)user_data;

	if(strncmp((char*)name, "hash", 4) == 0 && strlen((char*)name) == 4) {
		statuses->state = HASH;
	} else if(strncmp((char*)name, "error", 5) == 0 && statuses->state == HASH_ERROR) {
		statuses->state = HASH;
	} else if(strncmp((char*)name, "request", 7) == 0 && statuses->state == HASH_REQUEST) {
		statuses->state = HASH;
	} else if(strlen((char*)name) == 2 && strncmp((char*)name, "id", 2) == 0 && statuses->state == FT_ID) {
		statuses->current->id = atoll(statuses->current->id_str);
		free(statuses->current->id_str);
		statuses->state = FT_STATUS;
	} else if(strncmp((char*)name, "status", 6) == 0 && strlen((char*)name) == 6) {
		statuses->list = eina_list_append(statuses->list, (void*)statuses->current);
		statuses->state = FT_NULL;
	} else if(strncmp((char*)name, "name", 4) == 0)
		statuses->state = FT_USER;
	else if(strncmp((char*)name, "profile_image_url", 17) == 0) {
		statuses->state = FT_USER;
		ed_twitter_statuses_get_avatar(statuses->current->screen_name);
	} else if(strncmp((char*)name, "user", 4) == 0)
		statuses->state = FT_STATUS;
	else if(strncmp((char*)name, "screen_name", 11) == 0)
		statuses->state = FT_USER;
	else if(statuses->state == FT_CREATED_AT && strncmp((char*)name, "created_at", 10) == 0) {
		statuses->current->created_at = curl_getdate(statuses->current->created_at_str, NULL);
		free(statuses->current->created_at_str);
		statuses->state = FT_STATUS;
	} else if(strncmp((char*)name, "text", 4) == 0)
		statuses->state = FT_STATUS;
}
void ed_twitter_friends_characters(void *user_data, const xmlChar * ch, int len) {
	StatusesList *statuses = (StatusesList*)user_data;
	ub_Status *status = statuses->current;
	TimeLineStates * state = &(statuses->state);
	char * tmp, *tmp2;
	int res=0;


	if(*state == HASH_ERROR && ch && len > 0) {
		tmp = statuses->hash_error;
		tmp2 = strndup((char*)ch, len);
		if(tmp) {
			res = asprintf(&statuses->hash_error, "%s%s",tmp,tmp2);
			free(tmp);
			free(tmp2);
		} else {
			statuses->hash_error = tmp2;
		}
	} else if(*state == HASH_REQUEST && ch && len>0) {
		tmp = statuses->hash_request;
		tmp2 = strndup((char*)ch, len);
		if(tmp) {
			res = asprintf(&statuses->hash_request, "%s%s",tmp,tmp2);
			free(tmp);
			free(tmp2);
		} else {
			statuses->hash_request = tmp2;
		}
	} else if(*state == FT_ID && ch && len>0) {
		tmp = status->id_str;
		tmp2 = strndup((char*)ch, len);
		if(tmp) {
			res = asprintf(&status->id_str, "%s%s",tmp,tmp2);
			free(tmp);
			free(tmp2);
		} else {
			status->id_str = tmp2;
		}
	} else if(*state == FT_NAME && ch && len>0) {
		tmp = status->name;
		tmp2 = strndup((char*)ch, len);
		if(tmp) {
			res = asprintf(&status->name, "%s%s",tmp,tmp2);
			free(tmp);
			free(tmp2);
		} else {
			status->name = tmp2;
		}
	} else if(*state == FT_AVATAR && ch && len>0) {
		tmp = avatar;
		tmp2 = strndup((char*)ch, len);
		if(tmp) {
			res = asprintf(&avatar, "%s%s",tmp,tmp2);
			free(tmp);
			free(tmp2);
		} else {
			avatar = tmp2;
		}
	} else if(*state == FT_SCREEN_NAME && ch && len>0) {
		tmp = status->screen_name;
		tmp2 = strndup((char*)ch, len);
		if(tmp) {
			res = asprintf(&status->screen_name, "%s%s",tmp,tmp2);
			free(tmp);
			free(tmp2);
		} else {
			status->screen_name = tmp2;
		}
	} else if(*state == FT_CREATED_AT && ch && len>0) {
		tmp = status->created_at_str;
		tmp2 = strndup((char*)ch, len);
		if(tmp) {
			res = asprintf(&status->created_at_str, "%s%s",tmp,tmp2);
			free(tmp);
			free(tmp2);
		} else {
			status->created_at_str = tmp2;
		}
	} else if(*state == FT_TEXT && ch && len>0) {
		tmp = status->text;
		tmp2 = strndup((char*)ch, len);
		if(tmp) {
			res = asprintf(&status->text, "%s%s",tmp,tmp2);
			free(tmp);
			free(tmp2);
		} else {
			status->text = tmp2;
		}
	}
}

void ed_twitter_users_show_startDocument(void *user_data) {
}

void ed_twitter_users_show_endDocument(void *user_data) {
}
void ed_twitter_users_show_startElement(void *user_data, const xmlChar * name, const xmlChar ** attrs) {
	UserProfile *user = (UserProfile*)user_data;

	if(strncmp((char*)name, "hash", 4) == 0 && strlen((char*)name) == 4) {
		user->state = US_HASH;
	} else if(strncmp((char*)name, "error", 5) == 0 && user->state == US_HASH) {
		user->state = US_HASH_ERROR;
	} else if(strncmp((char*)name, "request", 7) == 0 && user->state == US_HASH) {
		user->state = US_HASH_REQUEST;
	} else if(strlen((char*)name) == 4 && strncmp((char*)name, "name", 4) == 0) {
		user->state = US_NAME;
	} else if(strlen((char*)name) == 11 && strncmp((char*)name, "description", 11) == 0) {
		user->state = US_DESCRIPTION;
	} else if(strlen((char*)name) == 17 && strncmp((char*)name, "profile_image_url", 17) == 0) {
		user->state = US_PROFILE_IMAGE;
	} else if(strlen((char*)name) == 9 && strncmp((char*)name, "protected", 9) == 0) {
		user->state = US_PROTECTED;
	} else if(strlen((char*)name) == 15 && strncmp((char*)name, "followers_count", 15) == 0) {
		user->state = US_FOLLOWERS_COUNT;
	} else if(strlen((char*)name) == 13 && strncmp((char*)name, "friends_count", 13) == 0) {
		user->state = US_FRIENDS_COUNT;
	} else if(strlen((char*)name) == 9 && strncmp((char*)name, "following", 9) == 0) {
		user->state = US_FOLLOWING;
	}
}

void ed_twitter_users_show_endElement(void *user_data, const xmlChar * name) {
	UserProfile *user = (UserProfile*)user_data;

	if(strncmp((char*)name, "hash", 4) == 0 && strlen((char*)name) == 4) {
		user->state = US_ERROR;
	} else if(strncmp((char*)name, "error", 5) == 0 && user->state == US_HASH_ERROR) {
		user->state = US_HASH;
	} else if(strncmp((char*)name, "request", 7) == 0 && user->state == US_HASH_REQUEST) {
		user->state = US_HASH;
	} else if(strlen((char*)name) == 4 && strncmp((char*)name, "name", 4) == 0) {
		user->state = US_NULL;
	} else if(strlen((char*)name) == 11 && strncmp((char*)name, "description", 11) == 0) {
		user->state = US_NULL;
	} else if(strlen((char*)name) == 17 && strncmp((char*)name, "profile_image_url", 17) == 0) {
		avatar=user->profile_image_url;
		ed_twitter_statuses_get_avatar(user->screen_name);
		user->state = US_NULL;
	} else if(strlen((char*)name) == 9 && strncmp((char*)name, "protected", 9) == 0) {
		user->state = US_NULL;
		if(user->tmp) {
			if(strncmp(user->tmp, "true", 4) == 0)
				user->protected = EINA_TRUE;
			else
				user->protected = EINA_FALSE;
			free(user->tmp);
			user->tmp=NULL;
		} else
			user->protected = EINA_FALSE;
	} else if(strlen((char*)name) == 15 && strncmp((char*)name, "followers_count", 15) == 0) {
		user->state = US_NULL;
		if(user->tmp) {
			user->followers_count = atoi(user->tmp);
			free(user->tmp);
			user->tmp=NULL;
		} else
			user->followers_count = 0;
	} else if(strlen((char*)name) == 13 && strncmp((char*)name, "friends_count", 13) == 0) {
		user->state = US_NULL;
		if(user->tmp) {
			user->friends_count = atoi(user->tmp);
			free(user->tmp);
			user->tmp=NULL;
		} else
			user->friends_count = 0;
	} else if(strlen((char*)name) == 9 && strncmp((char*)name, "following", 9) == 0) {
		user->state = US_NULL;
		if(user->tmp) {
			if(strncmp(user->tmp, "true", 4) == 0)
				user->following = EINA_TRUE;
			else
				user->following = EINA_FALSE;
			free(user->tmp);
			user->tmp=NULL;
		} else
			user->following = EINA_FALSE;
	}
}
void ed_twitter_users_show_characters(void *user_data, const xmlChar * ch, int len) {
	UserProfile *user = (UserProfile*)user_data;
	char *tmp=NULL, *tmp2=NULL;
	int res=0;

	if(user->state == US_HASH_ERROR && ch && len > 0) {
		tmp = user->hash_error;
		tmp2 = strndup((char*)ch, len);
		if(tmp) {
			res = asprintf(&user->hash_error, "%s%s",tmp,tmp2);
			free(tmp);
			free(tmp2);
		} else {
			user->hash_error = tmp2;
		}
	} else if(user->state == US_HASH_REQUEST && ch && len>0) {
		//tmp = user->hash_request;
		tmp2 = strndup((char*)ch, len);
		if(user->hash_request) {
			res = asprintf(&tmp, "%s%s",user->hash_request,tmp2);
			free(user->hash_request);
			user->hash_request = tmp;
			free(tmp2);
		} else {
			user->hash_request = tmp2;
		}
	} else if(user->state == US_NAME && ch && len>0) {
		//tmp = user->name;
		tmp2 = strndup((char*)ch, len);
		if(user->name) {
			res = asprintf(&tmp, "%s%s",user->name,tmp2);
			free(user->name);
			user->name = tmp;
			free(tmp2);
		} else {
			user->name = tmp2;
		}
	} else if(user->state == US_DESCRIPTION && ch && len>0) {
		tmp = user->description;
		tmp2 = strndup((char*)ch, len);
		if(tmp) {
			res = asprintf(&user->description, "%s%s",tmp,tmp2);
			free(tmp);
			free(tmp2);
		} else {
			user->description = tmp2;
		}
	} else if(user->state == US_PROFILE_IMAGE && ch && len>0) {
		tmp = user->profile_image_url;
		tmp2 = strndup((char*)ch, len);
		if(tmp) {
			res = asprintf(&user->profile_image_url, "%s%s",tmp,tmp2);
			free(tmp);
			free(tmp2);
		} else {
			user->profile_image_url = tmp2;
		}
	} else if(user->state == US_PROTECTED && ch && len>0) {
		tmp = user->tmp;
		tmp2 = strndup((char*)ch, len);
		if(tmp) {
			res = asprintf(&user->tmp, "%s%s",tmp,tmp2);
			free(tmp);
			free(tmp2);
		} else {
			user->tmp = tmp2;
		}
	} else if(user->state == US_FOLLOWERS_COUNT && ch && len>0) {
		tmp = user->tmp;
		tmp2 = strndup((char*)ch, len);
		if(tmp) {
			res = asprintf(&user->tmp, "%s%s",tmp,tmp2);
			free(tmp);
			free(tmp2);
		} else {
			user->tmp = tmp2;
		}
	} else if(user->state == US_FRIENDS_COUNT && ch && len>0) {
		tmp = user->tmp;
		tmp2 = strndup((char*)ch, len);
		if(tmp) {
			res = asprintf(&user->tmp, "%s%s",tmp,tmp2);
			free(tmp);
			free(tmp2);
		} else {
			user->tmp = tmp2;
		}
	} else if(user->state == US_FOLLOWING && ch && len>0) {
		//tmp = user->tmp;
		tmp2 = strndup((char*)ch, len);
		if(user->tmp) {
			res = asprintf(&tmp, "%s%s",user->tmp,tmp2);
			free(user->tmp);
			user->tmp = tmp;
			free(tmp2);
		} else {
			user->tmp = tmp2;
		}
	}
}

void ed_twitter_init_users_show(UserProfile *user) {
	user->state = US_NULL;
	memset(&saxHandler, '\0', sizeof(xmlSAXHandler));

	saxHandler.startDocument		= ed_twitter_users_show_startDocument;
	saxHandler.endDocument			= ed_twitter_users_show_endDocument;
	saxHandler.startElement			= ed_twitter_users_show_startElement;
	saxHandler.endElement			= ed_twitter_users_show_endElement;
	saxHandler.characters			= ed_twitter_users_show_characters;
}

static int ed_twitter_user_get_handler(void *data, int argc, char **argv, char **azColName) {
	UserGet *ug=(UserGet*)data;
	UserProfile *user = ug->user;
    char *screen_name=NULL, *password=NULL, *proto=NULL, *domain=NULL, *base_url=NULL;
    int port=0, id=0, xml_res;
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

	xml_res = asprintf(&request->url, "%s://%s:%d%s/users/show.xml?screen_name=%s", proto, domain, port, base_url, user->screen_name);

	if(xml_res != -1) {
		if (debug) printf("gnome-open %s\n", request->url);

		ed_curl_get(screen_name, password, request, ug->account_id);

		ed_twitter_init_users_show(user);
		xmlSubstituteEntitiesDefault(1);

		xml_res = xmlSAXUserParseMemory(&saxHandler, (void*)user, request->content.memory, request->content.size);

		if(xml_res != 0) {
			fprintf(stderr,_("FAILED TO SAX USERS SHOW: %d\n"),xml_res);
			if (debug) fprintf(stderr,"%s\n",request->content.memory);
		}

		//if(statuses->state != HASH) {
			//now = time(NULL);
			//messages_insert(account_id, statuses->list, timeline);
		//} else {
			////show_error(statuses);
		//}

	}

	if(request->url) free(request->url);
	if(request->content.memory) free(request->content.memory);
	if(request) free(request);

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

	res = asprintf(&request->url, "%s://%s:%d%s/friendships/create.xml?screen_name=%s", proto, domain, port, base_url, user_screen_name);
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

	res = asprintf(&request->url, "%s://%s:%d%s/friendships/destroy.xml?screen_name=%s", proto, domain, port, base_url, user_screen_name);
	if(res != -1) {
		ed_curl_post(screen_name, password, request, "", account_id);
		if(debug && request->response_code != 200)
			printf("User abandon failed with response code %ld\n", request->response_code);
		free(request->url);
	}
	if(request) free(request);
}
