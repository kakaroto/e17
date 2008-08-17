/**
@file entrance_auth.c
@brief Variables and data relating to system authentication
*/
#include "entrance_auth.h"
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

#if HAVE_PAM
/* PAM Conversation function */
static int
_entrance_auth_pam_conv(int num_msg, const struct pam_message **msg,
                        struct pam_response **resp, void *appdata_ptr)
{
   int replies = 0;
   Entrance_Auth *e = appdata_ptr;
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

/**
 * entrance_auth_new
 * @return a 0'd out Entrance_Auth Struct
 */
Entrance_Auth *
entrance_auth_new(void)
{
   Entrance_Auth *e;

   e = (Entrance_Auth *) calloc(1, sizeof(struct _Entrance_Auth));
   return (e);
}

/**
 * entrance_auth_session_end
 * @e the Entrance_Auth handle for this session
 */
void
entrance_auth_session_end(Entrance_Auth * e)
{
#if HAVE_PAM
   if (e->pam.handle)
   {
      pam_end(e->pam.handle, PAM_SUCCESS);
      e->pam.handle = NULL;
   }
#endif
   /* 
      syslog(LOG_INFO, "Auth: Session Closed Successfully."); */
}

/**
 * entrance_auth_clear_pass - Clear password from memory
 * @e the Entrance_Auth handle for this session
 */
void
entrance_auth_clear_pass(Entrance_Auth * e)
{
   if (e->pw)
   {
      if (e->pw->pw_passwd)
      {
         free(e->pw->pw_passwd);
         e->pw->pw_passwd = NULL;
      }
   }
   memset(e->pass, 0, sizeof(e->pass));
}

/**
 * entrance_auth_free
 * @e the Entrance_Auth struct to be freed
 */
void
entrance_auth_free(Entrance_Auth * e)
{
   if (e->pw)
      e->pw = struct_passwd_free(e->pw);

   memset(e->user, 0, sizeof(e->user));
   memset(e->pass, 0, sizeof(e->pass));
   e->env = NULL;
   entrance_auth_session_end(e);
   free(e);
}

/**
 * entrance_auth_reset
 * @e the Entrance_Auth struct to be reset
 */
void
entrance_auth_reset(Entrance_Auth * e)
{
   if (e->pw)
      e->pw = struct_passwd_free(e->pw);

   memset(e->user, 0, sizeof(e->user));
   memset(e->pass, 0, sizeof(e->pass));
}

#if HAVE_PAM
/**
 * entrance_auth_pam_initialize - initialize PAM session, structures etc.
 * This function will call pam_start() and set the conversation
 * function and others.
 */
int
entrance_auth_pam_initialize(Entrance_Auth * e, const char *display)
{
   int pamerr;

   /* Initialize pam_conv */
   e->pam.conv.conv = _entrance_auth_pam_conv;
   e->pam.conv.appdata_ptr = e;
   e->pam.handle = NULL;

   /* We'll use our own pam file */
   if ((pamerr =
        pam_start("entrance", e->user, &(e->pam.conv),
                  &(e->pam.handle))) != PAM_SUCCESS)
   {
      syslog(LOG_CRIT, "PAM: %s.", pam_strerror(e->pam.handle, pamerr));
      return ERROR_NO_PAM_INIT;
   }

   /* Set TTY to DISPLAY */
   if ((pamerr =
        pam_set_item(e->pam.handle, PAM_TTY, display)) != PAM_SUCCESS)
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

#if HAVE_PAM
/**
 * entrance_auth_cmp_pam - attempt to auth the user
 * @e The Entrance_Auth struct to attempt to validate on the system
 * @return - 0 on success, 1 on error
 */
static int
entrance_auth_cmp_pam(Entrance_Auth * e, const char *display)
{
   int result = AUTH_FAIL;
   int pamerr;

   if (entrance_auth_pam_initialize(e, display) != E_SUCCESS)
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

static int
entrance_auth_cmp_crypt(Entrance_Auth * e)
{
   char *encrypted;
   char *correct;

   correct = e->pw->pw_passwd;

   /* Only successfully auth blank password *if* a blank password is given */
   if ((!correct || !correct[0]) && !strcmp(e->pass, ""))
      return AUTH_SUCCESS;

   encrypted = crypt(e->pass, correct);

   return (strcmp(encrypted, correct)) ? ERROR_BAD_PASS : AUTH_SUCCESS;
}

#if HAVE_SHADOW
static int
entrance_auth_cmp_shadow(Entrance_Auth * e)
{
   char *encrypted;
   char *correct;
   struct spwd *sp;

   sp = getspnam(e->pw->pw_name);
   endspent();

   if (sp)
      correct = sp->sp_pwdp;
   else
   {
      syslog(LOG_CRIT, "FATAL: Unable to fetch shadow password.");
      return AUTH_FAIL;
   }

   /* Don't authenticate blank password unless blank password is given */
   if ((!correct || !correct[0]) && !strcmp(e->pass, ""))
      return AUTH_SUCCESS;

   encrypted = crypt(e->pass, correct);

   return (strcmp(encrypted, correct)) ? ERROR_BAD_PASS : AUTH_SUCCESS;
}
#endif

int
entrance_auth_cmp(Entrance_Auth * e, const char *display, int mode)
{
   switch (mode)
   {
     case ENTRANCE_USE_PAM:
#if HAVE_PAM
        return (entrance_auth_cmp_pam(e, display));
#else
        syslog(LOG_CRIT, "FATAL: PAM authentication support unavailable.");
        return (1);
#endif
        break;
     case ENTRANCE_USE_SHADOW:
#if HAVE_SHADOW
        return (entrance_auth_cmp_shadow(e));
#else
        syslog(LOG_CRIT, "FATAL: Shadow authentication support unavailable.");
        return (1);
#endif
        break;
     case ENTRANCE_USE_CRYPT:
        return (entrance_auth_cmp_crypt(e));
        break;
     default:
        syslog(LOG_CRIT, "FATAL: Invalid authentication mode %d requested",
               mode);
        break;
   }
   return AUTH_FAIL;

}

/**
 * entrance_auth_set_pass: keep the error checking here
 * Pass it a char* and it'll set it if it should
 */
void
entrance_auth_pass_set(Entrance_Auth * e, const char *str)
{
   if (str)
      snprintf(e->pass, PATH_MAX, "%s", str);
}

/**
 * set the username in the struct
 * @param e - the Entrance_Auth to set the user of
 * @param str - a string to set the user to, NULL is fine
 * @return 0 on success, 1 on failure(User not in system)
 * Check to see if the user exists on the system, if they do, e->user is set
 * to the passed in string, if they don't, e->user is unmodified.
 */
int
entrance_auth_user_set(Entrance_Auth * e, const char *str)
{
   int result = 1;

   if (e && str)
   {
      struct passwd *pwent;

      memset(e->pass, 0, sizeof(e->pass));
      memset(e->user, 0, sizeof(e->user));
      snprintf(e->user, PATH_MAX, "%s", str);

      e->pw = struct_passwd_free(e->pw);
      if ((pwent = getpwnam(e->user)))
      {
         e->pw = struct_passwd_dup(pwent);
         result = 0;
      }
      endpwent();
   }
   return (result);
}

/**
 * entrance_auth_setup_environment - setup the users environment
 * @e the Entrance_Auth to setup
 * I'm not sure if this is correct, but for now it works.
 */
void
entrance_auth_setup_environment(Entrance_Auth * e, const char *display)
{
   extern char **environ;
   int size;
   char *mail;
   char buf[PATH_MAX];

   if (!e || !e->pw)
      return;

/* clearenv() is only availble in glibc */
   /* FIXME: this will nuke any PATH from the system or anything else we try
    * and inherit - disable for now. we basically end up with things like an
    * emprt $PATH for logins for people
#if HAVE_CLEARENV
   clearenv();
#else
   environ = NULL;
#endif
    */
   e->env = environ;
   snprintf(buf, PATH_MAX, "%s/.Xauthority", e->pw->pw_dir);
   setenv("XAUTHORITY", buf, 1);
   setenv("TERM", "vt100", 0);  // TERM=linux?
   setenv("HOME", e->pw->pw_dir, 1);
   setenv("SHELL", e->pw->pw_shell, 1);
   setenv("USER", e->pw->pw_name, 1);
   setenv("LOGNAME", e->pw->pw_name, 1);

   setenv("DISPLAY", display, 1);

   size = (strlen(_PATH_MAILDIR) + strlen(e->pw->pw_name) + 2);
   mail = (char *) malloc(sizeof(char) * size);
   snprintf(mail, size, "%s/%s", _PATH_MAILDIR, e->pw->pw_name);
   setenv("MAIL", mail, 1);
   free(mail);

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
