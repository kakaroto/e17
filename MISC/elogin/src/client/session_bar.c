#include "session_bar.h"
#include <Estyle.h>

/**
 * This might be a little bit of an overkill, but for now this holds all of
 * our attributes for the session bar.  You should'nt need a static global
 * for this file.
 */
struct _Session_Bar
{
   struct
   {
      int x, y;
      int off_x, off_y;
   }
   pos;

   struct
   {
      int w, h;
   }
   win_geom;

   struct
   {
      double start, current;
   }
   tm;
   Estyle *header;
   Evas_List *sessions;
   Evas_Object *listbg, *bullet;
};
typedef struct _Session_Bar Session_Bar;

/* forward decl's */
static Session_Bar *session_bar_new(void);
static void session_bar_free(Session_Bar * sb);

static void session_bar_toggle_visibility(void *_data, Estyle * _es,
                                          void *event_info);
static void session_bar_session_clicked_cb(void *_data, Estyle * _es,
                                           void *event_info);
static void session_bar_session_mouse_in_cb(void *_data, Estyle * _es,
                                            void *event_info);
static void session_bar_session_mouse_out_cb(void *_data, Estyle * _es,
                                             void *event_info);
static double
get_time(void)
{
   struct timeval timev;

   gettimeofday(&timev, NULL);
   return (double) timev.tv_sec + (((double) timev.tv_usec) / 1000000);
}

void
e_login_session_bar_init(E_Login_Session e)
{
   int w, h;
   Evas *evas;
   Estyle *es = NULL;
   double bgx, bgy, bgw, bgh;
   int ix = 0, iy = 0;
   int iw = 0, ih = 0;
   Evas_List *l = NULL;
   Evas_Object *o = NULL;

   Session_Bar *sb = session_bar_new();

   /* Session list background image */
   evas = e->evas;

   o = evas_object_image_add(evas);
   evas_object_image_file_set(o, PACKAGE_DATA_DIR "/data/images/parch.png",
                              NULL);
   iw = ih = 0;
   evas_object_image_fill_set(o, 0.0, 0.0, (double) iw, (double) ih);
   evas_object_resize(o, iw, ih);
   evas_object_layer_set(o, 15);
   evas_object_color_set(o, e->config->greeting.font.r,
                         e->config->greeting.font.g,
                         e->config->greeting.font.b, 100);
   ix = ((((e->geom.w / e->config->screens.w) - w) * 0.5));
   iy = 120;
   evas_object_move(o, ix, iy);
   sb->listbg = o;

   /* Session list heading */
   es = estyle_new(evas, "XSessions", "raised");
   estyle_set_color(es, e->config->greeting.font.r,
                    e->config->greeting.font.g, e->config->greeting.font.b,
                    e->config->greeting.font.a);
   estyle_set_font(es, e->config->greeting.font.name,
                   e->config->greeting.font.size);
   estyle_callback_add(es, EVAS_CALLBACK_MOUSE_UP,
                       session_bar_toggle_visibility, sb);
   estyle_set_layer(es, 25);
   estyle_geometry(es, NULL, NULL, &w, &h);
   ix = ((((e->geom.w / e->config->screens.w) - w) * 0.9));
   iy = ((((e->geom.h / e->config->screens.h) - h) * 1.0));
   estyle_move(es, ix, iy);
   estyle_show(es);
   sb->header = es;

   /* Build session list */
   iy = 125;
   evas_object_geometry_get(sb->listbg, &bgx, &bgy, &bgw, &bgh);
   for (l = e->config->sessions; l && iy <= 330; l = l->next)
   {
      char *session_name = ((E_Login_Session_Type *) evas_list_data(l))->name;

      es = estyle_new(evas, session_name, "raised");
      estyle_set_color(es, e->config->greeting.font.r,
                       e->config->greeting.font.g, e->config->greeting.font.b,
                       e->config->greeting.font.a);
      estyle_set_font(es, e->config->greeting.font.name,
                      e->config->greeting.font.size);
      estyle_set_layer(es, 25);
      estyle_callback_add(es, EVAS_CALLBACK_MOUSE_UP,
                          session_bar_session_clicked_cb, e);
      estyle_callback_add(es, EVAS_CALLBACK_MOUSE_IN,
                          session_bar_session_mouse_in_cb, e);
      estyle_callback_add(es, EVAS_CALLBACK_MOUSE_OUT,
                          session_bar_session_mouse_out_cb, e);
      sb->sessions = evas_list_append(sb->sessions, es);
      estyle_geometry(es, NULL, NULL, &w, &h);
      ix = ((((e->geom.w / e->config->screens.w) - w) * 0.5));
      estyle_move(es, ix, iy);
      iy += h + 5;
      bgh += h + 5;
      if (w > bgw)
      {
         bgw = w + 12;
	 ix = ((((e->geom.w / e->config->screens.w) - bgw) * 0.5));
         evas_object_move(sb->listbg, ix, 120);
      }
      evas_object_image_fill_set(sb->listbg, 0.0, 0.0, (double) bgw,
                                 (double) bgh);
      evas_object_resize(sb->listbg, bgw, bgh);
   }

   /* Bullet */
   o = evas_object_image_add(evas);
   evas_object_image_file_set(o, PACKAGE_DATA_DIR "/data/images/bullet.png",
                              NULL);
   evas_object_image_size_get(o, &iw, &ih);
   evas_object_resize(o, iw, ih);
   evas_object_image_fill_set(o, 0.0, 0.0, (double) iw, (double) ih);
   evas_object_image_alpha_set(o, 1);
   evas_object_color_set(o, e->config->greeting.font.r,
                         e->config->greeting.font.g,
                         e->config->greeting.font.b,
                         e->config->greeting.font.a);
   evas_object_layer_set(o, 15);
   evas_object_move(o, -99999, -99999);
   sb->bullet = o;

   /* Set default session to first in list (for now) */ ;
   e->xsession = evas_list_data(e->config->sessions);

   sb->win_geom.w = e->geom.w;
   sb->win_geom.h = e->geom.h;
}

/**
 * Allocate a new session bar
 * Returns - A new 0'ed out session bar
 */
static Session_Bar *
session_bar_new(void)
{
   Session_Bar *sb = (Session_Bar *) malloc(sizeof(struct _Session_Bar));
   memset(sb, 0, sizeof(struct _Session_Bar));
   return (sb);
}

/**
 * Cleans up a session bar
 * @sb - the session bar we should clean up
 */
static void
session_bar_free(Session_Bar * sb)
{
   free(sb);
}

/**
 * mouse down cb for selecting a session from the session list 
 * @_data - a pointer to the E_Login_Session
 * @_es - the estyle that was clicked
 * @event_info - unused
 */
static void
session_bar_session_clicked_cb(void *_data, Estyle * _es, void *event_info)
{
   E_Login_Session e = NULL;

   if (_data)
   {
      char *text = NULL;

      e = (E_Login_Session) _data;
      if ((text = estyle_get_text(_es)))
      {
         e_login_session_select_xsession_named(e, text);
         free(text);
      }
   }
}

/**
 * timered scrollbar fade out callback
 * @val - the Alpha value for the stuff in the session bar
 * @_data - the pointer to the session bar struct
 */
static void
session_bar_fade_out_cb(int val, void *_data)
{
   Session_Bar *sb = (Session_Bar *) _data;
   Evas_List *l = NULL;

   if (val > 0)
   {
      int r, g, b, a;
      Estyle *es;

      for (l = sb->sessions; l; l = l->next)
      {
         es = (Estyle *) l->data;
         estyle_get_color(es, &r, &g, &b, &a);
         estyle_set_color(es, r, g, b, val);
      }
      evas_object_color_get(sb->listbg, &r, &g, &b, &a);
      if (a >= val)
         evas_object_color_set(sb->listbg, r, g, b, val);
      evas_object_color_get(sb->bullet, &r, &g, &b, &a);
      evas_object_color_set(sb->bullet, r, g, b, val);
      val -= 5;
      ecore_add_event_timer("session_bar_fade", 0.00005,
                            session_bar_fade_out_cb, val, _data);
   }
   else
   {
      evas_object_hide(sb->listbg);
      evas_object_hide(sb->bullet);
      for (l = sb->sessions; l; l = l->next)
         estyle_hide((Estyle *) l->data);
   }
}

/**
 * timered scroolbar fade in callback
 * @val - the Alpha value for the stuff in the session bar
 * @_data - the pointer to the session bar struct
 */
static void
session_bar_fade_in_cb(int val, void *_data)
{
   Session_Bar *sb = (Session_Bar *) _data;
   Evas_List *l = NULL;

   if (val < 255)
   {
      int r, g, b, a;
      Estyle *es;

      for (l = sb->sessions; l; l = l->next)
      {
         es = (Estyle *) l->data;
         estyle_get_color(es, &r, &g, &b, &a);
         estyle_set_color(es, r, g, b, val);
         if (!val)
            estyle_show(es);
      }
      if (val < 100)
      {
         evas_object_color_get(sb->listbg, &r, &g, &b, &a);
         evas_object_color_set(sb->listbg, r, g, b, val);
      }
      evas_object_color_get(sb->bullet, &r, &g, &b, &a);
      evas_object_color_set(sb->bullet, r, g, b, val);
      if (!val)
      {
         evas_object_show(sb->listbg);
         evas_object_show(sb->bullet);
      }
      val += 5;
      ecore_add_event_timer("session_bar_fade", 0.00005,
                            session_bar_fade_in_cb, val, _data);
   }
}

/**
 * Have the session bar fade off of the screen.
 * @sb - the session bar we want to appear
 */
static void
session_bar_vanish(Session_Bar * sb)
{
   ecore_add_event_timer("session_bar_fade_out", 0.0005,
                         session_bar_fade_out_cb, 255, sb);
}

/**
 * Have the session bar fade in on the screen.
 * @sb - the session bar we want to appear
 */
static void
session_bar_appear(Session_Bar * sb)
{
   ecore_add_event_timer("session_bar_fade_in", 0.0005,
                         session_bar_fade_in_cb, 0, sb);
}

/**
 * If the session bar is visible, hide it, if it's hidden, show it
 * @_data - pointer to the E_Login_Session 
 * @_es - the "XSessions" Text on screen
 * @event_info - unusded
 */
static void
session_bar_toggle_visibility(void *_data, Estyle * _es, void *event_info)
{
   static int grow_or_shrink = 0;

   if (_data)
   {
      E_Login_Session e = NULL;

      e = (E_Login_Session) _data;

      if (grow_or_shrink)
      {
         session_bar_vanish(_data);
         grow_or_shrink = 0;
      }
      else
      {
         session_bar_appear(_data);
         grow_or_shrink = 1;
      }
   }
}

/**
 * Callback for mouse in events on Session Items
 * @_data - unused
 * @_es - the estyle that received the mouse out event
 * @event_info - unused
 */
static void
session_bar_session_mouse_in_cb(void *_data, Estyle * _es, void *event_info)
{
   if (_es)
   {
      char *text = NULL;

      if ((text = estyle_get_text(_es)))
      {
         fprintf(stderr, "MouseIn on:%s\n", text);
         free(text);
      }
   }
}

/**
 * Callback for mouse in events out Session Items
 * @_data - unused
 * @_es - the estyle that received the mouse out event
 * @event_info - unused
 */
static void
session_bar_session_mouse_out_cb(void *_data, Estyle * _es, void *event_info)
{
   if (_es)
   {
      char *text = NULL;

      if ((text = estyle_get_text(_es)))
      {
         fprintf(stderr, "MouseOut on:%s\n", text);
         free(text);
      }
   }
}
