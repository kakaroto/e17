#include"entrance_auth.h"
#include "util.h"

static char *
_entrance_auth_get_running_username(void)
{
   char *result;
   struct passwd *pwent = NULL;

   pwent = getpwuid(getuid());
   result = strdup(pwent->pw_name);
   return (result);
}

#ifdef HAVE_PAM
/* PAM Conversation function */
static int
_entrance_auth_pam_conv(int num_msg, const struct pam_message **msg,
                        struct pam_response **resp, void *appdata_ptr)
{
   int replies = 0;
   Entrance_Auth e = appdata_ptr;
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
           syslog(LOG_INFO, "PAM: %s.", msg[replies]->msg);
           break;
        default:
           free(reply);
           return (PAM_CONV_ERR);
      }
   }

   *resp = reply;
   return (PAM_SUCCESS);
}
#endif

/* entrance_auth_new
 * Returns a 0'd out Entrance_Auth Struct
 */
Entrance_Auth
entrance_auth_new(void)
{
   Entrance_Auth e;

   e = (Entrance_Auth) malloc(sizeof(struct _Entrance_Auth));
   memset(e, 0, sizeof(struct _Entrance_Auth));
   e->env = (char **) malloc(sizeof(char *) * 2);
   e->env[0] = 0;
   return (e);
}

/* entrance_auth_free
 * @e the Entrance_Auth struct to be freed
 */
void
entrance_auth_free(Entrance_Auth e)
{
#ifdef HAVE_PAM
   if (e->pam.handle)
   {
      pam_close_session(e->pam.handle, 0);
      pam_end(e->pam.handle, PAM_SUCCESS);
      e->pam.handle = NULL;
   }
#endif

   e->pw = struct_passwd_free(e->pw);

   memset(e->pass, 0, sizeof(e->pass));
   free(e);
}

#ifdef HAVE_PAM
/*
 * _entrance_auth_pam_initialize - initialize PAM session, structures etc.
 * This function will call pam_start() and set the conversation
 * function and others.
 */
static int
_entrance_auth_pam_initialize(Entrance_Auth e)
{
   int pamerr;

   /* Initialize pam_conv */
   e->pam.conv.conv = _entrance_auth_pam_conv;
   e->pam.conv.appdata_ptr = e;
   e->pam.handle = NULL;

   if ((pamerr =
        pam_start("entrance", e->user, &(e->pam.conv),
                  &(e->pam.handle))) != PAM_SUCCESS)
   {
      syslog(LOG_CRIT, "PAM: %s.", pam_strerror(e->pam.handle, pamerr));
      return ERROR_NO_PAM_INIT;
   }

   /* Set TTY to current DISPLAY variable */
   if ((pamerr =
        pam_set_item(e->pam.handle, PAM_TTY,
                     getenv("DISPLAY"))) != PAM_SUCCESS)
   {
      syslog(LOG_CRIT, "Error: Unable to configure PAM_TTY.");
      return ERROR_PAM_SET;
   }

   /* Set requesting user */
   if ((pamerr =
        pam_set_item(e->pam.handle, PAM_RUSER,
                     _entrance_auth_get_running_username())) != PAM_SUCCESS)
   {
      syslog(LOG_CRIT, "Error: Unable to configure PAM_RUSER.");
      return ERROR_PAM_SET;
   }

   /* Set hostname of requesting user */
   /* This is set to localhost statically for now. Should be changed in the
      future if we include XDMCP support. */
   if ((pamerr =
        pam_set_item(e->pam.handle, PAM_RHOST, "localhost")) != PAM_SUCCESS)
   {
      syslog(LOG_CRIT, "Error: Unable to configure PAM_RHOST.");
      return ERROR_PAM_SET;
   }

   return E_SUCCESS;
}
#endif

#ifdef HAVE_PAM
/*
 * entrance_auth_cmp_pam - attempt to auth the user
 * @e The Entrance_Auth struct to attempt to validate on the system
 * Returns - 0 on success, 1 on error
 */
int
entrance_auth_cmp_pam(Entrance_Auth e)
{
   int result = 0;
   int pamerr;
   
   if (_entrance_auth_pam_initialize(e) != E_SUCCESS)
      return ERROR_NO_PAM_INIT;

   if ((pamerr = pam_authenticate(e->pam.handle, 0)) == PAM_SUCCESS)
   {
      pamerr = pam_acct_mgmt(e->pam.handle, 0);

      if (pamerr != PAM_SUCCESS)
         result = ERROR_NO_PERMS;
      else
      {
         pamerr = pam_setcred(e->pam.handle, 0);
    
         if (pamerr == PAM_SUCCESS)
            result = AUTH_SUCCESS;
         else
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
   else
      result = ERROR_BAD_PASS;
   
   syslog(LOG_CRIT, "PAM: %s.", pam_strerror(e->pam.handle, pamerr));

   return result;
}
#endif

int
entrance_auth_cmp_crypt(Entrance_Auth e, Entrance_Config cfg)
{
   char *encrypted;
   char *correct = e->pw->pw_passwd;
   struct spwd *sp;
   
   if (cfg->auth == ENTRANCE_USE_SHADOW)
   {
      sp = getspnam(e->pw->pw_name);
      endspent();

	  if (sp)
         correct = sp->sp_pwdp;
   }

   if (!correct || !correct[0])
      return AUTH_SUCCESS;
    
   encrypted = crypt(e->pass, correct);
   
   return (strcmp(encrypted, correct)) ? ERROR_BAD_PASS : AUTH_SUCCESS;
}

/**
 * entrance_auth_set_pass: keep the error checking here
 * Pass it a char* and it'll set it if it should
 */
void
entrance_auth_set_pass(Entrance_Auth e, const char *str)
{
   if (str)
      snprintf(e->pass, PATH_MAX, "%s", str);
}

/*
 * entrance_auth_set_user - set the username in the struct
 * @e - the Entrance_Auth to set the user of
 * @str - a string to set the user to, NULL is fine
 * Returns - 0 on success, 1 on failure(User not in system)
 * Check to see if the user exists on the system, if they do, e->user is set
 * to the passed in string, if they don't, e->user is unmodified.
 */
int
entrance_auth_set_user(Entrance_Auth e, const char *str)
{
   int result = 0;

   if (str)
   {
      struct passwd *pwent;

      memset(e->pass, 0, sizeof(e->pass));
      snprintf(e->user, PATH_MAX, "%s", str);

      e->pw = struct_passwd_free(e->pw);
      if ((pwent = getpwnam(e->user)))
      {
         e->pw = struct_passwd_dup(pwent);
         result = 0;
      }
      else
         result = 1;
      
	  endpwent();
   }
   return (result);
}

/*
 * entrance_auth_setup_environment - setup the users environment
 * @e the Entrance_Auth to setup
 * I'm not sure if this is correct, but for now it works.
 */
void
entrance_auth_setup_environment(Entrance_Auth e)
{
   extern char **environ;
   int size;
   char *mail;

   e->env = environ;
   setenv("TERM", "vt100", 0);  // TERM=linux?
   setenv("HOME", e->pw->pw_dir, 1);
   setenv("SHELL", e->pw->pw_shell, 1);
   setenv("USER", e->pw->pw_name, 1);
   setenv("LOGNAME", e->pw->pw_name, 1);

   size = (strlen(_PATH_MAILDIR) + strlen(e->pw->pw_name) + 2);
   mail = (char *) malloc(sizeof(char) * size);
   snprintf(mail, size, "%s/%s", _PATH_MAILDIR, e->pw->pw_name);
   setenv("MAIL", mail, 1);

   chdir(e->pw->pw_dir);
}

#if 0
/* test */
#define LOOPTEST 0

int
main(int argc, char *argv[])
{
   Entrance_Auth e;
   int i;

   if (argc < 3)
      exit(1);

#if LOOPTEST
   for (i = 0; i < 60; i++)
   {
#endif
      e = entrance_auth_new();
      entrance_auth_set_user(e, argv[1]);
      entrance_auth_set_pass(e, argv[2]);

      if (!entrance_auth_cmp(e))
      {
         entrance_auth_setup_environment(e);
         printf("Compare was a success\n");
      }
      else
      {
         printf("Compare was not a success\n");
      }
      entrance_auth_free(e);
#if LOOPTEST
   }
#endif
   return (0);
}
#endif
