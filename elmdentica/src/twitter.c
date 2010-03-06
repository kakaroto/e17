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

xmlSAXHandler ed_twitter_friends_saxHandler;
gchar * avatar=NULL;
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

void messages_insert(int account_id, GList *list, int timeline) {
	int sqlite_res=0;
	struct sqlite3_stmt *insert_stmt=NULL;
	const char *missed=NULL;
	char *db_err=NULL;
	char *query=NULL;

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
			list = g_list_reverse(list);
			g_list_foreach(list, message_insert, &insert_stmt);
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
				res = ed_curl_post(screen_name, password, request, ub_status);

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
	char *timeline_str=NULL;
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

	if(since_id > 0)
        	xml_res = asprintf(&request->url, "%s://%s:%d%s/statuses/%s_timeline.xml?since_id=%lld", proto, domain, port, base_url, timeline_str, since_id);
	else
        	xml_res = asprintf(&request->url, "%s://%s:%d%s/statuses/%s_timeline.xml", proto, domain, port, base_url, timeline_str);

	if(xml_res != -1) {
		if (debug) printf("gnome-open %s\n", request->url);

		ed_curl_get(screen_name, password, request);

		ed_twitter_init_friends();
		xmlSubstituteEntitiesDefault(1);

		xml_res = xmlSAXUserParseMemory(&ed_twitter_friends_saxHandler, (void*)statuses, request->content.memory, request->content.size);

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

	}

	free(request);

}

void ed_twitter_init_friends(void) {
	memset(&ed_twitter_friends_saxHandler, '\0', sizeof(xmlSAXHandler));

	ed_twitter_friends_saxHandler.startDocument		= ed_twitter_friends_startDocument;
	ed_twitter_friends_saxHandler.endDocument			= ed_twitter_friends_endDocument;
	ed_twitter_friends_saxHandler.startElement			= ed_twitter_friends_startElement;
	ed_twitter_friends_saxHandler.endElement			= ed_twitter_friends_endElement;
	ed_twitter_friends_saxHandler.characters			= ed_twitter_friends_characters;
}

void ed_twitter_statuses_insert_avatar(StatusesList *statuses) {
	http_request *request = NULL;
	int file, res=0;
	char * file_path=NULL;
	char * home=NULL;


	// check wether it's already cached FIXME: this cache doesn't support updating
	home = getenv("HOME");
	res = asprintf(&file_path, "%s/.elmdentica/cache/icons/%s", home, statuses->current->screen_name);
	if(res != 0) {
		file = open(file_path, O_RDONLY);
		// if not, then fetch the icon and write it to the cache
		if(file == -1) {
			file = open(file_path, O_WRONLY | O_CREAT, S_IRUSR|S_IWUSR);
			if(file != -1) {
				request = calloc(1, sizeof(http_request));
				request->url=avatar;
				res = ed_curl_get(NULL, NULL, request);
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
		statuses->list = g_list_append(statuses->list, (gpointer)statuses->current);
		statuses->state = FT_NULL;
	} else if(strncmp((char*)name, "name", 4) == 0)
		statuses->state = FT_USER;
	else if(strncmp((char*)name, "profile_image_url", 17) == 0) {
		statuses->state = FT_USER;
		ed_twitter_statuses_insert_avatar(statuses);
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
