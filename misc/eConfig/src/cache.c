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

#include "eConfig.internal.h"
#include "eConfig.h"

int
eConfigUpdateCache(void)
{

   /* this function should force an update of all the cache in memory
    * FIXME: currently it does nothing :)
    * This function is exported by eConfig.
    */

   return 0;

}

void
eConfigSetCacheSize(unsigned long newsize)
{

   /* This function allows an application to set the cache size
    * that eConfig will use.
    * This function is exported by eConfig.
    */

   eConfig.cachesize = newsize;
   eConfigUpdateCache();

   return;

}

unsigned long
eConfigGetCacheSize(void)
{

   /* This function will return the current cachesize that 
    * eConfig is using.
    * This function is exported by eConfig.
    */

   return eConfig.cachesize;

}

unsigned long
eConfigGetCacheFilled(void)
{

   /* This function will return the current amount of filled data
    * that eConfig has used in the cache size.  Useful for displaying
    * statistics, as well as trimming the size of the cache using
    * eConfigSetCacheSize().
    * FIXME: this function does nothing useful right now.
    * This function is exported by eConfig.
    */

   return eConfig.cachesize;

}
