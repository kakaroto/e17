#include "callbacks.h"
#include "e_login_auth.h"

extern void show_text_description(void);
extern void show_password_description(void);

static void elogin_start_x(E_Login_Auth e);

void
elogin_bg_mouse_down(void *_data, Evas _e, Evas_Object _o, int _b, int _x,
                     int _y)
{
   exit(0);
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
        show_password_description();
        e_login_auth_set_user(e->auth, buffer);
        break;
     case 1:
        e_login_auth_set_pass(e->auth, buffer);
        if (!e_login_auth_cmp(e->auth))
        {
           elogin_start_x(e->auth);
        }
        else
        {
           show_text_description();
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
#if 1
   snprintf(buf, PATH_MAX, "%s/.xinitrc", e->pam.pw->pw_dir);
#endif
#if 0
   snprintf(buf, PATH_MAX, "/usr/local/bin/gaim");
#endif

   ecore_sync();
   XCloseDisplay(ecore_display_get());
   if(initgroups(e->pam.pw->pw_name, e->pam.pw->pw_gid))
       fprintf(stderr, "Unable to initialize group\n");
   if (setgid(e->pam.pw->pw_gid))
       fprintf(stderr, "Unable to set group id\n");
   if (setuid(e->pam.pw->pw_uid))
       fprintf(stderr, "Unable to set user id\n");
     
   execl("/bin/sh", "/bin/sh", "-c", buf, NULL);
   e_login_auth_free(e);
}
