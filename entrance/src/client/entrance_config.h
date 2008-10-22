#ifndef _ENTRANCE_CONFIG
#define _ENTRANCE_CONFIG
/**
 * @file entrance_config.h
 * @brief Struct Definitions and shared function declarations
 */
#include "../config.h"
#include <Evas.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>

#define ENTRANCE_USE_CRYPT 0
#define ENTRANCE_USE_PAM 1
#define ENTRANCE_USE_SHADOW 2

#define ENTRANCE_AUTOLOGIN_NONE 0
#define ENTRANCE_AUTOLOGIN_DEFAULT 1
#define ENTRANCE_AUTOLOGIN_THEME 2

#define ENTRANCE_PRESEL_NONE 0
#define ENTRANCE_PRESEL_PREV 1

/**
 * This contains all of the configuration options that the system can set
 */
struct _Entrance_Config
{
   /**
    * theme - the theme we want entrance ui to use
    */
   char *theme;
   /**
    * background - the UI background */
   char *background;

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
   struct
   {
      char *string;
   } before, after, date, time;

   /**
    * don't know how applicable these are anymore, since layout is left
    * up to the edje the variables are currently unused
    */
   struct
   {
      int w, h;
   } screens, display;

   /**
    * ENTRANCE_USE_PAM - if set to use pam authentication
    * ENTRANCE_USE_SHADOW- if set to use shadow auth if pam is not avail
    */
   int auth;
   /**
    * engine - the evas rendering engine to use
    * 1 - OpenGL rendering engine
    * 0 - The software rendering engine (default)
    */
   int engine;

   /**
    * reboot - whether or not to allow themeable rebooting
    * 1 - Allow Rebooting
    * 0 - Disable theme rebooting
    */
   int reboot;
   /**
    * halt - whether or not to allow themeable halting
    * 1 - Allow Halting
    * 0 - Disable theme halting
    */
   int halt;

   /**
    * fonts - available font paths for fonts
    */
   Eina_List *fonts;

   /**
    * users - users known to the system
    */
   struct
   {
      int remember;
      int remember_n;
      Evas_Hash *hash;
      Eina_List *keys;
   } users;

   /**
    * sessions - sessions known to the system
    */
   struct
   {
      Evas_Hash *hash;
      Eina_List *keys;
   } sessions;

   char *xsession;

   struct
   {
	/**
	 * uid we can find in the users hash
	 */
      char *username;
	/**
	 * 0 = off, 1 = immediate exec, 2 = show theme & post auth
	 */
      int mode;
   } autologin;

   struct {
      char *prevuser;
      /**
       * 0 = off, 1 = presel previous
       */
      int mode;
   } presel;
};

typedef struct _Entrance_Config Entrance_Config;

Entrance_Config *entrance_config_load(char *file);
void entrance_config_print(Entrance_Config * e);
void entrance_config_free(Entrance_Config * e);
void entrance_config_prepend_recent_user(Entrance_Config * e, char *str);
void entrance_config_user_list_save(Entrance_Config * e, const char *file);
void entrance_config_prevuser_save(char *user, const char *file);
int  entrance_config_save(Entrance_Config * e, const char *file);

#endif
