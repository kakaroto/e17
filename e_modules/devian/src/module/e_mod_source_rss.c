#include "dEvian.h"

#ifdef HAVE_RSS

static int _cb_timer_rss_update(void *data);


/* PUBLIC FUNCTIONS */

int DEVIANF(source_rss_add) (DEVIANN *devian)
{
   Source_Rss *source;

   source = E_NEW(Source_Rss, 1);

   source->devian = devian;

   source->rss_feed = NULL;
   source->active_article = NULL;

   /* objects */
   source->obj0 = e_table_add(DEVIANM->container->bg_evas);
   e_table_homogenous_set(source->obj0, 1);
   source->obj1 = e_table_add(DEVIANM->container->bg_evas);
   e_table_homogenous_set(source->obj1, 1);

   /* set new name for devian */
   DEVIANF(devian_set_id) (devian, SOURCE_RSS, NULL);

   /* create rss feed */
   if (!DEVIANF(data_rss_new) (source))
      return 0;

   /* provide declarations */
   devian->source_info.provide_double_buf = 1;
   devian->source_info.provide_previous = 0;
   devian->source_info.provide_set_bg = 0;
   devian->source_info.allow_info_panel = 1;

   /* actions */
   devian->source_func.timer_change = DEVIANF(source_rss_timer_change);
   devian->source_func.refresh = DEVIANF(source_rss_change);
   devian->source_func.set_bg = DEVIANF(source_idle_set_bg);
   devian->source_func.viewer = DEVIANF(source_rss_viewer);
   devian->source_func.gui_update = DEVIANF(data_rss_gui_update);

   devian->conf->source_type = SOURCE_RSS;
   devian->size_policy = SIZE_POLICY_USER;
   devian->source = source;

   /* timer to update rss feed */
   if (devian->conf->rss_timer_active)
      DEVIANF(source_rss_timer_change) (devian, 1, 0);
   else
      DEVIANF(source_rss_timer_change) (devian, 0, 0);

   return 1;
}

void DEVIANF(source_rss_del) (Source_Rss *source)
{
   DEVIANN *devian;

   devian = source->devian;

   if (source->rss_feed)
      DEVIANF(data_rss_del) (source->rss_feed);

   evas_object_del(source->obj0);
   evas_object_del(source->obj1);

   if (source->timer)
      ecore_timer_del(source->timer);

   E_FREE(source);
}

int DEVIANF(source_rss_change) (DEVIANN *devian, int option)
{
   Source_Rss *source;

   source = devian->source;

   if (devian->conf->rss_doc)
     {
        if (!DEVIANF(data_rss_poll) (source->rss_feed, 1))
           return 0;
     }

   return 1;
}

int DEVIANF(source_rss_viewer) (DEVIANN *devian)
{
   Source_Rss *source;
   const char *url;
   char buf[4096];

   source = devian->source;

   if (!source->devian->conf->rss_doc)
      return 0;

   url = source->devian->conf->rss_doc->link;

   if (!url)
     {
        snprintf(buf, sizeof(buf),
                 _("<hilight>Unable to open link</hilight><br><br>"
                   "Feed '%s' has no link associated !"), source->devian->conf->rss_doc->name);
        e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
        return 0;
     }

   if (ecore_file_app_installed(DEVIANM->conf->viewer_http))
     {
        Ecore_Exe *exe;

        snprintf(buf, 4096, "%s %s", DEVIANM->conf->viewer_http, url);
        DSOURCE(("Viewer: %s", buf));
        exe = ecore_exe_pipe_run(buf, ECORE_EXE_USE_SH, NULL);
        if (exe > 0)
           ecore_exe_free(exe);
     }
   else
     {
        snprintf(buf, sizeof(buf),
                 _("<hilight>Viewer %s not found !</hilight><br><br>"
                   "You can change the viewer for http in DEVIANN configuration panel (Advanced view)"),
                 DEVIANM->conf->viewer_http);
        e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
        return 0;
     }

   return 1;
}

int DEVIANF(source_rss_evas_object_get) (DEVIANN *devian, Evas_Object **rss0, Evas_Object **rss1)
{
   Source_Rss *source;

   source = (Source_Rss *)devian->source;

   if ((!source->obj0) && (!source->obj1))
      return 0;

   if (source->obj0)
      *rss0 = source->obj0;
   else
      *rss0 = NULL;
   if (source->obj1)
      *rss1 = source->obj1;
   else
      *rss1 = NULL;

   return 1;
}

int DEVIANF(source_rss_original_size_get) (Source_Rss *source, int part, int *w, int *h)
{
   if (!source)
      return 0;

   if (part == -1)
     {
        if ((part = DEVIANF(container_edje_part_get) (source->devian)) == -1)
           return 0;
     }

   if (!part && source->obj0)
     {
        *w = 300;
        *h = source->devian->conf->rss_nb_items * 15;
        //e_table_min_size_get(source->obj0, w, h);
        return 1;
     }

   if (part && source->obj1)
     {
        *w = 300;
        *h = source->devian->conf->rss_nb_items * 15;
        //e_table_min_size_get(source->obj1, w, h);
        return 1;
     }

   return 0;
}

void DEVIANF(source_rss_dialog_infos) (Source_Rss *source)
{
   E_Dialog *dia;
   Rss_Doc *doc;
   char buf[4096];

   dia = e_dialog_new(DEVIANM->container);
   if (!dia)
      return;
   doc = source->devian->conf->rss_doc;
   if (!doc)
      return;

   snprintf(buf, 4096,
            "<hilight>Informations on %s</hilight><br><br>"
            "<underline=on underline_color=#000>Rss url :</> %s<br>"
            "<underline=on underline_color=#000>Version :</> %2.2f<br><br>"
            "<underline=on underline_color=#000>Host :</> %s<br>"
            "<underline=on underline_color=#000>Web site associated :</> %s<br><br>"
            "<underline=on underline_color=#000>Description :</><br>%s",
            doc->name, doc->url, doc->version, doc->host, doc->link, doc->description);
   e_dialog_title_set(dia, MODULE_NAME " Module - Picture Informations");
   e_dialog_icon_set(dia, "enlightenment/e", 64);
   e_dialog_text_set(dia, buf);
   e_dialog_button_add(dia, _("Ok"), NULL, NULL, NULL);
   e_win_centered_set(dia->win, 1);
   e_dialog_show(dia);
}

char *DEVIANF(source_rss_name_get) (Source_Rss *source)
{
   if (!source->devian->conf->rss_doc)
      return NULL;

   return (char *)source->devian->conf->rss_doc->name;
}

char *DEVIANF(source_rss_infos_get) (Source_Rss *source)
{
   char buf[8192];

   if (source->active_article)
     {
        Rss_Article *article;

        article = source->active_article;

        /* description and date about the selected article */
        snprintf(buf, sizeof(buf), "<underline=on underline_color=#000>%s<br>%s</><br><br>%s<br><br><u\
nderline=on underline_color=#000>Link: %s</>", article->date, article->title, article->description, article->url);
     }
   else
     {
        Rss_Doc *doc;

        doc = source->devian->conf->rss_doc;
        if (doc)
          {
             /* informations about current rss document */
             snprintf(buf, sizeof(buf),
                      "<underline=on underline_color=#000>%s</><br><br>"
                      "<underline=on underline_color=#000>Rss url :</> %s<br>"
                      "<underline=on underline_color=#000>Description :</><br>%s", doc->name, doc->url, doc->description);
          }
        else
          {
             snprintf(buf, sizeof(buf), "No rss document selected");
          }
     }

   return strdup(buf);
}

void DEVIANF(source_rss_timer_change) (DEVIANN *devian, int active, int time)
{
   Source_Rss *source;

   source = devian->source;

   if (time && (time < SOURCE_RSS_UPDATE_RATE_MIN))
      return;

   devian->conf->rss_timer_active = active;
   if (!time)
      time = source->devian->conf->rss_timer_s;
   else
      devian->conf->rss_timer_s = time;

   e_config_save_queue();

   if (!active)
     {
        devian->source_info.paused = 1;
        devian->container_func.update_actions(devian);
        if (source->timer)
          {
             ecore_timer_del(source->timer);
             source->timer = NULL;
          }
        return;
     }
   devian->source_info.paused = 0;
   devian->container_func.update_actions(devian);

   if (source->timer)
      ecore_timer_del(source->timer);
   source->timer = ecore_timer_add(time, _cb_timer_rss_update, source);
}


/* PRIVATE FUNCTIONS */

static int
_cb_timer_rss_update(void *data)
{
   Source_Rss *source;

   source = (Source_Rss *)data;

   DEVIANF(source_rss_change) (source->devian, 0);

   return 1;
}
#endif
