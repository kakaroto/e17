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

#include "eve_scrolled_webview.h"

#define EVE_SCROLLED_WEBVIEW_DATA_GET(o, ptr)				\
  Eve_Scrolled_Webview_Data *ptr = evas_object_smart_data_get(o)

#define EVE_SCROLLED_WEBVIEW_DATA_GET_OR_RETURN(o, ptr)		\
  EVE_SCROLLED_WEBVIEW_DATA_GET(o, ptr);				\
  if (!ptr)								\
    {									\
       fprintf(stderr, "CRITICAL: no widget data for object %p (%s)\n",	\
	       o, evas_object_type_get(o));				\
       fflush(stderr);							\
       abort();								\
       return;								\
    }

#define EVE_SCROLLED_WEBVIEW_DATA_GET_OR_RETURN_VAL(o, ptr, val)	\
  EVE_SCROLLED_WEBVIEW_DATA_GET(o, ptr);				\
  if (!ptr)								\
    {									\
       fprintf(stderr, "CRITICAL: no widget data for object %p (%s)\n",	\
	       o, evas_object_type_get(o));				\
       fflush(stderr);							\
       abort();								\
       return val;							\
    }

typedef struct _Eve_Scrolled_Webview_Data Eve_Scrolled_Webview_Data;

struct _Eve_Scrolled_Webview_Data
{
   Evas_Object_Smart_Clipped_Data base;
   Evas_Object *edje;
   Evas_Object *webview;
   Evas_Object *page;
   Evas_Object *mainframe;
   const char *file;
   const char *group;
   struct
   {
      Evas_Coord x, y;
      Ecore_Animator *anim;
   } mouse_move;
};

static Evas_Smart_Class _parent_sc = {NULL};
static const char EDJE_PART_CONTENT[] = "eve.swallow.content";

static void
_eve_scrolled_webview_on_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Key_Down *ev = event_info;
   char *value;

   value = ev->keyname;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     {
	fprintf(stderr, "DBG: key down event '%s' is on hold, ignored.\n",
		value);
	return;
     }

   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;

   if (strcmp(value, "Home") == 0)
     printf("Command:home\n");
   else if (strcmp(value, "End") == 0)
     printf("Command:end\n");
   else
     ev->event_flags &= ~EVAS_EVENT_FLAG_ON_HOLD; /* unset on_hold */
}

static void
_eve_scrolled_webview_on_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eve_Scrolled_Webview_Data *priv = data;

   evas_object_focus_set(priv->webview, 1);
}

static void
_eve_scrolled_webview_smart_del(Evas_Object *o)
{
   EVE_SCROLLED_WEBVIEW_DATA_GET(o, priv);

   evas_object_del(priv->webview); /* edje does not delete its children */

   eina_stringshare_del(priv->file);
   eina_stringshare_del(priv->group);

   _parent_sc.del(o);
}

static void
_eve_scrolled_webview_smart_resize(Evas_Object *o, Evas_Coord w, Evas_Coord h)
{
   EVE_SCROLLED_WEBVIEW_DATA_GET(o, priv);
   evas_object_resize(priv->edje, w, h);
}

static bool
_eve_scrolled_webview_edje_reset(Eve_Scrolled_Webview_Data *priv)
{
   edje_object_part_unswallow(priv->edje, priv->webview);

   if (!edje_object_file_set(priv->edje, priv->file, priv->group))
     {
	int error = edje_object_load_error_get(priv->edje);
	const char *msg = edje_load_error_str(error);
	fprintf(stderr,
		"ERROR: could not load theme from file '%s', group '%s': %s\n",
		priv->file, priv->group, msg);
	evas_object_hide(priv->webview);
	return 0;
     }

   edje_object_part_swallow(priv->edje, EDJE_PART_CONTENT, priv->webview);
   return 1;
}

static void
_eve_scrolled_webview_smart_add(Evas_Object *o)
{
   Eve_Scrolled_Webview_Data *priv = calloc(1, sizeof(*priv));
   if (!priv)
     {
	fprintf(stderr, "ERROR: could not allocate priv data %d bytes: %s\n",
		sizeof(*priv), strerror(errno));
	return;
     }
   evas_object_smart_data_set(o, priv);

   priv->file = eina_stringshare_add(PACKAGE_DATA_DIR "/data/b_and_w.edj");
   priv->group = eina_stringshare_add("eve.scrollframe");

   _parent_sc.add(o);

   priv->edje = edje_object_add(priv->base.evas);
   evas_object_show(priv->edje);
   evas_object_smart_member_add(priv->edje, o);

   priv->webview = ewk_webview_object_add(priv->base.evas);
   if (!priv->webview)
     {
	fputs("ERROR: could not create webview.\n", stderr);
	return;
     }

   priv->page = ewk_webview_object_webpage_get(priv->webview);
   priv->mainframe = ewk_webpage_object_mainframe_get(priv->page);

   _eve_scrolled_webview_edje_reset(priv);

   const struct event_map
   {
      int event;
      void (*cb)(void *, Evas *, Evas_Object *, void *);
   } *itr, map[] = {
     {EVAS_CALLBACK_KEY_DOWN, _eve_scrolled_webview_on_key_down},
     {EVAS_CALLBACK_MOUSE_DOWN, _eve_scrolled_webview_on_mouse_down},
     {-1, NULL}
   };
   for (itr = map; itr->cb != NULL; itr++)
     evas_object_event_callback_add(priv->webview, itr->event, itr->cb, priv);
}


static Evas_Smart *
_eve_scrolled_webview_smart_new(void)
{
   static Evas_Smart_Class sc = {
     "Eve_Scrolled_Webview", EVAS_SMART_CLASS_VERSION, NULL
   };

   if (!_parent_sc.name)
     {
	evas_object_smart_clipped_smart_set(&sc);
	_parent_sc = sc;
	sc.add = _eve_scrolled_webview_smart_add;
	sc.del = _eve_scrolled_webview_smart_del;
	sc.resize = _eve_scrolled_webview_smart_resize;
     }

   return evas_smart_class_new(&sc);
}

Evas_Object *
eve_scrolled_webview_add(Evas *e)
{
   static Evas_Smart *smart = NULL;
   if (!smart)
     smart = _eve_scrolled_webview_smart_new();
   return evas_object_smart_add(e, smart);
}

bool
eve_scrolled_webview_theme_file_set(Evas_Object *o, const char *file)
{
   EVE_SCROLLED_WEBVIEW_DATA_GET_OR_RETURN_VAL(o, priv, 0);
   eina_stringshare_replace(&priv->file, file);
   return _eve_scrolled_webview_edje_reset(priv);
}

const char *
eve_scrolled_webview_theme_file_get(const Evas_Object *o)
{
   EVE_SCROLLED_WEBVIEW_DATA_GET_OR_RETURN_VAL(o, priv, NULL);
   return priv->file;
}

bool
eve_scrolled_webview_theme_group_set(Evas_Object *o, const char *group)
{
   EVE_SCROLLED_WEBVIEW_DATA_GET_OR_RETURN_VAL(o, priv, 0);
   eina_stringshare_replace(&priv->group, group);
   return _eve_scrolled_webview_edje_reset(priv);
}

const char *
eve_scrolled_webview_theme_group_get(const Evas_Object *o)
{
   EVE_SCROLLED_WEBVIEW_DATA_GET_OR_RETURN_VAL(o, priv, NULL);
   return priv->group;
}

Evas_Object *
eve_scrolled_webview_get(const Evas_Object *o)
{
   EVE_SCROLLED_WEBVIEW_DATA_GET_OR_RETURN_VAL(o, priv, NULL);
   return priv->webview;
}

Evas_Object *
eve_scrolled_webview_page_get(const Evas_Object *o)
{
   EVE_SCROLLED_WEBVIEW_DATA_GET_OR_RETURN_VAL(o, priv, NULL);
   return priv->page;
}
