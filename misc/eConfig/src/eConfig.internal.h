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

#define FILEPATH_LEN_MAX 4096

typedef struct _pathstruct {

    char *path;
    struct _pathstruct *next;

} PathStruct;

typedef struct _econfdata {

    char              *loc;
    void              *data;
    unsigned long      length;
	int                refcount;
    struct _econfdata *next;

} eConfigData;

typedef struct _econffat {

	char            loc[FILEPATH_LEN_MAX];
	unsigned long   position;
	unsigned long   length;

} eConfigFAT;

typedef struct {

    PathStruct   *paths;
    eConfigData  *data;
	time_t        updated_on;
	unsigned long cachesize;

} eConfigType;

extern eConfigType eConfig; 

unsigned long _econf_finddatapointerinpath(char *path,char *loc,
                                           unsigned long *position);
void * _econf_get_data_from_disk(char *loc,unsigned long *length);
int _econf_save_data_to_disk(void *data, char *loc,unsigned long length);
int _econf_purge_data_from_disk(char *loc);
int _econf_purge_data_from_disk_at_path(char *loc, char *path);
int _econf_save_data_to_disk_at_position(unsigned long position,char *path,
		        unsigned long length, void *data);
