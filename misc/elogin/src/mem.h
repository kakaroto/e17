/***************************************************************************
                          mem.h  -  description
                             -------------------
    begin                : Sat Apr 8 2000
    copyright            : (C) 2000 by Chris Thomas
    email                : x5rings@fsck.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef E_MEM_H
#define E_MEM_H 1

#include <stdio.h>
#include <stdlib.h>

#define MEMCPY(src, dst, type, num) memcpy(dst, src, sizeof(type) * (num))

#if 1
#define NEW(dat, num) malloc(sizeof(dat) * (num))
#define NEW_PTR(num) malloc(sizeof(void *) * (num))
#define IF_FREE(dat) {if (dat) free(dat);}
#define FREE(dat) {free(dat);}
#define REALLOC(dat, type, num) {dat = realloc(dat, sizeof(type) * (num));}
#define REALLOC_PTR(dat, num) {dat = realloc(dat, sizeof(void *) * (num));}

#else
static void        *
_NEW(int size, int num)
{
   void               *dat;

   dat = malloc(size * num);
   printf("NEW(%i) = %p\n", size * num, dat);
   return dat;
}
static void        *
_NEW_PTR(int num)
{
   void               *dat;

   dat = malloc(sizeof(void *) * num);
   printf("NEW(%i) = %p\n", sizeof(void *) * num, dat);

   return dat;
}
static void
_IF_FREE(void *dat)
{
   printf("IF_FREE(%p)\n", dat);
   if (dat)
      free(dat);
}
static void
_FREE(void *dat)
{
   printf("FREE(%p)\n", dat);
   free(dat);
}
#define NEW(dat, num) _NEW(sizeof(dat), (num))
#define NEW_PTR(num) _NEW(sizeof(void *), (num))
#define IF_FREE(dat) {_IF_FREE(dat);}
#define FREE(dat) {_FREE(dat);}

#define REALLOC(dat, type, num) {dat = realloc(dat, sizeof(type) * (num)); printf("REALLOC(%i) = %p\n", sizeof(type) * (num), dat);}
#define REALLOC_PTR(dat, num) {dat = realloc(dat, sizeof(void *) * (num)); printf("REALLOC_PTR(%i) = %p\n", sizeof(void *) * (num), dat);}

#endif
#endif
