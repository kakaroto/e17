#ifndef EMOTE_MAIN_H
# define EMOTE_MAIN_H

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "Emote.h"

# include <signal.h>
# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <errno.h>
# include <sys/types.h>
# include <ctype.h>

# if TIME_WITH_SYS_TIME
#  include <sys/time.h>
#  include <time.h>
# else
#  if HAVE_SYS_TIME_H
#   include <sys/time.h>
#  else
#   include <time.h>
#  endif
# endif

# ifdef HAVE_ALLOCA_H
#  include <alloca.h>
# elif defined __GNUC__
#  define alloca __builtin_alloca
# elif defined _AIX
#  define alloca __alloca
# elif defined _MSC_VER
#  include <malloc.h>
#  define alloca _alloca
# else
#  include <stddef.h>
#  ifdef  __cplusplus
extern "C"
#  endif
void *alloca (size_t);
# endif

# include <limits.h>

# if HAVE_DLFCN_H
#  include <dlfcn.h>
# endif

struct
{
  char libdir[PATH_MAX];
  char datadir[PATH_MAX];
  char protocoldir[PATH_MAX];
} em_paths;

#endif
