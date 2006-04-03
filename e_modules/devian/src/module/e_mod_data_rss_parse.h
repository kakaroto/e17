#ifdef E_MOD_DEVIAN_TYPEDEFS

typedef struct _Rss_Article Rss_Article;

#else

#ifndef E_MOD_DATA_RSS_PARSE_H_INCLUDED
#define E_MOD_DATA_RSS_PARSE_H_INCLUDED

struct _Rss_Article
{
  /* Edje obj for item */
  Evas_Object *obj;

  Rss_Feed *feed;

  const char *title;
  const char *url;
  const char *description;
  const char *date;
  const char *date_simple;
  int new; /* User already saw it ? */
  int reused; /* Article was existing previously */
};


int DEVIANF(data_rss_parse_feed)(Rss_Feed *feed, Ecore_List *old_list, Ecore_List *list);
char *DEVIANF(data_rss_parse_article)(Rss_Feed *feed, char *buf, Ecore_List *list);
float DEVIANF(data_rss_parse_rss_version)(char *buf);
int DEVIANF(data_rss_parse_doc_infos)(Rss_Feed *feed, char *buf);

#endif
#endif
