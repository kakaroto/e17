/*****************************************************************************/
/* eConfig - the configuration library that just wouldn't die (yet)          */
/*****************************************************************************/
/* Copyright (C) 1999 - 1999 Carsten Haitzler (The Rasterman)                */
/*                       and Geoff Harrison   (Mandrake)                     */
/*                                                                           */
/* This program and utilites is free software; you can redistribute it       */
/* and/or modify it under the terms of the License shown in COPYING          */
/*                                                                           */
/* This software is distributed in the hope that it will be useful,          */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                      */
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/resource.h>

void    econf_set_cache_size(int bytes);
int     econf_get_cache_size(void);

char    econf_get_bool  (char *type, int instance, char *key, char    def);
int     econf_get_int   (char *type, int instance, char *key, int     def);
double  econf_get_float (char *type, int instance, char *key, double  def);
char   *econf_get_str   (char *type, int instance, char *key, char   *def);
void   *econf_get_data  (char *type, int instance, char *key, void   *def);

void    econf_set_bool  (char *type, int instance, char *key, char    val);
void    econf_set_int   (char *type, int instance, char *key, int     val);
void    econf_set_float (char *type, int instance, char *key, double  val);
void    econf_set_str   (char *type, int instance, char *key, char   *val);
void    econf_set_data  (char *type, int instance, char *key, void   *val);
