#ifndef __COMMON
#define __COMMON 1

#include <stdio.h>
#include <stdlib.h>
#ifdef __EMX__
#include <sys/types.h>
#endif
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

#ifdef DO_MMX_ASM
int __imlib_get_cpuid(void);
#define CPUID_MMX (1 << 23)
#define CPUID_XMM (1 << 25)
#endif

#endif
