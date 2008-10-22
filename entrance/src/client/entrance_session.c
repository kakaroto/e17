#include "config.h"
#include "entrance.h"
#include "entrance_session.h"
#include <Ecore_Config.h>
#include <Esmart/Esmart_Container.h>
#include <Esmart/Esmart_Text_Entry.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "entrance_user.h"
#include "entrance_x_session.h"
#include "entrance_ipc.h"

/**
@file entrance_session.c
@brief Variables and Data relating to an instance of the application as a whole

*/

extern void session_item_selected_cb(void *data, Evas_Object * o,
                                     const char *emission,
                                     const char *source);
extern void user_selected_cb(void *data, Evas_Object * o,
                             const char *emission, const char *source);
extern void user_unselected_cb(void *data, Evas_Object * o,
                               const char *emission, const char *source);
static void _entrance_session_user_list_fix(Entrance_Session * e);
static void _entrance_session_execute_in_shell(char *user, char *shell,
                                               char *session_cmd,
                                               char *session_name);

/**
 * entrance_session_new: allocate a new  Entrance_Session
 * @param config Parse this config file instead of the normal system one
 * @param config The display this session will be running on
 * @param testing Whether we're a real login app, or a test window
 * @return a valid Entrance_Session
 * Also Allocates the auth, and parse the config struct 
 */
Entrance_Session *
entrance_session_new(const char *config, const char *display, int testing)
{
   Entrance_Session *e;

   e = (Entrance_Session *) malloc(sizeof(struct _Entrance_Session));
   memset(e, 0, sizeof(struct _Entrance_Session));

   if (config)
      e->db = strdup(config);
   else
      e->db = strdup(PACKAGE_CFG_DIR "/entrance_config.cfg");

   openlog("entrance", LOG_NOWAIT, LOG_DAEMON);
   if (!display)
   {
      if (!getenv("DISPLAY"))
      {
         syslog(LOG_CRIT, "entrance_session_new: Unexpected error occured.");
         exit(EXITCODE);
      }
      e->display = strdup(getenv("DISPLAY"));
   }
   else
      e->display = strdup(display);

   e->auth = entrance_auth_new();
   e->config = entrance_config_load(e->db);
   if (!e->config)
   {
      fprintf(stderr, "Could not load %s\n", e->db);
      syslog(LOG_CRIT, "Fatal Error: Unable to read config file %s.", e->db);
      exit(EXITCODE);
   }
   e->session = strdup("");
   e->session_selected = 0;
   e->testing = testing;
   return (e);
}

/**
 * entrance_session_ecore_evas_set : 
 * @param e - the Entrance_Session to set the ecore evas for
 * @param ee - the pointer to a fully setup Ecore_Evas we want to run
 */
void
entrance_session_ecore_evas_set(Entrance_Session * e, Ecore_Evas * ee)
{
   Evas *evas = NULL;

   if (!e || !ee)
      return;

   e->ee = ee;
   if ((evas = ecore_evas_get(ee)))
   {
      Eina_List *l;

      /* free the font list when we're done */
      for (l = e->config->fonts; l; l = l->next)
      {
         evas_font_path_append(evas, (char *) l->data);
         free(l->data);
      }
      e->config->fonts = eina_list_free(e->config->fonts);
   }
}

/**
 * entrance_session_free: free the entrance session
 * @param e - the Entrance_Session to free
 */
void
entrance_session_free(Entrance_Session * e)
{
   if (e)
   {
      if (e->auth)
      {
         entrance_auth_free(e->auth);
         e->auth = NULL;
      }
      if (e->edje)
      {
         evas_object_hide(e->edje);
         evas_object_del(e->edje);
         e->edje = NULL;
      }
      if (e->user_container)
      {
         evas_object_hide(e->user_container);
         evas_object_del(e->user_container);
         e->user_container = NULL;
      }
      if (e->session_container)
      {
         evas_object_hide(e->session_container);
         evas_object_del(e->session_container);
         e->session_container = NULL;
      }
      if (e->config)
      {
         entrance_config_free(e->config);
         e->config = NULL;
      }
      if (e->db)
      {
         free(e->db);
         e->db = NULL;
      }
      if (e->ee)
      {
         ecore_evas_hide(e->ee);
         ecore_evas_free(e->ee);
         e->ee = NULL;
      }
      if (e->session)
      {
         free(e->session);
         e->session = NULL;
      }

      closelog();

      free(e);
   }
}

/**
 * entrance_session_run: Be a login dm, start running
 * @param e - the Entrance_Session to be run
 */
void
entrance_session_run(Entrance_Session * e)
{
   int ok = 0;
   Entrance_User *eu = NULL;

   switch (e->config->autologin.mode)
   {
     case ENTRANCE_AUTOLOGIN_NONE:
        ecore_evas_show(e->ee);
        if (e->config->presel.mode == ENTRANCE_PRESEL_PREV
            && strlen(e->config->presel.prevuser))
        {
           Evas_Object *oo =
              evas_object_name_find(evas_object_evas_get(e->edje),
                                    "entrance.entry.user");
           if (oo)
           {
              esmart_text_entry_text_set(oo, e->config->presel.prevuser);
              entrance_session_user_set(e, e->config->presel.prevuser);
           }
        }
        break;
     case ENTRANCE_AUTOLOGIN_DEFAULT:
        if ((eu =
             evas_hash_find(e->config->users.hash,
                            e->config->autologin.username)))
        {
           ok = 1;
           e->authed = 1;
           entrance_session_user_set(e, eu->name);
        }
        break;
     case ENTRANCE_AUTOLOGIN_THEME:
        ecore_evas_show(e->ee);
        if ((eu =
             evas_hash_find(e->config->users.hash,
                            e->config->autologin.username)))
        {
           e->authed = 1;
           entrance_session_user_set(e, eu->name);
           edje_object_signal_emit(e->edje, "entrance,user,auth,success", "");
        }
        break;
   }
   if (!ok)
   {
      ecore_main_loop_begin();
   }
}

/**
 * entrance_session_auth_user: attempt to authenticate the user
 * @param e - the entrance session we're trying to auth
 * @return 0 on success errors otherwise
 */
int
entrance_session_auth_user(Entrance_Session * e)
{
   return (entrance_auth_cmp(e->auth, e->display, e->config->auth));
}

/**
 * entrance_session_user_reset: forget what we know about the current user
 * @param e - the entrance session we should forget the user for 
 */
void
entrance_session_user_reset(Entrance_Session * e)
{
   if (e)
   {
      Evas_Object *obj = NULL;

      if (e->auth)
         entrance_auth_free(e->auth);
      e->auth = entrance_auth_new();
      if ((obj =
           edje_object_part_swallow_get(e->edje, "entrance.user.avatar")))
      {
         edje_object_part_unswallow(e->edje, obj);
         evas_object_del(obj);
      }
      edje_object_signal_emit(e->edje, "In", "entrance.entry.user");
   }
}

/**
 * entrance_session_user_set: forget what we know about the current user,
 * load what info we can from the entrance user parameter, so we have a new
 * user in our "entrance.user.avatar" edje
 * @param e - the entrance sesssion currently running
 * @param eu - the new entrance user we're setting as "current"
 */
int
entrance_session_user_set(Entrance_Session * e, const char *user)
{
   int result = 1;
   Evas_Object *obj = NULL;
   const char *file = NULL;
   Entrance_X_Session *exs = NULL;
   Entrance_User *eu = NULL;

   if (e)
   {
      if ((obj =
           edje_object_part_swallow_get(e->edje, "entrance.user.avatar")))
      {
         edje_object_part_unswallow(e->edje, obj);
         evas_object_del(obj);
      }
      if (!entrance_auth_user_set(e->auth, user))
      {
         if ((eu = evas_hash_find(e->config->users.hash, user)) == NULL)
            eu = entrance_user_new(strdup(user), NULL, e->session);

         if (!(e->session_selected) && (eu->session) && (eu->session[0] != 0))
         {
            if ((exs = evas_hash_find(e->config->sessions.hash, eu->session)))
            {
               if (e->session)
                  free(e->session);
               e->session = strdup(eu->session);
               entrance_session_x_session_set(e, exs);
            }
         }
         edje_object_file_get(e->edje, &file, NULL);
         if ((obj = entrance_user_edje_get(eu, e->edje, file)))
         {
            evas_object_layer_set(obj, evas_object_layer_get(e->edje));
            if (edje_object_part_exists(e->edje, "entrance.user.avatar"))
            {
               edje_object_part_swallow(e->edje, "entrance.user.avatar", obj);
            }
         }
         edje_object_signal_emit(e->edje, "In", "entrance.entry.pass");
         edje_object_signal_emit(e->edje, "entrance,user,success", "");
         edje_object_signal_emit(e->edje, "entrance,user,selected", "");
         result = 0;
      }
      else
      {
         edje_object_signal_emit(e->edje, "entrance,user,fail", "");
      }
   }
   return (result);
}

int
entrance_session_pass_set(Entrance_Session * e, const char *pass)
{
   int result = 1;

   if (e)
   {
      entrance_auth_pass_set(e->auth, pass);
      result = entrance_session_auth_user(e);
   }
   return (result);
}

/**
 * entrance_session_user_session_default_set : change the current
 * EntranceUser's default session to what Entrance itself currently has in
 * context.  This only will get written if someone successfully auths after
 * a selection like this is made, but it allows themes etc to have dialogs
 * deciding whether or not they should save this new session as default
 * @param e - the currently running session
 */
void
entrance_session_user_session_default_set(Entrance_Session * e)
{
   if (e && e->config && e->auth && e->auth->user && e->session)
   {
      Entrance_User *eu = NULL;

      if ((eu = evas_hash_find(e->config->users.hash, e->auth->user)))
      {
         if (eu->session)
            free(eu->session);
         eu->session = strdup(e->session);
      }
   }
}

/**
 * entrance_session_setup_user_session: Setup user session
 * Set up user's environment, including environment variables,
 * Xauth cookie and any other necessary parameters
 * @param e The current Entrance Session
 */
void
entrance_session_setup_user_session(Entrance_Session * e)
{
   char *homedir;

   entrance_auth_setup_environment(e->auth, e->display);
   homedir = getenv("HOME");
   if (entrance_ipc_connected_get())
      entrance_ipc_request_xauth(homedir, e->auth->pw->pw_uid,
                                 e->auth->pw->pw_gid);
   else
     {
	syslog(LOG_INFO, "Cannot send xauth reqest to entranced");
	/* No daemon available, assume no xauth */
	ecore_main_loop_quit();
     }
}

/**
 * entrance_session_start_user_session: Launch user session
 * This function launches the requested X session using the 
 * login watchdog process
 * @param e - the currently running session
 */
void
entrance_session_start_user_session(Entrance_Session * e)
{
   pid_t pid;
   char pids[128];
   char *shell = NULL;
   char *user = NULL;
   char *session_cmd = NULL;
   char *session_name = NULL;
   char *monitor_cmd = PACKAGE_LIBEXEC_DIR "/" PACKAGE "/entrance_login";
   struct passwd *pwent = NULL;
   Entrance_X_Session *exs = NULL;

   if (e->session)
      exs = evas_hash_find(e->config->sessions.hash, e->session);

   entrance_auth_setup_environment(e->auth, e->display);
   if ((exs->session) && (exs->session[0] != 0))
   {
      if (!strcmp(exs->session, "default"))
         session_cmd = strdup(e->config->xsession);
      else if (exs->session[0] == '/')
         session_cmd = strdup(exs->session);
      else
      {
         session_cmd = strdup(e->config->xsession);
         session_name = strdup(exs->session);
      }
   }
   else
   {
      /* Default session */
      session_cmd = strdup(e->config->xsession);
   }

   if (e->testing)
   {
      printf("Selected session command: %s", session_cmd);
      if (session_name)
         printf(" %s", session_name);
      printf("\n");
      fflush(stdout);
      session_cmd = strdup("xterm");
      if (session_name)
         free(session_name);
      session_name = NULL;
   }

   if (session_name)
      syslog(LOG_INFO, "Executing %s %s", session_cmd, session_name);
   else
      syslog(LOG_INFO, "Executing %s", session_cmd);

   if (e->ee)
   {
      ecore_evas_hide(e->ee);
      ecore_evas_free(e->ee);
      e->ee = NULL;
   }

   user = strdup(e->auth->user);

   syslog(LOG_NOTICE, "Starting session for user \"%s\".", user);

#ifdef HAVE_PAM
   if (e->config->auth == ENTRANCE_USE_PAM)
   {
      char **pamenv, **envitem;

      /* Tell PAM that session has begun */
      if (pam_open_session(e->auth->pam.handle, 0) != PAM_SUCCESS)
      {
         syslog(LOG_NOTICE, "Cannot open pam session for user \"%s\".",
                e->auth->user);
         if (!e->config->autologin.mode)
         {
            syslog(LOG_CRIT, "Unable to open PAM session. Aborting.");
            return;
         }
      }

      if ((pamenv = pam_getenvlist(e->auth->pam.handle)) != NULL)
      {
	for (envitem = pamenv; *envitem; envitem++)
	{
	  putenv(*envitem);
	  free(*envitem);
	}

	free(pamenv);
     }

      syslog(LOG_INFO, "Opened PAM session. %s : %s.", e->auth->pw->pw_name,
             e->display);
   }
#endif

   _entrance_session_user_list_fix(e);

   entrance_config_prevuser_save(e->auth->user, e->db);

   syslog(LOG_INFO, "DEBUG1");
   /* avoid doubling up pam handles before the fork */
   pwent = struct_passwd_dup(e->auth->pw);
   syslog(LOG_INFO, "DEBUG2");
   entrance_auth_free(e->auth);
   e->auth = NULL;
   syslog(LOG_INFO, "DEBUG3");

   /* Shutdown subsytems */
   edje_shutdown();
   syslog(LOG_INFO, "DEBUG4");
   ecore_evas_shutdown();
   syslog(LOG_INFO, "DEBUG5");
   ecore_config_shutdown();
   syslog(LOG_INFO, "DEBUG6");
   ecore_x_sync();
   syslog(LOG_INFO, "DEBUG7");
   entrance_ipc_shutdown();
   syslog(LOG_INFO, "DEBUG8");

   switch ((pid = fork()))
   {
     case 0:
        if (initgroups(pwent->pw_name, pwent->pw_gid))
           syslog(LOG_CRIT,
                  "Unable to initialize group (is entrance running as root?).");
        if (setgid(pwent->pw_gid))
           syslog(LOG_CRIT, "Unable to set group id.");
        if (setuid(pwent->pw_uid))
           syslog(LOG_CRIT, "Unable to set user id.");
        shell = strdup(pwent->pw_shell);

        /* replace this process with a clean small one that just waits for
           its */
        /* child to exit.. passed on the cmd-line */

        _entrance_session_execute_in_shell(user, shell, session_cmd,
                                           session_name);
        break;
     case -1:
        syslog(LOG_INFO, "FORK FAILED, UH OH");
        exit(0);
     default:
        syslog(LOG_NOTICE,
               "Replacing Entrance with simple login program to wait for session end.");
        /* this bypasses a race condition where entrance loses its x
           connection before the wm gets it and x goes and resets itself */
        sleep(30);

        /* FIXME These should be called! */
        ecore_x_shutdown();
        ecore_shutdown();

        break;
   }
/* no need to free - we are goign to exec ourselves and be replaced   
   struct_passwd_free(pwent);
   entrance_session_free(e);
   if (shell) free(shell);
   if (user) free(user);
 */
   /* replace this process with a clean small one that just waits for its */
   /* child to exit.. passed on the cmd-line */

   /* this causes entreance to reset - bad bad bad */
   snprintf(pids, sizeof(pids), "%i", (int) pid);
#ifdef HAVE_PAM
   if (e->config->auth == ENTRANCE_USE_PAM)
   {
      syslog(LOG_NOTICE, "Exec entrance login replacement: %s %s %s %s",
             monitor_cmd, pids, pwent->pw_name, e->display);
      execl(monitor_cmd, monitor_cmd, pids, pwent->pw_name, e->display, NULL);
   }
   else
#endif
   {
      syslog(LOG_NOTICE, "Exec entrance login replacement: %s %s",
             monitor_cmd, pids);
      execl(monitor_cmd, monitor_cmd, pids, NULL);
   }
   pause();
}


/**
 * entrance_session_xsession_set : Set the current xsesssion to the
 * specified key, emit a signal to the main edje letting it know the main
 * session has changed
 * @param e - the entrance session you want to set the session for
 * @param exs - the Entrance_X_Session we want to be the new current
 */
void
entrance_session_x_session_set(Entrance_Session * e, Entrance_X_Session * exs)
{
   if (e && e->edje && exs)
   {
      const char *file = NULL;
      Evas_Object *o = NULL, *old_o = NULL;
      Entrance_User *eu = NULL;

      edje_object_file_get(e->edje, &file, NULL);
      if ((o = entrance_x_session_edje_get(exs, e->edje, file)))
      {
         if (e->session)
            free(e->session);
         e->session = strdup(exs->name);

         if ((eu =
              evas_hash_find(e->config->users.hash, e->auth->user)) != NULL)
         {
            if (eu->session)
               free(eu->session);
            eu->session = strdup(e->session);
         }

         old_o =
            edje_object_part_swallow_get(e->edje,
                                         "entrance.xsessions.selected");
         if (old_o)
         {
            edje_object_part_unswallow(e->edje, old_o);
            evas_object_del(old_o);
         }
         edje_object_part_swallow(e->edje, "entrance.xsessions.selected", o);
         evas_object_layer_set(o, evas_object_layer_get(e->edje));
         edje_object_signal_emit(e->edje, "entrance,xsession,changed", "");
      }
   }
}

/**
 * Set the main edje for the session to be the parameter passed in
 * @param e The entrance session you want to modify
 * @param obj The new edje you're specifying
 */
void
entrance_session_edje_object_set(Entrance_Session * e, Evas_Object * obj)
{
   if (e)
   {
      if (e->edje)
         evas_object_del(e->edje);
      e->edje = obj;
   }
}

/**
 * Set the edje object for the background.
 * @param e The Entrance Session to be modified
 * @param obj The new Edje background object
 */
void
entrance_session_bg_object_set(Entrance_Session * e, Evas_Object * obj)
{
   if (e)
   {
      if (e->background)
         evas_object_del(e->background);
      e->background = obj;
   }
}

/**
 * entrance_session_xsession_list_add : fine the "entrance.xsessions.list" part in the
 * main edje, setup the container to hold the elements, and create session
 * edjes for the container based on our session list in the config
 * @param e - the entrance session you want to add the xsession list to
 */
void
entrance_session_xsession_list_add(Entrance_Session * e)
{
   Evas_Coord w, h;
   Eina_List *l = NULL;
   const char *key = NULL;
   Evas_Object *edje = NULL;
   Entrance_X_Session *exs = NULL;
   Evas_Object *container = NULL;

   if (!e || !e->edje || !e->config)
      return;
   edje_object_part_geometry_get(e->edje, "entrance.xsessions.list", NULL,
                                 NULL, &w, &h);
   if ((container = esmart_container_new(evas_object_evas_get(e->edje))))
   {
      esmart_container_padding_set(container, 0, 0, 0, 0);
      esmart_container_spacing_set(container, 0);
      esmart_container_move_button_set(container, 2);
      if (w > h)
      {
         esmart_container_fill_policy_set(container,
                                          CONTAINER_FILL_POLICY_KEEP_ASPECT);
         esmart_container_direction_set(container, 0);
      }
      else
      {
         esmart_container_fill_policy_set(container,
                                          CONTAINER_FILL_POLICY_KEEP_ASPECT);
         esmart_container_direction_set(container, 1);
      }

      for (l = e->config->sessions.keys; l; l = l->next)
      {
         key = (const char *) l->data;
         if ((exs = evas_hash_find(e->config->sessions.hash, key)))
         {
            edje = entrance_x_session_button_new(exs, e->edje);
            if (edje)
               esmart_container_element_append(container, edje);
         }

      }

      edje_object_part_swallow(e->edje, "entrance.xsessions.list", container);
      e->session_container = container;
   }
}

void
entrance_session_list_direction_set(Entrance_Session * e,
                                    Evas_Object * container,
                                    const char *direction)
{
   if (!e || !e->edje || !container || !direction)
      return;
   if (!strcmp(direction, "horizontal"))
   {
      esmart_container_direction_set(container, 0);
   }
   else
   {
      esmart_container_direction_set(container, 1);
   }
}

/**
 * entrance_session_user_list_add : find the "entrance.users.list" object in
 * the main edje, setup the container to hold the elements, and create user
 * edjes for the container with our user list in the config
 * @param e - the entrance session you want to add the user list to
 */
void
entrance_session_user_list_add(Entrance_Session * e)
{
   char *str = NULL;
   Evas_Coord w, h;
   const char *file = NULL;
   Entrance_User *key = NULL;
   Eina_List *l = NULL;
   Evas_Object *container = NULL, *edje;

   if (!e || !e->edje)
      return;
   edje_object_part_geometry_get(e->edje, "entrance.users.list", NULL, NULL,
                                 &w, &h);
   if ((container = esmart_container_new(evas_object_evas_get(e->edje))))
   {
      esmart_container_padding_set(container, 0, 0, 0, 0);
      esmart_container_spacing_set(container, 0);
      esmart_container_move_button_set(container, 2);
      if (w > h)
      {
         esmart_container_fill_policy_set(container,
                                          CONTAINER_FILL_POLICY_KEEP_ASPECT);
         esmart_container_direction_set(container, 0);
      }
      else
      {
         esmart_container_fill_policy_set(container,
                                          CONTAINER_FILL_POLICY_KEEP_ASPECT);
         esmart_container_direction_set(container, 1);
      }
      edje_object_file_get(e->edje, &file, NULL);
      for (l = e->config->users.keys; l; l = l->next)
      {
         str = (char *) l->data;
         if ((key = evas_hash_find(e->config->users.hash, str)))
         {
            if ((edje = entrance_user_edje_get(key, e->edje, file)))
               esmart_container_element_append(container, edje);
         }
      }
      edje_object_part_swallow(e->edje, "entrance.users.list", container);
      e->user_container = container;
   }

}

/**
 * entrance_session_default_xsession_get : Return the hash key for the
 * session that's the first item in the system's session list
 * @param e - the entrance session you're working with
 */
Entrance_X_Session *
entrance_session_x_session_default_get(Entrance_Session * e)
{
   Eina_List *l = NULL;
   Entrance_X_Session *result = NULL;

   if (e && e->config)
   {
      if ((l = e->config->sessions.keys))
      {
         result =
            evas_hash_find(e->config->sessions.hash, (const char *) l->data);
      }
   }
   return (result);
}

#if 0
/**
 * given the key, try loading an instance
 * of the "Session" group from the current theme.  printf on failure. :(
 * @return the new session object, or NULL on failure.
 * @param e - the entrance session you're working with
 * @param key - the key for the EntranceSessionIcon file in the
 * EntranceSession edje
 */
Evas_Object *
entrance_session_xsession_edje_load(Entrance_Session * e, const char *key)
{
   char *icon = NULL;
   char buf[PATH_MAX];

   if (!e || !e->edje || !key)
      return (NULL);

   if (e->config->theme && e->config->theme[0] == '/')
      snprintf(buf, PATH_MAX, "%s", e->config->theme);
   else
      snprintf(buf, PATH_MAX, "%s/themes/%s", PACKAGE_DATA_DIR,
               e->config->theme);

   icon = (char *) evas_hash_find(e->config->sessions.icons, key);
   return (entrance_x_session_xsession_load(e->edje, buf, icon, key));
}
#endif

/**
 * _entrance_session_user_list_fix : update the user's list with the current
 * user as the new head of the list.  If it's the first time the user has
 * logged in, create a new user element and prepend it to the list.
 * @param e - the entrance session you're working with
 */
static void
_entrance_session_user_list_fix(Entrance_Session * e)
{
   char buf[PATH_MAX];
   Eina_List *l = NULL;
   Entrance_User *eu = NULL;

   if (!e)
      return;

   if (e->auth && e->auth->user && e->config)
   {
      for (l = e->config->users.keys; l; l = l->next)
      {
         if (!strcmp(e->auth->user, (char *) l->data))
         {
            if ((eu = evas_hash_find(e->config->users.hash, e->auth->user)))
            {
               e->config->users.keys =
                  eina_list_prepend(eina_list_remove
                                    (e->config->users.keys, eu->name),
                                    eu->name);
               entrance_config_user_list_save(e->config, e->db);
               return;
            }
         }
      }
      snprintf(buf, PATH_MAX, "default.edj");
      if ((eu = entrance_user_new(e->auth->user, buf, e->session)))
      {
         e->config->users.hash =
            evas_hash_add(e->config->users.hash, eu->name, eu);
         e->config->users.keys =
            eina_list_prepend(e->config->users.keys, eu->name);
         entrance_config_user_list_save(e->config, e->db);
      }
   }
}

static void
_entrance_session_execute_in_shell(char *user, char *shell, char *session_cmd,
                                   char *session_name)
{
   int res = 0;
   char *shell_cmd;
   char buf[PATH_MAX];

   /* If the user's passwd entry has a shell try to run it in login mode */
   if (shell && (strlen(shell) > 0))
      shell_cmd = shell;
   else
      shell_cmd = strdup("/bin/sh");

   if (session_name)
      snprintf(buf, sizeof(buf), "%s %s", session_cmd, session_name);
   else
      snprintf(buf, sizeof(buf), "%s", session_cmd);

   res = execlp(shell_cmd, shell_cmd, "-l", "-c", "--", buf, NULL);

   /* Getting here means the previous didn't work * If /bin/sh isn't a login 
      shell run /bin/sh without loading the profile * Also log a warning
      because this will probably not behave correctly */
   if (res == -1)
      /* TODO: should actually hit the user in the face with this message */
      syslog(LOG_NOTICE,
             "Neither '%s' or '/bin/sh' are working login shells for user '%s'. Your session may not function properly. ",
             shell, user);
      shell_cmd = strdup("/bin/sh");
   
   res = execlp(shell_cmd, shell_cmd, "-c", buf, NULL);

   /* Damn, that didn't work either. * Bye! We call it quits and log an error 
    * TODO: Also hit the user in the face with this! (ouch!)*/
   syslog(LOG_CRIT,
          "Entrance could not find a working shell to start the session for user: \"%s\".",
          user);
}
