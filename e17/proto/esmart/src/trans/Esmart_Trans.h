#ifndef ESMART_OBJECT_TRANS
#define ESMART_OBJECT_TRANS
#include<Ecore.h>
#include<Evas.h>
#include<Ecore_Evas.h>

typedef struct _Esmart_Trans_X11 Esmart_Trans_X11;

struct _Esmart_Trans_X11
{
    Evas_Object *obj, *clip;
    int x, y, w, h;
};

Evas_Object * esmart_trans_x11_new(Evas *e);
void esmart_trans_x11_freshen(Evas_Object *o, int x, int y, int w,
int h);
#endif

