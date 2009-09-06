#include <stdio.h>
#include <string.h>

#include <Eina.h>
#include <Elementary.h>
#include <Eupnp.h>


#define IGD_DEVICE_TYPE "urn:schemas-upnp-org:device:InternetGatewayDevice:1"
#define IGD_WAN_DEVICE "urn:schemas-upnp-org:device:WANDevice:1"
#define IGD_WAN_CONN_DEVICE "urn:schemas-upnp-org:device:WANConnectionDevice:1"
#define IGD_DEVICE_ST IGD_DEVICE_TYPE

/*
 * List of found devices
 */
static Evas_Object *li = NULL;
static Eina_List *device_list = NULL;

typedef struct _IGD_Device IGD_Device;

struct _IGD_Device {
   Eupnp_Device_Info *device;
   Elm_List_Item *item;
};

static void
list_item_activate(void *data, Evas_Object *obj, void *event_info)
{
   fprintf(stderr, "event!\n");
}

static void
device_list_device_add(Eupnp_Device_Info *d)
{
   IGD_Device *device;

   device = malloc(sizeof(IGD_Device));

   if (!device)
     {
	ERROR("Failed to create a new device list item.\n");
	return;
     }

   device->device = eupnp_device_info_ref(d);
   device->item = elm_list_item_append(li,
				       d->friendly_name ? d->friendly_name : "Unnamed device",
				       NULL, // Icon
				       NULL,
				       list_item_activate,
				       device);
   elm_list_item_show(device->item);
   elm_list_go(li);

   INFO("Appending device %s to list!\n", d->udn);
   device_list = eina_list_append(device_list, device);
}

static void
device_list_device_del(IGD_Device *device)
{
   device_list = eina_list_remove(device_list, device);
   eupnp_device_info_unref(device->device);
   elm_list_item_del(device->item);
   free(device);
}

static Eina_Bool
is_device_added(Eupnp_Device_Info *d)
{
   IGD_Device *tmp;
   Eina_List *l;

   EINA_LIST_FOREACH(device_list, l, tmp)
     if (!strcmp(d->udn, tmp->device->udn))
	return EINA_TRUE;

   return EINA_FALSE;
}

/*
 * Called when a device is ready to be used
 */
static Eina_Bool
on_device_ready(void *user_data, Eupnp_Event_Type event_type, void *event_data)
{
   Eupnp_Device_Info *d = event_data;

   if (!strcmp(d->device_type, IGD_DEVICE_TYPE))
     {
	// Found an IGD device
	if (is_device_added(d))
	  {
	     DEBUG("Device already added. Skipped.\n");
	     return EINA_TRUE;
	  }

	device_list_device_add(d);
     }

   return EINA_TRUE;
}


/*
 * Called when a device annouces it left. Returns a false boolean so that this
 * callback gets unregistered.
 */
static Eina_Bool
on_device_gone(void *user_data, Eupnp_Event_Type event_type, void *event_data)
{
   Eupnp_Device_Info *d = event_data;
   IGD_Device *tmp;
   Eina_List *l;

   EINA_LIST_FOREACH(device_list, l, tmp)
     {
	if (!strcmp(d->udn, tmp->device->udn))
	  {
	     // Match, remove it
	     device_list_device_del(tmp);
	     break;
	  }
     }

   return EINA_TRUE;
}

static void
igd_client_win_del(void *data, Evas_Object *obj, void *event_info)
{
   elm_exit();
}

static void
igd_client_win_create(void)
{
   Evas_Object *win, *bg, *bx0, *lb, *fr;
   win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(win, "Internet Gateway Device Controller");
   evas_object_smart_callback_add(win, "delete-request", igd_client_win_del, NULL);
   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, 1.0, 1.0);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   bx0 = elm_box_add(win);
   evas_object_size_hint_weight_set(bx0, 1.0, 1.0);
   elm_win_resize_object_add(win, bx0);
   evas_object_show(bx0);

   fr = elm_frame_add(win);
   elm_frame_label_set(fr, "Instructions");
   elm_box_pack_end(bx0, fr);
   evas_object_show(fr);

   lb = elm_label_add(win);
   elm_label_label_set(lb, "IGD devices will be present on the list below<br>"
			   "as they're found. Click on them for performing <br>"
			   "actions.");
   elm_frame_content_set(fr, lb);
   evas_object_show(lb);

   li = elm_list_add(win);
   elm_list_always_select_mode_set(li, 1);
   evas_object_size_hint_weight_set(li, 1.0, 1.0);
   evas_object_size_hint_align_set(li, -1.0, -1.0);
   elm_box_pack_end(bx0, li);
   evas_object_show(li);

   evas_object_resize(win, 280, 280);
   evas_object_show(win);
}

EAPI int
elm_main(int argc, char **argv)
{
   Eupnp_Control_Point *c;

   if (!eupnp_ecore_init())
     {
	fprintf(stderr, "Could not initialize eupnp-ecore\n");
	return 0;
     }

   if (!eina_init())
     {
	fprintf(stderr, "Failed to initialize eina error module.\n");
	goto list_init_error;
     }

   if (!eupnp_control_point_init())
     {
	fprintf(stderr, "Could not initialize program resources\n");
	goto eupnp_cp_init_error;
     }

   c = eupnp_control_point_new();

   if (!c)
     {
	fprintf(stderr, "Could not create control point instance\n");
	goto eupnp_cp_alloc_error;
     }

   /* Subscribe for device events */
   eupnp_event_bus_subscribe(EUPNP_EVENT_DEVICE_READY,
			     EUPNP_CALLBACK(on_device_ready), NULL);
   eupnp_event_bus_subscribe(EUPNP_EVENT_DEVICE_GONE,
			     EUPNP_CALLBACK(on_device_gone), NULL);

   /* Start control point */
   eupnp_control_point_start(c);
   igd_client_win_create();

   /* Send a test search for all devices*/
   if (!eupnp_control_point_discovery_request_send(c, 5, IGD_DEVICE_ST))
     {
	WARN("Failed to perform MSearch.\n");
     }
   else
	DEBUG("MSearch sent sucessfully.\n");

   elm_run();

   /* Shutdown procedure */
   elm_shutdown();
   eupnp_control_point_stop(c);
   eupnp_control_point_free(c);
   eupnp_control_point_shutdown();
   eina_shutdown();
   eupnp_ecore_shutdown();
   return 0;

   eupnp_cp_alloc_error:
      eupnp_control_point_shutdown();
   eupnp_cp_init_error:
      eina_shutdown();
   list_init_error:
      eupnp_ecore_shutdown();
   return -1;
}
ELM_MAIN()
