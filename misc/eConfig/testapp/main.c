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

   eConfigAddPath("/usr/enlightenment");
   eConfigAddPath("/usr/local/enlightenment");
   eConfigAddPath("~/.enlightenment");

   if ((stuff = eConfigPaths(&num)))
     {
	int                 i;

	for (i = 0; i < num; i++)
	  {
	     printf("path: %s\n", stuff[i]);
	  }
	free(stuff);
     }

   printf("/usr/enlightenment,/usr/local/enlightenment,\n"
	  "and ~/.enlightenment should have shown up\n");
   eConfigRemovePath("/usr/enlightenment");
   printf("removing /usr/enlightenment");

   if ((stuff = eConfigPaths(&num)))
     {
	int                 i;

	for (i = 0; i < num; i++)
	  {
	     printf("path: %s\n", stuff[i]);
	  }
	free(stuff);
     }

   printf("now just /usr/local/enlightenment and\n"
	  "	~/.enlightenment should have shown up\n");

   eConfigAddPath(".");

   if ((stuff = eConfigPaths(&num)))
     {
	int                 i;

	for (i = 0; i < num; i++)
	  {
	     printf("path: %s\n", stuff[i]);
	  }
	free(stuff);
     }
   printf("now we have added . to the directory list\n");
   {
      char               *bg1 = NULL;
      char               *bg2 = NULL;

      econf_get_string(bg1, "bg1");
      econf_get_string(bg2, "bg2");
      printf("%s bg1\n%s bg2\n", bg1, bg2);
   }

   {
      char              **keys;
      unsigned long       i, num;

      keys = eConfigGetKeys("bg*", &num);

      if (keys)
	{
	   for (i = 0; i < num; i++)
	     {
		printf("key %lu : %s\n", i, keys[i]);
	     }
	   for (i = 0; i < num; i++)
	     {
		free(keys[i]);
	     }
	   free(keys);
	}
   }

   return 0;
}
