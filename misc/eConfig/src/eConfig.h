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

/*
 * this is the file that is exported by eConfig, to be included by your
 * application
 * usage: #include <eConfig.h>
 */


#define econf_get_integer() \
eConfigGetData();

#define econf_get_string() \
eConfigGetData();

#define econf_save_integer() \
eConfigStoreData();

#define econf_save_string() \
eConfigStoreData();


int              eConfigAddPath(char *path);
int              eConfigRemovePath(char *path);
char           **eConfigPaths(int *num);


void             eConfigInit(void);

void            *eConfigGetData(char *loc, unsigned long *length);
void            *eConfigRefreshData(char *loc, unsigned long *length);
int              eConfigUnloadData(char *loc);
int              eConfigStoreData(char *loc, void *data, unsigned long length,
                                  char *path);
int              eConfigStoreDataToFirstAvailablePath(char *loc, void *data,
                                                      unsigned long length);


int              eConfigExportData(char *loc,char local);

void             eConfigSetCacheSize(unsigned long newsize);
unsigned long    eConfigGetCacheSize(void);
unsigned long    eConfigGetCacheFilled(void);
int              eConfigUpdateCache(void);

int              eConfigFsckPath(char *path);
