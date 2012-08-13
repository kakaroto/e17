#include "ui.h"
#include <time.h>

static char logdir[4096];

static Eina_Bool
_logging_timer(Contact_List *cl)
{
   Eina_List *l;
   Contact *c;

   EINA_LIST_FOREACH(cl->users_list, l, c)
     if (c->log) logging_contact_file_refresh(c);
   ecore_timer_interval_set(cl->logs_refresh, 24 * 60 * 60);
   return EINA_TRUE;
}

const char *
logging_dir_get(void)
{
   return &logdir[0];
}

Eina_Bool
logging_dir_create(Contact_List *cl)
{
   snprintf(logdir, sizeof(logdir), "%s/logs/%s", util_configdir_get(), shotgun_jid_get(cl->account));
   if (ecore_file_is_dir(logdir)) return EINA_TRUE;
   return ecore_file_mkpath(logdir);
}

Eina_Bool
logging_contact_init(Contact *c)
{
   if (c->logdir_exists) return EINA_TRUE;

   c->logdir = eina_stringshare_printf("%s/%s", logging_dir_get(), c->base->jid);
   if (ecore_file_is_dir(c->logdir)) return ++c->logdir_exists;
   return c->logdir_exists = ecore_file_mkpath(c->logdir);
}

Eina_Bool
logging_contact_file_refresh(Contact *c)
{
   char buf[4096], ti[128];
   time_t t, t2;
   struct tm *tt;

   if (c->log) fclose(c->log);
   t = ecore_time_unix_get();
   tt = localtime(&t);
   strftime(ti, sizeof(ti), "%Y-%m-%d.txt", tt);
   if (!c->list->logs_refresh)
     {
        tt->tm_sec = 59;
        tt->tm_min = 59;
        tt->tm_hour = 23;
        tt->tm_wday = tt->tm_yday = 0;
        t2 = mktime(tt);
        /* create job to refresh log files on turn of day */
        c->list->logs_refresh = ecore_timer_add(difftime(t2, t) + 1.0, (Ecore_Task_Cb)_logging_timer, c->list);
     }
   snprintf(buf, sizeof(buf), "%s/%s", c->logdir, ti);
   c->log = fopen(buf, "a+");
   return !!c->log;
}

void
logging_contact_file_close(Contact *c)
{
   if (c->log) fclose(c->log);
   c->log = NULL;
}
