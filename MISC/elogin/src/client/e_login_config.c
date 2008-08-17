#include "elogin.h"
#include "e_login_config.h"
#include "e_login_session.h"

#define REMEMBER_USERS 3

E_Login_Config
e_login_config_new(void)
{
   E_Login_Config e;

   e = (E_Login_Config) malloc(sizeof(struct _E_Login_Config));
   memset(e, 0, sizeof(struct _E_Login_Config));

   e->screens.w = e->screens.h = e->display.w, e->display.h = 1;

   return (e);
}
static char *
get_hostname(void)
{
   char buf[255];
   char *result = NULL;

   if (!(gethostname(buf, 255)))
      result = strdup(buf);
   else
      result = strdup("Localhost");
   return (result);
}

static void
e_login_config_populate(E_Login_Config e, E_DB_File * db)
{
   char *str = NULL;
   char buf[PATH_MAX];
   Evas_List *l = NULL;
   int i = 0, num_session = 0;

   if ((!e) || (!db))
      return;

   if (e_db_int_get(db, "/elogin/session/count", &num_session))
   {
      for (i = 0; i < num_session; i++)
      {
         char buf[PATH_MAX];
         E_Login_Session_Type *st = NULL;

         st = (E_Login_Session_Type *) malloc(sizeof(E_Login_Session_Type));
         memset(st, 0, sizeof(E_Login_Session_Type));

         snprintf(buf, PATH_MAX, "/elogin/session/%d/name", i);
         st->name = e_db_str_get(db, buf);
         snprintf(buf, PATH_MAX, "/elogin/session/%d/path", i);
         st->path = e_db_str_get(db, buf);
         l = evas_list_append(l, st);
      }
      e->sessions = l;
   }
   else
   {
      fprintf(stderr, "Warning: No sessions found, using default\n");
   }

   if ((str = e_db_str_get(db, "/elogin/bg")))
      e->bg = str;
   else
      e->bg = strdup(PACKAGE_DATA_DIR "/data/bgs/elogin.bg.db");

   if ((str = e_db_str_get(db, "/elogin/welcome/mess")))
      e->welcome.mess = str;
   else
      e->welcome.mess = strdup("Enter your username");

   if ((str = e_db_str_get(db, "/elogin/welcome/font/name")))
      e->welcome.font.name = str;
   else
      e->welcome.font.name = strdup("notepad.ttf");

   if (!e_db_int_get(db, "/elogin/welcome/font/r", &(e->welcome.font.r)))
      e->welcome.font.r = 192;
   if (!e_db_int_get(db, "/elogin/welcome/font/g", &(e->welcome.font.g)))
      e->welcome.font.g = 192;
   if (!e_db_int_get(db, "/elogin/welcome/font/b", &(e->welcome.font.b)))
      e->welcome.font.b = 192;
   if (!e_db_int_get(db, "/elogin/welcome/font/a", &(e->welcome.font.a)))
      e->welcome.font.a = 192;
   if (!e_db_float_get(db, "/elogin/welcome/pos/x", &(e->welcome.pos.x)))
      e->welcome.pos.x = 0.0;
   if (!e_db_float_get(db, "/elogin/welcome/pos/y", &(e->welcome.pos.y)))
      e->welcome.pos.x = 0.0;
   if (!e_db_int_get
       (db, "/elogin/welcome/pos/offset/x", &(e->welcome.pos.offset_x)))
      e->welcome.pos.offset_x = 0;
   if (!e_db_int_get
       (db, "/elogin/welcome/pos/offset/y", &(e->welcome.pos.offset_y)))
      e->welcome.pos.offset_y = 0;
   if (!e_db_int_get
       (db, "/elogin/welcome/font/size", &(e->welcome.font.size)))
      e->welcome.font.size = 20;

   if ((str = e_db_str_get(db, "/elogin/passwd/mess")))
      e->passwd.mess = str;
   else
      e->passwd.mess = strdup("Enter your password...");

   if ((str = e_db_str_get(db, "/elogin/passwd/font/name")))
      e->passwd.font.name = str;
   else
      e->passwd.font.name = strdup("notepad.ttf");

   if (!e_db_int_get(db, "/elogin/passwd/font/r", &(e->passwd.font.r)))
      e->passwd.font.r = 192;
   if (!e_db_int_get(db, "/elogin/passwd/font/g", &(e->passwd.font.g)))
      e->passwd.font.g = 192;
   if (!e_db_int_get(db, "/elogin/passwd/font/b", &(e->passwd.font.b)))
      e->passwd.font.b = 192;
   if (!e_db_int_get(db, "/elogin/passwd/font/a", &(e->passwd.font.a)))
      e->passwd.font.a = 192;
   if (!e_db_int_get(db, "/elogin/passwd/font/size", &(e->passwd.font.size)))
      e->passwd.font.size = 20;
   if (!e_db_float_get(db, "/elogin/passwd/pos/x", &(e->passwd.pos.x)))
      e->passwd.pos.x = 0.0;
   if (!e_db_float_get(db, "/elogin/passwd/pos/y", &(e->passwd.pos.y)))
      e->passwd.pos.x = 0.0;
   if (!e_db_int_get
       (db, "/elogin/passwd/pos/offset/x", &(e->passwd.pos.offset_x)))
      e->passwd.pos.offset_x = 0;
   if (!e_db_int_get
       (db, "/elogin/passwd/pos/offset/y", &(e->passwd.pos.offset_y)))
      e->passwd.pos.offset_y = 0;
   if ((str = e_db_str_get(db, "/elogin/greeting/mess")))
      e->greeting.mess = str;
   else
      e->greeting.mess = strdup("Welcome to");

   if ((str = e_db_str_get(db, "/elogin/greeting/font/name")))
      e->greeting.font.name = str;
   else
      e->greeting.font.name = strdup("notepad.ttf");

   if (!e_db_int_get(db, "/elogin/greeting/font/r", &(e->greeting.font.r)))
      e->greeting.font.r = 192;
   if (!e_db_int_get(db, "/elogin/greeting/font/g", &(e->greeting.font.g)))
      e->greeting.font.g = 192;
   if (!e_db_int_get(db, "/elogin/greeting/font/b", &(e->greeting.font.b)))
      e->greeting.font.b = 192;
   if (!e_db_int_get(db, "/elogin/greeting/font/a", &(e->greeting.font.a)))
      e->greeting.font.a = 192;
   if (!e_db_float_get(db, "/elogin/greeting/pos/x", &(e->greeting.pos.x)))
      e->greeting.pos.x = 0.0;
   if (!e_db_float_get(db, "/elogin/greeting/pos/y", &(e->greeting.pos.y)))
      e->greeting.pos.x = 0.0;
   if (!e_db_int_get
       (db, "/elogin/greeting/pos/offset/x", &(e->greeting.pos.offset_x)))
      e->greeting.pos.offset_x = 0;
   if (!e_db_int_get
       (db, "/elogin/greeting/pos/offset/y", &(e->greeting.pos.offset_y)))
      e->greeting.pos.offset_y = 0;
   if (!e_db_int_get
       (db, "/elogin/greeting/font/size", &(e->greeting.font.size)))
      e->greeting.font.size = 20;

   if (!e_db_int_get(db, "/elogin/xinerama/screens/w", &(e->screens.w)))
      e->screens.w = 1;
   if (!e_db_int_get(db, "/elogin/xinerama/screens/h", &(e->screens.h)))
      e->screens.h = 1;
   if (!e_db_int_get(db, "/elogin/xinerama/on/w", &(e->display.w)))
      e->display.w = 1;
   if (!e_db_int_get(db, "/elogin/xinerama/on/h", &(e->display.h)))
      e->display.h = 1;

   if ((str = get_hostname()))
   {
      snprintf(buf, PATH_MAX, "%s %s", e->greeting.mess, str);
      free(e->greeting.mess);
      free(str);
      e->greeting.mess = strdup(buf);
   }
}

E_Login_Config
e_login_config_parse(char *file)
{
   E_Login_Config e = NULL;

   if (file)
   {
      E_DB_File *db;

      if ((db = e_db_open_read(file)))
      {
         e = e_login_config_new();
         e_login_config_populate(e, db);
         e_db_close(db);
      }
   }
   return (e);
}

void
e_login_config_print(E_Login_Config e)
{
   fprintf(stderr,
           "%s is the background\n"
           "%s is the welcome message\n%s is the message fontname\n"
           "%d is the fontsize{%d,%d,%d,%d}\n"
           "%s is the passwd message\n%s is the passwd fontname\n"
           "%d is the fontsize{%d,%d,%d,%d}\n", e->bg, e->welcome.mess,
           e->welcome.font.name, e->welcome.font.size, e->welcome.font.r,
           e->welcome.font.g, e->welcome.font.b, e->welcome.font.a,
           e->passwd.mess, e->passwd.font.name, e->passwd.font.size,
           e->passwd.font.r, e->passwd.font.g, e->passwd.font.b,
           e->passwd.font.a);
}

void
e_login_config_free(E_Login_Config e)
{
   if (e)
   {
      if (e->bg)
         free(e->bg);
      if (e->passwd.font.name)
         free(e->passwd.font.name);
      if (e->passwd.mess)
         free(e->passwd.mess);
      if (e->welcome.font.name)
         free(e->welcome.font.name);
      if (e->welcome.mess)
         free(e->welcome.mess);

      free(e);
   }
}

#if 0
int
main(int argc, char *argv[])
{
   E_Login_Config e;

   while (--argc)
   {
      e = e_login_config_parse(argv[argc]);
      e_login_config_print(e);
      e_login_config_free(e);
   }
   return (0);
}
#endif
