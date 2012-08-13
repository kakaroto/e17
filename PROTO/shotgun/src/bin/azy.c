#include "ui.h"
#ifdef HAVE_AZY

static Azy_Client *cli = NULL;
static Eina_Bool notified = EINA_FALSE;
static Ecore_Event_Handler *ac = NULL;
static Ecore_Event_Handler *ar = NULL;
static Ecore_Event_Handler *ad = NULL;
static Ecore_Event_Handler *ads = NULL;

static Eina_Bool
ui_azy_return(Contact_List *cl, int type __UNUSED__, Azy_Content *content)
{
   Azy_Rss *rss;
   const Eina_List *items, *l;
   Azy_Rss_Item *item;
   const char *id;
   unsigned int x = 1;
   char buf[256];

   if (azy_content_error_is_set(content))
     {
        printf("Error encountered: %s\n", azy_content_error_message_get(content));
        return ECORE_CALLBACK_RENEW;
     }

   rss = azy_content_return_get(content);
   //azy_rss_print("> ", 0, rss);
   items = azy_rss_items_get(rss);
   if (!items) return ECORE_CALLBACK_RENEW;
   item = items->data;
   id = azy_rss_item_id_get(item);
   /* current version is up-to-date! */
   if (!strcmp(id + sizeof("tag:github.com,2008:Grit::Commit/") - 1, VREV))
     return ECORE_CALLBACK_RENEW;
   EINA_LIST_FOREACH(items->next, l, item)
     {
        id = azy_rss_item_id_get(item);
        if (!strcmp(id + sizeof("tag:github.com,2008:Grit::Commit/") - 1, VREV))
          break;
        x++;
     }
   if (x == eina_list_count(items))
     snprintf(buf, sizeof(buf), "Your version of Shotgun! is at least %u commits old!", x);
   else
     snprintf(buf, sizeof(buf), "Your version of Shotgun! is %u commit%s old!", x, (x > 1) ? "s" : "");

#ifdef HAVE_NOTIFY
   ui_dbus_notify(cl, NULL, "Shotgun!", buf);
#endif
   notified = EINA_TRUE;
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
ui_azy_download_status(void *data __UNUSED__, int type __UNUSED__, Azy_Event_Download_Status *ev)
{
   int total = -1;

   if (ev->net)
     total = azy_net_message_length_get(ev->net);
   if (total > 0)
     INF("RSS: %zu bytes (of %i total) transferred for RSS feed", ev->size, total);
   else
     INF("RSS: %zu bytes transferred for RSS feed", ev->size);
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
ui_azy_disconnected(void *data __UNUSED__, int type __UNUSED__, Azy_Client *ev)
{
   if (ev != cli) return ECORE_CALLBACK_RENEW;
   azy_client_redirect(ev);
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
ui_azy_connected(void *data __UNUSED__, int type __UNUSED__, Azy_Client *ev)
{
   Azy_Client_Call_Id id;

   if (ev != cli) return ECORE_CALLBACK_RENEW;

   if (!azy_client_current(ev))
     {
        id = azy_client_blank(ev, AZY_NET_TYPE_GET, NULL, NULL, NULL);
        azy_client_callback_free_set(ev, id, (Ecore_Cb)azy_rss_free);
     }

   return ECORE_CALLBACK_RENEW;
}

void
ui_azy_init(Contact_List *cl)
{
   azy_init();

   cli = azy_client_new();

   azy_client_host_set(cli, "https://github.com", 443);
   azy_client_data_set(cli, cl);

   ac = ecore_event_handler_add(AZY_CLIENT_CONNECTED, (Ecore_Event_Handler_Cb)ui_azy_connected, NULL);
   ar = ecore_event_handler_add(AZY_CLIENT_RETURN, (Ecore_Event_Handler_Cb)ui_azy_return, cl);
   ad = ecore_event_handler_add(AZY_CLIENT_DISCONNECTED, (Ecore_Event_Handler_Cb)ui_azy_disconnected, NULL);
   ads = ecore_event_handler_add(AZY_EVENT_DOWNLOAD_STATUS, (Ecore_Event_Handler_Cb)ui_azy_download_status, NULL);
}

Eina_Bool
ui_azy_connect(Contact_List *cl __UNUSED__)
{
   Azy_Net *net;

   if (notified) return EINA_FALSE;
   azy_client_connect(cli, EINA_TRUE);
   net = azy_client_net_get(cli);
   if (!azy_net_uri_get(net))
     azy_net_uri_set(net, "/zmike/shotgun/commits/master.atom");
   azy_net_version_set(net, 0);
   return EINA_TRUE;
}

void
ui_azy_shutdown(Contact_List *cl __UNUSED__)
{
   azy_client_free(cli);
   cli = NULL;
   azy_shutdown();
   ecore_event_handler_del(ac);
   ecore_event_handler_del(ar);
   ecore_event_handler_del(ad);
   ecore_event_handler_del(ads);
}

#endif
