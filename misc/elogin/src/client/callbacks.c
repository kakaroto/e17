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
elogin_select_session(E_Login_Session e, int index)
{
   int ix, iy;

   /* Force within list bounds/wraparound */
   if (index >= evas_list_count(e->listitems))
      index = 0;
   else if (index < 0)
      index = evas_list_count(e->listitems) - 1;

   /* Update bullet position */
   ix = 300;
   iy = 120 + (index * 30);
   evas_object_move(e->bullet, ix, iy);

   /* Update current session */
   e->session_index = index;
   e->session = evas_list_nth(e->config->sessions, index);
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

#if X_TESTING
   snprintf(buf, PATH_MAX, "/usr/X11R6/bin/xterm");
#else
/*   snprintf(buf, PATH_MAX, "%s/.xinitrc", e->auth->pam.pw->pw_dir); */
   if (e->session)
      snprintf(buf, PATH_MAX, "/etc/X11/Xsession %s", e->session);
   else
      snprintf(buf, PATH_MAX, "/etc/X11/Xsession");
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
