#include <stdio.h>
#include <string.h>

#include <Eina.h>
#include <Elementary.h>
#include <eupnp.h>


/*
 * GUI functions
 */
void lighting_control_win_create(void);

EAPI int
elm_main(int argc, char **argv)
{
   Eupnp_Control_Point *c;

   if (!eupnp_ecore_init())
     {
	fprintf(stderr, "Could not initialize eupnp-ecore\n");
	return 0;
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
/*   eupnp_event_bus_subscribe(EUPNP_EVENT_DEVICE_READY,
			     EUPNP_CALLBACK(on_device_ready), NULL);
   eupnp_event_bus_subscribe(EUPNP_EVENT_DEVICE_GONE,
			     EUPNP_CALLBACK(on_device_gone), NULL);
*/

   /* Start control point */
   eupnp_control_point_start(c);
   lighting_control_win_create();

   /* Send a test search for all devices*/
/*   if (!eupnp_control_point_discovery_request_send(c, 5, IGD_DEVICE_ST))
     {
	WARN("Failed to perform MSearch.\n");
     }
   else
	DEBUG("MSearch sent sucessfully.\n");
*/
   elm_run();

   /* Shutdown procedure */
   elm_shutdown();
   eupnp_control_point_stop(c);
   eupnp_control_point_free(c);
   eupnp_control_point_shutdown();
   eupnp_ecore_shutdown();
   return 0;

   eupnp_cp_alloc_error:
      eupnp_control_point_shutdown();
   eupnp_cp_init_error:
      eupnp_ecore_shutdown();
   return -1;
}
ELM_MAIN()
