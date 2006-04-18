#include "dEvian.h"

static void *_main_create_data(E_Config_Dialog *cfd);
static void _fill_data(E_Config_Dialog_Data *cfdata);
static void _main_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static int _main_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static int _main_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_main_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static Evas_Object *_main_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);

#ifdef HAVE_PICTURE
static void _cb_button_add_devian_source_picture(void *data, void *data2);
#endif
#ifdef HAVE_RSS
static void _cb_button_add_devian_source_rss(void *data, void *data2);
#endif
#ifdef HAVE_FILE
static void _cb_button_add_devian_source_file(void *data, void *data2);
#endif
static void _cb_button_theme_select(void *data, void *data2);

#ifdef HAVE_PICTURE
static void _cb_button_regen_picture_list(void *data, void *data2);
#endif

struct _E_Config_Dialog_Data
{
  /*- BASIC -*/
   char *viewer_http;
   char *viewer_image;
   char *viewer_file;
   int boxs_always_id;
   int boxs_always_name;
#ifdef HAVE_PICTURE
   char *sources_picture_data_import_dir;
   int sources_picture_data_import_recursive;
   int sources_picture_data_import_hidden;
#endif
#ifdef HAVE_RSS
   int sources_rss_popup_news;
#endif
#ifdef HAVE_FILE
   int sources_file_font_size;
#endif
  /*- ADVANCED -*/
   int boxs_nice_resize;
   int boxs_nice_trans;
   int boxs_default_max_size;
   int boxs_default_anim;
   int boxs_anim_ghost_timer;
#ifdef HAVE_PICTURE
   int data_picture_thumb_default_size;
   int sources_picture_set_bg_purge;
   int sources_picture_show_devian_pics;
#endif
#ifdef HAVE_RSS
   int sources_rss_timer_s;
   int sources_rss_popup_news_timer;
#endif
#ifdef HAVE_FILE
   int sources_file_nb_lines_ini;
   int sources_file_nb_lines_max;
#endif
};

/* PUBLIC FUNCTIONS */

E_Config_Dialog *DEVIANF(config_dialog_main) (E_Container *con)
{
   E_Config_Dialog *cfd;
   E_Config_Dialog_View *v;

   /* If already open, return */
   /* Only works if open via our menu, not config panel */
   if (DEVIANM->dialog_conf)
      if (!e_object_is_del(E_OBJECT(DEVIANM->dialog_conf)))
         if (e_object_ref_get(E_OBJECT(DEVIANM->dialog_conf)) > 0)
            return NULL;

   v = E_NEW(E_Config_Dialog_View, 1);

   /* methods */
   v->create_cfdata = _main_create_data;
   v->free_cfdata = _main_free_data;
   v->basic.apply_cfdata = _main_basic_apply_data;
   v->basic.create_widgets = _main_basic_create_widgets;
   v->advanced.apply_cfdata = _main_advanced_apply_data;
   v->advanced.create_widgets = _main_advanced_create_widgets;

   cfd = e_config_dialog_new(con, _(MODULE_NAME " Module Main Configuration"), NULL, 0, v, NULL);

   e_object_ref(E_OBJECT(cfd));

   /* To make work our system of ref, even if module configuration panel open it */
   DEVIANM->dialog_conf = cfd;

   return cfd;
}

void DEVIANF(config_dialog_main_shutdown) (void)
{
   DEVIANF(config_dialog_theme_shutdown) ();

   if (DEVIANM->dialog_conf)
     {
        while (e_object_unref(E_OBJECT(DEVIANM->dialog_conf)) > 0);
        DEVIANM->dialog_conf = NULL;
     }
}

/* PRIVATE FUNCTIONS */

/**--DATA--**/

static void *
_main_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata;

   cfdata = E_NEW(E_Config_Dialog_Data, 1);

   _fill_data(cfdata);

   cfd->cfdata = (E_Config_Dialog_Data *)cfdata;
   return cfdata;
}

static void
_fill_data(E_Config_Dialog_Data *cfdata)
{
   DEVIAN_MAIN_CONF *conf;

   conf = DEVIANM->conf;

   cfdata->viewer_http = strdup(conf->viewer_http);
   cfdata->viewer_image = strdup(conf->viewer_image);
   cfdata->viewer_file = strdup(conf->viewer_file);

#ifdef HAVE_PICTURE
   cfdata->sources_picture_data_import_dir = strdup(conf->sources_picture_data_import_dir);
   cfdata->sources_picture_data_import_recursive = conf->sources_picture_data_import_recursive;
   cfdata->sources_picture_data_import_hidden = conf->sources_picture_data_import_hidden;
#endif
#ifdef HAVE_RSS
   cfdata->sources_rss_timer_s = conf->sources_rss_timer_s / 60;
   cfdata->sources_rss_popup_news = conf->sources_rss_popup_news;
   cfdata->sources_rss_popup_news_timer = conf->sources_rss_popup_news_timer;
#endif
#ifdef HAVE_FILE
   cfdata->sources_file_font_size = conf->sources_file_font_size;
#endif

   cfdata->boxs_always_id = conf->boxs_always_id;
   cfdata->boxs_always_name = conf->boxs_always_name;
   cfdata->boxs_nice_resize = conf->boxs_nice_resize;
   cfdata->boxs_nice_trans = conf->boxs_nice_trans;
   cfdata->boxs_default_max_size = conf->boxs_default_max_size;
   cfdata->boxs_default_anim = conf->boxs_default_anim;
   cfdata->boxs_anim_ghost_timer = conf->boxs_anim_ghost_timer;
#ifdef HAVE_PICTURE
   cfdata->data_picture_thumb_default_size = conf->data_picture_thumb_default_size;
   cfdata->sources_picture_set_bg_purge = conf->sources_picture_set_bg_purge;
   cfdata->sources_picture_show_devian_pics = conf->sources_picture_show_devian_pics;
#endif
#ifdef HAVE_FILE
   cfdata->sources_file_nb_lines_ini = conf->sources_file_nb_lines_ini;
   cfdata->sources_file_nb_lines_ini = conf->sources_file_nb_lines_ini;
#endif
}

static void
_main_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   /* Free the cfdata */
   E_FREE(cfdata->viewer_http);
   E_FREE(cfdata->viewer_image);
   E_FREE(cfdata->viewer_file);
#ifdef HAVE_PICTURE
   E_FREE(cfdata->sources_picture_data_import_dir);
#endif
   free(cfdata);
}

/**--APPLY--**/

static int
_main_basic_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   Evas_List *l;
   DEVIANN *devian;

#ifdef HAVE_PICTURE
   if (strcmp(DEVIANM->conf->sources_picture_data_import_dir, cfdata->sources_picture_data_import_dir))
     {
        evas_stringshare_del(DEVIANM->conf->sources_picture_data_import_dir);
        DEVIANM->conf->sources_picture_data_import_dir = evas_stringshare_add(cfdata->sources_picture_data_import_dir);
     }
   DEVIANM->conf->sources_picture_data_import_recursive = cfdata->sources_picture_data_import_recursive;
#endif
#ifdef HAVE_RSS
   DEVIANM->conf->sources_rss_timer_s = cfdata->sources_rss_timer_s * 60;
   if (DEVIANM->conf->sources_rss_popup_news != cfdata->sources_rss_popup_news)
     {
        if (!cfdata->sources_rss_popup_news)
           DEVIANF(popup_warn_devian_desactivate) ();
        else
           DEVIANM->conf->sources_rss_popup_news = cfdata->sources_rss_popup_news;
     }
#endif
#ifdef HAVE_FILE
   if ((DEVIANM->conf->sources_file_font_size != cfdata->sources_file_font_size))
     {
        DEVIANM->conf->sources_file_font_size = cfdata->sources_file_font_size;
        for (l = DEVIANM->devians; l; l = evas_list_next(l))
          {
             devian = evas_list_data(l);
             if (devian->conf->source_type == SOURCE_FILE)
               {
                  devian->source_func.refresh(devian, 0);
               }
          }
     }
#endif

   if ((DEVIANM->conf->boxs_always_id != cfdata->boxs_always_id) || (DEVIANM->conf->boxs_always_name != cfdata->boxs_always_name))
     {
        DEVIANM->conf->boxs_always_id = cfdata->boxs_always_id;
        DEVIANM->conf->boxs_always_name = cfdata->boxs_always_name;
        for (l = DEVIANM->devians; l; l = evas_list_next(l))
          {
             devian = evas_list_data(l);
             if (devian->conf->container_type == CONTAINER_BOX)
               {
                  DEVIANF(container_box_comments_display_set) (devian->container);
               }
          }
     }
   DEVIANM->conf->boxs_nice_resize = cfdata->boxs_nice_resize;
   DEVIANM->conf->boxs_nice_trans = cfdata->boxs_nice_trans;

   e_config_save_queue();

   return 1;                    /* Apply was OK */
}

static int
_main_advanced_apply_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   /* Actually take our cfdata settings and apply them in real life */
   Evas_List *l;
   DEVIANN *devian;

   if (strcmp(DEVIANM->conf->viewer_http, cfdata->viewer_http) && cfdata->viewer_http[0])
     {
        evas_stringshare_del(DEVIANM->conf->viewer_http);
        DEVIANM->conf->viewer_http = evas_stringshare_add(cfdata->viewer_http);
     }
   if (strcmp(DEVIANM->conf->viewer_image, cfdata->viewer_image) && cfdata->viewer_image[0])
     {
        evas_stringshare_del(DEVIANM->conf->viewer_image);
        DEVIANM->conf->viewer_image = evas_stringshare_add(cfdata->viewer_image);
     }
   if (strcmp(DEVIANM->conf->viewer_file, cfdata->viewer_file) && cfdata->viewer_file[0])
     {
        evas_stringshare_del(DEVIANM->conf->viewer_file);
        DEVIANM->conf->viewer_file = evas_stringshare_add(cfdata->viewer_file);
     }

#ifdef HAVE_PICTURE
   if (strcmp(DEVIANM->conf->sources_picture_data_import_dir, cfdata->sources_picture_data_import_dir))
     {
        evas_stringshare_del(DEVIANM->conf->sources_picture_data_import_dir);
        DEVIANM->conf->sources_picture_data_import_dir = evas_stringshare_add(cfdata->sources_picture_data_import_dir);
     }
   DEVIANM->conf->sources_picture_data_import_recursive = cfdata->sources_picture_data_import_recursive;
   DEVIANM->conf->sources_picture_data_import_hidden = cfdata->sources_picture_data_import_hidden;
#endif
#ifdef HAVE_RSS
   DEVIANM->conf->sources_rss_timer_s = cfdata->sources_rss_timer_s * 60;
   if (DEVIANM->conf->sources_rss_popup_news != cfdata->sources_rss_popup_news)
     {
        if (!cfdata->sources_rss_popup_news)
           DEVIANF(popup_warn_devian_desactivate) ();
        else
           DEVIANM->conf->sources_rss_popup_news = cfdata->sources_rss_popup_news;
     }
   DEVIANM->conf->sources_rss_popup_news_timer = cfdata->sources_rss_popup_news_timer;
#endif
#ifdef HAVE_FILE
   if ((DEVIANM->conf->sources_file_font_size != cfdata->sources_file_font_size))
     {
        DEVIANM->conf->sources_file_font_size = cfdata->sources_file_font_size;
        for (l = DEVIANM->devians; l; l = evas_list_next(l))
          {
             devian = evas_list_data(l);
             if (devian->conf->source_type == SOURCE_FILE)
               {
                  devian->source_func.refresh(devian, 0);
               }
          }
     }
#endif

   if ((DEVIANM->conf->boxs_always_id != cfdata->boxs_always_id) || (DEVIANM->conf->boxs_always_name != cfdata->boxs_always_name))
     {
        DEVIANM->conf->boxs_always_id = cfdata->boxs_always_id;
        DEVIANM->conf->boxs_always_name = cfdata->boxs_always_name;
        for (l = DEVIANM->devians; l; l = evas_list_next(l))
          {
             devian = evas_list_data(l);
             if (devian->conf->container_type == CONTAINER_BOX)
               {
                  DEVIANF(container_box_comments_display_set) (devian->container);
               }
          }
     }
   DEVIANM->conf->boxs_nice_resize = cfdata->boxs_nice_resize;
   DEVIANM->conf->boxs_nice_trans = cfdata->boxs_nice_trans;

   /* Advanced only */
   DEVIANM->conf->boxs_default_max_size = cfdata->boxs_default_max_size;
   DEVIANM->conf->boxs_default_anim = cfdata->boxs_default_anim;
   if (DEVIANM->conf->boxs_anim_ghost_timer != cfdata->boxs_anim_ghost_timer)
     {
        DEVIANM->conf->boxs_anim_ghost_timer = cfdata->boxs_anim_ghost_timer;
        for (l = DEVIANM->devians; l; l = evas_list_next(l))
          {
             devian = evas_list_data(l);
             if (devian->conf->container_type == CONTAINER_BOX)
               {
                  DEVIANF(container_box_animation_start) (devian->container, CONTAINER_BOX_ANIM_GHOST);
               }
          }
     }
#ifdef HAVE_PICTURE
   DEVIANM->conf->data_picture_thumb_default_size = cfdata->data_picture_thumb_default_size;
   DEVIANM->conf->sources_picture_set_bg_purge = cfdata->sources_picture_set_bg_purge;
   DEVIANM->conf->sources_picture_show_devian_pics = cfdata->sources_picture_show_devian_pics;
#endif
#ifdef HAVE_FILE
   DEVIANM->conf->sources_file_nb_lines_ini = cfdata->sources_file_nb_lines_ini;
   DEVIANM->conf->sources_file_nb_lines_max = cfdata->sources_file_nb_lines_max;
#endif

   e_config_save_queue();

   return 1;
}

/**--GUI--**/

static Evas_Object *
_main_basic_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;

   o = e_widget_table_add(evas, 0);

   of = e_widget_frametable_add(evas, _(MODULE_NAME "s"), 0);
#ifdef HAVE_PICTURE
   ob = e_widget_button_add(evas, "Add " MODULE_NAME " Picture", NULL, _cb_button_add_devian_source_picture, NULL, NULL);
#ifdef HAVE_ALL
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
#else
   e_widget_frametable_object_append(of, ob, 0, 0, 3, 1, 1, 1, 1, 1);
#endif
#endif
#ifdef HAVE_RSS
   ob = e_widget_button_add(evas, "Add " MODULE_NAME " Rss", NULL, _cb_button_add_devian_source_rss, NULL, NULL);
#ifdef HAVE_ALL
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);
#else
   e_widget_frametable_object_append(of, ob, 0, 0, 3, 1, 1, 1, 1, 1);
#endif
#endif
#ifdef HAVE_FILE
   ob = e_widget_button_add(evas, "Add " MODULE_NAME " Log", NULL, _cb_button_add_devian_source_file, NULL, NULL);
#ifdef HAVE_ALL
   e_widget_frametable_object_append(of, ob, 2, 0, 1, 1, 1, 1, 1, 1);
#else
   e_widget_frametable_object_append(of, ob, 0, 0, 3, 1, 1, 1, 1, 1);
#endif
#endif
   ob = e_widget_button_add(evas, MODULE_NAME "'s theme", NULL, _cb_button_theme_select, NULL, NULL);
   e_widget_frametable_object_append(of, ob, 0, 1, 3, 1, 1, 1, 1, 1);
   e_widget_table_object_append(o, of, 0, 0, 1, 1, 1, 1, 1, 1);

   of = e_widget_frametable_add(evas, _("Boxs"), 0);
   ob = e_widget_check_add(evas, _("Always number"), &(cfdata->boxs_always_id));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Always name"), &(cfdata->boxs_always_name));
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Nice resizes"), &(cfdata->boxs_nice_resize));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Nice transitions"), &(cfdata->boxs_nice_trans));
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 1, 1, 1);
   e_widget_table_object_append(o, of, 0, 1, 1, 1, 1, 1, 1, 1);

#ifdef HAVE_RSS
   of = e_widget_frametable_add(evas, _("Rss"), 0);
   ob = e_widget_check_add(evas, _("News popup active"), &(cfdata->sources_rss_popup_news));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   e_widget_table_object_append(o, of, 1, 0, 1, 1, 1, 1, 1, 1);
#endif

#ifdef HAVE_PICTURE
   of = e_widget_frametable_add(evas, _("Pictures"), 0);
   ob = e_widget_label_add(evas, _("Import dir"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_entry_add(evas, &(cfdata->sources_picture_data_import_dir));
   e_widget_min_size_set(ob, 100, 1);
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Load sub-directories"), &(cfdata->sources_picture_data_import_recursive));
   e_widget_frametable_object_append(of, ob, 0, 1, 2, 1, 1, 1, 1, 1);
   ob = e_widget_button_add(evas, "Regenerate pictures list", NULL, _cb_button_regen_picture_list, NULL, NULL);
   e_widget_frametable_object_append(of, ob, 0, 2, 2, 1, 1, 1, 1, 1);
   e_widget_table_object_append(o, of, 1, 1, 1, 1, 1, 1, 1, 1);
#endif

#ifdef HAVE_FILE
   of = e_widget_frametable_add(evas, _("Log File"), 0);
   ob = e_widget_label_add(evas, _("Font size"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f"), (float)DATA_FILE_FONT_MIN, (float)DATA_FILE_FONT_MAX, 1.0, 0,
                            NULL, &(cfdata->sources_file_font_size), 90);
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 0, 1, 0);
   e_widget_table_object_append(o, of, 1, 2, 1, 1, 1, 1, 1, 1);
#endif

   return o;
}

static Evas_Object *
_main_advanced_create_widgets(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o, *of, *ob;
   E_Radio_Group *rg;

   o = e_widget_table_add(evas, 0);

   of = e_widget_frametable_add(evas, _(MODULE_NAME "s"), 0);
#ifdef HAVE_PICTURE
   ob = e_widget_button_add(evas, "Add " MODULE_NAME " Picture", NULL, _cb_button_add_devian_source_picture, NULL, NULL);
#ifdef HAVE_ALL
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
#else
   e_widget_frametable_object_append(of, ob, 0, 0, 3, 1, 1, 1, 1, 1);
#endif
#endif
#ifdef HAVE_RSS
   ob = e_widget_button_add(evas, "Add " MODULE_NAME " Rss", NULL, _cb_button_add_devian_source_rss, NULL, NULL);
#ifdef HAVE_ALL
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);
#else
   e_widget_frametable_object_append(of, ob, 0, 0, 3, 1, 1, 1, 1, 1);
#endif
#endif
#ifdef HAVE_FILE
   ob = e_widget_button_add(evas, "Add " MODULE_NAME " Log", NULL, _cb_button_add_devian_source_file, NULL, NULL);
#ifdef HAVE_ALL
   e_widget_frametable_object_append(of, ob, 2, 0, 1, 1, 1, 1, 1, 1);
#else
   e_widget_frametable_object_append(of, ob, 0, 0, 3, 1, 1, 1, 1, 1);
#endif
#endif
   ob = e_widget_button_add(evas, MODULE_NAME "'s theme", NULL, _cb_button_theme_select, NULL, NULL);
   e_widget_frametable_object_append(of, ob, 0, 1, 3, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, _("Viewer http"));
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_entry_add(evas, &(cfdata->viewer_http));
   e_widget_min_size_set(ob, 100, 1);
   e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, _("Viewer image"));
   e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_entry_add(evas, &(cfdata->viewer_image));
   e_widget_min_size_set(ob, 100, 1);
   e_widget_frametable_object_append(of, ob, 1, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, _("Viewer log"));
   e_widget_frametable_object_append(of, ob, 0, 4, 1, 1, 1, 1, 1, 1);
   ob = e_widget_entry_add(evas, &(cfdata->viewer_file));
   e_widget_min_size_set(ob, 100, 1);
   e_widget_frametable_object_append(of, ob, 1, 4, 1, 1, 1, 1, 1, 1);
   e_widget_table_object_append(o, of, 0, 0, 1, 1, 1, 1, 1, 1);

   of = e_widget_frametable_add(evas, _("Boxs"), 0);
   ob = e_widget_check_add(evas, _("Always number"), &(cfdata->boxs_always_id));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Always name"), &(cfdata->boxs_always_name));
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Nice resizes"), &(cfdata->boxs_nice_resize));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Nice transitions"), &(cfdata->boxs_nice_trans));
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, _("Size of content at creation time"));
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f pixels"), (float)CONTAINER_BOX_SIZE_MIN, (float)CONTAINER_BOX_SIZE_MAX, 1.0, 0,
                            NULL, &(cfdata->boxs_default_max_size), 100);
   e_widget_frametable_object_append(of, ob, 0, 3, 2, 1, 1, 0, 1, 0);
   ob = e_widget_label_add(evas, _("Animation at creation time"));
   e_widget_frametable_object_append(of, ob, 0, 4, 2, 1, 1, 1, 1, 1);
   rg = e_widget_radio_group_new(&(cfdata->boxs_default_anim));
   ob = e_widget_radio_add(evas, _("No"), CONTAINER_BOX_ANIM_NO, rg);
   e_widget_frametable_object_append(of, ob, 0, 5, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Line"), CONTAINER_BOX_ANIM_LINE, rg);
   e_widget_frametable_object_append(of, ob, 1, 5, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Gouloum"), CONTAINER_BOX_ANIM_GOULOUM, rg);
   e_widget_frametable_object_append(of, ob, 0, 6, 1, 1, 1, 1, 1, 1);
   ob = e_widget_radio_add(evas, _("Ghost"), CONTAINER_BOX_ANIM_GHOST, rg);
   e_widget_frametable_object_append(of, ob, 1, 6, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, _("Ghost animation timer"));
   e_widget_frametable_object_append(of, ob, 0, 7, 2, 1, 1, 1, 1, 1);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f secs"), 7.0, 120.0, 1.0, 0, NULL, &(cfdata->boxs_anim_ghost_timer), 100);
   e_widget_frametable_object_append(of, ob, 0, 8, 2, 1, 1, 0, 1, 0);
   e_widget_table_object_append(o, of, 0, 1, 1, 1, 1, 1, 1, 1);

#ifdef HAVE_RSS
   of = e_widget_frametable_add(evas, _("Rss"), 0);
   ob = e_widget_label_add(evas, _("RSS timer at creation time"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
   ob =
      e_widget_slider_add(evas, 1, 0, _("%1.0f min"), (float)SOURCE_RSS_UPDATE_RATE_MIN / 60,
                          (float)SOURCE_RSS_UPDATE_RATE_MAX / 60, 1.0, 0, NULL, &(cfdata->sources_rss_timer_s), 130);
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 0, 1, 0);
   ob = e_widget_check_add(evas, _("News popup active"), &(cfdata->sources_rss_popup_news));
   e_widget_frametable_object_append(of, ob, 0, 3, 1, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, _("News popup timer"));
   e_widget_frametable_object_append(of, ob, 0, 4, 1, 1, 1, 1, 1, 1);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f secs"), 0.0, (float)POPUP_WARN_TIMER_MAX, 1.0, 0,
                            NULL, &(cfdata->sources_rss_popup_news_timer), 100);
   e_widget_frametable_object_append(of, ob, 0, 5, 1, 1, 1, 1, 1, 1);
   e_widget_table_object_append(o, of, 1, 0, 1, 1, 1, 1, 1, 1);
#endif

#ifdef HAVE_PICTURE
   of = e_widget_frametable_add(evas, _("Pictures"), 0);
   ob = e_widget_label_add(evas, _("Import dir"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_entry_add(evas, &(cfdata->sources_picture_data_import_dir));
   e_widget_min_size_set(ob, 100, 1);
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Load sub-directories"), &(cfdata->sources_picture_data_import_recursive));
   e_widget_frametable_object_append(of, ob, 0, 1, 2, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Load hidden files/directories"), &(cfdata->sources_picture_data_import_hidden));
   e_widget_frametable_object_append(of, ob, 0, 2, 2, 1, 1, 1, 1, 1);
   ob = e_widget_button_add(evas, "Regenerate pictures list", NULL, _cb_button_regen_picture_list, NULL, NULL);
   e_widget_frametable_object_append(of, ob, 0, 3, 2, 1, 1, 1, 1, 1);
   ob = e_widget_label_add(evas, _("Quality"));
   e_widget_frametable_object_append(of, ob, 0, 4, 1, 1, 1, 1, 1, 1);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f"), (float)CONTAINER_BOX_SIZE_MIN, (float)CONTAINER_BOX_SIZE_MAX, 1.0, 0,
                            NULL, &(cfdata->data_picture_thumb_default_size), 80);
   e_widget_frametable_object_append(of, ob, 1, 4, 1, 1, 1, 0, 1, 0);
   ob = e_widget_check_add(evas, _("Remove created .e/e/backgrounds/"), &(cfdata->sources_picture_set_bg_purge));
   e_widget_frametable_object_append(of, ob, 0, 5, 2, 1, 1, 1, 1, 1);
   ob = e_widget_check_add(evas, _("Show dEvian's logo in slideshow"), &(cfdata->sources_picture_show_devian_pics));
   e_widget_frametable_object_append(of, ob, 0, 6, 2, 1, 1, 1, 1, 1);
   e_widget_table_object_append(o, of, 1, 1, 1, 1, 1, 1, 1, 1);
#endif
#ifdef HAVE_FILE
   of = e_widget_frametable_add(evas, _("Log File"), 0);
   ob = e_widget_label_add(evas, _("Font size"));
   e_widget_frametable_object_append(of, ob, 0, 0, 1, 1, 1, 1, 1, 1);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f"), (float)DATA_FILE_FONT_MIN, (float)DATA_FILE_FONT_MAX, 1.0, 0,
                            NULL, &(cfdata->sources_file_font_size), 90);
   e_widget_frametable_object_append(of, ob, 1, 0, 1, 1, 1, 0, 1, 0);
   ob = e_widget_label_add(evas, _("Initial number of lines"));
   e_widget_frametable_object_append(of, ob, 0, 1, 1, 1, 1, 1, 1, 1);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f"), (float)DATA_FILE_NB_LINES_INI_MIN, (float)DATA_FILE_NB_LINES_INI_MAX, 1.0, 0,
                            NULL, &(cfdata->sources_file_nb_lines_ini), 90);
   e_widget_frametable_object_append(of, ob, 1, 1, 1, 1, 1, 0, 1, 0);
   ob = e_widget_label_add(evas, _("Maximum number of lines"));
   e_widget_frametable_object_append(of, ob, 0, 2, 1, 1, 1, 1, 1, 1);
   ob = e_widget_slider_add(evas, 1, 0, _("%1.0f"), (float)DATA_FILE_NB_LINES_MAX_MIN, (float)DATA_FILE_NB_LINES_MAX_MAX, 1.0, 0,
                            NULL, &(cfdata->sources_file_nb_lines_max), 90);
   e_widget_frametable_object_append(of, ob, 1, 2, 1, 1, 1, 0, 1, 0);
   e_widget_table_object_append(o, of, 1, 2, 1, 1, 1, 1, 1, 1);
#endif

   return o;
}

#ifdef HAVE_PICTURE
static void
_cb_button_add_devian_source_picture(void *data, void *data2)
{
   DEVIANF(devian_add) (SOURCE_PICTURE, NULL);
}
#endif

#ifdef HAVE_RSS
static void
_cb_button_add_devian_source_rss(void *data, void *data2)
{
   DEVIANF(devian_add) (SOURCE_RSS, NULL);
}
#endif

#ifdef HAVE_FILE
static void
_cb_button_add_devian_source_file(void *data, void *data2)
{
   DEVIANF(devian_add) (SOURCE_FILE, NULL);
}
#endif

static void
_cb_button_theme_select(void *data, void *data2)
{
   E_Config_Dialog *cfd;

   if ((cfd = DEVIANF(config_dialog_theme) ()))
      DEVIANM->dialog_conf_theme = cfd;
}

#ifdef HAVE_PICTURE
static void
_cb_button_regen_picture_list(void *data, void *data2)
{
   DEVIANF(data_picture_list_local_regen) ();
}
#endif
