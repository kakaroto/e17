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

#include "eConfig.h"
#include "eConfig.internal.h"

int
_econf_isafter(int p, char *s1, char *s2)
{
   int                 i, j;
   int                 len, len2;
   int                 match;

   len = strlen(s1);
   len2 = strlen(s2);

   match = 0;
   for (i = p; i < len; i++)
     {
	if (s1[i] == s2[0])
	  {
	     match = 1;
	     for (j = 0; j < len2; j++)
	       {
		  if ((i + j) >= len)
		     return (-1);
		  if (s1[i + j] != s2[j])
		     match = 0;
	       }
	  }
	if (match)
	   return (i + len2);
     }
   return (-1);
}

int
_econf_matchregexp(char *rx, char *s)
{
   int                 i, l, m;
   int                 len, lenr;
   int                 match;
   char                rx2[1024];

   if (!s)
      return (0);
   if (!rx)
      return (0);

   len = strlen(s);
   l = 0;
   lenr = 0;
   match = 1;
   if ((strcmp(rx, "*") || rx[0] == 0) && s[0] == 0)
      return (0);

   if (rx[0] != '*')
     {
	m = 0;
	while ((rx[l] != '*') && (rx[l]) && (m < 1023))
	   rx2[m++] = rx[l++];
	rx2[m] = 0;
	lenr = strlen(rx2);
	if (lenr > len)
	   return (0);
	for (i = 0; i < lenr; i++)
	  {
	     if (s[i] != rx[i])
		return (0);
	  }
     }
   if ((!rx[l]) && (s[lenr]))
      return (0);
   for (i = lenr; i < len; i++)
     {
	if (rx[l])
	   l++;
	if (rx[l])
	  {
	     m = 0;
	     while ((rx[l] != '*') && (rx[l]) && (m < 1023))
		rx2[m++] = rx[l++];
	     rx2[m] = 0;
	     i = _econf_isafter(i, s, rx2);
	     if (i < 0)
		return (0);
	  }
	else
	   return (match);
     }
   return (match);
}
