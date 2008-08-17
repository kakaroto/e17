#ifndef ENTRANCE_UTILS
#define ENTRANCE_UTILS

#include <pwd.h>
#include <grp.h>
#include <paths.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <Evas.h>

#define ENTRANCE_DEBUG 0

struct passwd *struct_passwd_dup(struct passwd *pwent);
void *struct_passwd_free(struct passwd *pwent);
void entrance_debug(char *msg);
void entrance_edje_object_resize_intercept_cb(void *data, Evas_Object * o,
                                              Evas_Coord w, Evas_Coord h);

char* theme_normalize_path(char*, const char*);
void atog(const char*, int *, int *);

#endif
