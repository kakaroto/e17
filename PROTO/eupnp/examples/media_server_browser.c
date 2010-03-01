/* Eupnp - UPnP library
 *
 * Copyright (C) 2009 Andre Dieb Martins <andre.dieb@gmail.com>
 *
 * This file is part of Eupnp.
 *
 * Eupnp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Eupnp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Eupnp.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * Description: application that searchs for media server devices. When a media
 * server device is located, the application then performs an action called
 * Browse on the server Content Directory service, which is virtually the same
 * as browsing a folder on any file manager system.
 */

#include <stdio.h>
#include <string.h>

#include <Eina.h>
#include <Elementary.h>
#include <Emotion.h>

#include "Eupnp.h"
#include "eupnp_av/Eupnp_AV.h"

#define MEDIA_SERVER_DEVICE_TYPE "urn:schemas-upnp-org:device:MediaServer:1"
#define CDS_SERVICE_TYPE "urn:schemas-upnp-org:service:ContentDirectory:1"
#define IS_MEDIA_SERVER(device) !strcmp(device->device_type, MEDIA_SERVER_DEVICE_TYPE)

#ifdef INF
  #undef INF
#endif
#define INF(...) EINA_LOG_DOM_INFO(_log_domain, __VA_ARGS__)

#ifdef ERR
  #undef ERR
#endif
#define ERR(...) EINA_LOG_DOM_ERR(_log_domain, __VA_ARGS__)

#ifdef DBG
  #undef DBG
#endif
#define DBG(...) EINA_LOG_DOM_DBG(_log_domain, __VA_ARGS__)


typedef struct _Media_Server Media_Server;

struct _Media_Server {
   Eupnp_Service_Proxy *cds;
   Eupnp_Device_Info *server;
   Elm_List_Item *item;
};


static Media_Server *ms = NULL;
static DIDL_Item *selected_item = NULL;
static char *browsing = NULL;
static Eina_List *servers = NULL;
static Elm_Genlist_Item_Class cls;
static Elm_Genlist_Item_Class cls_didl;
static Elm_Genlist_Item_Class cls_didl_item;

static Evas_Object *win, *bg, *root, *pager, *main_label, *box, *hbox, *bt, *emotion, *media_box, *controls;
static Evas_Object *media_label;

static _log_domain = -1;

static void         browse(Eupnp_Service_Proxy *proxy, const char *container_id, void *data);
static Evas_Object *folder_icon_get(Evas_Object *obj);
static Evas_Object *file_icon_get(Evas_Object *obj);


static void
item_play(void *data, Evas_Object *obj, void *event_info)
{
   Eina_List *l, *resources;
   DIDL_Resource *res;

   if (!selected_item)
     return;

   resources = (Eina_List *)eupnp_av_didl_item_resources_get(selected_item);

   EINA_LIST_FOREACH(resources, l, res)
     {
	const char *v = eupnp_av_didl_resource_value_get(res);
	if (v)
	  {
	     emotion_object_play_set(emotion, EINA_FALSE);
	     DBG("Playing %s", v);
	     emotion_object_file_set(emotion, v);
	     emotion_object_play_set(emotion, EINA_TRUE);
	     elm_label_label_set(media_label, eupnp_av_didl_object_title_get(DIDL_OBJECT_GET(data)));
	     break;
	  }
     }
}

static void
item_stop(void *data, Evas_Object *obj, void *event_info)
{
   emotion_object_play_set(emotion, EINA_FALSE);
}

static void
item_select(void *data, Evas_Object *obj, void *event_info)
{
   selected_item = data;

   DBG("Selecting item %s:%s",
       eupnp_av_didl_object_title_get(DIDL_OBJECT_GET(data)),
       eupnp_av_didl_object_id_get(DIDL_OBJECT_GET(data)));
}

static void
container_browse(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *list;
   const DIDL_Container *c;

   c = data;

   DBG("Browsing %s", eupnp_av_didl_object_id_get(DIDL_OBJECT_GET(c)));
   asprintf(&browsing, "%s/%s", browsing, eupnp_av_didl_object_title_get(DIDL_OBJECT_GET(c)));

   elm_label_label_set(main_label, browsing);

   list = elm_list_add(pager);
   evas_object_show(list);
   elm_pager_content_push(pager, list);

   browse(ms->cds, eupnp_av_didl_object_id_get(DIDL_OBJECT_GET(c)), ms);
}

void
on_container_found(void *data, DIDL_Container *c)
{
   Evas_Object *box;
   Elm_List_Item *item;

   DBG("Found container %s", eupnp_av_didl_object_title_get(DIDL_OBJECT_GET(c)));

   item = elm_list_item_append(elm_pager_content_top_get(pager),
			       eupnp_av_didl_object_title_get(DIDL_OBJECT_GET(c)),
			       folder_icon_get(pager), NULL,
			       &container_browse, c);

   elm_list_item_show(item);
   elm_list_go(elm_pager_content_top_get(pager));
}

void
on_item_found(void *data, DIDL_Item *i)
{
   Elm_List_Item *item;

   DBG("Found item %s", eupnp_av_didl_object_title_get(DIDL_OBJECT_GET(i)));

   item = elm_list_item_append(elm_pager_content_top_get(pager),
			       eupnp_av_didl_object_title_get(DIDL_OBJECT_GET(i)),
			       file_icon_get(pager), NULL,
			       &item_select, i);

   elm_list_item_show(item);
   elm_list_go(elm_pager_content_top_get(pager));
}

static void
action_response(void *data, Eina_Inlist *evented_vars)
{
   Eupnp_Service_Action_Argument *arg;

   EINA_INLIST_FOREACH(evented_vars, arg)
    {
       INF("%s: %s", arg->name, arg->value);

     if (!strcmp(arg->name, "Result"))
       if (eupnp_av_didl_parse(arg->value, strlen(arg->value),
			       on_item_found, on_container_found, data))
	  INF("Parsed DIDL fragment successfully.");
       else
	  ERR("Failed to parse DIDL fragment.");
    }
}

static void
browse(Eupnp_Service_Proxy *proxy, const char *container_id, void *data)
{
   if (!eupnp_service_proxy_action_send(proxy, "Browse", EUPNP_ACTION_RESPONSE_CB(action_response),
					data, // data
					"ObjectID", EUPNP_TYPE_STRING, container_id,
					"BrowseFlag", EUPNP_TYPE_STRING, "BrowseDirectChildren",
					"Filter", EUPNP_TYPE_STRING, "",
					"StartingIndex", EUPNP_TYPE_INT, 0,
					"RequestedCount", EUPNP_TYPE_INT, 25,
					"SortCriteria", EUPNP_TYPE_STRING, "",
					NULL))
      ERR("Failed to send proxy action.");

   DBG("Finished browse()");
}

static Eina_Bool
is_server_known(Eupnp_Device_Info *device)
{
   Eina_List *l;
   Media_Server *m;

   EINA_LIST_FOREACH(servers, l, m)
     if (!strcmp(device->udn, m->server->udn))
	return EINA_TRUE;

   return EINA_FALSE;
}

static void
server_browse(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *list;

   ms = data;
   DBG("Browsing server %s", ms->server->udn);
   browsing = strdup(ms->server->friendly_name);
   elm_label_label_set(main_label, browsing);

   list = elm_list_add(pager);
   evas_object_show(list);
   elm_pager_content_push(pager, list);
   evas_object_show(bt);

   browse(ms->cds, "0", ms);
}

static void
on_cds_ready(void *data, Eupnp_Service_Proxy *proxy)
{
   Media_Server *ms = data;

   DBG("Attaching proxy %p to %s", proxy, ms->server->udn);
   ms->cds = eupnp_service_proxy_ref(proxy);
}

static Evas_Object *
folder_icon_get(Evas_Object *obj)
{
   Evas_Object *ic = elm_icon_add(obj);
   elm_icon_standard_set(ic, "folder");
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   evas_object_show(ic);
   return ic;
}

static Evas_Object *
file_icon_get(Evas_Object *obj)
{
   Evas_Object *ic = elm_icon_add(obj);
   elm_icon_standard_set(ic, "file");
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   evas_object_show(ic);
   return ic;
}

static Eina_Bool
on_device_ready(void *user_data, Eupnp_Event_Type event_type, void *event_data)
{
   Media_Server *ms;
   Elm_List_Item *item;
   Eupnp_Device_Info *device;
   const Eupnp_Service_Info *cds;

   device = event_data;

   if (!IS_MEDIA_SERVER(device))
     return EINA_TRUE;

   cds = eupnp_device_info_service_get_by_type(device, CDS_SERVICE_TYPE);

   if ((!cds) || is_server_known(device))
     return EINA_TRUE;

   ms = calloc(1, sizeof(Media_Server));
   ms->server = eupnp_device_info_ref(device);
   item = elm_list_item_append(root, ms->server->friendly_name,
			       folder_icon_get(root), NULL,
			       &server_browse, ms);
   elm_list_item_show(item);
   elm_list_go(root);

   eupnp_service_proxy_new(cds, on_cds_ready, ms);
   servers = eina_list_append(servers, ms);
   evas_object_show(root);
   evas_object_show(pager);

   return EINA_TRUE;
}

static void
media_server_free(Media_Server *ms)
{
   if (!ms) return;

   elm_list_item_del(ms->item);
   eupnp_device_info_unref(ms->server);

   if (ms->cds)
      eupnp_service_proxy_unref(ms->cds);

   free(ms);
}

static Eina_Bool
on_device_gone(void *user_data, Eupnp_Event_Type event_type, void *event_data)
{
   Eina_List *l;
   Media_Server *ms;
   Eupnp_Device_Info *device;

   device = event_data;

   EINA_LIST_FOREACH(servers, l, ms)
     {
	if (!strcmp(ms->server->udn, device->udn)) {
	   servers = eina_list_remove(servers, ms);
	   media_server_free(ms);
	   break;
	}
     }

   return EINA_TRUE;
}

static void
on_back_clicked(void *data, Evas_Object *obj, void *event_info)
{
   if (elm_pager_content_top_get(pager) != elm_pager_content_bottom_get(pager))
     {
	elm_pager_content_pop(pager);

	if (browsing)
	 {
	    char *p;
	    p = strrchr(browsing, '/');

	    if (p)
	       *p = '\0';

	    elm_label_label_set(main_label, browsing);
	 }
     }
   else
     {
	elm_label_label_set(main_label, "UPnP Network");
	free(browsing);
	browsing = NULL;
	evas_object_hide(bt);
     }

}

static void
av_browser_del(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}


void
av_browser_win_create(void)
{
   Evas_Object *ic, *vbox;
   Evas *evas;

   win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(win, "Enlightenment UPnP AV Browser");
   evas_object_smart_callback_add(win, "delete-request", av_browser_del, NULL);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   hbox = elm_box_add(win);
   elm_box_horizontal_set(hbox, EINA_TRUE);
   evas_object_size_hint_align_set(hbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(hbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, hbox);
   evas_object_show(hbox);

   box = elm_box_add(win);
   evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(box);
   elm_box_pack_start(hbox, box);

   media_box = elm_box_add(win);
   evas_object_size_hint_align_set(media_box, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(media_box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(media_box);
   elm_box_pack_end(hbox, media_box);

   media_label = elm_label_add(win);
   elm_label_label_set(media_label, "No media selected.");
   evas_object_show(media_label);
   elm_box_pack_start(media_box, media_label);

   evas = evas_object_evas_get(win);
   emotion = emotion_object_add(evas);
   //emotion_object_smooth_scale_set(emotion, EINA_TRUE);
   evas_object_size_hint_weight_set(emotion, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(emotion, EVAS_HINT_FILL, EVAS_HINT_FILL);

   if (!emotion_object_init(emotion, "gstreamer"))
     {
	ERR("Failed to load gstreamer module for playing.");
     }

   int i;

   for (i = EMOTION_VIS_GOOM; i < EMOTION_VIS_LAST; i++)
     {
	if (!emotion_object_vis_supported(emotion, i))
	  continue;

	emotion_object_vis_set(emotion, i);
	DBG("Setting visualization %d", i);
	break;
     }

   elm_box_pack_end(media_box, emotion);
   evas_object_show(emotion);

   controls = elm_box_add(win);
   elm_box_horizontal_set(controls, EINA_TRUE);
   evas_object_size_hint_weight_set(controls, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(controls);
   elm_box_pack_end(media_box, controls);

   Evas_Object *b;

   b = elm_button_add(win);
   elm_button_label_set(b, "Play");
   evas_object_smart_callback_add(b, "clicked", item_play, NULL);
   elm_box_pack_start(controls, b);
   evas_object_show(b);

   b = elm_button_add(win);
   elm_button_label_set(b, "Stop");
   evas_object_smart_callback_add(b, "clicked", item_stop, NULL);
   elm_box_pack_end(controls, b);
   evas_object_show(b);

   ic = elm_icon_add(win);
   elm_icon_standard_set(ic, "arrow_left");
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   evas_object_show(ic);

   bt = elm_button_add(win);
   elm_button_label_set(bt, "Back");
   elm_button_icon_set(bt, ic);
   evas_object_smart_callback_add(bt, "clicked", on_back_clicked, NULL);

   main_label = elm_label_add(win);
   elm_label_label_set(main_label, "UPnP Network");
   evas_object_show(main_label);
   elm_box_pack_start(box, main_label);

   pager = elm_pager_add(win);
   evas_object_size_hint_align_set(pager, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(pager, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(pager);
   elm_box_pack_end(box, pager);
   elm_box_pack_end(box, bt);

   root = elm_list_add(win);
   evas_object_size_hint_weight_set(root, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(root);
   elm_pager_content_push(pager, root);

   evas_object_resize(win, 600, 480);
   evas_object_show(win);
}

/*
 * Run "EINA_LOG_LEVELS=media_server_browser:5 ./media_server_browser"
 * for watching only application log messages.
 */
EAPI int
elm_main(int argc, char **argv)
{
   Eupnp_Control_Point *c;
   int ret;

   ret = -1;

   if (!eupnp_av_init())
     {
	fprintf(stderr, "Failed to initialize eupnp_av\n");
	return ret;
     }

   if (!eupnp_init())
     {
	fprintf(stderr, "Failed to initialize eina log module.\n");
	goto eupnp_error;
     }

   if ((_log_domain = eina_log_domain_register("media_server_browser", EINA_COLOR_BLUE)) < 0)
     {
	fprintf(stderr, "Failed to create a logging domain for the application.\n");
	goto log_domain_reg_error;
     }

   if (!eupnp_ecore_init())
     {
	fprintf(stderr, "Could not initialize eupnp-ecore\n");
	goto eupnp_ecore_init_error;
     }

   av_browser_win_create();

   c = eupnp_control_point_new();
   if (!c)
     {
	fprintf(stderr, "Could not create control point instance\n");
	goto eupnp_cp_alloc_error;
     }
   eupnp_event_bus_subscribe(EUPNP_EVENT_DEVICE_READY,
			     EUPNP_CALLBACK(on_device_ready), pager);
   eupnp_event_bus_subscribe(EUPNP_EVENT_DEVICE_GONE,
			     EUPNP_CALLBACK(on_device_gone), NULL);
   eupnp_control_point_start(c);

   if (!eupnp_control_point_discovery_request_send(c, 5, MEDIA_SERVER_DEVICE_TYPE))
     ERR("Failed to perform MSearch.");
   else
     INF("MSearch sent sucessfully.");

   ret = 0;

   INF("Finished starting media server browser.");
   ecore_main_loop_begin();
   INF("Closing application.");

   /* Shutdown procedure */
   eupnp_control_point_stop(c);
   eupnp_control_point_free(c);

   eupnp_cp_alloc_error:
   eupnp_cp_init_error:
      eupnp_ecore_shutdown();
   eupnp_ecore_init_error:
      eina_log_domain_unregister(_log_domain);
   log_domain_reg_error:
      eupnp_shutdown();
   eupnp_error:
      eupnp_av_shutdown();

   return ret;
}
ELM_MAIN()
