#ifndef ENTRANCE_USER_H
#define ENTRANCE_USER_H

#include <Evas.h>

typedef struct _Entrance_User Entrance_User;

struct _Entrance_User
{
   char *name;
   char *icon;
   char *session;
   int autologin;
};

Entrance_User *entrance_user_new(char *user, char *icon, char *session);
void entrance_user_auto_login_set(Entrance_User * e, int allow);
void entrance_user_free(Entrance_User * e);
Evas_Object *entrance_user_edje_get(Entrance_User * e, Evas_Object * edje);

#endif
