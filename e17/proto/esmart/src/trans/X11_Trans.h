#ifndef ESMART_OBJECT_TRANS
#define ESMART_OBJECT_TRANS
#include<Ecore.h>
#include<Evas.h>
#include<Ecore_Evas.h>

typedef struct _Evas_Object_Trans Evas_Object_Trans;

struct _Evas_Object_Trans
{
    Evas_Object *obj, *clip;
    int x, y, w, h;
};

Evas_Object * evas_object_x11_trans_new(Evas *e);
void evas_object_x11_trans_freshen(Evas_Object *o, int x, int y, int w,
int h);
#endif

