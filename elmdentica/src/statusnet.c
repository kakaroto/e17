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
#include "statusnet.h"

#include "gettext.h"
#define _(string) gettext (string)

#include <config.h>
#include "curl.h"
#include "elmdentica.h"

extern struct sqlite3 *ed_DB;
extern int debug;
extern char *home, *dm_to, *reply_id;
extern Evas_Object *win;

int ed_statusnet_post(int id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, char *msg) {
	return(0);
}
void ed_statusnet_timeline_get(int id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, int timeline) {
}

void ed_statusnet_statuses_get_avatar(char *id, char *url) {
	ed_twitter_statuses_get_avatar(id, url);
}

// {"id":"8",
//  "url":"http:\/\/identi.ca\/group\/8\/id",
//  "nickname":"ubuntu",
//  "fullname":"Ubuntu users",
//  "member":true,
//  "blocked":false,
//  "member_count":7994,
//  "original_logo":"http:\/\/avatar.identi.ca\/8-120-20090222150102.png",
//  "homepage_logo":"http:\/\/avatar.identi.ca\/8-96-20090222150102.png",
//  "stream_logo":"http:\/\/avatar.identi.ca\/8-48-20090222150102.png",
//  "mini_logo":"http:\/\/avatar.identi.ca\/8-24-20090222150102.png",
//  "homepage":"http:\/\/www.ubuntu.com\/",
//  "description":"A view in to the lives of Ubuntu users",
//  "location":"Everywhere",
//  "created":"Wed Aug 05 17:58:45 +0000 2009",
//  "modified":"Wed Aug 05 13:58:45 +0000 2009"
// }

void json_group_show(GroupProfile *group, char *stream) {
	cJSON *json_stream, *obj;

	json_stream = cJSON_Parse(stream);
	if(debug) printf("About to parse\n%s\n", stream);

	if(!json_stream) {
		fprintf(stderr, "ERROR parsing json stream:\n%s\n", stream);
		return;
	}

	if(json_stream->type == cJSON_Object) {
		obj = cJSON_GetObjectItem(json_stream, "error");
		if(obj && obj->type == cJSON_String) {
			fprintf(stderr, "ERROR: %s\n", obj->valuestring);
			cJSON_Delete(json_stream);
			return;
		}

		obj = cJSON_GetObjectItem(json_stream, "nickname");
		if(obj && obj->type == cJSON_String) {
			if(group->name) free(group->name);
			group->name = strndup(obj->valuestring, PIPE_BUF);
		}

		obj = cJSON_GetObjectItem(json_stream, "fullname");
		if(obj && obj->type == cJSON_String)
			group->fullname = strndup(obj->valuestring, PIPE_BUF);

		obj = cJSON_GetObjectItem(json_stream, "description");
		if(obj && obj->type == cJSON_String)
			group->description = strndup(obj->valuestring, PIPE_BUF);

		obj = cJSON_GetObjectItem(json_stream, "original_logo");
		if(obj && obj->type == cJSON_String) {
			group->original_logo = strndup(obj->valuestring, PIPE_BUF);
			ed_statusnet_statuses_get_avatar(group->name, group->original_logo);
		}

		obj = cJSON_GetObjectItem(json_stream, "member");
		if(obj && (obj->type == cJSON_True || obj->type == cJSON_False))
			group->member = (Eina_Bool)obj->valueint;

		obj = cJSON_GetObjectItem(json_stream, "member_count");
		if(obj && obj->type == cJSON_Number)
			group->member_count = obj->valueint;
	} else { printf("oopsie\n"); }
	cJSON_Delete(json_stream);
}

static int ed_statusnet_group_get_handler(void *data, int argc, char **argv, char **azColName) {
	GroupGet *gg=(GroupGet*)data;
	GroupProfile *group = gg->group;
    char *screen_name=NULL, *password=NULL, *proto=NULL, *domain=NULL, *base_url=NULL;
    int port=0, id=0, res, redir=3;
	http_request * request=calloc(1, sizeof(http_request));
	cJSON *json_stream, *obj;
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

	res = asprintf(&request->url, "%s://%s:%d%s/statusnet/groups/show.json?id=%s", proto, domain, port, base_url, group->name);

	if(res != -1) {
		if (debug) printf("gnome-open %s\n", request->url);

		res = ed_curl_get(screen_name, password, request, gg->account_id);
		while(request->redir_url && redir--) {
			free(request->url);
			request->url = request->redir_url;
			request->redir_url = NULL;
			free(request->content.memory);
			request->content.size = 0;
			request->content.memory = NULL;
			res = ed_curl_get(screen_name, password, request, gg->account_id);
		}
		
		if((res == 0) && (request->response_code == 200))
			json_group_show(group, request->content.memory);
		else {
			group->failed = EINA_TRUE;
			json_stream = cJSON_Parse(request->content.memory);
			if(json_stream->type == cJSON_Object) {
				obj = cJSON_GetObjectItem(json_stream, "error");
				if(obj) {
					group->error = strdup(obj->valuestring);
					cJSON_Delete(obj);
				}
			}
		}
	}

	if(request->url) free(request->url);
	if(request->content.memory) free(request->content.memory);
	free(request);

	return(0);
}

void ed_statusnet_group_get(int account_id, GroupProfile *group) {
	char *query=NULL, *db_err=NULL;
	int sqlite_res;
	GroupGet gg;
	
	gg.group=group;
	gg.account_id=account_id;
	sqlite_res = asprintf(&query, "SELECT name,password,type,proto,domain,port,base_url,id FROM accounts WHERE id = %d and type = %d and enabled = 1;", account_id, ACCOUNT_TYPE_TWITTER);

	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, ed_statusnet_group_get_handler, (void*)&gg, &db_err);
		if(sqlite_res != 0) {
			fprintf(stderr, "Can't run %s: %d = %s\n", query, sqlite_res, db_err);
			sqlite3_free(db_err);
		}
		free(query);
	}
}

void ed_statusnet_group_free(GroupProfile *group) {
    if(group) {
        if(group->name) free(group->name);
        if(group->fullname) free(group->fullname);
        if(group->description) free(group->description);
        if(group->original_logo) free(group->original_logo);
		if(group->error) free(group->error);

        free(group);
    }
}

static int ed_statusnet_group_join_handler(void *data, int argc, char **argv, char **azColName) {
	GroupGet *gg=(GroupGet*)data;
	GroupProfile *group = gg->group;
    char *screen_name=NULL, *password=NULL, *proto=NULL, *domain=NULL, *base_url=NULL;
    int port=0, id=0, res;
	http_request * request=calloc(1, sizeof(http_request));
	cJSON *json_stream, *obj;
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

	res = asprintf(&request->url, "%s://%s:%d%s/statusnet/groups/join.json?id=%s", proto, domain, port, base_url, group->name);

	if(res != -1) {
		if (debug) printf("gnome-open %s\n", request->url);

		res = ed_curl_post(screen_name, password, request, "",  gg->account_id);
		if((res == 0) && (request->response_code == 200))
			json_group_show(group, request->content.memory);
		else {
			group->failed = EINA_TRUE;
			json_stream = cJSON_Parse(request->content.memory);
			if(json_stream->type == cJSON_Object) {
				obj = cJSON_GetObjectItem(json_stream, "error");
				if(obj) {
					group->error = strdup(obj->valuestring);
					cJSON_Delete(obj);
				}
			}
		}
	}

	if(request->url) free(request->url);
	if(request->content.memory) free(request->content.memory);
	free(request);

	return(0);
}

void ed_statusnet_group_join(int account_id, GroupProfile *group) {
	char *query=NULL, *db_err=NULL;
	int sqlite_res;
	GroupGet gg;
	
	gg.group=group;
	gg.account_id=account_id;

	sqlite_res = asprintf(&query, "SELECT name,password,type,proto,domain,port,base_url,id FROM accounts WHERE id = %d and type = %d and enabled = 1;", account_id, ACCOUNT_TYPE_TWITTER);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, ed_statusnet_group_join_handler, (void*)&gg, &db_err);
		if(sqlite_res != 0) {
			fprintf(stderr, "Can't run %s: %d = %s\n", query, sqlite_res, db_err);
			sqlite3_free(db_err);
		}
		free(query);
	}
}

static int ed_statusnet_group_leave_handler(void *data, int argc, char **argv, char **azColName) {
	GroupGet *gg=(GroupGet*)data;
	GroupProfile *group = gg->group;
    char *screen_name=NULL, *password=NULL, *proto=NULL, *domain=NULL, *base_url=NULL;
    int port=0, id=0, res;
	http_request * request=calloc(1, sizeof(http_request));
	cJSON *json_stream, *obj;
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

	res = asprintf(&request->url, "%s://%s:%d%s/statusnet/groups/leave.json?id=%s", proto, domain, port, base_url, group->name);

	if(res != -1) {
		if (debug) printf("gnome-open %s\n", request->url);

		res = ed_curl_post(screen_name, password, request, "",  gg->account_id);
		if((res == 0) && (request->response_code == 200))
			json_group_show(group, request->content.memory);
		else {
			group->failed = EINA_TRUE;
			json_stream = cJSON_Parse(request->content.memory);
			if(json_stream->type == cJSON_Object) {
				obj = cJSON_GetObjectItem(json_stream, "error");
				if(obj) {
					group->error = strdup(obj->valuestring);
					cJSON_Delete(obj);
				}
			}
		}
	}

	if(request->url) free(request->url);
	if(request->content.memory) free(request->content.memory);
	free(request);

	return(0);
}

void ed_statusnet_group_leave(int account_id, GroupProfile *group) {
	char *query=NULL, *db_err=NULL;
	int sqlite_res;
	GroupGet gg;
	
	gg.group=group;
	gg.account_id=account_id;

	sqlite_res = asprintf(&query, "SELECT name,password,type,proto,domain,port,base_url,id FROM accounts WHERE id = %d and type = %d and enabled = 1;", account_id, ACCOUNT_TYPE_TWITTER);
	if(sqlite_res != -1) {
		sqlite_res = sqlite3_exec(ed_DB, query, ed_statusnet_group_leave_handler, (void*)&gg, &db_err);
		if(sqlite_res != 0) {
			fprintf(stderr, "Can't run %s: %d = %s\n", query, sqlite_res, db_err);
			sqlite3_free(db_err);
		}
		free(query);
	}
}
