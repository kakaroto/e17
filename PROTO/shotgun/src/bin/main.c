#include "ui.h"

int ui_log_dom = -1;
static Ecore_Event_Handler *dh = NULL;
static Ecore_Event_Handler *ch = NULL;
Eina_Bool ssl_verify = EINA_TRUE;

static Eina_Bool
con_state(void *d __UNUSED__, int type __UNUSED__, Shotgun_Auth *auth __UNUSED__)
{
   INF("Connection state++");
   return ECORE_CALLBACK_RENEW;
}


static Eina_Bool
disc(Contact_List *cl, int type __UNUSED__, Shotgun_Auth *auth __UNUSED__)
{
   Eina_List *l;
   Contact *c;
   double dtime;
   INF("Disconnected");
   if (!cl)
     {
        Login_Window *lw;

        lw = login_new(auth);
        login_fill(lw);
        login_fail(lw, 0, NULL);
        dh = NULL;
        return ECORE_CALLBACK_CANCEL;
     }
   dtime = ecore_time_get();
   EINA_LIST_FOREACH(cl->users_list, l, c)
     contact_presence_clear(c);
   if (cl->pager)
     {
        evas_object_del(cl->pager);
        cl->pager_entries = eina_list_free(cl->pager_entries);
        cl->pager = NULL;
        cl->pager_state = 0;
     }
   if (dtime - cl->disconnect_time < 1.) ecore_main_loop_quit();
   else shotgun_connect(cl->account);
   cl->disconnect_time = dtime;
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
con(Contact_List *cl, int type __UNUSED__, Shotgun_Auth *auth)
{
   Shotgun_Settings *ss;
   INF("Connected!");

   if (!dh)
     dh = ecore_event_handler_add(SHOTGUN_EVENT_DISCONNECT, (Ecore_Event_Handler_Cb)disc, NULL);
   ss = shotgun_settings_get(auth);
   if ((!cl) || (cl->type))
     {/* don't mess up already-created list on reconnect */
        Eina_Bool set = EINA_TRUE;
        cl = contact_list_init(ss ? ss->ui : NULL, auth);
        if (cl->settings->enable_presence_save)
          set = !ui_eet_presence_get(auth);
        if (set)
          shotgun_presence_set(auth, SHOTGUN_USER_STATUS_CHAT, "testing SHOTGUN!", 1);
        if (!cl->settings->disable_reconnect)
          ecore_event_handler_data_set(dh, cl);
        ecore_event_handler_data_set(ch, cl);
        logging_dir_create(cl);
        if (cl->settings->allowed_image_age) ui_eet_idler_start(cl);
#ifdef HAVE_DBUS
        ui_dbus_init(cl);
#endif
#ifdef HAVE_AZY
        ui_azy_init(cl);
        if (ui_azy_connect(cl))
          ecore_timer_add(24 * 60 * 60, (Ecore_Task_Cb)ui_azy_connect, cl);
#endif

     }
   shotgun_iq_server_query(auth);
   shotgun_iq_roster_get(auth);
   shotgun_presence_send(auth);

   return ECORE_CALLBACK_RENEW;
}

static void
_setup_extension(void)
{
   char buf[4096];
   snprintf(buf, sizeof(buf), "%s/default.edj", util_configdir_get());
   if (ecore_file_exists(buf))
     elm_theme_extension_add(NULL, buf);
   else
     elm_theme_extension_add(NULL, PACKAGE_DATA_DIR "/default.edj");
}

int
main(int argc, char *argv[])
{
   char *pass;
   Shotgun_Auth *auth = NULL;
   char *getpass_x(const char *prompt);
   int x;

   eina_init();
   shotgun_init();
   efx_init();
   elm_init(argc, argv);
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);

   ui_log_dom = eina_log_domain_register("shotgun_ui", EINA_COLOR_LIGHTRED);
   eina_log_domain_level_set("shotgun_ui", EINA_LOG_LEVEL_DBG);
   if (!ecore_con_ssl_available_get())
     {
        CRI("SSL support is required in ecore!");
        exit(1);
     }
   _setup_extension();
   //eina_log_domain_level_set("ecore_con", EINA_LOG_LEVEL_DBG);
   ecore_event_handler_add(ECORE_CON_EVENT_URL_DATA, (Ecore_Event_Handler_Cb)chat_image_data, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_URL_COMPLETE, (Ecore_Event_Handler_Cb)chat_image_complete, NULL);
   ch = ecore_event_handler_add(SHOTGUN_EVENT_CONNECT, (Ecore_Event_Handler_Cb)con, NULL);
   ecore_event_handler_add(SHOTGUN_EVENT_CONNECTION_STATE, (Ecore_Event_Handler_Cb)con_state, NULL);
//   eina_log_abort_on_critical_level_set(EINA_LOG_LEVEL_CRITICAL);
//   eina_log_abort_on_critical_set(EINA_TRUE);

   for (x = 0; x < argc; x++)
     {
        if (argv[x][0] != '-') continue;
        if (strcmp(argv[x], "--noverify")) continue;
        ssl_verify = EINA_FALSE;
        break;
     }
   if (!ssl_verify)
     {
        /* ignore --noverify after parsing */
        for (;x < argc; x++)
          argv[x] = argv[x + 1];
        argc--;
     }
   ecore_app_args_set(argc, (const char**)argv);
   switch (argc - 1)
     {
      case 0:
        auth = ui_eet_auth_get(NULL, NULL);
        break;
      case 1:
        auth = ui_eet_auth_get(argv[1], NULL);
        break;
      case 2:
        auth = ui_eet_auth_get(argv[1], argv[2]);
        break;
      case 3:
        auth = shotgun_new(argv[1], argv[2], argv[3]);
      default:
        break;
     }

   if (auth)
     {
        if (!shotgun_password_get(auth))
          {
             pass = getpass_x("Password: ");
             if (!pass)
               {
                  ERR("No password entered!");
                  return 1;
               }
             shotgun_password_set(auth, pass);
          }
        if (!ui_eet_init(auth))
          {
             CRI("Could not initialize eet backend!");
             return 1;
          }
        shotgun_ssl_verify_set(auth, ssl_verify);
        dh = ecore_event_handler_add(SHOTGUN_EVENT_DISCONNECT, (Ecore_Event_Handler_Cb)disc, NULL);
        shotgun_connect(auth);
     }
   else
     login_new(NULL);

   ecore_main_loop_begin();
#if 0
     {
        fprintf(stderr, "Usage: %s [--(enable|disable)-illume] [server] [username] [domain]\n", argv[0]);
        fprintf(stderr, "Usage example: %s talk.google.com my_username gmail.com\n", argv[0]);
        fprintf(stderr, "Usage example (with saved account): %s\n", argv[0]);
        fprintf(stderr, "Usage example (with saved account): %s my_username gmail.com\n", argv[0]);
        fprintf(stderr, "Usage example (with saved account): %s my_username@gmail.com\n", argv[0]);
        return 1;
     }
#endif

   elm_shutdown();
   efx_shutdown();

   return 0;
}
