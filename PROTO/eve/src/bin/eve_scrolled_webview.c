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
   struct
   {
      Evas_Coord dx, dy;
      Ecore_Animator *anim;
   } scroll_delayed;
};

static Evas_Smart_Class _parent_sc = {NULL};
static const char EDJE_PART_CONTENT[] = "eve.swallow.content";

static bool
_eina_stringshare_replace(const char **p, const char *new)
{
   new = eina_stringshare_add(new);
   eina_stringshare_del(*p);
   if (*p == new)
     return 0;
   *p = new;
   return 1;
}

static void
_eve_scrolled_webview_scroll_report(Eve_Scrolled_Webview_Data *priv)
{
   Edje_Message_Float_Set *msg;
   int cw, ch, sx, sy, sw, sh;

   msg = alloca(sizeof(*msg) + 3 * sizeof(double));
   msg->count = 4;

   ewk_webframe_object_contents_size_get(priv->mainframe, &cw, &ch);
   if ((cw <= 0) || (ch <= 0)) goto no_scrollbars;
   ewk_webframe_object_scrollbar_max_get(priv->mainframe, &sw, &sh);
   if ((sw <= 0) && (sh <= 0)) goto no_scrollbars;
   ewk_webframe_object_scrollbar_value_get(priv->mainframe, &sx, &sy);

   msg->val[0] = (sw > 0) ? (sx / (float)sw) : -1;
   msg->val[1] = (sh > 0) ? (sy / (float)sh) : -1;
   msg->val[2] = (cw - sw) / (float)cw;
   msg->val[3] = (ch - sh) / (float)ch;

   edje_object_message_send(priv->edje, EDJE_MESSAGE_FLOAT_SET, 0, msg);
   return;

 no_scrollbars:
   msg->val[0] = -1.0;
   msg->val[1] = -1.0;
   msg->val[2] = 1.0;
   msg->val[3] = 1.0;
   edje_object_message_send(priv->edje, EDJE_MESSAGE_FLOAT_SET, 0, msg);
}

static void
_eve_scrolled_webview_scroll_by(Eve_Scrolled_Webview_Data *priv, int dx, int dy)
{
   EWebKit_Hit_Test_Contents contents;
   Evas_Object *webframe = ewk_webpage_object_mainframe_get(priv->page);
   ewk_webframe_object_hit_test(webframe, &contents, priv->mouse_move.x, priv->mouse_move.y);
   ewk_webframe_object_scroll(contents.frame, dx, dy);
   _eve_scrolled_webview_scroll_report(priv);
}

static int
_eve_scrolled_webview_scroll_by_delayed_do(void *data)
{
   Eve_Scrolled_Webview_Data *priv = data;

   if ((priv->scroll_delayed.dx == 0) && (priv->scroll_delayed.dy == 0))
     {
        priv->scroll_delayed.anim = NULL;
        return 0;
     }

   _eve_scrolled_webview_scroll_by
     (priv, priv->scroll_delayed.dx, priv->scroll_delayed.dy);
   priv->scroll_delayed.dx = 0;
   priv->scroll_delayed.dy = 0;
   return 1;
}

static void
_eve_scrolled_webview_scroll_by_delayed(Eve_Scrolled_Webview_Data *priv, Evas_Coord dx, Evas_Coord dy)
{
   if ((dx == 0) && (dy == 0))
     return;

   priv->scroll_delayed.dx += dx;
   priv->scroll_delayed.dy += dy;

   if (priv->scroll_delayed.anim)
     return;
   priv->scroll_delayed.anim = ecore_animator_add
     (_eve_scrolled_webview_scroll_by_delayed_do, priv);
}

static void
_eve_scrolled_webview_on_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eve_Scrolled_Webview_Data *priv = data;
   Evas_Event_Key_Down *ev = event_info;
   char *value;

   value = ev->keyname;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     {
	fprintf(stderr, "DBG: key down event '%s' is on hold, ignored.\n",
		value);
	return;
     }

   if (!ewk_event_feed_key_press(priv->webview, ev))
      return;

   ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;

   if (strcmp(value,"Left") == 0)
     _eve_scrolled_webview_scroll_by_delayed(priv, -10, 0);
   else if (strcmp(value,"Right") == 0)
     _eve_scrolled_webview_scroll_by_delayed(priv, 10, 0);
   else if (strcmp(value,"Up") == 0)
     _eve_scrolled_webview_scroll_by_delayed(priv, 0, -10);
   else if (strcmp(value, "Down") == 0)
     _eve_scrolled_webview_scroll_by_delayed(priv, 0, 10);
   else if (strcmp(value, "Prior") == 0)
     {
        int offset;
	edje_object_part_geometry_get
	  (priv->edje, EDJE_PART_CONTENT, NULL, NULL, NULL, &offset);
        offset = (offset * 9) / 10;
        _eve_scrolled_webview_scroll_by_delayed(priv, 0, -offset);
     }
   else if (strcmp(value, "Next") == 0)
     {
        int offset;
	edje_object_part_geometry_get
	  (priv->edje, EDJE_PART_CONTENT, NULL, NULL, NULL, &offset);
        offset = (offset * 9) / 10;
        _eve_scrolled_webview_scroll_by_delayed(priv, 0, offset);
     }
   else if (strcmp(value, "Home") == 0)
     printf("Command:home\n");
   else if (strcmp(value, "End") == 0)
     printf("Command:end\n");
   else
     ev->event_flags &= ~EVAS_EVENT_FLAG_ON_HOLD; /* unset on_hold */
}

static int
_eve_scrolled_webview_pan_anim(void *data)
{
   Eve_Scrolled_Webview_Data *priv = data;
   Evas_Coord x, y, dx, dy;

   evas_pointer_output_xy_get(priv->base.evas, &x, &y);
   dx = priv->mouse_move.x - x ;
   dy = priv->mouse_move.y - y;

   if ((dx == 0) && (dy == 0))
     return 1;

   _eve_scrolled_webview_scroll_by(priv, dx, dy);

   priv->mouse_move.x = x;
   priv->mouse_move.y = y;

   return 1;
}

static void
_eve_scrolled_webview_on_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eve_Scrolled_Webview_Data *priv = data;
   Evas_Event_Mouse_Move *ev = event_info;
   ewk_event_feed_mouse_move(priv->webview, ev);
}

static void
_eve_scrolled_webview_pan_anim_start(Eve_Scrolled_Webview_Data *priv)
{
   if (priv->mouse_move.anim)
     return;
   priv->mouse_move.anim = ecore_animator_add
     (_eve_scrolled_webview_pan_anim, priv);

   evas_object_event_callback_del
     (priv->webview, EVAS_CALLBACK_MOUSE_MOVE,
      _eve_scrolled_webview_on_mouse_move);
}

static void
_eve_scrolled_webview_pan_anim_stop(Eve_Scrolled_Webview_Data *priv)
{
   if (!priv->mouse_move.anim)
     return;
   ecore_animator_del(priv->mouse_move.anim);
   priv->mouse_move.anim = NULL;

   /* make sure it's not there already */
   evas_object_event_callback_del
     (priv->webview, EVAS_CALLBACK_MOUSE_MOVE,
      _eve_scrolled_webview_on_mouse_move);

   evas_object_event_callback_add
     (priv->webview, EVAS_CALLBACK_MOUSE_MOVE,
      _eve_scrolled_webview_on_mouse_move, priv);
}

static void
_eve_scrolled_webview_on_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eve_Scrolled_Webview_Data *priv = data;
   Evas_Event_Mouse_Down *ev = event_info;
   bool accepted;

   evas_object_focus_set(priv->webview, 1);

   ewk_event_feed_mouse_down(priv->webview, ev);

   _eve_scrolled_webview_pan_anim_stop(priv);
}

static void
_eve_scrolled_webview_on_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eve_Scrolled_Webview_Data *priv = data;
   Evas_Event_Mouse_Up *ev = event_info;

   ewk_event_feed_mouse_up(obj, ev);
}

static void
_eve_scrolled_webview_on_mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Eve_Scrolled_Webview_Data *priv = data;
   Evas_Event_Mouse_Wheel *ev = event_info;
   int offset = ev->z * 20;

   if (offset == 0)
     return;

   if (ev->direction == 0)
     _eve_scrolled_webview_scroll_by_delayed(priv, 0, offset);
   else if (ev->direction == 1)
     _eve_scrolled_webview_scroll_by_delayed(priv, offset, 0);
}

static void
_eve_scrolled_webview_smart_del(Evas_Object *o)
{
   EVE_SCROLLED_WEBVIEW_DATA_GET(o, priv);

   if (priv->scroll_delayed.anim)
     ecore_animator_del(priv->scroll_delayed.anim);
   _eve_scrolled_webview_pan_anim_stop(priv);

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
     {EVAS_CALLBACK_MOUSE_MOVE, _eve_scrolled_webview_on_mouse_move},
     {EVAS_CALLBACK_MOUSE_DOWN, _eve_scrolled_webview_on_mouse_down},
     {EVAS_CALLBACK_MOUSE_UP, _eve_scrolled_webview_on_mouse_up},
     {EVAS_CALLBACK_MOUSE_WHEEL, _eve_scrolled_webview_on_mouse_wheel},
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
   _eina_stringshare_replace(&priv->file, file);
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
   _eina_stringshare_replace(&priv->group, group);
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

void
eve_scrolled_webview_scroll_report(const Evas_Object *o)
{
   EVE_SCROLLED_WEBVIEW_DATA_GET_OR_RETURN(o, priv);
   _eve_scrolled_webview_scroll_report(priv);
}
