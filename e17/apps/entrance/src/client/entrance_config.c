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

static char *
get_hostname(void)
{
   char buf[255];               /* some standard somewhere limits hostname
                                   lengths to this */
   char *result = NULL;

   if (!(gethostname(buf, 255)))
      result = strdup(buf);
   else
      result = strdup("Localhost");
   return (result);
}


static void
entrance_config_populate(Entrance_Config e, E_DB_File * db)
{
   char *str;
   Evas_List *l = NULL;
   int i = 0, num_session = 0, def = 0;
   char buf[PATH_MAX];
   Entrance_Session_Type *st = NULL;

   if ((!e) || (!db))
      return;

   st = (Entrance_Session_Type *) malloc(sizeof(Entrance_Session_Type));
   memset(st, 0, sizeof(Entrance_Session_Type));
   st->name = "Default";
   st->path = "default";
   st->icon = PACKAGE_DATA_DIR "/data/images/sessions/default.png";
   l = evas_list_append(l, st);

   if (!e_db_int_get(db, "/entrance/session/default", &def))
      def = 0;

   if (e_db_int_get(db, "/entrance/session/count", &num_session))
   {
      for (i = 0; i < num_session; i++)
      {
         st = (Entrance_Session_Type *) malloc(sizeof(Entrance_Session_Type));
         memset(st, 0, sizeof(Entrance_Session_Type));

         snprintf(buf, PATH_MAX, "/entrance/session/%d/name", i);
         st->name = e_db_str_get(db, buf);
         snprintf(buf, PATH_MAX, "/entrance/session/%d/path", i);
         st->path = e_db_str_get(db, buf);
         snprintf(buf, PATH_MAX, "/entrance/session/%d/icon", i);
         str = e_db_str_get(db, buf);
         snprintf(buf, PATH_MAX, "%s/data/images/sessions/%s",
                  PACKAGE_DATA_DIR, str);
         st->icon = strdup(buf);
         l = evas_list_append(l, st);

         if (i == def)
         {
            e->default_session = st;
            e->default_index = i;
         }
      }
      e->sessions = l;
   }
   else
   {
      evas_list_append(e->sessions, NULL);
      e->sessions = NULL;
      fprintf(stderr, "Warning: No sessions found, using default\n");
   }

   if ((str = e_db_str_get(db, "/entrance/theme")))
      e->theme = str;
   else
      e->theme = strdup("BlueCrystal");

   if ((str = e_db_str_get(db, "/entrance/welcome")))
      e->welcome = str;
   else
      e->welcome = strdup("Enter your username");

   if ((str = e_db_str_get(db, "/entrance/passwd")))
      e->passwd = str;
   else
      e->passwd = strdup("Enter your password...");

   if ((str = e_db_str_get(db, "/entrance/greeting")))
      e->greeting = str;
   else
      e->greeting = strdup("Welcome to");

   if (!e_db_int_get(db, "/entrance/passwd_echo", &(e->passwd_echo)))
      e->passwd_echo = 1;

   if (!e_db_int_get(db, "/entrance/xinerama/screens/w", &(e->screens.w)))
      e->screens.w = 1;
   if (!e_db_int_get(db, "/entrance/xinerama/screens/h", &(e->screens.h)))
      e->screens.h = 1;
   if (!e_db_int_get(db, "/entrance/xinerama/on/w", &(e->display.w)))
      e->display.w = 1;
   if (!e_db_int_get(db, "/entrance/xinerama/on/h", &(e->display.h)))
      e->display.h = 1;

   str = get_hostname();
   snprintf(buf, PATH_MAX, "%s %s", e->greeting, str);
   free(e->greeting);
   free(str);
   e->greeting = strdup(buf);

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
      if (e->passwd)
         free(e->passwd);
      if (e->welcome)
         free(e->welcome);
      if (e->theme)
         free(e->theme);

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
