#include "News.h"


/*
 * Public functions
 */

int
news_url_image_get(char *url, void (*func_cb) (News_Feed_Document *doc, int error))
{
   // TODO

   return 0;
}

int
news_util_ecanvas_geometry_get(int *canvas_w, int *canvas_h)
{
   E_Container *c;

   c = e_container_current_get(e_manager_current_get());
   evas_output_viewport_get(c->bg_evas,
                            NULL, NULL,
                            canvas_w, canvas_h);

   return 1;
}

int
news_util_datecmp(struct tm *t1, struct tm *t2)
{
   if (t1->tm_year != t2->tm_year)
     return (t1->tm_year - t2->tm_year);
   if (t1->tm_mon != t2->tm_mon)
     return (t1->tm_mon - t2->tm_mon);
   if (t1->tm_mday != t2->tm_mday)
     return (t1->tm_mday - t2->tm_mday);
   if (t1->tm_hour != t2->tm_hour)
     return (t1->tm_hour - t2->tm_hour);
   if (t1->tm_min != t2->tm_min)
     return (t1->tm_min - t2->tm_min);
   if (t1->tm_sec != t2->tm_sec)
     return (t1->tm_sec - t2->tm_sec);
   return 0;
}

int
news_util_browser_open(const char *url)
{
   News_Config *c = news->config;
   Ecore_Exe *exe;
   char cmd[4096];
   char buf[4096];

   if (!url) return 0;

   switch(c->browser.wich)
     {
     case NEWS_UTIL_BROWSER_FIREFOX:
        snprintf(cmd, sizeof(cmd), "%s", "firefox -new-window");
        break;
     case NEWS_UTIL_BROWSER_MOZILLA:
        snprintf(cmd, sizeof(cmd), "%s", "mozilla");
        break;
     case NEWS_UTIL_BROWSER_OPERA:
        snprintf(cmd, sizeof(cmd), "%s", "opera");
        break;
     case NEWS_UTIL_BROWSER_DILLO:
        snprintf(cmd, sizeof(cmd), "%s", "dillo");
        break;
     case NEWS_UTIL_BROWSER_OWN:
        snprintf(cmd, sizeof(cmd), "%s", c->browser.own);
        break;
     }

   DBROWSER(("Exec %s with %s command", url, cmd));
   snprintf(buf, sizeof(buf), "%s \"%s\"", cmd, url);
   exe = ecore_exe_pipe_run(buf, ECORE_EXE_USE_SH, NULL);
   if (exe > 0)
     ecore_exe_free(exe);
   else
     {
        news_util_message_error_show(_("<hilight>Error</hilight> when executing the command"
                                       "for youre <hilight>browser</hilight> :<br><br>"
                                       "%s"), cmd);
        return 0;
     }

   return 1;
}

Evas_List *
news_util_lang_detect(void)
{
   Evas_List *list = NULL;
   News_Feed_Lang *lang;
   const char *key, *name;

   lang = E_NEW(News_Feed_Lang, 1);
   lang->key = evas_stringshare_add("en");
   lang->name = evas_stringshare_add("English");
   list = evas_list_append(list, lang);

   //FIXME is that get from env correct ?
   key = getenv("LC_MESSAGES");
   if (!key) key = getenv("LANGUAGE");
   if (!key) key = getenv("LC_ALL");
   if (!key) key = getenv("LANG");
   if (key && strncmp(key, "en", 2) &&
       (name = news_feed_lang_name_get(key)))
     {
        lang = E_NEW(News_Feed_Lang, 1);
        lang->key = evas_stringshare_add(key);
        lang->name = evas_stringshare_add(name);
        list = evas_list_append(list, lang);
     }

   return list;
}

/*
 * Private functions
 *
 */
