#include "dEvian.h"

static E_Config_DD *_edd_main;
static E_Config_DD *_edd_devians;

#ifdef HAVE_RSS
static E_Config_DD *_edd_rss_docs;
#endif

static E_Config_DD *_devian_config_main_edd_init();
static E_Config_DD *_devian_config_devians_edd_init();

#ifdef HAVE_RSS
static E_Config_DD *_devian_config_rss_docs_edd_init();
#endif

/* Public functions */

DEVIAN_MAIN_CONF *DEVIANF(config_init) (void)
{
   DEVIAN_MAIN_CONF *conf;

   _edd_devians = _devian_config_devians_edd_init();
   if (!_edd_devians)
      return NULL;
#ifdef HAVE_RSS
   _edd_rss_docs = _devian_config_rss_docs_edd_init();
   if (!_edd_rss_docs)
      return NULL;
#endif
   _edd_main = _devian_config_main_edd_init();
   if (!_edd_main)
      return NULL;

   conf = DEVIANF(config_load) ();
   if (!conf)
      return NULL;

   return conf;
}

DEVIAN_MAIN_CONF *DEVIANF(config_load) (void)
{
   DEVIAN_MAIN_CONF *cfg_main;

   cfg_main = e_config_domain_load("module." MODULE_NAME_NOCASE, _edd_main);

   /* If loaded, check for good version */
   if (cfg_main)
     {
        if (cfg_main->conf_version < CONFIG_VERSION)
          {
             char buf[4096];

             snprintf(buf, sizeof(buf),
                      _("<hilight>Configuration Upgraded</hilight><br><br>"
                        "Your configuration of " MODULE_NAME " module<br>"
                        "has been upgraded<br>"
                        "Your settings were removed<br>"
                        "Sorry for the inconvenience<br><br>" "(%d -> %d)"), cfg_main->conf_version, CONFIG_VERSION);
             e_module_dialog_show(_(MODULE_NAME " Module version " MODULE_VERSION), buf);

             cfg_main = NULL;
          }
        else
          {
             if (cfg_main->conf_version > CONFIG_VERSION)
               {
                  char buf[4096];

                  snprintf(buf, sizeof(buf),
                           _("<hilight>Configuration Downgraded</hilight><br><br>"
                             "Your configuration of " MODULE_NAME " module<br>"
                             "has been downgraded<br>"
                             "Your settings were removed<br>"
                             "Sorry for the inconvenience<br><br>" "(%d ->%d)"), cfg_main->conf_version, CONFIG_VERSION);
                  e_module_dialog_show(_(MODULE_NAME " Module version " MODULE_VERSION), buf);
                  cfg_main = NULL;
               }
          }
     }

   if (!cfg_main)
      cfg_main = DEVIANF(config_main_new) ();

   if (cfg_main->nb_devian != evas_list_count(cfg_main->devians_conf))
     {
        fprintf(stderr, MODULE_NAME ": nb_devian (%d) != count(devians_conf) (%d) !!!\n",
                cfg_main->nb_devian, evas_list_count(cfg_main->devians_conf));
        cfg_main->nb_devian = evas_list_count(cfg_main->devians_conf);
     }

   E_CONFIG_LIMIT(cfg_main->high_quality, 0, 1);
   //... ADD all config limits

   DMAIN(("Main config initialised"));

   return cfg_main;
}

void DEVIANF(config_save) (void)
{
   DEVIAN_MAIN_CONF *cfg_main;
   Evas_List *l;
   DEVIANN *devian;

   cfg_main = DEVIANM->conf;
   if (!cfg_main)
      return;

   /* Save number of devians */
   cfg_main->nb_devian = evas_list_count(DEVIANM->devians);

   /* Save config of each devian in main config */
   cfg_main->devians_conf = NULL;
   for (l = DEVIANM->devians; l; l = evas_list_next(l))
     {
        devian = evas_list_data(l);
        DEVIANF(config_devian_save) (devian);
        cfg_main->devians_conf = evas_list_append(cfg_main->devians_conf, devian->conf);
        DMAIN(("Main Config saved"));
     }

   e_config_domain_save("module." MODULE_NAME_NOCASE, _edd_main, DEVIANM->conf);
}

void DEVIANF(config_devian_save) (DEVIANN *devian)
{
   Container_Box *box;

   if (!devian->conf)
      return;

   if (devian->container && (devian->conf->container_type == CONTAINER_BOX))
     {
        box = devian->container;
        devian->conf->box_x = box->x + box->theme_border_w / 2;
        devian->conf->box_y = box->y + box->theme_border_h / 2;
        devian->conf->box_w = box->go_w - box->theme_border_w;
        devian->conf->box_h = box->go_h - box->theme_border_h;
        devian->conf->box_alpha = box->alpha;
     }
}

void DEVIANF(config_free) (void)
{
   Evas_List *l;

   DEVIAN_MAIN_CONF *cfg_main;

   cfg_main = DEVIANM->conf;

   evas_stringshare_del(cfg_main->viewer_http);
   evas_stringshare_del(cfg_main->viewer_image);
#ifdef HAVE_PICTURE
   evas_stringshare_del(cfg_main->theme_picture);
#endif
#ifdef HAVE_RSS
   evas_stringshare_del(cfg_main->theme_rss);
#endif
#ifdef HAVE_FILE
   evas_stringshare_del(cfg_main->theme_file);
#endif
   evas_stringshare_del(cfg_main->theme_popup);
#ifdef HAVE_PICTURE
   evas_stringshare_del(cfg_main->sources_picture_data_import_dir);
#endif

#ifdef HAVE_RSS
   for (l = cfg_main->sources_rss_docs; l; l = evas_list_next(l))
      DEVIANF(data_rss_doc_free) (evas_list_data(l), 0, 1);
#endif

   for (l = DEVIANM->devians; l; l = evas_list_next(l))
      DEVIANF(config_devian_free) (evas_list_data(l));

   E_FREE(DEVIANM->conf);

   E_CONFIG_DD_FREE(_edd_main);
   E_CONFIG_DD_FREE(_edd_devians);
#ifdef HAVE_RSS
   E_CONFIG_DD_FREE(_edd_rss_docs);
#endif
}

void DEVIANF(config_devian_free) (DEVIANN *devian)
{
   if (!devian->conf)
      return;

#ifdef HAVE_RSS
   if (devian->conf->rss_url)
      evas_stringshare_del(devian->conf->rss_url);
#endif

   E_FREE(devian->conf);
   devian->conf = NULL;
}

DEVIAN_MAIN_CONF *DEVIANF(config_main_new) (void)
{
   DEVIAN_MAIN_CONF *cfg_main;

#ifdef HAVE_PICTURE
   char buf[DEVIAN_MAX_PATH];
#endif

   /* Create new config */
   DMAIN(("Main: New config creation ..."));
   cfg_main = E_NEW(DEVIAN_MAIN_CONF, 1);
   cfg_main->conf_version = CONFIG_VERSION;
   cfg_main->high_quality = 0;
   cfg_main->nb_devian = 0;
#ifdef HAVE_PICTURE
   cfg_main->theme_picture = evas_stringshare_add(DEVIAN_THEME_DEFAULT);
#endif
#ifdef HAVE_RSS
   cfg_main->theme_rss = evas_stringshare_add(DEVIAN_THEME_DEFAULT);
#endif
#ifdef HAVE_FILE
   cfg_main->theme_file = evas_stringshare_add(DEVIAN_THEME_DEFAULT);
#endif
   cfg_main->theme_popup = evas_stringshare_add(DEVIAN_THEME_DEFAULT);

#ifdef HAVE_PICTURE
   snprintf(buf, DEVIAN_MAX_PATH, "%s/Tofs", e_user_homedir_get());
   cfg_main->sources_picture_data_import_dir = (char *)evas_stringshare_add(buf);
   cfg_main->sources_picture_data_import_thumbs_warn = 1;
   cfg_main->sources_picture_data_import_recursive = DATA_PICTURE_IMPORT_RECURSIVE_DEFAULT;
   cfg_main->sources_picture_data_import_hidden = DATA_PICTURE_IMPORT_HIDDEN_DEFAULT;
   cfg_main->sources_picture_default_location = DATA_PICTURE_LOCAL;
   cfg_main->sources_picture_set_bg_purge = SOURCE_PICTURE_SET_BG_PURGE_DEFAULT;
   cfg_main->sources_picture_show_devian_pics = DATA_PICTURE_SHOW_DEVIAN_PICS_DEFAULT;
#endif
#ifdef HAVE_RSS
   cfg_main->sources_rss_timer_s = SOURCE_RSS_UPDATE_RATE_DEFAULT;
   cfg_main->sources_rss_popup_news = POPUP_WARN_DEVIAN_ACTIVE_DEFAULT;
   cfg_main->sources_rss_popup_news_timer = POPUP_WARN_TIMER_DEFAULT;
   {
      Rss_Doc *doc;

      doc = E_NEW(Rss_Doc, 1);

      doc->name = evas_stringshare_add("E cvs");
      doc->url = evas_stringshare_add("http://cia.navi.cx/stats/project/e/.rss?ver=2&amp;medium=plaintext&amp;limit=");
      doc->host = evas_stringshare_add("cia.navi.cx");
      doc->file = evas_stringshare_add("/stats/project/e/.rss?ver=2&amp;medium=plaintext&amp;limit=");
      doc->description = evas_stringshare_add("Enlightenment cvs");
      doc->version = DATA_RSS_DOC_VERSION_DEFAULT;
      doc->w_name = DATA_RSS_DOC_W_NAME_DEFAULT;
      doc->w_description = DATA_RSS_DOC_W_DESCRIPTION_DEFAULT;
      doc->user = NULL;
      doc->state = DATA_RSS_DOC_STATE_USABLE;
      cfg_main->sources_rss_docs = evas_list_append(cfg_main->sources_rss_docs, doc);

      doc = E_NEW(Rss_Doc, 1);

      doc->name = evas_stringshare_add("Gentoo packages amd64");
      doc->url = evas_stringshare_add("http://packages.gentoo.org/archs/amd64/gentoo.rss");
      doc->host = evas_stringshare_add("packages.gentoo.org");
      doc->file = evas_stringshare_add("/archs/amd64/gentoo.rss");
      doc->description = evas_stringshare_add("");
      doc->version = DATA_RSS_DOC_VERSION_DEFAULT;
      doc->w_name = DATA_RSS_DOC_W_NAME_DEFAULT;
      doc->w_description = DATA_RSS_DOC_W_DESCRIPTION_DEFAULT;
      doc->user = NULL;
      doc->state = DATA_RSS_DOC_STATE_USABLE;
      cfg_main->sources_rss_docs = evas_list_append(cfg_main->sources_rss_docs, doc);

      doc = E_NEW(Rss_Doc, 1);

      doc->name = evas_stringshare_add("OSNews");
      doc->url = evas_stringshare_add("http://www.osnews.com/files/recent.rdf");
      doc->host = evas_stringshare_add("www.osnews.com");
      doc->file = evas_stringshare_add("/files/recent.rdf");
      doc->description = evas_stringshare_add("");
      doc->version = DATA_RSS_DOC_VERSION_DEFAULT;
      doc->w_name = DATA_RSS_DOC_W_NAME_DEFAULT;
      doc->w_description = DATA_RSS_DOC_W_DESCRIPTION_DEFAULT;
      doc->user = NULL;
      doc->state = DATA_RSS_DOC_STATE_USABLE;
      cfg_main->sources_rss_docs = evas_list_append(cfg_main->sources_rss_docs, doc);

      doc = E_NEW(Rss_Doc, 1);

      doc->name = evas_stringshare_add("Yahoo! News");
      doc->url = evas_stringshare_add("http://rss.news.yahoo.com/rss/topstories");
      doc->host = evas_stringshare_add("rss.news.yahoo.com");
      doc->file = evas_stringshare_add("/rss/topstories");
      doc->description = evas_stringshare_add("");
      doc->version = DATA_RSS_DOC_VERSION_DEFAULT;
      doc->w_name = DATA_RSS_DOC_W_NAME_DEFAULT;
      doc->w_description = DATA_RSS_DOC_W_DESCRIPTION_DEFAULT;
      doc->user = NULL;
      doc->state = DATA_RSS_DOC_STATE_USABLE;
      cfg_main->sources_rss_docs = evas_list_append(cfg_main->sources_rss_docs, doc);

      doc = E_NEW(Rss_Doc, 1);

      doc->name = evas_stringshare_add("Linux Today");
      doc->url = evas_stringshare_add("http://www.linuxtoday.com/backend/biglt.rss");
      doc->host = evas_stringshare_add("www.linuxtoday.com");
      doc->file = evas_stringshare_add("/backend/biglt.rss");
      doc->description = evas_stringshare_add("");
      doc->version = DATA_RSS_DOC_VERSION_DEFAULT;
      doc->w_name = DATA_RSS_DOC_W_NAME_DEFAULT;
      doc->w_description = DATA_RSS_DOC_W_DESCRIPTION_DEFAULT;
      doc->user = NULL;
      doc->state = DATA_RSS_DOC_STATE_USABLE;
      cfg_main->sources_rss_docs = evas_list_append(cfg_main->sources_rss_docs, doc);

      doc = E_NEW(Rss_Doc, 1);

      doc->name = evas_stringshare_add("Freshmeat");
      doc->url = evas_stringshare_add("http://rss.freshmeat.net/freshmeat/feeds/fm-releases-global");
      doc->host = evas_stringshare_add("rss.freshmeat.net");
      doc->file = evas_stringshare_add("/freshmeat/feeds/fm-releases-global");
      doc->description = evas_stringshare_add("");
      doc->version = DATA_RSS_DOC_VERSION_DEFAULT;
      doc->w_name = DATA_RSS_DOC_W_NAME_DEFAULT;
      doc->w_description = DATA_RSS_DOC_W_DESCRIPTION_DEFAULT;
      doc->user = NULL;
      doc->state = DATA_RSS_DOC_STATE_USABLE;
      cfg_main->sources_rss_docs = evas_list_append(cfg_main->sources_rss_docs, doc);

      doc = E_NEW(Rss_Doc, 1);

      doc->name = evas_stringshare_add("Slashdot");
      doc->url = evas_stringshare_add("http://rss.slashdot.org/Slashdot/slashdot");
      doc->host = evas_stringshare_add("rss.slashdot.org");
      doc->file = evas_stringshare_add("/Slashdot/slashdot");
      doc->description = evas_stringshare_add("News for nerds, stuff that matters");
      doc->version = DATA_RSS_DOC_VERSION_DEFAULT;
      doc->w_name = DATA_RSS_DOC_W_NAME_DEFAULT;
      doc->w_description = DATA_RSS_DOC_W_DESCRIPTION_DEFAULT;
      doc->user = NULL;
      doc->state = DATA_RSS_DOC_STATE_USABLE;
      cfg_main->sources_rss_docs = evas_list_append(cfg_main->sources_rss_docs, doc);

      doc = E_NEW(Rss_Doc, 1);

      doc->name = evas_stringshare_add("CNN Top stories");
      doc->url = evas_stringshare_add("http://rss.cnn.com/rss/cnn_topstories.rss");
      doc->host = evas_stringshare_add("rss.cnn.com");
      doc->file = evas_stringshare_add("/rss/cnn_topstories.rss");
      doc->description = evas_stringshare_add("");
      doc->version = DATA_RSS_DOC_VERSION_DEFAULT;
      doc->w_name = DATA_RSS_DOC_W_NAME_DEFAULT;
      doc->w_description = DATA_RSS_DOC_W_DESCRIPTION_DEFAULT;
      doc->user = NULL;
      doc->state = DATA_RSS_DOC_STATE_USABLE;
      cfg_main->sources_rss_docs = evas_list_append(cfg_main->sources_rss_docs, doc);
   }
#endif
#ifdef HAVE_FILE
   cfg_main->sources_file_font_size = SOURCE_FILE_FONT_SIZE_DEFAULT;
   cfg_main->sources_file_nb_lines_ini = DATA_FILE_NB_LINES_INI_DEFAULT;
   cfg_main->sources_file_nb_lines_max = DATA_FILE_NB_LINES_MAX_DEFAULT;
#endif

   cfg_main->container_default = CONTAINER_BOX;
   cfg_main->boxs_always_id = CONTAINER_BOX_ALWAYS_ID_DEFAULT;
   cfg_main->boxs_always_name = CONTAINER_BOX_ALWAYS_NAME_DEFAULT;
   cfg_main->boxs_stacking = CONTAINER_BOX_STACKING_DEFAULT;
   cfg_main->boxs_default_max_size = CONTAINER_BOX_SIZE_DEFAULT;
   cfg_main->boxs_default_anim = CONTAINER_BOX_ANIM_DEFAULT;
   cfg_main->boxs_anim_ghost_timer = CONTAINER_BOX_ANIM_GHOST_TIMER_DEFAULT;
   cfg_main->boxs_nice_resize = CONTAINER_BOX_NICE_RESIZE_DEFAULT;
   cfg_main->boxs_nice_trans = CONTAINER_BOX_NICE_TRANS_DEFAULT;

#ifdef HAVE_PICTURE
   cfg_main->data_picture_thumb_default_size = DATA_PICTURE_THUMB_SIZE_DEFAULT;
   cfg_main->data_picture_cache_size = DATA_PICTURE_CACHE_SIZE_DEFAULT;
#endif

   cfg_main->viewer_image = evas_stringshare_add(DEVIAN_VIEWER_IMAGE_DEFAULT);
   cfg_main->viewer_http = evas_stringshare_add(DEVIAN_VIEWER_HTTP_DEFAULT);
   cfg_main->viewer_file = evas_stringshare_add(DEVIAN_VIEWER_FILE_DEFAULT);

   cfg_main->devians_conf = NULL;

   return cfg_main;
}

/*
  Creates a new config for a devian
  Args:
    - Conf: if !NULL, the returned config will be a copy of this one
  Return:
    The new config
*/
DEVIAN_CONF *DEVIANF(config_devian_new) (int source_type, DEVIAN_CONF *conf)
{
   DEVIAN_CONF *cfg;

   cfg = E_NEW(DEVIAN_CONF, 1);

   if (conf)
     {
        cfg->source_type = source_type;
#ifdef HAVE_PICTURE
        cfg->picture_timer_active = conf->picture_timer_active;
        cfg->picture_timer_s = conf->picture_timer_s;
#endif
#ifdef HAVE_RSS
        if (conf->rss_url)
           cfg->rss_url = evas_stringshare_add(conf->rss_url);
        else
           cfg->rss_url = NULL;
        cfg->rss_doc = NULL;
        cfg->rss_timer_s = conf->rss_timer_s;
        cfg->rss_timer_active = conf->rss_timer_active;
        cfg->rss_nb_items = conf->rss_nb_items;
        cfg->rss_popup_news = conf->rss_popup_news;
        cfg->rss_reverse = conf->rss_reverse;
#endif
#ifdef HAVE_FILE
        cfg->file_path = evas_stringshare_add(conf->file_path);
        cfg->file_auto_scroll = conf->file_auto_scroll;
#endif

        cfg->container_type = conf->container_type;
        cfg->box_speed = conf->box_speed;
        cfg->box_max_size_source = conf->box_max_size_source;
        cfg->box_x = conf->box_x;
        cfg->box_y = conf->box_y;
        cfg->box_w = conf->box_w;
        cfg->box_h = conf->box_h;
        cfg->box_alpha = conf->box_alpha;
        cfg->box_auto_resize = conf->box_auto_resize;
        cfg->box_anim = conf->box_anim;
        cfg->box_allow_overlap = conf->box_allow_overlap;
        cfg->box_infos_show = conf->box_infos_show;
        cfg->box_infos_pos = conf->box_infos_pos;
     }
   else
     {
        cfg->source_type = source_type;
#ifdef HAVE_PICTURE
        cfg->picture_timer_active = 1;
        cfg->picture_timer_s = SOURCE_PICTURE_TIMER_DEFAULT;
#endif
#ifdef HAVE_RSS
        cfg->rss_doc = DEVIANF(data_rss_doc_find_unused) ();
        if (cfg->rss_doc)
           cfg->rss_url = evas_stringshare_add(cfg->rss_doc->url);
        else
           cfg->rss_url = NULL;
        cfg->rss_timer_s = DEVIANM->conf->sources_rss_timer_s;
        cfg->rss_timer_active = 1;
        cfg->rss_nb_items = SOURCE_RSS_NB_ITEMS_DEFAULT;
        cfg->rss_popup_news = POPUP_WARN_DEVIAN_ACTIVE_DEFAULT;
        cfg->rss_reverse = SOURCE_RSS_REVERSE_DEFAULT;
#endif
#ifdef HAVE_FILE
        cfg->file_path = evas_stringshare_add(DATA_FILE_PATH_DEFAULT);
        cfg->file_auto_scroll = DATA_FILE_AUTO_SCROLL_DEFAULT;
#endif

        cfg->container_type = DEVIANM->conf->container_default;
        cfg->box_speed = CONTAINER_BOX_SPEED_DEFAULT;
        cfg->box_max_size_source = DEVIANM->conf->boxs_default_max_size;
        DEVIANF(container_box_random_pos_get) (NULL, &cfg->box_x, &cfg->box_y, cfg->box_max_size_source);
        cfg->box_w = DEVIANM->conf->boxs_default_max_size;
        cfg->box_h = DEVIANM->conf->boxs_default_max_size;
        cfg->box_alpha = CONTAINER_BOX_ALPHA_DEFAULT;
        cfg->box_auto_resize = CONTAINER_BOX_AUTO_RESIZE_DEFAULT;
        cfg->box_anim = DEVIANM->conf->boxs_default_anim;
        cfg->box_allow_overlap = CONTAINER_BOX_ALLOW_OVERLAP_DEFAULT;
        cfg->box_infos_show = CONTAINER_BOX_INFOS_SHOW_DEFAULT;
        cfg->box_infos_pos = CONTAINER_BOX_INFOS_POS_DEFAULT;
     }

   DMAIN(("New config created"));

   return cfg;
}

/* Private functions */

static E_Config_DD *
_devian_config_main_edd_init(void)
{
   E_Config_DD *edd_main;

   edd_main = E_CONFIG_DD_NEW(MODULE_NAME "_Config", DEVIAN_MAIN_CONF);

#undef T
#undef D
#define T DEVIAN_MAIN_CONF
#define D edd_main

   E_CONFIG_VAL(D, T, conf_version, INT);
   E_CONFIG_VAL(D, T, high_quality, INT);
   E_CONFIG_VAL(D, T, nb_devian, INT);
#ifdef HAVE_PICTURE
   E_CONFIG_VAL(D, T, theme_picture, STR);
#endif
#ifdef HAVE_RSS
   E_CONFIG_VAL(D, T, theme_rss, STR);
#endif
#ifdef HAVE_FILE
   E_CONFIG_VAL(D, T, theme_file, STR);
#endif

   E_CONFIG_VAL(D, T, theme_popup, STR);

#ifdef HAVE_PICTURE
   E_CONFIG_VAL(D, T, sources_picture_data_import_dir, STR);
   E_CONFIG_VAL(D, T, sources_picture_data_import_thumbs_warn, INT);
   E_CONFIG_VAL(D, T, sources_picture_data_import_recursive, INT);
   E_CONFIG_VAL(D, T, sources_picture_data_import_hidden, INT);
   E_CONFIG_VAL(D, T, sources_picture_default_location, INT);
   E_CONFIG_VAL(D, T, sources_picture_set_bg_purge, INT);
   E_CONFIG_VAL(D, T, sources_picture_show_devian_pics, INT);
#endif
#ifdef HAVE_RSS
   E_CONFIG_VAL(D, T, sources_rss_timer_s, INT);
   E_CONFIG_LIST(D, T, sources_rss_docs, _edd_rss_docs);
   E_CONFIG_VAL(D, T, sources_rss_popup_news, INT);
   E_CONFIG_VAL(D, T, sources_rss_popup_news_timer, INT);
#endif
#ifdef HAVE_FILE
   E_CONFIG_VAL(D, T, sources_file_font_size, INT);
   E_CONFIG_VAL(D, T, sources_file_nb_lines_ini, INT);
   E_CONFIG_VAL(D, T, sources_file_nb_lines_max, INT);
#endif

   E_CONFIG_VAL(D, T, container_default, INT);

   E_CONFIG_VAL(D, T, boxs_always_id, INT);
   E_CONFIG_VAL(D, T, boxs_always_name, INT);
   E_CONFIG_VAL(D, T, boxs_stacking, INT);
   E_CONFIG_VAL(D, T, boxs_default_max_size, INT);
   E_CONFIG_VAL(D, T, boxs_default_anim, INT);
   E_CONFIG_VAL(D, T, boxs_anim_ghost_timer, INT);
   E_CONFIG_VAL(D, T, boxs_nice_resize, INT);
   E_CONFIG_VAL(D, T, boxs_nice_trans, INT);

#ifdef HAVE_PICTURE
   E_CONFIG_VAL(D, T, data_picture_thumb_default_size, INT);
   E_CONFIG_VAL(D, T, data_picture_cache_size, INT);
#endif

   E_CONFIG_VAL(D, T, viewer_image, STR);
   E_CONFIG_VAL(D, T, viewer_http, STR);
   E_CONFIG_VAL(D, T, viewer_file, STR);

   E_CONFIG_LIST(D, T, devians_conf, _edd_devians);

   return edd_main;
}

static E_Config_DD *
_devian_config_devians_edd_init(void)
{
   E_Config_DD *devians;

   devians = E_CONFIG_DD_NEW(MODULE_NAME "DEVIAN_CONF", DEVIAN_CONF);

#undef T
#undef D
#define T DEVIAN_CONF
#define D devians

   E_CONFIG_VAL(D, T, source_type, INT);
#ifdef HAVE_PICTURE
   E_CONFIG_VAL(D, T, picture_timer_active, INT);
   E_CONFIG_VAL(D, T, picture_timer_s, INT);
#endif
#ifdef HAVE_RSS
   E_CONFIG_VAL(D, T, rss_url, STR);
   E_CONFIG_VAL(D, T, rss_timer_s, INT);
   E_CONFIG_VAL(D, T, rss_timer_active, INT);
   E_CONFIG_VAL(D, T, rss_nb_items, INT);
   E_CONFIG_VAL(D, T, rss_popup_news, INT);
   E_CONFIG_VAL(D, T, rss_reverse, INT);
#endif
#ifdef HAVE_FILE
   E_CONFIG_VAL(D, T, file_path, STR);
   E_CONFIG_VAL(D, T, file_auto_scroll, INT);
#endif

   E_CONFIG_VAL(D, T, container_type, INT);

   E_CONFIG_VAL(D, T, box_speed, INT);
   E_CONFIG_VAL(D, T, box_x, INT);
   E_CONFIG_VAL(D, T, box_y, INT);
   E_CONFIG_VAL(D, T, box_w, INT);
   E_CONFIG_VAL(D, T, box_h, INT);
   E_CONFIG_VAL(D, T, box_alpha, INT);
   E_CONFIG_VAL(D, T, box_max_size_source, INT);
   E_CONFIG_VAL(D, T, box_auto_resize, INT);
   E_CONFIG_VAL(D, T, box_anim, INT);
   E_CONFIG_VAL(D, T, box_allow_overlap, INT);
   E_CONFIG_VAL(D, T, box_infos_show, INT);
   E_CONFIG_VAL(D, T, box_infos_pos, INT);

   return devians;
}

#ifdef HAVE_RSS
static E_Config_DD *
_devian_config_rss_docs_edd_init(void)
{
   E_Config_DD *rss_docs;

   rss_docs = E_CONFIG_DD_NEW(MODULE_NAME "_Config_Rss_Docs", Rss_Doc);

#undef T
#undef D
#define T Rss_Doc
#define D rss_docs

   E_CONFIG_VAL(D, T, name, STR);
   E_CONFIG_VAL(D, T, url, STR);
   E_CONFIG_VAL(D, T, host, STR);
   E_CONFIG_VAL(D, T, file, STR);
   E_CONFIG_VAL(D, T, description, STR);
   E_CONFIG_VAL(D, T, version, FLOAT);

   E_CONFIG_VAL(D, T, w_name, INT);
   E_CONFIG_VAL(D, T, w_description, INT);

   return rss_docs;
}
#endif
