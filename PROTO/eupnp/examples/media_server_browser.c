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
   Elm_Genlist_Item *item;
   Eupnp_Service_Proxy *cds;
   Eupnp_Device_Info *server;
};

static Eina_List *servers = NULL;
static Elm_Genlist_Item_Class cls;
static Elm_Genlist_Item_Class cls_didl;
static Elm_Genlist_Item_Class cls_didl_item;
static Evas_Object *win, *bg, *box, *gl;
static _log_domain = -1;

static void browse(Eupnp_Service_Proxy *proxy, const char *container_id, void *data);

static void
item_select(void *data, Evas_Object *obj, void *event_info)
{
   Media_Server *ms;
   const DIDL_Item *item;

   item = elm_genlist_item_data_get(event_info);
   ms = data;

   DBG("Selecting item %s:%s",
       eupnp_av_didl_object_title_get(DIDL_OBJECT_GET(item)),
       eupnp_av_didl_object_id_get(DIDL_OBJECT_GET(item)));
}


static void
container_browse(void *data, Evas_Object *obj, void *event_info)
{
   Media_Server *ms;
   const DIDL_Container *c;

   c = elm_genlist_item_data_get(event_info);
   ms = data;

   DBG("Browsing container %p", obj);
   DBG("id: %s", eupnp_av_didl_object_id_get(DIDL_OBJECT_GET(c)));

   browse(ms->cds, eupnp_av_didl_object_id_get(DIDL_OBJECT_GET(c)), ms);
}

static Elm_Genlist_Item *
parent_container_get(Media_Server *ms, Evas_Object *genlist, const char *parentID)
{
   Elm_Genlist_Item *item;
   const DIDL_Container *c;

   for (item = elm_genlist_first_item_get(genlist); item != NULL;
	item = elm_genlist_item_next_get(item))
     {
	// Root
	if (ms->item == item)
	  continue;

	c = elm_genlist_item_data_get(item);

	if (!c)
	  continue;

	if (!strcmp(c->parent.id, parentID))
	  {
	     //DBG("Parent is %s, item is %p", c->parent.id, item);
	     return item;
	  }
     }

   return NULL;
}

void
on_container_found(void *data, DIDL_Container *c)
{
   Media_Server *ms;
   Evas_Object *genlist;
   Elm_Genlist_Item *parent;
   Elm_Genlist_Item *item;

   ms = data;
   genlist = elm_genlist_item_genlist_get(ms->item);
   parent = parent_container_get(ms, genlist,
			         eupnp_av_didl_object_parent_id_get(DIDL_OBJECT_GET(c)));

   if (!parent)
     parent = ms->item;

   elm_genlist_item_append(genlist, &cls_didl, c, parent,
			   ELM_GENLIST_ITEM_SUBITEMS, &container_browse, ms);

   evas_object_show(genlist);
}

void
on_item_found(void *data, DIDL_Item *i)
{
   Media_Server *ms;
   Evas_Object *genlist;
   Elm_Genlist_Item *parent;

   ms = data;
   genlist = elm_genlist_item_genlist_get(ms->item);
   parent = parent_container_get(ms, genlist, eupnp_av_didl_object_parent_id_get(DIDL_OBJECT_GET(i)));

   if (!parent)
     parent = ms->item;

   elm_genlist_item_append(genlist, &cls_didl_item, i, parent,
			   ELM_GENLIST_ITEM_NONE, &item_select, ms);

   evas_object_show(genlist);
}

static void
action_response(void *data, Eina_Inlist *evented_vars)
{
   Eupnp_Service_Action_Argument *arg;

   EINA_INLIST_FOREACH(evented_vars, arg)
     if (!strcmp(arg->name, "Result"))
       if (eupnp_av_didl_parse(arg->value, strlen(arg->value),
			       on_item_found, on_container_found, data))
	  INF("Parsed DIDL fragment successfully.");
       else
	  ERR("Failed to parse DIDL fragment.");
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
					"SortCriteria", EUPNP_TYPE_STRING, "dc:title",
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
   Media_Server *ms = data;

   DBG("Browsing server %s", ms->server->udn);
   browse(ms->cds, "0", ms);
}

static void
on_cds_ready(void *data, Eupnp_Service_Proxy *proxy)
{
   Media_Server *ms = data;

   DBG("Attaching proxy %p to %s", proxy, ms->server->udn);
   ms->cds = eupnp_service_proxy_ref(proxy);
}

static Eina_Bool
on_device_ready(void *user_data, Eupnp_Event_Type event_type, void *event_data)
{
   Media_Server *ms;
   Evas_Object *gl;
   Eupnp_Device_Info *device;
   const Eupnp_Service_Info *cds;
   Elm_Genlist_Item *item;

   gl = user_data;
   device = event_data;

   if (!IS_MEDIA_SERVER(device))
     return EINA_TRUE;

   cds = eupnp_device_info_service_get_by_type(device, CDS_SERVICE_TYPE);

   if ((!cds) || is_server_known(device))
     return EINA_TRUE;

   ms = calloc(1, sizeof(Media_Server));
   ms->server = eupnp_device_info_ref(device);
   ms->item = elm_genlist_item_append(gl, &cls, ms, NULL,
				      ELM_GENLIST_ITEM_SUBITEMS,
				      &server_browse, ms);

   eupnp_service_proxy_new(cds, on_cds_ready, ms);
   servers = eina_list_append(servers, ms);
   evas_object_show(gl);

   return EINA_TRUE;
}

static void
av_browser_del(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

static char *
gl_label_get(const void *data, Evas_Object *obj, const char *part)
{
   const Media_Server *ms = data;
   return strdup(ms->server->friendly_name);
}

static char *
gl_didl_label_get(const void *data, Evas_Object *evas_obj, const char *part)
{
   const DIDL_Object *obj = data;
   return strdup(obj->title);
}

static Eina_Bool
gl_state_get(const void *data, Evas_Object *obj, const char *part)
{
   return EINA_FALSE;
}

static Evas_Object *
gl_icon_get(const void *data, Evas_Object *obj, const char *part)
{
   if (!strcmp(part, "elm.swallow.icon"))
     {
	Evas_Object *ic = elm_icon_add(obj);
	elm_icon_standard_set(ic, "folder");
	evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
	evas_object_show(ic);
	return ic;
     }

   return NULL;
}

static Evas_Object *
gl_didl_item_icon_get(const void *data, Evas_Object *obj, const char *part)
{
   if (!strcmp(part, "elm.swallow.icon"))
     {
	Evas_Object *ic = elm_icon_add(obj);
	elm_icon_standard_set(ic, "file");
	evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
	evas_object_show(ic);
	return ic;
     }

   return NULL;
}

static void
gl_del(const void *data, Evas_Object *obj)
{
}

static void
gl_expand_req(void *data, Evas_Object *object, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   elm_genlist_item_subitems_clear(it);
   elm_genlist_item_expanded_set(it, 1);
}

static void
gl_contract_req(void *data, Evas_Object *object, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   elm_genlist_item_expanded_set(it, 0);
}

static void
gl_contracted(void *data, Evas_Object *object, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   elm_genlist_item_subitems_clear(it);
}

static void
gl_expanded(void *data, Evas_Object *object, void *event_info)
{
   Elm_Genlist_Item *it = event_info;
   Evas_Object *gl = elm_genlist_item_genlist_get(it);

   // expand
}

void
av_browser_win_create(void)
{
   win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(win, "Enlightenment UPnP AV Browser");
   evas_object_smart_callback_add(win, "delete-request", av_browser_del, NULL);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   box = elm_box_add(win);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, box);

   gl = elm_genlist_add(win);
   evas_object_size_hint_align_set(gl, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(gl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(gl);

   cls.item_style = "default";
   cls.func.label_get = gl_label_get;
   cls.func.icon_get  = gl_icon_get;
   cls.func.state_get = gl_state_get;
   cls.func.del = gl_del;

   cls_didl.item_style = "default";
   cls_didl.func.label_get = gl_didl_label_get;
   cls_didl.func.icon_get  = gl_icon_get;
   cls_didl.func.state_get = gl_state_get;
   cls_didl.func.del = gl_del;

   cls_didl_item.item_style = "default";
   cls_didl_item.func.label_get = gl_didl_label_get;
   cls_didl_item.func.icon_get  = gl_didl_item_icon_get;
   cls_didl_item.func.state_get = gl_state_get;
   cls_didl_item.func.del = gl_del;

   evas_object_smart_callback_add(gl, "expand,request", gl_expand_req, gl);
   evas_object_smart_callback_add(gl, "contract,request", gl_contract_req, gl);
   evas_object_smart_callback_add(gl, "expanded", gl_expanded, gl);
   evas_object_smart_callback_add(gl, "contracted", gl_contracted, gl);

   elm_box_pack_end(box, gl);
   evas_object_show(box);

   evas_object_resize(win, 320, 320);
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
			     EUPNP_CALLBACK(on_device_ready), gl);
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
