#include "entrance.h"
#include "entrance_session.h"
#include <X11/Xlib.h>
#include <Esmart/container.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "entrance_user.h"

/**
@file entrance_session.c
@brief Variables and Data relating to an instance of the interface

*/

extern void session_item_selected_cb(void *data, Evas_Object * o,
                                     const char *emission,
                                     const char *source);
extern void user_selected_cb(void *data, Evas_Object * o,
                             const char *emission, const char *source);
extern void user_unselected_cb(void *data, Evas_Object * o,
                               const char *emission, const char *source);
static Evas_Object *_entrance_session_icon_load(Evas_Object * o,
                                                const char *file);
static Evas_Object *_entrance_session_load_session(Entrance_Session * e,
                                                   const char *key);
static void _entrance_session_user_list_fix(Entrance_Session * e);
static void entrance_session_xsession_load(Entrance_Session * e,
                                           const char *key);

extern int _entrance_test_en;

/**
 * entrance_session_new: allocate a new  Entrance_Session
 * Returns a valid Entrance_Session
 * Also Allocates the auth, and parse the config struct 
 */
Entrance_Session *
entrance_session_new(const char *config)
{
   Entrance_Session *e;
   char *db;

   if (config)
      db = strdup(config);
   else
      db = strdup(PACKAGE_CFG_DIR "/entrance_config.db");

   e = (Entrance_Session *) malloc(sizeof(struct _Entrance_Session));
   memset(e, 0, sizeof(struct _Entrance_Session));

   openlog("entrance", LOG_NOWAIT, LOG_DAEMON);
   e->auth = entrance_auth_new();
   e->config = entrance_config_parse(db);
   if (!e->config)
   {
      fprintf(stderr, "Could not load %s\n", db);
      syslog(LOG_CRIT, "Fatal Error: Unable to read config file %s.", db);
      exit(1);
   }
   e->session = strdup("");

   free(db);
   return (e);
}

/**
 * entrance_session_free: free the entrance session
 * @e - the Entrance_Session to set the ecore evas for
 * @ee - the pointer to a fully setup Ecore_Evas we want to run
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
      Evas_List *l;

      /* free the font list when we're done */
      for (l = e->config->fonts; l; l = l->next)
      {
         evas_font_path_append(evas, (char *) l->data);
         free(l->data);
      }
      e->config->fonts = evas_list_free(e->config->fonts);
   }
}

/**
 * entrance_session_free: free the entrance session
 * @e - the Entrance_Session to free
 */
void
entrance_session_free(Entrance_Session * e)
{
   if (e)
   {
      if (e->auth)
         entrance_auth_free(e->auth);
      if (e->config)
         entrance_config_free(e->config);
      if (e->ee)
         ecore_evas_free(e->ee);
      if (e->session)
         free(e->session);

      closelog();

      free(e);
   }
}

/**
 * entrance_session_run: Be a login dm, start running
 * @e - the Entrance_Session to be run
 */
void
entrance_session_run(Entrance_Session * e)
{
   ecore_evas_show(e->ee);
   ecore_main_loop_begin();
}

/**
 * entrance_session_auth_user: attempt to authenticate the user
 * @e - the entrance session we're trying to auth
 * Returns 0 on success errors otherwise
 */
int
entrance_session_auth_user(Entrance_Session * e)
{
#ifdef HAVE_PAM
   if (e->config->auth == ENTRANCE_USE_PAM)
      return (entrance_auth_cmp_pam(e->auth));
   else
#endif
      return (entrance_auth_cmp_crypt(e->auth, e->config));
}

/**
 * entrance_session_user_reset: forget what we know about the current user
 * @e - the entrance session we should forget the user for 
 */
void
entrance_session_user_reset(Entrance_Session * e)
{
   if (e)
   {
      entrance_auth_free(e->auth);
      e->auth = entrance_auth_new();
      edje_object_signal_emit(e->edje, "In", "EntranceUserEntry");
      /* FIXME: we shouldn't emit UserAuthFail here, but it gets us back to
         the beginning */
      edje_object_signal_emit(e->edje, "EntranceUserAuthFail", "");
   }
}

/**
 * entrance_session_user_set: forget what we know about the current user,
 * load what info we can from the entrance user parameter, so we have a new
 * user in our "EntranceFace" edje
 * @e - the entrance sesssion currently running
 * @eu - the new entrance user we're setting as "current"
 */
void
entrance_session_user_set(Entrance_Session * e, Entrance_User * eu)
{
   Evas_Object *obj = NULL;

   if (e && eu)
   {
      if ((obj = edje_object_part_swallow_get(e->edje, "EntranceFace")))
      {
         edje_object_part_unswallow(e->edje, obj);
         evas_object_del(obj);
      }
      if ((obj = entrance_user_edje_get(eu, e->edje)))
      {
         if (!entrance_auth_set_user(e->auth, eu->name))
         {
            if ((eu->session) && (strlen(eu->session) > 0))
            {
               if (e->session)
                  free(e->session);
               e->session = strdup(eu->session);
               entrance_session_xsession_load(e, eu->session);

               /* FIXME: Should this be optional ? */

            }
#if 0
            if ((pass = edje_file_data_get(file, "password")))
            {
               entrance_auth_set_pass(e->auth, pass);
               result = entrance_session_auth_user(e);
               if (result == 0)
               {
                  if (edje_object_part_exists(e->edje, "EntranceFace"))
                  {
                     edje_object_part_swallow(e->edje, "EntranceFace", obj);
                  }
                  edje_object_signal_emit(e->edje, "EntranceUserAuthSuccess",
                                          "");
                  e->authed = 1;
               }
               else
               {
                  edje_object_signal_emit(e->edje, "EntranceUserAuthFail",
                                          "");
                  evas_object_del(obj);
               }
            }
            else
            {
#endif
               if (edje_object_part_exists(e->edje, "EntranceFace"))
               {
                  edje_object_part_swallow(e->edje, "EntranceFace", obj);
               }
               edje_object_signal_emit(e->edje, "In", "EntrancePassEntry");
               edje_object_signal_emit(e->edje, "EntranceUserAuth", "");
               edje_object_signal_emit(e->edje, "UserSelected", "");
#if 0
            }
#endif
         }
         else
         {
            evas_object_del(obj);
            edje_object_signal_emit(e->edje, "In", "EntrancePassEntry");
            edje_object_signal_emit(e->edje, "EntranceUserFail", "");
         }
      }
   }
}

/**
 * entrance_session_user_session_default_set : change the current
 * EntranceUser's default session to what Entrance itself currently has in
 * context.  This only will get written if someone successfully auths after
 * a selection like this is made, but it allows themes etc to have dialogs
 * deciding whether or not they should save this new session as default
 * @e - the currently running session
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
 * entrance_session_start_user_session : If the user is authed, try to log
 * their asses in.
 * @e - the currently running session
 */
void
entrance_session_start_user_session(Entrance_Session * e)
{
   char buf[PATH_MAX];
   char *session_key = NULL;
   pid_t pid;

   entrance_auth_setup_environment(e->auth);

   if ((session_key =
        (char *) evas_hash_find(e->config->sessions.hash, e->session)))
   {
      if (!strcmp(session_key, "default"))
         snprintf(buf, PATH_MAX, "%s", ENTRANCE_XSESSION);
      else
         snprintf(buf, PATH_MAX, "%s %s", ENTRANCE_XSESSION, session_key);
   }
   else
      snprintf(buf, PATH_MAX, "%s", ENTRANCE_XSESSION);	
    /* Default  session */
   
   /* If an absolute path was specified for the session, use that path
    * instead of passing the session name to Xsession 
    */

   if (_entrance_test_en)
      snprintf(buf, PATH_MAX, "/usr/X11R6/bin/xterm");

   syslog(LOG_CRIT, "Executing %s", buf);

   ecore_evas_free(e->ee);
   e->ee = NULL;
   ecore_evas_shutdown();
   ecore_x_sync();

   syslog(LOG_NOTICE, "Starting session for user \"%s\".", e->auth->user);

#ifdef HAVE_PAM
   if (e->config->auth == ENTRANCE_USE_PAM)
   {
      /* Tell PAM that session has begun */
      if (pam_open_session(e->auth->pam.handle, 0) != PAM_SUCCESS)
      {
         syslog(LOG_CRIT, "Unable to open PAM session. Aborting.");
         return;
      }
   }
#endif

   switch ((pid = fork()))
   {
     case 0:
        if (initgroups(e->auth->pw->pw_name, e->auth->pw->pw_gid))
           syslog(LOG_CRIT,
                  "Unable to initialize group (is entrance running as root?).");
        if (setgid(e->auth->pw->pw_gid))
           syslog(LOG_CRIT, "Unable to set group id.");
        if (setuid(e->auth->pw->pw_uid))
           syslog(LOG_CRIT, "Unable to set user id.");
        entrance_auth_free(e->auth);
        e->auth = NULL;
        execl("/bin/sh", "/bin/sh", "-c", buf, NULL);
        exit(0);
        break;
     case -1:
        syslog(LOG_INFO, "FORK FAILED, UH OH");
        exit(0);
     default:
        break;
   }
   _entrance_session_user_list_fix(e);
   /* clear users's password out of memory */
   entrance_auth_clear_pass(e->auth);
   /* this bypasses a race condition where entrance loses its x connection */
   /* before the wm gets it and x goes and resets itself */
   sleep(10);
   /* replace this rpcoess with a clean small one that just waits for its */
   /* child to exit.. passed on the cmd-line */
   /* atmos : Could we just free up all of our memory usage at this point
    * instead of exec'ing this other tiny program ?
    */
   snprintf(buf, sizeof(buf), "%s/entrance_login %i", PACKAGE_BIN_DIR,
            (int) pid);
   execl("/bin/sh", "/bin/sh", "-c", buf, NULL);
}

/**
 * entrance_session_xsession_load : The error checking part of
 * entrance_session_xsession_set.  It only frees the current EntranceSession
 * object in the main edje if loading the newly requested one was
 * successful.
 * @e - the entrance session you want to set the session for
 * @key - the key in the config hash that has this session
 */
static void
entrance_session_xsession_load(Entrance_Session * e, const char *key)
{
   if (e && e->edje)
   {
      char *str = NULL;
      Evas_Object *o = NULL, *old_o = NULL;

      if ((o = _entrance_session_load_session(e, key)))
      {
         if ((str = evas_hash_find(e->config->sessions.hash, key)))
         {
            if (e->session)
               free(e->session);
            e->session = strdup(key);

            old_o = edje_object_part_swallow_get(e->edje, "EntranceSession");
            if (old_o)
            {
               edje_object_part_unswallow(e->edje, old_o);
               evas_object_del(old_o);
            }
            edje_object_part_swallow(e->edje, "EntranceSession", o);
            edje_object_signal_emit(e->edje, "SessionDefaultChanged", "");
         }
      }
   }
}

/**
 * entrance_session_xsession_set : Set the current xsesssion to the
 * specified key, emit a signal to the main edje letting it know the main
 * session has changed
 * @e - the entrance session you want to set the session for
 * @key - the key in the config hash that has this session
 */
void
entrance_session_xsession_set(Entrance_Session * e, const char *key)
{
   char *str = NULL;
   char buf[PATH_MAX];

   if (!e || !key)
      return;

   if ((str = evas_hash_find(e->config->sessions.hash, key)))
   {
      snprintf(buf, PATH_MAX, "%s", key);
      if (strcmp(str, e->session))
      {
         entrance_session_xsession_load(e, key);
      }
      edje_object_signal_emit(e->edje, "SessionSelected", "");
   }
   else
   {
      fprintf(stderr, "Unable to find session %s\n", key);
   }
}

/**
 * entrance_session_edje_object_set : Set the main edje for the session to
 * be the parameter passed in
 * @e - the entrance session you want to modify
 * @o - the new edje you're specifying
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
 * entrance_session_list_add : fine the "EntranceSessionList" part in the
 * main edje, setup the container to hold the elements, and create session
 * edjes for the container based on our session list in the config
 */
void
entrance_session_list_add(Entrance_Session * e)
{
   Evas_List *l = NULL;
   const char *key = NULL;
   Evas_Coord w, h;
   Evas_Object *edje = NULL;
   Evas_Object *container = NULL;

   if (!e || !e->edje || !e->config)
      return;
   edje_object_part_geometry_get(e->edje, "EntranceSessionList", NULL, NULL,
                                 &w, &h);
   if ((container = e_container_new(evas_object_evas_get(e->edje))))
   {
      e_container_padding_set(container, 4, 4, 4, 4);
      e_container_spacing_set(container, 4);
      e_container_move_button_set(container, 2);
      if (w > h)
      {
         e_container_fill_policy_set(container, CONTAINER_FILL_POLICY_FILL_Y);
         e_container_direction_set(container, 0);
      }
      else
      {
         e_container_fill_policy_set(container, CONTAINER_FILL_POLICY_FILL_X);
         e_container_direction_set(container, 1);
      }

      for (l = e->config->sessions.keys; l; l = l->next)
      {
         key = (const char *) l->data;
         if ((edje = _entrance_session_load_session(e, key)))
         {
            e_container_element_append(container, edje);
         }
      }
      edje_object_part_swallow(e->edje, "EntranceSessionList", container);
   }
}

/**
 * entrance_session_user_list_add : find the "EntranceUserList" object in
 * the main edje, setup the container to hold the elements, and create user
 * edjes for the container with our user list in the config
 */
void
entrance_session_user_list_add(Entrance_Session * e)
{
   Evas_Coord w, h;
   Entrance_User *key = NULL;
   Evas_List *l = NULL;
   Evas_Object *container = NULL, *edje;

   if (!e || !e->edje)
      return;
   edje_object_part_geometry_get(e->edje, "EntranceUserList", NULL, NULL, &w,
                                 &h);
   if ((container = e_container_new(evas_object_evas_get(e->edje))))
   {
      e_container_padding_set(container, 4, 4, 4, 4);
      e_container_spacing_set(container, 4);
      e_container_move_button_set(container, 2);
      if (w > h)
      {
         e_container_fill_policy_set(container, CONTAINER_FILL_POLICY_FILL_Y);
         e_container_direction_set(container, 0);
      }
      else
      {
         e_container_fill_policy_set(container, CONTAINER_FILL_POLICY_FILL_X);
         e_container_direction_set(container, 1);
      }
      for (l = e->config->users.keys; l; l = l->next)
      {
         key = (Entrance_User *) l->data;
         if ((edje = entrance_user_edje_get(key, e->edje)))
         {
            e_container_element_append(container, edje);
         }
      }
      edje_object_part_swallow(e->edje, "EntranceUserList", container);
   }

}

/**
 * entrance_session_default_xsession_get : Return the hash key for the
 * session that's the first item in the system's session list
 */
const char *
entrance_session_default_xsession_get(Entrance_Session * e)
{
   Evas_List *l = NULL;
   const char *result = NULL;

   if (e && e->config)
   {
      if ((l = e->config->sessions.keys))
      {
         result = (const char *) l->data;
      }
   }
   return (result);
}

/**
 * _entrance_session_icon_load : given the filename, create a new evas
 * object(edje or image) with the contents of file.  file can either bea
 * valid edje eet or anything your evas has images loaders for.
 * FIXME: Should this be its own smart object, user images are done similar 
 * FIXME: Should it support a "key" paramater as well
 */
static Evas_Object *
_entrance_session_icon_load(Evas_Object * o, const char *file)
{
   Evas_Object *result = NULL;
   char buf[PATH_MAX];

   if (!o || !file)
      return (NULL);

   result = edje_object_add(evas_object_evas_get(o));
   snprintf(buf, PATH_MAX, "%s/images/sessions/%s", PACKAGE_DATA_DIR, file);
   if (!edje_object_file_set(result, buf, "Icon"))
   {
      evas_object_del(result);
      result = evas_object_image_add(evas_object_evas_get(o));
      evas_object_image_file_set(result, buf, NULL);
      if (evas_object_image_load_error_get(result))
      {
         snprintf(buf, PATH_MAX, "%s/images/sessions/default.png",
                  PACKAGE_DATA_DIR);
         result = evas_object_image_add(evas_object_evas_get(o));
         evas_object_image_file_set(result, buf, NULL);
      }
   }
   evas_object_move(result, -999, -999);
   evas_object_resize(result, 48, 48);
   evas_object_layer_set(result, 0);
   evas_object_show(result);
   return (result);
}

/**
 * _entrance_session_load_session : given the key, try loading an instance
 * of the "Session" group from the current theme.  printf on failure. :(
 * Returns the new session object, or NULL on failure.
 */
static Evas_Object *
_entrance_session_load_session(Entrance_Session * e, const char *key)
{
   int result = 0;
   char *icon = NULL;
   char buf[PATH_MAX];
   Evas_Object *o = NULL;
   Evas_Object *edje = NULL;
   Evas_Coord w, h;

   if (!e || !e->edje || !key)
      return (NULL);

   edje = edje_object_add(evas_object_evas_get(e->edje));
   if (e->config->theme && e->config->theme[0] == '/')	
      snprintf(buf, PATH_MAX, "%s", e->config->theme);
   else
      snprintf(buf, PATH_MAX, "%s/themes/%s", PACKAGE_DATA_DIR,
               e->config->theme);
   if ((result = edje_object_file_set(edje, buf, "Session")) > 0)
   {
      evas_object_layer_set(edje, 0);
      evas_object_move(edje, -9999, -9999);

      if (edje_object_part_exists(e->edje, "EntranceSession"))
      {
         edje_object_part_geometry_get(e->edje, "EntranceSession", NULL, NULL,
                                       &w, &h);
         evas_object_resize(edje, w, h);
      }
      else
         evas_object_resize(edje, 48, 48);
      if (edje_object_part_exists(edje, "EntranceSessionIcon"))
      {
         icon = (char *) evas_hash_find(e->config->sessions.icons, key);
         if ((o = _entrance_session_icon_load(e->edje, icon)))
         {
            if (!strcmp(evas_object_type_get(o), "image"))
            {
               Evas_Coord w, h;

               edje_object_part_geometry_get(edje, "EntranceSessionIcon",
                                             NULL, NULL, &w, &h);
               evas_object_image_fill_set(o, 0.0, 0.0, w, h);
            }
            edje_object_part_swallow(edje, "EntranceSessionIcon", o);
         }
      }
      if (edje_object_part_exists(edje, "EntranceSessionTitle"))
      {
         edje_object_part_text_set(edje, "EntranceSessionTitle", key);
      }
      edje_object_signal_callback_add(edje, "SessionSelected", "",
                                      session_item_selected_cb, (char *) key);
      evas_object_show(edje);
   }
   else
   {
      fprintf(stderr, "Failed on: %s(%d)\n", key, result);
      evas_object_del(edje);
      evas_object_del(o);
      edje = NULL;
   }
   return (edje);
}

/**
 * _entrance_session_user_list_fix : update the user's list with the current
 * user as the new head of the list.  If it's the first time the user has
 * logged in, create a new user element and prepend it to the list.
 */
static void
_entrance_session_user_list_fix(Entrance_Session * e)
{
   char buf[PATH_MAX];
   Evas_List *l = NULL;
   Entrance_User *eu = NULL;

   if (!e)
      return;

   if (e->auth && e->auth->user && e->config)
   {
      for (l = e->config->users.keys; l; l = l->next)
      {
         if (!strcmp(e->auth->user, ((Entrance_User *) l->data)->name))
         {
            if ((eu = evas_hash_find(e->config->users.hash, e->auth->user)))
            {
               e->config->users.keys =
                  evas_list_prepend(evas_list_remove
                                    (e->config->users.keys, eu), eu);
               entrance_config_user_list_write(e->config);
               return;
            }
         }
      }
      snprintf(buf, PATH_MAX, "default.eet");
      eu = entrance_user_new(e->auth->user, buf, e->session);
      e->config->users.keys = evas_list_prepend(e->config->users.keys, eu);
      entrance_config_user_list_write(e->config);
   }
}
