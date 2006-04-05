#include "dEvian.h"

static void *_create_data(E_Config_Dialog *cfd);
static void _fill_data(E_Config_Dialog_Data *cfdata);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static int _basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static int _advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static Evas_Object *_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);

#ifdef HAVE_RSS
static void _ilist_rss_docs_append(E_Config_Dialog *cfd, Evas_Object *il, Evas_List *list);
static void _ilist_cb_rss_doc_selected(void *data);
static void _cb_button_rss_doc_add(void *data, void *data2);
static void _cb_button_rss_doc_edit(void *data, void *data2);
static void _cb_button_rss_doc_del(void *data, void *data2);
#endif

struct _E_Config_Dialog_Data
{
   DEVIAN_CONF *d_conf;
   Container_Box *c_box;
#ifdef HAVE_PICTURE
   Source_Picture *s_picture;
#endif
#ifdef HAVE_RSS
   Source_Rss *s_rss;
#endif
#ifdef HAVE_FILE
   Source_File *s_file;
#endif
  /*- BASIC -*/
   int container_box_size;
   int container_box_animation;
   int container_box_infos_show;
   int container_box_allow_overlap;
   int container_box_alpha;
#ifdef HAVE_RSS
   Evas_Object *sources_rss_docs_ilist;
   Evas_Object *source_rss_tb;
   Rss_Doc *source_rss_doc;     /* Pointer to sources_rss_docs list in main_conf
                                 * (the one selected) */
   char *source_rss_url;        /* Copy of source_rss_doc url
                                 * Used by ilist */
   int source_rss_nb_items;
   int source_rss_popup_news;
#endif
#ifdef HAVE_FILE
   char *source_file_path;
#endif
  /*- ADVANCED -*/
   int container_box_speed;
   int container_box_auto_resize;
   int container_box_infos_pos;
#ifdef HAVE_PICTURE
   int source_picture_timer;
   int source_picture_timer_yn;
#endif
#ifdef HAVE_RSS
   int source_rss_timer;
   int source_rss_timer_yn;
   int source_rss_reverse;
#endif
#ifdef HAVE_FILE
   int source_file_auto_scroll;
#endif
};

/* PUBLIC FUNCTIONS */

E_Config_Dialog *DEVIANF(config_dialog_devian) (E_Container *con, DEVIANN *devian)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;

   /* If already open, return */
   if (devian->dialog_conf)
      if (!e_object_is_del(E_OBJECT(devian->dialog_conf)))
         if (e_object_ref_get(E_OBJECT(devian->dialog_conf)) > 0)
            return NULL;

   v = E_NEW(E_Config_Dialog_View, 1);

   /* methods */
   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.apply_cfdata = _basic_apply_data;
   v->basic.create_widgets = _basic_create_widgets;
   v->advanced.apply_cfdata = _advanced_apply_data;
   v->advanced.create_widgets = _advanced_create_widgets;
   /* create config diaolg for NULL object/data */
   cfd = e_config_dialog_new(con, _(MODULE_NAME " Module Configuration"), NULL, 0, v, devian);

   e_object_ref(E_OBJECT(cfd));

   return cfd;
}

void DEVIANF(config_dialog_devian_shutdown) (DEVIANN *devian)
{
#ifdef HAVE_RSS
   DEVIANF(config_dialog_rss_shutdown) (devian);
#endif

   if (devian->dialog_conf)
     {
        while (e_object_unref(E_OBJECT(devian->dialog_conf)) > 0);
        devian->dialog_conf = NULL;
     }
}

#ifdef HAVE_RSS
void DEVIANF(config_dialog_devian_rss_doc_update) (Rss_Doc *doc)
{
   DEVIANN *devian;
   Evas_List *l;
   E_Config_Dialog_Data *cfdata;

   for (l = DEVIANM->devians; l; l = evas_list_next(l))
     {
        devian = evas_list_data(l);
        if ((devian->conf->source_type == SOURCE_RSS) && (devian->dialog_conf))
          {
             //... TODO: Remove and add only the item
             //if (doc) nanana, CAN BE NULL -> refr all
             cfdata = devian->dialog_conf->cfdata;
             _ilist_rss_docs_append(devian->dialog_conf, cfdata->sources_rss_docs_ilist, DEVIANM->conf->sources_rss_docs);
          }
     }
}
#endif

/* PRIVATE FUNCTIONS */

/**--DATA--**/

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;
   DEVIANN *devian;

   devian = cfd->data;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   cfdata->d_conf = devian->conf;
   cfdata->c_box = NULL;
#ifdef HAVE_PICTURE
   cfdata->s_picture = NULL;
#endif
#ifdef HAVE_RSS
   cfdata->s_rss = NULL;
#endif
#ifdef HAVE_FILE
   cfdata->s_file = NULL;
#endif

#ifdef HAVE_RSS
   cfdata->source_rss_doc = NULL;
   cfdata->source_rss_url = NULL;
#endif

   if (devian->conf->container_type == CONTAINER_BOX)
      cfdata->c_box = devian->container;

#ifdef HAVE_PICTURE
   if (devian->conf->source_type == SOURCE_PICTURE)
      cfdata->s_picture = devian->source;
   else
     {
#endif
#ifdef HAVE_RSS
        if (devian->conf->source_type == SOURCE_RSS)
           cfdata->s_rss = devian->source;
        else
          {
#endif
#ifdef HAVE_FILE
             if (devian->conf->source_type == SOURCE_FILE)
                cfdata->s_file = devian->source;
#endif
#ifdef HAVE_RSS
          }
#endif
#ifdef HAVE_PICTURE
     }
#endif

   _fill_data(cfdata);

   cfd->cfdata = (E_Config_Dialog_Data *)cfdata;
   return cfdata;
}

static void
_fill_data(E_Config_Dialog_Data *cfdata)
{
   DEVIAN_CONF *d_conf;

   d_conf = cfdata->d_conf;

   if (cfdata->c_box)
     {
        cfdata->container_box_size = d_conf->box_max_size_source;
        cfdata->container_box_auto_resize = d_conf->box_auto_resize;
        cfdata->container_box_animation = d_conf->box_anim;
        cfdata->container_box_infos_show = d_conf->box_infos_show;
        cfdata->container_box_allow_overlap = d_conf->box_allow_overlap;
        cfdata->container_box_alpha = cfdata->c_box->alpha;
        cfdata->container_box_infos_pos = d_conf->box_infos_pos;
        cfdata->container_box_speed = d_conf->box_speed;
     }

#ifdef HAVE_PICTURE
   if (cfdata->s_picture)
     {
        cfdata->source_picture_timer_yn = d_conf->picture_timer_active;
        cfdata->source_picture_timer = d_conf->picture_timer_s;
     }
   else
     {
#endif
#ifdef HAVE_RSS
        if (cfdata->s_rss)
          {
             if (d_conf->rss_doc)
               {
                  cfdata->source_rss_url = strdup(d_conf->rss_doc->url);
                  cfdata->source_rss_doc = d_conf->rss_doc;
               }
             else
               {
                  cfdata->source_rss_url = strdup("");
                  cfdata->source_rss_doc = NULL;
               }
             cfdata->source_rss_nb_items = d_conf->rss_nb_items;
             cfdata->source_rss_timer_yn = d_conf->rss_timer_active;
             cfdata->source_rss_timer = d_conf->rss_timer_s / 60;
             cfdata->source_rss_popup_news = d_conf->rss_popup_news;
             cfdata->source_rss_reverse = d_conf->rss_reverse;
          }
        else
          {
#endif
#ifdef HAVE_FILE
             if (cfdata->s_file)
               {
                  cfdata->source_file_path = strdup(d_conf->file_path);
                  cfdata->source_file_auto_scroll = d_conf->file_auto_scroll;
               }
#endif
#ifdef HAVE_RSS
          }
#endif
#ifdef HAVE_PICTURE
     }
#endif
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
#ifdef HAVE_RSS
   E_FREE(cfdata->source_rss_url);
#endif
#ifdef HAVE_FILE
   E_FREE(cfdata->source_file_path);
#endif

   free(cfdata);
}

/**--APPLY--**/

static int
_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   DEVIANN *devian;

   devian = cfd->data;

   if (cfdata->c_box)
     {
        if (devian->conf->box_max_size_source != cfdata->container_box_size)
          {
             devian->conf->box_max_size_source = cfdata->container_box_size;
             cfdata->c_box->max_size = devian->conf->box_max_size_source + cfdata->c_box->theme_border_h;
             DEVIANF(container_box_resize_auto) (devian);
          }
        if (devian->conf->box_anim != cfdata->container_box_animation)
          {
             devian->conf->box_anim = cfdata->container_box_animation;
             DEVIANF(container_box_animation_start) (devian->container, devian->conf->box_anim);
          }
        if (devian->conf->box_infos_show != cfdata->container_box_infos_show)
          {
             DEVIANF(container_box_infos_text_change_set) (devian->container, cfdata->container_box_infos_show);
          }
        if (devian->conf->box_allow_overlap != cfdata->container_box_allow_overlap)
          {
             devian->conf->box_allow_overlap = cfdata->container_box_allow_overlap;
             DEVIANF(container_box_gadman_policy_update) (devian->container);
          }
        if (cfdata->c_box->alpha != cfdata->container_box_alpha)
           devian->container_func.alpha_set(devian->container, cfdata->container_box_alpha);
     }

#ifdef HAVE_RSS
   if (cfdata->s_rss)
     {
        if (devian->conf->rss_doc != cfdata->source_rss_doc)
          {
             DEVIANF(data_rss_doc_set_new) (cfdata->s_rss->rss_feed, cfdata->source_rss_doc, NULL);
          }
        if (devian->conf->rss_nb_items != cfdata->source_rss_nb_items)
          {
             devian->conf->rss_nb_items = cfdata->source_rss_nb_items;
             DEVIANF(data_rss_poll) (cfdata->s_rss->rss_feed, 0);
          }
        devian->conf->rss_popup_news = cfdata->source_rss_popup_news;
     }
#endif
#ifdef HAVE_FILE
   if (cfdata->s_file)
     {
        if (!e_config->cfgdlg_auto_apply)
          {
             if (strcmp(devian->conf->file_path, cfdata->source_file_path))
               {
                  evas_stringshare_del(devian->conf->file_path);
                  devian->conf->file_path = evas_stringshare_add(cfdata->source_file_path);
                  devian->source_func.refresh(devian, 0);
               }
          }
     }
#endif

   e_config_save_queue();

   return 1;
}

static int
_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   DEVIANN *devian;

   devian = cfd->data;

   if (cfdata->c_box)
     {
        if (devian->conf->box_max_size_source != cfdata->container_box_size)
          {
             DCONTAINER(("CALLING MAX SIZE"));
             devian->conf->box_max_size_source = cfdata->container_box_size;
             cfdata->c_box->max_size = devian->conf->box_max_size_source + cfdata->c_box->theme_border_h;
             DEVIANF(container_box_resize_auto) (devian);
          }
        if (devian->conf->box_auto_resize != cfdata->container_box_auto_resize)
          {
             DCONTAINER(("CALLING RESIZE AUTO"));
             devian->conf->box_auto_resize = cfdata->container_box_auto_resize;
             DEVIANF(container_box_resize_auto) (devian);
          }
        if (devian->conf->box_anim != cfdata->container_box_animation)
          {
             DCONTAINER(("CALLING ANIM START"));
             devian->conf->box_anim = cfdata->container_box_animation;
             DEVIANF(container_box_animation_start) (devian->container, devian->conf->box_anim);
          }
        if (devian->conf->box_infos_show != cfdata->container_box_infos_show)
          {
             DEVIANF(container_box_infos_text_change_set) (devian->container, cfdata->container_box_infos_show);
          }
        if (devian->conf->box_infos_pos != cfdata->container_box_infos_pos)
          {
             devian->conf->box_infos_pos = cfdata->container_box_infos_pos;
             DEVIANF(container_box_infos_pos_set) (devian->container);
          }
        devian->conf->box_speed = cfdata->container_box_speed;
        if (devian->conf->box_allow_overlap != cfdata->container_box_allow_overlap)
          {
             devian->conf->box_allow_overlap = cfdata->container_box_allow_overlap;
             DEVIANF(container_box_gadman_policy_update) (devian->container);
          }
        if (cfdata->c_box->alpha != cfdata->container_box_alpha)
           devian->container_func.alpha_set(devian->container, cfdata->container_box_alpha);
     }

   /* Advanced only */

#ifdef HAVE_PICTURE
   if (cfdata->s_picture)
     {
        if (devian->conf->picture_timer_active != cfdata->source_picture_timer_yn)
           devian->source_func.timer_change(devian, cfdata->source_picture_timer_yn, cfdata->source_picture_timer);
        else if (devian->conf->picture_timer_s != cfdata->source_picture_timer)
           devian->source_func.timer_change(devian, devian->conf->picture_timer_active, cfdata->source_picture_timer);
     }
   else
     {
#endif
#ifdef HAVE_RSS
        if (cfdata->s_rss)
          {
             if (devian->conf->rss_doc != cfdata->source_rss_doc)
               {
                  DEVIANF(data_rss_doc_set_new) (cfdata->s_rss->rss_feed, cfdata->source_rss_doc, NULL);
               }
             if (devian->conf->rss_nb_items != cfdata->source_rss_nb_items)
               {
                  devian->conf->rss_nb_items = cfdata->source_rss_nb_items;
                  DEVIANF(data_rss_poll) (cfdata->s_rss->rss_feed, 0);
               }
             if (devian->conf->rss_timer_active != cfdata->source_rss_timer_yn)
                devian->source_func.timer_change(devian, cfdata->source_rss_timer_yn, cfdata->source_rss_timer * 60);
             else if (devian->conf->rss_timer_s != cfdata->source_rss_timer)
                devian->source_func.timer_change(devian, devian->conf->rss_timer_active, cfdata->source_rss_timer * 60);
             devian->conf->rss_popup_news = cfdata->source_rss_popup_news;
             if (devian->conf->rss_reverse != cfdata->source_rss_reverse)
               {
                  devian->conf->rss_reverse = cfdata->source_rss_reverse;
                  devian->source_func.gui_update(devian);
               }
          }
        else
          {
#endif
#ifdef HAVE_FILE
             if (cfdata->s_file)
               {
                  if (!e_config->cfgdlg_auto_apply)
                    {
                       if (strcmp(devian->conf->file_path, cfdata->source_file_path))
                         {
                            evas_stringshare_del(devian->conf->file_path);
                            devian->conf->file_path = evas_stringshare_add(cfdata->source_file_path);
                            devian->source_func.refresh(devian, 0);
                         }
                    }
                  devian->conf->file_auto_scroll = cfdata->source_file_auto_scroll;
               }
#endif
#ifdef HAVE_RSS
          }
#endif
#ifdef HAVE_PICTURE
     }
#endif

   e_config_save_queue();

   return 1;
}

/**--GUI--**/

static Evas_Object *
_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;
   E_Radio_Group *rg;

   o = e_widget_table_add(evas, 0);

   if (cfdata->c_box)
     {
        of = e_widget_frametable_add(evas, _("Box"), 0);
        ob = e_widget_check_add(evas, _("Allow windows to overlap the box"), &(cfdata->container_box_allow_overlap));
        e_widget_frametable_object_append(of, ob, 0, 0, 2, 1, 1, 1, 1, 1);
        ob = e_widget_label_add(evas, _("Size of content"));
        e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
        ob = e_widget_slider_add(evas, 1, 0, _("%1.0f pix"), (float)CONTAINER_BOX_SIZE_MIN, (float)CONTAINER_BOX_SIZE_MAX, 1.0, 0,
                                 NULL, &(cfdata->container_box_size), 100);
        e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 0, 1, 0);
        ob = e_widget_label_add(evas, _("Alpha"));
        e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 1, 1, 1);
        ob = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 10.0, 255.0, 1.0, 0, NULL, &(cfdata->container_box_alpha), 100);
        e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 1, 0, 1, 0);
        ob = e_widget_label_add(evas, _("Animation"));
        e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 1, 1, 1);
        rg = e_widget_radio_group_new(&(cfdata->container_box_animation));
        ob = e_widget_radio_add(evas, _("None"), CONTAINER_BOX_ANIM_NO, rg);
        e_widget_frametable_object_append(of, ob, 0, 4, 1, 1, 1, 1, 1, 1);
        ob = e_widget_radio_add(evas, _("Line"), CONTAINER_BOX_ANIM_LINE, rg);
        e_widget_frametable_object_append(of, ob, 1, 4, 1, 1, 1, 1, 1, 1);
        ob = e_widget_radio_add(evas, _("Gouloum"), CONTAINER_BOX_ANIM_GOULOUM, rg);
        e_widget_frametable_object_append(of, ob, 0, 5, 1, 1, 1, 1, 1, 1);
        ob = e_widget_radio_add(evas, _("Ghost"), CONTAINER_BOX_ANIM_GHOST, rg);
        e_widget_frametable_object_append(of, ob, 1, 5, 1, 1, 1, 1, 1, 1);
        ob = e_widget_check_add(evas, _("Show information panel"), &(cfdata->container_box_infos_show));
        e_widget_frametable_object_append(of, ob, 0, 6, 2, 1, 1, 1, 1, 1);
        e_widget_table_object_append(o, of, 0, 0, 1, 1, 1, 1, 1, 1);
     }

#ifdef HAVE_PICTURE
   if (cfdata->s_picture)
     {

     }
   else
     {
#endif
#ifdef HAVE_RSS
        if (cfdata->s_rss)
          {
             of = e_widget_frametable_add(evas, _("Rss"), 0);

             ob = e_widget_textblock_add(evas);
             cfdata->source_rss_tb = ob;
             e_widget_min_size_set(ob, 100, 55);
             e_widget_frametable_object_append(of, ob, 0, 0, 3, 1, 1, 1, 1, 1);
             ob = e_widget_ilist_add(evas, 0, 0, &(cfdata->source_rss_url));
             cfdata->sources_rss_docs_ilist = ob;
             e_widget_ilist_selector_set(ob, 1);
             e_widget_min_size_set(ob, 100, 130);
             _ilist_rss_docs_append(cfd, ob, DEVIANM->conf->sources_rss_docs);
             e_widget_ilist_go(ob);
             e_widget_frametable_object_append(of, ob, 0, 1, 3, 1, 1, 1, 1, 1);
             ob = e_widget_button_add(evas, "Add", NULL, _cb_button_rss_doc_add, cfd, NULL);
             e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 1, 1, 1);
             ob = e_widget_button_add(evas, "Edit", NULL, _cb_button_rss_doc_edit, cfd, NULL);
             e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 1, 1, 1, 1);
             ob = e_widget_button_add(evas, "Del", NULL, _cb_button_rss_doc_del, cfd, NULL);
             e_widget_frametable_object_append(of, ob, 2, 2, 1, 1, 1, 1, 1, 1);

             ob = e_widget_label_add(evas, _("Nb items"));
             e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 1, 1, 1);
             ob = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 1.0, 20.0, 1.0, 0, NULL, &(cfdata->source_rss_nb_items), 90);
             e_widget_frametable_object_append(of, ob, 1, 3, 2, 1, 1, 0, 1, 0);
             if (DEVIANM->conf->sources_rss_popup_news)
                ob = e_widget_check_add(evas, _("News popup"), &(cfdata->source_rss_popup_news));
             else
                ob = e_widget_check_add(evas, _("News popup [desactivated]"), &(cfdata->source_rss_popup_news));
             e_widget_frametable_object_append(of, ob, 0, 4, 2, 1, 1, 1, 1, 1);
             e_widget_table_object_append(o, of, 1, 0, 1, 1, 1, 1, 1, 1);
          }
        else
          {
#endif
#ifdef HAVE_FILE
             if (cfdata->s_file)
               {
                  of = e_widget_frametable_add(evas, _("File Log"), 0);
                  ob = e_widget_label_add(evas, _("Path"));
                  e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
                  ob = e_widget_entry_add(evas, &(cfdata->source_file_path));
                  e_widget_min_size_set(ob, 100, 1);
                  e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);
                  e_widget_table_object_append(o, of, 1, 0, 1, 1, 1, 1, 1, 1);
               }
#endif
#ifdef HAVE_RSS
          }
#endif
#ifdef HAVE_PICTURE
     }
#endif

   return o;
}

static Evas_Object *
_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   /* generate the core widget layout for an advanced dialog */
   Evas_Object *o, *of, *ob;
   E_Radio_Group *rg;

   o = e_widget_table_add(evas, 0);

   if (cfdata->c_box)
     {
        of = e_widget_frametable_add(evas, _("Box"), 0);
        ob = e_widget_check_add(evas, _("Allow windows to overlap the box"), &(cfdata->container_box_allow_overlap));
        e_widget_frametable_object_append(of, ob, 0, 0, 2, 1, 1, 1, 1, 1);
        ob = e_widget_label_add(evas, _("Size of content"));
        e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
        ob = e_widget_slider_add(evas, 1, 0, _("%1.0f pix"), (float)CONTAINER_BOX_SIZE_MIN, (float)CONTAINER_BOX_SIZE_MAX, 1.0, 0,
                                 NULL, &(cfdata->container_box_size), 100);
        e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 0, 1, 0);
        ob = e_widget_label_add(evas, _("Alpha"));
        e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 1, 1, 1);
        ob = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 10.0, 255.0, 1.0, 0, NULL, &(cfdata->container_box_alpha), 100);
        e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 1, 0, 1, 0);
        ob = e_widget_label_add(evas, _("Animation"));
        e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 1, 1, 1);
        rg = e_widget_radio_group_new(&(cfdata->container_box_animation));
        ob = e_widget_radio_add(evas, _("None"), CONTAINER_BOX_ANIM_NO, rg);
        e_widget_frametable_object_append(of, ob, 0, 4, 1, 1, 1, 1, 1, 1);
        ob = e_widget_radio_add(evas, _("Line"), CONTAINER_BOX_ANIM_LINE, rg);
        e_widget_frametable_object_append(of, ob, 1, 4, 1, 1, 1, 1, 1, 1);
        ob = e_widget_radio_add(evas, _("Gouloum"), CONTAINER_BOX_ANIM_GOULOUM, rg);
        e_widget_frametable_object_append(of, ob, 0, 5, 1, 1, 1, 1, 1, 1);
        ob = e_widget_radio_add(evas, _("Ghost"), CONTAINER_BOX_ANIM_GHOST, rg);
        e_widget_frametable_object_append(of, ob, 1, 5, 1, 1, 1, 1, 1, 1);
        ob = e_widget_label_add(evas, _("Speed"));
        e_widget_frametable_object_append(of, ob, 0, 6, 1, 1, 1, 1, 1, 1);
        ob = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 1.0, 3.0, 1.0, 0, NULL, &(cfdata->container_box_speed), 100);
        e_widget_frametable_object_append(of, ob, 1, 6, 1, 1, 1, 0, 1, 0);
        ob = e_widget_check_add(evas, _("Box auto resize"), &(cfdata->container_box_auto_resize));
        e_widget_frametable_object_append(of, ob, 0, 7, 2, 1, 1, 1, 1, 1);
        ob = e_widget_check_add(evas, _("Show information panel"), &(cfdata->container_box_infos_show));
        e_widget_frametable_object_append(of, ob, 0, 8, 2, 1, 1, 1, 1, 1);
        ob = e_widget_label_add(evas, _("Information panel position"));
        e_widget_frametable_object_append(of, ob, 0, 9, 2, 1, 1, 1, 1, 1);
        ob = e_widget_slider_add(evas, 1, 0, _("Position no%1.0f"), 1.0, 4.0, 1.0, 0,
                                 NULL, &(cfdata->container_box_infos_pos), 100);
        e_widget_frametable_object_append(of, ob, 0, 10, 2, 1, 1, 0, 1, 0);
        e_widget_table_object_append(o, of, 0, 0, 1, 1, 1, 1, 1, 1);
     }

#ifdef HAVE_PICTURE
   if (cfdata->s_picture)
     {
        of = e_widget_frametable_add(evas, _("Picture"), 0);
        ob = e_widget_check_add(evas, _("Change pictures"), &(cfdata->source_picture_timer_yn));
        e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
        ob = e_widget_label_add(evas, _("Timer"));
        e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
        ob = e_widget_slider_add(evas, 1, 0, _("%1.0fs"), 3.0, 300.0, 5.0, 0, NULL, &(cfdata->source_picture_timer), 100);
        e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 0, 1, 0);
        e_widget_table_object_append(o, of, 1, 0, 1, 1, 1, 1, 1, 1);
     }
   else
     {
#endif
#ifdef HAVE_RSS
        if (cfdata->s_rss)
          {
             of = e_widget_frametable_add(evas, _("Rss"), 0);

             ob = e_widget_textblock_add(evas);
             cfdata->source_rss_tb = ob;
             e_widget_min_size_set(ob, 100, 55);
             e_widget_frametable_object_append(of, ob, 0, 0, 3, 1, 1, 1, 1, 1);
             ob = e_widget_ilist_add(evas, 0, 0, &(cfdata->source_rss_url));
             cfdata->sources_rss_docs_ilist = ob;
             e_widget_ilist_selector_set(ob, 1);
             e_widget_min_size_set(ob, 100, 100);
             _ilist_rss_docs_append(cfd, ob, DEVIANM->conf->sources_rss_docs);
             e_widget_ilist_go(ob);
             e_widget_frametable_object_append(of, ob, 0, 1, 3, 1, 1, 1, 1, 1);
             ob = e_widget_button_add(evas, "Add", NULL, _cb_button_rss_doc_add, cfd, NULL);
             e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 1, 1, 1);
             ob = e_widget_button_add(evas, "Edit", NULL, _cb_button_rss_doc_edit, cfd, NULL);
             e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 1, 1, 1, 1);
             ob = e_widget_button_add(evas, "Del", NULL, _cb_button_rss_doc_del, cfd, NULL);
             e_widget_frametable_object_append(of, ob, 2, 2, 1, 1, 1, 1, 1, 1);

             ob = e_widget_label_add(evas, _("Nb items"));
             e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 1, 1, 1);
             ob = e_widget_slider_add(evas, 1, 0, _("%1.0f"), 1.0, 20.0, 1.0, 0, NULL, &(cfdata->source_rss_nb_items), 90);
             e_widget_frametable_object_append(of, ob, 1, 3, 2, 1, 1, 0, 1, 0);
             if (DEVIANM->conf->sources_rss_popup_news)
                ob = e_widget_check_add(evas, _("News popup"), &(cfdata->source_rss_popup_news));
             else
                ob = e_widget_check_add(evas, _("News popup [desactivated]"), &(cfdata->source_rss_popup_news));
             e_widget_frametable_object_append(of, ob, 0, 4, 2, 1, 1, 1, 1, 1);
             ob = e_widget_check_add(evas, _("Update RSS"), &(cfdata->source_rss_timer_yn));
             e_widget_frametable_object_append(of, ob, 0, 5, 2, 1, 1, 1, 1, 1);
             ob = e_widget_label_add(evas, _("Update timer"));
             e_widget_frametable_object_append(of, ob, 0, 6, 1, 1, 1, 1, 1, 1);
             ob =
                e_widget_slider_add(evas, 1, 0, _("%1.0f min"), (float)SOURCE_RSS_UPDATE_RATE_MIN / 60,
                                    (float)SOURCE_RSS_UPDATE_RATE_MAX / 60, 1.0, 0, NULL, &(cfdata->source_rss_timer), 90);
             e_widget_frametable_object_append(of, ob, 1, 6, 2, 1, 1, 0, 1, 0);
             ob = e_widget_check_add(evas, _("Show the feed in reverse order"), &(cfdata->source_rss_reverse));
             e_widget_frametable_object_append(of, ob, 0, 7, 2, 1, 1, 1, 1, 1);
             e_widget_table_object_append(o, of, 1, 0, 1, 1, 1, 1, 1, 1);
          }
        else
          {
#endif
#ifdef HAVE_FILE
             if (cfdata->s_file)
               {
                  of = e_widget_frametable_add(evas, _("File Log"), 0);
                  ob = e_widget_label_add(evas, _("Path"));
                  e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
                  ob = e_widget_entry_add(evas, &(cfdata->source_file_path));
                  e_widget_min_size_set(ob, 100, 1);
                  e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);
                  ob = e_widget_check_add(evas, _("Auto scroll when updates"), &(cfdata->source_file_auto_scroll));
                  e_widget_frametable_object_append(of, ob, 0, 1, 2, 1, 1, 1, 1, 1);
                  e_widget_table_object_append(o, of, 1, 0, 1, 1, 1, 1, 1, 1);
               }
#endif
#ifdef HAVE_RSS
          }
#endif
#ifdef HAVE_PICTURE
     }
#endif

   return o;
}

#ifdef HAVE_RSS
static void
_ilist_rss_docs_append(E_Config_Dialog *cfd, Evas_Object *il, Evas_List *list)
{
   Evas_List *l;
   Rss_Doc *odoc;
   E_Config_Dialog_Data *cfdata;
   int i = 0;

   cfdata = cfd->cfdata;

   e_widget_ilist_clear(il);

   for (l = list; l; l = evas_list_next(l))
     {
        odoc = evas_list_data(l);
        e_widget_ilist_append(il, NULL, (char *)odoc->name, _ilist_cb_rss_doc_selected, cfd, (char *)odoc->url);
        if (cfdata->d_conf->rss_url)
           if (!strcmp(odoc->url, cfdata->d_conf->rss_url))
             {
                e_widget_ilist_selected_set(il, i);
                _ilist_cb_rss_doc_selected(cfd);
             }
        i++;
     }
}

static void
_ilist_cb_rss_doc_selected(void *data)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_Data *cfdata;
   Rss_Doc *doc;
   char buf[4096];

   cfd = data;
   cfdata = cfd->cfdata;

   DMAIN(("Selected: %s", cfdata->source_rss_url));

   doc = DEVIANF(data_rss_doc_find_doc) ((const char *)cfdata->source_rss_url);
   if (doc)
     {
        /* Select the doc */
        cfdata->source_rss_doc = doc;

        /* Set it in panel infos tb */
        if (doc->description)
           snprintf(buf, sizeof(buf), "<underline=on underline_color=#000>%s</><br>%s", doc->url, doc->description);
        else
           snprintf(buf, sizeof(buf), "<underline=on underline_color=#000>%s</><br><i>No description</>", doc->url);
        e_widget_textblock_markup_set(cfdata->source_rss_tb, buf);
     }
}

static void
_cb_button_rss_doc_add(void *data, void *data2)
{
   E_Config_Dialog *cfd;
   Rss_Doc *doc;
   DEVIANN *devian;

   cfd = data;
   devian = cfd->data;
   doc = DEVIANF(data_rss_doc_new) (NULL, 0);

   if ((cfd = DEVIANF(config_dialog_rss) (doc, devian)))
      devian->dialog_conf_rss = cfd;
}

static void
_cb_button_rss_doc_edit(void *data, void *data2)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_Data *cfdata;
   DEVIANN *devian;
   Rss_Doc *doc;

   cfd = data;
   devian = cfd->data;
   cfdata = cfd->cfdata;
   doc = cfdata->source_rss_doc;

   if (!cfdata->source_rss_doc)
      return;

   if ((cfd = DEVIANF(config_dialog_rss) (doc, devian)))
      devian->dialog_conf_rss = cfd;
}

static void
_cb_button_rss_doc_del(void *data, void *data2)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_Data *cfdata;
   Rss_Doc *doc;

   cfd = data;
   cfdata = cfd->cfdata;
   doc = cfdata->source_rss_doc;

   if (!cfdata->source_rss_doc)
      return;

   if (!doc->user)
     {
        e_widget_ilist_clear(cfdata->sources_rss_docs_ilist);
        DEVIANM->conf->sources_rss_docs = evas_list_remove(DEVIANM->conf->sources_rss_docs, doc);
        DEVIANF(config_dialog_devian_rss_doc_update) (NULL);
        DEVIANF(data_rss_doc_free) (doc, 0, 1);
        e_config_save_queue();
     }
   else
     {
        char buf[4096];

        snprintf(buf, sizeof(buf),
                 _("<hilight>Rss '%s' is in use !</hilight><br><br>"
                   "You can't delete it while its in use.<br><br>" "Owner is %s"), doc->name, doc->user->source->devian->id);
        e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
     }
}
#endif
