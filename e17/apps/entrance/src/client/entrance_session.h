#ifndef _ENTRANCE_SESSION_H
#define _ENTRANCE_SESSION_H

#include<Ebg.h>
#include<Evas.h>
#include<Ecore.h>
#include<stdio.h>
#include<limits.h>
#include<string.h>
#include<unistd.h>

#include "entrance_auth.h"
#include "entrance_config.h"
#include "entrance_theme.h"

struct _Entrance_Session_Type
{
	char *name;
	char *path;
	char *icon;
};

typedef struct _Entrance_Session_Type Entrance_Session_Type;

struct _Entrance_Session
{
   Evas *evas;                  /* the evas being used in the X session */
   Evas_Object *pointer;        /* object for the pointer in the evas */
   Evas_Object *listbox;        /* Background for session list */
   Evas_List *listitems;        /* Items in session list */
   Evas_Object *sessiontext;
   Evas_Object *sessionicon;
   Evas_Object *face, *face_shadow;
   Evas_Object *entrybox;
   
   Evas_Object *date;
   Evas_Object *time;
   Evas_Object *hostname;
   
   E_Background bg;             /* ebg in the evas */
   Entrance_Auth auth;           /* encapsulated auth shit */
   Entrance_Config config;       /* configuration options */
   Entrance_Theme theme;		 /* Theme data */
   Window main_win, ewin;       /* window ids */

   struct
   {
      int w, h;
   }
   geom;                        /* the height/width of the evas */

   Entrance_Session_Type *session;  /* The session to load */
   int session_index;

   Evas_List *users;            /* recent users */
   /* find a way to have plugins running */
};

typedef struct _Entrance_Session *Entrance_Session;

Entrance_Session entrance_session_new(void);
void entrance_session_free(Entrance_Session e);
void entrance_session_init(Entrance_Session e);
int entrance_session_auth_user(Entrance_Session e);
void entrance_session_reset_user(Entrance_Session e);
#endif
