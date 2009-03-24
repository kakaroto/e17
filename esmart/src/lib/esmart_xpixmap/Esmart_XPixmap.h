#ifndef _ESMART_XPIXMAP_H
#define _ESMART_XPIXMAP_H

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
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#include <Evas.h>
#include <Ecore_X.h>

#ifdef __cplusplus 
extern "C" {
#endif

EAPI Evas_Object * esmart_xpixmap_new(Evas *e, Ecore_X_Pixmap pixmap, Ecore_X_Window win);

#ifdef __cplusplus
}
#endif
#endif

