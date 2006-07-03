#include "dEvian.h"

#ifdef HAVE_RSS

static void *_create_data(E_Config_Dialog *cfd);
static void _fill_data(Rss_Doc *doc, E_Config_Dialog_Data *cfdata);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

struct _E_Config_Dialog_Data
{
   /* basic */
   char *url;
   /* advanced */
   char *name;
   char *description;
   int w_name;
   int w_description;
};


/* PUBLIC FUNCTIONS */

E_Config_Dialog *DEVIANF(config_dialog_rss) (Rss_Doc *doc, DEVIANN *devian)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;
   char buf[4096];

   /* if was alreay open, return */
   if (devian->dialog_conf_rss)
     if (!e_object_is_del(E_OBJECT(devian->dialog_conf_rss)))
       if (e_object_ref_get(E_OBJECT(devian->dialog_conf_rss)) > 0)
         return NULL;

   v = E_NEW(E_Config_Dialog_View, 1);

   /* methods */
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = _advanced_apply_data;
   v->advanced.create_widgets = _advanced_create_widgets;

   snprintf(buf, sizeof(buf), "%s/module.eap", e_module_dir_get(DEVIANM->module));
   cfd = e_config_dialog_new(DEVIANM->container, _(MODULE_NAME " Module Configuration"), buf, 0, v, doc);

   e_object_ref(E_OBJECT(cfd));

   return cfd;
}

void DEVIANF(config_dialog_rss_shutdown) (DEVIANN *devian)
{
   if (devian->dialog_conf_rss)
     {
        while (e_object_unref(E_OBJECT(devian->dialog_conf_rss)) > 0);
        devian->dialog_conf_rss = NULL;
     }
}


/* PRIVATE FUNCTIONS */

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   Rss_Doc *doc;

   doc = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   cfdata->url = NULL;
   cfdata->name = NULL;
   cfdata->description = NULL;

   _fill_data(doc, cfdata);

   return cfdata;
}

static void
_fill_data(Rss_Doc *doc, E_Config_Dialog_Data *cfdata)
{
   cfdata->url = strdup(doc->url);
   if (doc->name)
     cfdata->name = strdup(doc->name);
   else
     cfdata->name = strdup("");
   if (doc->description)
     cfdata->description = strdup(doc->description);
   else
     cfdata->description = strdup("");
   cfdata->w_name = doc->w_name;
   cfdata->w_description = doc->w_description;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Rss_Doc *doc;

   doc = cfd->data;
   /* if still new, free doc */
   if (doc->state == DATA_RSS_DOC_STATE_NEW)
     DEVIANF(data_rss_doc_free) (doc, 0, 1);

   E_FREE(cfdata->url);
   E_FREE(cfdata->name);
   E_FREE(cfdata->description);

   E_FREE(cfdata);
}

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Rss_Doc *doc;
   int change = 0;
   int need_poll = 0;

   doc = cfd->data;

   if (doc->url)
     {
        if (strcmp(doc->url, cfdata->url))
          {
             evas_stringshare_del(doc->url);
             doc->url = evas_stringshare_add(cfdata->url);
             need_poll = 1;
             change = 1;
          }
     }
   else
     {
        if (cfdata->url[0])
          {
             doc->url = evas_stringshare_add(cfdata->url);
             need_poll = 1;
             change = 1;
          }
     }

   if (doc->name)
     {
        if (strcmp(doc->name, cfdata->name))
          {
             evas_stringshare_del(doc->name);
             doc->name = evas_stringshare_add(cfdata->name);
             change = 1;
          }
     }
   else
     {
        if (cfdata->name[0])
          {
             doc->name = evas_stringshare_add(cfdata->name);
             change = 1;
          }
     }
   doc->w_name = cfdata->w_name;
   doc->w_description = cfdata->w_description;

   /* do update ? */
   if (change)
     {
        if (doc->state == DATA_RSS_DOC_STATE_NEW)
          {
             if (!DEVIANF(data_rss_doc_new) (doc, 1))
               return 1;
             DEVIANF(config_dialog_devian_rss_doc_update) (doc);
          }
        else
          {
             if (doc->state == DATA_RSS_DOC_STATE_USABLE)
               {
                  if (!DEVIANF(data_rss_doc_new) (doc, 0))
                    return 1;
                  DEVIANF(config_dialog_devian_rss_doc_update) (doc);
                  if (!e_config->cfgdlg_auto_apply && need_poll && doc->user)
                    DEVIANF(data_rss_poll) (doc->user, 1);
               }
          }
     }

   e_config_save_queue();

   return 1;
}

static int
_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Rss_Doc *doc;
   int change = 0;
   int need_poll = 0;

   doc = cfd->data;

   if (doc->url)
     {
        if (strcmp(doc->url, cfdata->url))
          {
             evas_stringshare_del(doc->url);
             doc->url = evas_stringshare_add(cfdata->url);
             need_poll = 1;
             change = 1;
          }
     }
   else
     {
        if (cfdata->url[0])
          {
             doc->url = evas_stringshare_add(cfdata->url);
             need_poll = 1;
             change = 1;
          }
     }

   if (doc->name)
     {
        if (strcmp(doc->name, cfdata->name))
          {
             evas_stringshare_del(doc->name);
             doc->name = evas_stringshare_add(cfdata->name);
             change = 1;
          }
     }
   else
     {
        if (cfdata->name[0])
          {
             doc->name = evas_stringshare_add(cfdata->name);
             change = 1;
          }
     }

   if (doc->description)
     {
        if (strcmp(doc->description, cfdata->description))
          {
             evas_stringshare_del(doc->description);
             doc->description = evas_stringshare_add(cfdata->description);
          }
     }
   else
     {
        if (cfdata->description[0])
          {
             doc->description = evas_stringshare_add(cfdata->description);
          }
     }
   doc->w_name = cfdata->w_name;
   doc->w_description = cfdata->w_description;

   /* do update ? */
   if (change)
     {
        if (doc->state == DATA_RSS_DOC_STATE_NEW)
          {
             if (!DEVIANF(data_rss_doc_new) (doc, 1))
               return 1;
             DEVIANF(config_dialog_devian_rss_doc_update) (doc);
          }
        else
          {
             if (doc->state == DATA_RSS_DOC_STATE_USABLE)
               {
                  if (!DEVIANF(data_rss_doc_new) (doc, 0))
                    return 1;
                  DEVIANF(config_dialog_devian_rss_doc_update) (doc);
                  if (need_poll && doc->user)
                    DEVIANF(data_rss_poll) (doc->user, 1);
               }
          }
     }

   e_config_save_queue();

   return 1;
}

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;
   Rss_Doc *doc;

   doc = cfd->data;

   o = e_widget_table_add(evas, 0);
   of = e_widget_frametable_add(evas, _("Rss Document"), 0);

   ob = e_widget_label_add(evas, _("Name"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_entry_add(evas, &(cfdata->name));
   e_widget_min_size_set(ob, 200, 1);
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, _("Url"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_entry_add(evas, &(cfdata->url));
   e_widget_min_size_set(ob, 200, 1);
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 1, 1, 1);

   e_widget_table_object_append(o, of, 0, 0, 1, 1, 1, 1, 1, 1);

   return o;
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;
   Rss_Doc *doc;

   doc = cfd->data;

   o = e_widget_table_add(evas, 0);
   of = e_widget_frametable_add(evas, _("Rss Document"), 0);

   ob = e_widget_label_add(evas, _("Name"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_entry_add(evas, &(cfdata->name));
   e_widget_min_size_set(ob, 200, 1);
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);

   ob = e_widget_label_add(evas, _("Url"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_entry_add(evas, &(cfdata->url));
   e_widget_min_size_set(ob, 200, 1);
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 1, 1, 1);

   ob = e_widget_label_add(evas, _("Description"));
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_entry_add(evas, &(cfdata->description));
   e_widget_min_size_set(ob, 200, 1);
   e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Allow server to overwrite name"), &(cfdata->w_name));
   e_widget_frametable_object_append(of, ob, 0, 3, 2, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Allow server to overwrite description"), &(cfdata->w_description));
   e_widget_frametable_object_append(of, ob, 0, 4, 2, 1, 1, 1, 1, 1);

   e_widget_table_object_append(o, of, 0, 0, 1, 1, 1, 1, 1, 1);
   return o;
}
#endif
