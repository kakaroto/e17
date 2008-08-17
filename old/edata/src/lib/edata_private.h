#ifndef _EDATA_PRIVATE_H
#define _EDATA_PRIVATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <dirent.h>

#ifndef _WIN32
# include <sys/mman.h>
#endif

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef EAPI
# undef EAPI
#endif
#ifdef _MSC_VER
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#ifdef __GNUC__
# if __GNUC__ >= 4
// BROKEN in gcc 4 on amd64
//#  pragma GCC visibility push(hidden)
# endif
#endif

#if HAVE___ATTRIBUTE__
# define __UNUSED__ __attribute__((unused))
#else
# define __UNUSED__
#endif

#ifndef PATH_MAX
# define PATH_MAX 4096
#endif

#ifndef MIN
# define MIN(x, y) (((x) > (y)) ? (y) : (x))
#endif

#ifndef MAX
# define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef ABS
# define ABS(x) ((x) < 0 ? -(x) : (x))
#endif

#ifndef CLAMP
# define CLAMP(x, min, max) (((x) > (max)) ? (max) : (((x) < (min)) ? (min) : (x)))
#endif

#define READBUFSIZ 65536

#define EDATA_MAGIC_NONE            0x1234fedc
#define EDATA_MAGIC_EXE             0xf7e812f5
#define EDATA_MAGIC_TIMER           0xf7d713f4
#define EDATA_MAGIC_IDLER           0xf7c614f3
#define EDATA_MAGIC_IDLE_ENTERER    0xf7b515f2
#define EDATA_MAGIC_IDLE_EXITER     0xf7601afd
#define EDATA_MAGIC_FD_HANDLER      0xf7a416f1
#define EDATA_MAGIC_EVENT_HANDLER   0xf79317f0
#define EDATA_MAGIC_EVENT_FILTER    0xf78218ff
#define EDATA_MAGIC_EVENT           0xf77119fe
#define EDATA_MAGIC_ANIMATOR        0xf7643ea5

#define EDATA_MAGIC                 Edata_Magic  __magic

#define EDATA_MAGIC_SET(d, m)       (d)->__magic = (m)
#define EDATA_MAGIC_CHECK(d, m)     ((d) && ((d)->__magic == (m)))
#define EDATA_MAGIC_FAIL(d, m, fn)  _edata_magic_fail((d), (d) ? (d)->__magic : 0, (m), (fn));

/* undef the following, we want out version */
#undef FREE
#define FREE(ptr) free(ptr); ptr = NULL;

#undef IF_FREE
#define IF_FREE(ptr) if (ptr) free(ptr); ptr = NULL;

#undef IF_FN_DEL
#define IF_FN_DEL(_fn, ptr) if (ptr) { _fn(ptr); ptr = NULL; }

inline void edata_print_warning(const char *function, const char *sparam);

/* convenience macros for checking pointer parameters for non-NULL */
#undef CHECK_PARAM_POINTER_RETURN
#define CHECK_PARAM_POINTER_RETURN(sparam, param, ret) \
     if (!(param)) \
	 { \
	    edata_print_warning(__FUNCTION__, sparam); \
	    return ret; \
	 }

#undef CHECK_PARAM_POINTER
#define CHECK_PARAM_POINTER(sparam, param) \
     if (!(param)) \
	 { \
	    edata_print_warning(__FUNCTION__, sparam); \
	    return; \
	 }

typedef unsigned int              Edata_Magic;

typedef struct _Edata_List2       Edata_List2;
typedef struct _Edata_List2_Data  Edata_List2_Data;

struct _Edata_List2
{
   Edata_List2  *next, *prev;
   Edata_List2  *last;
};

struct _Edata_List2_Data
{
   Edata_List2   __list_data;
   void *data;
};

#ifndef _EDATA_H

#endif

EAPI void          _edata_magic_fail(void *d, Edata_Magic m, Edata_Magic req_m, const char *fname);

EAPI void         *_edata_list2_append           (void *in_list, void *in_item);
EAPI void         *_edata_list2_prepend          (void *in_list, void *in_item);
EAPI void         *_edata_list2_append_relative  (void *in_list, void *in_item, void *in_relative);
EAPI void         *_edata_list2_prepend_relative (void *in_list, void *in_item, void *in_relative);
EAPI void         *_edata_list2_remove           (void *in_list, void *in_item);
EAPI void         *_edata_list2_find             (void *in_list, void *in_item);

void          _edata_fps_debug_init(void);
void          _edata_fps_debug_shutdown(void);
void          _edata_fps_debug_runtime_add(double t);



extern int    _edata_fps_debug;

#endif
