#ifndef __COMMON
#define __COMMON 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <config.h>
#ifdef WITH_DMALLOC
# include <dmalloc.h>
#endif

#define DATABIG unsigned long long
#define DATA64  unsigned long long
#define DATA32  unsigned int
#define DATA16  unsigned short
#define DATA8   unsigned char

#endif
