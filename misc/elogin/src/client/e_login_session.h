#ifndef _ELOGIN_SESSION
#define _ELOGIN_SESSION

#include<Ebg.h>
#include<Evas.h>
#include<Ebits.h>
#include<Ecore.h>
#include<stdio.h>
#include<limits.h>
#include<string.h>

#include "e_login_auth.h"

struct _E_Login_Session
{
   Evas evas;
   E_Background bg;
   E_Login_Auth auth;
   Window main_win, ewin;

   struct
   {
      int w, h;
   }
   geom;

   Evas_List plugins;
   /* dlopen("foo.so"); */
};

typedef struct _E_Login_Session *E_Login_Session;

E_Login_Session e_login_session_new();
void e_login_session_free(E_Login_Session e);

void e_login_session_init(E_Login_Session e);

int e_login_session_auth_user(E_Login_Session e);
void e_login_session_reset_user(E_Login_Session e);

#endif
