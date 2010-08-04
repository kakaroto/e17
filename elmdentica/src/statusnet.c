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

#include "statusnet.h"

#include "gettext.h"
#define _(string) gettext (string)

#include <config.h>
#include "curl.h"
#include "elmdentica.h"

extern struct sqlite3 *ed_DB;
extern int debug;
extern char *dm_to, *reply_id;
extern Evas_Object *win;

extern char * avatar;

int ed_statusnet_post(int id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, char *msg) {
	return(0);
}
void ed_statusnet_timeline_get(int id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, int timeline) {
}

void ed_statusnet_statuses_get_avatar(char *screen_name) {
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
	json_object *json_stream, *obj;
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

		obj = json_object_object_get(json_stream, "fullname");
		if(obj) {
			group->fullname = strndup(json_object_get_string(obj), PIPE_BUF);
			json_object_put(obj);
		}

		obj = json_object_object_get(json_stream, "description");
		if(obj) {
			group->description = strndup(json_object_get_string(obj), PIPE_BUF);
			json_object_put(obj);
		}

		obj = json_object_object_get(json_stream, "original_logo");
		if(obj) {
			avatar = strndup(json_object_get_string(obj), PIPE_BUF);
			ed_statusnet_statuses_get_avatar(group->name);
			json_object_put(obj);
		}

		obj = json_object_object_get(json_stream, "member");
		if(obj) {
			group->member = (Eina_Bool)json_object_get_boolean(obj);
			json_object_put(obj);
		}

		obj = json_object_object_get(json_stream, "member_count");
		if(obj) {
			group->member_count = (int)json_object_get_int(obj);
			json_object_put(obj);
		}
	}
	json_object_put(json_stream);
}

static int ed_statusnet_group_get_handler(void *data, int argc, char **argv, char **azColName) {
	GroupGet *gg=(GroupGet*)data;
	GroupProfile *group = gg->group;
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

	res = asprintf(&request->url, "%s://%s:%d%s/statusnet/groups/show.json?id=%s", proto, domain, port, base_url, group->name);

	if(res != -1) {
		if (debug) printf("gnome-open %s\n", request->url);

		res = ed_curl_get(screen_name, password, request, gg->account_id);
		if((res == 0) && (request->response_code == 200))
			json_group_show(group, request->content.memory);

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

        free(group);
    }
}
