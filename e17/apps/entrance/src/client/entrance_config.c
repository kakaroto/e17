#include "entrance.h"
#include "entrance_config.h"
#include "entrance_user.h"

/**
@file entrance_config.c
@brief System-wide configuration options for various settings in Entrance
*/

#define REMEMBER_USERS 3

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
 * entrance_config_populate - populate the Entrance_Config struct with
 * the data provided by the valid E_DB_File
 * @param e Valid Entrance_Config struct
 * @param db a valid E_DB_File handle opened for reading
 */
static void
entrance_config_populate(Entrance_Config * e, E_DB_File * db)
{
   char *str;
   int i = 0, num_session = 0, num_user;
   char buf[PATH_MAX];
   int num_fonts;

   if ((!e) || (!db))
      return;

   /* strings 'n things */
   if ((str = e_db_str_get(db, "/entrance/theme")))
      e->theme = str;
   else
      e->theme = strdup("default.eet");

   if ((str = e_db_str_get(db, "/entrance/pointer")))
      e->pointer = str;
   else
      e->pointer = strdup(PACKAGE_DATA_DIR "/images/pointer.png");

   if ((str = e_db_str_get(db, "/entrance/greeting/before")))
      e->before.string = str;
   else
      e->before.string = strdup("Welcome to ");
   if ((str = e_db_str_get(db, "/entrance/greeting/after")))
      e->after.string = str;
   else
      e->after.string = strdup("");
   if ((str = e_db_str_get(db, "/entrance/date_format")))
      e->date.string = str;
   else
      e->date.string = strdup("%A %B %e, %Y");

   if ((str = e_db_str_get(db, "/entrance/time_format")))
      e->time.string = str;
   else
      e->time.string = strdup("%l:%M:%S %p");
   /* ints */
   if (!e_db_int_get(db, "/entrance/user/remember", &e->users.remember))
      e->users.remember = 1;
   if (!e_db_int_get(db, "/entrance/user/remember_n", &e->users.remember_n))
      e->users.remember_n = 5;
   if (!e_db_int_get(db, "/entrance/engine", &e->engine))
      e->engine = 0;
   if (!e_db_int_get(db, "/entrance/system/reboot", &(e->reboot)))
      e->reboot = 0;
   if (!e_db_int_get(db, "/entrance/system/halt", &(e->halt)))
      e->halt = 0;
   if (e_db_int_get(db, "/entrance/user/count", &num_user))
   {
      Entrance_User *eu = NULL;
      char *user = NULL;
      char *icon = NULL;
      char *session = NULL;

      for (i = 0; i < num_user; i++)
      {
         snprintf(buf, PATH_MAX, "/entrance/user/%d/user", i);
         if ((user = e_db_str_get(db, buf)))
         {
            snprintf(buf, PATH_MAX, "/entrance/user/%d/icon", i);
            icon = e_db_str_get(db, buf);
            snprintf(buf, PATH_MAX, "/entrance/user/%d/session", i);
            session = e_db_str_get(db, buf);

            if ((eu = entrance_user_new(user, icon, session)))
            {
               e->users.hash = evas_hash_add(e->users.hash, user, eu);
               e->users.keys = evas_list_append(e->users.keys, user);
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
      /* FIXME syslog(LOG_NORMAL, "Loaded %d users", num_user) */
   }
   else
   {
      syslog(LOG_WARNING, "Warning: No users found.");
   }

   /* session hash and font list */
   if (e_db_int_get(db, "/entrance/session/count", &num_session))
   {
      char *key = NULL;
      char *icon = NULL;
      char *value = NULL;

      for (i = 0; i < num_session; i++)
      {
         snprintf(buf, PATH_MAX, "/entrance/session/%d/title", i);
         key = e_db_str_get(db, buf);
         snprintf(buf, PATH_MAX, "/entrance/session/%d/session", i);
         value = e_db_str_get(db, buf);
         snprintf(buf, PATH_MAX, "/entrance/session/%d/icon", i);
         icon = e_db_str_get(db, buf);

         e->sessions.hash = evas_hash_add(e->sessions.hash, key, value);
         e->sessions.icons = evas_hash_add(e->sessions.icons, key, icon);
         e->sessions.keys = evas_list_append(e->sessions.keys, key);
      }
   }
   /* 
    * FIXME: With embedded fonts in your edjes, do we even wanna bother with
    * the font path nightmare anymore ?  Unless we use etox or something ...
    */
   if (e_db_int_get(db, "/entrance/fonts/count", &num_fonts))
   {
      char *value = NULL;

      for (i = 0; i < num_fonts; i++)
      {
         snprintf(buf, PATH_MAX, "/entrance/fonts/%d/str", i);
         if ((value = e_db_str_get(db, buf)))
         {
            e->fonts = evas_list_append(e->fonts, value);
         }

      }
   }

#if 0
   if (!e_db_int_get(db, "/entrance/xinerama/screens/w", &(e->screens.w)))
      e->screens.w = 1;
   if (!e_db_int_get(db, "/entrance/xinerama/screens/h", &(e->screens.h)))
      e->screens.h = 1;
   if (!e_db_int_get(db, "/entrance/xinerama/on/w", &(e->display.w)))
      e->display.w = 1;
   if (!e_db_int_get(db, "/entrance/xinerama/on/h", &(e->display.h)))
      e->display.h = 1;
#endif


   /* auth info */

   if (!e_db_int_get(db, "/entrance/auth", &(e->auth)))
      e->auth = 0;
   if (e->auth != ENTRANCE_USE_PAM)
   {
      /* check whether /etc/shadow can be used for authentication */
      if (!access("/etc/shadow", R_OK))
         e->auth = ENTRANCE_USE_SHADOW;
      else if (!access("/etc/shadow", F_OK))
      {
         syslog(LOG_CRIT,
                "/etc/shadow was found but couldn't be read. Run entrance as root.");
         exit(-1);
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
entrance_config_parse(char *file)
{
   Entrance_Config *e = NULL;

   if (file)
   {
      E_DB_File *db;

      if ((db = e_db_open_read(file)))
      {
         e = entrance_config_new();
         entrance_config_populate(e, db);
         e_db_close(db);
      }
   }
   return (e);
}

void
entrance_config_print(Entrance_Config * e)
{
   int i = 0;
   char *str = NULL;
   Entrance_User *eu;
   Evas_List *l = NULL;
   char *strings[] = { "Theme Edje %s\n",
      "Pointer Image %s\n", "Date Format %s\n", "Time Format %s\n",
      "Greeting Before %s\n", "Greeting After %s\n"
   };
   char *values[] = { e->theme, e->pointer, e->date.string,
      e->time.string, e->before.string, e->after.string
   };
   int ssize = sizeof(strings) / sizeof(char *);

   char *intstrings[] = { "Use Evas GL %d\n", "Allow reboot %d\n",
      "Allow Shutdown %d\n",
      "Remember Users who login %d\n",
      "Number of users to remember %d\n",
      "Authentication Mode %d\n"
   };
   int intvalues[] = { e->engine, e->reboot, e->halt, e->users.remember,
      e->users.remember_n, e->auth
   };
   int intsize = sizeof(intstrings) / sizeof(int);

   char *userstrings[] = { "\nUsername %s\n", "XSession %s\n",
      "Icon file %s\n"
   };

   for (i = 0; i < ssize; i++)
   {
      printf(strings[i], values[i]);
   }
   for (i = 0; i < intsize; i++)
   {
      printf(intstrings[i], intvalues[i]);
   }
   for (l = e->users.keys; l; l = l->next)
   {
      if ((eu = evas_hash_find(e->users.hash, (char *) l->data)))
      {
         printf(userstrings[0], eu->name);
         printf(userstrings[1], eu->session);
         printf(userstrings[2], eu->icon);

      }
   }
   for (l = e->sessions.keys; l; l = l->next)
   {
      printf("\n%s\n", (char *) l->data);
      if ((str = evas_hash_find(e->sessions.icons, (char *) l->data)))
         printf("Session Icon File: %s\n", str);
      if ((str = evas_hash_find(e->sessions.hash, (char *) l->data)))
         printf("XSession exec string %s\n", str);
   }
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
      free(e);
   }
}

/**
 * entrance_config_user_list_write : Write out the possibly reordered user
 * list into the config db.
 * @e - a pointer to the config struct we want to write the user list for
 */
void
entrance_config_user_list_write(Entrance_Config * e)
{
   int i = 0;
   int count = 0;
   Evas_List *l = NULL;
   E_DB_File *db = NULL;
   Entrance_User *eu = NULL;
   char **old_keys = NULL;
   char file[PATH_MAX], buf[PATH_MAX];

   snprintf(file, PATH_MAX, "%s/entrance_config.db", PACKAGE_CFG_DIR);
   if (!e->users.remember)
      return;
/* FIXME: I guess we can't free up old strings like this. */
#if 0
   if ((db = e_db_open(file)))
   {
      if ((old_keys = e_db_match_keys(db, "/entrance/user/", &count)))
      {
         for (i = 0; i < count; i++)
         {
            fprintf(stderr, "Nuking %s\n", old_keys[i]);
            e_db_data_del(db, old_keys[i]);
            free(old_keys[i]);
         }
         free(old_keys);
         e_db_close(db);
         e_db_flush();
      }
   }
#endif
   if ((db = e_db_open(file)))
   {
      for (i = 0, l = e->users.keys; l && i < e->users.remember_n;
           l = l->next, i++)
      {
         if ((eu = evas_hash_find(e->users.hash, (char*)l->data)))
         {
            if (eu->name)
            {
               snprintf(buf, PATH_MAX, "/entrance/user/%d/user", i);
               e_db_str_set(db, buf, eu->name);
            }
            if (eu->session)
            {
               snprintf(buf, PATH_MAX, "/entrance/user/%d/session", i);
               e_db_str_set(db, buf, eu->session);
            }
            if (eu->icon)
            {
               snprintf(buf, PATH_MAX, "/entrance/user/%d/icon", i);
               e_db_str_set(db, buf, eu->icon);
            }
         }
         else
            i--;
      }
      snprintf(buf, PATH_MAX, "/entrance/user/count");
      e_db_int_set(db, buf, i);
      e_db_close(db);
      e_db_flush();
   }
   else
   {
      fprintf(stderr, "Unable to open %s, sure you're root?", file);
   }
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
