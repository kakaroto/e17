#include "entrance.h"
#include "entrance_session.h"
#include <X11/Xlib.h>
#include <Esmart/container.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

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
static Evas_Object *_entrance_session_icon_load(Evas_Object * o, char *file);
static Evas_Object *_entrance_session_load_session(Entrance_Session * e,
                                                   char *key);
static Evas_Object *_entrance_session_user_load(Entrance_Session * e,
                                                char *key);

extern int _entrance_test_en;

/**
 * entrance_session_new: allocate a new  Entrance_Session
 * Returns a valid Entrance_Session
 * Also Allocates the auth, and parse the config struct 
 */
Entrance_Session *
entrance_session_new(void)
{
   Entrance_Session *e;

   e = (Entrance_Session *) malloc(sizeof(struct _Entrance_Session));
   memset(e, 0, sizeof(struct _Entrance_Session));

   openlog("entrance", LOG_NOWAIT, LOG_DAEMON);
   e->auth = entrance_auth_new();
   e->config = entrance_config_parse(PACKAGE_CFG_DIR "/entrance_config.db");
   if (!e->config)
   {
      fprintf(stderr, "%s\n", PACKAGE_CFG_DIR "/entrance_config.db");
      syslog(LOG_CRIT, "Fatal Error: Unable to read configuration.");
      exit(1);
   }
   e->session = strdup("");
/* ?
   e->theme->path = strdup(theme_path);
*/
   return (e);
}

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

      free(e);
   }
}

#if 0
static void
init_user_edje(Entrance_Session e, char *user)
{
   fprintf(stderr, "I should load /home/%s/.entrance.eet\n", user);
}
#endif

/**
 * entrance_session_run: Be a login dm
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
 * entrance_session_user_set: forget what we know about the current user
 */
void
entrance_session_user_set(Entrance_Session * e, char *key)
{
   int result = 0;
   const char *file = NULL;
   char *pass = NULL;
   char *session = NULL;
   Evas_Object *obj = NULL;

   if (e && key)
   {
      if ((obj = edje_object_part_swallow_get(e->edje, "EntranceFace")))
      {
         edje_object_part_unswallow(e->edje, obj);
         evas_object_del(obj);
      }
      if ((obj = _entrance_session_user_load(e, key)))
      {
         if (!entrance_auth_set_user(e->auth, key))
         {
            edje_object_file_get(obj, &file, NULL);
            if ((session = edje_file_data_get(file, "session")))
            {
               if ((strlen(session) > 0) && (strlen(e->session) == 0))
               {
                  if (e->session)
                     free(e->session);
                  e->session = strdup(session);
               }
            }
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
               if (edje_object_part_exists(e->edje, "EntranceFace"))
               {
                  edje_object_part_swallow(e->edje, "EntranceFace", obj);
               }
               edje_object_signal_emit(e->edje, "In", "EntrancePassEntry");
               edje_object_signal_emit(e->edje, "EntranceUserAuth", "");
               edje_object_signal_emit(e->edje, "UserSelected", "");
            }
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

void
entrance_session_start_user_session(Entrance_Session * e)
{
   char buf[PATH_MAX];
   char *session_key = NULL;
   pid_t pid;

   entrance_auth_setup_environment(e->auth);

   /* Assumption is that most common distributions have Xsession in *
      /etc/X11. A notable exception is Gentoo, but there is a customized *
      ebuild for this distribution. Please comment. */
   if ((session_key =
        (char *) evas_hash_find(e->config->sessions.hash, e->session)))
   {
      if (!strcmp(session_key, "default"))
         snprintf(buf, PATH_MAX, "%s", ENTRANCE_XSESSION);
      else
         snprintf(buf, PATH_MAX, "%s %s", ENTRANCE_XSESSION, session_key);
   }
   else
      snprintf(buf, PATH_MAX, "%s", ENTRANCE_XSESSION);	/* Default 
	   session 
	 */
   /* If a path was specified for the session, use that path instead of
      passing the session name to Xsession */

   if (_entrance_test_en)
      snprintf(buf, PATH_MAX, "/usr/X11R6/bin/xterm");

   syslog(LOG_CRIT, "Executing %s", buf);

   ecore_evas_free(e->ee);
   e->ee = NULL;

   ecore_x_sync();

   syslog(LOG_NOTICE, "Starting session for user \"%s\".", e->auth->user);

#ifdef HAVE_PAM
   if (e->config->auth == ENTRANCE_USE_PAM)
   {
      /* Tell PAM that session has begun */
      if (pam_open_session(e->auth->pam.handle, 0) != PAM_SUCCESS)
      {
         syslog(LOG_CRIT, "Unable to open PAM session. Aborting.");
         ecore_main_loop_quit();
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
   /* clear users's password out of memory */
   entrance_auth_clear_pass(e->auth);
   if (waitpid(pid, NULL, 0) == pid)
   {
      entrance_auth_session_end(e->auth);
      syslog(LOG_CRIT, "User Xsession Ended");
   }
}

static void
entrance_session_xsession_load(Entrance_Session * e, char *key)
{
   if (e && e->edje)
   {
      Evas_Object *o = NULL, *old_o = NULL;

      if ((o = _entrance_session_load_session(e, key)))
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
      }
   }
}

void
entrance_session_xsession_set(Entrance_Session * e, char *key)
{
   char *str = NULL;
   char buf[PATH_MAX];

   if (!e || !key)
      return;

   if ((str = evas_hash_find(e->config->sessions.hash, key)))
   {
      snprintf(buf, PATH_MAX, "%s", key);
      if (strcmp(key, e->session))
      {
         entrance_session_xsession_load(e, key);
      }
      edje_object_signal_emit(e->edje, "SessionSelected", "");
   }
   else
   {
      fprintf(stderr, "Unable to find %s\n", key);
   }
}

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

void
entrance_session_list_add(Entrance_Session * e)
{
   Evas_List *l = NULL;
   char *key = NULL;
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
         key = (char *) l->data;
         if ((edje = _entrance_session_load_session(e, key)))
         {
            e_container_element_append(container, edje);
         }
      }
      edje_object_part_swallow(e->edje, "EntranceSessionList", container);
   }
}
void
entrance_session_user_list_add(Entrance_Session * e)
{
   Evas_Coord w, h;
   char *key = NULL;
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
         key = (char *) l->data;
         if ((edje = _entrance_session_user_load(e, key)))
         {
            e_container_element_append(container, edje);
         }
      }
      edje_object_part_swallow(e->edje, "EntranceUserList", container);
   }

}

static Evas_Object *
_entrance_session_user_load(Entrance_Session * e, char *key)
{
   int result = 0;
   char *icon = NULL;
   char buf[PATH_MAX];
   Evas_Object *o = NULL;
   Evas_Object *edje = NULL;

   if (!e || !e->edje || !key)
      return (NULL);

   if ((icon = evas_hash_find(e->config->users.hash, key)))
   {
      edje = edje_object_add(evas_object_evas_get(e->edje));
      if (icon[0] == '/')       /* absolute path */
         snprintf(buf, PATH_MAX, "%s", icon);
      else
         snprintf(buf, PATH_MAX, "%s/users/%s", PACKAGE_DATA_DIR, icon);
      if ((result = edje_object_file_set(edje, buf, "User")) > 0)
      {
         Evas_Coord w = 48, h = 48;

         /* edje_object_size_min_get(edje, &w, &h); */
         evas_object_move(edje, -w, -h);
         evas_object_resize(edje, w, h);
         evas_object_layer_set(edje, 0);

         if (edje_object_part_exists(edje, "EntranceUser"))
         {
            edje_object_part_text_set(edje, "EntranceUser", key);
            evas_object_show(edje);
            edje_object_signal_callback_add(edje, "UserSelected", "",
                                            user_selected_cb, key);
            edje_object_signal_callback_add(edje, "UserUnSelected", "",
                                            user_unselected_cb, key);
         }
         else
         {
            if (o)
               evas_object_del(o);
            if (edje)
               evas_object_del(edje);
            edje = NULL;
         }
      }
      else
      {
         fprintf(stderr, "Failed on: %s(%d)\n", key, result);
         evas_object_del(edje);
         evas_object_del(o);
         edje = NULL;
      }
   }
   return (edje);

}
static Evas_Object *
_entrance_session_icon_load(Evas_Object * o, char *file)
{
   Evas_Object *result = NULL;
   char buf[PATH_MAX];

   if (!o || !file)
      return (NULL);

   result = edje_object_add(evas_object_evas_get(o));
   snprintf(buf, PATH_MAX, "%s/images/sessions/%s", PACKAGE_DATA_DIR, file);
   if (!edje_object_file_set(result, buf, "Icon") > 0)
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

static Evas_Object *
_entrance_session_load_session(Entrance_Session * e, char *key)
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
   if (e->config->theme && e->config->theme[0] == '/')	/* abs 
	   path 
	 */
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
         o = _entrance_session_icon_load(e->edje, icon);
         if (!strcmp(evas_object_type_get(o), "image"))
         {
            Evas_Coord w, h;

            edje_object_part_geometry_get(edje, "EntranceSessionIcon", NULL,
                                          NULL, &w, &h);
            evas_object_image_fill_set(o, 0.0, 0.0, w, h);
         }
         edje_object_part_swallow(edje, "EntranceSessionIcon", o);
      }
      if (edje_object_part_exists(edje, "EntranceSessionTitle"))
      {
         edje_object_part_text_set(edje, "EntranceSessionTitle", key);
      }
      edje_object_signal_callback_add(edje, "SessionSelected", "",
                                      session_item_selected_cb, key);
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
