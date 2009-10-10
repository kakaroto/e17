#include <E_Hal.h>
#include "e_hal_private.h"

int _e_dbus_hal_log_dom = -1;
int _e_dbus_hal_init_count = 0;

EAPI int
e_hal_init(void)
{
   if (++_e_dbus_hal_init_count != 1)
     return _e_dbus_hal_init_count;

   if (!eina_init())
     return --_e_dbus_hal_init_count;

   _e_dbus_hal_log_dom = eina_log_domain_register
     ("e_hal", E_DBUS_COLOR_DEFAULT);
   if (_e_dbus_hal_log_dom < 0)
     {
	EINA_LOG_ERR("Could not register 'e_hal' log domain.");
	goto shutdown_eina;
     }

   if (!e_dbus_init()) {
      ERR("Could not initialize E_DBus.");
      goto unregister_log_domain;
   }

   return _e_dbus_hal_init_count;

 unregister_log_domain:
   eina_log_domain_unregister(_e_dbus_hal_log_dom);
   _e_dbus_hal_log_dom = -1;
 shutdown_eina:
   eina_shutdown();

   return _e_dbus_hal_init_count;
}

EAPI int
e_hal_shutdown(void)
{
   if (--_e_dbus_hal_init_count != 0)
     return _e_dbus_hal_init_count;

   e_dbus_shutdown();

   eina_log_domain_unregister(_e_dbus_hal_log_dom);
   _e_dbus_hal_log_dom = -1;
   eina_shutdown();

   return _e_dbus_hal_init_count;
}
