#ifdef E_MOD_DEVIAN_TYPEDEFS

typedef struct _Rss_Feed Rss_Feed;
typedef struct _Rss_Doc Rss_Doc;

#else

#ifndef E_MOD_DATA_RSS_H_INCLUDED
#define E_MOD_DATA_RSS_H_INCLUDED

#define DEVIAN_DATA_RSS_EDJE_MSG_ITEM_NEW 0

#define DATA_RSS_DOC_STATE_USABLE 0
#define DATA_RSS_DOC_STATE_NEW 1

#define DATA_RSS_DOC_VERSION_DEFAULT 0.0
#define DATA_RSS_DOC_W_NAME_DEFAULT 0
#define DATA_RSS_DOC_W_DESCRIPTION_DEFAULT 1

struct _Rss_Feed
{
   Source_Rss *source;

   Ecore_Con_Server *server;
   Ecore_Event_Handler *handler_server_add;
   Ecore_Event_Handler *handler_server_del;
   Ecore_Event_Handler *handler_server_data;
   struct
   {
      char *host;
      int port;
   } proxy;
   int nb_tries;
   char *last_time;

   char *buffer;
   int buffer_size;
   char *item_meta;
   char *channel_meta;

   Ecore_List *list_articles0;
   Ecore_List *list_articles1;
};

struct _Rss_Doc
{
   const char *name;
   const char *url;

   const char *host;
   const char *file;

   const char *description;
   const char *link;
   float version;

   /* server can overwrite ? */
   int w_name;
   int w_description;

   int state;
   Rss_Feed *user;
};

int          DEVIANF(data_rss_new) (Source_Rss *source);
void         DEVIANF(data_rss_del) (Rss_Feed *feed);
int          DEVIANF(data_rss_poll) (void *data, int force_retry);

Rss_Doc     *DEVIANF(data_rss_doc_new) (Rss_Doc *s, int add);
int          DEVIANF(data_rss_doc_free) (Rss_Doc *doc, int remove_from_list, int force);
int          DEVIANF(data_rss_doc_set_new) (Rss_Feed *feed, Rss_Doc *doc, const char *url);
void         DEVIANF(data_rss_doc_detach) (Rss_Doc *doc);
Rss_Doc     *DEVIANF(data_rss_doc_find_doc) (const char *url);
Rss_Doc     *DEVIANF(data_rss_doc_find_unused) (void);

void         DEVIANF(data_rss_prev_detach) (Source_Rss *source, int part);
Evas_Object *DEVIANF(data_rss_object_create) (Rss_Article *article);
void         DEVIANF(data_rss_article_free) (Rss_Article *article);

int          DEVIANF(data_rss_gui_update) (DEVIANN *devian);

#endif
#endif
