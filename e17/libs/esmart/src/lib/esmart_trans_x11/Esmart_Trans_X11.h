#ifndef _ESMART_TRANS_X11_H
#define _ESMART_TRANS_X11_H

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

typedef enum _Esmart_Trans_X11_Type {
   Esmart_Trans_X11_Type_Background,
   Esmart_Trans_X11_Type_Screengrab
} Esmart_Trans_X11_Type;

EAPI Evas_Object * esmart_trans_x11_new(Evas *e);
EAPI void esmart_trans_x11_type_set(Evas_Object *o, Esmart_Trans_X11_Type type);
EAPI Esmart_Trans_X11_Type esmart_trans_x11_type_get(Evas_Object *o);
EAPI void esmart_trans_x11_window_set(Evas_Object *o, Ecore_X_Window win);
EAPI void esmart_trans_x11_freshen(Evas_Object *o, int x, int y, int w, int h);

#ifdef __cplusplus
}
#endif
#endif

