#ifndef _ENTRANCE_SESSION_H
#define _ENTRANCE_SESSION_H

#include<Edje.h>
#include<Evas.h>
#include<Ecore.h>
#include<Ecore_X.h>
#include<Ecore_Evas.h>
#include<stdio.h>
#include<limits.h>
#include<string.h>
#include<unistd.h>
#include<syslog.h>

#include "entrance_auth.h"
#include "entrance_config.h"

struct _Entrance_Session
{
   char *theme;                 /* Theme eet */
   char *session;               /* Theme eet */
   Ecore_Evas *ee;              /* the ecore_evas */
   Evas_Object *edje;           /* main theme edje */
   Entrance_Auth auth;          /* encapsulated auth shit */
   Entrance_Config config;      /* configuration options */

   Evas_List *EntUsers;
   int authed;
};

typedef struct _Entrance_Session *Entrance_Session;

Entrance_Session entrance_session_new(void);
void entrance_session_ecore_evas_set(Entrance_Session e, Ecore_Evas * ee);
void entrance_session_free(Entrance_Session e);
void entrance_session_run(Entrance_Session e);
int entrance_session_auth_user(Entrance_Session e);
void entrance_session_reset_user(Entrance_Session e);
void entrance_session_start_user_session(Entrance_Session e);
void entrance_session_xsession_set(Entrance_Session e, char *xsession);
void entrance_session_edje_object_set(Entrance_Session e, Evas_Object * obj);
void entrance_session_list_add(Entrance_Session e);

#endif
