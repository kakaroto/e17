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

/* This macro is useful for getting a numeric value (ptr format) from disk. */

#define econf_get_integer(myint,loc) \
{\
	unsigned long *tmp,tmp2;\
	tmp = (unsigned long *) eConfigGetData(loc,&tmp2); \
	*tmp = ntohl(*tmp); \
	myint = *tmp; \
}

/* This macro is useful for getting a string (char * format) from disk. */

#define econf_get_string(save,loc) \
{ \
	{\
		unsigned long tmp; \
		save = eConfigGetData(loc,&tmp); \
	}\
}

/* This macro is useful for saving a numeric value (ptr format) to disk. */

#define econf_save_integer(code,path,loc,data) \
{\
	unsigned long tmp; \
	tmp = htonl(*data); \
	code = eConfigStoreData(loc,&tmp,sizeof(unsigned long),path); \
}

/* This macro is useful for saving a string (char * format) to disk. */

#define econf_save_string(code, path,loc,string) \
{ \
	code = eConfigStoreData(loc,string,strlen(string),path); \
}

int                 eConfigAddPath(char *path);
int                 eConfigRemovePath(char *path);
char              **eConfigPaths(int *num);

void                eConfigInit(void);

char              **eConfigGetKeys(char *stringtomatch,
				   unsigned long *numberofmatches);

void               *eConfigGetData(char *loc, unsigned long *length);
void               *eConfigRefreshData(char *loc, unsigned long *length);
int                 eConfigUnloadData(char *loc);
int                 eConfigStoreData(char *loc, void *data,
				     unsigned long length, char *path);
int                 eConfigStoreDataToFirstAvailablePath(char *loc, void *data,
							 unsigned long length);

int                 eConfigExportData(char *loc, char local);

void                eConfigSetCacheSize(unsigned long newsize);
unsigned long       eConfigGetCacheSize(void);
unsigned long       eConfigGetCacheFilled(void);
int                 eConfigUpdateCache(void);

int                 eConfigFsckPath(char *path);
