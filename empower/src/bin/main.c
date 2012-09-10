#include "Empower.h"
#include "pka.h"
#include "gui.h"

static Eina_Bool _auth_idler(void *data);
static Eina_Bool _exit_cb(void *data, int type, void *event);

#ifndef ELM_LIB_QUICKLAUNCH
int elm_main(int argc, char** argv)
{
  // initialize gettext
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
  textdomain(PACKAGE);

  if(!eina_init())
  {
    printf(_("Unable to init Eina!"));
    return 1;
  }

  if(!ecore_init())
  {
    printf(_("Unable to init ecore\n"));
    return 1;
  }

  if(!efreet_init())
  {
    printf(_("Unable to init efreet"));
  }

  if(!elm_init(argc, argv))
  {
    printf(_("Unable to init elementary"));
  }

  empower_log_domain = eina_log_domain_register("empower", EINA_COLOR_BLUE);

  if (!gui_init())
  {
    ERR(_("Unable to initialize GUI!"));
    return 1;
  }

  if (!pka_init())
  {
    ERR(_("Unable to initialize Polkit Agent interface!"));
    return 1;
  }

  ecore_idle_enterer_add(_auth_idler, NULL);
  ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, _exit_cb, NULL);

  // Allow use of svg icons
  efreet_icon_extension_add(".svg");

  // Grab UID of the process.  This will be used to set
  // the default user when the UI is first shown.
  empower_uid = getuid();

  elm_run();

  elm_shutdown();
  efreet_shutdown();
  ecore_shutdown();
  eina_shutdown();
  eina_log_domain_unregister(empower_log_domain);
  empower_log_domain = -1;

  return 0;
}
#endif
ELM_MAIN()

static Eina_Bool _auth_idler(void *data)
{
  Empower_Auth_Info *info;

  if (pka_state() == UNREGISTERED)
  {
    elm_exit();
    return ECORE_CALLBACK_CANCEL;
  }

  info = pka_pending();
  if (info)
  {
    if (!gui_visible_get())
    {
      DBG("Action ID = %s", info->action);
      DBG("Message = %s", info->message);
      DBG("Icon = %s", info->icon);
      DBG("Cookie = %s", info->cookie);

      gui_show(info);
    }
  }

  return ECORE_CALLBACK_RENEW;
}

static Eina_Bool _exit_cb(void *data, int type, void *event)
{
  // Call auth shutdown which will exit elm main loop once
  // we've unregisterd as an authentication agent.
  pka_shutdown();

  return EINA_FALSE;
}
