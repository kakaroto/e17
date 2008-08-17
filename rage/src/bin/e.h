/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef E_H
#define E_H

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/param.h>
#include <dlfcn.h>
#include <math.h>
#include <fnmatch.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <glob.h>
#include <locale.h>
#include <libintl.h>
#include <errno.h>
#include <signal.h>

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>
#include <Ecore_Con.h>
#include <Ecore_Ipc.h>
#include <Ecore_Job.h>
#include <Ecore_Txt.h>
#include <Ecore_File.h>
#include <Eet.h>
#include <Edje.h>
#include <Emotion.h>

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

#ifdef EAPI
#undef EAPI
#endif
#ifdef WIN32
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
// BROKEN in gcc 4 on amd64
//#   pragma GCC visibility push(hidden)
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

/* convenience macro to compress code and avoid typos */
#define E_FN_DEL(_fn, _h) if (_h) { _fn(_h); _h = NULL; }
#define E_INTERSECTS(x, y, w, h, xx, yy, ww, hh) (((x) < ((xx) + (ww))) && ((y) < ((yy) + (hh))) && (((x) + (w)) > (xx)) && (((y) + (h)) > (yy)))
#define E_INSIDE(x, y, xx, yy, ww, hh) (((x) < ((xx) + (ww))) && ((y) < ((yy) + (hh))) && ((x) >= (xx)) && ((y) >= (yy)))
#define E_CONTAINS(x, y, w, h, xx, yy, ww, hh) (((xx) >= (x)) && (((x) + (w)) >= ((xx) + (ww))) && ((yy) >= (y)) && (((y) + (h)) >= ((yy) + (hh))))
#define E_SPANS_COMMON(x1, w1, x2, w2) (!((((x2) + (w2)) <= (x1)) || ((x2) >= ((x1) + (w1)))))
#define E_REALLOC(p, s, n) p = (s *)realloc(p, sizeof(s) * n)
#define E_NEW(s, n) (s *)calloc(n, sizeof(s))
#define E_NEW_BIG(s, n) (s *)malloc(n * sizeof(s))
#define E_FREE(p) { if (p) {free(p); p = NULL;} }

#define E_CLAMP(x, min, max) (x < min ? min : (x > max ? max : x))

#define E_TYPEDEFS 1
#include "e_box.h"
#include "e_table.h"
#include "e_layout.h"
#include "e_flowlayout.h"
#undef E_TYPEDEFS
#include "e_box.h"
#include "e_table.h"
#include "e_layout.h"
#include "e_flowlayout.h"

#endif
