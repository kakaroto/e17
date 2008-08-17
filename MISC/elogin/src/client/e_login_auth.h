#ifndef _E_LOGIN_AUTH
#define _E_LOGIN_AUTH

#include<pwd.h>
#include<grp.h>
#include<paths.h>
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<string.h>
#include<unistd.h>
#include<security/pam_appl.h>
#include<security/pam_misc.h>

#define AUTH_SUCCESS 0
#define E_SUCCESS 0
#define ERROR_NO_PAM_INIT 1
#define ERROR_BAD_PASS 2
#define ERROR_PAM_SET 3
#define ERROR_NO_PERMS 4
#define ERROR_CRED_EXPIRED 5
#define ERROR_BAD_CRED 6

struct _E_Login_Auth
{
   struct
   {
      struct pam_conv conv;
      struct passwd *pw;
      pam_handle_t *handle;
   }
   pam;

   char user[PATH_MAX];
   char pass[PATH_MAX];
   char **env;
};
typedef struct _E_Login_Auth *E_Login_Auth;

E_Login_Auth e_login_auth_new(void);
void e_login_auth_free(E_Login_Auth e);

/* 0 on success, 1 on failure */
int e_login_auth_initialize(E_Login_Auth e);
int e_login_auth_cmp(E_Login_Auth e);
void e_login_auth_set_pass(E_Login_Auth e, char *str);

/* 0 on success, 1 on no user by that name */
int e_login_auth_set_user(E_Login_Auth e, char *str);
void e_login_auth_setup_environment(E_Login_Auth e);

#endif
