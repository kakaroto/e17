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

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <glib.h>
#include <glib/gprintf.h>
#include <gconf/gconf-client.h>

#include <Elementary.h>
#include <Ecore_X.h>

#include "curl.h"
#include "elmdentica.h"
#include "gettext.h"
#define _(string) gettext (string)

#include <config.h>

extern Evas_Object *error_win, *win;

CURL * user_agent = NULL;
CURL * user_agent_images = NULL;
char error_buf[2048];

char * userpwd=NULL;
extern char * url_post;
extern char * url_friends;
extern int debug;
extern GConfClient *conf_client;

void show_http_error(long response_code, char * method, char * url) {
	Evas_Object *box=NULL, *frame1=NULL, *label=NULL, *button=NULL;
	int res=0;
	char *buf=calloc(2100, sizeof(char));

	/* Error Window */
	error_win = elm_win_inwin_add(win);

		/* Vertical Box */
		box = elm_box_add(error_win);
		evas_object_size_hint_weight_set(box, 1, 1);
		evas_object_size_hint_align_set(box, -1, -1);
	
			/* First frame (with message) */
			frame1 = elm_frame_add(win);
				if(buf) {
					res = asprintf(&buf, _("%s got a %ld HTTP Response..."), method, response_code);
					if(res != -1) {
						elm_frame_label_set(frame1, buf);
					}
					free(buf);
				}
				label = elm_label_add(win);
					elm_label_line_wrap_set(label, TRUE);
					elm_label_label_set(label, url);
					elm_frame_content_set(frame1, label);
				evas_object_show(label);
				elm_box_pack_end(box, frame1);
			evas_object_show(frame1);

			/* close button */
			button = elm_button_add(win);
				evas_object_smart_callback_add(button, "clicked", error_win_del, NULL);
				elm_button_label_set(button, _("Close"));
				elm_box_pack_end(box, button);
			evas_object_show(button);

		evas_object_show(box);

		elm_win_inwin_content_set(error_win, box);
	evas_object_show(error_win);
}

void show_curl_error(CURLcode curl_res, MemoryStruct * chunk) {
	Evas_Object *box=NULL, *frame=NULL, *label=NULL, *button=NULL;
	int res=0;
	char *buf=calloc(2100, sizeof(char));

	/* Error Window */
	error_win = elm_win_inwin_add(win);

		/* Vertical Box */
		box = elm_box_add(win);
			evas_object_size_hint_weight_set(box, 1, 1);
			evas_object_size_hint_align_set(box, -1, -1);
			evas_object_show(box);
	
			/* Frame (with message) */
			frame = elm_frame_add(win);
				elm_frame_label_set(frame, chunk->memory);
				res = asprintf(&buf, "%d: %s", curl_res, error_buf);
				if(res != -1) {
					label = elm_label_add(win);
						elm_label_line_wrap_set(label, TRUE);
						elm_label_label_set(label, buf);
						elm_frame_content_set(frame, label);
					evas_object_show(label);
				}
				if(buf) free(buf);
				elm_box_pack_end(box, frame);
			evas_object_show(frame);

			/* close button */
			button = elm_button_add(win);
				evas_object_smart_callback_add(button, "clicked", error_win_del, NULL);
				elm_button_label_set(button, _("Close"));
				elm_box_pack_end(box, button);
			evas_object_show(button);

		evas_object_show(box);

		elm_win_inwin_content_set(error_win, box);
	evas_object_show(error_win);
}

static void *my_realloc(void *ptr, size_t size) {
        if(ptr)
                return realloc(ptr, size);
        else
                return malloc(size);
}

size_t write_data(void *ptr, size_t size, size_t nmemb, void *userp) {
	MemoryStruct *mem = (MemoryStruct *)userp;
	size_t realsize = size*nmemb;

	mem->memory = my_realloc(mem->memory, mem->size + realsize + 1);

	if (mem->memory) {
		memcpy(&(mem->memory[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->memory[mem->size] = 0;
	}

	return(realsize);
}


char *ed_curl_escape(char *unescaped) {
	CURL *ua=NULL;
	char *escaped=NULL;

	ua = curl_easy_init();

	escaped = curl_easy_escape(ua, unescaped, 0);

	curl_easy_cleanup(ua);

	return escaped;
}

CURL * ed_curl_init(char *screen_name, char *password, http_request * request) {
	CURL *ua=NULL;
	int res = 0;

	ua = curl_easy_init();

	if(ua) {
		if(debug)
			curl_easy_setopt(ua, CURLOPT_VERBOSE,		1				);
		else
			curl_easy_setopt(ua, CURLOPT_VERBOSE,		0				);

		curl_easy_setopt(ua, CURLOPT_URL,		request->url			);

		curl_easy_setopt(ua, CURLOPT_WRITEDATA, 	(void *)&(request->content)	);
		curl_easy_setopt(ua, CURLOPT_WRITEFUNCTION,     write_data      		);


		if(screen_name != NULL && password != NULL) {
			res = asprintf(&userpwd, "%s:%s", screen_name, password);
			if(res != -1)
				curl_easy_setopt(ua, CURLOPT_USERPWD,   userpwd         		);
		}
	}
	return(ua);
}

gint ed_curl_get(char *screen_name, char *password, http_request * request) {
	CURLcode res;
	CURL *ua=NULL;
	char *userpwd=NULL;
	long	response_code;
	double	content_length=0;

	if(request ==NULL)
		return 2;

	if(request->url == NULL || strlen(request->url) <= 0)
		return 3;

	ua = ed_curl_init(screen_name, password, request);

	if(ua) {

		curl_easy_setopt(ua, CURLOPT_HTTPGET,		1L				);
		curl_easy_setopt(ua, CURLOPT_POST,		0L				);

printf("BEFORE\n");
		res = curl_easy_perform(ua);
printf("AFTER\n");

		free(userpwd);

		curl_easy_getinfo(ua, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &content_length);
		if((res == 18 && content_length != -1) || res != 0) {
			show_curl_error(res, &(request->content));
			curl_easy_cleanup(ua);
			return(4);
		} else {
			res = curl_easy_getinfo(ua, CURLINFO_RESPONSE_CODE, &response_code);
			curl_easy_cleanup(ua);
			if(response_code != 200) {
				show_http_error(response_code, "GET", request->url);
				return(5);
			}
			return(0);
		}
	} else
		return(6);
}

gint ed_curl_post(char *screen_name, char *password, http_request * request, char * post_fields) {
	CURLcode res;
	CURL *ua=NULL;
	char *userpwd=NULL;
	long	response_code;
	double content_length=0;

	if(request ==NULL)
		return 2;

	if(request->url == NULL || strlen(request->url) <= 0)
		return 3;

	ua = ed_curl_init(screen_name, password, request);

	curl_easy_setopt(ua, CURLOPT_HTTPGET,		0L				);
	curl_easy_setopt(ua, CURLOPT_POST,		1L				);
	curl_easy_setopt(ua, CURLOPT_POSTFIELDS,	post_fields			);

	res = curl_easy_perform(ua);

	free(userpwd);

	curl_easy_getinfo(ua, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &content_length);
	if((res == 18 && content_length != -1) || res != 0) {
		show_curl_error(res, &(request->content));
		curl_easy_cleanup(ua);
		return(4);
	} else {
		res = curl_easy_getinfo(ua, CURLINFO_RESPONSE_CODE, &response_code);
		curl_easy_cleanup(ua);
		if(response_code != 200) {
			show_http_error(response_code, "POST", request->url);
			return(5);
		}
		return(0);
	}
}

void ed_curl_cleanup(CURL * ua) {
	curl_easy_cleanup(ua);
}

