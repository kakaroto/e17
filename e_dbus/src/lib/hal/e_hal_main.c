#include <E_Hal.h>
#include <E_Ukit.h>
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

   if (!e_ukit_init()) {
      ERR("Could not initialize E_UKit.");
      goto shutdown_dbus;
   }

   return _e_dbus_hal_init_count;

 shutdown_dbus:
   e_dbus_shutdown();
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

   e_ukit_shutdown();
   e_dbus_shutdown();

   eina_log_domain_unregister(_e_dbus_hal_log_dom);
   _e_dbus_hal_log_dom = -1;
   eina_shutdown();

   return _e_dbus_hal_init_count;
}
