/* from old elogin */

#include <security/pam_appl.h>
#include <pwd.h>
#include <grp.h>
#include <paths.h>
#include "ui.h"

#define FAIL 0
#define SUCCESS 1

typedef struct _Userinfo Userinfo;

struct _Userinfo
{
  char *user;
  struct passwd *pw;
  char **env;
};

Elogin_Label *err;
  

/* funcs */
static int elogin_pam_conv (int num_msg, const struct pam_message **msg,
			    struct pam_response **resp, void *appdata_ptr);

int elogin_auth_user (char *user, char *passwd);
int elogin_start_client (Userinfo * info);
void elogin_auth_cleanup (void);
void elogin_set_environment (Userinfo * uinfo);
