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


#ifndef ELMDENTICA_H

#define ELMDENTICA_H

typedef enum _TimeLineStates {
	HASH,
	HASH_ERROR,
	HASH_REQUEST,
	FT_NULL,
	FT_START,
	FT_STATUS,
	FT_ID,
	FT_USER,
	FT_NAME,
	FT_AVATAR,
	FT_SCREEN_NAME,
	FT_TEXT,
	FT_CREATED_AT
} TimeLineStates;

typedef struct _ub_Status {
	char * screen_name;	// <screen_name>
	char * name;		// <name>
	char * text;		// <text>
	char * created_at_str;	// <created_at> as a string
	time_t created_at;	// <created_at>
	long long int    id;		// <id>
	char * id_str;		// <id> as string
} ub_Status;

typedef struct _ub_bubble {
	Evas_Object *box;
	char *screen_name;
	char *message;
	int   account_id;
} ub_Bubble;

typedef struct _StatusesList {
	GList 		*list;
	ub_Status 	*current;
	TimeLineStates	state;
	char		*hash_error;
	char		*hash_request;
} StatusesList;

#define ACCOUNT_TYPE_NONE 0
#define ACCOUNT_TYPE_STATUSNET 1
#define ACCOUNT_TYPE_TWITTER 2

void error_win_del(void *data, Evas_Object *zbr, void *event_info);

void set_urls(void);

void fill_message_list(void);

#endif
