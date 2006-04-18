#ifdef E_MOD_DEVIAN_TYPEDEFS
typedef struct _dEvian_Main_Conf DEVIAN_MAIN_CONF;
typedef struct _dEvian_Conf DEVIAN_CONF;

#else

#ifndef E_MOD_CONFIG_H_INCLUDED
#define E_MOD_CONFIG_H_INCLUDED

#ifdef HAVE_ALL
#define CONFIG_VERSION 50
#else
#ifdef HAVE_PICTURE
#define CONFIG_VERSION 5
#else
#ifdef HAVE_RSS
#define CONFIG_VERSION 4
#else
#ifdef HAVE_FILE
#define CONFIG_VERSION 6
#endif
#endif
#endif
#endif

#define CONFIG_THEME_VERSION 2

/* Struct for the module configuration */
struct _dEvian_Main_Conf
{
   /* General Module Settings */
   int conf_version;
   int high_quality;
   int nb_devian;
#ifdef HAVE_PICTURE
   const char *theme_picture;
#endif
#ifdef HAVE_RSS
   const char *theme_rss;
#endif
#ifdef HAVE_FILE
   const char *theme_file;
#endif
   const char *theme_popup;

   /* General Sources Settings */
#ifdef HAVE_PICTURE
   const char *sources_picture_data_import_dir;
   int sources_picture_data_import_recursive;
   int sources_picture_data_import_hidden;
   int sources_picture_data_import_thumbs_warn;
   int sources_picture_default_location;
   int sources_picture_set_bg_purge;
   int sources_picture_show_devian_pics;
#endif
#ifdef HAVE_RSS
   int sources_rss_timer_s;
   Evas_List *sources_rss_docs;
   int sources_rss_popup_news;
   int sources_rss_popup_news_timer;
#endif
#ifdef HAVE_FILE
   int sources_file_font_size;
   int sources_file_nb_lines_ini;
   int sources_file_nb_lines_max;
#endif

   /* General Containers Settings */
   int container_default;

   int boxs_always_id;
   int boxs_always_name;
   int boxs_stacking;
   int boxs_default_max_size;
   int boxs_default_anim;
   int boxs_anim_ghost_timer;
   int boxs_nice_resize;
   int boxs_nice_trans;

   /* General Data Settings */
#ifdef HAVE_PICTURE
   int data_picture_thumb_default_size;
   int data_picture_cache_size;
#endif

   /* Viewers */
   const char *viewer_image;
   const char *viewer_http;
   const char *viewer_file;

   /* Config for each devian */
   Evas_List *devians_conf;
};

/* Struct for sources config */

struct _dEvian_Conf
{
   /* About the source */
   int source_type;

#ifdef HAVE_PICTURE
   int picture_timer_s;
   int picture_timer_active;
#endif
#ifdef HAVE_RSS
   Rss_Doc *rss_doc;            /* Pointer to sources_rss_docs list in DEVIANM conf 
                                 * Not being saved */
   const char *rss_url;         /* Copy of rss_doc url 
                                 * To be saved */
   int rss_timer_s;
   int rss_timer_active;
   int rss_nb_items;
   int rss_popup_news;
   int rss_reverse;
#endif
#ifdef HAVE_FILE
   const char *file_path;
   int file_auto_scroll;
#endif

   /* About the container */
   int container_type;

   int box_x;                   /* ...TODO: rename *_source */
   int box_y;                   /* ...TODO: rename *_source */
   int box_w;                   /* ...TODO: rename *_source */
   int box_h;                   /* ...TODO: rename *_source */
   int box_alpha;
   int box_max_size_source;
   int box_speed;
   int box_auto_resize;
   int box_anim;
   int box_allow_overlap;
   int box_infos_show;
   int box_infos_pos;
};

DEVIAN_MAIN_CONF *DEVIANF(config_init) (void);
DEVIAN_MAIN_CONF *DEVIANF(config_load) (void);
void DEVIANF(config_save) (void);
void DEVIANF(config_devian_save) (DEVIANN *devian);

void DEVIANF(config_free) (void);
void DEVIANF(config_devian_free) (DEVIANN *devian);

DEVIAN_MAIN_CONF *DEVIANF(config_main_new) (void);
DEVIAN_CONF *DEVIANF(config_devian_new) (int source_type, DEVIAN_CONF *conf);

#endif
#endif
