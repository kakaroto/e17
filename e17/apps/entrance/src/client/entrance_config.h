#ifndef _ENTRANCE_CONFIG
#define _ENTRANCE_CONFIG

#include "../config.h"
#include <Edb.h>
#include <Evas.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>

#define ENTRANCE_USE_PAM 1
#define ENTRANCE_USE_SHADOW 2

struct _Entrance_Config
{
   /**
    * theme - the theme we want entrance ui to use
    */
   char *theme;
   /**
    * pointer - path to the file on disk we want to use as the pointer
    */
   char *pointer;
   
   /**
    * before - the string before the hostname in "EntranceHostname"
    * after - the string to append after hostname in "EntranceHostname"
    * date - the date format fed to strftime for "EntranceDate"
    * time - the time format fed to strftime for "EntranceTime"
    */
   struct {
       char *string;
   } before, after, date, time;

   /**
    * don't know how applicable these are anymore, since layout is left
    * up to the edje the variables are currently unused
    */
   struct {
      int w, h;
   } screens, display;            

   /**
    * ENTRANCE_USE_PAM - if set to use pam authentication
    * ENTRANCE_USE_SHADOW- if set to use shadow auth if pam is not avail
    */
   int auth;

   struct {
       int allow;
   } reboot, halt;

   /**
    * fonts - available font paths for fonts
    */
   Evas_List *fonts;
   /**
    * users - users known to the system
    */
   Evas_List *users;
   /**
    * sessions - a themable string to xsession string hash
    */
   Evas_Hash *sessions;
};

typedef struct _Entrance_Config *Entrance_Config;

#if 0
struct _Entrance_User
{
   char *name;
   char *img;
   int sys;
   // void *something
};

typedef struct _Entrance_User Entrance_User;
#endif

Entrance_Config entrance_config_parse(char *file);
void entrance_config_print(Entrance_Config e);
void entrance_config_free(Entrance_Config e);
void entrance_config_prepend_recent_user(Entrance_Config e, char *str);

#endif
