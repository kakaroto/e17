#ifndef ICONBAR_UTIL_H
#define ICONBAR_UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <Ecore.h>
#include <sys/stat.h>

Ecore_Exe *exec_run_in_dir(char *exe, char *dir);
char * get_user_home();
int e_file_exists(char *file);
int e_file_mkdir(char *dir);
int e_file_cp(char *src, char *dest);

#endif
