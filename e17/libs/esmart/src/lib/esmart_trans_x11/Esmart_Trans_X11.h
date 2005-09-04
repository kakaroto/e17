#ifndef _ESMART_TRANS_X11_H
#define _ESMART_TRANS_X11_H

#include <Evas.h>
#include <Ecore_X.h>

#ifdef __cplusplus 
extern "C" {
#endif

typedef enum _Esmart_Trans_X11_Type {
   Esmart_Trans_X11_Type_Background,
   Esmart_Trans_X11_Type_Screengrab
} Esmart_Trans_X11_Type;

Evas_Object * esmart_trans_x11_new(Evas *e);
void esmart_trans_x11_type_set(Evas_Object *o, Esmart_Trans_X11_Type type);
Esmart_Trans_X11_Type esmart_trans_x11_type_get(Evas_Object *o);
void esmart_trans_x11_window_set(Evas_Object *o, Ecore_X_Window win);
void esmart_trans_x11_freshen(Evas_Object *o, int x, int y, int w,
int h);

#ifdef __cplusplus
}
#endif
#endif

