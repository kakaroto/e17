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
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glib.h>
#include <glib/gprintf.h>

#include <Elementary.h>
#include <Ecore_X.h>

#include "curl.h"
#include "elmdentica.h"
#include "gettext.h"
#define _(string) gettext (string)

extern Evas_Object *error_win;
extern Gui gui;

CURL * user_agent = NULL;
CURL * user_agent_images = NULL;
char error_buf[2048];

extern char * url_post;
extern char * url_friends;
extern int debug;

Eina_Hash *user_agents=NULL;


void user_agent_free(void *data) {
	curl_easy_cleanup((CURL*)data);
}

void show_curl_error(CURLcode curl_res, MemoryStruct * chunk) {
	Evas_Object *box=NULL, *frame=NULL, *label=NULL, *button=NULL;
	int res=0;
	char *buf=NULL;

	/* Error Window */
	error_win = elm_win_inwin_add(gui.win);

		/* Vertical Box */
		box = elm_box_add(gui.win);
			evas_object_size_hint_weight_set(box, 1, 1);
			evas_object_size_hint_align_set(box, -1, -1);
			evas_object_show(box);
	
			/* Frame (with message) */
			frame = elm_frame_add(gui.win);
				elm_object_text_set(frame, chunk->memory);
				res = asprintf(&buf, "%d: %s", curl_res, error_buf);
				if(res != -1) {
					label = elm_label_add(gui.win);
						elm_label_line_wrap_set(label, TRUE);
						elm_object_text_set(label, buf);
						elm_frame_content_set(frame, label);
					evas_object_show(label);
				}
				if(buf) free(buf);
				elm_box_pack_end(box, frame);
			evas_object_show(frame);

			/* close button */
			button = elm_button_add(gui.win);
				evas_object_smart_callback_add(button, "clicked", error_win_del, NULL);
				elm_object_text_set(button, _("Close"));
				elm_box_pack_end(box, button);
			evas_object_show(button);

		evas_object_show(box);

		elm_win_inwin_content_set(error_win, box);
	evas_object_show(error_win);
}

size_t write_data(void *ptr, size_t size, size_t nmemb, void *userp) {
	MemoryStruct *mem = (MemoryStruct *)userp;
	char *newMem = NULL;
	size_t realsize = size*nmemb;

	newMem = calloc(sizeof(char), mem->size + realsize + 1);

	if (newMem) {
		memcpy(newMem, mem->memory, mem->size);
		memcpy(&(newMem[mem->size]), ptr, realsize);
		free(mem->memory);
		mem->memory=newMem;
		mem->size += realsize;
		mem->memory[mem->size] = 0;
		return(realsize);
	}
	return(0);
}


char *ed_curl_escape(char *unescaped) {
	CURL *ua=NULL;
	char *escaped=NULL;

	ua = curl_easy_init();

	escaped = curl_easy_escape(ua, unescaped, 0);

	curl_easy_cleanup(ua);

	return escaped;
}

CURL * ed_curl_init(char *screen_name, char *password, http_request * request, int account_id) {
	CURL *ua=NULL;
	int res = 0;
	char *key=NULL;

	ua = curl_easy_init();

	if(ua) {
		if(debug > 3)
			curl_easy_setopt(ua, CURLOPT_VERBOSE,		1				);
		else
			curl_easy_setopt(ua, CURLOPT_VERBOSE,		0				);

		curl_easy_setopt(ua, CURLOPT_WRITEFUNCTION,     write_data      );
		curl_easy_setopt(ua, CURLOPT_ENCODING,          ""              );

		if(account_id >= 0 && screen_name && password) {
			curl_easy_setopt(ua, CURLOPT_HTTPAUTH,   CURLAUTH_BASIC      );
			curl_easy_setopt(ua, CURLOPT_USERNAME,   screen_name         );
			curl_easy_setopt(ua, CURLOPT_PASSWORD,   password            );
		}

		res = asprintf(&key, "%d", account_id);
		if(res!=-1)
			eina_hash_add(user_agents, key, (void*)ua);
	}

	return(ua);
}

gint ed_curl_get(char *screen_name, char *password, http_request * request, int account_id) {
	CURLcode res;
	CURL *ua=NULL;
	char *key=NULL, *redir_url=NULL;

	if(!request)
		return 2;

	if(!request->url || strlen(request->url) <= 0)
		return 3;

	if(!user_agents) user_agents = eina_hash_string_superfast_new(user_agent_free);

	res = asprintf(&key, "%d", account_id);
	if(res!=-1) {
		ua = (CURL*)eina_hash_find(user_agents, key);
		free(key);
	} else return(6);

	if(!ua) {
		if(debug > 3) printf("No handler, baking a new one\n");
		ua = ed_curl_init(screen_name, password, request, account_id);
		if(!ua) return(6);

		curl_easy_setopt(ua, CURLOPT_HTTPGET,	1L				);
		curl_easy_setopt(ua, CURLOPT_POST,		0L				);
	} else
		if(debug > 3) printf("Already have a handler\n");

	if(debug > 3) printf("Prepping URL\n");
	curl_easy_setopt(ua, CURLOPT_URL,		request->url	);
	if(debug > 3) printf("Prepping content destination\n");
	curl_easy_setopt(ua, CURLOPT_WRITEDATA, (void *)&(request->content)	);

	if(debug > 3) printf("Fetching URL %s...\n", request->url);
	res = curl_easy_perform(ua);

	if(res == 0) {
		res = curl_easy_getinfo(ua, CURLINFO_RESPONSE_CODE, &request->response_code);
		if(CURLE_OK == curl_easy_getinfo(ua, CURLINFO_REDIRECT_URL, &redir_url) && redir_url)
			request->redir_url = strdup(redir_url);
		if(debug > 3) printf("Response code: %ld\n", request->response_code);
		return(request->content.size>0?0:-1);
	}

	return(-1);
}

gint ed_curl_post(char *screen_name, char *password, http_request * request, char * post_fields, int account_id) {
	CURLcode res;
	CURL *ua=NULL;
	double content_length=0;
	char *key=NULL;

	if(!request)
		return 2;

	if(!request->url || strlen(request->url) <= 0)
		return 3;

	if(!user_agents) user_agents = eina_hash_int32_new(user_agent_free);

	res = asprintf(&key, "%d", account_id);
	if(res!=-1) {
		ua = (CURL*)eina_hash_find(user_agents, key);
		free(key);
	} else return(6);

	if(!ua) {
		ua = ed_curl_init(screen_name, password, request, account_id);
		if(!ua) return(6);

		curl_easy_setopt(ua, CURLOPT_HTTPGET,	0L				);
		curl_easy_setopt(ua, CURLOPT_POST,		1L				);
	}

	curl_easy_setopt(ua, CURLOPT_URL,		request->url	);
	curl_easy_setopt(ua, CURLOPT_WRITEDATA, (void *)&(request->content)	);

	if(post_fields)
		curl_easy_setopt(ua, CURLOPT_POSTFIELDS,	post_fields			);

	res = curl_easy_perform(ua);

	curl_easy_getinfo(ua, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &content_length);
	curl_easy_getinfo(ua, CURLINFO_REDIRECT_URL, &request->redir_url);
	if((res == 18 && content_length != -1) || res != 0) {
		show_curl_error(res, &(request->content));
		return(4);
	} else {
		res = curl_easy_getinfo(ua, CURLINFO_RESPONSE_CODE, &request->response_code);
		return(0);
	}
}

void ed_curl_cleanup(CURL * ua) {
	curl_easy_cleanup(ua);
}

void ed_curl_ua_cleanup(int account_id) {
	char *key = NULL;
	int res = 0;
	CURL *ua;

	if(!user_agents) return;

	res = asprintf(&key, "%d", account_id);
	if(res != -1) {
		ua = (CURL*)eina_hash_find(user_agents, key);
		if(ua) {
			eina_hash_del(user_agents, key, ed_curl_cleanup);
		}
		free(key);
	}
}

size_t dump_data(void *ptr, size_t size, size_t nmemb, void *userp) {
	int file = *(int *)userp;
	size_t written = 0, realsize = size*nmemb;

	if (realsize == (written = write(file, ptr, realsize))) return(written);
	else return(realsize - written);
}

long int ed_curl_dump_url_to_file(char *url, char *file_path) {
	int file=0;
	long int ret_val=0;
	CURL *ua;
	CURLcode res;

	ua = curl_easy_init();

	if(ua) {
		file = open(file_path, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR|S_IWUSR);
		if(file != -1) {
			if(debug > 3) {
				printf("open(%s) == %d\n", file_path, file);
				curl_easy_setopt(ua, CURLOPT_VERBOSE,		1				);
			} else
				curl_easy_setopt(ua, CURLOPT_VERBOSE,		0				);

			curl_easy_setopt(ua, CURLOPT_WRITEFUNCTION,     dump_data       );
			curl_easy_setopt(ua, CURLOPT_ENCODING,          ""              );
			curl_easy_setopt(ua, CURLOPT_URL,				url				);

			curl_easy_setopt(ua, CURLOPT_WRITEDATA, (void *)&file			);

			res = curl_easy_perform(ua);
			if(res == CURLE_OK) {
				curl_easy_getinfo(ua, CURLINFO_RESPONSE_CODE, &ret_val);
				return(ret_val);
			} else {
				curl_easy_cleanup(ua);
				close(file);
				fprintf(stderr, _("Can't dump %s into %s:\n(%d) %s\n"), url, file_path, res, curl_easy_strerror(res));
				return(-3);
			}

			close(file);
		} else {
			curl_easy_cleanup(ua);
			fprintf(stderr, _("Can't open %s for writing: %s\n"),file_path, strerror(errno));
			return(-2);
		}
	} else return(-1);
}
