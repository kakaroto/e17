#ifndef _ELOGIN_SESSION
#define _ELOGIN_SESSION

#include<Ebg.h>
#include<Evas.h>
#include<Ecore.h>
#include<stdio.h>
#include<limits.h>
#include<string.h>

#include "e_login_auth.h"
#include "e_login_config.h"

#define X_TESTING 0             /* set to 1 if you wanna run inside a window
                                   in X */

struct _E_Login_Session
{
   Evas *evas;                  /* the evas being used in the X session */
   Evas_Object *pointer;        /* object for the pointer in the evas */
   E_Background bg;             /* ebg in the evas */
   E_Login_Auth auth;           /* encapsulated auth shit */
   E_Login_Config config;       /* configuration options */
   Window main_win, ewin;       /* window ids */

   struct
   {
      int w, h;
   }
   geom;                        /* the height/width of the evas */

   Evas_List *users;            /* recent users */
   /* find a way to have plugins running */
};
typedef struct _E_Login_Session *E_Login_Session;

E_Login_Session e_login_session_new(void);
void e_login_session_free(E_Login_Session e);

void e_login_session_init(E_Login_Session e);

int e_login_session_auth_user(E_Login_Session e);
void e_login_session_reset_user(E_Login_Session e);

#endif
