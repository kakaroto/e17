#ifndef E_LOGIN_UTILS
#define E_LOGIN_UTILS

#include<pwd.h>
#include<grp.h>
#include<paths.h>
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<string.h>
#include<unistd.h>

struct passwd *struct_passwd_dup(struct passwd *pwent);
void *struct_passwd_free(struct passwd *pwent);

#endif
