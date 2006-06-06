#include "dEvian.h"

/* Public module routines */

EAPI E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,
   MODULE_NAME
};

EAPI int
e_modapi_init(E_Module *m)
{
   /* Init new module */
   if (!DEVIANF(devian_main_init) (m))
     return NULL;
   DEVIANM->module = m;

   /* Return pointer on struct of module */
   return DEVIANM;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   DEVIANM = m->data;

   if (DEVIANM)
     DEVIANF(devian_main_shutdown) ();

   DMAIN(("---END---\n"));

   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   if (DEVIANM)
     if (DEVIANM->conf)
       DEVIANF(config_save) ();

   return 1;
}

EAPI int
e_modapi_about(E_Module *m)
{
   char buf[4096];

   snprintf(buf, sizeof(buf), _("Enlightenment %s Module - version " MODULE_VERSION), MODULE_NAME);
   e_module_dialog_show(buf, _(
#ifndef HAVE_ALL
                               "This module is part of dEvian module<br><br>"
#ifdef HAVE_PICTURE
                               "Module to display pictures on your desktop<br>"
#else
#ifdef HAVE_RSS
                               "Module to display RSS feeds on your desktop<br>"
#endif
#endif
#else
                               "Module to display informations / pictures on your desktop<br>"
#endif
                               "with eye-candy effects<br><br>" "http://oooo.zapto.org<br>" "ooookiwi@free.fr"));
   return 1;
}

EAPI int
e_modapi_config(E_Module *m)
{
   if (!m)
     return 0;
   if (!m->data)
     return 0;
   if (!DEVIANM)
     return 0;
   DEVIANF(config_dialog_main) (e_container_current_get(e_manager_current_get()));
   return 1;
}

/* End of public module routines */
