#include "entrance.h"
#include "entrance_config.h"

#define REMEMBER_USERS 3

Entrance_Config
entrance_config_new(void)
{
   Entrance_Config e;

   e = (Entrance_Config) malloc(sizeof(struct _Entrance_Config));
   memset(e, 0, sizeof(struct _Entrance_Config));

   e->screens.w = e->screens.h = e->display.w, e->display.h = 1;

   return (e);
}

static void
entrance_config_populate(Entrance_Config e, E_DB_File * db)
{
   char *str;
   int i = 0, num_session = 0;
   char buf[PATH_MAX];
   int num_fonts;

   if ((!e) || (!db))
      return;

   /* strings 'n things */
   if ((str = e_db_str_get(db, "/entrance/engine")))
      e->engine = str;
   else
      e->engine = strdup("software");

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
      e->after.string = strdup(":");
   if ((str = e_db_str_get(db, "/entrance/date_format")))
      e->date.string = str;
   else
      e->date.string = strdup("%A %B %e, %Y");

   if ((str = e_db_str_get(db, "/entrance/time_format")))
      e->time.string = str;
   else
      e->time.string = strdup("%l:%M:%S %p");
#if 0
   if (e_db_int_get(db, "/entrance/user/count", &num_user))
   {
      for (i = 0; i < num_user; i++)
      {
         eu = (Entrance_User *) malloc(sizeof(Entrance_User));
         memset(eu, 0, sizeof(Entrance_User));
         snprintf(buf, PATH_MAX, "/entrance/user/%d/name", i);
         eu->name = e_db_str_get(db, buf);
         snprintf(buf, PATH_MAX, "/entrance/user/%d/img", i);
         eu->img = e_db_str_get(db, buf);
         snprintf(buf, PATH_MAX, "/entrance/user/%d/sys", i);
         if (!e_db_int_get(db, buf, &(eu->sys)))
            eu->sys = 1;
         U = evas_list_append(U, eu);
      }

      e->users = U;
   }
   else
   {
      evas_list_append(e->users, NULL);
      e->users = NULL;
      syslog(LOG_WARNING, "Warning: No users found.");
   }
#endif

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
   if (!e_db_int_get(db, "/entrance/system/reboot", &(e->reboot.allow)))
      e->reboot.allow = 0;
   if (!e_db_int_get(db, "/entrance/system/halt", &(e->halt.allow)))
      e->halt.allow = 0;

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

Entrance_Config
entrance_config_parse(char *file)
{
   Entrance_Config e = NULL;

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

/*void
entrance_config_print(Entrance_Config e)
{
   fprintf(stderr,
           "%s is the background\n"
           "%s is the welcomeage\n%s is the message fontname\n"
           "%d is the fontsize{%d,%d,%d,%d}\n"
           "%s is the passwdage\n%s is the passwd fontname\n"
           "%d is the fontsize{%d,%d,%d,%d}\n", e->bg, e->welcome,
           e->welcome.font.name, e->welcome.font.size, e->welcome.font.r,
           e->welcome.font.g, e->welcome.font.b, e->welcome.font.a,
           e->passwd, e->passwd.font.name, e->passwd.font.size,
           e->passwd.font.r, e->passwd.font.g, e->passwd.font.b,
           e->passwd.font.a);
}*/

void
entrance_config_free(Entrance_Config e)
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
