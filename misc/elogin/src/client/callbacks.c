#include "callbacks.h"
#include "e_login_session.h"

extern void hide_text_description(void);
extern void show_text_description(void);
extern void show_password_description(void);
extern void show_error_description(char *);

static void elogin_start_x(E_Login_Session e);

void
elogin_select_next_session(E_Login_Session e)
{
   elogin_select_session(e, e->session_index + 1);
}

void
elogin_select_prev_session(E_Login_Session e)
{
   elogin_select_session(e, e->session_index - 1);
}

void
elogin_session_list_clicked(void *session, Evas * evas, Evas_Object * li,
                            void *event_info)
{
   E_Login_Session e = (E_Login_Session) session;
   int i = 0;
   Evas_List *t;
   Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up *) event_info;

   if (ev->button != 1)
      return;

   /* Find the index of the object that was clicked */
   for (t = e->listitems; t && evas_list_data(t) != li; t = t->next)
   {
      ++i;
   }

   if (t)
      elogin_select_session(e, i);
}


void
elogin_select_session(E_Login_Session e, int s_index)
{
   int ix, iy;

   /* Force within list bounds/wraparound */
   if (s_index >= evas_list_count(e->listitems))
      s_index = 0;
   else if (s_index < 0)
      s_index = evas_list_count(e->listitems) - 1;

   /* Update bullet position */
   ix = (e->geom.w - 250) / 2 + 20;
   iy = 120 + (s_index * 30);
   evas_object_move(e->bullet, ix, iy);

   /* Update current session */
   e->session_index = s_index;
   e->session = evas_list_nth(e->config->sessions, s_index);

#if ELOGIN_DEBUG
   printf("Session Selected: Name = %s, Path= %s\n", e->session->name,
          e->session->path);
#endif
}

int
elogin_return_key_cb(E_Login_Session e, char *buffer)
{
   static int return_request = 0;

   if (!e)
      return (return_request);
   if (!e->auth)
      e->auth = e_login_auth_new();

   switch (return_request++)
   {
     case 0:
        if (e_login_auth_set_user(e->auth, buffer))
        {
           show_error_description("Unknown User");
           e_login_session_reset_user(e);
           return_request = 0;
        }
        else
        {
           hide_text_description();
           show_password_description();
        }
        break;
     case 1:
        e_login_auth_set_pass(e->auth, buffer);
        if (!e_login_auth_cmp(e->auth))
        {
           elogin_start_x(e);
        }
        else
        {
           show_error_description("Login failed");
           e_login_session_reset_user(e);
        }
        return_request = 0;
        break;
     default:
        fprintf(stderr, "Elogin return request too high");
        break;
   }
   return (return_request);
}

static void
elogin_start_x(E_Login_Session e)
{
   char buf[PATH_MAX];
   int tmp;

   e_login_auth_setup_environment(e->auth);

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
              "Unable to initialize group (is elogin running as root?)\n");

   if (setgid(e->auth->pam.pw->pw_gid))
      fprintf(stderr, "Unable to set group id\n");
   if (setuid(e->auth->pam.pw->pw_uid))
      fprintf(stderr, "Unable to set user id\n");

   e_login_auth_free(e->auth);
   execl("/bin/sh", "/bin/sh", "-c", buf, NULL);
}
