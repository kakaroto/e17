#include "dEvian.h"

#ifdef HAVE_RSS

static void _rss_update(Rss_Feed *feed);

static int _net_init(Rss_Feed *feed);
static void _net_shutdown(Rss_Feed *feed);

static int _server_add(void *data, int type, void *event);
static int _server_del(void *data, int type, void *event);
static int _server_data(void *data, int type, void *event);

static void _feed_del(Rss_Feed *feed, Ecore_List *list, Evas_Object *table);
static int _feed_sort(Ecore_List *list);

static int _table_create(Rss_Feed *feed, Ecore_List *list, Ecore_List *old_list, Evas_Object *table);

static Rss_Article *_item_exists(Ecore_List *list, Rss_Article *article);

static void _time_get(char *str);
static const char *_get_host_from_url(const char *url);
static const char *_get_file_from_url(const char *url);

static void _cb_item_open(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _cb_infos_set(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _cb_infos_unset(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _cb_infos_scroll(void *data, Evas_Object *obj, const char *emission, const char *source);


/* PUBLIC FUNCTIONS */

int DEVIANF(data_rss_new) (Source_Rss *source)
{
   Rss_Feed *feed;

   feed = E_NEW(Rss_Feed, 1);

   feed->source = source;

   feed->server = NULL;
   feed->handler_server_add = NULL;
   feed->handler_server_data = NULL;
   feed->handler_server_del = NULL;
   feed->last_time = NULL;
   feed->item_meta = NULL;
   feed->channel_meta = NULL;

   feed->buffer = NULL;
   feed->buffer_size = 0;

   /* initialise lists of items */
   feed->list_articles0 = ecore_list_new();
   feed->list_articles1 = ecore_list_new();

   source->rss_feed = feed;

   /* initialise network */
   if (!_net_init(feed))
     {
        DEVIANF(data_rss_del) (feed);
        return 0;
     }

   feed->last_time = E_NEW(char, 9);
   strcpy(feed->last_time, "");

   /* set the rss document to follow */
   DEVIANF(data_rss_doc_set_new) (feed, feed->source->devian->conf->rss_doc, feed->source->devian->conf->rss_url);

   return 1;
}

void DEVIANF(data_rss_del) (Rss_Feed *feed)
{
   _net_shutdown(feed);

   if (feed->item_meta)
      E_FREE(feed->item_meta);
   if (feed->channel_meta)
      E_FREE(feed->channel_meta);

   _feed_del(feed, feed->list_articles0, feed->source->obj0);
   ecore_list_destroy(feed->list_articles0);
   _feed_del(feed, feed->list_articles1, feed->source->obj1);
   ecore_list_destroy(feed->list_articles1);

   E_FREE(feed->last_time);
   if (feed->buffer)
      E_FREE(feed->buffer);

   DEVIANF(data_rss_doc_detach) (feed->source->devian->conf->rss_doc);

   feed->source->rss_feed = NULL;

   E_FREE(feed);
}

int DEVIANF(data_rss_poll) (void *data, int force_retry)
{
   Rss_Feed *feed;

   feed = (Rss_Feed *)data;

   if (!feed)
      return 0;

   if (feed->server && !force_retry)
     {
        DDATARSS(("Was already in connection"));
        feed->nb_tries++;
        return 0;
     }

   /* delete current connection if was forced to retry */
   if (feed->server)
     {
        DDATARSS(("Was already in connection, but forcing !"));
        ecore_con_server_del(feed->server);
        feed->server = NULL;
        if (feed->buffer)
          {
             E_FREE(feed->buffer);
             feed->buffer = NULL;
             feed->buffer_size = 0;
          }
     }

   if (feed->proxy.port)
      feed->server = ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, feed->proxy.host, feed->proxy.port, feed);
   else
      feed->server = ecore_con_server_connect(ECORE_CON_REMOTE_SYSTEM, feed->source->devian->conf->rss_doc->host, 80, feed);

   if (!feed->server)
     {
        DDATARSS(("Could not connect to server"));
        return 0;
     }

   DEVIANF(container_loading_state_change) (feed->source->devian, 1);

   DDATARSS(("Connection to %s ...", feed->source->devian->conf->rss_doc->host));
   feed->nb_tries = 1;

   return 1;
}

/**
 * To create a new empty doc, or verify (and add) an existing (to list of docs)
 * We assume that the doc is not is the list, and obviously not used
 *
 * @param s If not NULL, going to be tested. If NULL, we create a new doc
 * @param add Used only when s is not NULL. If true, add doc to list of docs after checking
 * @return If test: NULL means test fails. If create, we get a pointer on the new doc
 */

Rss_Doc *DEVIANF(data_rss_doc_new) (Rss_Doc *s, int add)
{
   Rss_Doc *doc;

   if (s)
     {
        /* check if s is correct */
        {
           char *p, *p2;

           if (!s->name)
              return NULL;
           if (!s->url)
              return NULL;
           if (strncmp(s->url, "http://", 7))
              return NULL;
           if (strlen(s->url) <= 7)
              return NULL;
           p = (char *)(s->url + 7);
           if (!(p2 = strchr(p, '/')))
              return NULL;
           if (!s->description)
              s->description = evas_stringshare_add("");
           if (s->host)
              evas_stringshare_del(s->host);
           if (!(s->host = _get_host_from_url(s->url)))
              return NULL;
           if (s->file)
              evas_stringshare_del(s->file);
           if (!(s->file = _get_file_from_url(s->url)))
              return NULL;
        }

        if (add)
          {
             /* check if s already exists */
             {
                Evas_List *l;

                for (l = DEVIANM->conf->sources_rss_docs; l; l = evas_list_next(l))
                  {
                     doc = evas_list_data(l);
                     if (!strcmp(s->url, doc->url))
                        return NULL;
                  }
             }

             /* s is okay to be added ! lets do it */
             s->user = NULL;
             s->state = DATA_RSS_DOC_STATE_USABLE;
             DEVIANM->conf->sources_rss_docs = evas_list_append(DEVIANM->conf->sources_rss_docs, s);
          }

        doc = s;
     }
   else
     {
        doc = E_NEW(Rss_Doc, 1);

        doc->name = evas_stringshare_add("");
        doc->url = evas_stringshare_add("http://");
        doc->host = evas_stringshare_add("");
        doc->file = evas_stringshare_add("");
        doc->description = evas_stringshare_add("");
        doc->version = DATA_RSS_DOC_VERSION_DEFAULT;
        doc->w_name = DATA_RSS_DOC_W_NAME_DEFAULT;
        doc->w_description = DATA_RSS_DOC_W_DESCRIPTION_DEFAULT;
        doc->state = DATA_RSS_DOC_STATE_NEW;
        doc->user = NULL;
     }

   DDATARSS(("Doc OKay =)"));
   return doc;
}

int DEVIANF(data_rss_doc_free) (Rss_Doc *doc, int remove_from_list, int force)
{
   if (!force)
     {
        /* check */
        if (doc->user)
           return 0;

        DEVIANF(data_rss_doc_detach) (doc);
     }

   if (remove_from_list)
      DEVIANM->conf->sources_rss_docs = evas_list_remove(DEVIANM->conf->sources_rss_docs, doc);

   if (doc->name)
      evas_stringshare_del(doc->name);
   if (doc->url)
      evas_stringshare_del(doc->url);
   if (doc->host)
      evas_stringshare_del(doc->host);
   if (doc->file)
      evas_stringshare_del(doc->file);
   if (doc->description)
      evas_stringshare_del(doc->description);

   E_FREE(doc);

   return 1;
}

/**
 * Attach a DOCument to a feed
 * We assume that doc is right (already checked by devian_data_rss_doc_new)
 * @param feed The feed where to attach the doc
 * @param doc The doc to attach
 * @param url The url of the doc to attach (helps to find the doc)
 * @return -1 if doc already used, 0 if error, 1 if ok
 */
int DEVIANF(data_rss_doc_set_new) (Rss_Feed *feed, Rss_Doc *doc, const char *url)
{
   if (!doc)
     {
        if (!url)
           return 0;
        else
           doc = DEVIANF(data_rss_doc_find_doc) (url);
        if (!doc)
           return 0;
     }

   if (doc->user)
     {
        char buf[4096];

        snprintf(buf, sizeof(buf),
                 _("<hilight>Rss '%s' is aready in use !</hilight><br><br>"
                   "You can't set a rss document to more than one dEvian<br><br>" "Url: %s"), doc->name, doc->url);
        e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
        return -1;
     }

   DEVIANF(data_rss_doc_detach) (feed->source->devian->conf->rss_doc);

   doc->user = feed;
   feed->source->devian->conf->rss_doc = doc;

   if (feed->source->devian->conf->rss_url)
      evas_stringshare_del(feed->source->devian->conf->rss_url);
   feed->source->devian->conf->rss_url = evas_stringshare_add(feed->source->devian->conf->rss_doc->url);

   DDATARSS(("New rss set host: %s url: %s file: %s", doc->host, doc->url, doc->file));

   if (feed->last_time)
      E_FREE(feed->last_time);
   feed->last_time = E_NEW(char, 9);

   strcpy(feed->last_time, "");
   if (feed->item_meta)
     {
        E_FREE(feed->item_meta);
        feed->item_meta = NULL;
     }
   if (feed->channel_meta)
     {
        E_FREE(feed->channel_meta);
        feed->channel_meta = NULL;
     }

   /* if feed is rightly initialised, we can poll */
   if (feed->source->rss_feed)
      DEVIANF(data_rss_poll) (feed, 1);

   return 1;
}

void DEVIANF(data_rss_doc_detach) (Rss_Doc *doc)
{
   if (!doc)
      return;
   if (!doc->user)
      return;
   if (!doc->user->source->devian->conf)
      return;

   doc->user->source->devian->conf->rss_doc = NULL;
   doc->user = NULL;
}

Rss_Doc *DEVIANF(data_rss_doc_find_doc) (const char *url)
{
   Rss_Doc *doc;
   Evas_List *list;

   list = DEVIANM->conf->sources_rss_docs;
   do
     {
        doc = evas_list_data(list);
        if (!strcmp(doc->url, url))
           return doc;
     }
   while ((list = evas_list_next(list)));

   DDATARSS(("Url %s NOT found !", url));
   return NULL;
}

Rss_Doc *DEVIANF(data_rss_doc_find_unused) (void)
{
   Rss_Doc *doc;
   Evas_List *list;

   list = DEVIANM->conf->sources_rss_docs;
   do
     {
        doc = evas_list_data(list);
        if (!doc->user)
           return doc;
     }
   while ((list = evas_list_next(list)));

   DDATARSS(("Didnt find lonely rss doc !"));
   return NULL;
}

void DEVIANF(data_rss_prev_detach) (Source_Rss *source, int part)
{
   Rss_Feed *feed;
   Ecore_List *list;
   Evas_Object *obj;

   feed = source->rss_feed;
   if (!feed)
      return;

   DDATARSS(("Want to detach source %d", part));

   if (!part && !ecore_list_is_empty(feed->list_articles0))
     {
        list = feed->list_articles0;
        obj = source->obj0;
     }
   else
     {
        if (part && !ecore_list_is_empty(feed->list_articles1))
          {
             list = feed->list_articles1;
             obj = source->obj1;
          }
        else
          {
             DDATARSS(("Source %d is empty !", part));
             return;
          }
     }

   _feed_del(feed, list, obj);
}

Evas_Object *DEVIANF(data_rss_object_create) (Rss_Article *article)
{
   Evas_Object *obj;

   /* title (edje) */
   obj = edje_object_add(DEVIANM->container->bg_evas);
   if (!DEVIANF(devian_edje_load) (obj, "devian/rss/item", DEVIAN_THEME_TYPE_RSS))
      return NULL;
   edje_object_part_text_set(obj, "date", article->date_simple);
   edje_object_part_text_set(obj, "title", article->title);

   /* callbacks for open and description change & scroll */
   edje_object_signal_callback_add(obj, "open", "item", _cb_item_open, article);
   edje_object_signal_callback_add(obj, "infos_set", "item", _cb_infos_set, article);
   edje_object_signal_callback_add(obj, "infos_unset", "item", _cb_infos_unset, article);
   edje_object_signal_callback_add(obj, "infos_scroll", "item", _cb_infos_scroll, article);

   article->obj = obj;
   return obj;
}

void DEVIANF(data_rss_article_free) (Rss_Article *article)
{
   DDATARSSP(("Free article (%s)", article->title));
   if (article->title)
      evas_stringshare_del(article->title);
   if (article->url)
      evas_stringshare_del(article->url);
   if (article->description)
      evas_stringshare_del(article->description);
   if (article->date)
      evas_stringshare_del(article->date);
   if (article->date_simple)
      evas_stringshare_del(article->date_simple);
   if (article->obj)
     {
        edje_object_signal_callback_del(article->obj, "infos_set", "item", _cb_infos_set);
        edje_object_signal_callback_del(article->obj, "infos_unset", "item", _cb_infos_unset);
        edje_object_signal_callback_del(article->obj, "open", "item", _cb_item_open);
        evas_object_del(article->obj);
        article->obj = NULL;
     }
   E_FREE(article);
}

int DEVIANF(data_rss_gui_update) (DEVIANN *devian)
{
   Ecore_List *list;
   Evas_Object *table;
   Source_Rss *source;
   Rss_Feed *feed;
   Rss_Article *article;

   source = devian->source;
   feed = source->rss_feed;

   if (DEVIANF(container_edje_part_get) (feed->source->devian))
     {
        list = feed->list_articles1;
        table = feed->source->obj1;
     }
   else
     {
        list = feed->list_articles0;
        table = feed->source->obj0;
     }

   if (ecore_list_is_empty(list))
      return 0;

   ecore_list_goto_first(list);
   while ((article = (Rss_Article *)ecore_list_next(list)) != NULL)
     {
        if (article->obj)
          {
             edje_object_signal_callback_del(article->obj, "infos_set", "item", _cb_infos_set);
             edje_object_signal_callback_del(article->obj, "infos_unset", "item", _cb_infos_unset);
             edje_object_signal_callback_del(article->obj, "open", "item", _cb_item_open);
             evas_object_del(article->obj);
             article->obj = NULL;
          }
     }

   if (!_table_create(feed, list, NULL, table))
      return 0;

   return 1;
}


/* PRIVATE FUNCTIONS */

static void
_rss_update(Rss_Feed *feed)
{
   Ecore_List *list, *old_list;
   Evas_Object *table;
   char buf[8192];
   int i;

   DDATARSS(("%s", feed->buffer));

   if (!DEVIANF(container_edje_part_get) (feed->source->devian))
     {
        list = feed->list_articles1;
        old_list = feed->list_articles0;
        table = feed->source->obj1;
        DDATARSS(("Update on source 1"));
        /* clean in case we stoped an existing connection */
        _feed_del(feed, list, table);
     }
   else
     {
        list = feed->list_articles0;
        old_list = feed->list_articles1;
        table = feed->source->obj0;
        DDATARSS(("Update on source 0"));
        /* clean in case we stoped an existing connection */
        _feed_del(feed, list, table);
     }

   if (ecore_list_is_empty(old_list))
      old_list = NULL;

   /* parse, sort, repack table and the show result */
   if ((i = DEVIANF(data_rss_parse_feed) (feed, old_list, list)) > 0)
      if ((i = _feed_sort(list)))
         if ((i = _table_create(feed, list, old_list, table)))
            if (DEVIANF(container_edje_part_change) (feed->source->devian))
              {
                 feed->source->devian->container_func.resize_auto(feed->source->devian);
                 _time_get(feed->last_time);
              }

   if (!i)
     {
        Rss_Doc *doc;

        doc = feed->source->devian->conf->rss_doc;
        if (doc)
          {
             char *p;

             p = strstr(feed->buffer, "<rss");
             if (!p)
                p = strstr(feed->buffer, "<rdf");
             if (!p)
               {
                  snprintf(buf, sizeof(buf),
                           _("<hilight>Error reading '%s' !</hilight><br><br>"
                             "The document is not a valid rss feed<br><br>"
                             "<underline=on underline_color=#000>Url :</> %s<br>"
                             "<underline=on underline_color=#000>Begining of the document dEvian get :</><br>%100.100s"),
                           doc->name, doc->url, feed->buffer);
                  e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
               }
             else
               {
                  snprintf(buf, sizeof(buf),
                           _("<hilight>Error reading '%s' feed !</hilight><br><br>"
                             "The document is an RSS feed, but its not supported by dEvian<br>"
                             "Please send this url to ooookiwi@free.fr, and it will be supported in the next version :)<br><br>"
                             "<underline=on underline_color=#000>Url :</> %s<br>"
                             "<underline=on underline_color=#000>Begining of the document dEvian get :</><br>%100.100s<br>"
                             "%100.100s"), doc->name, doc->url, feed->buffer, p);
                  e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
               }
          }
        DEVIANF(container_loading_state_change) (feed->source->devian, 0);
     }
   else
     {
        if (i == -1)            /* no changes in feed */
           DEVIANF(container_loading_state_change) (feed->source->devian, 0);
     }

   E_FREE(feed->buffer);
   feed->buffer = NULL;
   feed->buffer_size = 0;

}

static int
_net_init(Rss_Feed *feed)
{
   char *env;

   /* proxy ? */
   env = getenv("http_proxy");
   if (!env)
      env = getenv("HTTP_PROXY");
   if ((env) && !strncmp(env, "http://", 7))
     {
        /* proxy ! */
        char *host = NULL, *p;
        int port = 0;

        host = strchr(env, ':');
        host += 3;
        p = strchr(host, ':');
        if (p)
          {
             *p = 0;
             p++;
             if (sscanf(p, "%d", &port) != 1)
                port = 0;
          }
#if 0
        else
          {
             p = strchr(host, '/');
             if (p)
                *p = 0;
          }
#endif
        if ((host) && (port))
          {
             feed->proxy.host = strdup(host);
             feed->proxy.port = port;
          }
        else
          {
             feed->proxy.host = NULL;
             feed->proxy.port = 0;
          }
     }
   else
     {
        feed->proxy.host = NULL;
        feed->proxy.port = 0;
     }

   DDATARSS(("Init network: proxy %s %d", feed->proxy.host, feed->proxy.port));

   /* net callbacks */
   feed->handler_server_add = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, _server_add, feed);
   feed->handler_server_del = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, _server_del, feed);
   feed->handler_server_data = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA, _server_data, feed);

   return 1;
}

static void
_net_shutdown(Rss_Feed *feed)
{
   if (feed->proxy.host)
      E_FREE(feed->proxy.host);

   /* net callbacks */
   if (feed->handler_server_add)
      ecore_event_handler_del(feed->handler_server_add);
   if (feed->handler_server_data)
      ecore_event_handler_del(feed->handler_server_data);
   if (feed->handler_server_del)
      ecore_event_handler_del(feed->handler_server_del);

   if (feed->server)
     {
        ecore_con_server_del(feed->server);
        feed->server = NULL;
     }
}

static int
_server_add(void *data, int type, void *event)
{
   Rss_Feed *feed;
   Ecore_Con_Event_Server_Add *ev;
   char buf[1024];

   feed = (Rss_Feed *)data;
   ev = event;

   /* check if the events is our event */
   if (feed->server != ev->server)
      return 1;

   DDATARSS(("Connection established after %d tries, sending request", feed->nb_tries));

   /* send request */
   snprintf(buf, sizeof(buf), "GET %s HTTP/1.0\r\n", feed->source->devian->conf->rss_doc->file);
   ecore_con_server_send(feed->server, buf, strlen(buf));
   snprintf(buf, sizeof(buf), "Host: %s\r\n", feed->source->devian->conf->rss_doc->host);
   ecore_con_server_send(feed->server, buf, strlen(buf));
   snprintf(buf, sizeof(buf), "User-Agent: %s/%s\r\n\r\n", "dEvian", DEVIAN_VERSION);
   ecore_con_server_send(feed->server, buf, strlen(buf));

   return 0;
}

static int
_server_del(void *data, int type, void *event)
{
   Rss_Feed *feed;
   Ecore_Con_Event_Server_Del *ev;

   ev = (Ecore_Con_Event_Server_Del *) event;
   feed = (Rss_Feed *)data;

   /* check if the events is our event */
   if (feed->server != ev->server)
      return 1;

   /* del server */
   ecore_con_server_del(ev->server);
   feed->server = NULL;

   DDATARSS(("Connection end"));

   if (!feed->buffer || !feed->buffer_size)
     {
        printf("RSS Connection Failed !\n");
        DEVIANF(container_loading_state_change) (feed->source->devian, 0);
        return 0;
     }

   /* update all with new data */
   _rss_update(feed);

   return 0;
}

static int
_server_data(void *data, int type, void *event)
{
   Rss_Feed *feed;
   Ecore_Con_Event_Server_Data *ev;

   ev = (Ecore_Con_Event_Server_Data *) event;
   feed = (Rss_Feed *)data;

   /* check if the events is our event */
   if (feed->server != ev->server)
      return 1;

   /* read add add in main buffer */
   feed->buffer = realloc(feed->buffer, feed->buffer_size + ev->size);
   memcpy(feed->buffer + feed->buffer_size, ev->data, ev->size);
   feed->buffer_size += ev->size;

   return 0;
}

static void
_feed_del(Rss_Feed *feed, Ecore_List *list, Evas_Object *table)
{
   Rss_Article *article;

   if (!ecore_list_is_empty(list))
     {
        ecore_list_goto_first(list);
        while ((article = (Rss_Article *)ecore_list_next(list)) != NULL)
          {
             if (!article->reused)
                DEVIANF(data_rss_article_free) (article);
             else
                article->reused = 0;
          }
        ecore_list_clear(list);
     }
}

static int
_feed_sort(Ecore_List *list)
{
   Rss_Article *first, *last;
   int cmp, count;
   int i;

   last = ecore_list_goto_last(list);
   first = ecore_list_goto_first(list);

   if (first == last)
      return 1;
   if (!first->date_simple || !last->date_simple)
      return 1;

   cmp = strcmp(first->date_simple, last->date_simple);

   if (!cmp || cmp > 0)
      return 1;

   /* first is older than last, we have to invert list */
   count = ecore_list_nodes(list);
   for (i = 0; i < count; i++)
     {
        first = ecore_list_remove_first(list);
        ecore_list_append(list, first);
     }

   return 1;
}

static int
_table_create(Rss_Feed *feed, Ecore_List *list, Ecore_List *old_list, Evas_Object *table)
{
   Rss_Article *article;
   int i;

   if (ecore_list_is_empty(list))
      return 0;

   {
      int count, i;
      Ecore_List *first;

      count = ecore_list_nodes(list);
      for (i = 0; i < count; i++)
        {
           first = ecore_list_remove_first(list);
           ecore_list_append(list, first);
        }
   }

   ecore_list_goto_first(list);
   if (feed->source->devian->conf->rss_reverse)
      i = ecore_list_nodes(list) - 1;
   else
      i = 0;
   while ((article = (Rss_Article *)ecore_list_next(list)) != NULL)
     {
        /* create object */
        if (!DEVIANF(data_rss_object_create) (article))
          {
             DEVIANF(data_rss_article_free) (article);
          }
        else
          {
             Rss_Article *old_article;

             if (old_list)
               {
                  if (!(old_article = _item_exists(old_list, article)))
                    {
                       if (strcmp(feed->last_time, ""))
                         {
                            /* new article */
                            edje_object_message_send(article->obj, EDJE_MESSAGE_INT,
                                                     DEVIAN_DATA_RSS_EDJE_MSG_ITEM_NEW, &article->new);
                            if (DEVIANM->conf->sources_rss_popup_news && feed->source->devian->conf->rss_popup_news)
                              {
                                 if (article->date_simple)
                                    DEVIANF(popup_warn_add) (&feed->source->devian->popup_warn,
                                                             POPUP_WARN_TYPE_DEVIAN, article->date_simple, feed->source->devian);
                                 else
                                    DEVIANF(popup_warn_add) (&feed->source->devian->popup_warn,
                                                             POPUP_WARN_TYPE_DEVIAN, "News", feed->source->devian);
                              }
                         }
                       else
                          article->new = 0;
                    }
                  else
                    {
                       if (old_article->new)
                         {
                            /* existing red article */
                            edje_object_message_send(article->obj, EDJE_MESSAGE_INT,
                                                     DEVIAN_DATA_RSS_EDJE_MSG_ITEM_NEW, &article->new);
                         }
                       else
                          article->new = 0;
                    }
               }
             else
                article->new = 0;

             DDATARSS(("Put %s in the table, obj=%p", article->title, article->obj));
             /* hop in the table ;p */
             e_table_pack(table, article->obj, 0, i, 1, 1);
             e_table_pack_options_set(article->obj, 1, 1,       /* fill */
                                      1, 1,     /* expand */
                                      0, 0,     /* align */
                                      0, 0,     /* min */
                                      9999, 9999        /* max */
                );
             evas_object_show(article->obj);
             if (feed->source->devian->conf->rss_reverse)
                i--;
             else
                i++;
          }
     }

   DDATARSS(("Table creation ok, %d items", i));

   return 1;
}

static Rss_Article *
_item_exists(Ecore_List *list, Rss_Article *article)
{
   Rss_Article *cur;

   if (!ecore_list_is_empty(list))
     {
        ecore_list_goto_first(list);
        while ((cur = (Rss_Article *)ecore_list_next(list)))
          {
             if (cur->url && article->url)
                if (strcmp(cur->url, article->url))
                   continue;

             if (cur->date && article->date)
                if (strcmp(cur->date, article->date))
                   continue;

             if (cur->title && article->title)
                if (strcmp(cur->title, article->title))
                   continue;

             return cur;
          }
     }

   return NULL;
}

static void
_time_get(char *str)
{
   struct tm *ts;
   time_t curtime;

   curtime = time(NULL);
   ts = localtime(&curtime);
   snprintf(str, 9, "%02d:%02d:%02d", ts->tm_hour, ts->tm_min, ts->tm_sec);
   //... E_FREE(ts); //maybe ?
}

static const char *
_get_host_from_url(const char *url)
{
   const char *host;
   char *tmp, *p;

   if (strncmp(url, "http://", 7))
      return NULL;
   tmp = strdup(url + 7);
   p = strchr(tmp, '/');
   if (!p)
     {
        E_FREE(tmp);
        return NULL;
     }
   *p = '\0';
   host = evas_stringshare_add(tmp);
   E_FREE(tmp);

   return host;
}

static const char *
_get_file_from_url(const char *url)
{
   const char *file;
   char *p;

   p = strstr(url, "://");
   if (!p)
      return NULL;
   p += 3;
   p = strstr(p, "/");
   if (!p)
      return NULL;
   file = evas_stringshare_add(p);

   return file;
}

static void
_cb_item_open(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Rss_Article *article;
   char buf[4096];
   const char *url;

   if (!data)
      return;
   article = (Rss_Article *)data;
   url = article->url;

   if (!url)
     {
        snprintf(buf, sizeof(buf),
                 _("<hilight>Unable to open link</hilight><br><br>" "Article '%s' has no link associated !"), article->title);
        e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
        return;
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
                   "You can change the viewer for http in dEvian configuration panel\
 (Advanced view)"), DEVIANM->conf->viewer_http);
        e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
        return;
     }

   DDATARSS(("Open %s\n", article->url));
}

static void
_cb_infos_set(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Rss_Article *article;

   if (!data)
      return;
   article = (Rss_Article *)data;
   if (!article->description)
      return;

   /* set item as viewed */
   if (article->new)
     {
        article->new = 0;
        edje_object_message_send(article->obj, EDJE_MESSAGE_INT, DEVIAN_DATA_RSS_EDJE_MSG_ITEM_NEW, &article->new);
        if (article->feed->source->devian->popup_warn)
           DEVIANF(popup_warn_del) (article->feed->source->devian->popup_warn);
     }

   article->feed->source->active_article = article;

   DEVIANF(container_infos_text_change) (article->feed->source->devian, NULL);
}

static void
_cb_infos_unset(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Rss_Article *article;

   if (!data)
      return;
   article = (Rss_Article *)data;

   /* set no current article only if its not another */
   if (article->feed->source->active_article == article)
      article->feed->source->active_article = NULL;
}

static void
_cb_infos_scroll(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Rss_Article *article;

   if (!data)
      return;
   article = (Rss_Article *)data;
   if (!article->description)
      return;

   DEVIANF(container_infos_text_scroll) (article->feed->source->devian);

}
#endif
