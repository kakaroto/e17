/* from old elogin */

#include "auth.h"

pam_handle_t *pamh;
extern char **environ;

static int
elogin_pam_conv (int num_msg, const struct pam_message **msg,
		 struct pam_response **resp, void *appdata_ptr)
{
  int replies = 0;
  struct pam_response *reply = NULL;

  reply = (struct pam_response *) malloc (sizeof (struct pam_response));

  if (!reply)
    return PAM_CONV_ERR;

  for (replies = 0; replies < num_msg; replies++)
    {
      switch (msg[replies]->msg_style)
	{
	case PAM_PROMPT_ECHO_ON:
	  reply[replies].resp_retcode = PAM_SUCCESS;
	  reply[replies].resp = (char *) strdup (appdata_ptr);
	  break;
	case PAM_PROMPT_ECHO_OFF:
	  reply[replies].resp_retcode = PAM_SUCCESS;
	  reply[replies].resp = (char *) strdup (appdata_ptr);
	  break;
	case PAM_ERROR_MSG:
	case PAM_TEXT_INFO:
//          elogin_greeter_msg((char *)msg[replies]->msg);
	  reply[replies].resp_retcode = PAM_SUCCESS;
	  reply[replies].resp = NULL;
	  break;
	default:
	  free (reply);
	  return (PAM_CONV_ERR);
	}
    }

  *resp = reply;
  return (PAM_SUCCESS);
}

static struct pam_conv pamc = { &elogin_pam_conv, NULL };

int
elogin_auth_user (char *user, char *passwd)
{
  int pamerr;

  pamc.appdata_ptr = (char *) strdup (passwd);

  if ((pamerr = pam_start ("elogin", user, &pamc, &pamh)) != PAM_SUCCESS)
    {
      printf
	("Cannot find /etc/pam.d/elogin !! Did you do a 'make install' ??\n");
      goto pamerr;
    }

  if ((pamerr =
       pam_set_item (pamh, PAM_TTY,
		     (char *) ecore_display_get ())) != PAM_SUCCESS)
    {
      printf ("Couldn't set PAM_TTY=%s\n", ecore_display_get ());
      goto pamerr;
    }
  if ((pamerr = pam_authenticate (pamh, 0)) != PAM_SUCCESS)
    {
      printf ("Couldn't authenticate %s\n", user);
/*	elogin_label_set_text(err,"Couldn't authenticate");
	elogin_label_show(err);*/
      goto pamerr;
    }

  pamerr = pam_acct_mgmt (pamh, 0);
  if (pamerr == PAM_NEW_AUTHTOK_REQD)
    {
      pamerr = pam_chauthtok (pamh, PAM_CHANGE_EXPIRED_AUTHTOK);
    }

  if (pamerr != PAM_SUCCESS)
    {
      printf ("Couldn't set acct. mgmt for %s\n", user);
      goto pamerr;
    }

  if (pamerr == PAM_SUCCESS)
    elogin_label_hide(err);
    printf ("WHOO! it worked!!!\n");

  return SUCCESS;

pamerr:
  elogin_label_set_text(err,"Authentication failed");
evas_set_color(evas,err->t,255,0,0,255);
  elogin_label_show(err);
  printf ("Authentication failed.\n");
  pam_end (pamh, pamerr);
  pamh = NULL;

  return FAIL;
}

void
elogin_auth_cleanup (void)
{
  if (pamh)
    {
      pam_close_session (pamh, 0);
      pam_end (pamh, PAM_SUCCESS);
      pamh = NULL;
    }
}

void
elogin_set_environment (Userinfo * uinfo)
{
  struct passwd *pw = uinfo->pw;
  char *mail;


  environ = uinfo->env;
  setenv ("TERM", "vt100", 0);	// TERM=linux?
  setenv ("HOME", pw->pw_dir, 1);
  setenv ("SHELL", pw->pw_shell, 1);
  setenv ("USER", pw->pw_name, 1);
  setenv ("LOGNAME", pw->pw_name, 1);
  setenv ("DISPLAY", ":0.0", 1);
  mail = (char *) malloc (strlen (_PATH_MAILDIR) + strlen (pw->pw_name) + 2);
  strcpy (mail, _PATH_MAILDIR);
  strcat (mail, "/");
  strcat (mail, pw->pw_name);
  setenv ("MAIL", mail, 1);
  chdir (pw->pw_dir);
}


int
elogin_start_client (Userinfo * uinfo)
{
  struct passwd *pw = uinfo->pw;
  char *app = (char *) strcat (pw->pw_dir, "/.xinitrc");

  ecore_sync ();
  XCloseDisplay (ecore_display_get ());

  if ((initgroups (pw->pw_name, pw->pw_gid) != 0) ||
      (setgid (pw->pw_gid) != 0) || (setuid (pw->pw_uid) != 0))
    {
      printf ("could not switch user id\n");
    }
  execl ("/bin/sh", "/bin/sh", "-c", app, NULL);

  return SUCCESS;
}
