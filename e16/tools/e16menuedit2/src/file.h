#ifndef _FILE_H
#define _FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/types.h>
#include <glib.h>
#include <sys/stat.h>
#include <sys/types.h>

char *field (char *s, int field);
void fword (char *s, int num, char *wd);
char *homedir (int uid);
int mkdir_with_parent (const char *pathname, mode_t mode);
char *strtok_left (char *s, const char *delim, unsigned int number);
char *strsplit (char *s, char **right, int count);
int version_cmp (char *ver1, char *ver2);
char *pkg_config_version (char *package);

#endif /* _FILE_H */
