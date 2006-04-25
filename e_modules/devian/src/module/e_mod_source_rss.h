#ifdef E_MOD_DEVIAN_TYPEDEFS

typedef struct _Source_Rss Source_Rss;

#else

#ifndef E_MOD_SOURCE_RSS_H_INCLUDED
#define E_MOD_SOURCE_RSS_H_INCLUDED

/* must be multiples of 60 */
#define SOURCE_RSS_UPDATE_RATE_DEFAULT 60*15
#define SOURCE_RSS_UPDATE_RATE_MIN 60
#define SOURCE_RSS_UPDATE_RATE_MAX 60*30

#define SOURCE_RSS_NB_ITEMS_DEFAULT 7
#define SOURCE_RSS_REVERSE_DEFAULT 0

struct _Source_Rss
{
   DEVIANN *devian;

   Evas_Object *obj0;
   Evas_Object *obj1;

   Rss_Feed *rss_feed;
   Rss_Article *active_article;

   Ecore_Timer *timer;
};

int   DEVIANF(source_rss_add) (DEVIANN *devian);
void  DEVIANF(source_rss_del) (Source_Rss *source);

int   DEVIANF(source_rss_change) (DEVIANN *devian, int option);
int   DEVIANF(source_rss_set_bg) (DEVIANN *devian);
int   DEVIANF(source_rss_viewer) (DEVIANN *devian);

int   DEVIANF(source_rss_evas_object_get) (DEVIANN *devian, Evas_Object **rss0, Evas_Object **rss1);
int   DEVIANF(source_rss_original_size_get) (Source_Rss *source, int part, int *w, int *h);
void  DEVIANF(source_rss_dialog_infos) (Source_Rss *source);
char *DEVIANF(source_rss_name_get) (Source_Rss *source);
char *DEVIANF(source_rss_infos_get) (Source_Rss *source);
void  DEVIANF(source_rss_timer_change) (DEVIANN *devian, int active, int time);

#endif
#endif
