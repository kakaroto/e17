#include "e_login_auth.h"
#include "util.h"

static char *
_e_login_auth_get_running_username(void)
{
   char *result;
   struct passwd *pwent = NULL;

   pwent = getpwuid(getuid());
   result = strdup(pwent->pw_name);
   return (result);
}

/* PAM Conversation function */
static int
_e_login_auth_pam_conv(int num_msg, const struct pam_message **msg,
                       struct pam_response **resp, void *appdata_ptr)
{
   int replies = 0;
   E_Login_Auth e = appdata_ptr;
   struct pam_response *reply = NULL;

   reply =
      (struct pam_response *) malloc(sizeof(struct pam_response) * num_msg);

   if (!reply)
      return PAM_CONV_ERR;

   for (replies = 0; replies < num_msg; replies++)
   {
      switch (msg[replies]->msg_style)
      {
        case PAM_PROMPT_ECHO_ON:
           reply[replies].resp_retcode = PAM_SUCCESS;
           reply[replies].resp = (char *) strdup(e->user);
           break;
        case PAM_PROMPT_ECHO_OFF:
           reply[replies].resp_retcode = PAM_SUCCESS;
           reply[replies].resp = (char *) strdup(e->pass);
           break;
        case PAM_ERROR_MSG:
        case PAM_TEXT_INFO:
           reply[replies].resp_retcode = PAM_SUCCESS;
           reply[replies].resp = NULL;
           printf("PAM: %s\n", msg[replies]->msg);
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
   e->pam.pw = struct_passwd_free(e->pam.pw);
   memset(e->pass, 0, sizeof(e->pass));
   free(e);
}

/*
 * e_login_auth_initialize - initialize PAM session, structures etc.
 * This function will call pam_start() and set the conversation
 * function and others.
 */
int
e_login_auth_initialize(E_Login_Auth e)
{
   int result = 0;
   int pamerr;

   /* Initialize pam_conv */
   e->pam.conv.conv = _e_login_auth_pam_conv;
   e->pam.conv.appdata_ptr = e;
   e->pam.handle = NULL;

   if ((pamerr =
        pam_start("elogin", e->user, &(e->pam.conv),
                  &(e->pam.handle))) != PAM_SUCCESS)
   {
      fprintf(stderr, "PAM: %s\n", pam_strerror(e->pam.handle, pamerr));
      result = ERROR_NO_PAM_INIT;
      return result;
   }

   /* Set TTY to current DISPLAY variable */
   if ((pamerr =
        pam_set_item(e->pam.handle, PAM_TTY,
                     getenv("DISPLAY"))) != PAM_SUCCESS)
   {
      fprintf(stderr, "Error: Unable to configure PAM_TTY.\n");
      result = ERROR_PAM_SET;
      return result;
   }

   /* Set requesting user */
   if ((pamerr =
        pam_set_item(e->pam.handle, PAM_RUSER,
                     _e_login_auth_get_running_username())) != PAM_SUCCESS)
   {
      fprintf(stderr, "Error: Unable to configure PAM_RUSER.\n");
      result = ERROR_PAM_SET;
      return result;
   }

   /* Set hostname of requesting user */
   /* This is set to localhost statically for now. Should be changed in the
      future if we include XDMCP support. */
   if ((pamerr =
        pam_set_item(e->pam.handle, PAM_RHOST, "localhost")) != PAM_SUCCESS)
   {
      fprintf(stderr, "Error: Unable to configure PAM_RHOST.\n");
      result = ERROR_PAM_SET;
      return result;
   }

   return E_SUCCESS;
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

   if (e_login_auth_initialize(e) != E_SUCCESS)
   {
      return ERROR_NO_PAM_INIT;
   }

   if ((pamerr = pam_authenticate(e->pam.handle, 0)) == PAM_SUCCESS)
   {
      pamerr = pam_acct_mgmt(e->pam.handle, 0);
      if (pamerr != PAM_SUCCESS)
         result = ERROR_NO_PERMS;
      else
      {
         pamerr = pam_setcred(e->pam.handle, 0);
         if (pamerr == PAM_SUCCESS)
         {
            result = AUTH_SUCCESS;
         }
         else
         {
            fprintf(stderr, "PAM: %s\n", pam_strerror(e->pam.handle, pamerr));
            switch (pamerr)
            {
              case PAM_CRED_UNAVAIL:
                 result = ERROR_BAD_CRED;
                 break;
              case PAM_CRED_EXPIRED:
                 result = ERROR_CRED_EXPIRED;
                 break;
              default:
                 result = ERROR_BAD_CRED;
                 break;
            }
         }
      }
   }
   else
   {
      result = ERROR_BAD_PASS;
      return result;
   }
   printf("PAM: %s\n", pam_strerror(e->pam.handle, pamerr));	// debug
   return (result);
}

/**
 * e_login_auth_set_pass: keep the error checking here
 * Pass it a char* and it'll set it if it should
 */
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
      struct passwd *pwent;

      memset(e->pass, 0, sizeof(e->pass));
      snprintf(e->user, PATH_MAX, "%s", str);
      e->pam.pw = struct_passwd_free(e->pam.pw);
      if ((pwent = getpwnam(e->user)))
      {
         e->pam.pw = struct_passwd_dup(pwent);
         result = 0;
      }
      else
         result = 1;
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

   e->env = environ;
   setenv("TERM", "vt100", 0);  // TERM=linux?
   setenv("HOME", e->pam.pw->pw_dir, 1);
   setenv("SHELL", e->pam.pw->pw_shell, 1);
   setenv("USER", e->pam.pw->pw_name, 1);
   setenv("LOGNAME", e->pam.pw->pw_name, 1);

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
      exit(1);

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
