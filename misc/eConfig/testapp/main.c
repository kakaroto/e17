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

#include "../src/eConfig.h"
#include "../src/eConfig.internal.h"

int
main(int argc, char **argv)
{

   char              **stuff;
   int                 num;

   argc = 0;
   argv = NULL;

   eConfigInit();

   eConfigAddPath("/home");
   eConfigAddPath("/usr");
   eConfigAddPath("/enlightenment");

   if ((stuff = eConfigPaths(&num)))
     {
	int                 i;

	for (i = 0; i < num; i++)
	  {
	     printf("path: %s\n", stuff[i]);
	  }
	free(stuff);

     }

   printf("/home,/usr,and /enlightenment should have shown up\n");
   eConfigRemovePath("/usr");

   if ((stuff = eConfigPaths(&num)))
     {
	int                 i;

	for (i = 0; i < num; i++)
	  {
	     printf("path: %s\n", stuff[i]);
	  }
	free(stuff);

     }
   printf("now just /home and /enlightenment should have shown up\n");

   return 0;
}
