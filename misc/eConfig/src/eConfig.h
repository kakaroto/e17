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
 * this is the .h file that is exported by eConfig, to be included by your
 * application
 */

int eConfigAddPath(char *path);
int eConfigRemovePath(char *path);
char **eConfigPaths(void);


void eConfigInit(void);

void *eConfigReturnDataByLoc(char *loc);
void *eConfigReturnDataByID(unsigned long id);
