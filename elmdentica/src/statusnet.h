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


#ifndef STATUSNET_H
#define STATUSNET_H

#include <Elementary.h>
#include <Ecore_X.h>
#include "statusnet_Common.h"

typedef void (*Status_List_Cb)(int, Eina_Bool);

typedef struct _StatusNetBaAccount {
	double id;
	char *screen_name;
	char *password;
	char *proto;
	char *domain;
	short port;
	char *base_url;
} StatusNetBaAccount;

typedef struct _Group_Profile {
    char  *name;
    char        *fullname;
    Eina_Bool   member;
    int         member_count;
    char        *original_logo;
    char        *description;
	Eina_Bool	failed;
	char		*error;
	int account_id;
} GroupProfile;

typedef struct _group_get {
	GroupProfile *group;
	int account_id;
} GroupGet;

typedef struct _User_Profile {
    char        *name;
    char        *screen_name;
    char        *description;
    char        *text;
    time_t      status_created_at;
    char        *tmp;
    Eina_Bool   protected;
    int         followers_count;
    int         friends_count;
    Eina_Bool   following;
} UserProfile;

typedef struct _an_User {
	statusnet_User	*user;
	time_t			created_at;
    int				account_id;
	short			account_type;
	Eina_Bool		in_db;
} anUser;

typedef struct _a_Status {
	statusnet_Status *status;
	time_t           created_at;
	anUser			*au;
	int              account_id;
	short            account_type;
	Eina_Bool        in_db;
	Elm_Genlist_Item *li;
} aStatus;

typedef void (*Group_Show_Cb)(void*);
typedef void (*User_Show_Cb)(void*);
typedef void (*Repeat_Cb)(aStatus *, void *);

typedef struct _user_get {
	UserProfile *user;
	int account_id;
	long long int id;
	anUser *au;
} UserGet;

typedef struct _group_data {
	aStatus *as;
	const char *group_name;
	Group_Show_Cb group_show;
	void *data;
	statusnet_Group *group;
	Evas_Object *win;
	Eina_Bool failed;
} groupData;

typedef struct _user_data {
	aStatus *as;
	anUser *au;
	const char *screen_name;
	User_Show_Cb user_show;
	void *data;
	Evas_Object *win;
	Eina_Bool failed;
} userData;


void statusnet_init();

void ed_statusnet_account_free(StatusNetBaAccount *account);

void ed_statusnet_group_get(aStatus *as, const char *group_name, Group_Show_Cb callback, void *data);
void ed_statusnet_group_free(groupData *gd);
void ed_statusnet_group_join(groupData *gd);
void ed_statusnet_group_leave(groupData *gd);

int ed_statusnet_post(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, char *msg);
void ed_statusnet_timeline_get(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, int timeline, Status_List_Cb update_status_list);
void ed_statusnet_favorite_create(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, long long int status_id);
void ed_statusnet_favorite_destroy(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, long long int status_id);
void ed_statusnet_init_friends(void);
void ed_statusnet_user_get(userData *ud, User_Show_Cb callback, void *data);
anUser *ed_statusnet_user_get_by_id(int account_id, long long int user_id);
void ed_statusnet_user_follow(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, char *user_screen_name);
void ed_statusnet_user_abandon(int account_id, char *screen_name, char *password, char *proto, char *domain, int port, char *base_url, char *user_screen_name);
void ed_statusnet_repeat(int account_id, aStatus *as, Repeat_Cb callback, void *data);
void ed_statusnet_related_status_get(aStatus *as, Repeat_Cb callback, void *data);

void status_hash_data_free(void *data);
void user_hash_data_free(void *data);

void ed_statusnet_statuses_get_avatar(char *id, char *url);

#endif
