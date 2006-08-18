#ifndef _EWL_PRIVATE_H
#define _EWL_PRIVATE_H

#include <Ecore_Config.h>
#include <Ecore_File.h>
#include <Ecore_Desktop.h>
#include <Edje.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <fnmatch.h>
#include <pwd.h>
#include <grp.h>
#include <langinfo.h>

#ifdef HAVE_CONFIG_H
#include "ewl-config.h"
#endif

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#ifdef ENABLE_EWL_FB
#include <Ecore_Fb.h>
#include <Evas_Engine_FB.h>
#endif

#ifdef ENABLE_EWL_GL_X11
#include <Ecore_X.h>
#include <Evas_Engine_GL_X11.h>
#endif

#ifdef ENABLE_EWL_SOFTWARE_X11
#include <Ecore_X.h>
#include <Evas_Engine_Software_X11.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

#endif
