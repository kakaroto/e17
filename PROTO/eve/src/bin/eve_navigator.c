#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <Ecore.h>
#include <Edje.h>
#include <EWebKit.h>

#include "eve_navigator.h"
#include "eve_scrolled_webview.h"

#define EVE_NAVIGATOR_DATA_GET(o, ptr)				\
  Eve_Navigator_Data *ptr = evas_object_smart_data_get(o)

#define EVE_NAVIGATOR_DATA_GET_OR_RETURN(o, ptr)			\
  EVE_NAVIGATOR_DATA_GET(o, ptr);					\
  if (!ptr)								\
    {									\
       fprintf(stderr, "CRITICAL: no widget data for object %p (%s)\n",	\
	       o, evas_object_type_get(o));				\
       fflush(stderr);							\
       abort();								\
       return;								\
    }

#define EVE_NAVIGATOR_DATA_GET_OR_RETURN_VAL(o, ptr, val)		\
  EVE_NAVIGATOR_DATA_GET(o, ptr);					\
  if (!ptr)								\
    {									\
       fprintf(stderr, "CRITICAL: no widget data for object %p (%s)\n",	\
	       o, evas_object_type_get(o));				\
       fflush(stderr);							\
       abort();								\
       return val;							\
    }

typedef struct _Eve_Navigator_Data Eve_Navigator_Data;

struct _Eve_Navigator_Data
{
   Evas_Object_Smart_Clipped_Data base;
   Evas_Object *edje;
   Evas_Object *scrolled;
   Evas_Object *webview;
   Evas_Object *page;
   const char *file;
   const char *group;
   const char *url;
   const char *title;
};

static Evas_Smart_Class _parent_sc = {NULL};
static const char EDJE_PART_CONTENT[] = "eve.swallow.content";
static const char EDJE_PART_LOCATION[] = "eve.text.location";

static void
_eve_navigator_load_url(Eve_Navigator_Data *priv, const char *url)
{
   char *buf;

   if (!url)
     return;

   if (!strstr(url, "://"))
     {
        int len = strlen(url);
        buf = alloca(len + sizeof("http://")); /* includes \0 */
        memcpy(buf, "http://", sizeof("http://") - 1);
        memcpy(buf + sizeof("http://") - 1, url, len + 1);
        url = buf;
     }

   if (!eina_stringshare_replace(&priv->url, url))
     return;

   ewk_webview_object_load_url(priv->webview, priv->url);
   edje_object_part_text_set(priv->edje, EDJE_PART_LOCATION, priv->url);
}

static void
_eve_navigator_on_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eve_Navigator_Data *priv = data;
   Evas_Event_Key_Down *ev = event_info;
   char *value;

   value = ev->keyname;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     return;
   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;

   if (strcmp(value, "F1") == 0)
     ewk_webview_object_navigation_back(priv->webview);
   else if (strcmp(value, "F2") == 0)
     ewk_webview_object_navigation_forward(priv->webview);
   else if (strcmp(value, "F5") == 0)
     ewk_webview_object_navigation_reload(priv->webview);
   else if (strcmp(value, "F6") == 0)
     ewk_webview_object_navigation_stop(priv->webview);
   else
     ev->event_flags &= ~EVAS_EVENT_FLAG_ON_HOLD; /* unset on_hold */
}

static void
_eve_navigator_on_progress(void *data, Evas_Object *obj, void *event_info)
{
   Eve_Navigator_Data *priv = data;
   EWebKit_Event_Load_Progress *ev = event_info;
   Edje_Message_Float msg;
   msg.val = ev->load_progress / 100.0;
   edje_object_message_send(priv->edje, EDJE_MESSAGE_FLOAT, 0, &msg);
}

static void
_eve_navigator_on_load_started(void *data, Evas_Object *obj, void *event_info)
{
   Eve_Navigator_Data *priv = data;
   EWebKit_Event_Load_Started *ev = event_info;
   if (ev->load_started)
     edje_object_signal_emit(priv->edje, "eve,action,load", "");
   else
     fputs("DBG: load started, but flag is not set.\n", stderr);
}

static void
_eve_navigator_on_load_finished(void *data, Evas_Object *obj, void *event_info)
{
   Eve_Navigator_Data *priv = data;
   EWebKit_Event_Load_Finished *ev = event_info;
   if (ev->load_succeeded)
     edje_object_signal_emit(priv->edje, "eve,action,load,done", "");
   else
     fputs("DBG: load finished, but load succeeded flag is not set.\n", stderr);
}

static void
_eve_navigator_on_title_changed(void *data, Evas_Object *obj, void *event_info)
{
   Eve_Navigator_Data *priv = data;
   EWebKit_Event_Title_Changed *ev = event_info;
   if (!eina_stringshare_replace(&priv->title, ev->title))
     return;
   edje_object_part_text_set(priv->edje, "eve.text.title", priv->title);
}

static void
_eve_navigator_on_url_changed(void *data, Evas_Object *obj, void *event_info)
{
   Eve_Navigator_Data *priv = data;
   EWebKit_Event_Url_Changed *ev = event_info;
   if (!eina_stringshare_replace(&priv->url, ev->url))
     return;
   edje_object_part_text_set(priv->edje, "eve.text.location", priv->url);
}

static void
_eve_navigator_on_scroll_requested(void *data, Evas_Object *obj, void *event_info)
{
   EWebKit_Event_Scroll_Requested *ev = event_info;

   printf("TODO: scroll requested: %dx%d %d,%d+%dx%d\n",
	  ev->width, ev->height,
	  ev->view.x, ev->view.y, ev->view.w, ev->view.h);
}

static void
_eve_navigator_back(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Eve_Navigator_Data *priv = data;
   ewk_webview_object_navigation_back(priv->webview);
}

static void
_eve_navigator_forward(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Eve_Navigator_Data *priv = data;
   ewk_webview_object_navigation_forward(priv->webview);
}

static void
_eve_navigator_reload(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Eve_Navigator_Data *priv = data;
   ewk_webview_object_navigation_reload(priv->webview);
}

static void
_eve_navigator_favorite(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Eve_Navigator_Data *priv = data;
   fprintf(stderr, "TODO: save url '%s' as favorite.\n", priv->url);
}

static void
_eve_navigator_go(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Eve_Navigator_Data *priv = data;
   const char *url;
   char *buf;
   int i, j, intag;

   url = edje_object_part_text_get(priv->edje, EDJE_PART_LOCATION);
   if (!url)
     {
        fputs("ERROR: no url.\n", stderr);
        return;
     }

   buf = alloca(strlen(url) + 1);
   j = 0;
   intag = 0;
   for (i = 0; url[i] != '\0'; i++)
     {
        if (url[i] == '>')
	  intag = 0;
        else if (url[i] == '<')
	  intag = 1;
        else if (!intag)
	  buf[j++] = url[i];
     }
   buf[j] = '\0';

   _eve_navigator_load_url(priv, buf);
}

static void
_eve_navigator_control(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   fputs("TODO: enter contig mode.\n", stderr);
}

static void
_eve_navigator_focus_location(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Eve_Navigator_Data *priv = data;
   evas_object_focus_set(priv->edje, 1);
}

static const struct edje_signal_map
{
   const char *sig;
   void (*cb)(void *, Evas_Object *o, const char *, const char *);
} edje_signal_map[] = {
  {"eve,back", _eve_navigator_back},
  {"eve,forward", _eve_navigator_forward},
  {"eve,reload", _eve_navigator_reload},
  {"eve,favorite", _eve_navigator_favorite},
  {"eve,go", _eve_navigator_go},
  {"eve,control", _eve_navigator_control},
  {"eve,focus,location", _eve_navigator_focus_location},
  {NULL, NULL}
};

static bool
_eve_navigator_edje_reset(Eve_Navigator_Data *priv)
{
   const struct edje_signal_map *itr;

   for (itr = edje_signal_map; itr->sig != NULL; itr++)
     edje_object_signal_callback_del(priv->edje, itr->sig, "", itr->cb);

   edje_object_part_unswallow(priv->edje, priv->scrolled);

   ewk_theme_set(priv->file);

   if (!edje_object_file_set(priv->edje, priv->file, priv->group))
     {
	int error = edje_object_load_error_get(priv->edje);
	const char *msg = edje_load_error_str(error);
	fprintf(stderr,
		"ERROR: could not load theme from file '%s', group '%s': %s\n",
		priv->file, priv->group, msg);
	evas_object_hide(priv->scrolled);
	return 0;
     }

   edje_object_part_swallow(priv->edje, EDJE_PART_CONTENT, priv->scrolled);

   for (itr = edje_signal_map; itr->sig != NULL; itr++)
     edje_object_signal_callback_add(priv->edje, itr->sig, "", itr->cb, priv);

   return 1;
}

static void
_eve_navigator_smart_del(Evas_Object *o)
{
   EVE_NAVIGATOR_DATA_GET(o, priv);

   evas_object_del(priv->scrolled); /* edje does not delete its children */

   eina_stringshare_del(priv->file);
   eina_stringshare_del(priv->group);
   eina_stringshare_del(priv->url);
   eina_stringshare_del(priv->title);

   _parent_sc.del(o);
}

static void
_eve_navigator_smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   EVE_NAVIGATOR_DATA_GET(o, priv);
   evas_object_resize(priv->edje, w, h);
}

static void
_eve_navigator_smart_add(Evas_Object *o)
{
   Eve_Navigator_Data *priv = calloc(1, sizeof(*priv));
   if (!priv)
     {
	fprintf(stderr, "ERROR: could not allocate priv data %d bytes: %s\n",
		sizeof(*priv), strerror(errno));
	return;
     }
   evas_object_smart_data_set(o, priv);

   priv->file = eina_stringshare_add(PACKAGE_DATA_DIR "/data/b_and_w.edj");
   priv->group = eina_stringshare_add("eve.navigator");

   _parent_sc.add(o);

   priv->edje = edje_object_add(priv->base.evas);
   evas_object_show(priv->edje);
   evas_object_smart_member_add(priv->edje, o);

   priv->scrolled = eve_scrolled_webview_add(priv->base.evas);
   if (!priv->scrolled)
     {
	fputs("ERROR: could not create scrolled webview.\n", stderr);
	return;
     }
   priv->webview = eve_scrolled_webview_get(priv->scrolled);
   priv->page = eve_scrolled_webview_page_get(priv->scrolled);

   ewk_callback_load_progress_add
     (priv->webview, _eve_navigator_on_progress, priv);
   ewk_callback_load_started_add
     (priv->webview, _eve_navigator_on_load_started, priv);
   ewk_callback_load_finished_add
     (priv->webview, _eve_navigator_on_load_finished, priv);
   ewk_callback_title_changed_add
     (priv->webview, _eve_navigator_on_title_changed, priv);
   ewk_callback_url_changed_add
     (priv->webview, _eve_navigator_on_url_changed, priv);
   ewk_callback_scroll_requested_add
     (priv->webview, _eve_navigator_on_scroll_requested, priv);

   evas_object_event_callback_add
     (o, EVAS_CALLBACK_KEY_DOWN, _eve_navigator_on_key_down, priv);

   _eve_navigator_edje_reset(priv);
}

static Evas_Smart *
_eve_navigator_smart_new(void)
{
   static Evas_Smart_Class sc = {
     "Eve_Navigator", EVAS_SMART_CLASS_VERSION, NULL
   };

   if (!_parent_sc.name)
     {
	evas_object_smart_clipped_smart_set(&sc);
	_parent_sc = sc;
	sc.add = _eve_navigator_smart_add;
	sc.del = _eve_navigator_smart_del;
	sc.resize = _eve_navigator_smart_resize;
     }

   return evas_smart_class_new(&sc);
}

Evas_Object *
eve_navigator_add(Evas *e)
{
   static Evas_Smart *smart = NULL;
   if (!smart)
     smart = _eve_navigator_smart_new();
   return evas_object_smart_add(e, smart);
}

bool
eve_navigator_theme_file_set(Evas_Object *o, const char *file)
{
   EVE_NAVIGATOR_DATA_GET_OR_RETURN_VAL(o, priv, 0);
   if (!eve_scrolled_webview_theme_file_set(priv->scrolled, file))
     return 0;
   eina_stringshare_replace(&priv->file, file);
   return _eve_navigator_edje_reset(priv);
}

const char *
eve_navigator_theme_file_get(const Evas_Object *o)
{
   EVE_NAVIGATOR_DATA_GET_OR_RETURN_VAL(o, priv, NULL);
   return priv->file;
}

bool
eve_navigator_theme_group_set(Evas_Object *o, const char *group)
{
   EVE_NAVIGATOR_DATA_GET_OR_RETURN_VAL(o, priv, 0);
   eina_stringshare_replace(&priv->group, group);
   return _eve_navigator_edje_reset(priv);
}

const char *
eve_navigator_theme_group_get(const Evas_Object *o)
{
   EVE_NAVIGATOR_DATA_GET_OR_RETURN_VAL(o, priv, NULL);
   return priv->group;
}

Evas_Object *
eve_navigator_scrolled_get(const Evas_Object *o)
{
   EVE_NAVIGATOR_DATA_GET_OR_RETURN_VAL(o, priv, NULL);
   return priv->scrolled;
}

Evas_Object *
eve_navigator_webview_get(const Evas_Object *o)
{
   EVE_NAVIGATOR_DATA_GET_OR_RETURN_VAL(o, priv, NULL);
   return priv->webview;
}

Evas_Object *
eve_navigator_page_get(const Evas_Object *o)
{
   EVE_NAVIGATOR_DATA_GET_OR_RETURN_VAL(o, priv, NULL);
   return priv->page;
}

void
eve_navigator_load_url(Evas_Object *o, const char *url)
{
   EVE_NAVIGATOR_DATA_GET_OR_RETURN(o, priv);
   _eve_navigator_load_url(priv, url);
}
