#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <E_Ukit.h>
#include "e_ukit_private.h"

int _e_dbus_ukit_log_dom = -1;
int _e_dbus_ukit_init_count = 0;

EAPI int
e_ukit_init(void)
{
   if (++_e_dbus_ukit_init_count != 1)
     return _e_dbus_ukit_init_count;

   if (!eina_init())
     return --_e_dbus_ukit_init_count;

   _e_dbus_ukit_log_dom = eina_log_domain_register
     ("e_ukit", E_DBUS_COLOR_DEFAULT);
   if (_e_dbus_ukit_log_dom < 0)
     {
        EINA_LOG_ERR("Could not register 'e_ukit' log domain.");
        goto shutdown_eina;
     }

   if (!e_dbus_init())
     {
        ERR("Could not initialize E_DBus.");
        goto unregister_log_domain;
     }

   return _e_dbus_ukit_init_count;

   unregister_log_domain:
     eina_log_domain_unregister(_e_dbus_ukit_log_dom);
     _e_dbus_ukit_log_dom = -1;
   shutdown_eina:
     eina_shutdown();

   return _e_dbus_ukit_init_count;
}

EAPI int
e_ukit_shutdown(void)
{
   if (--_e_dbus_ukit_init_count != 0)
     return _e_dbus_ukit_init_count;

   e_dbus_shutdown();

   eina_log_domain_unregister(_e_dbus_ukit_log_dom);
   _e_dbus_ukit_log_dom = -1;
   eina_shutdown();

   return _e_dbus_ukit_init_count;
}
