#include <Ecore_File.h>
#include <Ecore_Data.h>
#include <Ecore_Config.h>
#include <Efreet.h>

#include "entrance.h"
#include "entrance_config.h"
#include "entrance_user.h"
#include "entrance_x_session.h"

#include "../config.h"

struct _Entrance_Config_And_Path
{
   Entrance_Config *e;
   const char *path;
};


static void _cb_xsessions_foreach(void *list_data, void *data);
static void _entrance_xsessions_dir_scan(const char *dir, Entrance_Config *e);
static Eina_Bool _cb_users_free(const Eina_Hash *hash, const void *key, void *data, void *fdata);
static Eina_Bool _cb_x_sessions_free(const Eina_Hash *hash, const void *key, void *data, void *fdata);

/**
@file entrance_config.c
@brief System-wide configuration options for various settings in Entrance
*/

Entrance_Config *
entrance_config_new(void)
{
   Entrance_Config *e;

   e = (Entrance_Config *) malloc(sizeof(struct _Entrance_Config));
   memset(e, 0, sizeof(struct _Entrance_Config));

   e->screens.w = e->screens.h = e->display.w, e->display.h = 1;

   return (e);
}

/**
 * _entrance_config_defaults_set - set default values for
 * the Entrance_Config struct
 */
static void
_entrance_config_defaults_set()
{
   ecore_config_string_default("/entrance/theme", "default.edj");
   ecore_config_string_default("/entrance/background", "");
   ecore_config_string_default("/entrance/pointer",
                               PACKAGE_DATA_DIR "/images/pointer.png");
   ecore_config_string_default("/entrance/greeting/before", "Welcome to");
   ecore_config_string_default("/entrance/greeting/after", "");
   ecore_config_string_default("/entrance/date_format", "%x");
   ecore_config_string_default("/entrance/time_format", "%X");

   ecore_config_int_default("/entrance/autologin/mode", 0);
   ecore_config_string_default("/entrance/autologin/user", "");

   ecore_config_int_default("/entrance/presel/mode", 1);
   ecore_config_string_default("/entrance/presel/prevuser", "");

   ecore_config_int_default("/entrance/user/remember", 1);
   ecore_config_int_default("/entrance/user/remember_n", 5);
   ecore_config_int_default("/entrance/user/count", 0);

   ecore_config_int_default("/entrance/engine", 0);

   ecore_config_int_default("/entrance/system/reboot", 1);
   ecore_config_int_default("/entrance/system/halt", 1);

   ecore_config_int_default("/entrance/session/count", 1);
   ecore_config_string_default("/entrance/session/0/session", "default");
   ecore_config_string_default("/entrance/session/0/title", "Default");
   ecore_config_string_default("/entrance/session/0/icon", "default.png");

   ecore_config_string_default("/entrance/xsession", ENTRANCE_XSESSION);

   ecore_config_int_default("/entrance/auth", 1);
}

/**
 * entrance_config_populate - populate the Entrance_Config struct with
 * the data from ecore_config
 * @param e Valid Entrance_Config struct
 */
static void
entrance_config_populate(Entrance_Config * e)
{
   Entrance_User *eu = NULL;
   Eina_List *dirs;
   Eina_List *l;
   char *user = NULL;
   char *icon = NULL;
   char *session = NULL;
   char *path;

   int i, num_user;
   char buf[PATH_MAX];
   int num_session;

   if (!e)
      return;

   /* strings 'n things */
   e->theme = ecore_config_string_get("/entrance/theme");
   e->background = ecore_config_string_get("/entrance/background");
   e->pointer = ecore_config_string_get("/entrance/pointer");
   e->before.string = ecore_config_string_get("/entrance/greeting/before");
   e->after.string = ecore_config_string_get("/entrance/greeting/after");
   e->date.string = ecore_config_string_get("/entrance/date_format");
   e->time.string = ecore_config_string_get("/entrance/time_format");

   e->autologin.mode = ecore_config_int_get("/entrance/autologin/mode");
   e->autologin.username =
      ecore_config_string_get("/entrance/autologin/user");

   e->presel.mode = ecore_config_int_get("/entrance/presel/mode");
   e->presel.prevuser = ecore_config_string_get("/entrance/presel/prevuser");

   e->users.remember = ecore_config_int_get("/entrance/user/remember");
   e->users.remember_n = ecore_config_int_get("/entrance/user/remember_n");

   e->engine = ecore_config_int_get("/entrance/engine");

   e->reboot = ecore_config_int_get("/entrance/system/reboot");
   e->halt = ecore_config_int_get("/entrance/system/halt");
   e->xsession = ecore_config_string_get("/entrance/xsession");

   num_user = ecore_config_int_get("/entrance/user/count");
   for (i = 0; i < num_user; i++)
   {
      snprintf(buf, PATH_MAX, "/entrance/user/%d/user", i);
      if ((user = ecore_config_string_get(buf)))
      {
         snprintf(buf, PATH_MAX, "/entrance/user/%d/icon", i);
         icon = ecore_config_string_get(buf);
         snprintf(buf, PATH_MAX, "/entrance/user/%d/session", i);
         session = ecore_config_string_get(buf);

         printf("%s %s %s\n", user, icon, session);

         if ((eu = entrance_user_new(user, icon, session)))
         {
	    if (!e->users.hash) e->users.hash = eina_hash_string_superfast_new(NULL);
            eina_hash_add(e->users.hash, user, eu);
            e->users.keys = eina_list_append(e->users.keys, eu->name);
         }
         else
         {
            free(user);
            if (icon)
               free(icon);
            if (session)
               free(session);
         }
      }
   }

   /* Search the local session directory first. */
   _entrance_xsessions_dir_scan(ENTRANCE_SESSIONS_DIR, e);

   /* now the user and system XDG dirs (XXX does anything actually store xsession files here?) */
   snprintf(buf, sizeof(buf), "%s/xsessions", efreet_data_home_get());
   _entrance_xsessions_dir_scan(buf, e);

   dirs = efreet_data_dirs_get();
   EINA_LIST_FOREACH(dirs, l, path)
     {
	snprintf(buf, sizeof(buf), "%s/xsessions", path);
	_entrance_xsessions_dir_scan(buf, e);
     }

   /* check the system session dir */
   _entrance_xsessions_dir_scan("/etc/X11/sessions", e);

   num_session = ecore_config_int_get("/entrance/session/count");
   for (i = 0; i < num_session; i++)
   {
      Entrance_X_Session *exs;
      char *title;
      snprintf(buf, PATH_MAX, "/entrance/session/%d/title", i);
      title = ecore_config_string_get(buf);
      snprintf(buf, PATH_MAX, "/entrance/session/%d/session", i);
      session = ecore_config_string_get(buf);
      snprintf(buf, PATH_MAX, "/entrance/session/%d/icon", i);
      icon = ecore_config_string_get(buf);

      if ((exs = entrance_x_session_new(title, icon, session)))
      {
         e->sessions.keys = eina_list_append(e->sessions.keys, title);
	 if (!e->sessions.hash) e->sessions.hash = eina_hash_string_superfast_new(NULL);
	 eina_hash_add(e->sessions.hash, exs->name, exs);
      }
   }


   /* auth info */
   e->auth = ecore_config_int_get("/entrance/auth");
   if (e->auth != ENTRANCE_USE_PAM)
   {
      /* check whether /etc/shadow can be used for authentication */
      if (!access("/etc/shadow", R_OK))
         e->auth = ENTRANCE_USE_SHADOW;
      else if (!access("/etc/shadow", F_OK))
      {
         syslog(LOG_CRIT,
                "/etc/shadow was found but couldn't be read. Run entrance as root.");
         if (getuid() == 0)
         {
            exit(EXITCODE);
         }
      }
   }
#ifndef HAVE_PAM
   else
      syslog(LOG_WARNING,
             "Entrance has been built without PAM support, so PAM isn't used for authentication!");
#endif
}

/**
 * entrance_config_parse parse the config file named
 * @param file the file on disk we should load config opts from
 * @return a valid Entrance_Config file, or NULL on error
 */
Entrance_Config *
entrance_config_load(char *file)
{
   Entrance_Config *e = NULL;

   if (file)
   {
      e = entrance_config_new();
      _entrance_config_defaults_set();
      ecore_config_file_load(file);
      entrance_config_populate(e);
   }
   return (e);
}

void
entrance_config_print(Entrance_Config * e)
{
   int i = 0;
   char buf[PATH_MAX];
   Entrance_User *eu;
   Entrance_X_Session *exs;
   Eina_List *l = NULL;
   char *strings[] = { "/entrance/theme",
      "/entrance/pointer", "/entrance/date_format", "/entrance/time_format",
      "/entrance/greeting/before", "/entrance/greeting/after",
      "/entrance/xsession"
   };
   char *values[] = { e->theme, e->pointer, e->date.string,
      e->time.string, e->before.string, e->after.string
   };
   int ssize = sizeof(strings) / sizeof(char *);

   char *intstrings[] = { "/entrance/engine", "/entrance/system/reboot",
      "/entrance/system/halt", "/entrance/users/remember",
      "/entrance/users/remember_n", "/entrance/auth"
   };
   int intvalues[] = { e->engine, e->reboot, e->halt, e->users.remember,
      e->users.remember_n, e->auth
   };
   int intsize = sizeof(intstrings) / sizeof(int);

   for (i = 0; i < ssize; i++)
   {
      printf("%s %s\n", strings[i], values[i]);
   }
   for (i = 0; i < intsize; i++)
   {
      printf("%s %d\n", intstrings[i], intvalues[i]);
   }
   for (i = 0, l = e->users.keys; l; l = l->next, i++)
   {
      if ((eu = eina_hash_find(e->users.hash, (char *) l->data)))
      {
         snprintf(buf, PATH_MAX, "/entrance/user/%d/user", i);
         printf("%s %s\n", buf, eu->name);
         snprintf(buf, PATH_MAX, "/entrance/user/%d/session", i);
         printf("%s %s\n", buf, eu->session);
         snprintf(buf, PATH_MAX, "/entrance/user/%d/icon", i);
         printf("%s %s\n", buf, eu->icon);
      }
      else
         i--;
   }
   snprintf(buf, PATH_MAX, "/entrance/user/count");
   printf("%s %d\n", buf, i);
   for (i = 0, l = e->sessions.keys; l; l = l->next, i++)
   {
      if (l->data)
      {
         if ((exs = eina_hash_find(e->sessions.hash, (char *) l->data)))
         {
            snprintf(buf, PATH_MAX, "/entrance/session/%d/title", i);
            printf("%s %s\n", buf, exs->name);
            snprintf(buf, PATH_MAX, "/entrance/session/%d/session", i);
            printf("%s %s\n", buf, exs->session);
            snprintf(buf, PATH_MAX, "/entrance/session/%d/icon", i);
            printf("%s %s\n", buf, exs->icon);
         }
      }
   }
   snprintf(buf, PATH_MAX, "/entrance/session/count");
   printf("%s %d\n", buf, i);
}

void
entrance_config_store(Entrance_Config * e)
{
   int i = 0;
   char buf[PATH_MAX];
   Entrance_User *eu;
   Entrance_X_Session *exs;
   Eina_List *l = NULL;
   char *strings[] = { "/entrance/theme", "/entrance/background",
      "/entrance/pointer", "/entrance/date_format", "/entrance/time_format",
      "/entrance/greeting/before", "/entrance/greeting/after",
      "/entrance/xsession"
   };
   char *values[] = { e->theme, e->background, e->pointer, e->date.string,
      e->time.string, e->before.string, e->after.string
   };
   int ssize = sizeof(strings) / sizeof(char *);

   char *intstrings[] = { "/entrance/engine", "/entrance/system/reboot",
      "/entrance/system/halt", "/entrance/users/remember",
      "/entrance/users/remember_n", "/entrance/auth"
   };
   int intvalues[] = { e->engine, e->reboot, e->halt, e->users.remember,
      e->users.remember_n, e->auth
   };
   int intsize = sizeof(intstrings) / sizeof(int);

   for (i = 0; i < ssize; i++)
   {
      ecore_config_string_set(strings[i], values[i]);
   }
   for (i = 0; i < intsize; i++)
   {
      ecore_config_int_set(intstrings[i], intvalues[i]);
   }
   for (i = 0, l = e->users.keys; l; l = l->next, i++)
   {
      if ((eu = eina_hash_find(e->users.hash, (char *) l->data)))
      {
         snprintf(buf, PATH_MAX, "/entrance/user/%d/user", i);
         ecore_config_string_set(buf, eu->name);
         snprintf(buf, PATH_MAX, "/entrance/user/%d/session", i);
         ecore_config_string_set(buf, eu->session);
         snprintf(buf, PATH_MAX, "/entrance/user/%d/icon", i);
         ecore_config_string_set(buf, eu->icon);
      }
      else
         i--;
   }
   snprintf(buf, PATH_MAX, "/entrance/user/count");
   ecore_config_int_set(buf, i);
   for (i = 0, l = e->sessions.keys; l; l = l->next, i++)
   {
      if (l->data)
      {
         if ((exs = eina_hash_find(e->sessions.hash, (char *) l->data)))
         {
            snprintf(buf, PATH_MAX, "/entrance/session/%d/title", i);
            ecore_config_string_set(buf, exs->name);
            snprintf(buf, PATH_MAX, "/entrance/session/%d/session", i);
            ecore_config_string_set(buf, exs->session);
            snprintf(buf, PATH_MAX, "/entrance/session/%d/icon", i);
            ecore_config_string_set(buf, exs->icon);
         }
      }
   }
   snprintf(buf, PATH_MAX, "/entrance/session/count");
   ecore_config_int_set(buf, i);
}

int
entrance_config_save(Entrance_Config * e, const char *file)
{
   if (file)
   {
      entrance_config_store(e);
      ecore_config_file_save(file);
   }
   return (1);
}

/**
 * entrance_config_free Free up an Entrance_Config struct
 * @param e A Entrance_Config struct pointer
 */
void
entrance_config_free(Entrance_Config * e)
{
   if (e)
   {
      if (e->theme)
         free(e->theme);
      if (e->background)
         free(e->background);
      if (e->pointer)
         free(e->pointer);
      if (e->date.string)
         free(e->date.string);
      if (e->time.string)
         free(e->time.string);
      if (e->before.string)
         free(e->before.string);
      if (e->after.string)
         free(e->after.string);
      if (e->autologin.username)
         free(e->autologin.username);
      if (e->users.hash)
      {
         eina_hash_foreach(e->users.hash, _cb_users_free, NULL);
         eina_hash_free(e->users.hash);
      }
      if (e->users.keys)
         eina_list_free(e->users.keys);
      if (e->sessions.hash)
      {
         eina_hash_foreach(e->sessions.hash, _cb_x_sessions_free, NULL);
         eina_hash_free(e->sessions.hash);
      }
      if (e->sessions.keys)
         eina_list_free(e->sessions.keys);
      free(e);
   }
}

/**
 * entrance_config_user_list_write : Write out the possibly reordered user
 * list into the config db.
 * @e - a pointer to the config struct we want to write the user list for
 */
void
entrance_config_user_list_save(Entrance_Config * e, const char *file)
{
   int i = 0;
   Eina_List *l = NULL;
   Entrance_User *eu = NULL;
   char buf[PATH_MAX];

   if (!e->users.remember)
      return;

   for (i = 0, l = e->users.keys; l && i < e->users.remember_n;
        l = l->next, i++)
   {
      if ((eu = eina_hash_find(e->users.hash, (char *) l->data)))
      {
         if (eu->name)
         {
            snprintf(buf, PATH_MAX, "/entrance/user/%d/user", i);
            ecore_config_string_set(buf, eu->name);
         }
         if (eu->session)
         {
            snprintf(buf, PATH_MAX, "/entrance/user/%d/session", i);
            ecore_config_string_set(buf, eu->session);
         }
         if (eu->icon)
         {
            snprintf(buf, PATH_MAX, "/entrance/user/%d/icon", i);
            ecore_config_string_set(buf, eu->icon);
         }
      }
      else
         i--;
   }
   snprintf(buf, PATH_MAX, "/entrance/user/count");
   ecore_config_int_set(buf, i);
   ecore_config_file_save(file);
}

void
entrance_config_prevuser_save(char *user, const char *file)
{
   char buf[PATH_MAX];

   snprintf(buf, PATH_MAX, "/entrance/presel/prevuser");
   ecore_config_string_set(buf, user);
   ecore_config_file_save(file);
}

static void
_cb_xsessions_foreach(void *list_data, void *data)
{
   const char *filename = list_data;
   struct _Entrance_Config_And_Path *ep = data;
   Entrance_Config *e;
   Entrance_X_Session *exs = NULL;
   Eina_List *commands;
   char *command = NULL;
   char path[PATH_MAX];

   if (!filename)
      return;

   e = ep->e;
   if (!e)
      return;

   snprintf(path, PATH_MAX, "%s/%s", ep->path, filename);

   Efreet_Desktop *ed = efreet_desktop_get(path);

   if (!ed)
      return;

   /* Get the full command. */
   /* We are not passing a list of files, so we only expect one command. */
   commands = efreet_desktop_command_local_get(ed, NULL);
   if (commands)
   {
      char *temp;
      
      command = eina_list_data_get(commands);

      EINA_LIST_FREE(commands, temp)
	if (command != temp) free(temp);
   }
   if (!command)
     return;


   if ((exs = entrance_x_session_new(ed->name, ed->icon, command)))
   {
      /* Sessions found earlier in the FDO search sequence override those
         found later. */
      if (eina_hash_find(e->sessions.hash, exs->name) == NULL)
      {
         e->sessions.keys = eina_list_append(e->sessions.keys, exs->name);
	 if (!e->sessions.hash) e->sessions.hash = eina_hash_string_superfast_new(NULL);
	 eina_hash_add(e->sessions.hash, exs->name, exs);
      }
      else
      {
         entrance_x_session_free(exs);
      }
   }

   efreet_desktop_free(ed);
}

static void
_entrance_xsessions_dir_scan(const char *dir, Entrance_Config *e)
{
   struct _Entrance_Config_And_Path ep;
   Eina_List *xsessions;
   char *filename;

   if (!dir) return;

   ep.e = e;
   ep.path = dir; 

   xsessions = ecore_file_ls(dir);
   EINA_LIST_FREE(xsessions, filename)
   {
	_cb_xsessions_foreach(filename, &ep);
	free(filename);
   }
}

static Eina_Bool
_cb_users_free(const Eina_Hash *hash, const void *key, void *data, void *fdata)
{
  Entrance_User *user = data;
  entrance_user_free(user);
  return TRUE;
}

static Eina_Bool
_cb_x_sessions_free(const Eina_Hash *hash, const void *key, void *data, void *fdata)
{
  Entrance_X_Session *x_session = data;
  entrance_x_session_free(x_session);
  return TRUE;
}

#if 0
int
main(int argc, char *argv[])
{
   Entrance_Config e;

   while (--argc)
   {
      e = entrance_config_parse(argv[argc]);
      entrance_config_print(e);
      entrance_config_free(e);
   }
   return (0);
}
#endif
