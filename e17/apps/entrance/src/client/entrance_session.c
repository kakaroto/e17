#include "entrance.h"
#include "entrance_session.h"
#include <X11/Xlib.h>

/**
 * entrance_session_new: allocate a new  Entrance_Session
 * Returns a valid Entrance_Session
 * Also Allocates the auth, and parse the config struct 
 */
Entrance_Session
entrance_session_new(void)
{
   Entrance_Session e;
   char theme_path[PATH_MAX];

   e = (Entrance_Session) malloc(sizeof(struct _Entrance_Session));

   e->auth = entrance_auth_new();
   e->config = entrance_config_parse(PACKAGE_CFG_DIR "/entrance_config.db");
   if (!e->config)
   {
       fprintf(stderr, "%s\n", PACKAGE_CFG_DIR "/entrance_config.db");
      syslog(LOG_CRIT, "Fatal Error: Unable to read configuration.");
      exit(1);
   }
   snprintf(theme_path, PATH_MAX, PACKAGE_DATA_DIR "/themes/%s",
            e->config->theme);
   e->theme = strdup(e->config->theme);
/* ?
   e->theme->path = strdup(theme_path);
*/
   return(e);
}

void
entrance_session_ecore_evas_set(Entrance_Session e, Ecore_Evas *ee)
{
   Evas *evas  = NULL;
   if(!e || !ee) return;
   
    e->ee = ee;
    if((evas = ecore_evas_get(ee)))
    {
	Evas_List *l;
	/* free the font list when we're done */
	for(l = e->config->fonts; l; l = l->next)
	{
	    evas_font_path_append(evas, (char*)l->data);
	    free(l->data);
	}
	e->config->fonts = evas_list_free(e->config->fonts);
    }
}

/**
 * entrance_session_free: free the entrance session
 */
void
entrance_session_free(Entrance_Session e)
{
   if (e)
   {
      entrance_auth_free(e->auth);
      entrance_config_free(e->config);
      ecore_evas_free(e->ee);
      free(e->theme);

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
entrance_session_run(Entrance_Session e)
{
    ecore_evas_show(e->ee);
    ecore_main_loop_begin();
}

/**
 * entrance_session_auth_user: attempt to authenticate the user
 * Returns 0 on success errors otherwise
 */
int
entrance_session_auth_user(Entrance_Session e)
{
#ifdef HAVE_PAM
   if (e->config->auth == ENTRANCE_USE_PAM)
      return (entrance_auth_cmp_pam(e->auth));
   else
#endif
      return (entrance_auth_cmp_crypt(e->auth, e->config));
}

/**
 * entrance_session_reset_user: forget what we know about the current user
 */
void
entrance_session_reset_user(Entrance_Session e)
{
   entrance_auth_free(e->auth);
   e->auth = entrance_auth_new();
}
void
entrance_session_start_user_session(Entrance_Session e)
{
   char buf[PATH_MAX];
   char *session_key = NULL;
   
   entrance_auth_setup_environment(e->auth);

   /* Assumption is that most common distributions have Xsession in
	* /etc/X11. A notable exception is Gentoo, but there is a customized
	* ebuild for this distribution. Please comment. */
   if((session_key = (char*)evas_hash_find(e->config->sessions, e->session)))
       snprintf(buf, PATH_MAX, "/etc/X11/Xsession %s", session_key);
   else
       snprintf(buf, PATH_MAX, "/etc/X11/Xsession");	/* Default session */
   /* If a path was specified for the session, use that path instead of
      passing the session name to Xsession */

#if (X_TESTING == 1)
   snprintf(buf, PATH_MAX, "/usr/X11R6/bin/xterm");
#endif

   syslog(LOG_CRIT, "Executing %s", buf);

   ecore_evas_shutdown();

   syslog(LOG_NOTICE, "Starting session for user \"%s\".", e->auth->user);

#ifdef HAVE_PAM
   if (e->config->auth == ENTRANCE_USE_PAM)
   {
      /* Tell PAM that session has begun */
      if (pam_open_session(e->auth->pam.handle, 0) != PAM_SUCCESS)
      {
	 syslog(LOG_CRIT, "Unable to open PAM session. Aborting.");
	 exit(1);
      }
   }
#endif
   
   if (initgroups(e->auth->pw->pw_name, e->auth->pw->pw_gid))
      syslog(LOG_CRIT, "Unable to initialize group (is entrance running as root?).");

   if (setgid(e->auth->pw->pw_gid))
      syslog(LOG_CRIT, "Unable to set group id.");
	  
   if (setuid(e->auth->pw->pw_uid))
      syslog(LOG_CRIT, "Unable to set user id.");

   entrance_auth_free(e->auth);	/* clear users's password out of memory */
   execl("/bin/sh", "/bin/sh", "-c", buf, NULL);
}
