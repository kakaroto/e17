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

#ifndef ELM_DENTICA_CURL_H
# define ELM_DENTICA_CURL_H

#include <curl/curl.h>

typedef struct _MemoryStruct {
        char    *memory;
        size_t  size;
} MemoryStruct;

typedef struct _http_request {
	char			*url;
	MemoryStruct	content;
	long			response_code;
	char			*redir_url;
} http_request;

int ed_curl_get(char *screen_name, char *password, http_request * request, int account_id);

char *ed_curl_escape(char *unescaped);

int ed_curl_post(char *screen_name, char *password, http_request * request, char * post_fields, int account_id);

void ed_curl_cleanup(CURL * ua);

void ed_curl_ua_cleanup(int account_id);

#endif
