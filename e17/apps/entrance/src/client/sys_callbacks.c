#include "sys_callbacks.h"
#include "entrance_session.h"
#include "entrance_fx.h"

extern void hide_text_description(void);
extern void show_text_description(void);
extern void show_password_description(void);
extern void show_error_description(char *);

static void entrance_start_x(Entrance_Session e);

void
entrance_select_next_session(Entrance_Session e)
{
   entrance_select_session(e, e->session_index + 1);
}

void
entrance_select_prev_session(Entrance_Session e)
{
   entrance_select_session(e, e->session_index - 1);
}

void
entrance_select_session(Entrance_Session e, int sindex)
{

   /* Force within list bounds/wraparound */
   if (sindex >= evas_list_count(e->listitems))
      sindex = 0;
   else if (sindex < 0)
      sindex = evas_list_count(e->listitems) - 1;

   /* Update current session */
   e->session_index = sindex;
   e->session = evas_list_nth(e->config->sessions, sindex);
}

void
entrance_select_named_session(Entrance_Session e, char *name)
{
   Evas_List *l;
   int i = 0;
   Entrance_Session_Type *st;

   for (l = e->config->sessions; l; l = l->next)
   {
      st = evas_list_data(l);
      if (!strcmp(st->name, name))
      {
         e->session = st;
         e->session_index = i;
         return;
      }
      ++i;
   }
   fprintf(stderr,
           "WARNING: Could not find a matching session after processing %d objects.\n",
           i);
}

int
entrance_return_key_cb(Entrance_Session e, char *buffer)
{
   static int return_request = 0;

   if (!e)
      return (return_request);
   if (!e->auth)
      e->auth = entrance_auth_new();

   switch (return_request++)
   {
     case 0:
        if (entrance_auth_set_user(e->auth, buffer))
        {
           show_error_description("Unknown User");
           entrance_session_reset_user(e);
           return_request = 0;
        }
        else
        {
           hide_text_description();
           update_login_face(e, buffer);
           show_password_description();
        }
        break;
     case 1:
        entrance_auth_set_pass(e->auth, buffer);
        if (!entrance_auth_cmp(e->auth))
        {
           entrance_start_x(e);
        }
        else
        {
           show_error_description("Login failed");
           fx_fade_out(e->face, 0.2);
           fx_fade_out(e->face_shadow, 0.2);
           entrance_session_reset_user(e);
        }
        return_request = 0;
        break;
     default:
        fprintf(stderr, "Entrance return request too high");
        evas_object_hide(e->face);
        evas_object_hide(e->face_shadow);
        break;
   }
   return (return_request);
}

static void
entrance_start_x(Entrance_Session e)
{
   char buf[PATH_MAX];
   int tmp;

   entrance_auth_setup_environment(e->auth);

/*   snprintf(buf, PATH_MAX, "%s/.xinitrc", e->auth->pam.pw->pw_dir); */
   if (e->session)
   {
      /* If a path was specified for the session, use that path instead of
         passing the session name to Xsession */
      if (e->session->path && strlen(e->session->path))
      {
         /* Handle the failsafe session */
         if (!strcmp(e->session->path, "failsafe"))
            snprintf(buf, PATH_MAX, "/etc/X11/Xsession failsafe");
         else
            snprintf(buf, PATH_MAX, "%s", e->session->path);
      }

      else
         snprintf(buf, PATH_MAX, "/etc/X11/Xsession %s", e->session->name);
   }
   else
      snprintf(buf, PATH_MAX, "/etc/X11/Xsession");	/* Default 
	   Session 
	 */

#if X_TESTING
   printf("Would have executed: %s\n", buf);
   snprintf(buf, PATH_MAX, "/usr/X11R6/bin/xterm");
#endif

   ecore_sync();
   XCloseDisplay(ecore_display_get());
   /* Tell PAM that session has begun */
   if ((tmp = pam_open_session(e->auth->pam.handle, 0)) != PAM_SUCCESS)
   {
      fprintf(stderr, "Unable to open PAM session. Aborting.\n");
      exit(1);
   }
   if (initgroups(e->auth->pam.pw->pw_name, e->auth->pam.pw->pw_gid))
      fprintf(stderr,
              "Unable to initialize group (is entrance running as root?)\n");

   if (setgid(e->auth->pam.pw->pw_gid))
      fprintf(stderr, "Unable to set group id\n");
   if (setuid(e->auth->pam.pw->pw_uid))
      fprintf(stderr, "Unable to set user id\n");

   entrance_auth_free(e->auth);
   execl("/bin/sh", "/bin/sh", "-c", buf, NULL);
}
