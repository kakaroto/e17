#include "callbacks.h"
#include "e_login_session.h"

extern void hide_text_description(void);
extern void show_text_description(void);
extern void show_password_description(void);
extern void show_error_description(char *);

static void elogin_start_x(E_Login_Auth e);

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
           elogin_start_x(e->auth);
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
elogin_start_x(E_Login_Auth e)
{
   char buf[PATH_MAX];

   e_login_auth_setup_environment(e);
#if X_TESTING
   snprintf(buf, PATH_MAX, "/usr/X11R6/bin/xterm");
#else
   snprintf(buf, PATH_MAX, "%s/.xinitrc", e->pam.pw->pw_dir);
#endif

   ecore_sync();
   XCloseDisplay(ecore_display_get());
   if (initgroups(e->pam.pw->pw_name, e->pam.pw->pw_gid))
      fprintf(stderr, "Unable to initialize group\n");
   if (setgid(e->pam.pw->pw_gid))
      fprintf(stderr, "Unable to set group id\n");
   if (setuid(e->pam.pw->pw_uid))
      fprintf(stderr, "Unable to set user id\n");

   e_login_auth_free(e);
   execl("/bin/sh", "/bin/sh", "-c", buf, NULL);
}
