#ifndef ENTRANCE_UTILS
#define ENTRANCE_UTILS

#include<pwd.h>
#include<grp.h>
#include<paths.h>
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<string.h>
#include<unistd.h>

#define ENTRANCE_DEBUG 0

struct passwd *struct_passwd_dup(struct passwd *pwent);
void *struct_passwd_free(struct passwd *pwent);
void entrance_debug(char *msg);

#endif
