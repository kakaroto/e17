
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
#include <netinet/in.h>

#define FILEPATH_LEN_MAX 4096
#include "../config.h"

typedef struct _pathstruct
  {

     char               *path;
     struct _pathstruct *next;

  }
PathStruct;

typedef struct _econfdata
  {

     char               *loc;
     void               *data;
     unsigned long       length;
     int                 refcount;
     struct _econfdata  *next;

  }
eConfigData;

typedef struct _econffat
  {

     char                loc[FILEPATH_LEN_MAX];
     unsigned long       position;
     unsigned long       length;
     unsigned long       usage_index;
     unsigned long       updated_on;

  }
eConfigFAT;

typedef struct
  {

     PathStruct         *paths;
     eConfigData        *data;
     unsigned long       cachesize;

  }
eConfigType;

extern eConfigType  eConfig;

unsigned long       _econf_finddatapointerinpath(char *path, char *loc,
						 unsigned long *position,
						 unsigned long *timestamp);
char              **_econf_snarf_keys_from_fat_table(char *path,
					  char *regex, unsigned long *length);
void               *_econf_get_data_from_disk(char *loc, unsigned long *length);
int                 _econf_save_data_to_disk(void *data, char *loc, unsigned long length,
					     char *path);
int                 _econf_purge_data_from_disk(char *loc);
int                 _econf_purge_data_from_disk_at_path(char *loc, char *path);
int                 _econf_save_data_to_disk_at_position(unsigned long position, char *path,
					    unsigned long length, void *data);
unsigned long       _econf_append_data_to_disk_at_path(char *path,
						       unsigned long length,
						       void *data);
int                 _econf_new_fat_entry_to_disk(char *loc,
		    unsigned long position, unsigned long length, char *path);
unsigned long       _econf_timestamp(void);
int                 _econf_replace_fat_entry_to_disk(char *loc,
				 unsigned long position, unsigned long length,
						     char *path);
int                 _econf_create_new_data_repository(char *path);
int                 _econf_isafter(int p, char *s1, char *s2);
int                 _econf_matchregexp(char *rx, char *s);

int                 _econf_vsnprintf(char *str, size_t count, const char *fmt,
				     va_list args);

#ifdef HAVE_STDARGS
int                 _econf_snprintf(char *str, size_t count,
				    const char *fmt,...);

#else
int                 _econf_snprintf(va_alist);

#endif

int                 _econf_lock_path(char *path);
int                 _econf_unlock_path(char *path);
