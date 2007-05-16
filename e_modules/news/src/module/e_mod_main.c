#include "News.h"

#define E_MOD_INIT_FAIL(m, msg)    \
{                                  \
news_util_message_error_show(msg); \
   e_modapi_shutdown(m);           \
   return NULL;                    \
}

/* module requirements */
EAPI E_Module_Api e_modapi = 
   {
      E_MODULE_API_VERSION,
      "News"
   };

/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void             _gc_shutdown(E_Gadcon_Client *gcc);
static void             _gc_orient(E_Gadcon_Client *gcc);
static char            *_gc_label(void);
static Evas_Object     *_gc_icon(Evas *evas);
static const E_Gadcon_Client_Class _gadcon_class =
   {
      GADCON_CLIENT_CLASS_VERSION,
      "news",
      {
         _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon
      },
      E_GADCON_CLIENT_STYLE_INSET
   };

/*
 * Module functions
 */

EAPI void *
e_modapi_init(E_Module *m)
{
   news = E_NEW(News, 1);
   news->module = m;

   DMAIN(("Initialisation ..."));

   if (!news_config_init()) E_MOD_INIT_FAIL(m, _("Config init failed"));
   if (!news_theme_init())  E_MOD_INIT_FAIL(m, _("Theme init failed"));
   if (!news_parse_init())  E_MOD_INIT_FAIL(m, _("Parser init failed"));
   if (!news_feed_init())   E_MOD_INIT_FAIL(m, _("Feeds init failed"));
   if (!news_viewer_init()) E_MOD_INIT_FAIL(m, _("Viewer init failed"));
   if (!news_popup_warn_init()) E_MOD_INIT_FAIL(m, _("Popup warn subsystem init failed"));

   e_gadcon_provider_register((E_Gadcon_Client_Class *)&_gadcon_class);

   DMAIN(("Initialisation END"));

   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   if (!news) return 0;

   DMAIN(("Shutdown"));

   e_gadcon_provider_unregister((E_Gadcon_Client_Class *)&_gadcon_class);

   if (news->config_dialog)
     news_config_dialog_hide();
   if (news->config_dialog_feeds)
     news_config_dialog_feeds_hide();
   if (news->config_dialog_langs)
     news_config_dialog_langs_hide();
   if (news->config_dialog_feed_new)
     news_config_dialog_feed_hide(NULL);
   if (news->config_dialog_category_new)
     news_config_dialog_category_hide(NULL);

   news_popup_warn_shutdown();
   news_viewer_shutdown();
   news_feed_shutdown();
   news_parse_shutdown();
   news_theme_shutdown();
   news_config_shutdown();

   free(news);

   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   int ret;

   if (!news) return 0;
   DMAIN(("Save"));
   ret = news_config_save();

   return ret;
}

EAPI int
e_modapi_about(E_Module *m)
{
   news_util_message_show(_("Module to display information feeds<br>"
                            "like Rss or Atom, on youre desktop<br><br>"
                            "ooookiwi@gmail.com<br>"
                            "http://oooo.zapto.org"));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m)
{
   int ret;

   if (!news) return 0;
   ret = news_config_dialog_show();

   return ret;
}


/*
 * Gadcon functions
 */

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   News_Item *ni;

   DMAIN(("GCC init"));

   o = edje_object_add(gc->evas);
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   ni = news_item_new(gcc, o);
   gcc->data = ni;

   news->items = evas_list_append(news->items, ni);

   return ni->gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   News_Item *ni;
   
   ni = gcc->data;

   DMAIN(("GCC shutdown"));
   news->items = evas_list_remove(news->items, ni);
   news_item_free(ni);
}

static void
_gc_orient(E_Gadcon_Client *gcc)
{
   News_Item *ni;
   int nb_feeds;
   int gc_min_w = 16;
   int gc_min_h = 16;
   int gc_aspect_w = 16;
   int gc_aspect_h = 16;
   int gc_orient_horiz = 0;

   ni = gcc->data;

   switch (ni->config->view_mode)
     {
     case NEWS_ITEM_VIEW_MODE_ONE:
        nb_feeds = 1;
        break;
     case NEWS_ITEM_VIEW_MODE_FEED:
        nb_feeds = evas_list_count(ni->config->feed_refs);
        if (!nb_feeds) nb_feeds = 1;
        break;
     case NEWS_ITEM_VIEW_MODE_FEED_UNREAD:
        nb_feeds = ni->unread_count;
        if (!nb_feeds) nb_feeds = 1;
        break;
     }

   switch (gcc->gadcon->orient)
     {
     case E_GADCON_ORIENT_FLOAT:
     case E_GADCON_ORIENT_HORIZ:
     case E_GADCON_ORIENT_TOP:
     case E_GADCON_ORIENT_BOTTOM:
     case E_GADCON_ORIENT_CORNER_TL:
     case E_GADCON_ORIENT_CORNER_TR:
     case E_GADCON_ORIENT_CORNER_BL:
     case E_GADCON_ORIENT_CORNER_BR:
        gc_aspect_w = nb_feeds*16;
        gc_orient_horiz = 1;
        break;
     case E_GADCON_ORIENT_VERT:
     case E_GADCON_ORIENT_LEFT:
     case E_GADCON_ORIENT_RIGHT:
     case E_GADCON_ORIENT_CORNER_LT:
     case E_GADCON_ORIENT_CORNER_RT:
     case E_GADCON_ORIENT_CORNER_LB:
     case E_GADCON_ORIENT_CORNER_RB:
        gc_aspect_h = nb_feeds*16;
        break;
     default:
        break;
     }
   
   e_gadcon_client_aspect_set(gcc, gc_aspect_w, gc_aspect_h);
   e_gadcon_client_min_size_set(gcc, gc_min_w, gc_min_h);
   news_item_orient_set(ni, gc_orient_horiz);
}
   
static char *
_gc_label(void)
{
   return _("News");
}

static Evas_Object *
_gc_icon(Evas *evas)
{
   Evas_Object *o;

   o = edje_object_add(evas);
   edje_object_file_set(o, news->theme, "icon");
   return o;
}
