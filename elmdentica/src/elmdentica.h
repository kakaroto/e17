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


#ifndef ELMDENTICA_H

#define ELMDENTICA_H

#include <Elementary.h>
#include <Ecore_X.h>

#include <statusnet.h>

typedef struct anchor_data {
	Evas_Object *bubble;
	char        *url;
} AnchorData;

typedef struct gag_data {
	aStatus		*as;
	Eina_Bool	match;
} GagData;

typedef struct _StatusesList {
	Eina_List	*list;
	char		*hash_error;
	char		*hash_request;
} StatusesList;

typedef struct _Gui {
	Evas_Object *win, *timeline, *pager, *main, *status_detail, *status_detail_users, *status_detail_links, *status_detail_tags, *status_detail_groups, *hover,
			    *download_win, *fileselector_win, *status_detail_attachments, *edje;
	Elm_Toolbar_Item *timelines, *post;
	Evas *win_evas;
	Ecore_File_Progress_Return downloadstatus;
	Ecore_File_Download_Job* current_download;
	Ecore_Timer *hide_tb_timer;
} Gui;

typedef union _AccountTypes {
	StatusNetBaAccount *snba;
} AccountTypes;

typedef struct _Account {
	short sqlid;
	short type;
	AccountTypes details;
	Eina_Bool send;
	Eina_Bool receive;
	Eina_Bool disabled;
} Account;

#define ACCOUNT_TYPE_NONE 0
#define ACCOUNT_TYPE_STATUSNET 1

#define TIMELINE_FRIENDS 0
#define TIMELINE_USER 1
#define TIMELINE_PUBLIC 2
#define TIMELINE_FAVORITES 3
#define TIMELINE_MENTIONS 4
#define TIMELINE_DMSGS 5

#define BROWSER_XDG		0
#define	BROWSER_VENTURA	1
#define	BROWSER_MIDORI	2
#define	BROWSER_WOOSH	3
#define	BROWSER_DILLO	4
#define	BROWSER_EVE		5

void error_win_del(void *data, Evas_Object *zbr, void *event_info);

void set_urls(void);

void fill_message_list(int timeline, Eina_Bool fromdb);

Evas_Object *ed_make_bubble(Evas_Object *parent, aStatus *as, anUser *au);
Evas_Object *ed_make_message(char *text, Evas_Object *bubble, Evas_Object *window);

#endif
