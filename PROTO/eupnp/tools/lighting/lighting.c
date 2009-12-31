#include <stdio.h>
#include <string.h>

#include <Eina.h>
#include <Elementary.h>
#include <Eupnp.h>

#define BINARY_LIGHT_TYPE "urn:schemas-upnp-org:device:BinaryLight:1"
#define DIMMABLE_LIGHT_TYPE "urn:schemas-upnp-org:device:DimmableLight:1"
#define SWITCH_POWER_SERVICE_TYPE "urn:schemas-upnp-org:service:SwitchPower:1"
#define DIMMING_SERVICE_TYPE "urn:schemas-upnp-org:service:Dimming:1"
#define IGD_DEVICE_ST "upnp:rootdevice"

#define ERR(...) EINA_LOG_DOM_ERR(_log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_log_domain, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_log_domain, __VA_ARGS__)

static Elm_List_Item *selected_device = NULL;
extern Evas_Object *devices;
extern Evas_Object *set;
extern Evas_Object *dimm;
static _log_domain = -1;

typedef struct _Light_Control Light_Control;

struct _Light_Control {
   Eupnp_Service_Proxy *dimm_proxy;
   Eupnp_Service_Proxy *basic_proxy;
   Eupnp_Device_Info *device;
};

static Eina_Bool
is_binary_light(Eupnp_Device_Info *device)
{
   return (!strcmp(device->device_type, BINARY_LIGHT_TYPE)) ? EINA_TRUE : EINA_FALSE;
}

static Eina_Bool
is_dimmable_light(Eupnp_Device_Info *device)
{
   return (!strcmp(device->device_type, DIMMABLE_LIGHT_TYPE)) ? EINA_TRUE : EINA_FALSE;
}

static void
dimm_proxy_ready_cb(void *data, Eupnp_Service_Proxy *proxy)
{
   Light_Control *c = data;
   INF("Received Dimming proxy %p for light control %p, device %p", proxy, c, c->device);
   c->dimm_proxy = eupnp_service_proxy_ref(proxy);
}

static void
basic_proxy_ready_cb(void *data, Eupnp_Service_Proxy *proxy)
{
   Light_Control *c = data;
   INF("Received SwitchPower proxy %p for light control %p, device %p", proxy, c, c->device);
   c->basic_proxy = eupnp_service_proxy_ref(proxy);
}

static Light_Control *
light_control_new(Eupnp_Device_Info *device)
{
   Light_Control *l;

   l = malloc(sizeof(Light_Control));

   if (!l) return NULL;

   l->device = eupnp_device_info_ref(device);
   l->basic_proxy = NULL;
   l->dimm_proxy = NULL;

   Eupnp_Service_Info *switch_power = (Eupnp_Service_Info *)eupnp_device_info_service_get_by_type(device, SWITCH_POWER_SERVICE_TYPE);

   if (switch_power)
     {
	eupnp_service_proxy_new(switch_power, basic_proxy_ready_cb, l);
	DBG("Requested SwitchPower proxy for device %p", device);
     }

   if (is_dimmable_light(device))
     {
	Eupnp_Service_Info *dimming = (Eupnp_Service_Info *)eupnp_device_info_service_get_by_type(device, DIMMING_SERVICE_TYPE);

	if (dimming)
	  {
	     eupnp_service_proxy_new(dimming, dimm_proxy_ready_cb, l);
	     DBG("Requested Dimming proxy for device %p", device);
	  }
     }

   return l;
}

static void
light_control_free(Light_Control *c)
{
   if (c->dimm_proxy) eupnp_service_proxy_unref(c->dimm_proxy);
   if (c->basic_proxy) eupnp_service_proxy_unref(c->basic_proxy);
   eupnp_device_info_unref(c->device);
}

/*
 * UPnP functions
 */
static void
device_selected(void *data, Evas_Object *obj, void *event_info)
{
   selected_device = elm_list_selected_item_get(devices);
   DBG("Selected device is now %p", selected_device);
}

static Eina_Bool
is_device_added(Eupnp_Device_Info *device)
{
   const Eina_List *items = elm_list_items_get(devices);
   const Eina_List *l;
   Elm_List_Item *item = NULL;
   if (!items || !eina_list_count(items)) return EINA_FALSE;

   EINA_LIST_FOREACH(items, l, item)
     {
	Light_Control *c = elm_list_item_data_get(item);

	if (!c->device)
	  continue;

	if (!strcmp(c->device->udn, device->udn))
	   return EINA_TRUE;
     }

   return EINA_FALSE;
}

static void
pop_device(Eupnp_Device_Info *device)
{
   Elm_List_Item *selected = elm_list_selected_item_get(devices);

   const Eina_List *l;
   Elm_List_Item *item;
   const Eina_List *items = elm_list_items_get(devices);

   EINA_LIST_FOREACH(items, l, item)
     {
	Light_Control *c = elm_list_item_data_get(item);

	if (!strcmp(c->device->udn, device->udn))
          {
	    DBG("Matched device %p for list removal.", device);

	    if (item == selected)
	      {
		// Item is selected, user might be messing with controls
		DBG("Item for deletion is selected");
		selected_device = NULL;
	      }

	    elm_list_item_del(item);
	    light_control_free(c);
	    break;
	  }
     }
}

static Eina_Bool
on_device_ready(void *user_data, Eupnp_Event_Type event_type, void *event_data)
{
   Eupnp_Device_Info *device = event_data;

   if (!(!strcmp(device->device_type, BINARY_LIGHT_TYPE) ||
         !strcmp(device->device_type, DIMMABLE_LIGHT_TYPE)))
     return;

   DBG("Found device %p : %s", device, device->udn);

   if (!is_device_added(device))
     {
	const char *name = (!device->friendly_name) ? "Unnamed Device" :
						      device->friendly_name;

	Elm_List_Item *item = NULL;
	Light_Control *c = light_control_new(device);

        if (!(item = elm_list_item_append(devices,
				  name,
				  NULL, /* TODO insert device icon */
				  NULL,
				  device_selected,
				  c)))
	  ERR("Failed to append a new device on the list.");
	else
	  {
	     INF("%p Added device %p:%s:%s to the list.", item, c, name, device->udn);
	     elm_list_item_show(item);
	     elm_list_go(devices);
	  }

     }

   return EINA_TRUE;
}

static Eina_Bool
on_device_gone(void *user_data, Eupnp_Event_Type event_type, void *event_data)
{
   Eupnp_Device_Info *device = event_data;
   pop_device(device);
   return EINA_TRUE;
}

/*
 * GUI functions
 */

void lighting_control_win_create(void);

void
check_response_cb(void *data, Eina_Inlist *evented_vars)
{
   DBG("Status change response. %p", evented_vars);

   Eupnp_Service_Action_Argument *arg;

   EINA_INLIST_FOREACH(evented_vars, arg)
     DBG("%s: %s", arg->name, arg->value);
}

void
dimm_response_cb(void *data, Eina_Inlist *evented_vars)
{
   DBG("Dimm change response. %p", evented_vars);

   Eupnp_Service_Action_Argument *arg;

   EINA_INLIST_FOREACH(evented_vars, arg)
     DBG("%s: %s", arg->name, arg->value);
}

void
dimm_changed(void *data, Evas_Object *obj, void *event_info)
{
   if (!selected_device) return;

   Light_Control *c = elm_list_item_data_get(selected_device);

   if (!c->dimm_proxy)
     {
	DBG("Dimming proxy for %p not ready.", c);
	return;
     }

   if (!eupnp_service_proxy_action_send(c->dimm_proxy, "SetLoadLevelTarget", dimm_response_cb, c,
				   "newLoadlevelTarget", EUPNP_TYPE_INT, (int) elm_slider_value_get(dimm),
				   NULL))
     ERR("Failed to send action.");
   else
     INF("Finished sending action.");
}

void
check_changed(void *data, Evas_Object *obj, void *event_info)
{
   if (!selected_device) return;

   Light_Control *c = elm_list_item_data_get(selected_device);

   if (!c->basic_proxy)
     {
	DBG("Basic proxy for %p not ready.", c);
	return;
     }

   if (!eupnp_service_proxy_action_send(c->basic_proxy, "SetTarget", check_response_cb, c,
				   "NewTargetValue", EUPNP_TYPE_INT, (int) elm_check_state_get(set),
				   NULL))
     ERR("Failed to send action.");
   else
     INF("Finished sending action.");
}


EAPI int
elm_main(int argc, char **argv)
{
   Eupnp_Control_Point *c;

   if (!eina_init())
     {
	fprintf(stderr, "Failed to initialize eina library.\n");
	return 0;
     }

   if ((_log_domain = eina_log_domain_register("lighting_control",
					       EINA_COLOR_GREEN)) < 0)
     {
	fprintf(stderr, "Failed to register a logger for the application.\n");
	goto log_dom_failure;
     }

   if (!eupnp_ecore_init())
     {
	fprintf(stderr, "Could not initialize eupnp-ecore\n");
	goto eupnp_ecore_failure;
     }

   if (!eupnp_init())
     {
	fprintf(stderr, "Could not initialize application resources\n");
	goto eupnp_init_error;
     }

   c = eupnp_control_point_new();

   if (!c)
     {
	fprintf(stderr, "Could not create control point instance\n");
	goto eupnp_cp_alloc_error;
     }

   lighting_control_win_create();

   /* Subscribe for device events */
   eupnp_event_bus_subscribe(EUPNP_EVENT_DEVICE_READY,
			     EUPNP_CALLBACK(on_device_ready), NULL);
   eupnp_event_bus_subscribe(EUPNP_EVENT_DEVICE_GONE,
			     EUPNP_CALLBACK(on_device_gone), NULL);


   /* Start control point */
   eupnp_control_point_start(c);

   /* Send a test search for all devices*/
   if (!eupnp_control_point_discovery_request_send(c, 5, IGD_DEVICE_ST))
     WRN("Failed to perform MSearch.");
   else
     INF("MSearch sent sucessfully.");

   INF("Finished starting lighting control...");

   elm_run();

   /* Shutdown procedure */
   elm_shutdown();
   eupnp_control_point_stop(c);
   eupnp_control_point_free(c);
   eupnp_shutdown();
   eupnp_ecore_shutdown();
   eina_log_domain_unregister(_log_domain);
   _log_domain = -1;
   eina_shutdown();
   return 0;

   eupnp_cp_alloc_error:
      eupnp_shutdown();
   eupnp_init_error:
      eupnp_ecore_shutdown();
   eupnp_ecore_failure:
     eina_log_domain_unregister(_log_domain);
     _log_domain = -1;
   log_dom_failure:
     eina_shutdown();

   return -1;
}
ELM_MAIN()
