#ifndef _ENTRANCE_CONFIG
#define _ENTRANCE_CONFIG

#include "../config.h"
#include<Edb.h>
#include<Evas.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<limits.h>

struct _Entrance_Config
{
   char *welcome, *passwd, *greeting;

   int passwd_echo;
   struct
   {
      int w, h;
   }
   screens, display;            /* screens are the number of display */
   /* display is the screen for the "center" */
   Evas_List users;
   Evas_List *sessions;
   int default_index;
   void *default_session;
   char *theme;
};

typedef struct _Entrance_Config *Entrance_Config;

Entrance_Config entrance_config_parse(char *file);
void entrance_config_print(Entrance_Config e);
void entrance_config_free(Entrance_Config e);
void entrance_config_prepend_recent_user(Entrance_Config e, char *str);

#endif
