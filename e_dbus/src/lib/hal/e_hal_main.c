#include <E_Hal.h>
#include "e_hal_private.h"

int _e_dbus_hal_log_dom = -1;
int _e_dbus_hal_init_count = 0;

EAPI int
e_hal_init(void)
{
   if (_e_dbus_hal_init_count)
     return ++_e_dbus_hal_init_count;

   if (!eina_init())
     return 0;

   _e_dbus_hal_log_dom = eina_log_domain_register
     ("e_hal", E_DBUS_COLOR_DEFAULT);
   if (_e_dbus_hal_log_dom < 0)
     {
	EINA_LOG_ERR("Could not register 'e_hal' log domain.");
	goto error_log_domain;
	return 0;
     }

   if (!e_dbus_init()) {
      ERR("Could not initialize E_DBus.");
      goto error_e_dbus;
   }

   _e_dbus_hal_init_count = 1;
   return 1;

 error_e_dbus:
   eina_log_domain_unregister(_e_dbus_hal_log_dom);
   _e_dbus_hal_log_dom = -1;
 error_log_domain:
   eina_shutdown();
   return 0;
}

EAPI int
e_hal_shutdown(void)
{
   _e_dbus_hal_init_count--;
   if (_e_dbus_hal_init_count)
     return _e_dbus_hal_init_count;

   e_dbus_shutdown();

   eina_log_domain_unregister(_e_dbus_hal_log_dom);
   _e_dbus_hal_log_dom = -1;
   eina_shutdown();

   return 0;
}
