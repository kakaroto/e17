#include "e.h"
#include "e_mod_main.h"
#include "evry_api.h"
#include "json.h"

#include <curl/curl.h>
#include <E_Notify.h>


#define ACT_GOOGLE		1
#define ACT_FEELING_LUCKY	2
#define ACT_WIKIPEDIA		3
#define ACT_UPLOAD_IMGUR	4
#define ACT_YOUTUBE		5

#define YOUTUBE_DL		0
#define YOUTUBE_DL_PLAY		1
#define YOUTUBE_DL_ENQ		2
#define YOUTUBE_PLAY		3

/* #undef DBG
 * #define DBG(...) ERR(__VA_ARGS__) */

typedef struct _Plugin Plugin;
typedef int (*Handler_Func) (void *data, int type, void *event);
typedef struct _Module_Config Module_Config;
typedef struct _Json_Data Json_Data;
typedef struct _Url_Data Url_Data;
typedef struct _Youtube_Data Youtube_Data;
typedef struct _Web_Link Web_Link;

static Eina_List *handlers;

struct _Plugin
{
  Evry_Plugin base;
  Ecore_Timer *timer;

  const char *input;
  const char *request;
  const char *host;

  int browse_mode;
  const char *browse_input;

  int (*fetch) (void *data);
  int (*data_cb) (Plugin *p, const char *msg, int len);

  Url_Data *dd;

  Evry_Item *item;
};

struct _Module_Config
{
  int version;

  const char *lang;
  const char *browser;
  /* gtranslaste*/
  const char *translate;
  /* youtube */
  const char *convert_cmd;
  const char *download_cmd;
  const char *download_dir;
  const char *player_cmd;

  E_Config_Dialog *cfd;
  E_Module *module;
  char *theme;
};

struct _Url_Data
{
  Ecore_Con_Url *con_url;

  int (*data_cb)(Url_Data *data);
  int (*progress_cb)(Url_Data *data, Ecore_Con_Event_Url_Progress *ev);

  char *data;
  unsigned int size;

  void *user_data;
};

struct _Youtube_Data
{
  Ecore_Exe *exe1;
  Ecore_Exe *exe2;
  Ecore_Timer *timer;
  Ecore_Event_Handler *del_handler;
  const char *label;
  const char *filepath;
  const char *url;
  const char *fifo;
  int num;

  int method;
  int tries;
  int ready;
  int id;
  int size;

  Url_Data *dd;
};

struct _Web_Link
{
  Evry_Item base;

  const char *url;
  const char *thumb;

  const char *thumb_file;

  Url_Data *dd;
};

struct _Json_Data
{
  Json_Data *parent;
  Json_Data *cur;

  int type;
  const char *key;
  const char *value;
  Eina_List *values;
  Eina_List *list;

  int is_val;
};

static const Evry_API *evry = NULL;
static Evry_Module *evry_module = NULL;

static Module_Config *_conf;
static char _config_path[] =  "launcher/" PACKAGE;
static char _config_domain[] = "module." PACKAGE;
static E_Config_DD *_conf_edd = NULL;

static Eina_List *plugins = NULL;
static Eina_List *actions = NULL;
static Eina_List *download_handlers = NULL;
static char _trigger_google[] = "g ";
static char _trigger_wiki[] = "w ";
static char _trigger_gtranslate[] = "t ";
static char _trigger_youtube[] = "y ";

static char _request_goolge[] =
  "http://www.google.com/complete/search?hl=%s&output=text&q=%s";
static char _request_wiki[]   =
  "http://%s.wikipedia.org/w/api.php?action=opensearch&search=%s";
static char _request_gtranslate[] =
  "http://ajax.googleapis.com/ajax/services/language/translate?v=1.0&langpair=%s&q=%s";
static char _request_youtube[] =
  "http://gdata.youtube.com/feeds/videos?hl=%s"
  "&fields=entry(title,link,media:group(media:thumbnail))"
  "&alt=json&max-results=50&q=%s";
static const char _imgur_key[] =
  "1606e11f5c2ccd9b7440f1ffd80b17de";

static Evry_Type WEBLINK;
#define GET_WEBLINK(_weblink, _item) Web_Link *_weblink = (Web_Link *) _item

static Json_Data  *_json_parse(const char *string, int len);
static Json_Data  *_json_data_find(const Json_Data *d, const char *key, int level);
static void        _json_data_free(Json_Data *d);

/***************************************************************************/

static void
_send_notification(unsigned int id, const char *icon, const char *function,
		   const char *body, int timeout)
{
   E_Notification *n;

   n = e_notification_full_new("Everything", id, icon, function, body, timeout);
   e_notification_send(n, NULL, NULL);
   e_notification_unref(n);
}

static Url_Data *
_url_data_new(void *user_data,
	      int (*data_cb)(Url_Data *dd),
	      int (*progress_cb)(Url_Data *dd, Ecore_Con_Event_Url_Progress *ev),
	      const char *host)
{
   Url_Data *dd = E_NEW(Url_Data, 1);

   dd->con_url = ecore_con_url_new(NULL);
   dd->user_data = user_data;
   dd->data_cb = data_cb;
   dd->progress_cb = progress_cb;

   ecore_con_url_data_set(dd->con_url, dd);

   if (host)
     ecore_con_url_additional_header_add(dd->con_url, "Host", host);

   return dd;
}

static void
_url_data_free(Url_Data *dd)
{
   ecore_con_url_free(dd->con_url);
   E_FREE(dd->data);
   E_FREE(dd);
}

static int
_url_data_send(Url_Data *dd, const char *url)
{
   E_FREE(dd->data);
   dd->size = 0;

   ecore_con_url_url_set(dd->con_url, url);

   return ecore_con_url_get(dd->con_url);
}

static Eina_Bool
_common_data_cb(void *data, int ev_type, void *event)
{
   Ecore_Con_Event_Url_Data *ev = event;
   Url_Data *dd;

   if (!ev || !data || !(data == _conf))
     return EINA_TRUE;

   if (!(dd = ecore_con_url_data_get(ev->url_con)))
     return EINA_TRUE;

   dd->data = realloc(dd->data, sizeof(char) * (dd->size + ev->size));
   memcpy(dd->data + dd->size, ev->data, ev->size);
   dd->size += ev->size;

   return EINA_TRUE;
}

static Eina_Bool
_common_progress_cb(void *data, int ev_type, void *event)
{
   Ecore_Con_Event_Url_Progress *ev = event;
   Url_Data *dd;

   if (!ev || !data || !(data == _conf))
     return EINA_TRUE;

   if (!(dd = ecore_con_url_data_get(ev->url_con)))
     return EINA_TRUE;

   if (dd->progress_cb)
     dd->progress_cb(dd, ev);

   return EINA_TRUE;
}

static Eina_Bool
_common_complete_cb(void *data, int ev_type, void *event)
{
   Ecore_Con_Event_Url_Complete *ev = event;
   Url_Data *dd;

   if (!ev || !data || !(data == _conf))
     return EINA_TRUE;

   if (!(dd = ecore_con_url_data_get(ev->url_con)))
     return EINA_TRUE;

   if (!dd->data_cb(dd))
     {
	ERR("\n %*s\n", dd->size, (char *)dd->data);
	/* XXX free here ?*/
     }

   return EINA_TRUE;
}

static int
_plugin_data_cb(Url_Data *dd)
{
   GET_PLUGIN(p, dd->user_data);

   EVRY_PLUGIN_ITEMS_FREE(p);

   if (p->data_cb(p, dd->data, dd->size))
     {
	EVRY_PLUGIN_UPDATE(p, EVRY_UPDATE_ADD);
     }

   return 1;
}

static int
_icon_data_cb(Url_Data *dd)
{
   GET_WEBLINK(wl, dd->user_data);

   FILE *f = fopen(wl->thumb_file, "w");
   if (f)
     {
	fwrite(dd->data, dd->size, sizeof(char), f);
	fclose(f);
	evry->item_changed(EVRY_ITEM(wl), 1, 0);
     }

   _url_data_free(dd);
   wl->dd = NULL;

   return 1;
}

static int
_wikipedia_data_cb(Plugin *p, const char *msg, int len)
{
   Json_Data *d, *rsp;
   const char *val;
   Eina_List *l;
   Evry_Item *it;
   int ret = 0;

   rsp = _json_parse(msg, len);

   if (rsp && rsp->list &&
       (d = rsp->list->data) &&
       (d->type == JSON_ARRAY_BEGIN) &&
       (d = d->list->data) &&
       (d->type == JSON_ARRAY_BEGIN))
     {
	EINA_LIST_FOREACH(d->values, l, val)
	  {
	     it = EVRY_ITEM_NEW(Evry_Item, p, val, NULL, NULL);
	     EVRY_ITEM_CONTEXT_SET(it, EVRY_PLUGIN(p)->name);
	     it->fuzzy_match = -1;
	     EVRY_PLUGIN_ITEM_APPEND(p, it);
	  }
	ret = 1;
     }
   _json_data_free(rsp);

   return ret;
}

static int
_gtranslate_data_cb(Plugin *p, const char *msg, int len)
{
   Json_Data *d, *rsp;
   Evry_Item *it;
   int ret = 0;

   rsp = _json_parse(msg, len);

   if ((d = _json_data_find(rsp, "translatedText", 3)))
     {
	if (!p->item)
	  {
	     it = EVRY_ITEM_NEW(Evry_Item, p, d->value, NULL, NULL);
	     EVRY_ITEM_CONTEXT_SET(it, EVRY_PLUGIN(p)->name);
	     it->fuzzy_match = -1;
	     EVRY_ITEM_REF(it);
	     p->item = it;
	  }
	else
	  {
	     it = p->item;
	     EVRY_ITEM_REF(it);
	     EVRY_ITEM_LABEL_SET(it, d->value);
	     evry->item_changed(it, 0, 0);
	  }

	EVRY_PLUGIN_ITEM_APPEND(p, it);
	ret = 1;
     }
   _json_data_free(rsp);

   return ret;
}

static void
_web_link_free(Evry_Item *it)
{
   GET_WEBLINK(wl, it);

   IF_RELEASE(wl->url);
   IF_RELEASE(wl->thumb);
   IF_RELEASE(wl->thumb_file);

   if (wl->dd)
     _url_data_free(wl->dd);

   E_FREE(wl);
}

static Evas_Object *
_web_link_icon_get(Evry_Item *it, Evas *e)
{
   Web_Link *wl = (Web_Link *) it;
   Evas_Object *o;

   static char thumb_buf[4096];

   if (!wl->thumb_file)
     {
	char *sum = evry->util_md5_sum(wl->thumb);

	snprintf(thumb_buf, sizeof(thumb_buf),
		 "%s/.cache/youtube/%s.jpeg",
		 e_user_homedir_get(), sum);

	wl->thumb_file = eina_stringshare_add(thumb_buf);
	E_FREE(sum);
     }

   if (ecore_file_exists(wl->thumb_file))
     {
	o = e_icon_add(e);
	e_icon_preload_set(o, 1);
	e_icon_file_set(o, wl->thumb_file);
	if (o) return o;
     }
   else if (!wl->dd)
     {
	wl->dd = _url_data_new(wl, _icon_data_cb, NULL, "i.ytimg.com");
	_url_data_send(wl->dd, wl->thumb);
     }

   return NULL;
}

static int
_youtube_data_cb(Plugin *p, const char *msg, int len)
{
   Json_Data *d, *d2, *rsp;
   Eina_List *l;
   Web_Link *it;

   rsp = _json_parse(msg, len);
   const char *title, *url, *thumb;

   d = _json_data_find(rsp, "entry", 3);
   if (d && d->list)
     {
	d = d->list->data;

	EINA_LIST_FOREACH(d->list, l, d)
	  {
	     url = thumb = title = NULL;

	     if ((d2 = _json_data_find(d, "$t", 2)))
	       title = d2->value;

	     if ((d2 = _json_data_find(d, "href", 3)))
	       url = d2->value;

	     if ((d2 = _json_data_find(d, "media$thumbnail", 2)) &&
		 (d2 = _json_data_find(d2, "url", 2)))
	       thumb = d2->value;

	     if (title && url && thumb)
	       {
		  it = EVRY_ITEM_NEW(Web_Link, p, NULL, _web_link_icon_get, _web_link_free);
		  EVRY_ITEM_LABEL_SET(it, title);
		  EVRY_ITEM_CONTEXT_SET(it, EVRY_PLUGIN(p)->name);
		  it->url = eina_stringshare_ref(url);
		  it->thumb = eina_stringshare_ref(thumb);

		  EVRY_PLUGIN_ITEM_APPEND(p, it);
	       }
	  }
     }
   _json_data_free(rsp);
   return 1;
}

static int
_google_data_cb(Plugin *p, const char *msg, int len)
{
   Json_Data *d, *d2, *rsp = NULL;
   const char *val;
   Eina_List *l, *ll;
   Evry_Item *it;
   char *beg;

   if (!msg) return 1;
   beg = strchr(msg, '(');
   if (beg) beg++;

   rsp = _json_parse(beg, len);

   if (rsp && rsp->list &&
       (d = rsp->list->data) &&
       (d->type == JSON_ARRAY_BEGIN) &&
       (d = d->list->data) &&
       (d->type == JSON_ARRAY_BEGIN))
     {
	EINA_LIST_FOREACH(d->list, l, d2)
	  {
	     ll = d2->values;

	     if (!ll->data || !ll->next->data)
	       continue;

	     val = ll->data;
	     it = EVRY_ITEM_NEW(Evry_Item, p, val, NULL, NULL);
	     EVRY_ITEM_CONTEXT_SET(it, EVRY_PLUGIN(p)->name);
	     val = ll->next->data;
	     EVRY_ITEM_DETAIL_SET(it, val);
	     it->fuzzy_match = -1;
	     EVRY_PLUGIN_ITEM_APPEND(p, it);
	  }
     }
   _json_data_free(rsp);

   return 1;
}

static Evry_Plugin *
_begin(Evry_Plugin *plugin, const Evry_Item *it)
{
   Plugin *p;

   GET_PLUGIN(parent, plugin);

   EVRY_PLUGIN_INSTANCE(p, plugin);
   p->host    = parent->host;
   p->request = parent->request;
   p->data_cb = parent->data_cb;

   if (it && CHECK_TYPE(it, EVRY_TYPE_TEXT))
     {
	p->browse_input = eina_stringshare_ref(it->label);
	p->browse_mode = 1;
     }

   p->dd = _url_data_new(p, _plugin_data_cb, NULL, p->host);

   return EVRY_PLUGIN(p);
}

static void
_finish(Evry_Plugin *plugin)
{
   Eina_List *l;
   Evry_Item *it;

   GET_PLUGIN(p, plugin);

   if (p->dd)
     _url_data_free(p->dd);

   if (p->timer)
     ecore_timer_del(p->timer);

   IF_RELEASE(p->input);
   IF_RELEASE(p->browse_input);

   EINA_LIST_FOREACH(p->base.items, l, it)
     if (CHECK_TYPE(it, WEBLINK))
       {
	  GET_WEBLINK(wl, it);
	  if (!wl->dd) continue;
	  _url_data_free(wl->dd);
	  wl->dd = NULL;
       }

   EVRY_PLUGIN_ITEMS_FREE(p);

   if (p->item)
     EVRY_ITEM_FREE(p->item);

   E_FREE(p);
}

static Eina_Bool
_send_request(void *data)
{
   Plugin *p = data;
   char buf[1024];
   char *query;
   Eina_Bool active;

   if (!p->input) return EINA_FALSE;

   query = evry->util_url_escape(p->input, 0);

   if (!strcmp(p->base.name, N_("Translate")))
     snprintf(buf, sizeof(buf), p->request, _conf->translate, query);
   else
     snprintf(buf, sizeof(buf), p->request, _conf->lang, query);

   DBG("send request %s", buf);

   active = _url_data_send(p->dd, buf);

   free(query);
   p->timer = NULL;

   return !active;
}

static int
_fetch(Evry_Plugin *plugin, const char *input)
{
   GET_PLUGIN(p, plugin);

   IF_RELEASE(p->input);

   if (p->timer)
     {
	ecore_timer_del(p->timer);
	p->timer = NULL;
     }

   if (p->browse_mode)
     {
	char buf[128];

	if (p->browse_mode == 1)
	  {
	     p->input = eina_stringshare_ref(p->browse_input);
	     _send_request(p);
	     p->browse_mode = 2;
	     return 1;
	  }
	else if (input)
	  {
	     snprintf(buf, sizeof(buf), "%s %s", p->browse_input, input);
	     p->input = eina_stringshare_add(buf);
	  }
	else
	  {
	     p->input = eina_stringshare_ref(p->browse_input);
	  }
     }
   else if (input && strlen(input) >= plugin->config->min_query)
     {
	p->input = eina_stringshare_add(input);
     }

   if (p->input)
     p->timer = ecore_timer_add(0.1, _send_request, p);
   else
     EVRY_PLUGIN_ITEMS_FREE(p);

   return EVRY_PLUGIN_HAS_ITEMS(p);
}

/***************************************************************************/

static int
_open_url(const char *url)
{
   Evry_Item_App *app = E_NEW(Evry_Item_App, 1);
   Evry_Item_File *file = E_NEW(Evry_Item_File, 1);
   Eina_List *l;
   E_Border *bd;

   app->desktop = efreet_util_desktop_exec_find(_conf->browser);

   if (!app->desktop)
     app->file = "xdg-open";

   file->path = url;

   evry->util_exec_app(EVRY_ITEM(app), EVRY_ITEM(file));

   if (app->desktop)
     {
	EINA_LIST_FOREACH(e_border_client_list(), l, bd)
	  {
	     if (bd->desktop && bd->desktop == app->desktop)
	       {
		  e_desk_show(bd->desk);
		  e_border_raise(bd);
		  break;
	       }
	  }
	efreet_desktop_free(app->desktop);
     }

   E_FREE(file);
   E_FREE(app);

   return 1;
}

static int
_action(Evry_Action *act)
{
   char buf[1024];

   char *tmp = evry->util_url_escape(act->it1.item->label, 0);

   if (EVRY_ITEM_DATA_INT_GET(act) == ACT_GOOGLE)
     {
	snprintf(buf, sizeof(buf), "http://www.google.com/search?hl=%s&q=%s",
		 _conf->lang, tmp);
     }
   else if (EVRY_ITEM_DATA_INT_GET(act) == ACT_WIKIPEDIA)
     {
	snprintf(buf, sizeof(buf), "http://%s.wikipedia.org/wiki/%s",
		 _conf->lang, tmp);
     }
   else if (EVRY_ITEM_DATA_INT_GET(act) == ACT_FEELING_LUCKY)
     {
	snprintf(buf, sizeof(buf), "http://www.google.com/search?hl=%s&q=%s&btnI=745",
		 _conf->lang, tmp);
     }
   else if (EVRY_ITEM_DATA_INT_GET(act) == ACT_YOUTUBE)
     {
	GET_WEBLINK(wl, act->it1.item);
	snprintf(buf, sizeof(buf), "%s", wl->url);
     }

   E_FREE(tmp);

   _open_url(buf);

   return 1;
}

/***************************************************************************/

static void _youtube_dl_dequeue(void);
static Eina_List *youtube_dl_queue = NULL;
static unsigned int youtube_dl_active = 0;

static void
_youtube_dl_finish(Youtube_Data *yd, int abort)
{
   youtube_dl_active--;

   if (abort)
     {
	_send_notification(yd->id, "emblem-sound", N_("Abort download"),
			   yd->label, -1);

	if (yd->exe1) ecore_exe_kill(yd->exe1);
	if (yd->exe2) ecore_exe_kill(yd->exe2);
	ecore_file_remove(yd->filepath);
	ERR("abort download\n");
     }

   download_handlers = eina_list_remove(download_handlers, yd);
   ecore_event_handler_del(yd->del_handler);
   ecore_timer_del(yd->timer);
   ecore_file_remove(yd->fifo);
   IF_RELEASE(yd->label);
   IF_RELEASE(yd->filepath);
   IF_RELEASE(yd->fifo);
   IF_RELEASE(yd->url);
   E_FREE(yd);

   _youtube_dl_dequeue();
}

static Eina_Bool
_youtube_dl_timer(void *d)
{
   Youtube_Data *yd = d;
   struct stat s;

   if (yd->ready || yd->tries++ > 10)
     {
	_youtube_dl_finish(yd, 1);
	return EINA_FALSE;
     }

   if (stat(yd->filepath, &s) == 0)
     {
	if (s.st_size < 262144)
	  {
	     if (!yd->ready && yd->tries > 5 && s.st_size < 1024)
	       {
		  _youtube_dl_finish(yd, 1);
		  return EINA_FALSE;
	       }

	     char buf[128];
	     snprintf(buf, sizeof(buf), N_("Got %d kbytes of"),
		      ((unsigned int)s.st_size / 1024));

	     _send_notification(yd->id, "emblem-sound", buf, yd->label, 5000);
	     return EINA_TRUE;
	  }

	Evry_Item_File *f = E_NEW(Evry_Item_File, 1);
	Evry_Action *act;

	f->path = eina_stringshare_ref(yd->filepath);
	f->mime = eina_stringshare_add("audio/");

	if ((yd->method == YOUTUBE_DL_ENQ) &&
	    (act = evry->action_find(N_("Enqueue in Playlist"))))
	  {
	     act->it1.item = EVRY_ITEM(f);
	     act->action(act);
	     _send_notification(yd->id, "emblem-sound", N_("Enqueue"), yd->label, -1);
	  }
	if ((yd->method == YOUTUBE_DL_PLAY) &&
	    (act = evry->action_find(N_("Play File"))))
	  {
	     act->it1.item = EVRY_ITEM(f);
	     act->action(act);
	     _send_notification(yd->id, "emblem-sound", N_("Play"), yd->label, -1);
	  }
	IF_RELEASE(f->path);
	IF_RELEASE(f->mime);
	E_FREE(f);

	yd->ready = 1;

	/* after five minutes it should be finished */
	yd->timer = ecore_timer_add(15 * 60.0, _youtube_dl_timer, yd);
	return EINA_FALSE;
     }

   return EINA_TRUE;
}

static Eina_Bool
_youtube_dl_cb_del(void *data, int type __UNUSED__, void *event)
{
   Youtube_Data *yd = data;
   Ecore_Exe_Event_Del *e = event;

   if (e->exe != yd->exe1 && e->exe != yd->exe2)
     return EINA_TRUE;

   if (e->exe == yd->exe1) ecore_exe_kill(yd->exe2);
   if (e->exe == yd->exe2) ecore_exe_kill(yd->exe1);
   yd->exe1 = NULL;
   yd->exe2 = NULL;

   _send_notification(yd->id, "emblem-sound", N_("Finished download"),
		      yd->label, -1);

   _youtube_dl_finish(yd, 0);

   return EINA_FALSE;
}

static int
_youtube_dl_data_cb(Url_Data *dd)
{
   Youtube_Data *yd = dd->user_data;

   Json_Data *rsp = NULL, *d, *d2;
   char *msg;
   const char *video_id, *t, *fmt;
   Ecore_Exe *exe = NULL;
   char url[1024];
   char buf[1024];
   char fifo[1024];

   if (!dd->data)
     goto error;

   msg = dd->data;

   msg = strstr(msg, "var swfConfig = ");
   if ((!msg) || !(msg += 16))
     goto error;

   rsp = _json_parse(msg, dd->size - (msg - dd->data));

   if ((d = _json_data_find(rsp, "args", 1)))
     {
	d2 = _json_data_find(rsp, "video_id", 3);
	if (d2) video_id = d2->value;

	d2 = _json_data_find(rsp, "t", 3);
	if (d2) t = d2->value;

	d2 = _json_data_find(rsp, "fmt_url_map", 3);
	if (d2) fmt = d2->value;
     }

   if (!t || !video_id)
     goto error;

   snprintf(url, sizeof(url),
   	    "\"http://www.youtube.com/get_video?"
   	    "video_id=%s&t=%s"
   	    "&eurl=&el=detailpage&ps=default&gl=US&hl=en&"
   	    "fmt=18\"", video_id, t);

   if (yd->method == YOUTUBE_PLAY)
     {
	snprintf(buf, sizeof(buf), _conf->player_cmd, url);
	exe = ecore_exe_run(buf, NULL);
     }
   else
     {
	snprintf(fifo, sizeof(fifo), "/tmp/convert-%f", ecore_time_unix_get());
	mkfifo(fifo, 0666);

	snprintf(buf, sizeof(buf),
		 "mplayer %s -msglevel all=2 -cache 256 -cache-min 10 "
		 "-ao pcm:fast:file=%s -vo none </dev/null",
		 url, fifo);
	yd->exe1 = ecore_exe_run(buf, yd);

	snprintf(buf, sizeof(buf),
		 "lame --quiet --tt \"%s\" --id3v2-only -V2 %s \"%s\"",
		 yd->label, fifo, yd->filepath);
	yd->exe2 = ecore_exe_run(buf, yd);

	yd->fifo = eina_stringshare_add(fifo);
	yd->tries = 0;
	yd->id = rand();
	if (!yd->id) yd->id = 1;

	yd->del_handler = ecore_event_handler_add
	  (ECORE_EXE_EVENT_DEL, _youtube_dl_cb_del, yd);

	yd->timer = ecore_timer_add(3.0, _youtube_dl_timer, yd);

	download_handlers = eina_list_append(download_handlers, yd);

	_send_notification(yd->id, "emblem-sound", N_("Start download"), yd->label, -1);
     }

   _json_data_free(rsp);
   _url_data_free(dd);
   yd->dd = NULL;
   return 1;

 error:
   ERR("parse failed!!!\n");
   _json_data_free(rsp);
   _url_data_free(dd);

   IF_RELEASE(yd->label);
   IF_RELEASE(yd->filepath);
   IF_RELEASE(yd->url);
   E_FREE(yd);

   youtube_dl_active--;
   _youtube_dl_dequeue();
   return 0;
}

static void
_youtube_dl_dequeue(void)
{
   Youtube_Data *yd;

   if (youtube_dl_queue)
     {
	yd = youtube_dl_queue->data;
	youtube_dl_queue = eina_list_remove_list(youtube_dl_queue, youtube_dl_queue);

	yd->dd = _url_data_new(yd, _youtube_dl_data_cb, NULL, NULL);
	_url_data_send(yd->dd, yd->url);
	youtube_dl_active++;
     }
}

static int
_youtube_dl_action(Evry_Action *act)
{
   GET_ITEM(it, act->it1.item);
   GET_WEBLINK(wl, it);

   int method = EVRY_ITEM_DATA_INT_GET(act);
   char path[PATH_MAX];
   Youtube_Data *yd;
   int i;
   char file[PATH_MAX];

   strcpy(file, it->label);
   for (i = 0; i < strlen(it->label); i++)
     if (file[i] == '\"' || file[i] == '/')
       file[i] = ' ';

   snprintf(path, sizeof(path), "%s/%s.mp3",
	     _conf->download_dir, file);

   for (i = 0; i < 10;)
     {
	if (!ecore_file_exists(path)) break;

	snprintf(path, sizeof(path), "%s/%s-%d.mp3",
		 _conf->download_dir, file, ++i);
     }

   printf("create file %s\n", path);

   if (i == 10) return 0;

   yd = E_NEW(Youtube_Data, 1);
   yd->label = eina_stringshare_add(file);
   yd->filepath = eina_stringshare_add(path);
   yd->url = eina_stringshare_ref(wl->url);
   yd->method = method;
   yd->num = i;

   if (((method == YOUTUBE_DL) ||
	(method == YOUTUBE_DL_ENQ)) &&
       (youtube_dl_active > 1))
     {
	_send_notification(1337, "emblem-sound", N_("Enqueue files for download"), "", -1);
	youtube_dl_queue = eina_list_append(youtube_dl_queue, yd);
     }
   else
     {
	yd->dd = _url_data_new(yd, _youtube_dl_data_cb, NULL, NULL);
	_url_data_send(yd->dd, yd->url);
	youtube_dl_active++;
     }
   return 0;
}

static int
_youtube_dl_check(Evry_Action *act, const Evry_Item *it)
{
   if (EVRY_ITEM_DATA_INT_GET(act) == ACT_YOUTUBE)
     {
	if (strcmp(it->plugin->name, N_("Youtube")))
	  return 0;
     }

   return 1;
}


/***************************************************************************/

typedef struct _Upload_Data Upload_Data;

struct _Upload_Data
{
  int id;
  const char *file;
  int progress;

  char *data;
  unsigned int size;

  Url_Data *dd;
};

static int
_upload_data(Url_Data *dd)
{
   Json_Data *d, *rsp;
   Upload_Data *ud = dd->user_data;
   int len;

   if (ud->data) free(ud->data);

   rsp = _json_parse(dd->data, dd->size);
   d = _json_data_find(rsp, "imgur_page", 5);

   if (d)
     {
	len = strlen(d->value);
	ecore_x_selection_primary_set(ecore_x_window_root_first_get(), d->value, len);
	ecore_x_selection_clipboard_set(ecore_x_window_root_first_get(), d->value, len);
	_send_notification(ud->id, "image", N_("Upload Image"), N_("Link copied to clipboard") , -1);
	_open_url(d->value);
     }
   else
     {
	_send_notification(ud->id, "image", N_("Upload Image"), N_("Something went wrong :("), -1);
     }

   _json_data_free(rsp);

   _url_data_free(dd);
   eina_stringshare_del(ud->file);
   E_FREE(ud);

   return 1;
}

static int
_upload_progress(Url_Data *dd, Ecore_Con_Event_Url_Progress *ev)
{
   Upload_Data *ud = dd->user_data;
   double up;

   up = (ev->up.now / ev->up.total) * 20.0;

   if ((int) up > ud->progress)
     {
	char buf[128];

	ud->progress = up;
	snprintf(buf, sizeof(buf), N_("Completed %1.1f%% of %s"), up * 5.0, ud->file);

	_send_notification(ud->id, "image", N_("Upload Image"), buf, -1);
     }

   return 1;
}

static size_t
_cb_curl_form_get(void *data, const char *buf, size_t len)
{
   Upload_Data *ud = data;

   ud->data = realloc(ud->data, sizeof(char) * (ud->size + len));
   memcpy(ud->data + ud->size, buf, len);
   ud->size += len;

   return len;
}

static int
_action_upload(Evry_Action *act)
{
   struct curl_httppost* post = NULL;
   struct curl_httppost* last = NULL;
   Upload_Data *ud;

   GET_FILE(file, act->it1.item);
   if (!evry->file_path_get(file))
     return 0;

   ud = E_NEW(Upload_Data, 1);
   ud->dd = _url_data_new(ud, _upload_data, _upload_progress, NULL);
   ecore_con_url_url_set(ud->dd->con_url, "http://imgur.com/api/upload.json");
   ud->id = rand() + 1;
   ud->file = eina_stringshare_ref(act->it1.item->label);

   curl_formadd(&post, &last,
		CURLFORM_COPYNAME, "key",
		CURLFORM_COPYCONTENTS, _imgur_key,
		CURLFORM_END);

   curl_formadd(&post, &last,
		CURLFORM_COPYNAME, "image",
		CURLFORM_FILE, file->path,
		CURLFORM_END);

   if (!curl_formget(post, ud, _cb_curl_form_get))
     {
   	int i;
   	char buf[200];
   	const char *line;

	/* first line is: Content-Type: ... */
	for(i = 0, line = ud->data; i < 200 && *line != '\n'; i++, line++);
	/* strip 'Content-Type:' == 14 */
   	snprintf(buf, i-14, "%s", ud->data+14);

   	ecore_con_url_post(ud->dd->con_url, ud->data+i, ud->size-i, buf);
     }

   /* ecore_con_url_http_post_send(ud->dd->con_url, post); */

   _send_notification(ud->id, "image", N_("Upload Image"), ud->file, -1);

   return 0;
}

Evas_Object *
_icon_get(Evry_Item *it, Evas *e)
{
   Evas_Object *o;

   if (!it->icon)
     return NULL;

   if (edje_file_group_exists(_conf->theme, it->icon))
     {
	o = e_icon_add(e);
	e_icon_preload_set(o, 1);
	if (e_icon_file_edje_set(o, _conf->theme, it->icon))
	  return o;

	evas_object_del(o);
     }

   o = evry->icon_theme_get(it->icon, e);

   return o;
}

static int
_action_upload_check(Evry_Action *act, const Evry_Item *it)
{
   return (EVRY_FILE(it)->mime && !(strncmp(EVRY_FILE(it)->mime, "image/", 6)));
}


/***************************************************************************/

static int
_complete(Evry_Plugin *p, const Evry_Item *item, char **input)
{
   char buf[128];
   snprintf(buf, sizeof(buf), "%s ", item->label);

   *input = strdup(buf);

   return EVRY_COMPLETE_INPUT;
}

static int
_plugins_init(const Evry_API *_api)
{
   Evry_Plugin *p;
   Evry_Action *act;

   evry = _api;

   if (!evry->api_version_check(EVRY_API_VERSION))
     return EINA_FALSE;

   WEBLINK = evry->type_register("WEBLINK");

#define PLUGIN_NEW(_name, _type, _icon, _begin, _finish, _fetch, _complete, _request, _data_cb, _host, _trigger) { \
      p = EVRY_PLUGIN_NEW(Plugin, _name, _icon, _type, _begin, _finish, _fetch, NULL); \
      p->config_path = _config_path;					\
      plugins = eina_list_append(plugins, p);				\
      p->complete = _complete;						\
      p->input_type = EVRY_TYPE_TEXT;					\
      EVRY_ITEM(p)->icon_get = &_icon_get;				\
      GET_PLUGIN(plug, p);						\
      plug->request = _request;						\
      plug->data_cb = _data_cb;						\
      if (evry->plugin_register(p, EVRY_PLUGIN_SUBJECT, 10)) {		\
	 Plugin_Config *pc = p->config;					\
	 pc->view_mode = VIEW_MODE_LIST;				\
	 pc->aggregate = EINA_FALSE;					\
	 pc->top_level = EINA_TRUE;					\
	 pc->view_mode = VIEW_MODE_DETAIL;				\
	 pc->min_query = 3;						\
	 pc->trigger_only = EINA_TRUE;					\
	 pc->trigger = eina_stringshare_add(_trigger); }}		\

   PLUGIN_NEW("Google", EVRY_TYPE_TEXT, "google",
	      _begin, _finish, _fetch, &_complete,
	      _request_goolge, _google_data_cb,
	      NULL, _trigger_google);

   PLUGIN_NEW("Wikipedia", EVRY_TYPE_TEXT, "wikipedia",
	      _begin, _finish, _fetch, &_complete,
	      _request_wiki, _wikipedia_data_cb,
	      NULL, _trigger_wiki);

   PLUGIN_NEW("Youtube", WEBLINK, "youtube",
	      _begin, _finish, _fetch, &_complete,
	      _request_youtube, _youtube_data_cb,
	      "gdata.youtube.com", _trigger_youtube);

   PLUGIN_NEW("Translate", EVRY_TYPE_TEXT, "text-html",
	      _begin, _finish, _fetch, NULL,
	      _request_gtranslate, _gtranslate_data_cb,
	      "ajax.googleapis.com", _trigger_gtranslate);

#define ACTION_NEW(_name, _type, _icon, _action, _check, _method)	\
   act = EVRY_ACTION_NEW(_name, _type, 0, _icon, _action, _check);	\
   act->remember_context = EINA_TRUE;					\
   EVRY_ITEM_DATA_INT_SET(act, _method);				\
   EVRY_ITEM(act)->icon_get = &_icon_get;				\
   evry->action_register(act, 1);				       	\
   actions = eina_list_append(actions, act);				\

   ACTION_NEW("Google for it", EVRY_TYPE_TEXT, "google",
	      _action, NULL, ACT_GOOGLE);

   ACTION_NEW("Wikipedia Page", EVRY_TYPE_TEXT, "wikipedia",
	      _action, NULL, ACT_WIKIPEDIA);

   ACTION_NEW("Feeling Lucky", EVRY_TYPE_TEXT, "feeling-lucky",
	      _action, NULL, ACT_FEELING_LUCKY);

   ACTION_NEW("Watch on Youtube", WEBLINK, "youtube",
	      _action, _youtube_dl_check, ACT_YOUTUBE);

#if 0
   ACTION_NEW("Download as Audio", WEBLINK, "youtube",
	      _youtube_dl_action, _youtube_dl_check, YOUTUBE_DL);

   ACTION_NEW("Play Video", WEBLINK, "youtube",
	      _youtube_dl_action, _youtube_dl_check, YOUTUBE_PLAY);

   ACTION_NEW("Download and enqueue", WEBLINK, "youtube",
	      _youtube_dl_action, _youtube_dl_check, YOUTUBE_DL_ENQ);

   ACTION_NEW("Download and play", WEBLINK, "youtube",
	      _youtube_dl_action, _youtube_dl_check, YOUTUBE_DL_PLAY);
#endif

   ACTION_NEW("Upload Image", EVRY_TYPE_FILE, "image",
	      _action_upload, _action_upload_check, ACT_UPLOAD_IMGUR);
   act->remember_context = EINA_TRUE;


   handlers = eina_list_append
     (handlers, ecore_event_handler_add
      (ECORE_CON_EVENT_URL_DATA, _common_data_cb, _conf));

   handlers = eina_list_append
     (handlers, ecore_event_handler_add
      (ECORE_CON_EVENT_URL_PROGRESS, _common_progress_cb, _conf));

   handlers = eina_list_append
     (handlers, ecore_event_handler_add
      (ECORE_CON_EVENT_URL_COMPLETE, _common_complete_cb, _conf));

   return EINA_TRUE;
}

static void
_plugins_shutdown(void)
{
   Evry_Plugin *p;
   Evry_Action *act;
   Ecore_Event_Handler *h;

   EINA_LIST_FREE(plugins, p)
     EVRY_PLUGIN_FREE(p);

   EINA_LIST_FREE(actions, act)
     evry->action_free(act);

   EINA_LIST_FREE(handlers, h)
     ecore_event_handler_del(h);
}

/***************************************************************************/

struct _E_Config_Dialog_Data
{
  char *browser;
  char *lang;
  char *translate;
  char *convert_cmd;
  char *player_cmd;
  char *download_dir;
};

static void *_create_data(E_Config_Dialog *cfd);
static void _free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);
static Evas_Object *_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata);
static int _basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata);

static E_Config_Dialog *
_conf_dialog(E_Container *con, const char *params)
{
   E_Config_Dialog *cfd = NULL;
   E_Config_Dialog_View *v = NULL;
   char buf[4096];

   if (e_config_dialog_find(_config_path, _config_path))
     return NULL;

   v = E_NEW(E_Config_Dialog_View, 1);
   if (!v) return NULL;

   v->create_cfdata = _create_data;
   v->free_cfdata = _free_data;
   v->basic.create_widgets = _basic_create;
   v->basic.apply_cfdata = _basic_apply;

   snprintf(buf, sizeof(buf), "%s/e-module.edj", _conf->module->dir);

   cfd = e_config_dialog_new(con, _("Everything Websearch"),
			     _config_path, _config_path, buf, 0, v, NULL);

   _conf->cfd = cfd;
   return cfd;
}

static Evas_Object *
_basic_create(E_Config_Dialog *cfd, Evas *evas, E_Config_Dialog_Data *cfdata)
{
   Evas_Object *o = NULL, *of = NULL, *ow = NULL;

   o = e_widget_list_add(evas, 0, 0);

   of = e_widget_framelist_add(evas, _("General"), 0);
   e_widget_framelist_content_align_set(of, 0.0, 0.0);

   ow = e_widget_label_add(evas, _("Browser"));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_entry_add(evas, &cfdata->browser, NULL, NULL, NULL);
   e_widget_framelist_object_append(of, ow);

   ow = e_widget_label_add(evas, _("Language"));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_entry_add(evas, &cfdata->lang, NULL, NULL, NULL);
   e_widget_framelist_object_append(of, ow);

   ow = e_widget_label_add(evas, _("Translate"));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_entry_add(evas, &cfdata->translate, NULL, NULL, NULL);
   e_widget_framelist_object_append(of, ow);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
#if 0
   of = e_widget_framelist_add(evas, _("Youtube"), 0);
   e_widget_framelist_content_align_set(of, 0.0, 0.0);

   ow = e_widget_label_add(evas, _("Video player"));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_entry_add(evas, &cfdata->player_cmd, NULL, NULL, NULL);
   e_widget_framelist_object_append(of, ow);

   ow = e_widget_label_add(evas, _("Download directory"));
   e_widget_framelist_object_append(of, ow);
   ow = e_widget_entry_add(evas, &cfdata->download_dir, NULL, NULL, NULL);
   e_widget_framelist_object_append(of, ow);

   ow = e_widget_label_add(evas, _("Download requires mplayer and mp3lame"));
   e_widget_framelist_object_append(of, ow);
   e_widget_list_object_append(o, of, 1, 1, 0.5);
#endif
   return o;
}

static void *
_create_data(E_Config_Dialog *cfd)
{
   E_Config_Dialog_Data *cfdata = NULL;
   cfdata = E_NEW(E_Config_Dialog_Data, 1);

#define CP(_name) cfdata->_name = strdup(_conf->_name);
#define C(_name) cfdata->_name = _conf->_name;
   CP(browser);
   CP(lang);
   CP(translate);
   CP(convert_cmd);
   CP(download_dir);
   CP(player_cmd);
#undef CP
#undef C
   return cfdata;
}

static void
_free_data(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
   E_FREE(cfdata->browser);
   E_FREE(cfdata->lang);
   E_FREE(cfdata->translate);
   E_FREE(cfdata->convert_cmd);
   E_FREE(cfdata->player_cmd);
   E_FREE(cfdata->download_dir);
   _conf->cfd = NULL;
   E_FREE(cfdata);
}

static int
_basic_apply(E_Config_Dialog *cfd, E_Config_Dialog_Data *cfdata)
{
#define CP(_name)					\
   if (_conf->_name)					\
     eina_stringshare_del(_conf->_name);		\
   _conf->_name = eina_stringshare_add(cfdata->_name);
#define C(_name) _conf->_name = cfdata->_name;
   CP(browser);
   CP(lang);
   CP(translate);
   CP(convert_cmd);
   CP(download_dir);
   CP(player_cmd);
#undef CP
#undef C

   e_config_domain_save(_config_domain, _conf_edd, _conf);
   e_config_save_queue();
   return 1;
}

static void
_conf_new(void)
{
   if (!_conf)
     {
	_conf = E_NEW(Module_Config, 1);
	_conf->version = (MOD_CONFIG_FILE_EPOCH << 16);
     }

#define IFMODCFG(v) if ((_conf->version & 0xffff) < v) {
#define IFMODCFGEND }

   /* setup defaults */
   IFMODCFG(0x008d);
   _conf->browser = eina_stringshare_add("firefox");
   _conf->lang = eina_stringshare_add("en");
   IFMODCFGEND;

   IFMODCFG(0x009d);
   _conf->translate = eina_stringshare_add("en|de");
   IFMODCFGEND;

   IFMODCFG(0x00dd);
   _conf->convert_cmd = eina_stringshare_add("");
   IFMODCFGEND;

   IFMODCFG(0x00ed);
   _conf->player_cmd = eina_stringshare_add("mplayer -fs %s");
   IFMODCFGEND;

   IFMODCFG(0x00ee);
   IF_RELEASE(_conf->download_dir);
   _conf->download_dir = eina_stringshare_add(e_user_homedir_get());
   IFMODCFGEND;

   _conf->version = MOD_CONFIG_FILE_VERSION;

   e_config_save_queue();
}

static void
_conf_free(void)
{
   if (!_conf) return;
   eina_stringshare_del(_conf->browser);
   eina_stringshare_del(_conf->lang);
   eina_stringshare_del(_conf->translate);
   eina_stringshare_del(_conf->convert_cmd);
   eina_stringshare_del(_conf->player_cmd);
   eina_stringshare_del(_conf->download_dir);
   free(_conf->theme);
   E_FREE(_conf);
}

static void
_conf_init(E_Module *m)
{
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/e-module.edj", m->dir);

   e_configure_registry_category_add
     ("launcher", 80, _("Launcher"), NULL, "modules-launcher");

   e_configure_registry_item_add
     (_config_path, 110, _("Everything Websearch"), NULL, buf, _conf_dialog);

   _conf_edd = E_CONFIG_DD_NEW("Module_Config", Module_Config);

#undef T
#undef D
#define T Module_Config
#define D _conf_edd
   E_CONFIG_VAL(D, T, version, INT);
   E_CONFIG_VAL(D, T, browser, STR);
   E_CONFIG_VAL(D, T, lang, STR);
   E_CONFIG_VAL(D, T, translate, STR);
   E_CONFIG_VAL(D, T, convert_cmd, STR);
   E_CONFIG_VAL(D, T, player_cmd, STR);
   E_CONFIG_VAL(D, T, download_dir, STR);
#undef T
#undef D
   _conf = e_config_domain_load(_config_domain, _conf_edd);

   if (_conf && !e_util_module_config_check
       (_("Everything Websearch"), _conf->version,
	MOD_CONFIG_FILE_EPOCH, MOD_CONFIG_FILE_VERSION))
     {
	_conf_free();
     }

   _conf_new();

   _conf->module = m;
   _conf->theme = strdup(buf);
}

static void
_conf_shutdown(void)
{
   e_configure_registry_item_del(_config_path);
   e_configure_registry_category_del("launcher");

   _conf_free();

   E_CONFIG_DD_FREE(_conf_edd);
}

/***************************************************************************/

EAPI E_Module_Api e_modapi =
  {
    E_MODULE_API_VERSION,
    PACKAGE
  };

EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   e_notification_init();
   ecore_con_url_init();
   _conf_init(m);

   snprintf(buf, sizeof(buf), "%s/.cache/youtube", e_user_homedir_get());
   if (!ecore_file_exists(buf))
     ecore_file_mkdir(buf);

   EVRY_MODULE_NEW(evry_module, evry, _plugins_init, _plugins_shutdown);
   
   e_module_delayed_set(m, 1);

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   EVRY_MODULE_FREE(evry_module);
   
   _conf_shutdown();
   e_notification_shutdown();
   ecore_con_url_shutdown();

   /* XXX free download handler */
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   e_config_domain_save(_config_domain, _conf_edd, _conf);
   return 1;
}

/***************************************************************************/

static int
_parse_callback(void *userdata, int type, const char *data, uint32_t length)
{
   Json_Data *d = userdata;
   Json_Data *d2;

   switch (type)
     {
      case JSON_OBJECT_BEGIN:
      case JSON_ARRAY_BEGIN:
	 d2 = E_NEW(Json_Data, 1);
	 if (d->cur->key)
	   d2->is_val = 1;
	 d2->parent = d->cur;
	 d2->type = type;
	 d->cur->list = eina_list_append(d->cur->list, d2);
	 d->cur = d2;
	 break;

      case JSON_OBJECT_END:
      case JSON_ARRAY_END:
	 if (d->cur->is_val)
	   d->cur = d->cur->parent;
	 d->cur = d->cur->parent;
	 break;

      case JSON_KEY:
	 d2 = E_NEW(Json_Data, 1);
	 d2->key = eina_stringshare_add_length(data, length);
	 d2->parent = d->cur;
	 d->cur->list =  eina_list_append(d->cur->list, d2);
	 d->cur = d2;
	 break;

      case JSON_STRING:
      case JSON_INT:
      case JSON_FLOAT:
	 if (d->cur->type == JSON_ARRAY_BEGIN)
	   {
	      d->cur->values = eina_list_append
		(d->cur->values, eina_stringshare_add_length(data, length));
	   }
	 else
	   {
	      d->cur->type = type;
	      if (d->cur->value) eina_stringshare_del(d->cur->value);
	      d->cur->value = eina_stringshare_add_length(data, length);
	      d->cur = d->cur->parent;
	   }
	 break;

      case JSON_NULL:
      case JSON_TRUE:
      case JSON_FALSE:
	 d->cur = d->cur->parent;
     }

   return 0;
}

static Json_Data *
_json_data_find2(const Json_Data *jd, const char *key, int level)
{
   Json_Data *d = NULL;
   Eina_List *l;

   if (!jd) return NULL;

   EINA_LIST_FOREACH(jd->list, l, d)
     {
	if (d && d->key == key)
	  {
	     //DBG("found %d %s",level, key);
	     break;
	  }

	if (level && (d = _json_data_find2(d, key, level - 1)))
	  break;
     }

   return d;
}

static Json_Data *
_json_data_find(const Json_Data *jd, const char *key, int level)
{
   Json_Data *d;
   const char *k;

   k = eina_stringshare_add(key);
   d = _json_data_find2(jd, k, level);
   eina_stringshare_del(k);

   return d;
}

static void
_json_data_free(Json_Data *jd)
{
   Json_Data *d;
   const char *val;
   if (!jd) return;

   EINA_LIST_FREE(jd->list, d)
     {
	if (d->key) eina_stringshare_del(d->key);
	if (d->value) eina_stringshare_del(d->value);
	EINA_LIST_FREE(d->values, val)
	  {
	     eina_stringshare_del(val);
	  }
	_json_data_free(d);
     }

   E_FREE(jd);
}

static Json_Data *
_json_parse(const char *string, int len)
{
   struct json_parser parser;
   int ret;
   Json_Data *d;

   if (!string)
     return NULL;

   if (!len)
     len = strlen(string);

   d = E_NEW(Json_Data, 1);
   d->cur = d;

   if (json_parser_init(&parser, NULL, _parse_callback, d))
     {
	ERR("something wrong happened in parser init");
	E_FREE(d);
	return NULL;
     }

   if ((ret = json_parser_string(&parser, string, len, NULL)) && !(d->cur == d))
     {
	ERR("%d\n", ret);
	_json_data_free(d);
	d = NULL;
     }

   json_parser_free(&parser);

   return d;
}
