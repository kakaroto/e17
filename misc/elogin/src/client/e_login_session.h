#ifndef _ELOGIN_SESSION
#define _ELOGIN_SESSION

#include<Ebg.h>
#include<Evas.h>
#include<Ecore.h>
#include<stdio.h>
#include<limits.h>
#include<string.h>
#include<unistd.h>

#include "e_login_auth.h"
#include "e_login_config.h"

struct _E_Login_Session_Type
{
   char *name;
   char *path;
};

typedef struct _E_Login_Session_Type E_Login_Session_Type;

struct _E_Login_Session
{
   Evas *evas;                  /* the evas being used in the X session */
   Evas_Object *pointer;        /* object for the pointer in the evas */
   Evas_Object *listbg;         /* Background for session list */
   Evas_Object *listhead;       /* Session List heading */
   Evas_Object *bullet;
   Evas_List *listitems;        /* Items in session list */
   E_Background bg;             /* ebg in the evas */
   E_Login_Auth auth;           /* encapsulated auth shit */
   E_Login_Config config;       /* configuration options */
   Window main_win, ewin;       /* window ids */

   struct
   {
      int w, h;
   }
   geom;                        /* the height/width of the evas */

   E_Login_Session_Type *session;	/* The 
	   session 
	   to 
	   load 
	 */
   int session_index;

   Evas_List *users;            /* recent users */
   /* find a way to have plugins running */
};
typedef struct _E_Login_Session *E_Login_Session;

E_Login_Session e_login_session_new(void);
void e_login_session_free(E_Login_Session e);

void e_login_session_init(E_Login_Session e);

int e_login_session_auth_user(E_Login_Session e);
void e_login_session_reset_user(E_Login_Session e);
void e_login_session_select_session(E_Login_Session e,
                                    E_Login_Session_Type * t);

#endif
