#include"e_login_auth.h"

static int
_e_login_auth_pam_conv(int num_msg, const struct pam_message **msg,
                       struct pam_response **resp, void *appdata_ptr)
{
   int replies = 0;
   struct pam_response *reply = NULL;

   reply = (struct pam_response *) malloc(sizeof(struct pam_response));

   if (!reply)
      return PAM_CONV_ERR;

   for (replies = 0; replies < num_msg; replies++)
   {
      switch (msg[replies]->msg_style)
      {
        case PAM_PROMPT_ECHO_ON:
           reply[replies].resp_retcode = PAM_SUCCESS;
           reply[replies].resp = (char *) strdup(appdata_ptr);
           break;
        case PAM_PROMPT_ECHO_OFF:
           reply[replies].resp_retcode = PAM_SUCCESS;
           reply[replies].resp = (char *) strdup(appdata_ptr);
           break;
        case PAM_ERROR_MSG:
        case PAM_TEXT_INFO:
           reply[replies].resp_retcode = PAM_SUCCESS;
           reply[replies].resp = NULL;
           break;
        default:
           free(reply);
           return (PAM_CONV_ERR);
      }
   }

   *resp = reply;
   return (PAM_SUCCESS);
}

/* e_login_auth_new
 * Returns a 0'd out E_Login_Auth Struct
 */
E_Login_Auth
e_login_auth_new(void)
{
   E_Login_Auth e;

   e = (E_Login_Auth) malloc(sizeof(struct _E_Login_Auth));
   memset(e, 0, sizeof(struct _E_Login_Auth));
   e->pam.conv.conv = &_e_login_auth_pam_conv;
   e->env = (char **) malloc(sizeof(char *) * 2);
   e->env[0] = 0;
   return (e);
}

/* e_login_auth_free
 * @e the E_Login_Auth struct to be freed
 */
void
e_login_auth_free(E_Login_Auth e)
{
   if (e->pam.handle)
   {
      pam_close_session(e->pam.handle, 0);
      pam_end(e->pam.handle, PAM_SUCCESS);
      e->pam.handle = NULL;
   }
   if (e->pam.pw)
      e->pam.pw = NULL;
   free(e);
}

/*
 * e_login_auth_cmp - attempt to auth the user
 * @e The E_Login_Auth struct to attempt to validate on the system
 * Returns - 0 on success, 1 on error
 */
int
e_login_auth_cmp(E_Login_Auth e)
{
   int result = 0;
   int pamerr;

   if (e->pam.conv.appdata_ptr)
      free(e->pam.conv.appdata_ptr);
   e->pam.conv.appdata_ptr = (char *) strdup(e->pass);

   if ((pamerr =
        pam_start("elogin", e->user, &e->pam.conv,
                  &e->pam.handle)) != PAM_SUCCESS)
   {
      fprintf(stderr,
              "Can not find /etc/pam.d/elogin !! Did you do a make install ?\n");
      result = ERROR_NO_PAM_INIT;
   }
   if ((pamerr = pam_authenticate(e->pam.handle, 0)) == PAM_SUCCESS)
   {
      result = AUTH_SUCCESS;
   }
   else
   {
      result = ERROR_BAD_PASS;
   }
   return (result);
}

void
e_login_auth_set_pass(E_Login_Auth e, char *str)
{
   if (str)
      snprintf(e->pass, PATH_MAX, "%s", str);
}

/*
 * e_login_auth_set_user - set the username in the struct
 * @e - the E_Login_Auth to set the user of
 * @str - a string to set the user to, NULL is fine
 * Returns - 0 on success, 1 on failure(User not in system)
 * Check to see if the user exists on the system, if they do, e->user is set
 * to the passed in string, if they don't, e->user is unmodified.
 */
int
e_login_auth_set_user(E_Login_Auth e, char *str)
{
   int result = 0;

   if (str)
   {
      snprintf(e->user, PATH_MAX, "%s", str);
      if (e->pam.pw)
      {
         free(e->pam.pw);
         e->pam.pw = NULL;
      }
      if ((e->pam.pw = getpwnam(e->user)))
         endpwent();
      else
      {
         result = 1;
      }
   }
   return (result);
}

/*
 * e_login_auth_setup_environment - setup the users environment
 * @e the E_Login_Auth to setup
 * I'm not sure if this is correct, but for now it works.
 */
void
e_login_auth_setup_environment(E_Login_Auth e)
{
   extern char **environ;
   int size;
   char *mail;

   environ = e->env;
   setenv("TERM", "vt100", 0);  // TERM=linux?
   setenv("HOME", e->pam.pw->pw_dir, 1);
   setenv("SHELL", e->pam.pw->pw_shell, 1);
   setenv("USER", e->pam.pw->pw_name, 1);
   setenv("LOGNAME", e->pam.pw->pw_name, 1);
   setenv("DISPLAY", ":0.0", 1);

   size = (strlen(_PATH_MAILDIR) + strlen(e->pam.pw->pw_name) + 2);
   mail = (char *) malloc(sizeof(char) * size);
   snprintf(mail, size, "%s/%s", _PATH_MAILDIR, e->pam.pw->pw_name);
   setenv("MAIL", mail, 1);

   chdir(e->pam.pw->pw_dir);
}

#if 0
/* test */
#define LOOPTEST 0

int
main(int argc, char *argv[])
{
   E_Login_Auth e;
   int i;

   if (argc < 3)
      exit();

#if LOOPTEST
   for (i = 0; i < 60; i++)
   {
#endif
      e = e_login_auth_new();
      e_login_auth_set_user(e, argv[1]);
      e_login_auth_set_pass(e, argv[2]);

      if (!e_login_auth_cmp(e))
      {
         e_login_auth_setup_environment(e);
         printf("Compare was a success\n");
      }
      else
      {
         printf("Compare was not a success\n");
      }
      e_login_auth_free(e);
#if LOOPTEST
   }
#endif
   return (0);
}
#endif
