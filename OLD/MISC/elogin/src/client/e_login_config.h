#ifndef _ELOGIN_CONFIG
#define _ELOGIN_CONFIG

#include "../config.h"
#include<Edb.h>
#include<Evas.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<limits.h>

struct _E_Login_Config
{
   char *bg;

   struct
   {
      struct
      {
         int size;
         int r, g, b, a;
         char *name;
      }
      font;
      struct
      {
         float x, y;
         int offset_x, offset_y;
      }
      pos;
      char *mess;
   }
   welcome, passwd, greeting;

   struct
   {
      int w, h;
   }
   screens, display;            /* screens are the number of display */
   /* display is the screen for the "center" */
   struct
   {
      float h, v;
   }
   place;
   Evas_List *users;
   Evas_List *sessions;
};
typedef struct _E_Login_Config *E_Login_Config;

E_Login_Config e_login_config_parse(char *file);
void e_login_config_print(E_Login_Config e);
void e_login_config_free(E_Login_Config e);
void e_login_config_prepend_recent_user(E_Login_Config e, char *str);

#endif
